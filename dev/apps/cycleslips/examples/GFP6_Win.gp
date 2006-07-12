set xlabel "GPS Seconds of Week"
set autoscale y2
set ytics nomirror
set y2tics
set label 1 "DC GFRF GFP, fit GFR" at graph 0.5,0.96 center
set label 2 "LEO data Uniq 6 PRN G31" at graph 0.5,0.91 center
set ylabel "GFP / Residual (m)"
set y2label "GFR / Fit (m)"
plot "GFRF6G31.dat" using 1:2 t "GFP" with points pointsize 0.5
replot "GFRF6G31.dat" using 1:4 axes x1y2 t "GFR" with points pointsize 0.5
replot "GFRF6G31.dat" using 1:5 axes x1y2 t "Fit" with points pointsize 0.5
replot "GFRF6G31.dat" using 1:6 t "Residual" with points pointsize 0.5
pause -1 "End GFRF 6"
