#!/bin/sh
cd ./raw
echo "cd `pwd`"

if [ "$1x" = "pushx" ]
then
    echo "start push"
    for file in `ls`
    do
        #echo "adb push $file /sdcard/VeriCodeIdenti/raw/"
        adb push $file /sdcard/VeriCodeIdenti/raw/
    done
    echo "end push"
else
    echo "start pull"
    adb pull /sdcard/VeriCodeIdenti/raw/ .
    echo "end pull"
fi
