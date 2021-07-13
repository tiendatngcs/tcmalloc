TEST=$1
MALLOC_SIZE=$2
NUM=$3

REDIS_SRC="/home/minh/Desktop/redis/src"
cd $REDIS_SRC

# available_test: PING_INLINE, PINT_BULK, SET, GET, INCR, SADD, Lrange_300, LRANFE_600, LPUSH, RPUSH,
# LPOP, RPOP, MSET, LPUSH, LRANGE

if [ $TEST = "set" ]
then
    ./redis-benchmark -t $TEST -c 1000 -r 2000000 -d $MALLOC_SIZE -n $NUM -q
    echo -n "dbsize: "; ./redis-cli dbsize
    echo -n "flushing db "; ./redis-cli flushdb
    echo -n "dbsize: "; ./redis-cli dbsize
    echo "------------------------------------------"
else
     ./redis-benchmark -t $TEST -c 1000 -r 2000000 -d $MALLOC_SIZE -n $NUM -q
fi