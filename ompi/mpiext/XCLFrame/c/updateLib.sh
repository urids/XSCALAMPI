#!/bin/bash

host0=148.247.102.190
host2=148.247.102.192
host3=148.247.102.193

make && make install

declare -a hosts=($host0)

for i in "${hosts[@]}"
do
rsync -r $(pwd) uriel@$i:$(pwd)/../
rsh uriel@$i 'cd /home/uriel/Dev/mpisrc/XSCALAMPI/ompi/mpiext/XCLFrame/c && make && make install'
done

