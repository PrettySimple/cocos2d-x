/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

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
 
#include <cocos/platform/CCPlatformConfig.h>
#if defined(CC_USE_GL) || defined(CC_USE_GLES)
#include "ShaderModuleGL.h"

#include <cocos/platform/CCPlatformMacros.h>
#include <cocos/base/ccMacros.h>

#include "glsl_optimizer.h"

CC_BACKEND_BEGIN

ShaderModuleGL::ShaderModuleGL(ShaderStage stage, const std::string& source)
: ShaderModule(stage)
{
    compileShader(stage, source);
}

ShaderModuleGL::~ShaderModuleGL()
{
    deleteShader();
}

void ShaderModuleGL::compileShader(ShaderStage stage, const std::string &source)
{
    // Optimise GLSL shader for GLES20
    static glslopt_ctx* ctx = glslopt_initialize(kGlslTargetOpenGLES20);
    glslopt_shader_type st = stage == ShaderStage::VERTEX ? kGlslOptShaderVertex : kGlslOptShaderFragment;
    glslopt_shader* glslShader = glslopt_optimize(ctx, st, source.c_str(), 0);
    if (!glslShader)
    {
        CCLOG("Can not optimise GLSL shader:");
        CCLOG("%s", source.c_str());
        return;
    }

    const char* optimShader = glslopt_get_output(glslShader);
    if (!optimShader)
    {
        CCLOG("Can not get optimised shader:");
        CCLOG("%s", source.c_str());
        CCLOG("glslopt log %s", glslopt_get_log(glslShader));
        return;
    }
    
    GLenum shaderType = stage == ShaderStage::VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
    const GLchar* sourcePtr = reinterpret_cast<const GLchar*>(optimShader);
    _shader = glCreateShader(shaderType);
    if (!_shader)
        return;
    
    glShaderSource(_shader, 1, &sourcePtr, nullptr);
    glCompileShader(_shader);
    
    GLint status = 0;
    glGetShaderiv(_shader, GL_COMPILE_STATUS, &status);
    if (! status)
    {
        CCLOG("cocos2d: ERROR: Failed to compile shader:\n%s", source.c_str());
        CCLOG("cocos2d: %s", getErrorLog(_shader));
        deleteShader();
        CCASSERT(false, "Shader compile failed!");
    }

    glslopt_shader_delete(glslShader);
}

char* ShaderModuleGL::getErrorLog(GLuint shader) const
{
    GLint logLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    char* log = (char*)malloc(sizeof(char) * logLength);
    glGetShaderInfoLog(shader, logLength, nullptr, log);
    return log;
}

void ShaderModuleGL::deleteShader()
{
    if (_shader)
    {
        glDeleteShader(_shader);
        _shader = 0;
    }
}

CC_BACKEND_END
#endif
