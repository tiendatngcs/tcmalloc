# build the test with debug symbol
bazel build --strip=never main:test

# run
bazel run //main:test
