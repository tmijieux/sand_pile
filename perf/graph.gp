#!/usr/bin/gnuplot

set term png enhanced
set key right bottom

set xlabel "Iterations"
set ylabel "Time in micro seconds" 


#############################################################
set output "omp_sync_it.png"

plot "./sp_omp_sync_1.data" with lines title "sync by 1", \
     "./sp_omp_sync_5.data" with lines title "sync by 5", \
     "./sp_omp_sync_10.data" with lines title "sync by 10", \
     "./sp_omp_sync_10.data" with lines title "sync by 20"

#############################################################
set output "omp_async_it.png"

plot "./sp_omp_async_1.data" with lines title "async by 1", \
     "./sp_omp_async_5.data" with lines title "async by 5", \
     "./sp_omp_async_10.data" with lines title "async by 10", \
     "./sp_omp_async_10.data" with lines title "async by 20"

#############################################################
set output "omp_thread.png"

plot "./sp_omp_sync_100_1.data" with lines title "1 thread", \
     "./sp_omp_sync_100_2.data" with lines title "2 threads", \
     "./sp_omp_sync_100_3.data" with lines title "3 threads", \
     "./sp_omp_sync_100_4.data" with lines title "4 threads"
