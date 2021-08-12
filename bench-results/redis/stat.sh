TEST_NAME=$1
RELEASERATE=$2
PROFILE=$3
DRAINCHECKCYCLE=$4

CURRENT_DIR=$(pwd)

echo "moving stat files"
cd $CURRENT_DIR
mkdir stats
cd stats
mkdir $PROFILE
cd $PROFILE
mkdir $TEST_NAME
cd $TEST_NAME
mkdir $RELEASERATE
cd $RELEASERATE
mkdir $DRAINCHECKCYCLE
if [ $? -ne 0 ]
then
    rm -fr ./$DRAINCHECKCYCLE
    mkdir $DRAINCHECKCYCLE
fi
cd $DRAINCHECKCYCLE
mv ${REDIS_SRC}/stats/* .