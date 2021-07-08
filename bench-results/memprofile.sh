TEST_NAME=$1
MEMO=$2

TIME_STAMP=$(date +%m%d%y_%H%M%S)
CURRENT_DIR=$(pwd)

mkdir smem
cd smem
mkdir $TEST_NAME
cd $TEST_NAME
mkdir $MEMO

cd $CURRENT_DIR
SMEM_FILE=./smem/$TEST_NAME/$MEMO/$TIME_STAMP.txt

touch $SMEM_FILE

echo $TIME_STAMP | tee -a $SMEM_FILE
echo "Test: $TEST_NAME" | tee -a $SMEM_FILE
echo $MEMO | tee -a $SMEM_FILE

if [ "$TEST_NAME" == "get" -o "$TEST_NAME" == "set" ]
then
    smem -P redis-server -c "name rss pss uss" | head -n 1 >> $SMEM_FILE
    for i in `seq 1 1000`
    do
        smem -P redis-server -c "name rss pss uss" | grep redis-server >> $SMEM_FILE
        sleep 1
    done
elif [ "$TEST_NAME" == "streamcluster" -o "$TEST_NAME" == "canneal" ]
then
    smem -P $TEST_NAME -c "name rss pss uss" | head -n 1 >> $SMEM_FILE
    for i in `seq 1 1000`
    do
        smem -P $TEST_NAME -c "name rss pss uss" | grep $TEST_NAME >> $SMEM_FILE
        sleep 1
    done
fi
