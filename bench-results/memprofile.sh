TEST_SUITE=$1
TEST_NAME=$2
RELEASE_RATE=$3
PROFILE=$4
DRAINCHECKCYCLE=$5

TIME_STAMP=$(date +%m_%d_%y)
CURRENT_DIR=$(pwd)

echo $TEST_NAME

mkdir smem
cd smem
mkdir $PROFILE
cd $PROFILE
mkdir $TEST_NAME
cd $TEST_NAME
mkdir $RELEASE_RATE
cd $RELEASE_RATE
mkdir $DRAINCHECKCYCLE
cd $CURRENT_DIR
SMEM_FILE=./smem/$PROFILE/$TEST_NAME/$RELEASE_RATE/$DRAINCHECKCYCLE/$TIME_STAMP.txt
# check if file exists, if yes then remove
if [ -e $SMEM_FILE ]
then
    echo "Delete old stat file."
    rm -f $SMEM_FILE
fi
touch $SMEM_FILE
echo $TEST_NAME >> $SMEM_FILE
echo $RELEASE_RATE >> $SMEM_FILE
echo $TIME_STAMP >> $SMEM_FILE
if [ "$TEST_SUITE" = "redis" ]
then
    printf "%s\t\t%s\t\t\t%s\t\t%s\t\t%s\n" "Time" $(smem --processfilter="redis-server" -c "name rss pss uss" | head -n 1) >> $SMEM_FILE
    while true
    do
        printf "%s\t%s\t%s\t%s\t%s\n" $(date +"%T") $(smem --processfilter="redis-server" -c "name rss pss uss" | grep redis-server | head -1) >> $SMEM_FILE
        sleep 1
    done
elif [ "$TEST_SUITE" = "mybench" ]
then
    printf "%s\t\t%s\t\t\t%s\t\t%s\t\t%s\n" "Time" $(smem --processfilter="hello_world" -c "name rss pss uss" | head -n 1) >> $SMEM_FILE
    while true
    do
        printf "%s\t%s\t\t%s\t%s\t%s\n" $(date +"%T") $(smem --processfilter="hello_world" -c "name rss pss uss" | grep hello_world | head -1) >> $SMEM_FILE
        sleep 1
    done
fi