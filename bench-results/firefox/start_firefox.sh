cd ..
TCMALLOC_DIR=$(pwd)
TCMALLOC_LIB=$TCMALLOC_DIR/bazel-bin/tcmalloc/libtcmalloc.so
OBJ_DIR=/home/minh/Desktop/mozilla/obj-x86_64-pc-linux-gnu/dist/bin
LD_PRELOAD=$TCMALLOC_LIB $OBJ_DIR/firefox -no-remote
