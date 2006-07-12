echo off
REM Run all tests in the tests directory
REM $Id:$

echo.
echo.
echo rinex_met_test
echo -------------------------------------------------------------------------
rinex_met_test data/405_077A.02M

echo.
echo.
echo daytimetest:
echo -------------------------------------------------------------------------
daytimetest

echo.
echo.
echo exceptiontest:
echo -------------------------------------------------------------------------
exceptiontest

echo.
echo.
echo petest:
echo -------------------------------------------------------------------------
petest data/nga12600.apc

echo.
echo.
echo stringutiltest:
echo -------------------------------------------------------------------------
stringutiltest >stringutiltest.out
diff stringutiltest.out data/stringutiltest.ref
echo Look in stringutiltest.out

echo.
echo.
echo MatrixTest
echo -------------------------------------------------------------------------
MatrixTest data/anotsym.dat  data/cov.dat data/dia.dat data/lt.dat data/partials.dat data/squ.dat data/sym.dat data/tmatrix.dat data/ut.dat > MatrixTest.out
echo Look in MatrixTest.out
echo Here is a 'diff' with what MatrixTest.out should be:
diff MatrixTest.out data/MatrixTest.ref.Win32

echo.
echo.
echo Generating directory for FileSpecTest
echo -------------------------------------------------------------------------
perl genFileSpecTestDirs.pl
FileSpecTest

echo.
echo.
echo PositionTest
echo -------------------------------------------------------------------------
positiontest > positiontest.out
diff positiontest.out data/positiontest.ref
echo Look in positiontest.out

echo.
echo.
echo EphComp:
echo -------------------------------------------------------------------------
echo first merge three RINEX navigation files to create merged.n
..\apps\Rinextools\NavMerge -omerged.n data\s011061a.04n data\s021061a.04n data\s031061a.04n
echo EphComp will throw an exception when it can't find PRN 13 here --
echo    that's ok, its part of the test:
EphComp -p data/nga12600.apc -p data/nga12601.apc -b merged.n > EphComp.out
echo ...end of EphComp. Look in EphComp.out -- here's the plot:
gnuplot EphCompWin.gp

