#!/bin/bash
SOLUTION_PATH=/solution
# path to clang++, llvm-dis, and opt
LLVM_BIN=/LLVM_ROOT/build/bin
# path to CSE231.so
LLVM_SO=/LLVM_ROOT/build/lib
# path to lib231.c
LIB_DIR=/lib231
# path to the test directory
TEST_DIR=.

mkdir -p /tmp/part2
$LLVM_BIN/clang -c -O0 $TEST_DIR/test1.c -emit-llvm -S -o /tmp/part2/test1-c.ll
$LLVM_BIN/clang -c -O0 $TEST_DIR/test2-bubble.c -emit-llvm -S -o /tmp/part2/test2-bubble.ll
$LLVM_BIN/clang -c -O0 $TEST_DIR/test3-matrix.c -emit-llvm -S -o /tmp/part2/test3-matrix.ll
$LLVM_BIN/clang -c -O0 $TEST_DIR/test4-fibonacci.c -emit-llvm -S -o /tmp/part2/test4-fibonacci.ll
$LLVM_BIN/clang -c -O0 $TEST_DIR/test5-fibonacci.c -emit-llvm -S -o /tmp/part2/test5-fibonacci.ll
$LLVM_BIN/clang -c -O0 $TEST_DIR/prime.c -emit-llvm -S -o /tmp/part2/test6-prime.ll
# $LLVM_BIN/clang++ -c -O0 $TEST_DIR/test1.cpp -emit-llvm -S -o /tmp/test1.ll
# $LLVM_BIN/clang++ -c $LIB_DIR/lib231.cpp -emit-llvm -S -o /tmp/lib231.ll
# $LLVM_BIN/clang++ -c $TEST_DIR/test1-main.cpp -emit-llvm -S -o /tmp/test1-main.ll
# $LLVM_BIN/clang++ -c -O0 $TEST_DIR/test2.cpp -emit-llvm -S -o /tmp/test2.ll
$LLVM_BIN/opt -load $LLVM_SO/CSE231-DFA.so -cse231-reaching < /tmp/part2/test6-prime.ll > /dev/null 2> /tmp/part2/prime.result
$LLVM_BIN/opt -load $LLVM_SO/CSE231-DFA.so -cse231-reaching < /tmp/part2/test1-c.ll > /dev/null 2> /tmp/part2/reaching.result
$LLVM_BIN/opt -load $LLVM_SO/CSE231-DFA.so -cse231-reaching < /tmp/part2/test2-bubble.ll > /dev/null 2> /tmp/part2/bubble.result
$LLVM_BIN/opt -load $LLVM_SO/CSE231-DFA.so -cse231-reaching < /tmp/part2/test3-matrix.ll > /dev/null 2> /tmp/part2/matrix.result
$LLVM_BIN/opt -load $LLVM_SO/CSE231-DFA.so -cse231-reaching < /tmp/part2/test4-fibonacci.ll > /dev/null 2> /tmp/part2/fib20.result
$LLVM_BIN/opt -load $LLVM_SO/CSE231-DFA.so -cse231-reaching < /tmp/part2/test5-fibonacci.ll > /dev/null 2> /tmp/part2/fib30.result
$LLVM_BIN/opt -load $LLVM_SO/CSE231-DFA.so -cse231-reaching < /LLVM_ROOT/llvm/test/Transforms/SimplifyCFG/multiple-phis.ll 2> /tmp/part2/multiple-phi.result
# $LLVM_BIN/opt -load $LLVM_SO/CSE231.so -cse231-cdi < /tmp/test2.ll -o /tmp/test2-cdi.bc
# $LLVM_BIN/opt -load $LLVM_SO/CSE231.so -cse231-bb < /tmp/test1.ll -o /tmp/test1-bb.bc
# $LLVM_BIN/opt -load $LLVM_SO/CSE231.so -cse231-bb < /tmp/test2.ll -o /tmp/test2-bb.bc
$SOLUTION_PATH/opt -cse231-reaching < /tmp/part2/test6-prime.ll >/dev/null 2> /tmp/part2/sample-prime.result
$SOLUTION_PATH/opt -cse231-reaching < /tmp/part2/test1-c.ll >/dev/null 2> /tmp/part2/sample-reaching.result
$SOLUTION_PATH/opt -cse231-reaching < /tmp/part2/test2-bubble.ll > /dev/null 2> /tmp/part2/sample-bubble.result
$SOLUTION_PATH/opt -cse231-reaching < /tmp/part2/test3-matrix.ll > /dev/null 2> /tmp/part2/sample-matrix.result
$SOLUTION_PATH/opt -cse231-reaching < /tmp/part2/test4-fibonacci.ll > /dev/null 2> /tmp/part2/sample-fib20.result
$SOLUTION_PATH/opt -cse231-reaching < /tmp/part2/test5-fibonacci.ll > /dev/null 2> /tmp/part2/sample-fib30.result
$SOLUTION_PATH/opt -cse231-reaching < /LLVM_ROOT/llvm/test/Transforms/SimplifyCFG/multiple-phis.ll 2> /tmp/part2/sample-multiple-phi.result
# $SOLUTION_PATH/opt -cse231-cdi </tmp/test1.ll  -o /tmp/sam-test1-cdi.bc
# $SOLUTION_PATH/opt -cse231-cdi </tmp/test2.ll  -o /tmp/sam-test2-cdi.bc
# $SOLUTION_PATH/opt -cse231-bb </tmp/test1.ll   -o /tmp/sam-test1-bb.bc
# $SOLUTION_PATH/opt -cse231-bb </tmp/test2.ll   -o /tmp/sam-test2-bb.bc

# $LLVM_BIN/llvm-dis /tmp/test1-cdi.bc
# $LLVM_BIN/llvm-dis /tmp/test2-cdi.bc
# $LLVM_BIN/llvm-dis /tmp/test1-bb.bc
# $LLVM_BIN/llvm-dis /tmp/test2-bb.bc
#
# $LLVM_BIN/llvm-dis /tmp/sam-test1-cdi.bc
# $LLVM_BIN/llvm-dis /tmp/sam-test2-cdi.bc
# $LLVM_BIN/llvm-dis /tmp/sam-test1-bb.bc
# $LLVM_BIN/llvm-dis /tmp/sam-test2-bb.bc
#
#
# $LLVM_BIN/clang++ /tmp/lib231.ll /tmp/test2-cdi.bc /tmp/test1-cdi.ll /tmp/test1-main.ll -o /tmp/cdi_test1
# $LLVM_BIN/clang++ /tmp/lib231.ll /tmp/test2-cdi.bc /tmp/test1-bb.ll /tmp/test1-main.ll -o /tmp/bb_test1
#
# $LLVM_BIN/clang++ /tmp/lib231.ll /tmp/sam-test2-cdi.bc /tmp/test1-cdi.ll /tmp/test1-main.ll -o /tmp/sam_cdi_test1
# $LLVM_BIN/clang++ /tmp/lib231.ll /tmp/sam-test2-cdi.bc /tmp/test1-bb.ll /tmp/test1-main.ll -o /tmp/sam_bb_test1
#
# /tmp/cdi_test1 2> /tmp/cdi.result
# /tmp/bb_test1 2> /tmp/bb.result
# /tmp/sam_cdi_test1 2> /tmp/sam_cdi.result
# /tmp/sam_bb_test1 2> /tmp/sam_bb.result
