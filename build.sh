#!/usr/bin/env bash
#  !bash
# type './build.sh'  for release build
# type './build.sh debug'  for debug build

#########
# Please change the following home directories of your LLVM builds
########
LLVMRELEASE=$LLVM_OBJ
LLVMDEBUG=$LLVM_OBJ

export PATH=$LLVM_DIR/bin:$PATH


if [[ $1 == 'debug' ]]
then
Type='Debug'
else
Type='Release'
fi

Build=$Type'-build'

rm -rf $Build
mkdir $Build
cd $Build

if [[ $1 == 'debug' ]]
then
cmake -D CMAKE_BUILD_TYPE:STRING=Debug ../
else
cmake ../
fi



make -j4

