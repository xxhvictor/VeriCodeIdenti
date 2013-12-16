javah -classpath ./bin/classes com.github.xxhvictor.VeriCodeIdenti.VeriCodeIdenti

filename=`ls | grep VeriCodeIdenti.h`
echo "mv $filename ./jni"
mv "$filename" ./jni/
