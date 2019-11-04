# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.MyGame.Debug:
PostBuild.cocos2d.Debug: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Debug/MyGame.app/MyGame
PostBuild.external.Debug: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Debug/MyGame.app/MyGame
PostBuild.ext_recast.Debug: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Debug/MyGame.app/MyGame
PostBuild.ext_tinyxml2.Debug: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Debug/MyGame.app/MyGame
PostBuild.ext_xxhash.Debug: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Debug/MyGame.app/MyGame
PostBuild.ext_xxtea.Debug: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Debug/MyGame.app/MyGame
PostBuild.ext_clipper.Debug: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Debug/MyGame.app/MyGame
PostBuild.ext_edtaa3func.Debug: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Debug/MyGame.app/MyGame
PostBuild.ext_convertUTF.Debug: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Debug/MyGame.app/MyGame
PostBuild.ext_poly2tri.Debug: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Debug/MyGame.app/MyGame
PostBuild.ext_md5.Debug: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Debug/MyGame.app/MyGame
PostBuild.ext_unzip.Debug: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Debug/MyGame.app/MyGame
/Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Debug/MyGame.app/MyGame:\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libcocos2d.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libexternal.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/Box2D/prebuilt/ios/libbox2d.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/chipmunk/prebuilt/ios/libchipmunk.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/freetype2/prebuilt/ios/libfreetype.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_recast.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/bullet/prebuilt/ios/libLinearMath.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/bullet/prebuilt/ios/libBulletDynamics.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/bullet/prebuilt/ios/libBulletCollision.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/bullet/prebuilt/ios/libLinearMath.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/bullet/prebuilt/ios/libBulletMultiThreaded.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/bullet/prebuilt/ios/libMiniCL.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/jpeg/prebuilt/ios/libjpeg.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/webp/prebuilt/ios/libwebp.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/websockets/prebuilt/ios/libwebsockets.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/openssl/prebuilt/ios/libssl.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/openssl/prebuilt/ios/libcrypto.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/uv/prebuilt/ios/libuv_a.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_tinyxml2.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_xxhash.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_xxtea.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_clipper.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_edtaa3func.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_convertUTF.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_poly2tri.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_md5.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/curl/prebuilt/ios/libcurl.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/png/prebuilt/ios/libpng.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_unzip.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/glsl-optimizer/prebuilt/ios/libglcpp-library.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/glsl-optimizer/prebuilt/ios/libglsl_optimizer.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/glsl-optimizer/prebuilt/ios/libmesa.a\
	/usr/lib/libz.dylib\
	/usr/lib/libiconv.dylib
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Debug/MyGame.app/MyGame


PostBuild.cocos2d.Debug:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libcocos2d.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libcocos2d.a


PostBuild.ext_clipper.Debug:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_clipper.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_clipper.a


PostBuild.ext_convertUTF.Debug:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_convertUTF.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_convertUTF.a


PostBuild.ext_edtaa3func.Debug:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_edtaa3func.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_edtaa3func.a


PostBuild.ext_md5.Debug:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_md5.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_md5.a


PostBuild.ext_poly2tri.Debug:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_poly2tri.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_poly2tri.a


PostBuild.ext_recast.Debug:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_recast.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_recast.a


PostBuild.ext_tinyxml2.Debug:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_tinyxml2.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_tinyxml2.a


PostBuild.ext_unzip.Debug:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_unzip.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_unzip.a


PostBuild.ext_xxhash.Debug:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_xxhash.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_xxhash.a


PostBuild.ext_xxtea.Debug:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_xxtea.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_xxtea.a


PostBuild.external.Debug:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libexternal.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libexternal.a


PostBuild.MyGame.Release:
PostBuild.cocos2d.Release: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Release/MyGame.app/MyGame
PostBuild.external.Release: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Release/MyGame.app/MyGame
PostBuild.ext_recast.Release: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Release/MyGame.app/MyGame
PostBuild.ext_tinyxml2.Release: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Release/MyGame.app/MyGame
PostBuild.ext_xxhash.Release: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Release/MyGame.app/MyGame
PostBuild.ext_xxtea.Release: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Release/MyGame.app/MyGame
PostBuild.ext_clipper.Release: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Release/MyGame.app/MyGame
PostBuild.ext_edtaa3func.Release: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Release/MyGame.app/MyGame
PostBuild.ext_convertUTF.Release: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Release/MyGame.app/MyGame
PostBuild.ext_poly2tri.Release: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Release/MyGame.app/MyGame
PostBuild.ext_md5.Release: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Release/MyGame.app/MyGame
PostBuild.ext_unzip.Release: /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Release/MyGame.app/MyGame
/Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Release/MyGame.app/MyGame:\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libcocos2d.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libexternal.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/Box2D/prebuilt/ios/libbox2d.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/chipmunk/prebuilt/ios/libchipmunk.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/freetype2/prebuilt/ios/libfreetype.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_recast.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/bullet/prebuilt/ios/libLinearMath.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/bullet/prebuilt/ios/libBulletDynamics.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/bullet/prebuilt/ios/libBulletCollision.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/bullet/prebuilt/ios/libLinearMath.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/bullet/prebuilt/ios/libBulletMultiThreaded.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/bullet/prebuilt/ios/libMiniCL.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/jpeg/prebuilt/ios/libjpeg.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/webp/prebuilt/ios/libwebp.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/websockets/prebuilt/ios/libwebsockets.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/openssl/prebuilt/ios/libssl.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/openssl/prebuilt/ios/libcrypto.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/uv/prebuilt/ios/libuv_a.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_tinyxml2.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_xxhash.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_xxtea.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_clipper.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_edtaa3func.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_convertUTF.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_poly2tri.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_md5.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/curl/prebuilt/ios/libcurl.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/png/prebuilt/ios/libpng.a\
	/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_unzip.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/glsl-optimizer/prebuilt/ios/libglcpp-library.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/glsl-optimizer/prebuilt/ios/libglsl_optimizer.a\
	/Users/christophegenova/MyCompany/MyGame/cocos2d/external/glsl-optimizer/prebuilt/ios/libmesa.a\
	/usr/lib/libz.dylib\
	/usr/lib/libiconv.dylib
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/bin/MyGame/Release/MyGame.app/MyGame


PostBuild.cocos2d.Release:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libcocos2d.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libcocos2d.a


PostBuild.ext_clipper.Release:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_clipper.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_clipper.a


PostBuild.ext_convertUTF.Release:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_convertUTF.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_convertUTF.a


PostBuild.ext_edtaa3func.Release:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_edtaa3func.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_edtaa3func.a


PostBuild.ext_md5.Release:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_md5.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_md5.a


PostBuild.ext_poly2tri.Release:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_poly2tri.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_poly2tri.a


PostBuild.ext_recast.Release:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_recast.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_recast.a


PostBuild.ext_tinyxml2.Release:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_tinyxml2.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_tinyxml2.a


PostBuild.ext_unzip.Release:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_unzip.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_unzip.a


PostBuild.ext_xxhash.Release:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_xxhash.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_xxhash.a


PostBuild.ext_xxtea.Release:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_xxtea.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_xxtea.a


PostBuild.external.Release:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libexternal.a:
	/bin/rm -f /Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libexternal.a




# For each target create a dummy ruleso the target does not have to exist
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/Box2D/prebuilt/ios/libbox2d.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/bullet/prebuilt/ios/libBulletCollision.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/bullet/prebuilt/ios/libBulletDynamics.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/bullet/prebuilt/ios/libBulletMultiThreaded.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/bullet/prebuilt/ios/libLinearMath.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/bullet/prebuilt/ios/libMiniCL.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/chipmunk/prebuilt/ios/libchipmunk.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/curl/prebuilt/ios/libcurl.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/freetype2/prebuilt/ios/libfreetype.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/glsl-optimizer/prebuilt/ios/libglcpp-library.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/glsl-optimizer/prebuilt/ios/libglsl_optimizer.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/glsl-optimizer/prebuilt/ios/libmesa.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/jpeg/prebuilt/ios/libjpeg.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/openssl/prebuilt/ios/libcrypto.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/openssl/prebuilt/ios/libssl.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/png/prebuilt/ios/libpng.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/uv/prebuilt/ios/libuv_a.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/webp/prebuilt/ios/libwebp.a:
/Users/christophegenova/MyCompany/MyGame/cocos2d/external/websockets/prebuilt/ios/libwebsockets.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libcocos2d.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_clipper.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_convertUTF.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_edtaa3func.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_md5.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_poly2tri.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_recast.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_tinyxml2.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_unzip.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_xxhash.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libext_xxtea.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Debug/libexternal.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libcocos2d.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_clipper.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_convertUTF.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_edtaa3func.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_md5.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_poly2tri.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_recast.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_tinyxml2.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_unzip.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_xxhash.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libext_xxtea.a:
/Users/christophegenova/MyCompany/MyGame/ios-build/lib/Release/libexternal.a:
/usr/lib/libiconv.dylib:
/usr/lib/libz.dylib:
