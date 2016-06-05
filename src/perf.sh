#!/bin/bash

for i in 1 10 100
do
    ./sable -m sp_omp_sync -D 0 -i $i > tmp.data
    tail tmp.data -n +9 > ../perf/sp_omp_sync_$i.data
    echo "done with sync" $i
done

for i in 1 10 100
do
    ./sable -m sp_omp_async -D 0 -i $i > tmp.data
    tail tmp.data -n +9 > ../perf/sp_omp_async_$i.data
    echo "done with async" $i
done

rm -f tmp.data
