#!/usr/bin/env bash
#  !bash
# type './build.sh'  for release build
# type './build.sh debug'  for debug build

#########
# Please change the following home directories of your LLVM builds
########

# Install SVF if not exist
if [ -z "$SVF_DIR" ]; then
  # Download SVF
  npm i --silent svf-lib --prefix ${HOME}
  # Set environmental parameters
  install_path=$(npm root)
  LLVMHome="llvm-13.0.0.obj"
  Z3Home="z3.obj"
  export LLVM_DIR=$install_path/$LLVMHome
  export Z3_DIR=$install_path/$Z3Home
  export PATH=$LLVM_DIR/bin:$PATH
  export SVF_DIR=$install_path/SVF/

  echo "LLVM_DIR="$LLVM_DIR
  echo "SVF_DIR="$SVF_DIR
  echo "Z3_DIR="$Z3_DIR
fi

# Build POCR
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

