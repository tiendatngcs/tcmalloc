TEST_NAME=$1
MEMO=$2

CURRENT_DIR=$(pwd)
REDIS_SRC="/home/minh/Desktop/redis/src"

echo "moving stat files"
cd $CURRENT_DIR
mkdir stats
cd stats
mkdir $TEST_NAME
cd $TEST_NAME
# check if the stat dir for this test had been created, if so we deleted it
mkdir $MEMO
if [ $? -ne 0 ]
then
    rm -fr ./$MEMO
    mkdir $MEMO
fi
cd $MEMO
mv $REDIS_SRC/stats/* .