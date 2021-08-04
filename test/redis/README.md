# How to set up tcmalloc for redis
## 0. Setup
Requirements
```
redis
tcmalloc
make
bazel
smem
```
## 1. Build tcmalloc
```shell
../../build.sh
```
## 2. Set environemnt variables
Add the following lines to ~/.bashrc
```shell
export TCMALLOC_BIN="/path/to/tcmalloc/bazel-bin/tcmalloc"
export REDIS_SRC="/path/to/redis/src"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$TCMALLOC_BIN"
```
## 3. Mod redis Makefile
* go to redis source
```shell
cd $REDIS_SRC
```
* open Makefile file
* add the following lines to line 235 in Makefile
```makefile

# Dat mod
ifeq ($(MALLOC), libc)
	FINAL_LIBS+= $(MALLOC_LIB)
endif
# Dat mod ends

```
## 4. Build redis
```shell
./build.sh
```
## 5. Run redis-server
```shell
../../../redis/src/redis-server
```

# Bonus
## Run benchmark script
```shell
./bench.sh set
```
## Graph stats
```shell
python3 mem_graph.py
```