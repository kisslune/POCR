#!/usr/bin/env bash
echo "Setting up environment for PTA"


#########
# Please change LLVM_OBJ_ROOT before using it
########

export LLVM_OBJ_ROOT=$LLVM_OBJ

export PATH=$LLVM_OBJ_ROOT/bin:$PATH
export LLVM_DIR=$LLVM_OBJ_ROOT
export LLVMOPT=opt
export CLANG=$LLVM_OBJ_ROOT/bin/clang
export CLANGCPP=$LLVM_OBJ_ROOT/bin/clang++
export LLVMDIS=llvm-dis
export LLVMLLC=llc


##############check what os we have
PLATFORM='unknown'
unamestr=`uname`
if [[ "$unamestr" == 'Linux' ]]; then
export PLATFORM='linux'
elif [[ "$unamestr" == 'Darwin' ]]; then
export PLATFORM='darwin'
elif [[ "$unamestr" == 'FreeBSD' ]]; then
export PLATFORM='freebsd'
fi


#########PATH FOR PTA##############
export PTAHOME=`pwd`
if [[ $1 == 'debug' ]]
then
PTAOBJTY='Debug'
else
PTAOBJTY='Release'
fi
Build=$PTAOBJTY'-build'
export PTABIN=$PTAHOME/$Build/bin
export PTALIB=$PTAHOME/$Build/lib
export PTARTLIB=$PTAHOME/lib/RuntimeLib
export PATH=$PTABIN:$PATH


### for mac 10.10.1###
rm -rf $PTALIB/liblib*
for file in $(find $PTALIB -name "*.dylib")
do
    basefilename=`basename $file`
    newfile=`echo $basefilename | sed s/lib/liblib/`
    ln -s $PTALIB/$basefilename $PTALIB/$newfile
done
