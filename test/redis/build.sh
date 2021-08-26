echo "Make sure you run below"

echo make MALLOC=libc MALLOC_LIB='-L'$TCMALLOC_BIN' -ltcmalloc'

cd $REDIS_SRC
make MALLOC=libc MALLOC_LIB="-L${TCMALLOC_BIN} -ltcmalloc"
