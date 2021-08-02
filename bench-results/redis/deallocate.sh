#!/bin/bash
NUM=$1
MALLOC_SIZE=$2
TEST=$3
REDIS_SRC="/home/minh/Desktop/redis/src"
cd $REDIS_SRC
if [ $TEST = "SET" ]
then
    ./redis-cli minhrandomkey $NUM $MALLOC_SIZE
elif [ $TEST = "LPOP" -o $TEST = "RPOP" ]
then
    ./redis-cli $TEST mylist
fi