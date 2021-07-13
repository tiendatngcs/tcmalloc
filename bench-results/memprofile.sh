SET_UP=$1
TEST_NAME=$2
RELEASE_RATE=$3

TIME_STAMP=$(date +%m_%d_%y)
CURRENT_DIR=$(pwd)

if [ $SET_UP = "true" ] 
then
    mkdir smem
    cd smem
    mkdir $TEST_NAME
    cd $TEST_NAME
    mkdir $RELEASE_RATE
    cd $CURRENT_DIR
    SMEM_FILE=./smem/$TEST_NAME/$RELEASE_RATE/$TIME_STAMP.txt
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
    smem -P redis-server -c "name rss pss uss" | head -n 1 >> $SMEM_FILE
elif [ $SET_UP = "false" ]
then
    SMEM_FILE=./smem/$TEST_NAME/$RELEASE_RATE/$TIME_STAMP.txt
    smem --processfilter="redis-server" -c "name rss pss uss" | grep redis-server >> $SMEM_FILE
fi
