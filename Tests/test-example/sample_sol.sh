#/!bin/bash
LLVM_BIN=/LLVM_ROOT/build/bin
SOLUTION_PATH=/solution
TEST_PATH=/tests/test-example
LIB_DIR=/lib231

$SOLUTION_PATH/opt -cse231-csi < $TEST_PATH/test2.bc >/dev/null 2> /tmp/sample-csi.result
opt -load CSE231.so -cse231-bb < $TEST_PATH/test2.bc >/dev/null 2> /tmp/mysample-csi.result

$SOLUTION_PATH/opt -cse231-bb < $TEST_PATH/test2.bc -o /tmp/sample-bb.bc
opt -load CSE231.so -cse231-bb < $TEST_PATH/test2.bc -o /tmp/mysample-bb.bc

$SOLUTION_PATH/opt -cse231-cdi < $TEST_PATH/test2.bc -o /tmp/sample-cdi.bc
opt -load CSE231.so -cse231-cdi < $TEST_PATH/test2.bc -o /tmp/mysample-cdi.bc

$LLVM_BIN/llvm-dis /tmp/sample-bb.bc
$LLVM_BIN/llvm-dis /tmp/mysample-bb.bc
$LLVM_BIN/llvm-dis /tmp/sample-cdi.bc
$LLVM_BIN/llvm-dis /tmp/mysample-cdi.bc

$LLVM_BIN/clang++ /tmp/lib231.ll /tmp





