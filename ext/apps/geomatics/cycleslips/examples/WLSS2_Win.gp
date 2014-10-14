set xlabel "GPS Seconds of Week"
set autoscale y2
set ytics nomirror
set y2tics
set label 1 "DC WL Small Slip Detection" at graph 0.5,0.96 center
set label 2 "Uniq 2 PRN G17" at graph 0.5,0.91 center
set ylabel "WL cycles (bias,ave,sig)"
set y2label "Test & Limit (WL cycles)"
plot "WLSS2G17.dat" using 1:4 t "P Sig" with linespoints pointsize 0.5
replot "WLSS2G17.dat" using 1:7 t "F Sig" with linespoints pointsize 0.5
replot "WLSS2G17.dat" using 1:12 t "WLbias" with points pointsize 0.5
replot "WLSS2G17.dat" using 1:10 axes x1y2 t "|dAve|" with linespoints pointsize 0.5
replot "WLSS2G17.dat" using 1:11 axes x1y2 t "limit" with linespoints pointsize 0.5
replot "WLSS2G17.dat" using 1:3 t "P Ave" with lines
replot "WLSS2G17.dat" using 1:6 t "F Ave" with lines
pause -1 "End WLSS 2"
