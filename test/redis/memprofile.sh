FILE=malloc_profile.txt
touch $FILE
date >> $FILE
for i in `seq 1 1000`
do
    echo 1 >> $FILE
    free --kilo >> $FILE
    sleep 1
done
echo ------------------------------------------------------------------ >> $FILE