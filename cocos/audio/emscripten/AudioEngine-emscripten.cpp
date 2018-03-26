#include "audio/emscripten/AudioEngine-emscripten.h"
#include "audio/include/AudioEngine.h"
#include <mutex>
#include <stdlib.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <stdio.h>

using namespace cocos2d::experimental;

AudioEngineImpl::AudioEngineImpl()
:	_pendingPreloads()
{
	//printf("*** AudioEngineImpl::AudioEngineImpl()\n");

	// Initializing JS in the constructor rather than in ::init()...

	static std::once_flag	run_once;

	std::call_once(
		run_once,
		[]()
		{
			emscripten_run_script(
				#include "AudioEngine-emscripten.cpp.js"
			);
		}
	);

	EM_ASM({Module.cocos_AudioEngine._construct($0)}, this);
}

AudioEngineImpl::~AudioEngineImpl()
{
	//printf("*** AudioEngineImpl::~AudioEngineImpl()\n");

	EM_ASM({Module.cocos_AudioEngine._destruct();});
	// TODO: should we do anything else here?
}

bool	AudioEngineImpl::init()
{
	//printf("*** AudioEngineImpl::init()\n");
	// JS initialization done in constructor, not doing anything here
	// TODO: reconsider this - perhaps if the sound/mp3 are unsupported on the browser, we might return false ?
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC > PRELOADING/UNCACHING (great naming consistency BTW)


//void AudioEngineImpl::preload(const std::string& filePath, std::function<void(bool isSuccess)> callback)
void AudioEngineImpl::preload(const std::string& filePath, const preload_callback_t& callback)
{
	//printf("*** AudioEngineImpl::preload(%s)\n", filePath.c_str());

	// Finally only supporting one callback per filePath. We'll dumbly override the callback should
	// we receive a subsequent request on a same pending filePath.

	auto search = _pendingPreloads.find(filePath);

	if(search == _pendingPreloads.end())
	{
		_pendingPreloads.insert({ filePath, callback });

		EM_ASM(
			{
				Module.cocos_AudioEngine.preload($0, $1);
			},
			filePath.c_str(), filePath.size()
		);
	}
	else
		search->second = callback;
}

void	AudioEngineImpl::uncache(const std::string& filePath)
{
	//printf("*** AudioEngineImpl::uncache(%s)\n", filePath.c_str());

	// Dumb implementation (see comments in preload()).

	auto search = _pendingPreloads.find(filePath);

	if(search != _pendingPreloads.end())
		_pendingPreloads.erase(search);
}

void	AudioEngineImpl::uncacheAll()
{
	//printf("*** AudioEngineImpl::uncacheAll()\n");
	// NOT SUPPORTED!
	std::abort();
}





/////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC > PLAYING



int	AudioEngineImpl::play2d(const std::string& filePath, bool loop, float volume, bool isMusic)
{
	//printf("*** AudioEngineImpl::play2d(%s, %s, %f)\n", filePath.c_str(), loop ? "true" : "false", volume);
	(void)isMusic;	// Unused on EM platform

	return EM_ASM_INT(
		{
			return Module.cocos_AudioEngine.play($0, $1, $2, $3);
		},
		filePath.c_str(), filePath.size(),
		loop,
		volume
	);
}

void	AudioEngineImpl::setVolume(int audioID, float volume)
{
	//printf("*** AudioEngineImpl::setVolume(%d, %f)\n", audioID, volume);

	EM_ASM({Module.cocos_AudioEngine.setVolume($0, $1)}, audioID, volume);
}

void	AudioEngineImpl::setLoop(int audioID, bool loop)
{
	//printf("*** AudioEngineImpl::setLoop(%d, %s)\n", audioID, loop ? "true" : "false");

	EM_ASM({Module.cocos_AudioEngine.setLoop($0, $1)}, audioID, loop);
}

void	AudioEngineImpl::pause(int audioID)
{
	//printf("*** AudioEngineImpl::pause(%d)\n", audioID);

	EM_ASM({Module.cocos_AudioEngine.pause($0)}, audioID);
}

void AudioEngineImpl::resume(int audioID)
{
	//printf("*** AudioEngineImpl::resume(%d)\n", audioID);

	EM_ASM({Module.cocos_AudioEngine.resume($0)}, audioID);
}

void	AudioEngineImpl::stop(int audioID)
{
	//printf("*** AudioEngineImpl::stop(%d)\n", audioID);

	const auto& search = _finishCallbacks.find(audioID);

	if(search != _finishCallbacks.end())
		_finishCallbacks.erase(search);

	EM_ASM({Module.cocos_AudioEngine.stop($0)}, audioID);
}

void	AudioEngineImpl::stopAll()
{
	//printf("*** AudioEngineImpl::stopAll()\n");
	// NOT SUPPORTED!
	std::abort();
}








float	AudioEngineImpl::getDuration(int audioID)
{
	//printf("*** AudioEngineImpl::getDuration(%d)\n", audioID);

	return EM_ASM_DOUBLE({return Module.cocos_AudioEngine.getDuration($0)}, audioID);
}

float	AudioEngineImpl::getCurrentTime(int audioID)
{
	//printf("*** AudioEngineImpl::getCurrentTime(%d)\n", audioID);

	return EM_ASM_DOUBLE({return Module.cocos_AudioEngine.getCurrentTime($0)}, audioID);
}

bool	AudioEngineImpl::setCurrentTime(int audioID, float time)
{
	//printf("*** AudioEngineImpl::setCurrentTime(%d, %f)\n", audioID, time);

	return EM_ASM_INT({return Module.cocos_AudioEngine.setCurrentTime($0, $1)}, audioID, time);
}

void	AudioEngineImpl::setFinishCallback(int audioID, const finish_callback_t& callback)
{
	//printf("*** AudioEngineImpl::setFinishCallback(%d, ...)\n", audioID);

	// AudioEngine filtered the audioID for us...
	auto	search = _finishCallbacks.find(audioID);

	if(search != _finishCallbacks.end())
		search->second = callback;
	else
		_finishCallbacks.insert({ audioID, callback });
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// cocos_AudioEngineImpl_emscripten_binding class methods and EMSCRIPTEN_BINDINGS()


/*

TODO: Unlike (all?) other implementations, we're firing these callbacks synchronously, not in an ::update() method.
Need to check if that can actually be harmful? It might trigger some UI actions while not in a render frame...?

*/


void	AudioEngineImpl::js2cpp_preloadCallback(uintptr_t class_ptr, const std::string& filePath, bool success)
{
	auto	self = reinterpret_cast<AudioEngineImpl *>(class_ptr);

	//printf("*** js2cpp_preloadCallback(%s, %s)\n", filePath.c_str(), success ? "true" : "false");

	const auto& search = self->_pendingPreloads.find(filePath);

	if(search != self->_pendingPreloads.end())
	{
		const auto	callback = search->second;

		self->_pendingPreloads.erase(search);

		if(callback)
			callback(success);
	}
}

void	AudioEngineImpl::js2cpp_finishCallback(uintptr_t class_ptr, int audioID, const std::string& filePath)
{
	auto	self = reinterpret_cast<AudioEngineImpl *>(class_ptr);

	//printf("*** js2cpp_finishCallback(%d, %s)\n", audioID, filePath.c_str());

	// Awful anti-pattern
	AudioEngine::remove(audioID);

	const auto& search = self->_finishCallbacks.find(audioID);

	if(search != self->_finishCallbacks.end())
	{
		const auto	callback = search->second;

		self->_finishCallbacks.erase(search);

		if(callback)
			callback(audioID, filePath);
	}
}







// https://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/embind.html

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"

EMSCRIPTEN_BINDINGS(cocos_AudioEngineImpl_emscripten_binding)
{
	emscripten::class_<AudioEngineImpl>("AudioEngineImpl")
		.class_function("preloadCallback", &AudioEngineImpl::js2cpp_preloadCallback)
		.class_function("finishCallback",  &AudioEngineImpl::js2cpp_finishCallback)
	;
}

#pragma clang diagnostic pop

