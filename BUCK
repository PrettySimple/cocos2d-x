
def my_glob(includes, excludes=None, include_dotfiles=False, build_env=None, search_base=None, allow_safe_import=None):
  results = []
  for include in includes:
    if isinstance(include, tuple):
      tincludes = glob([include[0]])
      for tinclude in tincludes:
        results.append((tinclude, include[1]))
    elif isinstance(include, basestring):
      results.extend(glob([include]))

  return results

IOS_PREPROCESSOR_FLAGS = [
  '-D USE_FILE32API=1',
  '-D CC_TARGET_OS_IPHONE=1',
]
MAC_PREPROCESSOR_FLAGS = [
  '-D USE_FILE32API=1',
  '-D CC_TARGET_OS_MAC=1',
  '-D CC_KEYBOARD_SUPPORT=1',
]

cxx_library(
  name = 'cocos2dx',
  header_namespace = '',
  exported_platform_preprocessor_flags = [
    ('^iphone.*', IOS_PREPROCESSOR_FLAGS),
    ('^macosx.*', MAC_PREPROCESSOR_FLAGS),
  ],
  headers = subdir_glob([
    ('cocos', 'cocos2d.h'),
    # 2d
    ('cocos', '2d/*.h'),
    # 3d
    ('cocos', '3d/*.h'),
    ('cocos', '3d/*.inl'),
    # audio
    ('cocos', 'audio/include/*.h'),
    # base
    ('cocos', 'base/**/*.h'),
    ('cocos', 'base/**/*.hpp'),
    # deprecated
    ('cocos', 'deprecated/*.h'),
    # editor-support
    ('cocos/editor-support', 'cocosbuilder/*.h'),
    ('cocos/editor-support', 'cocostudio/**/*.h'),
    ('cocos', 'editor-support/cocostudio/**/*.h'),
    # math
    ('cocos', 'math/*.h'),
    ('cocos', 'math/*.inl'),
    # navmesh
    ('cocos', 'navmesh/*.h'),
    # network
    ('cocos', 'network/CCDownloader-curl.h'),
    ('cocos', 'network/CCDownloader.h'),
    ('cocos', 'network/CCIDownloaderImpl.h'),
    ('cocos', 'network/HttpClient.h'),
    ('cocos', 'network/HttpCookie.h'),
    ('cocos', 'network/HttpRequest.h'),
    ('cocos', 'network/HttpResponse.h'),
    ('cocos', 'network/SocketIO.h'),
    ('cocos', 'network/WebSocket.h'),
    # physics
    ('cocos', 'physics/*.h'),
    # physics3d
    ('cocos', 'physics3d/*.h'),
    # platform
    ('cocos', 'platform/*.h'),
    # renderer
    ('cocos', 'renderer/*.h'),
    ('cocos', 'renderer/*.frag'),
    ('cocos', 'renderer/*.vert'),
    # scripting
    # storage
    ('cocos', 'storage/local-storage/*.h'),
    # ui
    ('cocos', 'ui/*.h'),
    ('cocos', 'ui/UIEditBox/UIEditBox.h'),
    ('cocos', 'ui/UIEditBox/UIEditBoxImpl.h'),
    ('cocos', 'ui/UIEditBox/UIEditBoxImpl-common.h'),
    # vr
    ('cocos', 'vr/*.h'),
  ]),
  platform_headers = [
    ('^macosx.*' , subdir_glob([
      # audio
      ('cocos', 'audio/apple/*.h'),
      ('cocos', 'audio/mac/*.h'),
      # network
      ('cocos', 'network/CCDownloader-apple.h'),
      ('cocos', 'network/HttpAsynConnection-apple.h'),
      # platform
      ('cocos', 'platform/apple/*.h'),
      ('cocos', 'platform/mac/*.h'),
      ('cocos', 'platform/desktop/*.h'),
      # ui
      ('cocos', 'ui/UIEditBox/UIEditBoxImpl-mac.h'),
      ('cocos', 'ui/UIEditBox/Mac/*.h'),
    ])),
    ('^iphone.*' , subdir_glob([
      # audio
      ('cocos', 'audio/apple/*.h'),
      ('cocos', 'audio/ios/*.h'),
      # network
      ('cocos', 'network/CCDownloader-apple.h'),
      ('cocos', 'network/HttpAsynConnection-apple.h'),
      # platform
      ('cocos', 'platform/apple/*.h'),
      ('cocos', 'platform/ios/*.h'),
      # ui
      ('cocos', 'ui/UIEditBox/UIEditBoxImpl-ios.h'),
      ('cocos', 'ui/UIEditBox/iOS/*.h'),
    ])),
  ],
  exported_headers = subdir_glob([
    ('cocos', 'cocos2d.h'),
    # 2d
    ('cocos', '2d/*.h'),
    # 3d
    ('cocos', '3d/*.h'),
    ('cocos', '3d/*.inl'),
    # audio
    ('cocos', 'audio/include/*.h'),
    # base
    ('cocos', 'base/**/*.h'),
    ('cocos', 'base/**/*.hpp'),
    # deprecated
    ('cocos', 'deprecated/*.h'),
    # editor-support
    ('cocos/editor-support', 'cocosbuilder/*.h'),
    ('cocos/editor-support', 'cocostudio/**/*.h'),
    ('cocos', 'editor-support/cocostudio/**/*.h'),
    # math
    ('cocos', 'math/*.h'),
    ('cocos', 'math/*.inl'),
    # navmesh
    ('cocos', 'navmesh/*.h'),
    # network
    ('cocos', 'network/CCDownloader-curl.h'),
    ('cocos', 'network/CCDownloader.h'),
    ('cocos', 'network/CCIDownloaderImpl.h'),
    ('cocos', 'network/HttpClient.h'),
    ('cocos', 'network/HttpCookie.h'),
    ('cocos', 'network/HttpRequest.h'),
    ('cocos', 'network/HttpResponse.h'),
    ('cocos', 'network/SocketIO.h'),
    ('cocos', 'network/WebSocket.h'),
    # physics
    ('cocos', 'physics/*.h'),
    # physics3d
    ('cocos', 'physics3d/*.h'),
    # platform
    ('cocos', 'platform/*.h'),
    # renderer
    ('cocos', 'renderer/*.h'),
    ('cocos', 'renderer/*.frag'),
    ('cocos', 'renderer/*.vert'),
    # scripting
    # storage
    ('cocos', 'storage/local-storage/*.h'),
    # ui
    ('cocos', 'ui/*.h'),
    ('cocos', 'ui/UIEditBox/UIEditBox.h'),
    ('cocos', 'ui/UIEditBox/UIEditBoxImpl.h'),
    ('cocos', 'ui/UIEditBox/UIEditBoxImpl-common.h'),
    # vr
    ('cocos', 'vr/*.h'),

    ('', 'cocos/cocos2d.h'),
    # 2d
    ('', 'cocos/2d/*.h'),
    # 3d
    ('', 'cocos/3d/*.h'),
    ('', 'cocos/3d/*.inl'),
    # audio
    ('', 'cocos/audio/include/*.h'),
    # base
    ('', 'cocos/base/**/*.h'),
    ('', 'cocos/base/**/*.hpp'),
    # deprecated
    ('', 'cocos/deprecated/*.h'),
    # editor-support
    ('', 'cocos/editor-support/cocosbuilder/*.h'),
    ('', 'cocos/editor-support/cocostudio/**/*.h'),
    ('', 'cocos/editor-support/cocostudio/**/*.h'),
    # math
    ('', 'cocos/math/*.h'),
    ('', 'cocos/math/*.inl'),
    # navmesh
    ('', 'cocos/navmesh/*.h'),
    # network
    ('', 'cocos/network/CCDownloader-curl.h'),
    ('', 'cocos/network/CCDownloader.h'),
    ('', 'cocos/network/CCIDownloaderImpl.h'),
    ('', 'cocos/network/HttpClient.h'),
    ('', 'cocos/network/HttpCookie.h'),
    ('', 'cocos/network/HttpRequest.h'),
    ('', 'cocos/network/HttpResponse.h'),
    ('', 'cocos/network/SocketIO.h'),
    ('', 'cocos/network/WebSocket.h'),
    # physics
    ('', 'cocos/physics/*.h'),
    # physics3d
    ('', 'cocos/physics3d/*.h'),
    # platform
    ('', 'cocos/platform/*.h'),
    # renderer
    ('', 'cocos/renderer/*.h'),
    ('', 'cocos/renderer/*.frag'),
    ('', 'cocos/renderer/*.vert'),
    # scripting
    # storage
    ('', 'cocos/storage/local-storage/*.h'),
    # ui
    ('', 'cocos/ui/*.h'),
    ('', 'cocos/ui/UIEditBox/UIEditBox.h'),
    ('', 'cocos/ui/UIEditBox/UIEditBoxImpl.h'),
    ('', 'cocos/ui/UIEditBox/UIEditBoxImpl-common.h'),
    # vr
    ('', 'cocos/vr/*.h'),
  ]),
  exported_platform_headers = [
    ('^macosx.*' , subdir_glob([
      # audio
      ('cocos', 'audio/apple/*.h'),
      ('cocos', 'audio/mac/*.h'),
      # network
      ('cocos', 'network/CCDownloader-apple.h'),
      ('cocos', 'network/HttpAsynConnection-apple.h'),
      # platform
      ('cocos', 'platform/apple/*.h'),
      ('cocos', 'platform/mac/*.h'),
      ('cocos', 'platform/desktop/*.h'),
      # ui
      ('cocos', 'ui/UIEditBox/UIEditBoxImpl-mac.h'),
      ('cocos', 'ui/UIEditBox/Mac/*.h'),

      # audio
      ('', 'cocos/audio/apple/*.h'),
      ('', 'cocos/audio/mac/*.h'),
      # network
      ('', 'cocos/network/CCDownloader-apple.h'),
      ('', 'cocos/network/HttpAsynConnection-apple.h'),
      # platform
      ('', 'cocos/platform/apple/*.h'),
      ('', 'cocos/platform/mac/*.h'),
      ('', 'cocos/platform/desktop/*.h'),
      # ui
      ('', 'cocos/ui/UIEditBox/UIEditBoxImpl-mac.h'),
      ('', 'cocos/ui/UIEditBox/Mac/*.h'),
    ])),

    ('^iphone.*' , subdir_glob([
      # audio
      ('cocos', 'audio/apple/*.h'),
      ('cocos', 'audio/ios/*.h'),
      # network
      ('cocos', 'network/CCDownloader-apple.h'),
      ('cocos', 'network/HttpAsynConnection-apple.h'),
      # platform
      ('cocos', 'platform/apple/*.h'),
      ('cocos', 'platform/ios/*.h'),
      # ui
      ('cocos', 'ui/UIEditBox/UIEditBoxImpl-ios.h'),
      ('cocos', 'ui/UIEditBox/iOS/*.h'),

      # audio
      ('', 'cocos/audio/apple/*.h'),
      ('', 'cocos/audio/ios/*.h'),
      # network
      ('', 'cocos/network/CCDownloader-apple.h'),
      ('', 'cocos/network/HttpAsynConnection-apple.h'),
      # platform
      ('', 'cocos/platform/apple/*.h'),
      ('', 'cocos/platform/ios/*.h'),
      # ui
      ('', 'cocos/ui/UIEditBox/UIEditBoxImpl-ios.h'),
      ('', 'cocos/ui/UIEditBox/iOS/*.h'),
    ])),
  ],
  srcs = my_glob([
    'cocos/cocos2d.cpp',
    # 2d
    'cocos/2d/*.cpp',
    # 3d
    'cocos/3d/*.cpp',
    # audio
    'cocos/audio/AudioEngine.cpp',
    # base
    'cocos/base/allocator/*.cpp',
    'cocos/base/atitc.cpp',
    'cocos/base/base64.cpp',
    'cocos/base/CCAsyncTaskPool.cpp',
    'cocos/base/CCAutoreleasePool.cpp',
    'cocos/base/ccCArray.cpp',
    'cocos/base/CCConfiguration.cpp',
    'cocos/base/CCConsole.cpp',
    'cocos/base/CCController.cpp',
    'cocos/base/CCData.cpp',
    'cocos/base/CCDataVisitor.cpp',
    'cocos/base/CCDirector.cpp',
    'cocos/base/CCEvent.cpp',
    'cocos/base/CCEventAcceleration.cpp',
    'cocos/base/CCEventController.cpp',
    'cocos/base/CCEventCustom.cpp',
    'cocos/base/CCEventDispatcher.cpp',
    'cocos/base/CCEventFocus.cpp',
    'cocos/base/CCEventKeyboard.cpp',
    'cocos/base/CCEventListener.cpp',
    'cocos/base/CCEventListenerAcceleration.cpp',
    'cocos/base/CCEventListenerController.cpp',
    'cocos/base/CCEventListenerCustom.cpp',
    'cocos/base/CCEventListenerFocus.cpp',
    'cocos/base/CCEventListenerKeyboard.cpp',
    'cocos/base/CCEventListenerMouse.cpp',
    'cocos/base/CCEventListenerTouch.cpp',
    'cocos/base/CCEventMouse.cpp',
    'cocos/base/CCEventTouch.cpp',
    'cocos/base/ccFPSImages.c',
    'cocos/base/CCIMEDispatcher.cpp',
    'cocos/base/CCNinePatchImageParser.cpp',
    'cocos/base/CCNS.cpp',
    'cocos/base/CCProfiling.cpp',
    'cocos/base/CCProperties.cpp',
    'cocos/base/ccRandom.cpp',
    'cocos/base/CCRef.cpp',
    'cocos/base/CCScheduler.cpp',
    'cocos/base/CCScriptSupport.cpp',
    'cocos/base/CCStencilStateManager.cpp',
    'cocos/base/CCTouch.cpp',
    'cocos/base/ccTypes.cpp',
    'cocos/base/CCUserDefault.cpp',
    'cocos/base/ccUTF8.cpp',
    'cocos/base/ccUtils.cpp',
    'cocos/base/CCValue.cpp',
    'cocos/base/etc1.cpp',
    'cocos/base/etc2.cpp',
    'cocos/base/etc2unpack.cpp',
    'cocos/base/etcdec.cxx',
    'cocos/base/ObjectFactory.cpp',
    'cocos/base/pvr.cpp',
    'cocos/base/s3tc.cpp',
    'cocos/base/TGAlib.cpp',
    'cocos/base/ZipUtils.cpp',
    # deprecated
    'cocos/deprecated/*.cpp',
    # editor-support
    #'cocos/editor-support/cocosbuilder/*.cpp',
    #'cocos/editor-support/cocostudio/**/*.cpp',
    # math
    'cocos/math/*.cpp',
    # navmesh
    'cocos/navmesh/*.cpp',
    # network
    'cocos/network/CCDownloader-curl.cpp',
    'cocos/network/CCDownloader.cpp',
    'cocos/network/HttpClient.cpp',
    'cocos/network/HttpCookie.cpp',
    'cocos/network/SocketIO.cpp',
    'cocos/network/WebSocket.cpp',
    # physics
    'cocos/physics/*.cpp',
    # physics3d
    'cocos/physics3d/*.cpp',
    # platform
    'cocos/platform/*.cpp',
    # renderer
    'cocos/renderer/*.cpp',
    # scripting
    # storage
    'cocos/storage/local-storage/LocalStorage.cpp',
    # ui
    'cocos/ui/CocosGUI.cpp',
    'cocos/ui/UIButton.cpp',
    'cocos/ui/UIAbstractCheckButton.cpp',
    'cocos/ui/UICheckBox.cpp',
    'cocos/ui/UIRadioButton.cpp',
    'cocos/ui/UIDeprecated.cpp',
    'cocos/ui/UIHBox.cpp',
    'cocos/ui/UIHelper.cpp',
    'cocos/ui/UIImageView.cpp',
    'cocos/ui/UILayout.cpp',
    'cocos/ui/UILayoutManager.cpp',
    'cocos/ui/UILayoutParameter.cpp',
    'cocos/ui/UIListView.cpp',
    'cocos/ui/UILoadingBar.cpp',
    'cocos/ui/UIPageView.cpp',
    'cocos/ui/UIPageViewIndicator.cpp',
    'cocos/ui/UIRelativeBox.cpp',
    'cocos/ui/UIRichText.cpp',
    'cocos/ui/UIScale9Sprite.cpp',
    'cocos/ui/UIScrollView.cpp',
    'cocos/ui/UIScrollViewBar.cpp',
    'cocos/ui/UISlider.cpp',
    'cocos/ui/UIText.cpp',
    'cocos/ui/UITextAtlas.cpp',
    'cocos/ui/UITextBMFont.cpp',
    'cocos/ui/UITextField.cpp',
    'cocos/ui/UIVBox.cpp',
    'cocos/ui/UIWidget.cpp',
    'cocos/ui/UIEditBox/UIEditBox.cpp',
    'cocos/ui/UILayoutComponent.cpp',
    'cocos/ui/UIEditBox/UIEditBoxImpl-common.cpp',
    'cocos/ui/UITabControl.cpp',
    # vr
    'cocos/vr/CC.cpp',
    'cocos/vr/CCVRDistortion.cpp',
    'cocos/vr/CCVRDistortionMesh.cpp',
    'cocos/vr/CCVRGenericRenderer.cpp',
    ('cocos/vr/CCVRGenericHeadTracker.cpp', ['-x objective-c++']),
  ]),
  platform_srcs = [
    ('^macosx.*' , glob([
      # audio
      'cocos/audio/apple/*.mm',
      'cocos/audio/mac/*.mm',
      'cocos/audio/mac/*.m',
      # base
      'cocos/base/CCController-apple.mm',
      'cocos/base/CCUserDefault-apple.mm',
      # network
      'cocos/network/CCDownloader-apple.mm',
      'cocos/network/HttpAsyncConnection-apple.mm',
      'cocos/network/HttpClient-apple.mm',
      # platform
      'cocos/platform/desktop/*.cpp',
      'cocos/platform/apple/*.cpp',
      'cocos/platform/apple/*.mm',
      'cocos/platform/mac/*.mm',
      # ui
      'cocos/ui/UIEditBox/UIEditBoxImpl-mac.mm',
      'cocos/ui/UIEditBox/Mac/*.mm',
      'cocos/ui/UIEditBox/Mac/*.m',
    ])),

    ('^iphone.*', glob([
      # audio
      'cocos/audio/apple/*.mm',
      'cocos/audio/ios/*.mm',
      'cocos/audio/ios/*.m',
      # base
      'cocos/base/CCController-apple.mm',
      'cocos/base/CCUserDefault-apple.mm',
      # network
      'cocos/network/CCDownloader-apple.mm',
      'cocos/network/HttpAsyncConnection-apple.mm',
      'cocos/network/HttpClient-apple.mm',
      # platform
      'cocos/platform/apple/*.cpp',
      'cocos/platform/apple/*.mm',
      'cocos/platform/ios/*.mm',
      'cocos/platform/ios/*.m',
      # ui
      'cocos/ui/UIEditBox/UIEditBoxImpl-ios.mm',
      'cocos/ui/UIEditBox/iOS/*.mm',
      'cocos/ui/UIEditBox/iOS/*.m',
      'cocos/ui/*.mm',
    ])),
  ],
  platform_compiler_flags = [
    ('^iphone.*', [
      '-fmodules',
    ]),
    ('^macosx.*', [
      '-fmodules',
    ]),
  ],
  exported_platform_linker_flags = [
    ('^iphone.*', glob([
      '-ObjC',
    ])),
  ],
  exported_deps = [
    ':freetype2',
    ':poly2tri',
    ':recast',
    ':clipper',
    ':edtaa3func',
    ':json',
    ':unzip',
    ':tinyxml2',
    ':convertutf',
    ':websockets',
    ':chipmunk',
    ':bullet',
    ':rapidxml',
    #':glfw3',
    ':png',
    ':tiff',
    ':jpeg',
    ':webp',
    ':xxhash',
    ':flatbuffers',
    ':curl',
  ],
  visibility = [
    'PUBLIC',
  ],
)

prebuilt_cxx_library(
  name = 'freetype2',
  lib_name = 'freetype',
  lib_dir = 'external/freetype2/prebuilt/ios',
  header_namespace = '',
  exported_platform_headers = [
    ('^macosx.*' , subdir_glob([
      ('external/freetype2/include/mac/freetype2', '**/*.h'),
    ])),
    ('^iphone.*' , subdir_glob([
      ('external/freetype2/include/ios/freetype2', '**/*.h'),
    ])),
  ],
)

cxx_library(
  name = 'poly2tri',
  header_namespace = 'poly2tri',
  srcs = glob([
    'external/poly2tri/**/*.cc',
  ]),
  headers = subdir_glob([
    ('external/poly2tri', '**/*.h'),
  ]),
  exported_headers = subdir_glob([
    ('external/poly2tri', '**/*.h'),
  ])
)

cxx_library(
  name = 'recast',
  header_namespace = 'recast',
  srcs = glob([
    'external/recast/**/*.cpp',
  ]),
  headers = subdir_glob([
    ('external/recast', '**/*.h'),
  ]),
  exported_headers = subdir_glob([
    ('external/recast', '**/*.h'),
  ])
)

cxx_library(
  name = 'clipper',
  header_namespace = 'clipper',
  srcs = glob([
    'external/clipper/*.cpp',
  ]),
  headers = subdir_glob([
    ('external/clipper', '*.hpp'),
  ]),
  exported_headers = subdir_glob([
    ('external/clipper', '*.hpp'),
  ])
)

cxx_library(
  name = 'edtaa3func',
  header_namespace = '',
  srcs = glob([
    'external/edtaa3func/*.cpp',
  ]),
  headers = subdir_glob([
    ('external/edtaa3func', '*.h'),
  ]),
  exported_headers = subdir_glob([
    ('external/edtaa3func', '*.h'),
  ])
)

prebuilt_cxx_library(
  name = 'json',
  header_only = True,
  header_namespace = 'json',
  exported_preprocessor_flags = [
    '-D RAPIDJSON_HAS_STDSTRING=1',
  ],
  exported_headers = subdir_glob([
    ('external/json', '**/*.h'),
  ]),
)

prebuilt_cxx_library(
  name = 'rapidxml',
  header_only = True,
  header_namespace = 'rapidxml',
  exported_headers = subdir_glob([
    ('external/rapidxml', '*.hpp'),
  ]),
)

cxx_library(
  name = 'unzip',
  header_namespace = '',
  platform_preprocessor_flags = [
    ('^iphone.*', IOS_PREPROCESSOR_FLAGS),
    ('^macosx.*', MAC_PREPROCESSOR_FLAGS),
  ],
  srcs = glob([
    'external/unzip/*.cpp',
  ]),
  headers = subdir_glob([
    ('cocos', 'platform/CCPlatformConfig.h'),
    ('cocos', 'platform/CCPlatformDefine.h'),
  ]),
  platform_headers = [
    ('^iphone.*' , subdir_glob([
      ('cocos', 'platform/ios/CCPlatformDefine-ios.h'),
    ])),
  ],
  exported_headers = subdir_glob([
    ('external/unzip', '*.h'),
    ('external', 'unzip/*.h'),
  ]),
  visibility = [
    'PUBLIC',
  ],
)

cxx_library(
  name = 'tinyxml2',
  header_namespace = '',
  platform_preprocessor_flags = [
    ('^iphone.*', IOS_PREPROCESSOR_FLAGS),
    ('^macosx.*', MAC_PREPROCESSOR_FLAGS),
  ],
  srcs = glob([
    'external/tinyxml2/*.cpp',
  ]),
  headers = subdir_glob([
    ('cocos', 'platform/CCPlatformConfig.h'),
    ('cocos', 'platform/CCPlatformMacros.h'),
    ('cocos', 'platform/CCPlatformDefine.h'),
    ('cocos', 'base/ccConfig.h'),
    ('external/tinyxml2', '*.h'),
  ]),
  platform_headers = [
    ('^iphone.*' , subdir_glob([
      ('cocos', 'platform/ios/CCPlatformDefine-ios.h'),
    ])),
  ],
  exported_headers = subdir_glob([
    ('external/tinyxml2', '*.h'),
  ])
)

cxx_library(
  name = 'convertutf',
  header_namespace = '',
  srcs = glob([
    'external/ConvertUTF/*.cpp',
    'external/ConvertUTF/*.c',
  ]),
  headers = subdir_glob([
    ('external/ConvertUTF', '*.h'),
  ]),
  exported_headers = subdir_glob([
    ('external/ConvertUTF', '*.h'),
  ])
)

cxx_library(
  name = 'xxhash',
  header_namespace = '',
  srcs = glob([
    'external/xxhash/*.c',
  ]),
  headers = subdir_glob([
    ('external/xxhash', '*.h'),
  ]),
  exported_headers = subdir_glob([
    ('external/xxhash', '*.h'),
  ])
)

cxx_library(
  name = 'flatbuffers',
  header_namespace = 'flatbuffers',
  srcs = glob([
    'external/flatbuffers/*.cpp',
  ]),
  headers = subdir_glob([
    ('external/flatbuffers', '*.h'),
  ]),
  exported_headers = subdir_glob([
    ('external/flatbuffers', '*.h'),
  ])
)

cxx_library(
  name = 'extensions',
  header_namespace = 'extensions',
  srcs = glob([
    'extensions/**/*.cpp',
  ]),
  headers = subdir_glob([
    ('extensions', '**/*.h'),
  ]),
  exported_headers = subdir_glob([
    ('extensions', '**/*.h'),
  ]),
  exported_deps = [
    ':cocos2dx',
  ],
  visibility = [
    'PUBLIC',
  ],
)

prebuilt_cxx_library(
  name = 'websockets',
  lib_dir = 'external/websockets/prebuilt/ios',
  header_namespace = '',
  exported_platform_headers = [
    ('^macosx.*' , subdir_glob([
      ('external/websockets/include/mac', '*.h'),
    ])),
    ('^iphone.*' , subdir_glob([
      ('external/websockets/include/ios', '*.h'),
    ])),
  ],
)

prebuilt_cxx_library(
  name = 'png',
  lib_dir = 'external/png/prebuilt/ios',
  header_namespace = '',
  exported_platform_headers = [
    ('^macosx.*' , subdir_glob([
      ('external/png/include/mac', '*.h'),
    ])),
    ('^iphone.*' , subdir_glob([
      ('external/png/include/ios', '*.h'),
    ])),
  ],
)

prebuilt_cxx_library(
  name = 'tiff',
  lib_dir = 'external/tiff/prebuilt/ios',
  header_namespace = '',
  exported_platform_headers = [
    ('^macosx.*' , subdir_glob([
      ('external/tiff/include/mac', '*.h'),
    ])),
    ('^iphone.*' , subdir_glob([
      ('external/tiff/include/ios', '*.h'),
    ])),
  ],
)

prebuilt_cxx_library(
  name = 'jpeg',
  lib_dir = 'external/jpeg/prebuilt/ios',
  header_namespace = '',
  exported_platform_headers = [
    ('^macosx.*' , subdir_glob([
      ('external/jpeg/include/mac', '*.h'),
    ])),
    ('^iphone.*' , subdir_glob([
      ('external/jpeg/include/ios', '*.h'),
    ])),
  ],
)

prebuilt_cxx_library(
  name = 'webp',
  lib_dir = 'external/webp/prebuilt/ios',
  header_namespace = '',
  exported_platform_headers = [
    ('^macosx.*' , subdir_glob([
      ('external/webp/include/mac', '*.h'),
    ])),
    ('^iphone.*' , subdir_glob([
      ('external/webp/include/ios', '*.h'),
    ])),
  ],
)

prebuilt_cxx_library(
  name = 'glfw3',
  lib_dir = 'external/glfw3/prebuilt/mac',
  header_namespace = '',
  exported_platform_headers = [
    ('^macosx.*' , subdir_glob([
      ('external/glfw3/include/mac', '*.h'),
    ])),
  ],
)

prebuilt_cxx_library(
  name = 'chipmunk',
  lib_dir = 'external/chipmunk/prebuilt/ios',
  header_namespace = 'chipmunk',
  exported_headers = subdir_glob([
    ('external/chipmunk/include/chipmunk', '**/*.h'),
  ]),
)

prebuilt_cxx_library(
  name = 'curl',
  lib_dir = 'external/curl/prebuilt/ios',
  header_namespace = 'curl',
  exported_platform_headers = [
    ('^macosx.*' , subdir_glob([
      ('external/curl/include/mac/curl', '*.h'),
    ])),
    ('^iphone.*' , subdir_glob([
      ('external/curl/include/ios/curl', '*.h'),
    ])),
  ],
)

cxx_library(
  name = 'bullet',
  header_namespace = 'bullet',
  srcs = glob([
    'external/bullet/BulletCollision/BroadphaseCollision/btAxisSweep3.cpp',
    'external/bullet/BulletCollision/BroadphaseCollision/btBroadphaseProxy.cpp',
    'external/bullet/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp',
    'external/bullet/BulletCollision/BroadphaseCollision/btDbvt.cpp',
    'external/bullet/BulletCollision/BroadphaseCollision/btDbvtBroadphase.cpp',
    'external/bullet/BulletCollision/BroadphaseCollision/btDispatcher.cpp',
    'external/bullet/BulletCollision/BroadphaseCollision/btMultiSapBroadphase.cpp',
    'external/bullet/BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp',
    'external/bullet/BulletCollision/BroadphaseCollision/btQuantizedBvh.cpp',
    'external/bullet/BulletCollision/BroadphaseCollision/btSimpleBroadphase.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btCollisionObject.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btCollisionWorld.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btCompoundCompoundCollisionAlgorithm.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btGhostObject.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btHashedSimplePairCache.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btInternalEdgeUtility.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btManifoldResult.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/btUnionFind.cpp',
    'external/bullet/BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btBoxShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btBox2dShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btCapsuleShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btCollisionShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btCompoundShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btConcaveShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btConeShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btConvexHullShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btConvexInternalShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btConvexPointCloudShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btConvexPolyhedron.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btConvexShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btConvex2dShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btCylinderShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btEmptyShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btMinkowskiSumShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btMultiSphereShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btOptimizedBvh.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btShapeHull.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btSphereShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btStaticPlaneShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btStridingMeshInterface.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btTetrahedronShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btTriangleBuffer.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btTriangleCallback.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btTriangleMesh.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btTriangleMeshShape.cpp',
    'external/bullet/BulletCollision/CollisionShapes/btUniformScalingShape.cpp',
    'external/bullet/BulletCollision/Gimpact/btContactProcessing.cpp',
    'external/bullet/BulletCollision/Gimpact/btGenericPoolAllocator.cpp',
    'external/bullet/BulletCollision/Gimpact/btGImpactBvh.cpp',
    'external/bullet/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.cpp',
    'external/bullet/BulletCollision/Gimpact/btGImpactQuantizedBvh.cpp',
    'external/bullet/BulletCollision/Gimpact/btGImpactShape.cpp',
    'external/bullet/BulletCollision/Gimpact/btTriangleShapeEx.cpp',
    'external/bullet/BulletCollision/Gimpact/gim_box_set.cpp',
    'external/bullet/BulletCollision/Gimpact/gim_contact.cpp',
    'external/bullet/BulletCollision/Gimpact/gim_memory.cpp',
    'external/bullet/BulletCollision/Gimpact/gim_tri_collision.cpp',
    'external/bullet/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp',
    'external/bullet/BulletCollision/NarrowPhaseCollision/btConvexCast.cpp',
    'external/bullet/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp',
    'external/bullet/BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp',
    'external/bullet/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp',
    'external/bullet/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp',
    'external/bullet/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp',
    'external/bullet/BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp',
    'external/bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp',
    'external/bullet/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp',
    'external/bullet/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp',
    'external/bullet/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.cpp',
    'external/bullet/BulletDynamics/Character/btKinematicCharacterController.cpp',
    'external/bullet/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp',
    'external/bullet/BulletDynamics/ConstraintSolver/btContactConstraint.cpp',
    'external/bullet/BulletDynamics/ConstraintSolver/btFixedConstraint.cpp',
    'external/bullet/BulletDynamics/ConstraintSolver/btGearConstraint.cpp',
    'external/bullet/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp',
    'external/bullet/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.cpp',
    'external/bullet/BulletDynamics/ConstraintSolver/btHinge2Constraint.cpp',
    'external/bullet/BulletDynamics/ConstraintSolver/btHingeConstraint.cpp',
    'external/bullet/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.cpp',
    'external/bullet/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp',
    'external/bullet/BulletDynamics/ConstraintSolver/btSliderConstraint.cpp',
    'external/bullet/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.cpp',
    'external/bullet/BulletDynamics/ConstraintSolver/btTypedConstraint.cpp',
    'external/bullet/BulletDynamics/ConstraintSolver/btUniversalConstraint.cpp',
    'external/bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp',
    'external/bullet/BulletDynamics/Dynamics/btRigidBody.cpp',
    'external/bullet/BulletDynamics/Dynamics/btSimpleDynamicsWorld.cpp',
    'external/bullet/BulletDynamics/Vehicle/btRaycastVehicle.cpp',
    'external/bullet/BulletDynamics/Vehicle/btWheelInfo.cpp',
    'external/bullet/BulletDynamics/Featherstone/btMultiBody.cpp',
    'external/bullet/BulletDynamics/Featherstone/btMultiBodyConstraintSolver.cpp',
    'external/bullet/BulletDynamics/Featherstone/btMultiBodyDynamicsWorld.cpp',
    'external/bullet/BulletDynamics/Featherstone/btMultiBodyJointLimitConstraint.cpp',
    'external/bullet/BulletDynamics/Featherstone/btMultiBodyConstraint.cpp',
    'external/bullet/BulletDynamics/Featherstone/btMultiBodyPoint2Point.cpp',
    'external/bullet/BulletDynamics/Featherstone/btMultiBodyJointMotor.cpp',
    'external/bullet/BulletDynamics/MLCPSolvers/btDantzigLCP.cpp',
    'external/bullet/BulletDynamics/MLCPSolvers/btMLCPSolver.cpp',
    'external/bullet/BulletMultiThreaded/btGpu3DGridBroadphase.cpp',
    'external/bullet/BulletMultiThreaded/btParallelConstraintSolver.cpp',
    'external/bullet/BulletMultiThreaded/btThreadSupportInterface.cpp',
    'external/bullet/BulletMultiThreaded/PosixThreadSupport.cpp',
    'external/bullet/BulletMultiThreaded/SequentialThreadSupport.cpp',
    'external/bullet/BulletMultiThreaded/SpuCollisionObjectWrapper.cpp',
    'external/bullet/BulletMultiThreaded/SpuCollisionTaskProcess.cpp',
    'external/bullet/BulletMultiThreaded/SpuContactManifoldCollisionAlgorithm.cpp',
    'external/bullet/BulletMultiThreaded/SpuFakeDma.cpp',
    'external/bullet/BulletMultiThreaded/SpuGatheringCollisionDispatcher.cpp',
    'external/bullet/BulletMultiThreaded/SpuLibspe2Support.cpp',
    'external/bullet/BulletMultiThreaded/SpuSampleTaskProcess.cpp',
    'external/bullet/BulletMultiThreaded/SpuLibspe2Support.cpp',
    'external/bullet/BulletMultiThreaded/SpuNarrowPhaseCollisionTask/boxBoxDistance.cpp',
    'external/bullet/BulletMultiThreaded/SpuNarrowPhaseCollisionTask/SpuCollisionShapes.cpp',
    'external/bullet/BulletMultiThreaded/SpuNarrowPhaseCollisionTask/SpuContactResult.cpp',
    'external/bullet/BulletMultiThreaded/SpuNarrowPhaseCollisionTask/SpuGatheringCollisionTask.cpp',
    'external/bullet/BulletMultiThreaded/SpuNarrowPhaseCollisionTask/SpuMinkowskiPenetrationDepthSolver.cpp',
    'external/bullet/BulletMultiThreaded/SpuSampleTask/SpuSampleTask.cpp',
    'external/bullet/LinearMath/btAlignedAllocator.cpp',
    'external/bullet/LinearMath/btConvexHull.cpp',
    'external/bullet/LinearMath/btConvexHullComputer.cpp',
    'external/bullet/LinearMath/btGeometryUtil.cpp',
    'external/bullet/LinearMath/btPolarDecomposition.cpp',
    'external/bullet/LinearMath/btQuickprof.cpp',
    'external/bullet/LinearMath/btSerializer.cpp',
    'external/bullet/LinearMath/btVector3.cpp',
    'external/bullet/MiniCL/MiniCLTask/MiniCLTask.cpp',
    'external/bullet/MiniCL/MiniCL.cpp',
    'external/bullet/MiniCL/MiniCLTaskScheduler.cpp',
  ]),
  headers = subdir_glob([
    ('external/bullet', '**/*.h'),
  ]),
  exported_headers = subdir_glob([
    ('external/bullet', '**/*.h'),
  ])
)
