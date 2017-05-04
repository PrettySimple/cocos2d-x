// Overrides stdio file functions for android, so that assets are searched in the apk if necessary
// based on http://www.50ply.com/blog/2013/01/19/loading-compressed-android-assets-with-file-pointer/#comment-1850768990
//

#include <stdio.h>

#include "android_fopen.h"
#include "../../cocos/platform/android/CCFileUtils-android.h"
#include <errno.h>
#include <android/asset_manager.h>

static int android_read(void* cookie, char* buf, int size) {
    return AAsset_read((AAsset*)cookie, buf, size);
}

static int android_write(void* cookie, const char* buf, int size) {
    return EACCES; // can't provide write access to the apk
}

static fpos_t android_seek(void* cookie, fpos_t offset, int whence) {
    return AAsset_seek((AAsset*)cookie, offset, whence);
}

static int android_close(void* cookie) {
    AAsset_close((AAsset*)cookie);
    return 0;
}

extern FILE* android_fopen(const char* fname, const char* mode) {
    if(mode[0] == 'w') return NULL;

    AAsset* asset = AAssetManager_open(cocos2d::FileUtilsAndroid::getAssetManager(), fname, 0);
    if(!asset) return NULL;

    return funopen(asset, android_read, android_write, android_seek, android_close);
}
