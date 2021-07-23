#!/bin/bash
NUM=$1
MALLOC_SIZE=$2

REDIS_SRC="/home/minh/Desktop/redis/src"
cd $REDIS_SRC
./redis-cli minhrandomkey $NUM $MALLOC_SIZE
