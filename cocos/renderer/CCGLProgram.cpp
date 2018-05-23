/****************************************************************************
Copyright 2011 Jeff Lamarche
Copyright 2012 Goffredo Marocchi
Copyright 2012 Ricardo Quesada
Copyright 2012 cocos2d-x.org
Copyright 2013-2016 Chukong Technologies Inc.

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
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN false EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "renderer/CCGLProgram.h"

#ifndef WIN32
#include <alloca.h>
#endif

#include "base/CCDirector.h"
#include "base/ccUTF8.h"
#include "platform/CCFileUtils.h"
#include "platform/CCPlatformConfig.h"
#include "renderer/ccGLStateCache.h"

#include <cstring>
#include <utility>

// helper functions

static void replaceDefines(const std::string& compileTimeDefines, std::string& out)
{
    // Replace semicolons with '#define ... \n'
    if (!compileTimeDefines.empty())
    {
        // append ';' if the last char doesn't have one
        auto copyDefines = compileTimeDefines;
        if (copyDefines[copyDefines.length()-1] != ';')
            copyDefines.append(1, ';');

        std::string currentDefine;

        for (auto itChar: copyDefines)
        {
            if (itChar == ';')
            {
                if (!currentDefine.empty())
                {
                    out.append("\n#define " + currentDefine);
                    currentDefine.clear();
                }
            }
            else
            {
                currentDefine.append(1, itChar);
            }
        }
        out += "\n";
    }
}


NS_CC_BEGIN
const char* GLProgram::SHADER_NAME_ETC1AS_POSITION_TEXTURE_COLOR = "#ShaderETC1ASPositionTextureColor";
const char* GLProgram::SHADER_NAME_ETC1AS_POSITION_TEXTURE_COLOR_NO_MVP = "#ShaderETC1ASPositionTextureColor_noMVP";

const char* GLProgram::SHADER_NAME_ETC1AS_POSITION_TEXTURE_GRAY = "#ShaderETC1ASPositionTextureGray";
const char* GLProgram::SHADER_NAME_ETC1AS_POSITION_TEXTURE_GRAY_NO_MVP = "#ShaderETC1ASPositionTextureGray_noMVP";

const char* GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR = "ShaderPositionTextureColor";
const char* GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP = "ShaderPositionTextureColor_noMVP";
const char* GLProgram::SHADER_NAME_POSITION_TEXTURE_ALPHA_TEST = "ShaderPositionTextureColorAlphaTest";
const char* GLProgram::SHADER_NAME_POSITION_TEXTURE_ALPHA_TEST_NO_MV = "ShaderPositionTextureColorAlphaTest_NoMV";
const char* GLProgram::SHADER_NAME_POSITION_COLOR = "ShaderPositionColor";
const char* GLProgram::SHADER_NAME_POSITION_COLOR_TEXASPOINTSIZE = "ShaderPositionColorTexAsPointsize";
const char* GLProgram::SHADER_NAME_POSITION_COLOR_NO_MVP = "ShaderPositionColor_noMVP";

const char* GLProgram::SHADER_NAME_POSITION_TEXTURE = "ShaderPositionTexture";
const char* GLProgram::SHADER_NAME_POSITION_TEXTURE_U_COLOR = "ShaderPositionTexture_uColor";
const char* GLProgram::SHADER_NAME_POSITION_TEXTURE_A8_COLOR = "ShaderPositionTextureA8Color";
const char* GLProgram::SHADER_NAME_POSITION_U_COLOR = "ShaderPosition_uColor";
const char* GLProgram::SHADER_NAME_POSITION_LENGTH_TEXTURE_COLOR = "ShaderPositionLengthTextureColor";
const char* GLProgram::SHADER_NAME_POSITION_GRAYSCALE = "ShaderUIGrayScale";
const char* GLProgram::SHADER_NAME_LABEL_DISTANCEFIELD_NORMAL = "ShaderLabelDFNormal";
const char* GLProgram::SHADER_NAME_LABEL_DISTANCEFIELD_GLOW = "ShaderLabelDFGlow";
const char* GLProgram::SHADER_NAME_LABEL_NORMAL = "ShaderLabelNormal";
const char* GLProgram::SHADER_NAME_LABEL_OUTLINE = "ShaderLabelOutline";

const char* GLProgram::SHADER_3D_POSITION = "Shader3DPosition";
const char* GLProgram::SHADER_3D_POSITION_TEXTURE = "Shader3DPositionTexture";
const char* GLProgram::SHADER_3D_SKINPOSITION_TEXTURE = "Shader3DSkinPositionTexture";
const char* GLProgram::SHADER_3D_POSITION_NORMAL = "Shader3DPositionNormal";
const char* GLProgram::SHADER_3D_POSITION_NORMAL_TEXTURE = "Shader3DPositionNormalTexture";
const char* GLProgram::SHADER_3D_SKINPOSITION_NORMAL_TEXTURE = "Shader3DSkinPositionNormalTexture";
const char* GLProgram::SHADER_3D_POSITION_BUMPEDNORMAL_TEXTURE = "Shader3DPositionBumpedNormalTexture";
const char* GLProgram::SHADER_3D_SKINPOSITION_BUMPEDNORMAL_TEXTURE = "Shader3DSkinPositionBumpedNormalTexture";
const char* GLProgram::SHADER_3D_PARTICLE_COLOR = "Shader3DParticleColor";
const char* GLProgram::SHADER_3D_PARTICLE_TEXTURE = "Shader3DParticleTexture";
const char* GLProgram::SHADER_3D_SKYBOX = "Shader3DSkybox";
const char* GLProgram::SHADER_3D_TERRAIN = "Shader3DTerrain";
const char* GLProgram::SHADER_CAMERA_CLEAR = "ShaderCameraClear";


// uniform names
const char* GLProgram::UNIFORM_NAME_AMBIENT_COLOR = "CC_AmbientColor";
const char* GLProgram::UNIFORM_NAME_P_MATRIX = "CC_PMatrix";
const char* GLProgram::UNIFORM_NAME_MV_MATRIX = "CC_MVMatrix";
const char* GLProgram::UNIFORM_NAME_MVP_MATRIX  = "CC_MVPMatrix";
const char* GLProgram::UNIFORM_NAME_NORMAL_MATRIX = "CC_NormalMatrix";
const char* GLProgram::UNIFORM_NAME_TIME = "CC_Time";
const char* GLProgram::UNIFORM_NAME_SIN_TIME = "CC_SinTime";
const char* GLProgram::UNIFORM_NAME_COS_TIME = "CC_CosTime";
const char* GLProgram::UNIFORM_NAME_RANDOM01 = "CC_Random01";
const char* GLProgram::UNIFORM_NAME_SAMPLER0 = "CC_Texture0";
const char* GLProgram::UNIFORM_NAME_SAMPLER1 = "CC_Texture1";
const char* GLProgram::UNIFORM_NAME_SAMPLER2 = "CC_Texture2";
const char* GLProgram::UNIFORM_NAME_SAMPLER3 = "CC_Texture3";
const char* GLProgram::UNIFORM_NAME_ALPHA_TEST_VALUE = "CC_alpha_value";
#ifdef DEBUG_TEXTURE_SIZE
const char* GLProgram::UNIFORM_NAME_DEBUG = "CC_Debug";
const char* GLProgram::UNIFORM_NAME_TEX_SIZE = "u_TexSize";
#endif

// Attribute names
const char* GLProgram::ATTRIBUTE_NAME_COLOR = "a_color";
const char* GLProgram::ATTRIBUTE_NAME_POSITION = "a_position";
const char* GLProgram::ATTRIBUTE_NAME_TEX_COORD = "a_texCoord";
const char* GLProgram::ATTRIBUTE_NAME_TEX_COORD1 = "a_texCoord1";
const char* GLProgram::ATTRIBUTE_NAME_TEX_COORD2 = "a_texCoord2";
const char* GLProgram::ATTRIBUTE_NAME_TEX_COORD3 = "a_texCoord3";
const char* GLProgram::ATTRIBUTE_NAME_NORMAL = "a_normal";
const char* GLProgram::ATTRIBUTE_NAME_BLEND_WEIGHT = "a_blendWeight";
const char* GLProgram::ATTRIBUTE_NAME_BLEND_INDEX = "a_blendIndex";
const char* GLProgram::ATTRIBUTE_NAME_TANGENT = "a_tangent";
const char* GLProgram::ATTRIBUTE_NAME_BINORMAL = "a_binormal";

#ifdef DEBUG_TEXTURE_SIZE
GLuint GLProgram::_mipmapId = 0;
#endif

static const char * COCOS2D_SHADER_UNIFORMS = R"(
#ifdef GL_ES
uniform highp mat4 CC_PMatrix;
uniform highp mat4 CC_MVMatrix;
uniform highp mat4 CC_MVPMatrix;
uniform highp mat3 CC_NormalMatrix;
uniform highp vec4 CC_Time;
uniform highp vec4 CC_SinTime;
uniform highp vec4 CC_CosTime;
uniform lowp vec4 CC_Random01;
uniform mediump sampler2D CC_Texture0;
uniform mediump sampler2D CC_Texture1;
uniform mediump sampler2D CC_Texture2;
uniform mediump sampler2D CC_Texture3;
    #ifdef DEBUG_TEXTURE_SIZE
uniform lowp int CC_Debug;
uniform mediump vec2 u_TexSize;
    #endif
#else
uniform mat4 CC_PMatrix;
uniform mat4 CC_MVMatrix;
uniform mat4 CC_MVPMatrix;
uniform mat3 CC_NormalMatrix;
uniform vec4 CC_Time;
uniform vec4 CC_SinTime;
uniform vec4 CC_CosTime;
uniform vec4 CC_Random01;
uniform sampler2D CC_Texture0;
uniform sampler2D CC_Texture1;
uniform sampler2D CC_Texture2;
uniform sampler2D CC_Texture3;
    #ifdef DEBUG_TEXTURE_SIZE
uniform int CC_Debug;
uniform vec2 u_TexSize;
    #endif
#endif
//CC INCLUDES END

)";

static const std::string EMPTY_DEFINE;

GLProgram* GLProgram::createWithByteArrays(const GLchar* vShaderByteArray, const GLchar* fShaderByteArray)
{
    return createWithByteArrays(vShaderByteArray, fShaderByteArray, EMPTY_DEFINE);
}

GLProgram* GLProgram::createWithByteArrays(const GLchar* vShaderByteArray, const GLchar* fShaderByteArray, const std::string& compileTimeDefines)
{
    auto ret = new (std::nothrow) GLProgram();
    if(ret && ret->initWithByteArrays(vShaderByteArray, fShaderByteArray, compileTimeDefines)) {
        ret->link();
        ret->updateUniforms();
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}


GLProgram* GLProgram::createWithFilenames(const std::string& vShaderFilename, const std::string& fShaderFilename)
{
    return createWithFilenames(vShaderFilename, fShaderFilename, EMPTY_DEFINE);
}

GLProgram* GLProgram::createWithFilenames(const std::string& vShaderFilename, const std::string& fShaderFilename, const std::string& compileTimeDefines)
{
    auto ret = new (std::nothrow) GLProgram();
    if(ret && ret->initWithFilenames(vShaderFilename, fShaderFilename, compileTimeDefines)) {
        ret->link();
        ret->updateUniforms();
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

GLProgram::GLProgram()
{
    _director = Director::getInstance();
    CCASSERT(nullptr != _director, "Director is null when init a GLProgram");
    _builtInUniforms.fill(-1);
}

GLProgram::~GLProgram()
{
    CCLOGINFO("%s %d deallocing GLProgram: %p", __FUNCTION__, __LINE__, this);

    clearShader();

    if (_program)
    {
        GL::deleteProgram(_program);
    }

    _hashForUniforms.clear();
}

bool GLProgram::initWithByteArrays(const GLchar* vShaderByteArray, const GLchar* fShaderByteArray)
{
    return initWithByteArrays(vShaderByteArray, fShaderByteArray, EMPTY_DEFINE);
}

bool GLProgram::initWithByteArrays(const GLchar* vShaderByteArray, const GLchar* fShaderByteArray, const std::string& compileTimeDefines)
{
    _program = glCreateProgram();
    CHECK_GL_ERROR_DEBUG();

    // convert defines here. If we do it in "compileShader" we will do it twice.
    // a cache for the defines could be useful, but seems like overkill at this point
    std::string replacedDefines = "";
    replaceDefines(compileTimeDefines, replacedDefines);

    _vertShader = _fragShader = 0;

    if (vShaderByteArray)
    {
        if (!compileShader(&_vertShader, GL_VERTEX_SHADER, vShaderByteArray, replacedDefines))
        {
            CCLOG("cocos2d: ERROR: Failed to compile vertex shader");
            return false;
       }
    }

    // Create and compile fragment shader
    if (fShaderByteArray)
    {
        if (!compileShader(&_fragShader, GL_FRAGMENT_SHADER, fShaderByteArray, replacedDefines))
        {
            CCLOG("cocos2d: ERROR: Failed to compile fragment shader");
            return false;
        }
    }

    if (_vertShader)
    {
        glAttachShader(_program, _vertShader);
    }
    CHECK_GL_ERROR_DEBUG();

    if (_fragShader)
    {
        glAttachShader(_program, _fragShader);
    }

    _hashForUniforms.clear();

    CHECK_GL_ERROR_DEBUG();

    return true;
}

bool GLProgram::initWithFilenames(const std::string& vShaderFilename, const std::string& fShaderFilename)
{
    return initWithFilenames(vShaderFilename, fShaderFilename, EMPTY_DEFINE);
}

bool GLProgram::initWithFilenames(const std::string& vShaderFilename, const std::string& fShaderFilename, const std::string& compileTimeDefines)
{
    auto fileUtils = FileUtils::getInstance();
    std::string vertexSource = fileUtils->getStringFromFile(FileUtils::getInstance()->fullPathForFilename(vShaderFilename));
    std::string fragmentSource = fileUtils->getStringFromFile(FileUtils::getInstance()->fullPathForFilename(fShaderFilename));

    return initWithByteArrays(vertexSource.c_str(), fragmentSource.c_str(), compileTimeDefines);
}

void GLProgram::bindPredefinedVertexAttribs()
{
    std::array<std::pair<char const*, GLuint>, 7> const attribute_locations = {
        std::make_pair(GLProgram::ATTRIBUTE_NAME_POSITION, static_cast<GLuint>(GLProgram::VERTEX_ATTRIB_POSITION))
        , std::make_pair(GLProgram::ATTRIBUTE_NAME_COLOR, static_cast<GLuint>(GLProgram::VERTEX_ATTRIB_COLOR))
        , std::make_pair(GLProgram::ATTRIBUTE_NAME_TEX_COORD, static_cast<GLuint>(GLProgram::VERTEX_ATTRIB_TEX_COORD))
        , std::make_pair(GLProgram::ATTRIBUTE_NAME_TEX_COORD1, static_cast<GLuint>(GLProgram::VERTEX_ATTRIB_TEX_COORD1))
        , std::make_pair(GLProgram::ATTRIBUTE_NAME_TEX_COORD2, static_cast<GLuint>(GLProgram::VERTEX_ATTRIB_TEX_COORD2))
        , std::make_pair(GLProgram::ATTRIBUTE_NAME_TEX_COORD3, static_cast<GLuint>(GLProgram::VERTEX_ATTRIB_TEX_COORD3))
        , std::make_pair(GLProgram::ATTRIBUTE_NAME_NORMAL, static_cast<GLuint>(GLProgram::VERTEX_ATTRIB_NORMAL))
    };

    for (auto [attributeName, location] : attribute_locations)
    {
        glBindAttribLocation(_program, location, attributeName);
    }
}

void GLProgram::parseVertexAttribs()
{
    //_vertexAttribs.clear();

    // Query and store vertex attribute meta-data from the program.
    GLint activeAttributes;
    GLint length;
    glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &activeAttributes);
    if(activeAttributes > 0)
    {
        VertexAttrib attribute;

        glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &length);
        if(length > 0)
        {
            GLchar* attribName = (GLchar*) alloca(length + 1);

            for(int i = 0; i < activeAttributes; ++i)
            {
                // Query attribute info.
                glGetActiveAttrib(_program, i, length, nullptr, &attribute.size, &attribute.type, attribName);
                attribName[length] = '\0';
                attribute.name = std::string(attribName);

                // Query the pre-assigned attribute location
                attribute.index = glGetAttribLocation(_program, attribName);
                _vertexAttribs[attribute.name] = attribute;
            }
        }
    }
    else
    {
        GLchar ErrorLog[1024];
        glGetProgramInfoLog(_program, sizeof(ErrorLog), nullptr, ErrorLog);
        CCLOG("Error linking shader program: '%s'\n", ErrorLog);
    }
}

void GLProgram::parseUniforms()
{
    //_userUniforms.clear();

    // Query and store uniforms from the program.
    GLint activeUniforms;
    glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &activeUniforms);
    if(activeUniforms > 0)
    {
        GLint length;
        glGetProgramiv(_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &length);
        if(length > 0)
        {
            Uniform uniform;

            GLchar* uniformName = (GLchar*)alloca(length + 1);

            for(int i = 0; i < activeUniforms; ++i)
            {
                // Query uniform info.
                glGetActiveUniform(_program, i, length, nullptr, &uniform.size, &uniform.type, uniformName);
                uniformName[length] = '\0';

                // Only add uniforms that are not built-in.
                // The ones that start with 'CC_' are built-ins
                if(strncmp("CC_", uniformName, 3) != 0) {

                    // remove possible array '[]' from uniform name
                    if(length > 3)
                    {
                        char* c = strrchr(uniformName, '[');
                        if(c)
                        {
                            *c = '\0';
                        }
                    }
                    uniform.name = std::string(uniformName);
                    uniform.location = glGetUniformLocation(_program, uniformName);
                    GLenum __gl_error_code = glGetError();
                    if (__gl_error_code != GL_NO_ERROR)
                    {
                        CCLOG("error: 0x%x  uniformName: %s", (int)__gl_error_code, uniformName);
                    }
                    assert(__gl_error_code == GL_NO_ERROR);

                    _userUniforms[uniform.name] = uniform;
                }
            }
        }
    }
    else
    {
        GLchar ErrorLog[1024];
        glGetProgramInfoLog(_program, sizeof(ErrorLog), nullptr, ErrorLog);
        CCLOG("Error linking shader program: '%s'\n", ErrorLog);

    }

}

Uniform* GLProgram::getUniform(const std::string &name)
{
    const auto itr = _userUniforms.find(name);
    if( itr != _userUniforms.end())
        return &itr->second;
    return nullptr;
}

VertexAttrib* GLProgram::getVertexAttrib(const std::string &name)
{
    const auto itr = _vertexAttribs.find(name);
    if( itr != _vertexAttribs.end())
        return &itr->second;
    return nullptr;
}

std::string GLProgram::getDescription() const
{
    return StringUtils::format("<GLProgram = "
                                      CC_FORMAT_PRINTF_SIZE_T
                                      " | Program = %i, VertexShader = %i, FragmentShader = %i>",
                                      (size_t)this, _program, _vertShader, _fragShader);
}

bool GLProgram::compileShader(GLuint * shader, GLenum type, const GLchar* source)
{
    return compileShader(shader, type, source, "");
}

bool GLProgram::compileShader(GLuint* shader, GLenum type, const GLchar* source, const std::string& convertedDefines)
{
    GLint status;

    if (!source)
    {
        return false;
    }

    GLchar const* sources[] = {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT || CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN)
        "precision mediump float;\nprecision mediump int;\n",
#elif (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32 && CC_TARGET_PLATFORM != CC_PLATFORM_LINUX && CC_TARGET_PLATFORM != CC_PLATFORM_MAC)
        (type == GL_VERTEX_SHADER ? "precision highp float;\nprecision highp int;\n" : "precision mediump float;\nprecision mediump int;\n"),
#endif
#ifdef DEBUG_TEXTURE_SIZE
        "#define DEBUG_TEXTURE_SIZE 1\n",
#endif
        COCOS2D_SHADER_UNIFORMS,
        convertedDefines.c_str(),
        source
    };

    *shader = glCreateShader(type);
    glShaderSource(*shader, sizeof(sources)/sizeof(*sources), sources, nullptr);
    glCompileShader(*shader);

    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);

    if (! status)
    {
        GLsizei length;
        glGetShaderiv(*shader, GL_SHADER_SOURCE_LENGTH, &length);
        GLchar* src = new GLchar[length];

        glGetShaderSource(*shader, length, nullptr, src);
        CCLOG("cocos2d: ERROR: Failed to compile shader:\n%s", src);

        if (type == GL_VERTEX_SHADER)
        {
            CCLOG("cocos2d: %s", getVertexShaderLog().c_str());
        }
        else
        {
            CCLOG("cocos2d: %s", getFragmentShaderLog().c_str());
        }
        delete[] src;

        return false;
    }

    return (status == GL_TRUE);
}

GLint GLProgram::getAttribLocation(const std::string &attributeName) const
{
    return glGetAttribLocation(_program, attributeName.c_str());
}

GLint GLProgram::getUniformLocation(const std::string &attributeName) const
{
    return glGetUniformLocation(_program, attributeName.c_str());
}

void GLProgram::bindAttribLocation(const std::string &attributeName, GLuint index) const
{
    glBindAttribLocation(_program, index, attributeName.c_str());
}

void GLProgram::updateUniforms()
{
    _builtInUniforms[UNIFORM_AMBIENT_COLOR] = glGetUniformLocation(_program, UNIFORM_NAME_AMBIENT_COLOR);
    _builtInUniforms[UNIFORM_P_MATRIX] = glGetUniformLocation(_program, UNIFORM_NAME_P_MATRIX);
    _builtInUniforms[UNIFORM_MV_MATRIX] = glGetUniformLocation(_program, UNIFORM_NAME_MV_MATRIX);
    _builtInUniforms[UNIFORM_MVP_MATRIX] = glGetUniformLocation(_program, UNIFORM_NAME_MVP_MATRIX);
    _builtInUniforms[UNIFORM_NORMAL_MATRIX] = glGetUniformLocation(_program, UNIFORM_NAME_NORMAL_MATRIX);

    _builtInUniforms[UNIFORM_TIME] = glGetUniformLocation(_program, UNIFORM_NAME_TIME);
    _builtInUniforms[UNIFORM_SIN_TIME] = glGetUniformLocation(_program, UNIFORM_NAME_SIN_TIME);
    _builtInUniforms[UNIFORM_COS_TIME] = glGetUniformLocation(_program, UNIFORM_NAME_COS_TIME);

    _builtInUniforms[UNIFORM_RANDOM01] = glGetUniformLocation(_program, UNIFORM_NAME_RANDOM01);

    _builtInUniforms[UNIFORM_SAMPLER0] = glGetUniformLocation(_program, UNIFORM_NAME_SAMPLER0);
    _builtInUniforms[UNIFORM_SAMPLER1] = glGetUniformLocation(_program, UNIFORM_NAME_SAMPLER1);
    _builtInUniforms[UNIFORM_SAMPLER2] = glGetUniformLocation(_program, UNIFORM_NAME_SAMPLER2);
    _builtInUniforms[UNIFORM_SAMPLER3] = glGetUniformLocation(_program, UNIFORM_NAME_SAMPLER3);

#ifdef DEBUG_TEXTURE_SIZE
    _builtInUniforms[UNIFORM_DEBUG] = glGetUniformLocation(_program, UNIFORM_NAME_DEBUG);
#endif

    _flags |= _builtInUniforms[UNIFORM_P_MATRIX] != -1 ? UniformFlags::P : UniformFlags::None;
    _flags |= _builtInUniforms[UNIFORM_MV_MATRIX] != -1 ? UniformFlags::MV : UniformFlags::None;
    _flags |= _builtInUniforms[UNIFORM_MVP_MATRIX] != -1 ? UniformFlags::MVP : UniformFlags::None;
    _flags |= _builtInUniforms[UNIFORM_NORMAL_MATRIX] != -1 ? UniformFlags::Normal : UniformFlags::None;
    _flags |= (
                       _builtInUniforms[UNIFORM_TIME] != -1 ||
                       _builtInUniforms[UNIFORM_SIN_TIME] != -1 ||
                       _builtInUniforms[UNIFORM_COS_TIME] != -1
                       ) ? UniformFlags::Time : UniformFlags::None;
    _flags |= _builtInUniforms[UNIFORM_RANDOM01] != -1 ? UniformFlags::Random : UniformFlags::None;

    this->use();

    // Since sample most probably won't change, set it to 0,1,2,3 now.
    if(_builtInUniforms[UNIFORM_SAMPLER0] != -1)
       setUniformLocationWith1i(_builtInUniforms[UNIFORM_SAMPLER0], 0);
    if(_builtInUniforms[UNIFORM_SAMPLER1] != -1)
        setUniformLocationWith1i(_builtInUniforms[UNIFORM_SAMPLER1], 1);
    if(_builtInUniforms[UNIFORM_SAMPLER2] != -1)
        setUniformLocationWith1i(_builtInUniforms[UNIFORM_SAMPLER2], 2);
    if(_builtInUniforms[UNIFORM_SAMPLER3] != -1)
        setUniformLocationWith1i(_builtInUniforms[UNIFORM_SAMPLER3], 3);


#ifdef DEBUG_TEXTURE_SIZE
    if (_builtInUniforms[UNIFORM_DEBUG] != -1)
    {
        _flags |= UniformFlags::Debug;
        setUniformLocationWith1i(_builtInUniforms[UNIFORM_DEBUG], static_cast<GLint>(_debug));
    }
#endif
}

bool GLProgram::link()
{
    CCASSERT(_program != 0, "Cannot link invalid program");

    GLint status = GL_TRUE;

    bindPredefinedVertexAttribs();

    glLinkProgram(_program);

    // Calling glGetProgramiv(...GL_LINK_STATUS...) will force linking of the program at this moment.
    // Otherwise, they might be linked when they are used for the first time. (I guess this depends on the driver implementation)
    // So it might slow down the "booting" process on certain devices. But, on the other hand it is important to know if the shader
    // linked successfully. Some shaders might be downloaded in runtime so, release version should have this check.
    // For more info, see Github issue #16231
    glGetProgramiv(_program, GL_LINK_STATUS, &status);

    if (status == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &maxLength);
        if (maxLength > 1)
        {
            char* buf = new char[maxLength];
            glGetProgramInfoLog(_program, maxLength, &maxLength, buf);
            CCLOG("cocos2d: ERROR: Failed to link program: %i - %s", _program, buf);
            delete[] buf;
        }
        GL::deleteProgram(_program);
        _program = 0;
    }
    else
    {
        parseVertexAttribs();
        parseUniforms();

        clearShader();
    }

    return (status == GL_TRUE);
}

void GLProgram::use()
{
    GL::useProgram(_program);
}

static std::string logForOpenGLShader(GLuint shader)
{
    GLint logLength = 0;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength < 1)
        return "";

    char* logBytes = new char[logLength];
    glGetShaderInfoLog(shader, logLength, nullptr, logBytes);
    std::string const ret(logBytes);
    delete[] logBytes;
    return ret;
}

static std::string logForOpenGLProgram(GLuint program)
{
    GLint logLength = 0;

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength < 1)
        return "";

    char* logBytes = new char[logLength];
    glGetProgramInfoLog(program, logLength, nullptr, logBytes);
    std::string const ret(logBytes);
    delete[] logBytes;
    return ret;
}

std::string GLProgram::getVertexShaderLog() const
{
    return cocos2d::logForOpenGLShader(_vertShader);
}

std::string GLProgram::getFragmentShaderLog() const
{
    return cocos2d::logForOpenGLShader(_fragShader);
}

std::string GLProgram::getProgramLog() const
{
    return logForOpenGLProgram(_program);
}

#ifdef DEBUG_TEXTURE_SIZE
void GLProgram::onContextRecovered()
{
    GLProgram::_mipmapId = 0;
    createMipmapDebug();
}

void GLProgram::createMipmapDebug()
{
    if (GLProgram::_mipmapId == 0)
    {
        glGenTextures(1, &GLProgram::_mipmapId);
        GL::bindTexture2D(GLProgram::_mipmapId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        static constexpr int SIZE = 32;
        static constexpr std::array<std::uint32_t, 6> COLORS = {
            0x66ff0000
            , 0x33ff7f00
            , 0x00ffffff
            , 0x3300b2ff
            , 0x66004cff
            , 0xcc0000ff
        };

        int levelSize = SIZE;
        for (int i = 0; i < 6; ++i)
        {
            std::vector<std::uint32_t> const data(levelSize*levelSize, COLORS[i]);
            glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, levelSize, levelSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
            levelSize /= 2;
        }

        GL::bindTexture2D(static_cast<GLuint>(0));
    }
}
#endif

// Uniform cache

bool GLProgram::updateUniformLocation(GLint location, GLvoid const* data, unsigned int bytes)
{
    if (location < 0)
    {
        return false;
    }

    auto element = _hashForUniforms.find(location);
    if (element == _hashForUniforms.end())
    {
        std::vector<std::uint8_t> const local_data(static_cast<std::uint8_t const*>(data), static_cast<std::uint8_t const*>(data)+bytes);
        _hashForUniforms.emplace(location, local_data);
    }
    else
    {
        if (element->second.size() != bytes || std::memcmp(element->second.data(), data, bytes) != 0)
        {
            element->second.assign(static_cast<std::uint8_t const*>(data), static_cast<std::uint8_t const*>(data)+bytes);
        }
        else
        {
            return false;
        }
    }

    return true;
}

GLint GLProgram::getUniformLocationForName(const char* name) const
{
    CCASSERT(name != nullptr, "Invalid uniform name" );
    CCASSERT(_program != 0, "Invalid operation. Cannot get uniform location when program is not initialized");

    return glGetUniformLocation(_program, name);
}

void GLProgram::setUniformLocationWith1i(GLint location, GLint i1)
{
    std::array<GLint, 1> const ints = { i1 };
    bool const updated = updateUniformLocation(location, ints.data(), ints.size()*sizeof(GLint));

    if (updated)
    {
        glUniform1i(location, i1);
    }
}

void GLProgram::setUniformLocationWith2i(GLint location, GLint i1, GLint i2)
{
    std::array<GLint, 2> const ints = { i1, i2 };
    bool const updated = updateUniformLocation(location, ints.data(), ints.size()*sizeof(GLint));

    if (updated)
    {
        glUniform2i(location, i1, i2);
    }
}

void GLProgram::setUniformLocationWith3i(GLint location, GLint i1, GLint i2, GLint i3)
{
    std::array<GLint, 3> const ints = { i1, i2, i3 };
    bool const updated = updateUniformLocation(location, ints.data(), ints.size()*sizeof(GLint));

    if (updated)
    {
        glUniform3i(location, i1, i2, i3);
    }
}

void GLProgram::setUniformLocationWith4i(GLint location, GLint i1, GLint i2, GLint i3, GLint i4)
{
    std::array<GLint, 4> const ints = { i1, i2, i3, i4 };
    bool const updated = updateUniformLocation(location, ints.data(), ints.size()*sizeof(GLint));

    if (updated)
    {
        glUniform4i(location, i1, i2, i3, i4);
    }
}

void GLProgram::setUniformLocationWith2iv(GLint location, GLint const* ints, unsigned int numberOfArrays)
{
    bool const updated = updateUniformLocation(location, ints, sizeof(GLint)*2*numberOfArrays);

    if (updated)
    {
        glUniform2iv(location, static_cast<GLsizei>(numberOfArrays), ints);
    }
}

void GLProgram::setUniformLocationWith3iv(GLint location, GLint const* ints, unsigned int numberOfArrays)
{
    bool const updated = updateUniformLocation(location, ints, sizeof(GLint)*3*numberOfArrays);

    if (updated)
    {
        glUniform3iv(location, static_cast<GLsizei>(numberOfArrays), ints);
    }
}

void GLProgram::setUniformLocationWith4iv(GLint location, GLint const* ints, unsigned int numberOfArrays)
{
    bool const updated = updateUniformLocation(location, ints, sizeof(GLint)*4*numberOfArrays);

    if (updated)
    {
        glUniform4iv(location, static_cast<GLsizei>(numberOfArrays), ints);
    }
}

void GLProgram::setUniformLocationWith1f(GLint location, GLfloat f1)
{
    std::array<GLfloat, 1> const floats = { f1 };
    bool const updated = updateUniformLocation(location, floats.data(), floats.size()*sizeof(GLfloat));

    if (updated)
    {
        glUniform1f(location, f1);
    }
}

void GLProgram::setUniformLocationWith2f(GLint location, GLfloat f1, GLfloat f2)
{
    std::array<GLfloat, 2> const floats = { f1, f2 };
    bool const updated = updateUniformLocation(location, floats.data(), floats.size()*sizeof(GLfloat));

    if (updated)
    {
        glUniform2f(location, f1, f2);
    }
}

void GLProgram::setUniformLocationWith3f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3)
{
    std::array<GLfloat, 3> const floats = { f1, f2, f3 };
    bool const updated = updateUniformLocation(location, floats.data(), floats.size()*sizeof(GLfloat));

    if (updated)
    {
        glUniform3f(location, f1, f2, f3);
    }
}

void GLProgram::setUniformLocationWith4f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4)
{
    std::array<GLfloat, 4> const floats = { f1, f2, f3, f4 };
    bool const updated = updateUniformLocation(location, floats.data(), floats.size()*sizeof(GLfloat));

    if (updated)
    {
        glUniform4f(location, f1, f2, f3, f4);
    }
}


void GLProgram::setUniformLocationWith1fv(GLint location, GLfloat const* floats, unsigned int numberOfArrays )
{
    bool const updated = updateUniformLocation(location, floats, sizeof(GLfloat)*numberOfArrays);

    if (updated)
    {
        glUniform1fv(location, static_cast<GLsizei>(numberOfArrays), floats);
    }
}

void GLProgram::setUniformLocationWith2fv(GLint location, GLfloat const* floats, unsigned int numberOfArrays)
{
    bool const updated = updateUniformLocation(location, floats, sizeof(GLfloat)*2*numberOfArrays);

    if (updated)
    {
        glUniform2fv(location, static_cast<GLsizei>(numberOfArrays), floats);
    }
}

void GLProgram::setUniformLocationWith3fv(GLint location, GLfloat const* floats, unsigned int numberOfArrays)
{
    bool const updated = updateUniformLocation(location, floats, sizeof(GLfloat)*3*numberOfArrays);

    if (updated)
    {
        glUniform3fv(location, static_cast<GLsizei>(numberOfArrays), floats);
    }
}

void GLProgram::setUniformLocationWith4fv(GLint location, GLfloat const* floats, unsigned int numberOfArrays)
{
    bool const updated = updateUniformLocation(location, floats, sizeof(GLfloat)*4*numberOfArrays);

    if (updated)
    {
        glUniform4fv(location, static_cast<GLsizei>(numberOfArrays), floats);
    }
}

void GLProgram::setUniformLocationWithMatrix2fv(GLint location, GLfloat const* matrixArray, unsigned int numberOfMatrices) {
    bool const updated = updateUniformLocation(location, matrixArray, sizeof(GLfloat)*4*numberOfMatrices);

    if (updated)
    {
        glUniformMatrix2fv(location, static_cast<GLsizei>(numberOfMatrices), GL_FALSE, matrixArray);
    }
}

void GLProgram::setUniformLocationWithMatrix3fv(GLint location, GLfloat const* matrixArray, unsigned int numberOfMatrices) {
    bool const updated = updateUniformLocation(location, matrixArray, sizeof(GLfloat)*9*numberOfMatrices);

    if (updated)
    {
        glUniformMatrix3fv(location, static_cast<GLsizei>(numberOfMatrices), GL_FALSE, matrixArray);
    }
}


void GLProgram::setUniformLocationWithMatrix4fv(GLint location, GLfloat const* matrixArray, unsigned int numberOfMatrices)
{
    bool const updated = updateUniformLocation(location, matrixArray, sizeof(GLfloat)*16*numberOfMatrices);

    if (updated)
    {
        glUniformMatrix4fv(location, static_cast<GLsizei>(numberOfMatrices), GL_FALSE, matrixArray);
    }
}

void GLProgram::setUniformsForBuiltins()
{
    setUniformsForBuiltins(_director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW));
}

void GLProgram::setUniformsForBuiltins(const Mat4 &matrixMV)
{
    const auto& matrixP = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

    if ((_flags & UniformFlags::P) == UniformFlags::P)
        setUniformLocationWithMatrix4fv(_builtInUniforms[UNIFORM_P_MATRIX], matrixP.m, 1);

    if ((_flags & UniformFlags::MV) == UniformFlags::MV)
        setUniformLocationWithMatrix4fv(_builtInUniforms[UNIFORM_MV_MATRIX], matrixMV.m, 1);

    if ((_flags & UniformFlags::MVP) == UniformFlags::MVP)
    {
        Mat4 matrixMVP = matrixP * matrixMV;
        setUniformLocationWithMatrix4fv(_builtInUniforms[UNIFORM_MVP_MATRIX], matrixMVP.m, 1);
    }

    if ((_flags & UniformFlags::Normal) == UniformFlags::Normal)
    {
        Mat4 mvInverse = matrixMV;
        mvInverse.m[12] = mvInverse.m[13] = mvInverse.m[14] = 0.0f;
        mvInverse.inverse();
        mvInverse.transpose();
        GLfloat normalMat[9];
        normalMat[0] = mvInverse.m[0];normalMat[1] = mvInverse.m[1];normalMat[2] = mvInverse.m[2];
        normalMat[3] = mvInverse.m[4];normalMat[4] = mvInverse.m[5];normalMat[5] = mvInverse.m[6];
        normalMat[6] = mvInverse.m[8];normalMat[7] = mvInverse.m[9];normalMat[8] = mvInverse.m[10];
        setUniformLocationWithMatrix3fv(_builtInUniforms[UNIFORM_NORMAL_MATRIX], normalMat, 1);
    }

    if ((_flags & UniformFlags::Time) == UniformFlags::Time)
    {
        // This doesn't give the most accurate global time value.
        // Cocos2D doesn't store a high precision time value, so this will have to do.
        // Getting Mach time per frame per shader using time could be extremely expensive.
        float time = _director->getTotalFrames() * _director->getAnimationInterval();

        setUniformLocationWith4f(_builtInUniforms[GLProgram::UNIFORM_TIME], time/10.0f, time, time*2, time*4);
        setUniformLocationWith4f(_builtInUniforms[GLProgram::UNIFORM_SIN_TIME], time/8.0f, time/4.0f, time/2.0f, sinf(time));
        setUniformLocationWith4f(_builtInUniforms[GLProgram::UNIFORM_COS_TIME], time/8.0f, time/4.0f, time/2.0f, cosf(time));
    }

    if ((_flags & UniformFlags::Random) == UniformFlags::Random)
        setUniformLocationWith4f(_builtInUniforms[GLProgram::UNIFORM_RANDOM01], CCRANDOM_0_1(), CCRANDOM_0_1(), CCRANDOM_0_1(), CCRANDOM_0_1());

#ifdef DEBUG_TEXTURE_SIZE
    if ((_flags & UniformFlags::Debug) == UniformFlags::Debug)
    {
        setUniformLocationWith1i(_builtInUniforms[GLProgram::UNIFORM_DEBUG], static_cast<GLint>(_debug));
        GL::bindTexture2DN(3, GLProgram::_mipmapId);
    }
#endif
}

void GLProgram::reset()
{
    _vertShader = _fragShader = 0;
    _builtInUniforms.fill(-1);


    // it is already deallocated by android when context is lost
    //GL::deleteProgram(_program);
    _program = 0;

    _hashForUniforms.clear();

    _flags = UniformFlags::None;
}

inline void GLProgram::clearShader()
{
    if (_vertShader)
    {
        glDeleteShader(_vertShader);
    }

    if (_fragShader)
    {
        glDeleteShader(_fragShader);
    }

    _vertShader = _fragShader = 0;
}

NS_CC_END
