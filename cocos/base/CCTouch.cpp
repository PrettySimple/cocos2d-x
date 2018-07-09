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

#include "base/CCTouch.h"

#include "base/CCDirector.h"

NS_CC_BEGIN

Touch::~Touch()
{
}

// returns the current touch location in OpenGL coordinates
Vec2 Touch::getLocation() const
{
    return Director::getInstance()->convertToGL(_point);
}

// returns the previous touch location in OpenGL coordinates
Vec2 Touch::getPreviousLocation() const
{
    return Director::getInstance()->convertToGL(_prevPoint);
}

// returns the start touch location in OpenGL coordinates
Vec2 Touch::getStartLocation() const
{
    return Director::getInstance()->convertToGL(_startPoint);
}

void Touch::setTouchInfo(int id, float x, float y)
{
    _id = id;
    _prevPoint = _point;
    _point.x = x;
    _point.y = y;
    _curForce = 0.0f;
    _maxForce = 0.0f;
    if (!_startPointCaptured)
    {
        _startPoint = _point;
        _startPointCaptured = true;
        _prevPoint = _point;
    }
}

void Touch::setTouchInfo(int id, float x, float y, float force, float maxForce)
{
    _id = id;
    _prevPoint = _point;
    _point.x = x;
    _point.y = y;
    _curForce = force;
    _maxForce = maxForce;
    if (!_startPointCaptured)
    {
        _startPoint = _point;
        _startPointCaptured = true;
        _prevPoint = _point;
    }
}

// returns the delta position between the current location and the previous location in OpenGL coordinates
Vec2 Touch::getDelta() const
{
    return getLocation() - getPreviousLocation();
}

NS_CC_END
