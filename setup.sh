#!/usr/bin/env bash
echo "Setting up environment for POCR"

export PTAHOME=`pwd`
if [[ $1 == 'debug' ]]
then
PTAOBJTY='Debug'
else
PTAOBJTY='Release'
fi
Build=$PTAOBJTY'-build'
export PATH=$PTAHOME/$Build/bin:$PATH
