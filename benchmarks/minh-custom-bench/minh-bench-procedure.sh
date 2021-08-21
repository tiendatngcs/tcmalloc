START_DATE=$(date)
#build tcmalloc
../../build.sh
#build bechmark
./build.sh
#run profile
./hello_world
#run python visualizer
# python3 ../bench_stats.py
echo Experiment Starts at $START_DATE
echo Experiment Ends at $(date)