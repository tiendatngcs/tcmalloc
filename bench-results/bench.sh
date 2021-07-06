BENCH_MARK=$1
TEST_NAME=$2
MEMO=$3

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
	REDIS_SERVER_ID=$!

	# Run profiler
	cd $CURRENT_DIR
	echo $CURRENT_DIR
	./memprofile.sh $REDIS_SERVER_ID $TEST_NAME $MEMO&
	PROFILER_PID=$!

	# Benchmarking
	cd $REDIS_SRC
	sleep 0.1
	if [ $TEST_NAME == "set" ]
	then
		echo populating ...
		./redis-benchmark -t set -c 1000 -r 2000000 -d 1000 -n 200000 -q
	fi
	
	for i in `seq 1 5`
	do
		echo loop $i
		echo "Running test $TEST_NAME"
		if [ $TEST_NAME == "get" ]
		then
			echo populating ...
			./redis-benchmark -t set -c 1000 -r 2000000 -d 1000 -n 200000 -q
		fi
		./redis-benchmark -t $TEST_NAME -c 1000 -r 2000000 -d 1000 -n 200000 -q
		echo -ne dbsize:
		./redis-cli dbsize
		sleep 60
		echo -ne Flushing db 
		./redis-cli flushdb
		echo -ne dbsize:
		./redis-cli dbsize
		echo ------------------------------------------------------
		sleep 60
	done
	echo terminating memprofiler
	kill -15 $REDIS_SERVER_ID
	kill -15 $PROFILER_PID
	
	# get the stat files from the redis src
	cp -avr $REDIS_SRC/stats/ /stats/$TEST_NAME/$MEMO/
elif [ "$BENCH_MARK" == "canneal" ]
then
	echo "Benchmark using caneal"
elif [ "$BENCH_MARK" == "streamcluster" ]
then
	echo "Benchmark using streamcluster"
	cd /home/minh/Desktop/parsec-benchmark/pkgs/kernels/streamcluster/inst/amd64-linux.gcc/bin
fi

