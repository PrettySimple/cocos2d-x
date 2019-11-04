/****************************************************************************
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2012 		cocos2d-x.org
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
#include <cocos/renderer/ccShaders.h>

NS_CC_BEGIN
//

//backend shader file
#include <cocos/renderer/shaders/positionColor.vert>
#include <cocos/renderer/shaders/positionColor.frag>
#include <cocos/renderer/shaders/positionTexture.vert>
#include <cocos/renderer/shaders/positionTexture.frag>
#include <cocos/renderer/shaders/positionTextureColor.vert>
#include <cocos/renderer/shaders/positionTextureColor.frag>
#include <cocos/renderer/shaders/positionTextureColorAlphaTest.frag>
#include <cocos/renderer/shaders/label_normal.frag>
#include <cocos/renderer/shaders/label_distanceNormal.frag>
#include <cocos/renderer/shaders/label_outline.frag>
#include <cocos/renderer/shaders/label_distanceFieldGlow.frag>
#include <cocos/renderer/shaders/positionColorLengthTexture.vert>
#include <cocos/renderer/shaders/positionColorLengthTexture.frag>
#include <cocos/renderer/shaders/positionColorTextureAsPointsize.vert>
#include <cocos/renderer/shaders/position.vert>
#include <cocos/renderer/shaders/positionNoMVP.vert>
#include <cocos/renderer/shaders/layer_radialGradient.frag>
#include <cocos/renderer/shaders/ui_Gray.frag>
#include <cocos/renderer/shaders/positionTextureUColor.vert>
#include <cocos/renderer/shaders/positionTextureUColor.frag>
#include <cocos/renderer/shaders/positionUColor.vert>
#include <cocos/renderer/shaders/positionUColor.frag>
#include <cocos/renderer/shaders/etc1.frag>
#include <cocos/renderer/shaders/etc1_Gray.frag>
#include <cocos/renderer/shaders/cameraClear.vert>
#include <cocos/renderer/shaders/cameraClear.frag>


#include <cocos/renderer/shaders/3D_color.frag>
#include <cocos/renderer/shaders/3D_colorNormal.frag>
#include <cocos/renderer/shaders/3D_colorNormalTexture.frag>
#include <cocos/renderer/shaders/3D_colorTexture.frag>
#include <cocos/renderer/shaders/3D_particle.vert>
#include <cocos/renderer/shaders/3D_particle.frag>
#include <cocos/renderer/shaders/3D_positionNormalTexture.vert>
#include <cocos/renderer/shaders/3D_positionTexture.vert>
#include <cocos/renderer/shaders/3D_skybox.vert>
#include <cocos/renderer/shaders/3D_skybox.frag>
#include <cocos/renderer/shaders/3D_terrain.frag>
#include <cocos/renderer/shaders/3D_terrain.vert>

#include <cocos/renderer/shaders/lineColor.frag>
#include <cocos/renderer/shaders/lineColor.vert>

NS_CC_END
