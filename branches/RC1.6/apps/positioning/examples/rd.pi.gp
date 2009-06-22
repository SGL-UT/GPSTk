set title "GRACE satellite positions, interpolated to 1 sec (gpstk::posInterp)"
set xlabel 'GPS seconds of week 1242'
set ylabel 'ECEF position components (m)'
unset mouse
set term x11 enhanced font 'luxi sans,17'
set key outside
plot 'rd.pi.out' using 2:4 t '' with points, '' using 2:5 t '' with points, '' using 2:6 t '' with points
