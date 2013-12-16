#!/bin/sh
mycmd=`echo $0 $*`
echo "$mycmd" >> ./prepare_hist.txt

#check param
if [ $# -ne 2 ]
then
    echo "wrong input!"
    echo "command filename labe"
    exit 1
fi

code=$2
codeLen=`expr length $code`
if [ $codeLen -ne 4 ]
then
    echo "need length 4"
    exit 2
fi

#judge exist
filename=$1
fullfilename="$filename.png"
if [ ! -f $fullfilename ]
then
    echo "file: $fullfilename not exist"
    exit 3
fi

#copy file
if [ -f "../raw/$fullfilename" ]
then
    rm "../raw/$fullfilename"
fi
cp "$fullfilename" "../raw/"


#modifiy raw/config.ini
#make config.ini
if [ ! -f "../raw/config.ini" ]
then
    echo "make config.ini"
    touch "../raw/config.ini"
fi
exits_finame=`grep $filename ../raw/config.ini`
if [ "$exits_finame"x = "x" ]
then
    echo "$filename" >> "../raw/config.ini"
fi

#mkconfig
if [ ! -f "../sample/charlist.ini" ]
then
    echo "charlist.ini not exist, creat it"
    echo "" > "../sample/charlist.ini"
fi

#decode code
code1=`expr substr $code 1 1`
code2=`expr substr $code 2 1`
code3=`expr substr $code 3 1`
code4=`expr substr $code 4 1`
mycount=1
#for x in echo $code1 $code2 $code3 $code4
for x in $code1 $code2 $code3 $code4
do
    codein=`grep ${x} ../sample/charlist.ini`
    if [ "$codein"x = "x" ]
    then
        oldstring=`cat ../sample/charlist.ini`
        oldstring="${oldstring}${x}"
        rm ../sample/charlist.ini
        echo "$oldstring" >> ../sample/charlist.ini
    fi

    if [ ! -f "../sample/${x}.ini" ]
    then
        touch "../sample/${x}.ini"
    fi

    tag="${filename}-${mycount}"
    tagin=`grep ${tag} ../sample/${x}.ini`
    if [ "$tagin"x = "x" ]
    then
        echo $tag >> ../sample/${x}.ini
    fi
    mycount=`expr $mycount + 1`
done

