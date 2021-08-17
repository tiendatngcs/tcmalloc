#!/bin/bash
cd $REDIS_SRC

x=1
NUM=(1 + $RANDOM % 1000)
MALLOC_SIZE=(1 + $RANDOM % 100)
./redis-server& 
sleep 5

while [ $x -le 500 ]
do
    deallocate=(1 + $RANDOM % 10)
    if [ $deallocate -ge 5 ]
    then
        ./redis-cli minhrandomkey $NUM  $MALLOC_SIZE
    else
        ./redis-benchmark -t SET -r 3000000 -n $NUM -d $MALLOC_SIZE -q
        echo "----------------------------------------------------------"
        x=($x + 1)
        sleep 3
    fi
done
