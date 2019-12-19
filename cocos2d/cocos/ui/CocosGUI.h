/****************************************************************************
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

#pragma once

#include <cocos/ui/UIButton.h>
#include <cocos/ui/UICheckBox.h>
#include <cocos/ui/UIHBox.h>
#include <cocos/ui/UIHelper.h>
#include <cocos/ui/UIImageView.h>
#include <cocos/ui/UILayout.h>
#include <cocos/ui/UIListView.h>
#include <cocos/ui/UILoadingBar.h>
#include <cocos/ui/UIPageView.h>
#include <cocos/ui/UIRadioButton.h>
#include <cocos/ui/UIRelativeBox.h>
#include <cocos/ui/UIRichText.h>
#include <cocos/ui/UIScrollView.h>
#include <cocos/ui/UISlider.h>
#include <cocos/ui/UIText.h>
#include <cocos/ui/UITextAtlas.h>
#include <cocos/ui/UITextBMFont.h>
#include <cocos/ui/UITextField.h>
#include <cocos/ui/UIVBox.h>
#include <cocos/ui/UIWidget.h>
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN)
#    include <cocos/ui/UIVideoPlayer.h>
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN)
#    include <cocos/ui/UIWebView.h>
#endif
#include <cocos/editor-support/cocostudio/CocosStudioExtension.h>
#include <cocos/ui/GUIExport.h>
#include <cocos/ui/UIDeprecated.h>
#include <cocos/ui/UIEditBox/UIEditBox.h>
#include <cocos/ui/UILayoutComponent.h>
#include <cocos/ui/UIScale9Sprite.h>
#include <cocos/ui/UITabControl.h>

/**
 * @addtogroup ui
 * @{
 */
NS_CC_BEGIN
namespace ui
{
    /**
     * Get current cocos GUI module version string.
     *@return A string representation of GUI module version number
     */
    CC_GUI_DLL const char* CocosGUIVersion();

} // namespace ui

NS_CC_END
// end of ui group
/// @}
