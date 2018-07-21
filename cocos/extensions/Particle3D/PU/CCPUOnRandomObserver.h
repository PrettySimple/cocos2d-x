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

#ifndef CC_EXTENSIONS_PARTICLE3D_PU_ONRANDOMOBSERVER_H
#define CC_EXTENSIONS_PARTICLE3D_PU_ONRANDOMOBSERVER_H

#include "base/CCRef.h"
#include "extensions/Particle3D/PU/CCPUObserver.h"
#include "math/CCMath.h"
#include <string>
#include <vector>

NS_CC_BEGIN

struct PUParticle3D;
class PUParticleSystem3D;
class CC_DLL PUOnRandomObserver : public PUObserver
{
public:
    // Constants
    static const float DEFAULT_THRESHOLD;

    static PUOnRandomObserver* create();

    /** See ParticleObserver::_preProcessParticles()
     */
    void preUpdateObserver(float deltaTime) override;

    /** See ParticleObserver::_processParticle()
     */
    void updateObserver(PUParticle3D* particle, float deltaTime, bool firstParticle) override;

    /**
     */
    bool observe(PUParticle3D* particle, float timeElapsed) override;

    /**
     */
    inline float getThreshold(void) const noexcept { return _threshold; }
    inline void setThreshold(float threshold) noexcept { _threshold = threshold; }

    void copyAttributesTo(PUObserver* observer) override;

    CC_CONSTRUCTOR_ACCESS : PUOnRandomObserver();
    ~PUOnRandomObserver() override = default;

protected:
    float _threshold; // Value between 0..1
};

NS_CC_END

#endif // CC_EXTENSIONS_PARTICLE3D_PU_ONRANDOMOBSERVER_H
