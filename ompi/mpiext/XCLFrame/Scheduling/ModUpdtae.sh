#!/bin/bash

host0=192.168.1.25
host2=148.247.102.192
host3=148.247.102.193

make && make install

declare -a hosts=($host0)

for i in "${hosts[@]}"
do
rsync -r $(pwd)/Base uriel@$i:$(pwd)/Base/../
rsh uriel@$i 'cd /home/uriel/Dev/mpisrc/XSCALAMPI/ompi/mpiext/XCLFrame/Scheduling && make && make install'
done

