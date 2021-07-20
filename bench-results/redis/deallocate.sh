#!/bin/bash
END=$1

REDIS_SRC="/home/minh/Desktop/redis/src"
cd $REDIS_SRC

declare -A KeyArray
for i in $(seq 0 $END)
do
    ./redis-cli randomkey
    KeyArray[$i]=$?
done