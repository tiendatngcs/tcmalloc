echo "Make sure you run below"

echo make USE_JEMALLOC=no MALLOC_LIB='-L'$TCMALLOC_BIN' -ltcmalloc'

cd $SRC_DIR
make USE_JEMALLOC=no MALLOC_LIB="-L${TCMALLOC_BIN} -ltcmalloc"

echo export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$TCMALLOC_BIN
