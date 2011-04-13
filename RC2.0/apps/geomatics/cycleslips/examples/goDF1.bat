REM run DiscFix
..\DiscFix -fdf.inp -ieusk2950.01o.G17
REM
REM correct the data, creating a new Rinex file
..\..\Rinextools\EditRinex -IFeusk2950.01o.G17 -OFeuskDF.obs -TT0.5 -fdf.out
REM
REM see a table of the results
grep "^GDC" df.log
REM
REM look at results for Sat G17, unique number 3, using gnuplot
sed -n -e "s/WLSS 3 G17 [1-9] //p" df.log > junk
awk -f mjd2gps.awk -v C=1 -v ROLL=NO -v WEEK=NO junk > WLSS3G17.dat
sed -n -e "s/GFRF 3 G17 [1-9] //p" df.log > junk
awk -f mjd2gps.awk -v C=1 -v ROLL=NO -v WEEK=NO junk > GFRF3G17.dat
start /B c:\gnuplot\bin\wgnuplot.exe WLSS3_Win.gp
start /B c:\gnuplot\bin\wgnuplot.exe GFP3_Win.gp
REM
REM look at results for Sat G17, unique number 2, using gnuplot
sed -n -e "s/WLSS 2 G17 [1-9] //p" df.log > junk
awk -f mjd2gps.awk -v C=1 -v ROLL=NO -v WEEK=NO junk > WLSS2G17.dat
sed -n -e "s/GFRF 2 G17 [1-9] //p" df.log > junk
awk -f mjd2gps.awk -v C=1 -v ROLL=NO -v WEEK=NO junk > GFRF2G17.dat
start /B c:\gnuplot\bin\wgnuplot.exe WLSS2_Win.gp
start /B c:\gnuplot\bin\wgnuplot.exe GFP2_Win.gp
