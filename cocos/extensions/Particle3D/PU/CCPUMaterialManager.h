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

#ifndef CC_EXTENSIONS_PARTICLE3D_PU_MATERIALMANAGER_H
#define CC_EXTENSIONS_PARTICLE3D_PU_MATERIALMANAGER_H

#include <cocos/base/CCRef.h>
#include <cocos/base/ccTypes.h>
#include <cocos/math/Vec4.h>
#include <cocos/platform/CCGL.h>

#include <string>
#include <vector>

NS_CC_BEGIN

struct CC_DLL PUMaterial : public Ref
{
    std::string fileName;
    std::string name;
    bool isEnabledLight = true;
    Vec4 ambientColor = Vec4::ONE;
    Vec4 diffuseColor = Vec4::ONE;
    Vec4 specularColor = Vec4::ZERO;
    Vec4 emissiveColor = Vec4::ZERO;
    float shininess = 0.0f;

    BlendFunc blendFunc = {GL_ONE, GL_ZERO};
    bool depthTest = true;
    bool depthWrite = true;

    std::string textureFile;
    GLuint wrapMode = GL_CLAMP_TO_EDGE;
};

class CC_DLL PUMaterialCache
{
public:
    PUMaterialCache();
    ~PUMaterialCache();

    static PUMaterialCache* Instance();

    bool loadMaterials(const std::string& file);
    bool loadMaterialsFromSearchPaths(const std::string& fileFolder);
    PUMaterial* getMaterial(const std::string& name);
    void addMaterial(PUMaterial* material);

protected:
    using MaterialVector = std::vector<PUMaterial*>;
    MaterialVector _materialMap;
};

NS_CC_END

#endif // CC_EXTENSIONS_PARTICLE3D_PU_MATERIALMANAGER_H
