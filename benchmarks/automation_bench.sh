# this script will run the bench_profile in redis-custom-bench and minh-custom-bench
BENCHMARK=("Producer-Consumer")
RELEASE_RATE=("0MB" "1MB" "10MB")
TEST_NAME=("SET")
PROFILE_NAME=("Beta" "Bravo" "Charlie" "Delta" "Echo" "Foxtrot" "Merced" "Sierra" "Sigma" "Uniform")
DRAIN_CYCLE=("0s" "5s")
CURRENT_DIR=$(pwd)

# update redis bench
echo -n "Building Redis bench..."
cd redis-custom-bench/
./build_bench.sh
REDIS_BENCH_DIR=$(pwd)
echo " Done"

# build producer-consumer
echo -n "Building Producer-Consumer bench..."
cd $CURRENT_DIR
cd minh-custom-bench/
./build.sh
PROD_CONS_BENCH_DIR=$(pwd)
echo " Done"


cd $CURRENT_DIR
for bench in "${BENCHMARK[@]}"
do
    for release_rate in "${RELEASE_RATE[@]}"
    do
        # setting up the first half of the command. In redis, we need 
        # to specify the test name so we need to take care of that
        if [ $bench = "redis" ]
        then
            for test_name in "${TEST_NAME[@]}"
            do
                temp="$bench $release_rate $test_name"
            done
        else
            temp="$bench $release_rate"
        fi

        for profile in "${PROFILE_NAME[@]}"
        do
            for drain_cycle in "${DRAIN_CYCLE[@]}"
            do
                run="$temp $profile $drain_cycle"
                echo "Running: $bench $release_rate $test_name $profile $drain_cycle"
                if [ $bench = "redis" ]
                then
                    cd $REDIS_BENCH_DIR/
                    ./bench_profile $run
                else
                    cd $PROD_CONS_BENCH_DIR/
                    ./hello_world $run
                fi
                echo "Done"
                echo "-----------------------------------------------------------------------------------------"
            done
        done
    done
done
