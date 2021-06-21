# How to set up tcmalloc for redis
## 0. Setup
```shell
    |
    |_ redis
    |  :
    |  :
    |  |_ src
    |     :
    |     :
    |     |_Makefile
    |
    |_ tcmalloc
       :
       :
       |_ test
          |_ redis <you are here>
             :
             :

```
## 1. Build tcmalloc
```shell
../../build.sh
```

## 2. Mod redis Makefile
* go to redis source
```shell
cd ../../../redis/src
```
* open Makefile file
* add the following lines to line 240 in Makefile
```makefile

# Dat mod
ifeq ($(MALLOC), libc)
	FINAL_LIBS+= $(MALLOC_LIB)
endif

```
## 3. Build redis
```shell
./build.sh
```
## 4. Set LD_LIBRARY_PATH as shown on terminal from redis build result
```shell
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:absolute/path/to/tcmalloc/bazel-bin/tcmalloc
echo $LD_LIBRARY_PATH
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