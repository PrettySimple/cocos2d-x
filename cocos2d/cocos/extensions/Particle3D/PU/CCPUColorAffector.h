/****************************************************************************
 Copyright (C) 2013 Henry van Merode. All rights reserved.
 Copyright (c) 2015 Chukong Technologies Inc.

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

#include <cocos/base/ccTypes.h>
#include <cocos/extensions/Particle3D/PU/CCPUAffector.h>

NS_CC_BEGIN

class CC_DLL PUColorAffector : public PUAffector
{
public:
    typedef std::map<float, Vec4> ColorMap;
    typedef std::map<float, Vec4>::iterator ColorMapIterator;
    enum ColorOperation
    {
        CAO_MULTIPLY,
        CAO_SET
    };

    // Constants
    static const ColorOperation DEFAULT_COLOR_OPERATION;

    static PUColorAffector* create();

    void updatePUAffector(PUParticle3D* particle, float deltaTime) override;

    /**
     */
    void addColor(float timeFraction, const Vec4& color);

    /**
     */
    const ColorMap& getTimeAndColor() const;

    /**
     */
    void clearColorMap();

    /**
     */
    const ColorOperation& getColorOperation() const;

    /**
     */
    void setColorOperation(const ColorOperation& colorOperation);

    void copyAttributesTo(PUAffector* affector) override;

    CC_CONSTRUCTOR_ACCESS : PUColorAffector();
    ~PUColorAffector() override;

protected:
    /**
     */
    inline ColorMapIterator findNearestColorMapIterator(float timeFraction);

protected:
    ColorMap _colorMap;
    ColorOperation _colorOperation;
};
NS_CC_END
