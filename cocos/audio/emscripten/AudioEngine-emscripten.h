
#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN

#ifndef __AUDIO_ENGINE_EMPSCRIPTEN_H_
#define __AUDIO_ENGINE_EMPSCRIPTEN_H_

#include "base/CCRef.h"
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>

NS_CC_BEGIN

namespace experimental
{
	#define MAX_AUDIOINSTANCES 32

	class CC_DLL AudioEngineImpl : public cocos2d::Ref
	{
	public:
		using preload_callback = std::function<void(bool)>;

	private:

		// Required as JS calls us back using the path (I prefered that over passing the callback ptr to JS)
		std::unordered_map<std::string, std::vector<preload_callback>>	_preloadCallbacks;

	public:
		AudioEngineImpl();
		~AudioEngineImpl();

		bool init();
		int play2d(const std::string& fileFullPath, bool loop, float volume);
		void setVolume(int audioID,float volume);
		void setLoop(int audioID, bool loop);
		bool pause(int audioID);
		bool resume(int audioID);
		bool stop(int audioID);
		void stopAll();
		float getDuration(int audioID);
		float getCurrentTime(int audioID);
		bool setCurrentTime(int audioID, float time);
		void setFinishCallback(int audioID, const std::function<void (int, const std::string &)>& callback);

		void uncache(const std::string& filePath);
		void uncacheAll();

		void preload(const std::string& filePath, const preload_callback& callback);
        void cancelPreload(const std::string& filePath);

		void update(float dt);

		// Must be public, by design...
		static void		js2cpp_preloadCallback(uintptr_t class_ptr, const std::string& filePath, bool success);

	};
}

NS_CC_END

#endif // __AUDIO_ENGINE_EMPSCRIPTEN_H_

#endif
