#!/bin/bash

cd $REDIS_SRC
make uninstall
make distclean
make clean