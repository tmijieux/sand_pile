#!/bin/bash

for i in 1 5 10 20
do
    ./sable -m sp_omp_sync -D 0 -i $i > tmp.data
    tail tmp.data -n +9 > ../perf/sp_omp_sync_$i.data
    echo "done with sync" $i
done

for i in 1 5 10 20
do
    ./sable -m sp_omp_async -D 0 -i $i > tmp.data
    tail tmp.data -n +9 > ../perf/sp_omp_async_$i.data
    echo "done with async" $i
done

for i in 1 2 3 4 
do
    OMP_NUM_THREADS=$i ./sable -m sp_omp_sync -D 0 -i 100 > tmp.data 
    tail tmp.data -n +9 > ../perf/sp_omp_sync_100_$i.data
    echo "done with" $i "threads"
done

rm -f tmp.data
