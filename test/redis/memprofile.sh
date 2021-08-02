REDIS_SERVER_ID=$1
TEST_NAME=$2
MEMO=$3
TIME_STAMP=$(date +%m%d%y_%H%M%S)

mkdir smem
mkdir pmap

SMEM_FILE=./smem/malloc_profile_$TIME_STAMP.txt
PMAP_FILE=./pmap/malloc_profile_$TIME_STAMP.txt
touch $SMEM_FILE $PMAP_FILE


echo $TIME_STAMP | tee -a $SMEM_FILE $PMAP_FILE
echo "Test: $TEST_NAME" | tee -a $SMEM_FILE $PMAP_FILE
echo $MEMO | tee -a $SMEM_FILE $PMAP_FILE
smem -P redis-server -c "name rss pss uss" | head -n 1 >> $SMEM_FILE
pmap $REDIS_SERVER_ID -x | sed -n '2 p' >> $PMAP_FILE
for i in `seq 1 1000`
do
    smem -P redis-server -c "name rss pss uss" | grep redis-server >> $SMEM_FILE
    pmap $REDIS_SERVER_ID -x | tail -n 1 >> $PMAP_FILE
    sleep 1
done
echo ------------------------------------------------------------------ | tee -a $SMEM_FILE $PMAP_FILE