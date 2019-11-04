# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
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
