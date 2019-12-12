cmake_minimum_required(VERSION 3.6)

# need to whole-archive this lib
set(lib_name cpp_android_spec)
set(target_name lib${lib_name})

project(${lib_name})

set (src_dir ${PROJECT_SOURCE_DIR})
set (cocos_inc ${PROJECT_SOURCE_DIR}/../../..)

set(${target_name}_src
        ${src_dir}/CCDevice-android.cpp
        ${src_dir}/javaactivity-android.cpp
        ${src_dir}/jni/JniHelper.cpp
        ${src_dir}/jni/TouchesJni.cpp
        ${src_dir}/jni/Java_org_cocos2dx_lib_Cocos2dxAccelerometer.cpp
        ${src_dir}/jni/Java_org_cocos2dx_lib_Cocos2dxBitmap.cpp
        ${src_dir}/jni/Java_org_cocos2dx_lib_Cocos2dxHelper.cpp
        ${src_dir}/jni/Java_org_cocos2dx_lib_Cocos2dxRenderer.cpp
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_include_directories(${target_name}
  PRIVATE ..
  PRIVATE ../..
  PRIVATE ${cocos_inc}

)