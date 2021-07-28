CURRENT_DIR=$(pwd)
cd ..
TCMALLOC_BIN=$(pwd)/bazel-bin/tcmalloc
echo $TCMALLOC_BIN
cd $CURRENT_DIR
g++ hello_world.cc -std=c++17 -c -g -o hello_world.o -Wall -Wextra
g++ hello_world.o -std=c++17 -o hello_world -L$TCMALLOC_BIN -ltcmalloc -lpthread -Wall -Wextra

