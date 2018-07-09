/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
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

#ifndef CC_2D_ATLASNODE_H
#define CC_2D_ATLASNODE_H

#include "2d/CCNode.h"
#include "base/CCProtocols.h"
#include "base/ccTypes.h"
#include "renderer/CCQuadCommand.h"

NS_CC_BEGIN

/**
 * @addtogroup _2d
 * @{
 */

class TextureAtlas;

/** @brief AtlasNode is a subclass of Node that implements the RGBAProtocol and TextureProtocol protocol.
 * It knows how to render a TextureAtlas object.
 * If you are going to render a TextureAtlas consider subclassing AtlasNode (or a subclass of AtlasNode).
 * All features from Node are valid, plus the following features:
 * - opacity and RGB colors.
 */
class CC_DLL AtlasNode : public Node, public TextureProtocol
{
public:
    /** creates a AtlasNode  with an Atlas file the width and height of each item and the quantity of items to render.
     *
     * @param filename The path of Atlas file.
     * @param tileWidth The width of the item.
     * @param tileHeight The height of the item.
     * @param itemsToRender The quantity of items to render.
     */
    static AtlasNode* create(const std::string& filename, int tileWidth, int tileHeight, int itemsToRender);

    /** updates the Atlas (indexed vertex array).
     * Shall be overridden in subclasses.
     */
    virtual void updateAtlasValues();

    /** Set an buffer manager of the texture vertex. */
    void setTextureAtlas(TextureAtlas* textureAtlas);

    /** Return the buffer manager of the texture vertex.
     *
     * @return Return A TextureAtlas.
     */
    TextureAtlas* getTextureAtlas() const;

    void setQuadsToDraw(std::size_t quadsToDraw);
    std::size_t getQuadsToDraw() const;

    // Overrides
    void draw(Renderer* renderer, const Mat4& transform, uint32_t flags) override;
    Texture2D* getTexture() const override;
    void setTexture(Texture2D* texture) override;
    bool isOpacityModifyRGB() const override;
    void setOpacityModifyRGB(bool isOpacityModifyRGB) override;
    const Color3B& getColor(void) const override;
    void setColor(const Color3B& color) override;
    void setOpacity(GLubyte opacity) override;
    /**
     * @code
     * When this function bound into js or lua,the parameter will be changed
     * In js: var setBlendFunc(var src, var dst)
     * @endcode
     * @lua NA
     */
    void setBlendFunc(const BlendFunc& blendFunc) override;
    /**
     * @lua NA
     */
    const BlendFunc& getBlendFunc() const override;

    CC_CONSTRUCTOR_ACCESS : AtlasNode();
    ~AtlasNode() override;

    /** Initializes an AtlasNode  with an Atlas file the width and height of each item and the quantity of items to render*/
    bool initWithTileFile(const std::string& tile, int tileWidth, int tileHeight, int itemsToRender);

    /** Initializes an AtlasNode  with a texture the width and height of each item measured in points and the quantity of items to render*/
    bool initWithTexture(Texture2D* texture, int tileWidth, int tileHeight, int itemsToRender);

protected:
    void calculateMaxItems();
    void updateBlendFunc();
    void updateOpacityModifyRGB();

    friend class Director;
    void setIgnoreContentScaleFactor(bool bIgnoreContentScaleFactor);

    /** Chars per row. */
    int _itemsPerRow;
    /** Chars per column. */
    int _itemsPerColumn;

    /** Width of each char. */
    int _itemWidth;
    /** Height of each char. */
    int _itemHeight;

    Color3B _colorUnmodified;

    TextureAtlas* _textureAtlas;
    /** Protocol variables. */
    bool _isOpacityModifyRGB;
    BlendFunc _blendFunc;

    /** Quads to draw. */
    std::size_t _quadsToDraw;
    /** Color uniform. */
    GLint _uniformColor;
    /** This variable is only used for LabelAtlas FPS display. So plz don't modify its value. */
    bool _ignoreContentScaleFactor;
    /** Quad command. */
    QuadCommand _quadCommand;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(AtlasNode)
};

// end of base_node group
/// @}

NS_CC_END

#endif // CC_2D_ATLASNODE_H
