TEST_NAME=$1
MEMO=$2

CURRENT_DIR=$(pwd)
#Run redis server
cd $REDIS_SRC
./redis-server&
REDIS_SERVER_ID=$!
#Run profiler
cd $CURRENT_DIR
./memprofile.sh $REDIS_SERVER_ID $TEST_NAME $MEMO&
PROFILER_PID=$!
#Benchmarking
cd $REDIS_SRC
sleep 0.1
echo populating ...
./redis-benchmark -t set -c 1000 -r 2000000 -d 1000 -n 200000 -q
#./redis-cli dbsize
#./redis-cli --scan --pattern '*'
#./redis-cli llen mylist
for i in `seq 1 5`
do
  echo loop $i
  echo "Running test $TEST_NAME"
  ./redis-benchmark -t $TEST_NAME -c 1000 -r 2000000 -d 1000 -n 200000 -q
  echo dbsize:
  ./redis-cli dbsize
  sleep 1
  echo Flushing db
  ./redis-cli flushdb
  echo dbsize:
  ./redis-cli dbsize
  sleep 1
  echo ------------------------------------------------------
done
echo terminating memprofiler
kill -15 $REDIS_SERVER_ID
kill -15 $PROFILER_PID