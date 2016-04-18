LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libcrypto_static
LOCAL_MODULE_FILENAME := libcrypto
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libcrypto.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_EXPORT_LDFLAGS := -Wl,--exclude-libs=libcrypto
include $(PREBUILT_STATIC_LIBRARY)
