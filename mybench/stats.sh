TEST_NAME=$1
MEMO=$2
PROFILE=$3

# put the stat files in the correct folder
cd stats
CURRENT_DIR=$(pwd)
mkdir $TEST_NAME
cd $TEST_NAME
mkdir $PROFILE
cd $PROFILE
mkdir $MEMO
if [ $? -ne 0 ]
then
    rm -fr ./$MEMO
    mkdir $MEMO
fi
cd $MEMO
mv $CURRENT_DIR/*.txt .
