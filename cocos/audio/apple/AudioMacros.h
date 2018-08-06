/****************************************************************************
 Copyright (c) 2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#pragma once

#define QUEUEBUFFER_NUM (3)
#define QUEUEBUFFER_TIME_STEP (0.05f)

#define QUOTEME_(x) #x
#define QUOTEME(x) QUOTEME_(x)

#if defined(COCOS2D_AUDIO_DEBUG) && COCOS2D_AUDIO_DEBUG > 0
#    define ALOGV(...) cocos2d::log2("V/" LOG_TAG " (" QUOTEME(__LINE__) "): ", __VA_ARGS__)
#    define ALOGD(...) cocos2d::log2("D/" LOG_TAG " (" QUOTEME(__LINE__) "): ", __VA_ARGS__)
#    define ALOGI(...) cocos2d::log2("I/" LOG_TAG " (" QUOTEME(__LINE__) "): ", __VA_ARGS__)
#    define ALOGW(...) cocos2d::log2("W/" LOG_TAG " (" QUOTEME(__LINE__) "): ", __VA_ARGS__)
#    define ALOGE(...) cocos2d::log2("E/" LOG_TAG " (" QUOTEME(__LINE__) "): ", __VA_ARGS__)
#else
#    define ALOGV(...) void(0)
#    define ALOGD(...) void(0)
#    define ALOGI(...) void(0)
#    define ALOGW(...) void(0)
#    define ALOGE(...) void(0)
#endif

#if defined(COCOS2D_AUDIO_DEBUG) && COCOS2D_AUDIO_DEBUG > 0
#    define CHECK_AL_ERROR_DEBUG()                                                                     \
        do                                                                                             \
        {                                                                                              \
            GLenum __error = alGetError();                                                             \
            if (__error)                                                                               \
            {                                                                                          \
                ALOGE("OpenAL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
            }                                                                                          \
        } while (false)
#else
#    define CHECK_AL_ERROR_DEBUG()
#endif

#define BREAK_IF(condition) \
    if (!!(condition))      \
    {                       \
        break;              \
    }

#define BREAK_IF_ERR_LOG(condition, ...)                                  \
    if (!!(condition))                                                    \
    {                                                                     \
        ALOGE("(" QUOTEME(condition) ") failed, message: ", __VA_ARGS__); \
        break;                                                            \
    }
