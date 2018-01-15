/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __PLATFORM_EMSCRIPTEN_CCGL_H__
#define __PLATFORM_EMSCRIPTEN_CCGL_H__

#include "platform/CCPlatformConfig.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN)

#define GL_GLEXT_PROTOTYPES 1
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define glClearDepth                    glClearDepthf
#define GL_BGRA                         GL_BGRA_EXT

#define GL_DEPTH24_STENCIL8             GL_DEPTH24_STENCIL8_OES
#define GL_WRITE_ONLY                   GL_WRITE_ONLY_OES


#define glGenVertexArrays               glGenVertexArraysOES
#define glDeleteVertexArrays            glDeleteVertexArraysOES
#define glBindVertexArrays              glBindVertexArraysOES
#define glBindVertexArray               glBindVertexArrayOES

#define glMapBuffer                     glMapBufferOES
#define glUnmapBuffer                   glUnmapBufferOES

#define GL_DEPTH_STENCIL				0x84F9
#define GL_DEPTH_STENCIL_ATTACHMENT		0x821A


#if defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)

/*******************************************************************************/
/*
	Emscripten-specific for now, but may be extended for use on all platforms.

	We're wrapping all the GL functions, so that the errors are reported
	when and where they occur. This makes CHECK_GL_ERROR_DEBUG() obsolete,
	although it will continue to work as before.

	Since glGetError() and eglGetError() are being actually used in a couple of
	places, we're also redefining them so that they keep reporting the error(s).

	Note that we're using printf() instead of cocos2d::log(), as some of the
	places where the below functions are invoked are not in cocos2d namespace
	(despite being part of the original cocos2d implementation...)

*/

#include <stdio.h>


#ifndef GL_STACK_OVERFLOW
#define GL_STACK_OVERFLOW 0x0503
#endif

#ifndef GL_STACK_UNDERFLOW
#define GL_STACK_UNDERFLOW 0x0504
#endif


/***********************************************************************************************************************/
/* GL2/GL2EXT wrappers */

void	wrappedGLErrorSetBit(GLenum bit);
GLenum	wrappedGLErrorGetBit();

// This should never happen - it'd mean we missed wrapping functions
#define	GLCALL_WRAPPER_BEFORE(method)\
	for(auto __error = (glGetError)(); __error != GL_NO_ERROR; __error = (glGetError)())					\
	{																										\
		const char *__errstr = nullptr;																		\
		switch(__error)																						\
		{																									\
			case GL_INVALID_ENUM:					__errstr = "GL_INVALID_ENUM";					break;	\
			case GL_INVALID_VALUE:					__errstr = "GL_INVALID_VALUE";					break;	\
			case GL_INVALID_OPERATION:				__errstr = "GL_INVALID_OPERATION";				break;	\
			case GL_INVALID_FRAMEBUFFER_OPERATION:	__errstr = "GL_INVALID_FRAMEBUFFER_OPERATION";	break;	\
			case GL_OUT_OF_MEMORY:					__errstr = "GL_OUT_OF_MEMORY";					break;	\
			case GL_STACK_OVERFLOW:					__errstr = "GL_STACK_OVERFLOW";					break;	\
			case GL_STACK_UNDERFLOW:				__errstr = "GL_STACK_UNDERFLOW";				break;	\
			default:								__errstr = "UNKNOWN";									\
		}																									\
		wrappedGLErrorSetBit(__error);																		\
		printf("*** Previously unreported OpenGL error [%s] (0x%04X) before the call to %s() from %s in %s:%d\n", __errstr, __error, method, __PRETTY_FUNCTION__, __FILE__, __LINE__);	\
	}

#define	GLCALL_WRAPPER_AFTER(method)\
	for(auto __error = (glGetError)(); __error != GL_NO_ERROR; __error = (glGetError)())					\
	{																										\
		const char *__errstr = nullptr;																		\
		switch(__error)																						\
		{																									\
			case GL_INVALID_ENUM:					__errstr = "GL_INVALID_ENUM";					break;	\
			case GL_INVALID_VALUE:					__errstr = "GL_INVALID_VALUE";					break;	\
			case GL_INVALID_OPERATION:				__errstr = "GL_INVALID_OPERATION";				break;	\
			case GL_INVALID_FRAMEBUFFER_OPERATION:	__errstr = "GL_INVALID_FRAMEBUFFER_OPERATION";	break;	\
			case GL_OUT_OF_MEMORY:					__errstr = "GL_OUT_OF_MEMORY";					break;	\
			case GL_STACK_OVERFLOW:					__errstr = "GL_STACK_OVERFLOW";					break;	\
			case GL_STACK_UNDERFLOW:				__errstr = "GL_STACK_UNDERFLOW";				break;	\
			default:								__errstr = "UNKNOWN";									\
		}																									\
		wrappedGLErrorSetBit(__error);																		\
		printf("*** Caught OpenGL error [%s] (0x%04X) in the call to %s() from %s in %s:%d\n", __errstr, __error, method, __PRETTY_FUNCTION__, __FILE__, __LINE__);	\
	}

#define	WRAP_GLCALL_RET(method, ...) ([&]() mutable { GLCALL_WRAPPER_BEFORE(#method); auto ret = (method)(__VA_ARGS__); GLCALL_WRAPPER_AFTER(#method); return ret; })()
#define WRAP_GLCALL_NORET(method, ...) do { GLCALL_WRAPPER_BEFORE(#method); (method)(__VA_ARGS__); GLCALL_WRAPPER_AFTER(#method); } while(0)



/***********************************************************************************************************************/
/* GL2 functions */

//////////////////////////
// Non-void functions

#define glCheckFramebufferStatus(...)							WRAP_GLCALL_RET(glCheckFramebufferStatus, __VA_ARGS__)
#define glCreateProgram(...)									WRAP_GLCALL_RET(glCreateProgram, __VA_ARGS__)
#define glCreateShader(...)										WRAP_GLCALL_RET(glCreateShader, __VA_ARGS__)
#define glGetAttribLocation(...)								WRAP_GLCALL_RET(glGetAttribLocation, __VA_ARGS__)
#define glGetUniformLocation(...)								WRAP_GLCALL_RET(glGetUniformLocation, __VA_ARGS__)
#define glGetString(...)										WRAP_GLCALL_RET(glGetString, __VA_ARGS__)
#define glIsBuffer(...)											WRAP_GLCALL_RET(glIsBuffer, __VA_ARGS__)
#define glIsEnabled(...)										WRAP_GLCALL_RET(glIsEnabled, __VA_ARGS__)
#define glIsFramebuffer(...)									WRAP_GLCALL_RET(glIsFramebuffer, __VA_ARGS__)
#define glIsProgram(...)										WRAP_GLCALL_RET(glIsProgram, __VA_ARGS__)
#define glIsRenderbuffer(...)									WRAP_GLCALL_RET(glIsRenderbuffer, __VA_ARGS__)
#define glIsShader(...)											WRAP_GLCALL_RET(glIsShader, __VA_ARGS__)
#define glIsTexture(...)										WRAP_GLCALL_RET(glIsTexture, __VA_ARGS__)


//////////////////////////
// void functions

#define glActiveTexture(...)									WRAP_GLCALL_NORET(glActiveTexture, __VA_ARGS__)
#define glAttachShader(...)										WRAP_GLCALL_NORET(glAttachShader, __VA_ARGS__)
#define glBindAttribLocation(...)								WRAP_GLCALL_NORET(glBindAttribLocation, __VA_ARGS__)
#define glBindBuffer(...)										WRAP_GLCALL_NORET(glBindBuffer, __VA_ARGS__)
#define glBindFramebuffer(...)									WRAP_GLCALL_NORET(glBindFramebuffer, __VA_ARGS__)
#define glBindRenderbuffer(...)									WRAP_GLCALL_NORET(glBindRenderbuffer, __VA_ARGS__)
#define glBindTexture(...)										WRAP_GLCALL_NORET(glBindTexture, __VA_ARGS__)
#define glBlendColor(...)										WRAP_GLCALL_NORET(glBlendColor, __VA_ARGS__)
#define glBlendEquation(...)									WRAP_GLCALL_NORET(glBlendEquation, __VA_ARGS__)
#define glBlendEquationSeparate(...)							WRAP_GLCALL_NORET(glBlendEquationSeparate, __VA_ARGS__)
#define glBlendFunc(...)										WRAP_GLCALL_NORET(glBlendFunc, __VA_ARGS__)
#define glBlendFuncSeparate(...)								WRAP_GLCALL_NORET(glBlendFuncSeparate, __VA_ARGS__)
#define glBufferData(...)										WRAP_GLCALL_NORET(glBufferData, __VA_ARGS__)
#define glBufferSubData(...)									WRAP_GLCALL_NORET(glBufferSubData, __VA_ARGS__)
#define glClear(...)											WRAP_GLCALL_NORET(glClear, __VA_ARGS__)
#define glClearColor(...)										WRAP_GLCALL_NORET(glClearColor, __VA_ARGS__)
#define glClearDepthf(...)										WRAP_GLCALL_NORET(glClearDepthf, __VA_ARGS__)
#define glClearStencil(...)										WRAP_GLCALL_NORET(glClearStencil, __VA_ARGS__)
#define glColorMask(...)										WRAP_GLCALL_NORET(glColorMask, __VA_ARGS__)
#define glCompileShader(...)									WRAP_GLCALL_NORET(glCompileShader, __VA_ARGS__)
#define glCompressedTexImage2D(...)								WRAP_GLCALL_NORET(glCompressedTexImage2D, __VA_ARGS__)
#define glCompressedTexSubImage2D(...)							WRAP_GLCALL_NORET(glCompressedTexSubImage2D, __VA_ARGS__)
#define glCopyTexImage2D(...)									WRAP_GLCALL_NORET(glCopyTexImage2D, __VA_ARGS__)
#define glCopyTexSubImage2D(...)								WRAP_GLCALL_NORET(glCopyTexSubImage2D, __VA_ARGS__)
#define glCullFace(...)											WRAP_GLCALL_NORET(glCullFace, __VA_ARGS__)
#define glDeleteBuffers(...)									WRAP_GLCALL_NORET(glDeleteBuffers, __VA_ARGS__)
#define glDeleteFramebuffers(...)								WRAP_GLCALL_NORET(glDeleteFramebuffers, __VA_ARGS__)
#define glDeleteProgram(...)									WRAP_GLCALL_NORET(glDeleteProgram, __VA_ARGS__)
#define glDeleteRenderbuffers(...)								WRAP_GLCALL_NORET(glDeleteRenderbuffers, __VA_ARGS__)
#define glDeleteShader(...)										WRAP_GLCALL_NORET(glDeleteShader, __VA_ARGS__)
#define glDeleteTextures(...)									WRAP_GLCALL_NORET(glDeleteTextures, __VA_ARGS__)
#define glDepthFunc(...)										WRAP_GLCALL_NORET(glDepthFunc, __VA_ARGS__)
#define glDepthMask(...)										WRAP_GLCALL_NORET(glDepthMask, __VA_ARGS__)
#define glDepthRangef(...)										WRAP_GLCALL_NORET(glDepthRangef, __VA_ARGS__)
#define glDetachShader(...)										WRAP_GLCALL_NORET(glDetachShader, __VA_ARGS__)
#define glDisable(...)											WRAP_GLCALL_NORET(glDisable, __VA_ARGS__)
#define glDisableVertexAttribArray(...)							WRAP_GLCALL_NORET(glDisableVertexAttribArray, __VA_ARGS__)
#define glDrawArrays(...)										WRAP_GLCALL_NORET(glDrawArrays, __VA_ARGS__)
#define glDrawElements(...)										WRAP_GLCALL_NORET(glDrawElements, __VA_ARGS__)
#define glEnable(...)											WRAP_GLCALL_NORET(glEnable, __VA_ARGS__)
#define glEnableVertexAttribArray(...)							WRAP_GLCALL_NORET(glEnableVertexAttribArray, __VA_ARGS__)
#define glFinish(...)											WRAP_GLCALL_NORET(glFinish, __VA_ARGS__)
#define glFlush(...)											WRAP_GLCALL_NORET(glFlush, __VA_ARGS__)
#define glFramebufferRenderbuffer(...)							WRAP_GLCALL_NORET(glFramebufferRenderbuffer, __VA_ARGS__)
#define glFramebufferTexture2D(...)								WRAP_GLCALL_NORET(glFramebufferTexture2D, __VA_ARGS__)
#define glFrontFace(...)										WRAP_GLCALL_NORET(glFrontFace, __VA_ARGS__)
#define glGenBuffers(...)										WRAP_GLCALL_NORET(glGenBuffers, __VA_ARGS__)
#define glGenerateMipmap(...)									WRAP_GLCALL_NORET(glGenerateMipmap, __VA_ARGS__)
#define glGenFramebuffers(...)									WRAP_GLCALL_NORET(glGenFramebuffers, __VA_ARGS__)
#define glGenRenderbuffers(...)									WRAP_GLCALL_NORET(glGenRenderbuffers, __VA_ARGS__)
#define glGenTextures(...)										WRAP_GLCALL_NORET(glGenTextures, __VA_ARGS__)
#define glGetActiveAttrib(...)									WRAP_GLCALL_NORET(glGetActiveAttrib, __VA_ARGS__)
#define glGetActiveUniform(...)									WRAP_GLCALL_NORET(glGetActiveUniform, __VA_ARGS__)
#define glGetAttachedShaders(...)								WRAP_GLCALL_NORET(glGetAttachedShaders, __VA_ARGS__)
#define glGetBooleanv(...)										WRAP_GLCALL_NORET(glGetBooleanv, __VA_ARGS__)
#define glGetBufferParameteriv(...)								WRAP_GLCALL_NORET(glGetBufferParameteriv, __VA_ARGS__)
#define glGetFloatv(...)										WRAP_GLCALL_NORET(glGetFloatv, __VA_ARGS__)
#define glGetFramebufferAttachmentParameteriv(...)				WRAP_GLCALL_NORET(glGetFramebufferAttachmentParameteriv, __VA_ARGS__)
#define glGetIntegerv(...)										WRAP_GLCALL_NORET(glGetIntegerv, __VA_ARGS__)
#define glGetProgramiv(...)										WRAP_GLCALL_NORET(glGetProgramiv, __VA_ARGS__)
#define glGetProgramInfoLog(...)								WRAP_GLCALL_NORET(glGetProgramInfoLog, __VA_ARGS__)
#define glGetRenderbufferParameteriv(...)						WRAP_GLCALL_NORET(glGetRenderbufferParameteriv, __VA_ARGS__)
#define glGetShaderiv(...)										WRAP_GLCALL_NORET(glGetShaderiv, __VA_ARGS__)
#define glGetShaderInfoLog(...)									WRAP_GLCALL_NORET(glGetShaderInfoLog, __VA_ARGS__)
#define glGetShaderPrecisionFormat(...)							WRAP_GLCALL_NORET(glGetShaderPrecisionFormat, __VA_ARGS__)
#define glGetShaderSource(...)									WRAP_GLCALL_NORET(glGetShaderSource, __VA_ARGS__)
#define glGetTexParameterfv(...)								WRAP_GLCALL_NORET(glGetTexParameterfv, __VA_ARGS__)
#define glGetTexParameteriv(...)								WRAP_GLCALL_NORET(glGetTexParameteriv, __VA_ARGS__)
#define glGetUniformfv(...)										WRAP_GLCALL_NORET(glGetUniformfv, __VA_ARGS__)
#define glGetUniformiv(...)										WRAP_GLCALL_NORET(glGetUniformiv, __VA_ARGS__)
#define glGetVertexAttribfv(...)								WRAP_GLCALL_NORET(glGetVertexAttribfv, __VA_ARGS__)
#define glGetVertexAttribiv(...)								WRAP_GLCALL_NORET(glGetVertexAttribiv, __VA_ARGS__)
#define glGetVertexAttribPointerv(...)							WRAP_GLCALL_NORET(glGetVertexAttribPointerv, __VA_ARGS__)
#define glHint(...)												WRAP_GLCALL_NORET(glHint, __VA_ARGS__)
#define glLineWidth(...)										WRAP_GLCALL_NORET(glLineWidth, __VA_ARGS__)
#define glLinkProgram(...)										WRAP_GLCALL_NORET(glLinkProgram, __VA_ARGS__)
#define glPixelStorei(...)										WRAP_GLCALL_NORET(glPixelStorei, __VA_ARGS__)
#define glPolygonOffset(...)									WRAP_GLCALL_NORET(glPolygonOffset, __VA_ARGS__)
#define glReadPixels(...)										WRAP_GLCALL_NORET(glReadPixels, __VA_ARGS__)
#define glReleaseShaderCompiler(...)							WRAP_GLCALL_NORET(glReleaseShaderCompiler, __VA_ARGS__)
#define glRenderbufferStorage(...)								WRAP_GLCALL_NORET(glRenderbufferStorage, __VA_ARGS__)
#define glSampleCoverage(...)									WRAP_GLCALL_NORET(glSampleCoverage, __VA_ARGS__)
#define glScissor(...)											WRAP_GLCALL_NORET(glScissor, __VA_ARGS__)
#define glShaderBinary(...)										WRAP_GLCALL_NORET(glShaderBinary, __VA_ARGS__)
#define glShaderSource(...)										WRAP_GLCALL_NORET(glShaderSource, __VA_ARGS__)
#define glStencilFunc(...)										WRAP_GLCALL_NORET(glStencilFunc, __VA_ARGS__)
#define glStencilFuncSeparate(...)								WRAP_GLCALL_NORET(glStencilFuncSeparate, __VA_ARGS__)
#define glStencilMask(...)										WRAP_GLCALL_NORET(glStencilMask, __VA_ARGS__)
#define glStencilMaskSeparate(...)								WRAP_GLCALL_NORET(glStencilMaskSeparate, __VA_ARGS__)
#define glStencilOp(...)										WRAP_GLCALL_NORET(glStencilOp, __VA_ARGS__)
#define glStencilOpSeparate(...)								WRAP_GLCALL_NORET(glStencilOpSeparate, __VA_ARGS__)
#define glTexImage2D(...)										WRAP_GLCALL_NORET(glTexImage2D, __VA_ARGS__)
#define glTexParameterf(...)									WRAP_GLCALL_NORET(glTexParameterf, __VA_ARGS__)
#define glTexParameterfv(...)									WRAP_GLCALL_NORET(glTexParameterfv, __VA_ARGS__)
#define glTexParameteri(...)									WRAP_GLCALL_NORET(glTexParameteri, __VA_ARGS__)
#define glTexParameteriv(...)									WRAP_GLCALL_NORET(glTexParameteriv, __VA_ARGS__)
#define glTexSubImage2D(...)									WRAP_GLCALL_NORET(glTexSubImage2D, __VA_ARGS__)
#define glUniform1f(...)										WRAP_GLCALL_NORET(glUniform1f, __VA_ARGS__)
#define glUniform1fv(...)										WRAP_GLCALL_NORET(glUniform1fv, __VA_ARGS__)
#define glUniform1i(...)										WRAP_GLCALL_NORET(glUniform1i, __VA_ARGS__)
#define glUniform1iv(...)										WRAP_GLCALL_NORET(glUniform1iv, __VA_ARGS__)
#define glUniform2f(...)										WRAP_GLCALL_NORET(glUniform2f, __VA_ARGS__)
#define glUniform2fv(...)										WRAP_GLCALL_NORET(glUniform2fv, __VA_ARGS__)
#define glUniform2i(...)										WRAP_GLCALL_NORET(glUniform2i, __VA_ARGS__)
#define glUniform2iv(...)										WRAP_GLCALL_NORET(glUniform2iv, __VA_ARGS__)
#define glUniform3f(...)										WRAP_GLCALL_NORET(glUniform3f, __VA_ARGS__)
#define glUniform3fv(...)										WRAP_GLCALL_NORET(glUniform3fv, __VA_ARGS__)
#define glUniform3i(...)										WRAP_GLCALL_NORET(glUniform3i, __VA_ARGS__)
#define glUniform3iv(...)										WRAP_GLCALL_NORET(glUniform3iv, __VA_ARGS__)
#define glUniform4f(...)										WRAP_GLCALL_NORET(glUniform4f, __VA_ARGS__)
#define glUniform4fv(...)										WRAP_GLCALL_NORET(glUniform4fv, __VA_ARGS__)
#define glUniform4i(...)										WRAP_GLCALL_NORET(glUniform4i, __VA_ARGS__)
#define glUniform4iv(...)										WRAP_GLCALL_NORET(glUniform4iv, __VA_ARGS__)
#define glUniformMatrix2fv(...)									WRAP_GLCALL_NORET(glUniformMatrix2fv, __VA_ARGS__)
#define glUniformMatrix3fv(...)									WRAP_GLCALL_NORET(glUniformMatrix3fv, __VA_ARGS__)
#define glUniformMatrix4fv(...)									WRAP_GLCALL_NORET(glUniformMatrix4fv, __VA_ARGS__)
#define glUseProgram(...)										WRAP_GLCALL_NORET(glUseProgram, __VA_ARGS__)
#define glValidateProgram(...)									WRAP_GLCALL_NORET(glValidateProgram, __VA_ARGS__)
#define glVertexAttrib1f(...)									WRAP_GLCALL_NORET(glVertexAttrib1f, __VA_ARGS__)
#define glVertexAttrib1fv(...)									WRAP_GLCALL_NORET(glVertexAttrib1fv, __VA_ARGS__)
#define glVertexAttrib2f(...)									WRAP_GLCALL_NORET(glVertexAttrib2f, __VA_ARGS__)
#define glVertexAttrib2fv(...)									WRAP_GLCALL_NORET(glVertexAttrib2fv, __VA_ARGS__)
#define glVertexAttrib3f(...)									WRAP_GLCALL_NORET(glVertexAttrib3f, __VA_ARGS__)
#define glVertexAttrib3fv(...)									WRAP_GLCALL_NORET(glVertexAttrib3fv, __VA_ARGS__)
#define glVertexAttrib4f(...)									WRAP_GLCALL_NORET(glVertexAttrib4f, __VA_ARGS__)
#define glVertexAttrib4fv(...)									WRAP_GLCALL_NORET(glVertexAttrib4fv, __VA_ARGS__)
#define glVertexAttribPointer(...)								WRAP_GLCALL_NORET(glVertexAttribPointer, __VA_ARGS__)
#define glViewport(...)											WRAP_GLCALL_NORET(glViewport, __VA_ARGS__)


//////////////////////////
// The error function

#define glGetError()											wrappedGLErrorGetBit()


/***********************************************************************************************************************/
/* GL2EXT functions */

//////////////////////////
// Non-void functions

#define glGetDebugMessageLogKHR(...)							WRAP_GLCALL_RET(glGetDebugMessageLogKHR, __VA_ARGS__)
#define glGetGraphicsResetStatusKHR(...)						WRAP_GLCALL_RET(glGetGraphicsResetStatusKHR, __VA_ARGS__)
#define glIsEnablediOES(...)									WRAP_GLCALL_RET(glIsEnablediOES, __VA_ARGS__)
#define glUnmapBufferOES(...)									WRAP_GLCALL_RET(glUnmapBufferOES, __VA_ARGS__)
#define glMapBufferOES(...)										WRAP_GLCALL_RET(glMapBufferOES, __VA_ARGS__)
#define glIsVertexArrayOES(...)									WRAP_GLCALL_RET(glIsVertexArrayOES, __VA_ARGS__)
#define glFenceSyncAPPLE(...)									WRAP_GLCALL_RET(glFenceSyncAPPLE, __VA_ARGS__)
#define glIsSyncAPPLE(...)										WRAP_GLCALL_RET(glIsSyncAPPLE, __VA_ARGS__)
#define glClientWaitSyncAPPLE(...)								WRAP_GLCALL_RET(glClientWaitSyncAPPLE, __VA_ARGS__)
#define glGetProgramResourceLocationIndexEXT(...)				WRAP_GLCALL_RET(glGetProgramResourceLocationIndexEXT, __VA_ARGS__)
#define glGetFragDataIndexEXT(...)								WRAP_GLCALL_RET(glGetFragDataIndexEXT, __VA_ARGS__)
#define glIsQueryEXT(...)										WRAP_GLCALL_RET(glIsQueryEXT, __VA_ARGS__)
#define glIsEnablediEXT(...)									WRAP_GLCALL_RET(glIsEnablediEXT, __VA_ARGS__)
#define glMapBufferRangeEXT(...)								WRAP_GLCALL_RET(glMapBufferRangeEXT, __VA_ARGS__)
#define glGetGraphicsResetStatusEXT(...)						WRAP_GLCALL_RET(glGetGraphicsResetStatusEXT, __VA_ARGS__)
#define glCreateShaderProgramvEXT(...)							WRAP_GLCALL_RET(glCreateShaderProgramvEXT, __VA_ARGS__)
#define glIsProgramPipelineEXT(...)								WRAP_GLCALL_RET(glIsProgramPipelineEXT, __VA_ARGS__)
#define glGetFramebufferPixelLocalStorageSizeEXT(...)			WRAP_GLCALL_RET(glGetFramebufferPixelLocalStorageSizeEXT, __VA_ARGS__)
#define glGetTextureHandleIMG(...)								WRAP_GLCALL_RET(glGetTextureHandleIMG, __VA_ARGS__)
#define glGetTextureSamplerHandleIMG(...)						WRAP_GLCALL_RET(glGetTextureSamplerHandleIMG, __VA_ARGS__)
#define glGetTextureHandleNV(...)								WRAP_GLCALL_RET(glGetTextureHandleNV, __VA_ARGS__)
#define glGetTextureSamplerHandleNV(...)						WRAP_GLCALL_RET(glGetTextureSamplerHandleNV, __VA_ARGS__)
#define glGetImageHandleNV(...)									WRAP_GLCALL_RET(glGetImageHandleNV, __VA_ARGS__)
#define glIsTextureHandleResidentNV(...)						WRAP_GLCALL_RET(glIsTextureHandleResidentNV, __VA_ARGS__)
#define glIsImageHandleResidentNV(...)							WRAP_GLCALL_RET(glIsImageHandleResidentNV, __VA_ARGS__)
#define glIsFenceNV(...)										WRAP_GLCALL_RET(glIsFenceNV, __VA_ARGS__)
#define glTestFenceNV(...)										WRAP_GLCALL_RET(glTestFenceNV, __VA_ARGS__)
#define glGenPathsNV(...)										WRAP_GLCALL_RET(glGenPathsNV, __VA_ARGS__)
#define glIsPathNV(...)											WRAP_GLCALL_RET(glIsPathNV, __VA_ARGS__)
#define glIsPointInFillPathNV(...)								WRAP_GLCALL_RET(glIsPointInFillPathNV, __VA_ARGS__)
#define glIsPointInStrokePathNV(...)							WRAP_GLCALL_RET(glIsPointInStrokePathNV, __VA_ARGS__)
#define glGetPathLengthNV(...)									WRAP_GLCALL_RET(glGetPathLengthNV, __VA_ARGS__)
#define glPointAlongPathNV(...)									WRAP_GLCALL_RET(glPointAlongPathNV, __VA_ARGS__)
#define glPathGlyphIndexRangeNV(...)							WRAP_GLCALL_RET(glPathGlyphIndexRangeNV, __VA_ARGS__)
#define glPathGlyphIndexArrayNV(...)							WRAP_GLCALL_RET(glPathGlyphIndexArrayNV, __VA_ARGS__)
#define glPathMemoryGlyphIndexArrayNV(...)						WRAP_GLCALL_RET(glPathMemoryGlyphIndexArrayNV, __VA_ARGS__)
#define glIsEnablediNV(...)										WRAP_GLCALL_RET(glIsEnablediNV, __VA_ARGS__)
#define glExtIsProgramBinaryQCOM(...)							WRAP_GLCALL_RET(glExtIsProgramBinaryQCOM, __VA_ARGS__)


//////////////////////////
// void functions

#define glBlendBarrierKHR(...)									WRAP_GLCALL_NORET(glBlendBarrierKHR, __VA_ARGS__)
#define glDebugMessageControlKHR(...)							WRAP_GLCALL_NORET(glDebugMessageControlKHR, __VA_ARGS__)
#define glDebugMessageInsertKHR(...)							WRAP_GLCALL_NORET(glDebugMessageInsertKHR, __VA_ARGS__)
#define glDebugMessageCallbackKHR(...)							WRAP_GLCALL_NORET(glDebugMessageCallbackKHR, __VA_ARGS__)
#define glPushDebugGroupKHR(...)								WRAP_GLCALL_NORET(glPushDebugGroupKHR, __VA_ARGS__)
#define glPopDebugGroupKHR(...)									WRAP_GLCALL_NORET(glPopDebugGroupKHR, __VA_ARGS__)
#define glObjectLabelKHR(...)									WRAP_GLCALL_NORET(glObjectLabelKHR, __VA_ARGS__)
#define glGetObjectLabelKHR(...)								WRAP_GLCALL_NORET(glGetObjectLabelKHR, __VA_ARGS__)
#define glObjectPtrLabelKHR(...)								WRAP_GLCALL_NORET(glObjectPtrLabelKHR, __VA_ARGS__)
#define glGetObjectPtrLabelKHR(...)								WRAP_GLCALL_NORET(glGetObjectPtrLabelKHR, __VA_ARGS__)
#define glGetPointervKHR(...)									WRAP_GLCALL_NORET(glGetPointervKHR, __VA_ARGS__)
#define glReadnPixelsKHR(...)									WRAP_GLCALL_NORET(glReadnPixelsKHR, __VA_ARGS__)
#define glGetnUniformfvKHR(...)									WRAP_GLCALL_NORET(glGetnUniformfvKHR, __VA_ARGS__)
#define glGetnUniformivKHR(...)									WRAP_GLCALL_NORET(glGetnUniformivKHR, __VA_ARGS__)
#define glGetnUniformuivKHR(...)								WRAP_GLCALL_NORET(glGetnUniformuivKHR, __VA_ARGS__)
#define glEGLImageTargetTexture2DOES(...)						WRAP_GLCALL_NORET(glEGLImageTargetTexture2DOES, __VA_ARGS__)
#define glEGLImageTargetRenderbufferStorageOES(...)				WRAP_GLCALL_NORET(glEGLImageTargetRenderbufferStorageOES, __VA_ARGS__)
#define glCopyImageSubDataOES(...)								WRAP_GLCALL_NORET(glCopyImageSubDataOES, __VA_ARGS__)
#define glEnableiOES(...)										WRAP_GLCALL_NORET(glEnableiOES, __VA_ARGS__)
#define glDisableiOES(...)										WRAP_GLCALL_NORET(glDisableiOES, __VA_ARGS__)
#define glBlendEquationiOES(...)								WRAP_GLCALL_NORET(glBlendEquationiOES, __VA_ARGS__)
#define glBlendEquationSeparateiOES(...)						WRAP_GLCALL_NORET(glBlendEquationSeparateiOES, __VA_ARGS__)
#define glBlendFunciOES(...)									WRAP_GLCALL_NORET(glBlendFunciOES, __VA_ARGS__)
#define glBlendFuncSeparateiOES(...)							WRAP_GLCALL_NORET(glBlendFuncSeparateiOES, __VA_ARGS__)
#define glColorMaskiOES(...)									WRAP_GLCALL_NORET(glColorMaskiOES, __VA_ARGS__)
#define glDrawElementsBaseVertexOES(...)						WRAP_GLCALL_NORET(glDrawElementsBaseVertexOES, __VA_ARGS__)
#define glDrawRangeElementsBaseVertexOES(...)					WRAP_GLCALL_NORET(glDrawRangeElementsBaseVertexOES, __VA_ARGS__)
#define glDrawElementsInstancedBaseVertexOES(...)				WRAP_GLCALL_NORET(glDrawElementsInstancedBaseVertexOES, __VA_ARGS__)
#define glMultiDrawElementsBaseVertexOES(...)					WRAP_GLCALL_NORET(glMultiDrawElementsBaseVertexOES, __VA_ARGS__)
#define glFramebufferTextureOES(...)							WRAP_GLCALL_NORET(glFramebufferTextureOES, __VA_ARGS__)
#define glGetProgramBinaryOES(...)								WRAP_GLCALL_NORET(glGetProgramBinaryOES, __VA_ARGS__)
#define glProgramBinaryOES(...)									WRAP_GLCALL_NORET(glProgramBinaryOES, __VA_ARGS__)
#define glGetBufferPointervOES(...)								WRAP_GLCALL_NORET(glGetBufferPointervOES, __VA_ARGS__)
#define glPrimitiveBoundingBoxOES(...)							WRAP_GLCALL_NORET(glPrimitiveBoundingBoxOES, __VA_ARGS__)
#define glMinSampleShadingOES(...)								WRAP_GLCALL_NORET(glMinSampleShadingOES, __VA_ARGS__)
#define glPatchParameteriOES(...)								WRAP_GLCALL_NORET(glPatchParameteriOES, __VA_ARGS__)
#define glTexImage3DOES(...)									WRAP_GLCALL_NORET(glTexImage3DOES, __VA_ARGS__)
#define glTexSubImage3DOES(...)									WRAP_GLCALL_NORET(glTexSubImage3DOES, __VA_ARGS__)
#define glCopyTexSubImage3DOES(...)								WRAP_GLCALL_NORET(glCopyTexSubImage3DOES, __VA_ARGS__)
#define glCompressedTexImage3DOES(...)							WRAP_GLCALL_NORET(glCompressedTexImage3DOES, __VA_ARGS__)
#define glCompressedTexSubImage3DOES(...)						WRAP_GLCALL_NORET(glCompressedTexSubImage3DOES, __VA_ARGS__)
#define glFramebufferTexture3DOES(...)							WRAP_GLCALL_NORET(glFramebufferTexture3DOES, __VA_ARGS__)
#define glTexParameterIivOES(...)								WRAP_GLCALL_NORET(glTexParameterIivOES, __VA_ARGS__)
#define glTexParameterIuivOES(...)								WRAP_GLCALL_NORET(glTexParameterIuivOES, __VA_ARGS__)
#define glGetTexParameterIivOES(...)							WRAP_GLCALL_NORET(glGetTexParameterIivOES, __VA_ARGS__)
#define glGetTexParameterIuivOES(...)							WRAP_GLCALL_NORET(glGetTexParameterIuivOES, __VA_ARGS__)
#define glSamplerParameterIivOES(...)							WRAP_GLCALL_NORET(glSamplerParameterIivOES, __VA_ARGS__)
#define glSamplerParameterIuivOES(...)							WRAP_GLCALL_NORET(glSamplerParameterIuivOES, __VA_ARGS__)
#define glGetSamplerParameterIivOES(...)						WRAP_GLCALL_NORET(glGetSamplerParameterIivOES, __VA_ARGS__)
#define glGetSamplerParameterIuivOES(...)						WRAP_GLCALL_NORET(glGetSamplerParameterIuivOES, __VA_ARGS__)
#define glTexBufferOES(...)										WRAP_GLCALL_NORET(glTexBufferOES, __VA_ARGS__)
#define glTexBufferRangeOES(...)								WRAP_GLCALL_NORET(glTexBufferRangeOES, __VA_ARGS__)
#define glTexStorage3DMultisampleOES(...)						WRAP_GLCALL_NORET(glTexStorage3DMultisampleOES, __VA_ARGS__)
#define glTextureViewOES(...)									WRAP_GLCALL_NORET(glTextureViewOES, __VA_ARGS__)
#define glBindVertexArrayOES(...)								WRAP_GLCALL_NORET(glBindVertexArrayOES, __VA_ARGS__)
#define glDeleteVertexArraysOES(...)							WRAP_GLCALL_NORET(glDeleteVertexArraysOES, __VA_ARGS__)
#define glGenVertexArraysOES(...)								WRAP_GLCALL_NORET(glGenVertexArraysOES, __VA_ARGS__)
#define glViewportArrayvOES(...)								WRAP_GLCALL_NORET(glViewportArrayvOES, __VA_ARGS__)
#define glViewportIndexedfOES(...)								WRAP_GLCALL_NORET(glViewportIndexedfOES, __VA_ARGS__)
#define glViewportIndexedfvOES(...)								WRAP_GLCALL_NORET(glViewportIndexedfvOES, __VA_ARGS__)
#define glScissorArrayvOES(...)									WRAP_GLCALL_NORET(glScissorArrayvOES, __VA_ARGS__)
#define glScissorIndexedOES(...)								WRAP_GLCALL_NORET(glScissorIndexedOES, __VA_ARGS__)
#define glScissorIndexedvOES(...)								WRAP_GLCALL_NORET(glScissorIndexedvOES, __VA_ARGS__)
#define glDepthRangeArrayfvOES(...)								WRAP_GLCALL_NORET(glDepthRangeArrayfvOES, __VA_ARGS__)
#define glDepthRangeIndexedfOES(...)							WRAP_GLCALL_NORET(glDepthRangeIndexedfOES, __VA_ARGS__)
#define glGetFloati_vOES(...)									WRAP_GLCALL_NORET(glGetFloati_vOES, __VA_ARGS__)
#define glGetPerfMonitorGroupsAMD(...)							WRAP_GLCALL_NORET(glGetPerfMonitorGroupsAMD, __VA_ARGS__)
#define glGetPerfMonitorCountersAMD(...)						WRAP_GLCALL_NORET(glGetPerfMonitorCountersAMD, __VA_ARGS__)
#define glGetPerfMonitorGroupStringAMD(...)						WRAP_GLCALL_NORET(glGetPerfMonitorGroupStringAMD, __VA_ARGS__)
#define glGetPerfMonitorCounterStringAMD(...)					WRAP_GLCALL_NORET(glGetPerfMonitorCounterStringAMD, __VA_ARGS__)
#define glGetPerfMonitorCounterInfoAMD(...)						WRAP_GLCALL_NORET(glGetPerfMonitorCounterInfoAMD, __VA_ARGS__)
#define glGenPerfMonitorsAMD(...)								WRAP_GLCALL_NORET(glGenPerfMonitorsAMD, __VA_ARGS__)
#define glDeletePerfMonitorsAMD(...)							WRAP_GLCALL_NORET(glDeletePerfMonitorsAMD, __VA_ARGS__)
#define glSelectPerfMonitorCountersAMD(...)						WRAP_GLCALL_NORET(glSelectPerfMonitorCountersAMD, __VA_ARGS__)
#define glBeginPerfMonitorAMD(...)								WRAP_GLCALL_NORET(glBeginPerfMonitorAMD, __VA_ARGS__)
#define glEndPerfMonitorAMD(...)								WRAP_GLCALL_NORET(glEndPerfMonitorAMD, __VA_ARGS__)
#define glGetPerfMonitorCounterDataAMD(...)						WRAP_GLCALL_NORET(glGetPerfMonitorCounterDataAMD, __VA_ARGS__)
#define glBlitFramebufferANGLE(...)								WRAP_GLCALL_NORET(glBlitFramebufferANGLE, __VA_ARGS__)
#define glRenderbufferStorageMultisampleANGLE(...)				WRAP_GLCALL_NORET(glRenderbufferStorageMultisampleANGLE, __VA_ARGS__)
#define glDrawArraysInstancedANGLE(...)							WRAP_GLCALL_NORET(glDrawArraysInstancedANGLE, __VA_ARGS__)
#define glDrawElementsInstancedANGLE(...)						WRAP_GLCALL_NORET(glDrawElementsInstancedANGLE, __VA_ARGS__)
#define glVertexAttribDivisorANGLE(...)							WRAP_GLCALL_NORET(glVertexAttribDivisorANGLE, __VA_ARGS__)
#define glGetTranslatedShaderSourceANGLE(...)					WRAP_GLCALL_NORET(glGetTranslatedShaderSourceANGLE, __VA_ARGS__)
#define glCopyTextureLevelsAPPLE(...)							WRAP_GLCALL_NORET(glCopyTextureLevelsAPPLE, __VA_ARGS__)
#define glRenderbufferStorageMultisampleAPPLE(...)				WRAP_GLCALL_NORET(glRenderbufferStorageMultisampleAPPLE, __VA_ARGS__)
#define glResolveMultisampleFramebufferAPPLE(...)				WRAP_GLCALL_NORET(glResolveMultisampleFramebufferAPPLE, __VA_ARGS__)
#define glDeleteSyncAPPLE(...)									WRAP_GLCALL_NORET(glDeleteSyncAPPLE, __VA_ARGS__)
#define glWaitSyncAPPLE(...)									WRAP_GLCALL_NORET(glWaitSyncAPPLE, __VA_ARGS__)
#define glGetInteger64vAPPLE(...)								WRAP_GLCALL_NORET(glGetInteger64vAPPLE, __VA_ARGS__)
#define glGetSyncivAPPLE(...)									WRAP_GLCALL_NORET(glGetSyncivAPPLE, __VA_ARGS__)
#define glDrawArraysInstancedBaseInstanceEXT(...)				WRAP_GLCALL_NORET(glDrawArraysInstancedBaseInstanceEXT, __VA_ARGS__)
#define glDrawElementsInstancedBaseInstanceEXT(...)				WRAP_GLCALL_NORET(glDrawElementsInstancedBaseInstanceEXT, __VA_ARGS__)
#define glDrawElementsInstancedBaseVertexBaseInstanceEXT(...)	WRAP_GLCALL_NORET(glDrawElementsInstancedBaseVertexBaseInstanceEXT, __VA_ARGS__)
#define glBindFragDataLocationIndexedEXT(...)					WRAP_GLCALL_NORET(glBindFragDataLocationIndexedEXT, __VA_ARGS__)
#define glBindFragDataLocationEXT(...)							WRAP_GLCALL_NORET(glBindFragDataLocationEXT, __VA_ARGS__)
#define glBufferStorageEXT(...)									WRAP_GLCALL_NORET(glBufferStorageEXT, __VA_ARGS__)
#define glClearTexImageEXT(...)									WRAP_GLCALL_NORET(glClearTexImageEXT, __VA_ARGS__)
#define glClearTexSubImageEXT(...)								WRAP_GLCALL_NORET(glClearTexSubImageEXT, __VA_ARGS__)
#define glCopyImageSubDataEXT(...)								WRAP_GLCALL_NORET(glCopyImageSubDataEXT, __VA_ARGS__)
#define glLabelObjectEXT(...)									WRAP_GLCALL_NORET(glLabelObjectEXT, __VA_ARGS__)
#define glGetObjectLabelEXT(...)								WRAP_GLCALL_NORET(glGetObjectLabelEXT, __VA_ARGS__)
#define glInsertEventMarkerEXT(...)								WRAP_GLCALL_NORET(glInsertEventMarkerEXT, __VA_ARGS__)
#define glPushGroupMarkerEXT(...)								WRAP_GLCALL_NORET(glPushGroupMarkerEXT, __VA_ARGS__)
#define glPopGroupMarkerEXT(...)								WRAP_GLCALL_NORET(glPopGroupMarkerEXT, __VA_ARGS__)
#define glDiscardFramebufferEXT(...)							WRAP_GLCALL_NORET(glDiscardFramebufferEXT, __VA_ARGS__)
#define glGenQueriesEXT(...)									WRAP_GLCALL_NORET(glGenQueriesEXT, __VA_ARGS__)
#define glDeleteQueriesEXT(...)									WRAP_GLCALL_NORET(glDeleteQueriesEXT, __VA_ARGS__)
#define glBeginQueryEXT(...)									WRAP_GLCALL_NORET(glBeginQueryEXT, __VA_ARGS__)
#define glEndQueryEXT(...)										WRAP_GLCALL_NORET(glEndQueryEXT, __VA_ARGS__)
#define glQueryCounterEXT(...)									WRAP_GLCALL_NORET(glQueryCounterEXT, __VA_ARGS__)
#define glGetQueryivEXT(...)									WRAP_GLCALL_NORET(glGetQueryivEXT, __VA_ARGS__)
#define glGetQueryObjectivEXT(...)								WRAP_GLCALL_NORET(glGetQueryObjectivEXT, __VA_ARGS__)
#define glGetQueryObjectuivEXT(...)								WRAP_GLCALL_NORET(glGetQueryObjectuivEXT, __VA_ARGS__)
#define glGetQueryObjecti64vEXT(...)							WRAP_GLCALL_NORET(glGetQueryObjecti64vEXT, __VA_ARGS__)
#define glGetQueryObjectui64vEXT(...)							WRAP_GLCALL_NORET(glGetQueryObjectui64vEXT, __VA_ARGS__)
#define glDrawBuffersEXT(...)									WRAP_GLCALL_NORET(glDrawBuffersEXT, __VA_ARGS__)
#define glEnableiEXT(...)										WRAP_GLCALL_NORET(glEnableiEXT, __VA_ARGS__)
#define glDisableiEXT(...)										WRAP_GLCALL_NORET(glDisableiEXT, __VA_ARGS__)
#define glBlendEquationiEXT(...)								WRAP_GLCALL_NORET(glBlendEquationiEXT, __VA_ARGS__)
#define glBlendEquationSeparateiEXT(...)						WRAP_GLCALL_NORET(glBlendEquationSeparateiEXT, __VA_ARGS__)
#define glBlendFunciEXT(...)									WRAP_GLCALL_NORET(glBlendFunciEXT, __VA_ARGS__)
#define glBlendFuncSeparateiEXT(...)							WRAP_GLCALL_NORET(glBlendFuncSeparateiEXT, __VA_ARGS__)
#define glColorMaskiEXT(...)									WRAP_GLCALL_NORET(glColorMaskiEXT, __VA_ARGS__)
#define glDrawElementsBaseVertexEXT(...)						WRAP_GLCALL_NORET(glDrawElementsBaseVertexEXT, __VA_ARGS__)
#define glDrawRangeElementsBaseVertexEXT(...)					WRAP_GLCALL_NORET(glDrawRangeElementsBaseVertexEXT, __VA_ARGS__)
#define glDrawElementsInstancedBaseVertexEXT(...)				WRAP_GLCALL_NORET(glDrawElementsInstancedBaseVertexEXT, __VA_ARGS__)
#define glMultiDrawElementsBaseVertexEXT(...)					WRAP_GLCALL_NORET(glMultiDrawElementsBaseVertexEXT, __VA_ARGS__)
#define glDrawArraysInstancedEXT(...)							WRAP_GLCALL_NORET(glDrawArraysInstancedEXT, __VA_ARGS__)
#define glDrawElementsInstancedEXT(...)							WRAP_GLCALL_NORET(glDrawElementsInstancedEXT, __VA_ARGS__)
#define glDrawTransformFeedbackEXT(...)							WRAP_GLCALL_NORET(glDrawTransformFeedbackEXT, __VA_ARGS__)
#define glDrawTransformFeedbackInstancedEXT(...)				WRAP_GLCALL_NORET(glDrawTransformFeedbackInstancedEXT, __VA_ARGS__)
#define glFramebufferTextureEXT(...)							WRAP_GLCALL_NORET(glFramebufferTextureEXT, __VA_ARGS__)
#define glVertexAttribDivisorEXT(...)							WRAP_GLCALL_NORET(glVertexAttribDivisorEXT, __VA_ARGS__)
#define glFlushMappedBufferRangeEXT(...)						WRAP_GLCALL_NORET(glFlushMappedBufferRangeEXT, __VA_ARGS__)
#define glMultiDrawArraysEXT(...)								WRAP_GLCALL_NORET(glMultiDrawArraysEXT, __VA_ARGS__)
#define glMultiDrawElementsEXT(...)								WRAP_GLCALL_NORET(glMultiDrawElementsEXT, __VA_ARGS__)
#define glMultiDrawArraysIndirectEXT(...)						WRAP_GLCALL_NORET(glMultiDrawArraysIndirectEXT, __VA_ARGS__)
#define glMultiDrawElementsIndirectEXT(...)						WRAP_GLCALL_NORET(glMultiDrawElementsIndirectEXT, __VA_ARGS__)
#define glRenderbufferStorageMultisampleEXT(...)				WRAP_GLCALL_NORET(glRenderbufferStorageMultisampleEXT, __VA_ARGS__)
#define glFramebufferTexture2DMultisampleEXT(...)				WRAP_GLCALL_NORET(glFramebufferTexture2DMultisampleEXT, __VA_ARGS__)
#define glReadBufferIndexedEXT(...)								WRAP_GLCALL_NORET(glReadBufferIndexedEXT, __VA_ARGS__)
#define glDrawBuffersIndexedEXT(...)							WRAP_GLCALL_NORET(glDrawBuffersIndexedEXT, __VA_ARGS__)
#define glGetIntegeri_vEXT(...)									WRAP_GLCALL_NORET(glGetIntegeri_vEXT, __VA_ARGS__)
#define glPolygonOffsetClampEXT(...)							WRAP_GLCALL_NORET(glPolygonOffsetClampEXT, __VA_ARGS__)
#define glPrimitiveBoundingBoxEXT(...)							WRAP_GLCALL_NORET(glPrimitiveBoundingBoxEXT, __VA_ARGS__)
#define glRasterSamplesEXT(...)									WRAP_GLCALL_NORET(glRasterSamplesEXT, __VA_ARGS__)
#define glReadnPixelsEXT(...)									WRAP_GLCALL_NORET(glReadnPixelsEXT, __VA_ARGS__)
#define glGetnUniformfvEXT(...)									WRAP_GLCALL_NORET(glGetnUniformfvEXT, __VA_ARGS__)
#define glGetnUniformivEXT(...)									WRAP_GLCALL_NORET(glGetnUniformivEXT, __VA_ARGS__)
#define glActiveShaderProgramEXT(...)							WRAP_GLCALL_NORET(glActiveShaderProgramEXT, __VA_ARGS__)
#define glBindProgramPipelineEXT(...)							WRAP_GLCALL_NORET(glBindProgramPipelineEXT, __VA_ARGS__)
#define glDeleteProgramPipelinesEXT(...)						WRAP_GLCALL_NORET(glDeleteProgramPipelinesEXT, __VA_ARGS__)
#define glGenProgramPipelinesEXT(...)							WRAP_GLCALL_NORET(glGenProgramPipelinesEXT, __VA_ARGS__)
#define glGetProgramPipelineInfoLogEXT(...)						WRAP_GLCALL_NORET(glGetProgramPipelineInfoLogEXT, __VA_ARGS__)
#define glGetProgramPipelineivEXT(...)							WRAP_GLCALL_NORET(glGetProgramPipelineivEXT, __VA_ARGS__)
#define glProgramParameteriEXT(...)								WRAP_GLCALL_NORET(glProgramParameteriEXT, __VA_ARGS__)
#define glProgramUniform1fEXT(...)								WRAP_GLCALL_NORET(glProgramUniform1fEXT, __VA_ARGS__)
#define glProgramUniform1fvEXT(...)								WRAP_GLCALL_NORET(glProgramUniform1fvEXT, __VA_ARGS__)
#define glProgramUniform1iEXT(...)								WRAP_GLCALL_NORET(glProgramUniform1iEXT, __VA_ARGS__)
#define glProgramUniform1ivEXT(...)								WRAP_GLCALL_NORET(glProgramUniform1ivEXT, __VA_ARGS__)
#define glProgramUniform2fEXT(...)								WRAP_GLCALL_NORET(glProgramUniform2fEXT, __VA_ARGS__)
#define glProgramUniform2fvEXT(...)								WRAP_GLCALL_NORET(glProgramUniform2fvEXT, __VA_ARGS__)
#define glProgramUniform2iEXT(...)								WRAP_GLCALL_NORET(glProgramUniform2iEXT, __VA_ARGS__)
#define glProgramUniform2ivEXT(...)								WRAP_GLCALL_NORET(glProgramUniform2ivEXT, __VA_ARGS__)
#define glProgramUniform3fEXT(...)								WRAP_GLCALL_NORET(glProgramUniform3fEXT, __VA_ARGS__)
#define glProgramUniform3fvEXT(...)								WRAP_GLCALL_NORET(glProgramUniform3fvEXT, __VA_ARGS__)
#define glProgramUniform3iEXT(...)								WRAP_GLCALL_NORET(glProgramUniform3iEXT, __VA_ARGS__)
#define glProgramUniform3ivEXT(...)								WRAP_GLCALL_NORET(glProgramUniform3ivEXT, __VA_ARGS__)
#define glProgramUniform4fEXT(...)								WRAP_GLCALL_NORET(glProgramUniform4fEXT, __VA_ARGS__)
#define glProgramUniform4fvEXT(...)								WRAP_GLCALL_NORET(glProgramUniform4fvEXT, __VA_ARGS__)
#define glProgramUniform4iEXT(...)								WRAP_GLCALL_NORET(glProgramUniform4iEXT, __VA_ARGS__)
#define glProgramUniform4ivEXT(...)								WRAP_GLCALL_NORET(glProgramUniform4ivEXT, __VA_ARGS__)
#define glProgramUniformMatrix2fvEXT(...)						WRAP_GLCALL_NORET(glProgramUniformMatrix2fvEXT, __VA_ARGS__)
#define glProgramUniformMatrix3fvEXT(...)						WRAP_GLCALL_NORET(glProgramUniformMatrix3fvEXT, __VA_ARGS__)
#define glProgramUniformMatrix4fvEXT(...)						WRAP_GLCALL_NORET(glProgramUniformMatrix4fvEXT, __VA_ARGS__)
#define glUseProgramStagesEXT(...)								WRAP_GLCALL_NORET(glUseProgramStagesEXT, __VA_ARGS__)
#define glValidateProgramPipelineEXT(...)						WRAP_GLCALL_NORET(glValidateProgramPipelineEXT, __VA_ARGS__)
#define glProgramUniform1uiEXT(...)								WRAP_GLCALL_NORET(glProgramUniform1uiEXT, __VA_ARGS__)
#define glProgramUniform2uiEXT(...)								WRAP_GLCALL_NORET(glProgramUniform2uiEXT, __VA_ARGS__)
#define glProgramUniform3uiEXT(...)								WRAP_GLCALL_NORET(glProgramUniform3uiEXT, __VA_ARGS__)
#define glProgramUniform4uiEXT(...)								WRAP_GLCALL_NORET(glProgramUniform4uiEXT, __VA_ARGS__)
#define glProgramUniform1uivEXT(...)							WRAP_GLCALL_NORET(glProgramUniform1uivEXT, __VA_ARGS__)
#define glProgramUniform2uivEXT(...)							WRAP_GLCALL_NORET(glProgramUniform2uivEXT, __VA_ARGS__)
#define glProgramUniform3uivEXT(...)							WRAP_GLCALL_NORET(glProgramUniform3uivEXT, __VA_ARGS__)
#define glProgramUniform4uivEXT(...)							WRAP_GLCALL_NORET(glProgramUniform4uivEXT, __VA_ARGS__)
#define glProgramUniformMatrix2x3fvEXT(...)						WRAP_GLCALL_NORET(glProgramUniformMatrix2x3fvEXT, __VA_ARGS__)
#define glProgramUniformMatrix3x2fvEXT(...)						WRAP_GLCALL_NORET(glProgramUniformMatrix3x2fvEXT, __VA_ARGS__)
#define glProgramUniformMatrix2x4fvEXT(...)						WRAP_GLCALL_NORET(glProgramUniformMatrix2x4fvEXT, __VA_ARGS__)
#define glProgramUniformMatrix4x2fvEXT(...)						WRAP_GLCALL_NORET(glProgramUniformMatrix4x2fvEXT, __VA_ARGS__)
#define glProgramUniformMatrix3x4fvEXT(...)						WRAP_GLCALL_NORET(glProgramUniformMatrix3x4fvEXT, __VA_ARGS__)
#define glProgramUniformMatrix4x3fvEXT(...)						WRAP_GLCALL_NORET(glProgramUniformMatrix4x3fvEXT, __VA_ARGS__)
#define glFramebufferPixelLocalStorageSizeEXT(...)				WRAP_GLCALL_NORET(glFramebufferPixelLocalStorageSizeEXT, __VA_ARGS__)
#define glClearPixelLocalStorageuiEXT(...)						WRAP_GLCALL_NORET(glClearPixelLocalStorageuiEXT, __VA_ARGS__)
#define glTexPageCommitmentEXT(...)								WRAP_GLCALL_NORET(glTexPageCommitmentEXT, __VA_ARGS__)
#define glPatchParameteriEXT(...)								WRAP_GLCALL_NORET(glPatchParameteriEXT, __VA_ARGS__)
#define glTexParameterIivEXT(...)								WRAP_GLCALL_NORET(glTexParameterIivEXT, __VA_ARGS__)
#define glTexParameterIuivEXT(...)								WRAP_GLCALL_NORET(glTexParameterIuivEXT, __VA_ARGS__)
#define glGetTexParameterIivEXT(...)							WRAP_GLCALL_NORET(glGetTexParameterIivEXT, __VA_ARGS__)
#define glGetTexParameterIuivEXT(...)							WRAP_GLCALL_NORET(glGetTexParameterIuivEXT, __VA_ARGS__)
#define glSamplerParameterIivEXT(...)							WRAP_GLCALL_NORET(glSamplerParameterIivEXT, __VA_ARGS__)
#define glSamplerParameterIuivEXT(...)							WRAP_GLCALL_NORET(glSamplerParameterIuivEXT, __VA_ARGS__)
#define glGetSamplerParameterIivEXT(...)						WRAP_GLCALL_NORET(glGetSamplerParameterIivEXT, __VA_ARGS__)
#define glGetSamplerParameterIuivEXT(...)						WRAP_GLCALL_NORET(glGetSamplerParameterIuivEXT, __VA_ARGS__)
#define glTexBufferEXT(...)										WRAP_GLCALL_NORET(glTexBufferEXT, __VA_ARGS__)
#define glTexBufferRangeEXT(...)								WRAP_GLCALL_NORET(glTexBufferRangeEXT, __VA_ARGS__)
#define glTexStorage1DEXT(...)									WRAP_GLCALL_NORET(glTexStorage1DEXT, __VA_ARGS__)
#define glTexStorage2DEXT(...)									WRAP_GLCALL_NORET(glTexStorage2DEXT, __VA_ARGS__)
#define glTexStorage3DEXT(...)									WRAP_GLCALL_NORET(glTexStorage3DEXT, __VA_ARGS__)
#define glTextureStorage1DEXT(...)								WRAP_GLCALL_NORET(glTextureStorage1DEXT, __VA_ARGS__)
#define glTextureStorage2DEXT(...)								WRAP_GLCALL_NORET(glTextureStorage2DEXT, __VA_ARGS__)
#define glTextureStorage3DEXT(...)								WRAP_GLCALL_NORET(glTextureStorage3DEXT, __VA_ARGS__)
#define glTextureViewEXT(...)									WRAP_GLCALL_NORET(glTextureViewEXT, __VA_ARGS__)
#define glWindowRectanglesEXT(...)								WRAP_GLCALL_NORET(glWindowRectanglesEXT, __VA_ARGS__)
#define glUniformHandleui64IMG(...)								WRAP_GLCALL_NORET(glUniformHandleui64IMG, __VA_ARGS__)
#define glUniformHandleui64vIMG(...)							WRAP_GLCALL_NORET(glUniformHandleui64vIMG, __VA_ARGS__)
#define glProgramUniformHandleui64IMG(...)						WRAP_GLCALL_NORET(glProgramUniformHandleui64IMG, __VA_ARGS__)
#define glProgramUniformHandleui64vIMG(...)						WRAP_GLCALL_NORET(glProgramUniformHandleui64vIMG, __VA_ARGS__)
#define glFramebufferTexture2DDownsampleIMG(...)				WRAP_GLCALL_NORET(glFramebufferTexture2DDownsampleIMG, __VA_ARGS__)
#define glFramebufferTextureLayerDownsampleIMG(...)				WRAP_GLCALL_NORET(glFramebufferTextureLayerDownsampleIMG, __VA_ARGS__)
#define glRenderbufferStorageMultisampleIMG(...)				WRAP_GLCALL_NORET(glRenderbufferStorageMultisampleIMG, __VA_ARGS__)
#define glFramebufferTexture2DMultisampleIMG(...)				WRAP_GLCALL_NORET(glFramebufferTexture2DMultisampleIMG, __VA_ARGS__)
#define glApplyFramebufferAttachmentCMAAINTEL(...)				WRAP_GLCALL_NORET(glApplyFramebufferAttachmentCMAAINTEL, __VA_ARGS__)
#define glBeginPerfQueryINTEL(...)								WRAP_GLCALL_NORET(glBeginPerfQueryINTEL, __VA_ARGS__)
#define glCreatePerfQueryINTEL(...)								WRAP_GLCALL_NORET(glCreatePerfQueryINTEL, __VA_ARGS__)
#define glDeletePerfQueryINTEL(...)								WRAP_GLCALL_NORET(glDeletePerfQueryINTEL, __VA_ARGS__)
#define glEndPerfQueryINTEL(...)								WRAP_GLCALL_NORET(glEndPerfQueryINTEL, __VA_ARGS__)
#define glGetFirstPerfQueryIdINTEL(...)							WRAP_GLCALL_NORET(glGetFirstPerfQueryIdINTEL, __VA_ARGS__)
#define glGetNextPerfQueryIdINTEL(...)							WRAP_GLCALL_NORET(glGetNextPerfQueryIdINTEL, __VA_ARGS__)
#define glGetPerfCounterInfoINTEL(...)							WRAP_GLCALL_NORET(glGetPerfCounterInfoINTEL, __VA_ARGS__)
#define glGetPerfQueryDataINTEL(...)							WRAP_GLCALL_NORET(glGetPerfQueryDataINTEL, __VA_ARGS__)
#define glGetPerfQueryIdByNameINTEL(...)						WRAP_GLCALL_NORET(glGetPerfQueryIdByNameINTEL, __VA_ARGS__)
#define glGetPerfQueryInfoINTEL(...)							WRAP_GLCALL_NORET(glGetPerfQueryInfoINTEL, __VA_ARGS__)
#define glMakeTextureHandleResidentNV(...)						WRAP_GLCALL_NORET(glMakeTextureHandleResidentNV, __VA_ARGS__)
#define glMakeTextureHandleNonResidentNV(...)					WRAP_GLCALL_NORET(glMakeTextureHandleNonResidentNV, __VA_ARGS__)
#define glMakeImageHandleResidentNV(...)						WRAP_GLCALL_NORET(glMakeImageHandleResidentNV, __VA_ARGS__)
#define glMakeImageHandleNonResidentNV(...)						WRAP_GLCALL_NORET(glMakeImageHandleNonResidentNV, __VA_ARGS__)
#define glUniformHandleui64NV(...)								WRAP_GLCALL_NORET(glUniformHandleui64NV, __VA_ARGS__)
#define glUniformHandleui64vNV(...)								WRAP_GLCALL_NORET(glUniformHandleui64vNV, __VA_ARGS__)
#define glProgramUniformHandleui64NV(...)						WRAP_GLCALL_NORET(glProgramUniformHandleui64NV, __VA_ARGS__)
#define glProgramUniformHandleui64vNV(...)						WRAP_GLCALL_NORET(glProgramUniformHandleui64vNV, __VA_ARGS__)
#define glBlendParameteriNV(...)								WRAP_GLCALL_NORET(glBlendParameteriNV, __VA_ARGS__)
#define glBlendBarrierNV(...)									WRAP_GLCALL_NORET(glBlendBarrierNV, __VA_ARGS__)
#define glBeginConditionalRenderNV(...)							WRAP_GLCALL_NORET(glBeginConditionalRenderNV, __VA_ARGS__)
#define glEndConditionalRenderNV(...)							WRAP_GLCALL_NORET(glEndConditionalRenderNV, __VA_ARGS__)
#define glSubpixelPrecisionBiasNV(...)							WRAP_GLCALL_NORET(glSubpixelPrecisionBiasNV, __VA_ARGS__)
#define glConservativeRasterParameteriNV(...)					WRAP_GLCALL_NORET(glConservativeRasterParameteriNV, __VA_ARGS__)
#define glCopyBufferSubDataNV(...)								WRAP_GLCALL_NORET(glCopyBufferSubDataNV, __VA_ARGS__)
#define glCoverageMaskNV(...)									WRAP_GLCALL_NORET(glCoverageMaskNV, __VA_ARGS__)
#define glCoverageOperationNV(...)								WRAP_GLCALL_NORET(glCoverageOperationNV, __VA_ARGS__)
#define glDrawBuffersNV(...)									WRAP_GLCALL_NORET(glDrawBuffersNV, __VA_ARGS__)
#define glDrawArraysInstancedNV(...)							WRAP_GLCALL_NORET(glDrawArraysInstancedNV, __VA_ARGS__)
#define glDrawElementsInstancedNV(...)							WRAP_GLCALL_NORET(glDrawElementsInstancedNV, __VA_ARGS__)
#define glDeleteFencesNV(...)									WRAP_GLCALL_NORET(glDeleteFencesNV, __VA_ARGS__)
#define glGenFencesNV(...)										WRAP_GLCALL_NORET(glGenFencesNV, __VA_ARGS__)
#define glGetFenceivNV(...)										WRAP_GLCALL_NORET(glGetFenceivNV, __VA_ARGS__)
#define glFinishFenceNV(...)									WRAP_GLCALL_NORET(glFinishFenceNV, __VA_ARGS__)
#define glSetFenceNV(...)										WRAP_GLCALL_NORET(glSetFenceNV, __VA_ARGS__)
#define glFragmentCoverageColorNV(...)							WRAP_GLCALL_NORET(glFragmentCoverageColorNV, __VA_ARGS__)
#define glBlitFramebufferNV(...)								WRAP_GLCALL_NORET(glBlitFramebufferNV, __VA_ARGS__)
#define glCoverageModulationTableNV(...)						WRAP_GLCALL_NORET(glCoverageModulationTableNV, __VA_ARGS__)
#define glGetCoverageModulationTableNV(...)						WRAP_GLCALL_NORET(glGetCoverageModulationTableNV, __VA_ARGS__)
#define glCoverageModulationNV(...)								WRAP_GLCALL_NORET(glCoverageModulationNV, __VA_ARGS__)
#define glRenderbufferStorageMultisampleNV(...)					WRAP_GLCALL_NORET(glRenderbufferStorageMultisampleNV, __VA_ARGS__)
#define glUniform1i64NV(...)									WRAP_GLCALL_NORET(glUniform1i64NV, __VA_ARGS__)
#define glUniform2i64NV(...)									WRAP_GLCALL_NORET(glUniform2i64NV, __VA_ARGS__)
#define glUniform3i64NV(...)									WRAP_GLCALL_NORET(glUniform3i64NV, __VA_ARGS__)
#define glUniform4i64NV(...)									WRAP_GLCALL_NORET(glUniform4i64NV, __VA_ARGS__)
#define glUniform1i64vNV(...)									WRAP_GLCALL_NORET(glUniform1i64vNV, __VA_ARGS__)
#define glUniform2i64vNV(...)									WRAP_GLCALL_NORET(glUniform2i64vNV, __VA_ARGS__)
#define glUniform3i64vNV(...)									WRAP_GLCALL_NORET(glUniform3i64vNV, __VA_ARGS__)
#define glUniform4i64vNV(...)									WRAP_GLCALL_NORET(glUniform4i64vNV, __VA_ARGS__)
#define glUniform1ui64NV(...)									WRAP_GLCALL_NORET(glUniform1ui64NV, __VA_ARGS__)
#define glUniform2ui64NV(...)									WRAP_GLCALL_NORET(glUniform2ui64NV, __VA_ARGS__)
#define glUniform3ui64NV(...)									WRAP_GLCALL_NORET(glUniform3ui64NV, __VA_ARGS__)
#define glUniform4ui64NV(...)									WRAP_GLCALL_NORET(glUniform4ui64NV, __VA_ARGS__)
#define glUniform1ui64vNV(...)									WRAP_GLCALL_NORET(glUniform1ui64vNV, __VA_ARGS__)
#define glUniform2ui64vNV(...)									WRAP_GLCALL_NORET(glUniform2ui64vNV, __VA_ARGS__)
#define glUniform3ui64vNV(...)									WRAP_GLCALL_NORET(glUniform3ui64vNV, __VA_ARGS__)
#define glUniform4ui64vNV(...)									WRAP_GLCALL_NORET(glUniform4ui64vNV, __VA_ARGS__)
#define glGetUniformi64vNV(...)									WRAP_GLCALL_NORET(glGetUniformi64vNV, __VA_ARGS__)
#define glProgramUniform1i64NV(...)								WRAP_GLCALL_NORET(glProgramUniform1i64NV, __VA_ARGS__)
#define glProgramUniform2i64NV(...)								WRAP_GLCALL_NORET(glProgramUniform2i64NV, __VA_ARGS__)
#define glProgramUniform3i64NV(...)								WRAP_GLCALL_NORET(glProgramUniform3i64NV, __VA_ARGS__)
#define glProgramUniform4i64NV(...)								WRAP_GLCALL_NORET(glProgramUniform4i64NV, __VA_ARGS__)
#define glProgramUniform1i64vNV(...)							WRAP_GLCALL_NORET(glProgramUniform1i64vNV, __VA_ARGS__)
#define glProgramUniform2i64vNV(...)							WRAP_GLCALL_NORET(glProgramUniform2i64vNV, __VA_ARGS__)
#define glProgramUniform3i64vNV(...)							WRAP_GLCALL_NORET(glProgramUniform3i64vNV, __VA_ARGS__)
#define glProgramUniform4i64vNV(...)							WRAP_GLCALL_NORET(glProgramUniform4i64vNV, __VA_ARGS__)
#define glProgramUniform1ui64NV(...)							WRAP_GLCALL_NORET(glProgramUniform1ui64NV, __VA_ARGS__)
#define glProgramUniform2ui64NV(...)							WRAP_GLCALL_NORET(glProgramUniform2ui64NV, __VA_ARGS__)
#define glProgramUniform3ui64NV(...)							WRAP_GLCALL_NORET(glProgramUniform3ui64NV, __VA_ARGS__)
#define glProgramUniform4ui64NV(...)							WRAP_GLCALL_NORET(glProgramUniform4ui64NV, __VA_ARGS__)
#define glProgramUniform1ui64vNV(...)							WRAP_GLCALL_NORET(glProgramUniform1ui64vNV, __VA_ARGS__)
#define glProgramUniform2ui64vNV(...)							WRAP_GLCALL_NORET(glProgramUniform2ui64vNV, __VA_ARGS__)
#define glProgramUniform3ui64vNV(...)							WRAP_GLCALL_NORET(glProgramUniform3ui64vNV, __VA_ARGS__)
#define glProgramUniform4ui64vNV(...)							WRAP_GLCALL_NORET(glProgramUniform4ui64vNV, __VA_ARGS__)
#define glVertexAttribDivisorNV(...)							WRAP_GLCALL_NORET(glVertexAttribDivisorNV, __VA_ARGS__)
#define glGetInternalformatSampleivNV(...)						WRAP_GLCALL_NORET(glGetInternalformatSampleivNV, __VA_ARGS__)
#define glUniformMatrix2x3fvNV(...)								WRAP_GLCALL_NORET(glUniformMatrix2x3fvNV, __VA_ARGS__)
#define glUniformMatrix3x2fvNV(...)								WRAP_GLCALL_NORET(glUniformMatrix3x2fvNV, __VA_ARGS__)
#define glUniformMatrix2x4fvNV(...)								WRAP_GLCALL_NORET(glUniformMatrix2x4fvNV, __VA_ARGS__)
#define glUniformMatrix4x2fvNV(...)								WRAP_GLCALL_NORET(glUniformMatrix4x2fvNV, __VA_ARGS__)
#define glUniformMatrix3x4fvNV(...)								WRAP_GLCALL_NORET(glUniformMatrix3x4fvNV, __VA_ARGS__)
#define glUniformMatrix4x3fvNV(...)								WRAP_GLCALL_NORET(glUniformMatrix4x3fvNV, __VA_ARGS__)
#define glDeletePathsNV(...)									WRAP_GLCALL_NORET(glDeletePathsNV, __VA_ARGS__)
#define glPathCommandsNV(...)									WRAP_GLCALL_NORET(glPathCommandsNV, __VA_ARGS__)
#define glPathCoordsNV(...)										WRAP_GLCALL_NORET(glPathCoordsNV, __VA_ARGS__)
#define glPathSubCommandsNV(...)								WRAP_GLCALL_NORET(glPathSubCommandsNV, __VA_ARGS__)
#define glPathSubCoordsNV(...)									WRAP_GLCALL_NORET(glPathSubCoordsNV, __VA_ARGS__)
#define glPathStringNV(...)										WRAP_GLCALL_NORET(glPathStringNV, __VA_ARGS__)
#define glPathGlyphsNV(...)										WRAP_GLCALL_NORET(glPathGlyphsNV, __VA_ARGS__)
#define glPathGlyphRangeNV(...)									WRAP_GLCALL_NORET(glPathGlyphRangeNV, __VA_ARGS__)
#define glWeightPathsNV(...)									WRAP_GLCALL_NORET(glWeightPathsNV, __VA_ARGS__)
#define glCopyPathNV(...)										WRAP_GLCALL_NORET(glCopyPathNV, __VA_ARGS__)
#define glInterpolatePathsNV(...)								WRAP_GLCALL_NORET(glInterpolatePathsNV, __VA_ARGS__)
#define glTransformPathNV(...)									WRAP_GLCALL_NORET(glTransformPathNV, __VA_ARGS__)
#define glPathParameterivNV(...)								WRAP_GLCALL_NORET(glPathParameterivNV, __VA_ARGS__)
#define glPathParameteriNV(...)									WRAP_GLCALL_NORET(glPathParameteriNV, __VA_ARGS__)
#define glPathParameterfvNV(...)								WRAP_GLCALL_NORET(glPathParameterfvNV, __VA_ARGS__)
#define glPathParameterfNV(...)									WRAP_GLCALL_NORET(glPathParameterfNV, __VA_ARGS__)
#define glPathDashArrayNV(...)									WRAP_GLCALL_NORET(glPathDashArrayNV, __VA_ARGS__)
#define glPathStencilFuncNV(...)								WRAP_GLCALL_NORET(glPathStencilFuncNV, __VA_ARGS__)
#define glPathStencilDepthOffsetNV(...)							WRAP_GLCALL_NORET(glPathStencilDepthOffsetNV, __VA_ARGS__)
#define glStencilFillPathNV(...)								WRAP_GLCALL_NORET(glStencilFillPathNV, __VA_ARGS__)
#define glStencilStrokePathNV(...)								WRAP_GLCALL_NORET(glStencilStrokePathNV, __VA_ARGS__)
#define glStencilFillPathInstancedNV(...)						WRAP_GLCALL_NORET(glStencilFillPathInstancedNV, __VA_ARGS__)
#define glStencilStrokePathInstancedNV(...)						WRAP_GLCALL_NORET(glStencilStrokePathInstancedNV, __VA_ARGS__)
#define glPathCoverDepthFuncNV(...)								WRAP_GLCALL_NORET(glPathCoverDepthFuncNV, __VA_ARGS__)
#define glCoverFillPathNV(...)									WRAP_GLCALL_NORET(glCoverFillPathNV, __VA_ARGS__)
#define glCoverStrokePathNV(...)								WRAP_GLCALL_NORET(glCoverStrokePathNV, __VA_ARGS__)
#define glCoverFillPathInstancedNV(...)							WRAP_GLCALL_NORET(glCoverFillPathInstancedNV, __VA_ARGS__)
#define glCoverStrokePathInstancedNV(...)						WRAP_GLCALL_NORET(glCoverStrokePathInstancedNV, __VA_ARGS__)
#define glGetPathParameterivNV(...)								WRAP_GLCALL_NORET(glGetPathParameterivNV, __VA_ARGS__)
#define glGetPathParameterfvNV(...)								WRAP_GLCALL_NORET(glGetPathParameterfvNV, __VA_ARGS__)
#define glGetPathCommandsNV(...)								WRAP_GLCALL_NORET(glGetPathCommandsNV, __VA_ARGS__)
#define glGetPathCoordsNV(...)									WRAP_GLCALL_NORET(glGetPathCoordsNV, __VA_ARGS__)
#define glGetPathDashArrayNV(...)								WRAP_GLCALL_NORET(glGetPathDashArrayNV, __VA_ARGS__)
#define glGetPathMetricsNV(...)									WRAP_GLCALL_NORET(glGetPathMetricsNV, __VA_ARGS__)
#define glGetPathMetricRangeNV(...)								WRAP_GLCALL_NORET(glGetPathMetricRangeNV, __VA_ARGS__)
#define glGetPathSpacingNV(...)									WRAP_GLCALL_NORET(glGetPathSpacingNV, __VA_ARGS__)
#define glMatrixLoad3x2fNV(...)									WRAP_GLCALL_NORET(glMatrixLoad3x2fNV, __VA_ARGS__)
#define glMatrixLoad3x3fNV(...)									WRAP_GLCALL_NORET(glMatrixLoad3x3fNV, __VA_ARGS__)
#define glMatrixLoadTranspose3x3fNV(...)						WRAP_GLCALL_NORET(glMatrixLoadTranspose3x3fNV, __VA_ARGS__)
#define glMatrixMult3x2fNV(...)									WRAP_GLCALL_NORET(glMatrixMult3x2fNV, __VA_ARGS__)
#define glMatrixMult3x3fNV(...)									WRAP_GLCALL_NORET(glMatrixMult3x3fNV, __VA_ARGS__)
#define glMatrixMultTranspose3x3fNV(...)						WRAP_GLCALL_NORET(glMatrixMultTranspose3x3fNV, __VA_ARGS__)
#define glStencilThenCoverFillPathNV(...)						WRAP_GLCALL_NORET(glStencilThenCoverFillPathNV, __VA_ARGS__)
#define glStencilThenCoverStrokePathNV(...)						WRAP_GLCALL_NORET(glStencilThenCoverStrokePathNV, __VA_ARGS__)
#define glStencilThenCoverFillPathInstancedNV(...)				WRAP_GLCALL_NORET(glStencilThenCoverFillPathInstancedNV, __VA_ARGS__)
#define glStencilThenCoverStrokePathInstancedNV(...)			WRAP_GLCALL_NORET(glStencilThenCoverStrokePathInstancedNV, __VA_ARGS__)
#define glProgramPathFragmentInputGenNV(...)					WRAP_GLCALL_NORET(glProgramPathFragmentInputGenNV, __VA_ARGS__)
#define glGetProgramResourcefvNV(...)							WRAP_GLCALL_NORET(glGetProgramResourcefvNV, __VA_ARGS__)
#define glPolygonModeNV(...)									WRAP_GLCALL_NORET(glPolygonModeNV, __VA_ARGS__)
#define glReadBufferNV(...)										WRAP_GLCALL_NORET(glReadBufferNV, __VA_ARGS__)
#define glFramebufferSampleLocationsfvNV(...)					WRAP_GLCALL_NORET(glFramebufferSampleLocationsfvNV, __VA_ARGS__)
#define glNamedFramebufferSampleLocationsfvNV(...)				WRAP_GLCALL_NORET(glNamedFramebufferSampleLocationsfvNV, __VA_ARGS__)
#define glResolveDepthValuesNV(...)								WRAP_GLCALL_NORET(glResolveDepthValuesNV, __VA_ARGS__)
#define glViewportArrayvNV(...)									WRAP_GLCALL_NORET(glViewportArrayvNV, __VA_ARGS__)
#define glViewportIndexedfNV(...)								WRAP_GLCALL_NORET(glViewportIndexedfNV, __VA_ARGS__)
#define glViewportIndexedfvNV(...)								WRAP_GLCALL_NORET(glViewportIndexedfvNV, __VA_ARGS__)
#define glScissorArrayvNV(...)									WRAP_GLCALL_NORET(glScissorArrayvNV, __VA_ARGS__)
#define glScissorIndexedNV(...)									WRAP_GLCALL_NORET(glScissorIndexedNV, __VA_ARGS__)
#define glScissorIndexedvNV(...)								WRAP_GLCALL_NORET(glScissorIndexedvNV, __VA_ARGS__)
#define glDepthRangeArrayfvNV(...)								WRAP_GLCALL_NORET(glDepthRangeArrayfvNV, __VA_ARGS__)
#define glDepthRangeIndexedfNV(...)								WRAP_GLCALL_NORET(glDepthRangeIndexedfNV, __VA_ARGS__)
#define glGetFloati_vNV(...)									WRAP_GLCALL_NORET(glGetFloati_vNV, __VA_ARGS__)
#define glEnableiNV(...)										WRAP_GLCALL_NORET(glEnableiNV, __VA_ARGS__)
#define glDisableiNV(...)										WRAP_GLCALL_NORET(glDisableiNV, __VA_ARGS__)
#define glViewportSwizzleNV(...)								WRAP_GLCALL_NORET(glViewportSwizzleNV, __VA_ARGS__)
#define glFramebufferTextureMultiviewOVR(...)					WRAP_GLCALL_NORET(glFramebufferTextureMultiviewOVR, __VA_ARGS__)
#define glFramebufferTextureMultisampleMultiviewOVR(...)		WRAP_GLCALL_NORET(glFramebufferTextureMultisampleMultiviewOVR, __VA_ARGS__)
#define glAlphaFuncQCOM(...)									WRAP_GLCALL_NORET(glAlphaFuncQCOM, __VA_ARGS__)
#define glGetDriverControlsQCOM(...)							WRAP_GLCALL_NORET(glGetDriverControlsQCOM, __VA_ARGS__)
#define glGetDriverControlStringQCOM(...)						WRAP_GLCALL_NORET(glGetDriverControlStringQCOM, __VA_ARGS__)
#define glEnableDriverControlQCOM(...)							WRAP_GLCALL_NORET(glEnableDriverControlQCOM, __VA_ARGS__)
#define glDisableDriverControlQCOM(...)							WRAP_GLCALL_NORET(glDisableDriverControlQCOM, __VA_ARGS__)
#define glExtGetTexturesQCOM(...)								WRAP_GLCALL_NORET(glExtGetTexturesQCOM, __VA_ARGS__)
#define glExtGetBuffersQCOM(...)								WRAP_GLCALL_NORET(glExtGetBuffersQCOM, __VA_ARGS__)
#define glExtGetRenderbuffersQCOM(...)							WRAP_GLCALL_NORET(glExtGetRenderbuffersQCOM, __VA_ARGS__)
#define glExtGetFramebuffersQCOM(...)							WRAP_GLCALL_NORET(glExtGetFramebuffersQCOM, __VA_ARGS__)
#define glExtGetTexLevelParameterivQCOM(...)					WRAP_GLCALL_NORET(glExtGetTexLevelParameterivQCOM, __VA_ARGS__)
#define glExtTexObjectStateOverrideiQCOM(...)					WRAP_GLCALL_NORET(glExtTexObjectStateOverrideiQCOM, __VA_ARGS__)
#define glExtGetTexSubImageQCOM(...)							WRAP_GLCALL_NORET(glExtGetTexSubImageQCOM, __VA_ARGS__)
#define glExtGetBufferPointervQCOM(...)							WRAP_GLCALL_NORET(glExtGetBufferPointervQCOM, __VA_ARGS__)
#define glExtGetShadersQCOM(...)								WRAP_GLCALL_NORET(glExtGetShadersQCOM, __VA_ARGS__)
#define glExtGetProgramsQCOM(...)								WRAP_GLCALL_NORET(glExtGetProgramsQCOM, __VA_ARGS__)
#define glExtGetProgramBinarySourceQCOM(...)					WRAP_GLCALL_NORET(glExtGetProgramBinarySourceQCOM, __VA_ARGS__)
#define glStartTilingQCOM(...)									WRAP_GLCALL_NORET(glStartTilingQCOM, __VA_ARGS__)
#define glEndTilingQCOM(...)									WRAP_GLCALL_NORET(glEndTilingQCOM, __VA_ARGS__)





/***********************************************************************************************************************/
/* EGL wrappers */

void	wrappedEGLErrorSet(EGLint error);
EGLint	wrappedEGLErrorGet();


// For consistence with GL version (and due to my lazyness to compile for the 100th time),
// EGLCALL_WRAPPER_BEFORE() and EGLCALL_WRAPPER_AFTER() use a for() loop, although eglGetError()
// is supposed to reset the error to EGL_SUCCESS at each invocation

// This should never happen - it'd mean we missed wrapping functions
#define	EGLCALL_WRAPPER_BEFORE(method)\
	for(auto __error = (eglGetError)(); __error != EGL_SUCCESS; __error = (eglGetError)())			\
	{																								\
		const char *__errstr = nullptr;																\
		switch(__error)																				\
		{																							\
			case EGL_NOT_INITIALIZED:				__errstr = "EGL_NOT_INITIALIZED"; break;		\
			case EGL_BAD_ACCESS:					__errstr = "EGL_BAD_ACCESS"; break;				\
			case EGL_BAD_ALLOC:						__errstr = "EGL_BAD_ALLOC"; break;				\
			case EGL_BAD_ATTRIBUTE:					__errstr = "EGL_BAD_ATTRIBUTE"; break;			\
			case EGL_BAD_CONFIG:					__errstr = "EGL_BAD_CONFIG"; break;				\
			case EGL_BAD_CONTEXT:					__errstr = "EGL_BAD_CONTEXT"; break;			\
			case EGL_BAD_CURRENT_SURFACE:			__errstr = "EGL_BAD_CURRENT_SURFACE"; break;	\
			case EGL_BAD_DISPLAY:					__errstr = "EGL_BAD_DISPLAY"; break;			\
			case EGL_BAD_MATCH:						__errstr = "EGL_BAD_MATCH"; break;				\
			case EGL_BAD_NATIVE_PIXMAP:				__errstr = "EGL_BAD_NATIVE_PIXMAP"; break;		\
			case EGL_BAD_NATIVE_WINDOW:				__errstr = "EGL_BAD_NATIVE_WINDOW"; break;		\
			case EGL_BAD_PARAMETER:					__errstr = "EGL_BAD_PARAMETER"; break;			\
			case EGL_BAD_SURFACE:					__errstr = "EGL_BAD_SURFACE"; break;			\
			case EGL_CONTEXT_LOST:					__errstr = "EGL_CONTEXT_LOST"; break;			\
			default:								__errstr = "UNKNOWN";							\
		}																							\
		printf("*** Previously unreported EGL error [%s] (0x%04X) before the call to %s() from %s in %s:%d\n", __errstr, __error, method, __PRETTY_FUNCTION__, __FILE__, __LINE__);	\
	}

#define	EGLCALL_WRAPPER_AFTER(method)\
	for(auto __error = (eglGetError)(); __error != EGL_SUCCESS; __error = (eglGetError)())			\
	{																								\
		const char *__errstr = nullptr;																\
		switch(__error)																				\
		{																							\
			case EGL_NOT_INITIALIZED:				__errstr = "EGL_NOT_INITIALIZED"; break;		\
			case EGL_BAD_ACCESS:					__errstr = "EGL_BAD_ACCESS"; break;				\
			case EGL_BAD_ALLOC:						__errstr = "EGL_BAD_ALLOC"; break;				\
			case EGL_BAD_ATTRIBUTE:					__errstr = "EGL_BAD_ATTRIBUTE"; break;			\
			case EGL_BAD_CONFIG:					__errstr = "EGL_BAD_CONFIG"; break;				\
			case EGL_BAD_CONTEXT:					__errstr = "EGL_BAD_CONTEXT"; break;			\
			case EGL_BAD_CURRENT_SURFACE:			__errstr = "EGL_BAD_CURRENT_SURFACE"; break;	\
			case EGL_BAD_DISPLAY:					__errstr = "EGL_BAD_DISPLAY"; break;			\
			case EGL_BAD_MATCH:						__errstr = "EGL_BAD_MATCH"; break;				\
			case EGL_BAD_NATIVE_PIXMAP:				__errstr = "EGL_BAD_NATIVE_PIXMAP"; break;		\
			case EGL_BAD_NATIVE_WINDOW:				__errstr = "EGL_BAD_NATIVE_WINDOW"; break;		\
			case EGL_BAD_PARAMETER:					__errstr = "EGL_BAD_PARAMETER"; break;			\
			case EGL_BAD_SURFACE:					__errstr = "EGL_BAD_SURFACE"; break;			\
			case EGL_CONTEXT_LOST:					__errstr = "EGL_CONTEXT_LOST"; break;			\
			default:								__errstr = "UNKNOWN";							\
		}																							\
		wrappedEGLErrorSet(__error);																\
		printf("*** Caught EGL error [%s] (0x%04X) in the call to %s() from %s in %s:%d\n", __errstr, __error, method, __PRETTY_FUNCTION__, __FILE__, __LINE__);	\
	}

// All EGL functions happen to return (non-void)
#define	WRAP_EGLCALL(method, ...) ([&]() mutable { EGLCALL_WRAPPER_BEFORE(#method); wrappedEGLErrorSet(EGL_SUCCESS); auto ret = (method)(__VA_ARGS__); EGLCALL_WRAPPER_AFTER(#method); return ret; })()



/***********************************************************************************************************************/
/* EGL functions */

//////////////////////////
// Non-void functions

#define eglGetDisplay(...)										WRAP_EGLCALL(eglGetDisplay, __VA_ARGS__)
#define eglInitialize(...)										WRAP_EGLCALL(eglInitialize, __VA_ARGS__)
#define eglTerminate(...)										WRAP_EGLCALL(eglTerminate, __VA_ARGS__)
#define eglQueryString(...)										WRAP_EGLCALL(eglQueryString, __VA_ARGS__)
#define eglGetConfigs(...)										WRAP_EGLCALL(eglGetConfigs, __VA_ARGS__)
#define eglChooseConfig(...)									WRAP_EGLCALL(eglChooseConfig, __VA_ARGS__)
#define eglGetConfigAttrib(...)									WRAP_EGLCALL(eglGetConfigAttrib, __VA_ARGS__)
#define eglCreateWindowSurface(...)								WRAP_EGLCALL(eglCreateWindowSurface, __VA_ARGS__)
#define eglCreatePbufferSurface(...)							WRAP_EGLCALL(eglCreatePbufferSurface, __VA_ARGS__)
#define eglCreatePixmapSurface(...)								WRAP_EGLCALL(eglCreatePixmapSurface, __VA_ARGS__)
#define eglDestroySurface(...)									WRAP_EGLCALL(eglDestroySurface, __VA_ARGS__)
#define eglQuerySurface(...)									WRAP_EGLCALL(eglQuerySurface, __VA_ARGS__)
#define eglBindAPI(...)											WRAP_EGLCALL(eglBindAPI, __VA_ARGS__)
#define eglQueryAPI(...)										WRAP_EGLCALL(eglQueryAPI, __VA_ARGS__)
#define eglWaitClient(...)										WRAP_EGLCALL(eglWaitClient, __VA_ARGS__)
#define eglReleaseThread(...)									WRAP_EGLCALL(eglReleaseThread, __VA_ARGS__)
#define eglCreatePbufferFromClientBuffer(...)					WRAP_EGLCALL(eglCreatePbufferFromClientBuffer, __VA_ARGS__)
#define eglSurfaceAttrib(...)									WRAP_EGLCALL(eglSurfaceAttrib, __VA_ARGS__)
#define eglBindTexImage(...)									WRAP_EGLCALL(eglBindTexImage, __VA_ARGS__)
#define eglReleaseTexImage(...)									WRAP_EGLCALL(eglReleaseTexImage, __VA_ARGS__)
#define eglSwapInterval(...)									WRAP_EGLCALL(eglSwapInterval, __VA_ARGS__)
#define eglCreateContext(...)									WRAP_EGLCALL(eglCreateContext, __VA_ARGS__)
#define eglDestroyContext(...)									WRAP_EGLCALL(eglDestroyContext, __VA_ARGS__)
#define eglMakeCurrent(...)										WRAP_EGLCALL(eglMakeCurrent, __VA_ARGS__)
#define eglGetCurrentContext(...)								WRAP_EGLCALL(eglGetCurrentContext, __VA_ARGS__)
#define eglGetCurrentSurface(...)								WRAP_EGLCALL(eglGetCurrentSurface, __VA_ARGS__)
#define eglGetCurrentDisplay(...)								WRAP_EGLCALL(eglGetCurrentDisplay, __VA_ARGS__)
#define eglQueryContext(...)									WRAP_EGLCALL(eglQueryContext, __VA_ARGS__)
#define eglWaitGL(...)											WRAP_EGLCALL(eglWaitGL, __VA_ARGS__)
#define eglWaitNative(...)										WRAP_EGLCALL(eglWaitNative, __VA_ARGS__)
#define eglSwapBuffers(...)										WRAP_EGLCALL(eglSwapBuffers, __VA_ARGS__)
#define eglCopyBuffers(...)										WRAP_EGLCALL(eglCopyBuffers, __VA_ARGS__)
#define eglGetProcAddress(...)									WRAP_EGLCALL(eglGetProcAddress, __VA_ARGS__)


//////////////////////////
// The error function

#define eglGetError()											wrappedEGLErrorGet()



#endif // defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)



#endif // (CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN)

#endif // __PLATFORM_EMSCRIPTEN_CCGL_H__
