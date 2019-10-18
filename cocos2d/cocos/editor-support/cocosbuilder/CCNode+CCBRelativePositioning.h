#ifndef CC_EDITORSUPPORT_COCOSBUILDER_NODERELATIVEPOSITIONING_H
#define CC_EDITORSUPPORT_COCOSBUILDER_NODERELATIVEPOSITIONING_H

#include "CCBReader.h"

namespace cocosbuilder
{
    extern CC_DLL cocos2d::Vec2
    getAbsolutePosition(const cocos2d::Vec2& pt, CCBReader::PositionType type, const cocos2d::Size& containerSize, const std::string& propName);

    extern CC_DLL void setRelativeScale(cocos2d::Node* node, float scaleX, float scaleY, CCBReader::ScaleType type, const std::string& propName);

} // namespace cocosbuilder

#endif // CC_EDITORSUPPORT_COCOSBUILDER_NODERELATIVEPOSITIONING_H
