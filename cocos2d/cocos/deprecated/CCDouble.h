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

#ifndef CC_DEPRECATED_DOUBLE_H
#define CC_DEPRECATED_DOUBLE_H
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

class CC_DLL __Double final : public Ref, public Clonable
{
    double _value = 0.0;

public:
    __Double() = default;
    __Double(__Double const&) = default;
    __Double& operator=(__Double const&) = default;
    __Double(__Double&&) noexcept = default;
    __Double& operator=(__Double&&) noexcept = default;
    ~__Double() final;

    inline __Double(double v)
    : _value(v)
    {
    }
    inline double getValue() const noexcept { return _value; }

    static __Double* create(double v);

    /* override functions */
    void acceptVisitor(DataVisitor& visitor);

    __Double* clone() const final;
};

// end of data_structure group
/// @}

NS_CC_END

/// @endcond
#endif // CC_DEPRECATED_DOUBLE_H
