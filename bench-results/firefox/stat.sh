$MEMO=$1

echo "moving stat files"
cd stats
mkdir firefox
cd firefox
mkdir $MEMO
cd $MEMO
mv /home/minh/Desktop/tcmalloc/stats/* .