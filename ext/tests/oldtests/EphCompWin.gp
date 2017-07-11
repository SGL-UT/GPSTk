#
# Used to plot the output from EphComp
#
set label 1 "Ephemeris Comparison" at graph 0.5,0.96 center
set label 2 "Broadcast - SP3" at graph 0.5,0.91 center
set key box linewidth 0.250

set xlabel "Time"
set xdata time
set timefmt "%m/%d/%Y %H:%M:%S"
set format x "%H:%02M\n%j\n%Y"
set xtics nomirror
set grid xtics

set ylabel "Position difference (m)"
set autoscale y2
set ytics nomirror
set y2tics
set y2label "Velocity difference (m/s)"
set grid ytics

# set terminal postscript landscape color
# set output "EphComp.ps"

plot "EphComp.out" using 1:4 t "X" with points pointsize 0.5,\
     "" using 1:5 t "Y" with points pointsize 0.5,\
     "" using 1:6 t "Z" with points pointsize 0.5,\
     "" using 1:8 axes x1y2 t "VX" with points pointsize 0.5,\
     "" using 1:9 axes x1y2 t "VY" with points pointsize 0.5,\
     "" using 1:10 axes x1y2 t "VZ" with points pointsize 0.5
pause -1 "EphComp test"
