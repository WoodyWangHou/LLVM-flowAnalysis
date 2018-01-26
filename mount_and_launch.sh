#!/bin/bash

SOURCE_PATH=`pwd`/Passes
TESTS_PATH=`pwd`/Tests
OUTPUT_PATH=`pwd`/Output
LIB_PATH=`pwd`/Lib231

# MOUNT POINTS IN CONTAINER: 
# --------------------------
#
# SOURCE CODE: /LLVM_ROOT/llvm/lib/Transforms/CSE231_Project
# TESTS: /tests
# OUTPUT: /output
# LIB: /lib231

docker run --rm -it -v ${SOURCE_PATH}:/LLVM_ROOT/llvm/lib/Transforms/CSE231_Project -v ${TESTS_PATH}:/tests -v ${OUTPUT_PATH}:/output -v ${LIB_PATH}:/lib231 prodromou87/llvm:5.0.1
