#!/bin/sh
cd ./train
echo "cd `pwd`"

if [ "$1x" = "pushx" ]
then
    echo "start push"
    for file in `ls`
    do
        #echo "adb push $file /sdcard/VeriCodeIdenti/train/"
        adb push $file /sdcard/VeriCodeIdenti/train/
    done
    echo "end push"
else
    echo "start pull"
    adb pull /sdcard/VeriCodeIdenti/train/ .
    echo "end pull"
fi
