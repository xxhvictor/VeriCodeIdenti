#!/bin/sh
cd ./sample
echo "cd `pwd`"

if [ "$1" = "push" ]
then
    echo "start push"
    for file in `ls`
    do
        #echo "adb push $file /sdcard/VeriCodeIdenti/sample/"
        adb push $file /sdcard/VeriCodeIdenti/sample/
    done
    echo "end push"
else
    echo "only pull *.png"
    echo "start pull"
    adb pull /sdcard/VeriCodeIdenti/sample/ .
    echo "end pull"
fi
