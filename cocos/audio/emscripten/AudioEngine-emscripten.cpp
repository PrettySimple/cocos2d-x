#include "audio/emscripten/AudioEngine-emscripten.h"
#include <mutex>
#include <stdlib.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <stdio.h>

using namespace cocos2d::experimental;

AudioEngineImpl::AudioEngineImpl()
:	_preloadCallbacks()
{
	printf("*** AudioEngineImpl::AudioEngineImpl()\n");

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

	EM_ASM(
		{
			Module.cocos_AudioEngine._construct($0);
		},
		this
	);
}

AudioEngineImpl::~AudioEngineImpl()
{
	printf("*** AudioEngineImpl::~AudioEngineImpl()\n");

	EM_ASM({Module.cocos_AudioEngine._destruct();});

	// Fire remaining _preloadCallbacks
	for(const auto& it : _preloadCallbacks)
	{
		for(const auto& cb : it.second)
		{
			cb(false);
		}
	}
}

bool AudioEngineImpl::init()
{
	printf("*** AudioEngineImpl::init()\n");
	// JS initialization done in constructor, not doing anything here

	// TODO: reconsider this - perhaps if the sound/mp3 are unsupported on the browser, we might return false ?

	return true;
}

int AudioEngineImpl::play2d(const std::string& filePath ,bool loop ,float volume)
{
	printf("*** AudioEngineImpl::play2d(%s, %s, %f)\n", filePath.c_str(), loop ? "true" : "false", volume);

	//TODO EMSCRIPTEN: Implement
	return -1;
}

void AudioEngineImpl::setVolume(int audioID, float volume)
{
	printf("*** AudioEngineImpl::setVolume(%d, %f)\n", audioID, volume);

	//TODO EMSCRIPTEN: Implement
}

void AudioEngineImpl::setLoop(int audioID, bool loop)
{
	printf("*** AudioEngineImpl::setLoop(%d, %s)\n", audioID, loop ? "true" : "false");
	//TODO EMSCRIPTEN: Implement
}

bool AudioEngineImpl::pause(int audioID)
{
	printf("*** AudioEngineImpl::pause(%d)\n", audioID);

	//TODO EMSCRIPTEN: Implement
	return true;
}

bool AudioEngineImpl::resume(int audioID)
{
	printf("*** AudioEngineImpl::resume(%d)\n", audioID);

	//TODO EMSCRIPTEN: Implement
	return true;
}

bool AudioEngineImpl::stop(int audioID)
{
	printf("*** AudioEngineImpl::stop(%d)\n", audioID);

	//TODO EMSCRIPTEN: Implement
	return true;
}

void AudioEngineImpl::stopAll()
{
	printf("*** AudioEngineImpl::stopAll()\n");

	//TODO EMSCRIPTEN: Implement
}

float AudioEngineImpl::getDuration(int audioID)
{
	printf("*** AudioEngineImpl::getDuration(%d)\n", audioID);

	//TODO EMSCRIPTEN: Implement
	return -1.f;
}

float AudioEngineImpl::getCurrentTime(int audioID)
{
	printf("*** AudioEngineImpl::getCurrentTime(%d)\n", audioID);

	//TODO EMSCRIPTEN: Implement
	return -1.f;
}

bool AudioEngineImpl::setCurrentTime(int audioID, float time)
{
	printf("*** AudioEngineImpl::setCurrentTime(%d, %f)\n", audioID, time);

	//TODO EMSCRIPTEN: Implement
	return true;
}

void AudioEngineImpl::setFinishCallback(int audioID, const std::function<void (int, const std::string &)>& callback)
{
	printf("*** AudioEngineImpl::setFinishCallback(%d, ...)\n", audioID);
	(void)audioID;
	(void)callback;

	//TODO EMSCRIPTEN: Implement
}

void AudioEngineImpl::uncache(const std::string& filePath)
{
	printf("*** AudioEngineImpl::uncache(%s)\n", filePath.c_str());

	//TODO EMSCRIPTEN: Implement
}

void AudioEngineImpl::uncacheAll()
{
	printf("*** AudioEngineImpl::uncacheAll()\n");

	EM_ASM({
		Module.cocos_AudioEngine.uncacheAll();
	});
}

//void AudioEngineImpl::preload(const std::string& filePath, std::function<void(bool isSuccess)> callback)
void AudioEngineImpl::preload(const std::string& filePath, const preload_callback& callback)
{
	printf("*** AudioEngineImpl::preload(%s)\n", filePath.c_str());

	auto search = _preloadCallbacks.find(filePath);

	if(search != _preloadCallbacks.end())
		search->second.push_back(callback);
	else
		_preloadCallbacks.insert( { filePath, { callback } } );

	EM_ASM(
		{
			Module.cocos_AudioEngine.preload($0, $1);
		},
		filePath.c_str(), filePath.size()
	);
}

void AudioEngineImpl::update(float dt)
{
	printf("*** AudioEngineImpl::update(%f)\n", dt);

	//TODO EMSCRIPTEN: Implement
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// cocos_AudioEngineImpl_emscripten_binding class methods and EMSCRIPTEN_BINDINGS()



void	AudioEngineImpl::js2cpp_preloadCallback(uintptr_t class_ptr, const std::string& filePath, bool success)
{
	auto	self = reinterpret_cast<AudioEngineImpl *>(class_ptr);

	printf("*** js2cpp_preloadCallback(%s, %s)\n", filePath.c_str(), success ? "true" : "false");

	auto search = self->_preloadCallbacks.find(filePath);

	if(search != self->_preloadCallbacks.end())
	{
		for(const auto& cb : search->second)
			cb(success);

		self->_preloadCallbacks.erase(search->first);
	}
}




// https://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/embind.html

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"

EMSCRIPTEN_BINDINGS(cocos_AudioEngineImpl_emscripten_binding)
{
	emscripten::class_<AudioEngineImpl>("AudioEngineImpl")
		.class_function("preloadCallback", &AudioEngineImpl::js2cpp_preloadCallback)
	;
}

#pragma clang diagnostic pop

