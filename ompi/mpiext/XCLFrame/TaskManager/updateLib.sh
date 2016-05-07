#!/bin/bash

host0=148.247.102.190
host2=148.247.102.192
host3=148.247.102.193

make && make install

declare -a hosts=($host0 $host2 $host3)

for i in "${hosts[@]}"
do
rsync -r $(pwd)/Base uriel@$i:$(pwd)/Base/../
rsh uriel@$i 'cd /home/uriel/Dev/mpisrc/XSCALAMPI/ompi/mpiext/XCLFrame/TaskManager && make && make install'
done

