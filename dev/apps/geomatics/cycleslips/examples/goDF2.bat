REM run DiscFix
..\DiscFix -fdf_occult.inp -ichmp0110.02o.G31
REM
REM correct the data, creating a new Rinex file
..\..\Rinextools\EditRinex -IFchmp0110.02o.G31 -OFchmpDF.obs -TT0.5 -fdf.out
REM
REM see a table of the results
grep "^GDC" df.log
REM
REM look at results for Sat G31, unique number 6, using gnuplot
sed -n -e "s/WLSS 6 G31 [1-9] //p" df.log > junk
awk -f mjd2gps.awk -v C=1 -v ROLL=NO -v WEEK=NO junk > WLSS6G31.dat
sed -n -e "s/GFRF 6 G31 [1-9] //p" df.log > junk
awk -f mjd2gps.awk -v C=1 -v ROLL=NO -v WEEK=NO junk > GFRF6G31.dat
start /B c:\gnuplot\bin\wgnuplot.exe WLSS6_Win.gp
start /B c:\gnuplot\bin\wgnuplot.exe GFP6_Win.gp
