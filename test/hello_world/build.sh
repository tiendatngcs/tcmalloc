#!/bin//bash

g++ hello_world.cc -c -g -o hello_world.o
g++ hello_world.o -o hello_world -L../../bazel-bin/tcmalloc -ltcmalloc


g++ hello_world_.c -c -g -o hello_world_.o
g++ hello_world_.o -o hello_world_ -L/home/tiendatngcs/Documents/github_repos/tcmalloc/bazel-bin/tcmalloc -ltcmalloc
echo "Please set LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:<path/to/libtcmalloc.so>"