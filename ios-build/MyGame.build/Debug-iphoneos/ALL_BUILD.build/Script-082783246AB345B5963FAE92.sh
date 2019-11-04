#!/bin/sh
make -C /Users/christophegenova/MyCompany/MyGame/ios-build -f /Users/christophegenova/MyCompany/MyGame/ios-build/CMakeScripts/ALL_BUILD_cmakeRulesBuildPhase.make$CONFIGURATION OBJDIR=$(basename "$OBJECT_FILE_DIR_normal") all
