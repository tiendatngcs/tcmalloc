echo "Make sure you run below"

echo make USE_JEMALLOC=no MALLOC_LIB='-L'$TCMALLOC_BIN' -ltcmalloc'

cd $REDIS_SRC
make USE_JEMALLOC=no MALLOC_LIB="-L${TCMALLOC_BIN} -ltcmalloc"
