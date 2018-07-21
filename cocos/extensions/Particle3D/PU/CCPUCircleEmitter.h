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

#ifndef CC_EXTENSIONS_PARTICLE3D_PU_CIRCLEEMITTER_H
#define CC_EXTENSIONS_PARTICLE3D_PU_CIRCLEEMITTER_H

#include "extensions/Particle3D/PU/CCPUEmitter.h"

NS_CC_BEGIN

class CC_DLL PUCircleEmitter : public PUEmitter
{
public:
    // Constants
    static const float DEFAULT_RADIUS;
    static const float DEFAULT_STEP;
    static const float DEFAULT_ANGLE;
    static const bool DEFAULT_RANDOM;
    static const Vec3 DEFAULT_NORMAL;

    static PUCircleEmitter* create();

    /**
     */
    float getRadius() const;
    void setRadius(const float radius);

    /**
     */
    float getCircleAngle() const;
    void setCircleAngle(const float circleAngle);

    /**
     */
    float getStep() const;
    void setStep(const float step);

    /**
     */
    bool isRandom() const;
    void setRandom(const bool random);

    /*
     */
    const Quaternion& getOrientation() const;
    const Vec3& getNormal() const;
    void setNormal(const Vec3& normal);

    /** See ParticleEmiter
     */
    void notifyStart() override;

    PUCircleEmitter* clone() override;
    void copyAttributesTo(PUEmitter* emitter) override;

    CC_CONSTRUCTOR_ACCESS : PUCircleEmitter();
    ~PUCircleEmitter() override = default;

protected:
    /** Determine a particle position on the circle.
     */
    void initParticlePosition(PUParticle3D* particle) override;

    /** Determine the particle direction.
     */
    void initParticleDirection(PUParticle3D* particle) override;

    Quaternion getRotationTo(const Vec3& src, const Vec3& dest, const Vec3& fallbackAxis = Vec3::ZERO) const;

protected:
    float _radius;
    float _circleAngle;
    float _originalCircleAngle;
    float _step;
    float _x;
    float _z;
    bool _random;
    Quaternion _orientation;
    Vec3 _normal;
};
NS_CC_END

#endif // CC_EXTENSIONS_PARTICLE3D_PU_CIRCLEEMITTER_H
