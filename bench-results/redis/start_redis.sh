REDIS_SRC="/home/minh/Desktop/redis/src"
cd $REDIS_SRC
./redis-server&
./redis-cli config set save ""
./redis-cli config set appendonly no