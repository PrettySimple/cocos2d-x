
#include "platform/CCPlatformConfig.h"

// Support for redefined glGetError() and eglGetError(), only in debug builds

#if (CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN) && defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)

#define GL_GLEXT_PROTOTYPES 1
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#ifndef GL_STACK_OVERFLOW
#define GL_STACK_OVERFLOW 0x0503
#endif

#ifndef GL_STACK_UNDERFLOW
#define GL_STACK_UNDERFLOW 0x0504
#endif


/*

Since we wrap all the GL and EGL functions in CCGL-emscripten.h, the actual errors reported by glGetError()/eglGetError()
get cleared by our wrappers.
However, Cocos uses glGetError() in a couple of places (though in a dumb way), hence we need
to keep their code behaving exactly the same way as before the wrap.

The below functions achieve this.

*/


namespace {

	static GLenum	gl_error_bits = GL_NO_ERROR;	// 0 as per spec
	static EGLint	egl_error = EGL_SUCCESS;
}

// Make the compiler happy
void	wrappedGLErrorSetBit(GLenum bit);
GLenum	wrappedGLErrorGetBit();
void	wrappedEGLErrorSet(EGLint error);
EGLint	wrappedEGLErrorGet();


void	wrappedGLErrorSetBit(GLenum bit)
{
	gl_error_bits |= bit;
}


GLenum	wrappedGLErrorGetBit()
{
	// What the spec calls bits are not actually single bits. Hence, we first need to process the known bits,
	// then we'll fallback to individual bits tests...
	// These must be sorted from highest to lowest!
	static GLenum	known_bits[] = { GL_STACK_UNDERFLOW, GL_STACK_OVERFLOW,GL_INVALID_FRAMEBUFFER_OPERATION, GL_OUT_OF_MEMORY, GL_INVALID_OPERATION, GL_INVALID_VALUE, GL_INVALID_ENUM };


	for(unsigned int i = 0; i < sizeof(known_bits) / sizeof(GLenum); ++i)
	{
		if((gl_error_bits & known_bits[i]) == known_bits[i])
		{
			gl_error_bits &= ~known_bits[i];
			return known_bits[i];
		}
	}

	for(unsigned int i = 0; i < sizeof(GLenum) * 8; ++i)
	{
		GLenum	bit = (1 << i);

		if((gl_error_bits & bit))
		{
			gl_error_bits &= ~bit;
			return bit;
		}
	}

	return 0;
}

void	wrappedEGLErrorSet(EGLint error)
{
	egl_error = error;
}

EGLint	wrappedEGLErrorGet()
{
	return egl_error;
}



#endif // (CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN) && defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)
