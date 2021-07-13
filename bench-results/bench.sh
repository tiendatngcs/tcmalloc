BENCH_MARK=$1
TEST_NAME=$2
MEMO=$3
MALLOC_SIZE=$4

# redis: PING_INLINE, PINT_BULK, SET, GET, INCR, SADD, Lrange_300, LRANFE_600, LPUSH, RPUSH,
# LPOP, RPOP, MSET, LPUSH, LRANGE

CURRENT_DIR=$(pwd)
mkdir stats

save_stat_file() {
	# get the stat files
	echo "moving stat files"
	cd $CURRENT_DIR
	cd stats
	mkdir $TEST_NAME
	cd $TEST_NAME
	# check if the stat dir for this test had been created, if so we deleted it
	mkdir $MEMO
	if [ $? -ne 0 ]
	then
		rm -fr ./$MEMO
    	mkdir $MEMO
	fi
	cd $MEMO
	mv $REDIS_SRC/stats/* .
}

populate() {
	echo "Populating"
}

redis_set() {
	./redis-benchmark -t $TEST_NAME -c 1000 -r 2000000 -d $MALLOC_SIZE -n 1 -q
	echo -ne "dbsize: "
	./redis-cli dbsize
	sleep 30
	echo -ne Flushing db
	./redis-cli flushdb
	echo -ne "dbsize: "
	./redis-cli dbsize
	echo "------------------------------------------------------"
	sleep 30
}

kill_process() {
	echo "terminating memprofiler"
	kill -15 $REDIS_SERVER_PID
	kill -15 $PROFILER_PID
}
############
### MAIN ###
############
# for redis
if [ $BENCH_MARK = "redis" ]
then
	# redis-server is compiled with tcmalloc
	REDIS_SRC="/home/minh/Desktop/redis/src"

	#Run redis server
	cd $REDIS_SRC
	./redis-server&
	REDIS_SERVER_PID=$!

	# Run profiler
	cd $CURRENT_DIR
	./memprofile.sh $TEST_NAME $MEMO&
	PROFILER_PID=$!

	# Benchmarking
	cd $REDIS_SRC
	# for i in `seq 1 5`
	# do
	# 	echo "loop" $i
	echo "Running test $TEST_NAME"
	if [ $TEST_NAME = "set" ]
	then
		redis_set
	fi
	# done

	kill_process

	save_stat_file
elif [ $BENCH_MARK = "firefox" ]
then
	echo "Benchmark using firefox"
fi
