TEST_NAME=$1
MEMO=$2

# put the stat files in the correct order
cd stats
CURRENT_DIR=$(pwd)
mkdir $TEST_NAME
cd $TEST_NAME
mkdir $MEMO
if [ $? -ne 0 ]
then
    rm -fr ./$MEMO
    mkdir $MEMO
fi
cd $MEMO
mv $CURRENT_DIR/*.txt .
