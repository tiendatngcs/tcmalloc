#!/bin//bash

g++ hello_world.cc -g -o hello_world -I../../tcmalloc -L/home/tiendatngcs/Documents/github_repos/tcmalloc/bazel-bin/tcmalloc -ltcmalloc
echo "Please set LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:<path/to/libtcmalloc.so>"