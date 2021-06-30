cd ../..
TCMALLOC_DIR=$(pwd)
SRC_DIR=../redis/src
TCMALLOC_BIN=$TCMALLOC_DIR/bazel-bin/tcmalloc
echo 'Make sure you run below'
echo make USE_JEMALLOC=no MALLOC_LIB=\'$TCMALLOC_BIN -ltcmalloc\'
cd $SRC_DIR
make USE_JEMALLOC=no MALLOC_LIB='-L//home/minh/Desktop/tcmalloc/bazel-bin/tcmalloc -ltcmalloc'

echo export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$TCMALLOC_BIN

