#build tcmalloc
../../build.sh
#build bechmark
./build_bench.sh
#run profile
./run.sh
#run python visualizer
python3 ../bench_stats.py
