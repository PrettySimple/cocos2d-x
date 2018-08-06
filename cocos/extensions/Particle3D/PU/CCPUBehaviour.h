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

#ifndef CC_EXTENSIONS_PARTICLE3D_PU_BEHAVIOUR_H
#define CC_EXTENSIONS_PARTICLE3D_PU_BEHAVIOUR_H

#include <cocos/base/CCRef.h>
#include <cocos/extensions/Particle3D/PU/CCPUParticleSystem3D.h>
#include <cocos/math/CCMath.h>
#include <string>
#include <vector>

NS_CC_BEGIN

struct PUParticle3D;
class PUParticleSystem3D;

class CC_DLL PUBehaviour : public Ref
{
    friend class PUParticleSystem3D;

public:
    /** Todo
     */
    inline std::string const& getBehaviourType() const noexcept { return _behaviourType; }
    inline void setBehaviourType(std::string const& behaviourType) { _behaviourType = behaviourType; }

    /** Notify that the Behaviour is rescaled.
     */
    inline virtual void notifyRescaled(Vec3 const& scale) { _behaviourScale = scale; }

    inline virtual void prepare() {}
    inline virtual void unPrepare() {}

    inline virtual void updateBehaviour(PUParticle3D* particle, float deltaTime) {}

    /** Perform initialising activities as soon as the particle with which the ParticleBehaviour is
        associated, is emitted.
    */
    inline virtual void initParticleForEmission(PUParticle3D* particle) {}
    /** Perform some action if a particle expires.
     */
    inline virtual void initParticleForExpiration(PUParticle3D* particle, float timeElapsed) {}

    virtual PUBehaviour* clone();
    virtual void copyAttributesTo(PUBehaviour* behaviour);

    CC_CONSTRUCTOR_ACCESS : PUBehaviour();
    ~PUBehaviour() override;

protected:
    PUParticleSystem3D* _particleSystem;

    // Type of behaviour
    std::string _behaviourType;

    /** Although the scale is on a Particle System level, the behaviour can also be scaled.
     */
    Vec3 _behaviourScale;
};

NS_CC_END

#endif // CC_EXTENSIONS_PARTICLE3D_PU_BEHAVIOUR_H
