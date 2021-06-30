#!/bin/bash
bazel clean
bazel build //tcmalloc:libtcmalloc.so --verbose_failures
