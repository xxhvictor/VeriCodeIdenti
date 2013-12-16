#!/bin/sh
ndk-build $1
echo "cp opencv_ndk/libs/armeabi/libopencv_info.so"
cp opencv_ndk/libs/armeabi/libopencv_info.so libs/armeabi/
echo "cp opencv_ndk/libs/armeabi/libopencv_java.so" 
cp opencv_ndk/libs/armeabi/libopencv_java.so libs/armeabi/
ant clean
ant debug
