#!/bin/bash
NUM=$1
MALLOC_SIZE=$2
cd $REDIS_SRC
./redis-cli minhrandomkey $NUM  $MALLOC_SIZE