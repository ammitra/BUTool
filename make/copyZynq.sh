#!/bin/bash
LIBS=$(find -L ./ | grep "lib.*\.so")
ENVS=$(find -L ./ | grep "env\.sh" | grep -v "doc")

zynq=root@192.168.30.12

scp $LIBS $zynq:/work/lib

scp -r ./bin/* $zynq:/work/bin



rm -f temp.env
touch temp.env
for file in $ENVS
do 
    cat $file >> temp.env
done
scp temp.env $zynq:/work/env.sh
rm temp.env
