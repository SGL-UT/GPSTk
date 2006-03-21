@echo off
if "%OS%" == "Windows_NT" goto WinNT
perl -S RinexPlot.pl %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
:WinNT
REM start /b perl -S RinexPlot.pl %*
perl -S RinexPlot.pl %*
if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto endofperl
if %errorlevel% == 9009 echo You do not have Perl in your PATH.
:endofperl
