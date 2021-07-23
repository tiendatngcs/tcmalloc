#!/bin/bash
MALLOC_SIZE=$1
NUM=$2

REDIS_SRC="/home/minh/Desktop/redis/src"

cd $REDIS_SRC
./redis-benchmark -t set -c 1000 -r 2000000 -d $MALLOC_SIZE -n $NUM -q
echo -ne "dbsize: "; ./redis-cli dbsize
