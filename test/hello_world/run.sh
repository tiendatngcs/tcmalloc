CURRENT_DIR=$(pwd)
cd ../../
TCMALLOC_BIN=$(pwd)/bazel-bin/tcmalloc
cd $CURRENT_DIR

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$TCMALLOC_BIN

#gdb --args ./hello_world
./hello_world
