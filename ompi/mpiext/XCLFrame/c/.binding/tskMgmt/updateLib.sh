#!/bin/bash

host1=148.247.102.190
#host2=148.247.102.192
#host3=148.247.102.193

make && make install

declare -a hosts=($host1)



for i in "${hosts[@]}"
do
rsync -r $(pwd) uriel@$i:$(pwd)/../
rsh uriel@$i 'cd /home/uriel/Dev/mpisrc/XCLMPI/ompi/mpiext/XCLFrame/c/binding/tskMgmt && make && make install'
done

