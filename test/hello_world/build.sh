CURRENT_DIR=$(pwd)
cd ../../
TCMALLOC_BIN=$(pwd)/bazel-bin/tcmalloc
cd $CURRENT_DIR

g++ hello_world.cc -std=c++17 -c -g -o hello_world.o -Wall -Wextra
g++ hello_world.o -std=c++17 -o hello_world -L/home/minh/Desktop/tcmalloc/bazel-bin/tcmalloc -ltcmalloc -Wall -Wextra
echo export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$TCMALLOC_BIN

