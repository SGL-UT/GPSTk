file=input("File name (in quotes, e.g. 'file'): ")
gset title "Dynamic Allan Deviation"
gset xlabel "Window #"
gset ylabel "Tau (seconds)"
gset zlabel "Allan Deviation"
gset nokey
gset log yz
gset pm3d
gset style line 100 lt 5 lw 0.5
gset pm3d hidden3d 100
gset palette negative rgbformulae 23,28,3
gset view 70,215
gsplot file u 1:2:3 with lines 3
pause -1

