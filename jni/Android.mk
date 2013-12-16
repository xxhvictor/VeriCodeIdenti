LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

OPENCV_INSTALL_MODULES:=on
OPENCV_CAMERA_MODULES:=off
OPENCV_LIB_TYPE:=STATIC
include opencv_ndk/jni/OpenCV.mk

#my config
LOCAL_CFLAGS += -femit-class-debug-always
#use arm thumb, use arm for debug
LOCAL_ARM_MODE := arm

LOCAL_MODULE    := VeriCodeIdenti
LOCAL_SRC_FILES := VeriCodeIdenti.cpp jni_part.cpp
LOCAL_LDLIBS +=  -llog -ldl

include $(BUILD_SHARED_LIBRARY)
