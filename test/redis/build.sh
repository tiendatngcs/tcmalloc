cd ../..
SRC_DIR=../redis/src
cd $SRC_DIR
make USE_JEMALLOC=no MALLOC_LIB='-L/home/minh/Desktop/tcmalloc/bazel-bin/tcmalloc -ltcmalloc'