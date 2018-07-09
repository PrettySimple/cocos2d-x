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

#ifndef CC_DEPRECATED_INTEGER_H
#define CC_DEPRECATED_INTEGER_H
/// @cond DO_NOT_SHOW

#include "base/CCRef.h"
#include "platform/CCPlatformDefine.h"
#include "platform/CCPlatformMacros.h"

NS_CC_BEGIN

class DataVisitor;

/**
 * @addtogroup data_structures
 * @{
 */

class CC_DLL __Integer : public Ref, public Clonable
{
    int _value = 0;

public:
    __Integer() = default;
    __Integer(__Integer const&) = default;
    __Integer& operator=(__Integer const&) = default;
    __Integer(__Integer&&) noexcept = default;
    __Integer& operator=(__Integer&&) noexcept = default;
    ~__Integer() final;

    inline __Integer(int v)
    : _value(v)
    {
    }
    inline int getValue() const noexcept { return _value; }

    static __Integer* create(int v);

    /* override functions */
    void acceptVisitor(DataVisitor& visitor);

    __Integer* clone() const final;
};

// end of data_structure group
/// @}

NS_CC_END

/// @endcond
#endif // CC_DEPRECATED_INTEGER_H
