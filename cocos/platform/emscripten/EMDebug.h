//
// EMDebug.h
//

#include "platform/CCPlatformConfig.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN) && defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)

#include <stdio.h>
#include <stdlib.h>
#include <type_traits>
#include <string>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <cocos/2d/CCNode.h>
#pragma clang diagnostic pop


/*

Class that

	* Checks underflows/overflows at every n-th frame, in cocos2d::Node::update()
	* Provides CPP interface (assert_ptr)
	* Provides JS interface

*/

/*

class	EmscriptenMemorySanitizer : public cocos2d::Node
{
	private:

		static EmscriptenMemorySanitizer	*_instance;

		bool								_enabled;
		uint64_t							_frame_counter;
		uint32_t							_check_every;

	public:

		EmscriptenMemorySanitizer();
		EmscriptenMemorySanitizer(const EmscriptenMemorySanitizer&) = delete;
		EmscriptenMemorySanitizer& operator=(const EmscriptenMemorySanitizer&) = delete;
		EmscriptenMemorySanitizer(EmscriptenMemorySanitizer&&) noexcept = delete;
		EmscriptenMemorySanitizer& operator=(EmscriptenMemorySanitizer&&) noexcept = delete;
		~EmscriptenMemorySanitizer();

	protected:

		void			update(float);

	private:

		static void		stackTrace();
*/

class	EmscriptenMemorySanitizer
{
	public:

		EmscriptenMemorySanitizer() = delete;

		///////////////////////////////////////////////////////////////////////
		// CPP API


		static void		assert_ptr(void *, const char *file, int line);

		static void		assert_ptr(uintptr_t ptr, const char *file, int line)
		{
			assert_ptr(reinterpret_cast<void *>(ptr), file, line);
		}

		template<class T>
		static void		assert_ptr(T *ptr, const char *file, int line)
		{
			if constexpr (std::is_polymorphic<T>::value)
			{
				assert_ptr(dynamic_cast<void *>(ptr), file, line);
			}
			else
			{
				assert_ptr(reinterpret_cast<void *>(ptr), file, line);
			}
		}

		template<class T>
		static void		assert_ptr(const T *ptr, const char *file, int line)
		{
			assert_ptr(const_cast<T *>(ptr), file, line);
		}


		///////////////////////////////////////////////////////////////////////
		// JS API

		static void		js2cpp_stats();
		static void		js2cpp_size_stats();
		static void		js2cpp_ptr_info(uintptr_t ptr);
		static bool		js2cpp_check();
		static bool		js2cpp_retain(bool retain);
};


class	EmscriptenSticky
{
	public:
		EmscriptenSticky(const char *name)
		{
			printf("\n[STICKY]:%s\n", name);
		}

		~EmscriptenSticky()
		{
			printf("\n[/STICKY]\n");
		}
};

#define	EMS_ASSERT_PTR(ptr)					EmscriptenMemorySanitizer::assert_ptr(ptr, __FILE__, __LINE__)
#define	EM_STICKY(name)						EmscriptenSticky __STICKY_##name(#name)
#define	EM_STICKY_PRINT(...)				printf(__VA_ARGS__)


#else	// #if (CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN) && defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)


#define	EMS_ASSERT_PTR(ptr)					do { (void)ptr; } while(0)
#define	EM_STICKY(name)						do { } while(0)
#define	EM_STICKY_PRINT(...)				do { } while(0)


#endif	// #if (CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN) && defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)
