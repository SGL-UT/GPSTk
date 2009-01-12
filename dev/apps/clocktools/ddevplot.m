file=input("File name (in quotes, e.g. 'file'): ") % gets input filename
gset title "Dynamic Allan Deviation" % set graph title
gset xlabel "Window #" % set x, y, z labels
gset ylabel "Tau (seconds)"
gset zlabel "Allan Deviation"
gset nokey
gset log yz % set y & z axes to log
gset pm3d % set 3d graphing style
gset style line 100 lt 5 lw 0.5
gset pm3d hidden3d 100
gset palette negative rgbformulae 23,28,3
gset view 70,215 % set view perspective
gsplot file u 1:2:3 with lines 3 % plot data
pause -1

