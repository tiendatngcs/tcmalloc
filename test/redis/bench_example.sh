#redis-server is compiled with tcmalloc
SRC_DIR=../../../redis
REDIS_DIR=$(pwd)
cd $SRC_DIR
make
cd $REDIS_DIR
cp $SRC_DIR/src/redis-server ./
export LD_LIBRARY_PATH=../../bazel-bin/tcmalloc
./redis-server&
PID=$!
#echo -e "\033[31m$PID\033[0m"
sleep 0.1
./redis-benchmark -t set -c 1000 -r 2000000 -d 1000 -n 400000 -q
#./redis-cli dbsize
#./redis-cli --scan --pattern '*'
#./redis-cli llen mylist
for i in `seq 1 5`
do
  #./redis-benchmark -c 1000 -r 10000 -n 1000 lpush mylist "test:__rand_int__"
  #./redis-benchmark -c 1000 -n 1000 lpop mylist
  #./redis-benchmark -t sadd -r 1000000 -c 1000 -d 1000 -n 1000 -q
  #./redis-benchmark -t spop -c 1000 -n 1000
  ./redis-benchmark -t set -c 1000 -r 2000000 -d 1000 -n 400000 -q
  ./redis-cli dbsize
  sleep 5
  #./redis-benchmark -q -c 1000 -n 400000 -r 1000000 del "key:__rand_int__"
  ./redis-cli flushdb
  ./redis-cli dbsize
  sleep 2
done
#sleep 15
kill -15 $PID
rm dump.rdb