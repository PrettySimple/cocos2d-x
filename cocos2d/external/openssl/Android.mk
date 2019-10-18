LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libssl_static
LOCAL_MODULE_FILENAME := libssl
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libssl.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_EXPORT_LDFLAGS := -Wl,--exclude-libs=libssl
include $(PREBUILT_STATIC_LIBRARY)

