# Gnuplot script file for plotting data in file "data.dat"
set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set title "Time to sort vs number of threads"
set ylabel "Time to sort (milliseconds)"
set xlabel "Number of Threads"
set style data linespoints
set xrange [1:32]
set grid         
set term png
set output filename
plot "data.dat" using 1:2 title "time to sort"