#!/usr/bin/gnuplot

set term pdf #enhanced monochrome dashed
set output "omp.pdf"

set key right bottom

set xlabel "Iterations"
set ylabel "Time in micro seconds"

plot "./sp_omp_sync_1.data" with linespoints title "Performance for omp synchronised with by 1"
plot "./sp_omp_sync_10.data" with linespoints title "Performance for omp synchronised with by 10"
plot "./sp_omp_sync_100.data" with linespoints title "Performance for omp synchronised with by 100"

plot "./sp_omp_async_1.data" with linespoints title "Performance for omp asynchronised with by 1"
plot "./sp_omp_async_10.data" with linespoints title "Performance for omp asynchronised with by 10"
#plot "./sp_omp_async_100.data" with linespoints title "Performance for omp asynchronised with by 100"