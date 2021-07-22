#!/bin/bash
TEST=$1
MALLOC_SIZE=$2
NUM=$3

CURRENT_DIR=$(pwd)
REDIS_SRC="/home/minh/Desktop/redis/src"
cd $REDIS_SRC
./redis-benchmark -t $TEST -c 1000 -r $NUM -d $MALLOC_SIZE -n $NUM -q
sleep 5
echo -ne "dbsize: "; ./redis-cli dbsize
if [ $TEST != "LPUSH" ] && [ $TEST != "RPUSH" ]
then
    echo "Deallocate"
else
    if [ "$TEST" = "LPUSH" ]
    then
        echo "Deallocate using LPOP"
        ./redis-benchmark -t LPOP -c 1000 -r $NUM -d $MALLOC_SIZE -n $NUM -q
    else
        echo "Deallocate using RPOP"
        ./redis-benchmark -t RPOP -c 1000 -r $NUM -d $MALLOC_SIZE -n $NUM -q
    fi
fi
# cd $REDIS_SRC
# echo -ne "purging memory "; ./redis-cli MEMORY PURGE
# sleep 5
# echo -ne "dbsize: "; ./redis-cli dbsize
echo "------------------------------------------"
