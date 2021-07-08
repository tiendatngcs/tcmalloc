BENCH_MARK=$1
TEST_NAME=$2
MEMO=$3

# redis: PING_INLINE, PINT_BULK, SET, GET, INCR, SADD, Lrange_300, LRANFE_600, LPUSH, RPUSH,
# LPOP, RPOP, MSET, LPUSH, LRANGE

mkdir stats
CURRENT_DIR=$(pwd)

# Get path to tcmalloc
cd ../bazel-bin/tcmalloc
TCMALLOC_BIN=$(pwd)

# for redis
if [ "$BENCH_MARK" == "redis" ]
then
	# redis-server is compiled with tcmalloc
	REDIS_SRC="/home/minh/Desktop/redis/src"

	#Run redis server
	cd $REDIS_SRC
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$TCMALLOC_BIN
	./redis-server&
	REDIS_SERVER_PID=$!

	# Run profiler
	cd $CURRENT_DIR
	./memprofile.sh $TEST_NAME $MEMO&
	PROFILER_PID=$!

	# Benchmarking
	cd $REDIS_SRC
	sleep 0.1
	echo populating ...
	./redis-benchmark -t set -c 1000 -r 2000000 -d 1000 -n 20000 -q
	
	for i in `seq 1 5`
	do
		echo loop $i
		echo "Running test $TEST_NAME"
		./redis-benchmark -t $TEST_NAME -c 1000 -r 2000000 -d 1000 -n 20000 -q
		echo -ne dbsize:
		./redis-cli dbsize
		sleep 30
		if [ $TEST_NAME == "set" ]
		then
			echo -ne Flushing db
			./redis-cli flushdb
		fi
		echo -ne dbsize:
		./redis-cli dbsize
		echo ------------------------------------------------------
		sleep 30
	done
	echo terminating memprofiler
	kill -15 $REDIS_SERVER_PID
	kill -15 $PROFILER_PID
	
	# get the stat files
	echo moving stat files
	cd $CURRENT_DIR
	cd stats
	mkdir $TEST_NAME
	cd $TEST_NAME

	mkdir $MEMO
	if [ $? -ne 0 ]
	then
		rm -fr ./$MEMO
    	mkdir $MEMO
	fi

	cd $MEMO
	mv $REDIS_SRC/stats/* .
elif [ "$BENCH_MARK" == "firefox" ]
then
	echo Benchmark using firefox

	# cd $CURRENT_DIR
	# ./memprofile.sh $TEST_NAME $MEMO&
	# PROFILER_PID=$!
	
	# wait
	# kill -15 $CANNEAL_PID
	# kill -15 $PROFILER_PID

	# get the stat files
	# echo moving stat files
	# cd $CURRENT_DIR
	# cd stats
	# mkdir $TEST_NAME
	# cd $TEST_NAME
	# mkdir $MEMO
	# cd $MEMO
	# mv /home/minh/Desktop/parsec-benchmark/pkgs/kernels/canneal/inst/amd64-linux.gcc/bin/stats/* .
fi
