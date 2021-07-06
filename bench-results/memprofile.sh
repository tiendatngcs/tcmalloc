REDIS_SERVER_ID=$1
TEST_NAME=$2
MEMO=$3
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
smem -P redis-server -c "name rss pss uss" | head -n 1 >> $SMEM_FILE
for i in `seq 1 1000`
do
    smem -P redis-server -c "name rss pss uss" | grep redis-server >> $SMEM_FILE
    sleep 1
done
echo ------------------------------------------------------------------ >> $FILE
