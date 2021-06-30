CURRENT_DIR=$(pwd)
cd ../../
TCMALLOC_BIN=$(pwd)/bazel-bin/tcmalloc
cd $CURRENT_DIR

g++ hello_world.cc -c -g -o hello_world.o
g++ hello_world.o -o hello_world -L../../bazel-bin/tcmalloc -ltcmalloc

echo export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$TCMALLOC_BIN