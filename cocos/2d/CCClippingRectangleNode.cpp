
#include <cocos/2d/CCClippingRectangleNode.h>

#include <cocos/base/CCDirector.h>
#include <cocos/math/Vec2.h>
#include <cocos/platform/CCGLView.h>
#include <cocos/renderer/CCRenderer.h>

NS_CC_BEGIN

ClippingRectangleNode* ClippingRectangleNode::create(const Rect& clippingRegion)
{
    ClippingRectangleNode* node = new (std::nothrow) ClippingRectangleNode();
    if (node && node->init())
    {
        node->setClippingRegion(clippingRegion);
        node->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(node);
    }

    return node;
}

ClippingRectangleNode* ClippingRectangleNode::create()
{
    ClippingRectangleNode* node = new (std::nothrow) ClippingRectangleNode();
    if (node && node->init())
    {
        node->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(node);
    }

    return node;
}

void ClippingRectangleNode::setClippingRegion(const Rect& clippingRegion)
{
    _clippingRegion = clippingRegion;
}

void ClippingRectangleNode::onBeforeVisitScissor()
{
    if (_clippingEnabled)
    {
        glEnable(GL_SCISSOR_TEST);

        GLint currentFBO;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBO);
        
        if(currentFBO != 0)
        {
            GLint params;
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &params);
        
            if(params == GL_TEXTURE)
            {
                /* FIX to make compatible ClippingRectangleNode with the Newyork filter system */
            
                // We are rendering in a RT
                Vec4 BL = Vec4(_clippingRegion.origin.x, _clippingRegion.origin.y, 0.0f, 1.0f);
                Vec4 TR = Vec4(_clippingRegion.origin.x + _clippingRegion.size.width, _clippingRegion.origin.y + _clippingRegion.size.height, 0.0f, 1.0f);

                // retrieve current viewport (mapped to the Rt)
                GLint currentVP[4];
                glGetIntegerv(GL_VIEWPORT, currentVP);

                // retrieve current transforms
                Mat4 curXForm;
                curXForm = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW) * Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

                // transform coordinates to clip space
                curXForm.transformVector(BL);
                curXForm.transformVector(TR);
            
                // w divide
                BL.x = ((BL.x / BL.w) + 1.0f) * 0.5f;
                BL.y = ((BL.y / BL.w) + 1.0f) * 0.5f;
                TR.x = ((TR.x / TR.w) + 1.0f) * 0.5f;
                TR.y = ((TR.y / TR.w) + 1.0f) * 0.5f;

                // apply the RT viewport
                BL.x = (BL.x * currentVP[2]) + currentVP[0];
                BL.y = (BL.y * currentVP[3]) + currentVP[1];
                TR.x = (TR.x * currentVP[2]) + currentVP[0];
                TR.y = (TR.y * currentVP[3]) + currentVP[1];

                // clip origin negative coordinates
                BL.x = std::max(0.0f, BL.x);
                BL.y = std::max(0.0f, BL.y);

                // apply scissor
                glScissor((GLint)BL.x, (GLint)BL.y, (GLsizei)(TR.x - BL.x), (GLsizei)(TR.y - BL.y));
            
                return;
            }
        }
        
        // Standard COCOS implementation (FrameBuffer)
        float scaleX = _scaleX;
        float scaleY = _scaleY;
        Node* parent = this->getParent();
        while (parent)
        {
            scaleX *= parent->getScaleX();
            scaleY *= parent->getScaleY();
            parent = parent->getParent();
        }

        const Point pos = convertToWorldSpace(Point(_clippingRegion.origin.x, _clippingRegion.origin.y));
        GLView* glView = Director::getInstance()->getOpenGLView();
        glView->setScissorInPoints(pos.x, pos.y, _clippingRegion.size.width * scaleX, _clippingRegion.size.height * scaleY);
    }
}

void ClippingRectangleNode::onAfterVisitScissor()
{
    if (_clippingEnabled)
    {
        glDisable(GL_SCISSOR_TEST);
    }
}

void ClippingRectangleNode::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    _beforeVisitCmdScissor.init(_globalZOrder);
    _beforeVisitCmdScissor.setFunc([this]() { onBeforeVisitScissor(); });
    renderer->addCommand(&_beforeVisitCmdScissor);

    Node::visit(renderer, parentTransform, parentFlags);

    _afterVisitCmdScissor.init(_globalZOrder);
    _afterVisitCmdScissor.setFunc([this]() { onAfterVisitScissor(); });
    renderer->addCommand(&_afterVisitCmdScissor);
}

NS_CC_END
