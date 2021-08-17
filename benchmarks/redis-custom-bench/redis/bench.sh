#!/bin/bash
TEST=$1
MALLOC_SIZE=$2
NUM=$3
CURRENT_DIR=$(pwd)

cd $REDIS_SRC
./redis-benchmark -t $TEST -r 3000000 -n $NUM -d $MALLOC_SIZE -q
echo "----------------------------------------------------------"
