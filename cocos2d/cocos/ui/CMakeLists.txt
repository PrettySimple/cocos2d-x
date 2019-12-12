
if(WINDOWS AND BUILD_SHARED_LIBS)
  ADD_DEFINITIONS (-D_USEGUIDLL)
endif()

#todo: android UIWebViewImpl and UIVideoPlayer
# will add when doing android support

set(COCOS_UI_SRC

  ui/UIScale9Sprite.cpp
  ${COCOS_UI_SPECIFIC_SRC}

)
