/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#ifndef __COCOS2D_H__
#define __COCOS2D_H__

// 0x00 HI ME LO
// 00   03 08 00
#define COCOS2D_VERSION 0x00040000

//
// all cocos2d include files
//
#include <cocos/base/ccConfig.h>

// base
#include <cocos/base/CCAsyncTaskPool.h>
#include <cocos/base/CCAutoreleasePool.h>
#include <cocos/base/CCConfiguration.h>
#include <cocos/base/CCConsole.h>
#include <cocos/base/CCData.h>
#include <cocos/base/CCDirector.h>
#include <cocos/base/CCIMEDelegate.h>
#include <cocos/base/CCIMEDispatcher.h>
#include <cocos/base/CCMap.h>
#include <cocos/base/CCNS.h>
#include <cocos/base/CCProfiling.h>
#include <cocos/base/CCProperties.h>
#include <cocos/base/CCRef.h>
#include <cocos/base/CCRefPtr.h>
#include <cocos/base/CCScheduler.h>
#include <cocos/base/CCUserDefault.h>
#include <cocos/base/CCValue.h>
#include <cocos/base/CCVector.h>
#include <cocos/base/ZipUtils.h>
#include <cocos/base/base64.h>
#include <cocos/base/ccConfig.h>
#include <cocos/base/ccMacros.h>
#include <cocos/base/ccTypes.h>
#include <cocos/base/ccUTF8.h>
#include <cocos/base/ccUtils.h>

// EventDispatcher
#include <cocos/base/CCEventAcceleration.h>
#include <cocos/base/CCEventCustom.h>
#include <cocos/base/CCEventDispatcher.h>
#include <cocos/base/CCEventFocus.h>
#include <cocos/base/CCEventKeyboard.h>
#include <cocos/base/CCEventListenerAcceleration.h>
#include <cocos/base/CCEventListenerCustom.h>
#include <cocos/base/CCEventListenerFocus.h>
#include <cocos/base/CCEventListenerKeyboard.h>
#include <cocos/base/CCEventListenerMouse.h>
#include <cocos/base/CCEventListenerController.h>
#include <cocos/base/CCEventListenerTouch.h>
#include <cocos/base/CCEventMouse.h>
#include <cocos/base/CCEventController.h>
#include <cocos/base/CCController.h>
#include <cocos/base/CCEventTouch.h>
#include <cocos/base/CCEventType.h>

// math
#include <cocos/math/CCAffineTransform.h>
#include <cocos/math/CCGeometry.h>
#include <cocos/math/CCVertex.h>
#include <cocos/math/Mat4.h>
#include <cocos/math/MathUtil.h>
#include <cocos/math/Quaternion.h>
#include <cocos/math/Vec2.h>
#include <cocos/math/Vec3.h>
#include <cocos/math/Vec4.h>

// actions
#include <cocos/2d/CCAction.h>
#include <cocos/2d/CCActionCamera.h>
#include <cocos/2d/CCActionCatmullRom.h>
#include <cocos/2d/CCActionEase.h>
#include <cocos/2d/CCActionGrid.h>
#include <cocos/2d/CCActionGrid3D.h>
#include <cocos/2d/CCActionInstant.h>
#include <cocos/2d/CCActionInterval.h>
#include <cocos/2d/CCActionManager.h>
#include <cocos/2d/CCActionPageTurn3D.h>
#include <cocos/2d/CCActionProgressTimer.h>
#include <cocos/2d/CCActionTiledGrid.h>
#include <cocos/2d/CCActionTween.h>
#include <cocos/2d/CCTweenFunction.h>

// 2d nodes
#include <cocos/2d/CCAtlasNode.h>
#include <cocos/2d/CCClippingNode.h>
#include <cocos/2d/CCClippingRectangleNode.h>
#include <cocos/2d/CCDrawNode.h>
#include <cocos/2d/CCFontFNT.h>
#include <cocos/2d/CCLabel.h>
#include <cocos/2d/CCLabelAtlas.h>
#include <cocos/2d/CCLayer.h>
#include <cocos/2d/CCMenu.h>
#include <cocos/2d/CCMenuItem.h>
#include <cocos/2d/CCMotionStreak.h>
#include <cocos/2d/CCNode.h>
#include <cocos/2d/CCNodeGrid.h>
#include <cocos/2d/CCParticleBatchNode.h>
#include <cocos/2d/CCParticleExamples.h>
#include <cocos/2d/CCParticleSystem.h>
#include <cocos/2d/CCParticleSystemQuad.h>
#include <cocos/2d/CCProgressTimer.h>
#include <cocos/2d/CCProtectedNode.h>
#include <cocos/2d/CCRenderTexture.h>
#include <cocos/2d/CCScene.h>
#include <cocos/2d/CCTransition.h>
#include <cocos/2d/CCTransitionPageTurn.h>
#include <cocos/2d/CCTransitionProgress.h>

// 2d utils
#include <cocos/2d/CCCamera.h>
#include <cocos/2d/CCCameraBackgroundBrush.h>
#include <cocos/2d/CCGrid.h>
#include <cocos/2d/CCLight.h>

// include
#include <cocos/base/CCProtocols.h>

// renderer
#include <cocos/renderer/CCCallbackCommand.h>
#include <cocos/renderer/CCCustomCommand.h>
#include <cocos/renderer/CCGroupCommand.h>
#include <cocos/renderer/CCMaterial.h>
#include <cocos/renderer/CCPass.h>
#include <cocos/renderer/CCQuadCommand.h>
#include <cocos/renderer/CCRenderCommand.h>
#include <cocos/renderer/CCRenderCommandPool.h>
#include <cocos/renderer/CCRenderState.h>
#include <cocos/renderer/CCRenderer.h>
#include <cocos/renderer/CCTechnique.h>
#include <cocos/renderer/CCTexture2D.h>
#include <cocos/renderer/CCTextureCube.h>
#include <cocos/renderer/CCTextureCache.h>
#include <cocos/renderer/CCTrianglesCommand.h>
#include <cocos/renderer/ccShaders.h>

// physics
#include "physics/CCPhysicsBody.h"
#include "physics/CCPhysicsContact.h"
#include "physics/CCPhysicsJoint.h"
#include "physics/CCPhysicsShape.h"
#include "physics/CCPhysicsWorld.h"

// platform
#include <cocos/platform/CCCommon.h>
#include <cocos/platform/CCDevice.h>
#include <cocos/platform/CCFileUtils.h>
#include <cocos/platform/CCImage.h>
#include <cocos/platform/CCPlatformConfig.h>
#include <cocos/platform/CCPlatformMacros.h>
#include <cocos/platform/CCSAXParser.h>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    #include <cocos/platform/ios/CCApplication-ios.h>
    #include <cocos/platform/ios/CCGLViewImpl-ios.h>
    #include <cocos/platform/ios/CCGL-ios.h>
    #include <cocos/platform/ios/CCStdC-ios.h>
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_IOS

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    #include <cocos/platform/android/CCApplication-android.h>
    #include <cocos/platform/android/CCGLViewImpl-android.h>
    #include <cocos/platform/android/CCGL-android.h>
    #include <cocos/platform/android/CCStdC-android.h>
//Enhance modification begin
    #include <cocos/platform/android/CCEnhanceAPI-android.h>
//Enhance modification end
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    #include <cocos/platform/win32/CCApplication-win32.h>
    #include <cocos/platform/desktop/CCGLViewImpl-desktop.h>
    #include <cocos/platform/win32/CCGL-win32.h>
    #include <cocos/platform/win32/CCStdC-win32.h>
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    #include <cocos/platform/desktop/CCGLViewImpl-desktop.h>
    #include <cocos/platform/mac/CCApplication-mac.h>
    #include <cocos/platform/mac/CCGL-mac.h>
    #include <cocos/platform/mac/CCStdC-mac.h>
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_MAC

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    #include <cocos/platform/linux/CCApplication-linux.h>
    #include <cocos/platform/desktop/CCGLViewImpl-desktop.h>
    #include <cocos/platform/linux/CCGL-linux.h>
    #include <cocos/platform/linux/CCStdC-linux.h>
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_LINUX

// script_support
#include <cocos/base/CCScriptSupport.h>

// sprite_nodes
#include <cocos/2d/CCAnimation.h>
#include <cocos/2d/CCAnimationCache.h>
#include <cocos/2d/CCSprite.h>
#include <cocos/2d/CCAutoPolygon.h>
#include <cocos/2d/CCSpriteBatchNode.h>
#include <cocos/2d/CCSpriteFrame.h>
#include <cocos/2d/CCSpriteFrameCache.h>

// text_input_node
#include <cocos/2d/CCTextFieldTTF.h>

// textures
#include <cocos/renderer/CCTextureAtlas.h>

// tilemap_parallax_nodes
#include <cocos/2d/CCParallaxNode.h>
#include <cocos/2d/CCTMXLayer.h>
#include <cocos/2d/CCTMXObjectGroup.h>
#include <cocos/2d/CCTMXTiledMap.h>
#include <cocos/2d/CCTMXXMLParser.h>
#include <cocos/2d/CCTileMapAtlas.h>
#include <cocos/2d/CCFastTMXLayer.h>
#include <cocos/2d/CCFastTMXTiledMap.h>

// component
#include <cocos/2d/CCComponent.h>
#include <cocos/2d/CCComponentContainer.h>

//3d
#include <cocos/3d/CCAABB.h>
#include <cocos/3d/CCAnimate3D.h>
#include <cocos/3d/CCAnimation3D.h>
#include <cocos/3d/CCAttachNode.h>
#include <cocos/3d/CCBillBoard.h>
#include <cocos/3d/CCFrustum.h>
#include <cocos/3d/CCMesh.h>
#include <cocos/3d/CCMeshSkin.h>
#include <cocos/3d/CCMotionStreak3D.h>
#include <cocos/3d/CCMeshVertexIndexData.h>
#include <cocos/3d/CCOBB.h>
#include <cocos/3d/CCPlane.h>
#include <cocos/3d/CCRay.h>
#include <cocos/3d/CCSkeleton3D.h>
#include <cocos/3d/CCSkybox.h>
#include <cocos/3d/CCSprite3D.h>
#include <cocos/3d/CCSprite3DMaterial.h>
#include <cocos/3d/CCTerrain.h>
#include <cocos/3d/CCVertexAttribBinding.h>

NS_CC_BEGIN

CC_DLL const char* cocos2dVersion();

NS_CC_END

#endif // __COCOS2D_H__
