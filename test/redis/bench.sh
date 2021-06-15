#redis-server is compiled with tcmalloc
SRC_DIR=../../../redis/src
./memprofile.sh&
PROFILER_PID=$!
cd $SRC_DIR
sleep 0.1
echo populating ...
./redis-benchmark -t set -c 1000 -r 2000000 -d 1000 -n 400000 -q
#./redis-cli dbsize
#./redis-cli --scan --pattern '*'
#./redis-cli llen mylist
for i in `seq 1 5`
do
  echo loop $i
  #./redis-benchmark -c 1000 -r 10000 -n 1000 lpush mylist "test:__rand_int__"
  #./redis-benchmark -c 1000 -n 1000 lpop mylist
  #./redis-benchmark -t sadd -r 1000000 -c 1000 -d 1000 -n 1000 -q
  #./redis-benchmark -t spop -c 1000 -n 1000
  ./redis-benchmark -t set -c 1000 -r 2000000 -d 1000 -n 400000
  echo dbsize:
  ./redis-cli dbsize
  sleep 5
  #./redis-benchmark -q -c 1000 -n 400000 -r 1000000 del "key:__rand_int__"
  echo flushing db ...
  ./redis-cli flushdb
  echo dbsize:
  ./redis-cli dbsize
  sleep 2
  echo ------------------------------------------------------
done
echo terminating memprofiler
kill -15 $PROFILER_PID