https://github.com/cirosantilli/parsec-benchmark

https://parsec.cs.princeton.edu/download/tutorial/2.0/parsec-2.0-tutorial.pdf

# IMPORTANT
1. To include the tcmalloc lib, make sure to update the .bashrc in ~ with tcmalloc dir.
	export LD_LIBRARY_PATH="/home/minh/Desktop/tcmalloc/bazel-bin/tcmalloc:$LD_LIBRARY_PATH"
	
2. Update in config/gcc.bldconf:
	line 43 export CXXFLAGS="$CXXFLAGS -O0 -g -fpermissive -fno-exceptions"
	line 46 export LIBS="-L/home/minh/Desktop/tcmalloc/bazel-bin/tcmalloc -ltcmalloc"
	
3. Compile with gcc config
	#start
		source env.sh
	# build
		parsecmgmt -a build -p canneal -c gcc
	# run bench
		parsecmgmt -a run -p canneal -c gcc -i simlarge

	# gdb
		extract the .nets file from the /input folder for test
		gdb --args /home/minh/Desktop/parsec-benchmark/pkgs/kernels/canneal/inst/amd64-linux.gcc/bin/canneal 1 10000 2000 400000.nets 32
		b TCMallocInternalMalloc

	# clean
		parsecmgmt -a fulluninstall -p all

4. Go to pkgs/kernels/canneal/inst/amd64-linux.gcc/bin, use ldd canneal to check for libtcmalloc and its location
