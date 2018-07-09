/****************************************************************************
Copyright (c) 2009      Lam Pham
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2012      Ricardo Quesada
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

#ifndef CC_2D_TRANSITIONPROGRESS_H
#define CC_2D_TRANSITIONPROGRESS_H

#include "2d/CCTransition.h"

#include <chrono>

NS_CC_BEGIN

class ProgressTimer;
class RenderTexture;

/**
 * @addtogroup _2d
 * @{
 */
/** @class TransitionProgress
 * @brief A base class of progress transition.
 */
class CC_DLL TransitionProgress : public TransitionScene
{
public:
    /** Creates a transition with duration and incoming scene.
     *
     * @param t Duration time, in seconds.
     * @param scene A given scene.
     * @return An autoreleased TransitionProgress object.
     */
    static TransitionProgress* create(std::chrono::milliseconds t, Scene* scene);

    //
    // Overrides
    //
    void onEnter() override;
    void onExit() override;

    CC_CONSTRUCTOR_ACCESS : TransitionProgress();
    ~TransitionProgress() override {}

protected:
    void sceneOrder() override;

protected:
    virtual ProgressTimer* progressTimerNodeWithRenderTexture(RenderTexture* texture);
    virtual void setupTransition();

protected:
    float _to;
    float _from;
    Scene* _sceneToBeModified;
};

/** @class TransitionProgressRadialCCW
 * @brief TransitionRadialCCW transition.
 A counter clock-wise radial transition to the next scene
 */
class CC_DLL TransitionProgressRadialCCW : public TransitionProgress
{
public:
    /** Creates a transition with duration and incoming scene.
     *
     * @param t Duration time, in seconds.
     * @param scene A given scene.
     * @return An autoreleased TransitionProgressRadialCCW object.
     */
    static TransitionProgressRadialCCW* create(std::chrono::milliseconds t, Scene* scene);

    CC_CONSTRUCTOR_ACCESS :
        /**
         * @js ctor
         */
        TransitionProgressRadialCCW()
    {
    }
    ~TransitionProgressRadialCCW() override {}

protected:
    //
    // Overrides
    //
    ProgressTimer* progressTimerNodeWithRenderTexture(RenderTexture* texture) override;
};

/** @class TransitionProgressRadialCW
 * @brief TransitionRadialCW transition.
 A counter clock-wise radial transition to the next scene.
*/
class CC_DLL TransitionProgressRadialCW : public TransitionProgress
{
public:
    /** Creates a transition with duration and incoming scene.
     *
     * @param t Duration time, in seconds.
     * @param scene A given scene.
     * @return An autoreleased TransitionProgressRadialCW object.
     */
    static TransitionProgressRadialCW* create(std::chrono::milliseconds t, Scene* scene);

    CC_CONSTRUCTOR_ACCESS :
        /**
         * @js ctor
         */
        TransitionProgressRadialCW()
    {
    }
    ~TransitionProgressRadialCW() override {}

protected:
    //
    // Overrides
    //
    ProgressTimer* progressTimerNodeWithRenderTexture(RenderTexture* texture) override;
};

/** @class TransitionProgressHorizontal
  * @brief TransitionProgressHorizontal transition.
 A  clock-wise radial transition to the next scene
 */
class CC_DLL TransitionProgressHorizontal : public TransitionProgress
{
public:
    /** Creates a transition with duration and incoming scene.
     *
     * @param t Duration time, in seconds.
     * @param scene A given scene.
     * @return An autoreleased TransitionProgressHorizontal object.
     */
    static TransitionProgressHorizontal* create(std::chrono::milliseconds t, Scene* scene);

    CC_CONSTRUCTOR_ACCESS :
        /**
         * @js ctor
         */
        TransitionProgressHorizontal()
    {
    }
    ~TransitionProgressHorizontal() override {}

protected:
    //
    // Overrides
    //
    ProgressTimer* progressTimerNodeWithRenderTexture(RenderTexture* texture) override;
};

/** @class TransitionProgressVertical
 * @brief TransitionProgressVertical transition.
 */
class CC_DLL TransitionProgressVertical : public TransitionProgress
{
public:
    /** Creates a transition with duration and incoming scene.
     *
     * @param t Duration time, in seconds.
     * @param scene A given scene.
     * @return An autoreleased TransitionProgressVertical object.
     */
    static TransitionProgressVertical* create(std::chrono::milliseconds t, Scene* scene);

    CC_CONSTRUCTOR_ACCESS :
        /**
         * @js ctor
         */
        TransitionProgressVertical()
    {
    }
    ~TransitionProgressVertical() override {}

protected:
    //
    // Overrides
    //
    ProgressTimer* progressTimerNodeWithRenderTexture(RenderTexture* texture) override;
};

/** @class TransitionProgressInOut
 * @brief TransitionProgressInOut transition.
 */
class CC_DLL TransitionProgressInOut : public TransitionProgress
{
public:
    /** Creates a transition with duration and incoming scene.
     *
     * @param t Duration time, in seconds.
     * @param scene A given scene.
     * @return An autoreleased TransitionProgressInOut object.
     */
    static TransitionProgressInOut* create(std::chrono::milliseconds t, Scene* scene);

    CC_CONSTRUCTOR_ACCESS :
        /**
         * @js ctor
         */
        TransitionProgressInOut()
    {
    }
    ~TransitionProgressInOut() override {}

protected:
    //
    // Overrides
    //
    ProgressTimer* progressTimerNodeWithRenderTexture(RenderTexture* texture) override;
    void sceneOrder() override;
    void setupTransition() override;
};

/** @class TransitionProgressOutIn
 * @brief TransitionProgressOutIn transition.
 */
class CC_DLL TransitionProgressOutIn : public TransitionProgress
{
public:
    /** Creates a transition with duration and incoming scene.
     *
     * @param t Duration time, in seconds.
     * @param scene A given scene.
     * @return An autoreleased TransitionProgressOutIn object.
     */
    static TransitionProgressOutIn* create(std::chrono::milliseconds t, Scene* scene);

    CC_CONSTRUCTOR_ACCESS :
        /**
         * @js ctor
         */
        TransitionProgressOutIn()
    {
    }
    ~TransitionProgressOutIn() override {}

protected:
    //
    // Overrides
    //
    ProgressTimer* progressTimerNodeWithRenderTexture(RenderTexture* texture) override;
};

// end of _2d group
/// @}

NS_CC_END

#endif // CC_2D_TRANSITIONPROGRESS_H
