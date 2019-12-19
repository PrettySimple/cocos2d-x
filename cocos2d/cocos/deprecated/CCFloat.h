/****************************************************************************
 Copyright (c) 2013-2015 Chukong Technologies

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

/// @cond DO_NOT_SHOW

#include <cocos/base/CCRef.h>
#include <cocos/platform/CCPlatformDefine.h>
#include <cocos/platform/CCPlatformMacros.h>

NS_CC_BEGIN

class DataVisitor;

/**
 * @addtogroup data_structures
 * @{
 */

class CC_DLL __Float : public Ref, public Clonable
{
    float _value = 0.f;

public:
    __Float() = default;
    __Float(__Float const&) = default;
    __Float& operator=(__Float const&) = default;
    __Float(__Float&&) noexcept = default;
    __Float& operator=(__Float&&) noexcept = default;
    ~__Float() final;

    inline __Float(float v)
    : _value(v)
    {
    }
    inline float getValue() const noexcept { return _value; }

    static __Float* create(float v);

    /* override functions */
    void acceptVisitor(DataVisitor& visitor);

    __Float* clone() const final;
};

// end of data_structure group
/// @}

NS_CC_END

/// @endcond