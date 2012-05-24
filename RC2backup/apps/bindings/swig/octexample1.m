
gpstk;
time=gpstk.DayTime();

disp('Hello World!')
disp('the current GPS week is: ')
disp(time.GPSfullweek())
disp('the day of the GPS week is: ')
disp(time.GPSday())
disp('The seconds of the GPS week is: ')
disp(time.GPSsecond())
