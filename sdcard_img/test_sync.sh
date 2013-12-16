#!/bin/sh
cd ./test
echo "cd `pwd`"

if [ "$1x" = "pushx" ]
then
    echo "start push"
    for file in `ls`
    do
        #echo "adb push $file /sdcard/VeriCodeIdenti/raw/"
        adb push $file /sdcard/VeriCodeIdenti/test/
    done
    echo "end push"
else
    echo "only pull test result.txt"
    echo "start pull"
    adb pull /sdcard/VeriCodeIdenti/test/result.txt .
    echo "end pull"
fi
