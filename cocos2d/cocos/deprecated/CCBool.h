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

#pragma once

/// @cond DO_NOT_SHOW

#include <cocos/base/CCRef.h>
#include <cocos/platform/CCPlatformDefine.h>
#include <cocos/platform/CCPlatformMacros.h>

NS_CC_BEGIN

/**
 * @addtogroup data_structures
 * @{
 */

class DataVisitor;

class CC_DLL __Bool final : public Ref, public Clonable
{
    bool _value = false;

public:
    __Bool() = default;
    __Bool(__Bool const&) = default;
    __Bool& operator=(__Bool const&) = default;
    __Bool(__Bool&&) noexcept = default;
    __Bool& operator=(__Bool&&) noexcept = default;
    ~__Bool() final;

    inline __Bool(bool v)
    : _value(v)
    {
    }
    inline bool getValue() const noexcept { return _value; }

    static __Bool* create(bool v);

    /* override functions */
    void acceptVisitor(DataVisitor& visitor);

    __Bool* clone() const final;
};

// end of data_structure group
/// @}

NS_CC_END

/// @endcond