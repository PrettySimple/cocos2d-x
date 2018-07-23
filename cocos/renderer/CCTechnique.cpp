/****************************************************************************
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

 Ideas taken from:
 - GamePlay3D: http://gameplay3d.org/
 - OGRE3D: http://www.ogre3d.org/
 - Qt3D: http://qt-project.org/
 ****************************************************************************/

#include <cocos/renderer/CCTechnique.h>

#include <cocos/renderer/CCMaterial.h>
#include <cocos/renderer/CCPass.h>

#include <new>

NS_CC_BEGIN

Technique* Technique::createWithGLProgramState(Material* parent, GLProgramState* state)
{
    auto technique = new (std::nothrow) Technique();
    if (technique && technique->init(parent))
    {
        auto pass = Pass::createWithGLProgramState(technique, state);
        technique->addPass(pass);

        technique->autorelease();
        return technique;
    }
    return nullptr;
}

Technique* Technique::create(Material* material)
{
    auto technique = new (std::nothrow) Technique();
    if (technique && technique->init(material))
    {
        technique->autorelease();
        return technique;
    }
    return nullptr;
}

Technique::~Technique()
{
}

bool Technique::init(Material* parent)
{
    _parent = parent;
    return true;
}

Technique* Technique::clone() const
{
    auto technique = new (std::nothrow) Technique();

    if (technique)
    {
        technique->_name = _name;
        RenderState::cloneInto(technique);

        for (const auto pass : _passes)
        {
            auto p = pass->clone();
            p->_parent = technique;
            technique->_passes.pushBack(p);
        }

        technique->autorelease();
    }
    return technique;
}

void Technique::addPass(Pass* pass)
{
    _passes.pushBack(pass);
}

Pass* Technique::getPassByIndex(std::size_t index) const
{
    CC_ASSERT(index >= 0 && index < _passes.size() && "Invalid index");
    return _passes.at(index);
}

NS_CC_END
