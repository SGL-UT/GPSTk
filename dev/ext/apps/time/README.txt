Application: timcvt.exe
Purpose:     To convert time between different time specifications
Usage:       timcvt {option}  "{time specification}"
Help:        timcvt -h
Notes:       1) timcvt must be invoked from the command line.
             2) All years are 4 digits.
             3) Time specifications should be enclosed in quotes.

Optional arguments:
  -d, --debug                       Increase debug level
  -v, --verbose                     Increase verbosity
  -h, --help                        Print help usage
  -c, --calendar=TIME               "Month(numeric) DayOfMonth Year"
  -r, --rinex=TIME                  "Month(numeric) DayOfMonth Year Hour:Minute:Second"
  -R, --rinex-file=TIME             "Year(2-digit) Month(numeric) DayOfMonth Hour Minute Second"
  -y, --doy=TIME                    "Year DayOfYear SecondsOfDay"
  -m, --mjd=TIME                    "ModifiedJulianDate"
  -o, --shortweekandsow=TIME        "10bitGPSweek SecondsOfWeek Year"
  -z, --shortweekandzcounts=TIME    "10bitGPSweek ZCounts Year"
  -f, --fullweekandsow=TIME         "FullGPSweek SecondsOfWeek"
  -w, --fullweekandzcounts=TIME     "FullGPSweek ZCounts"
  -u, --unixtime=TIME               "UnixSeconds UnixMicroseconds"
  -Z, --fullZcounts=TIME            "fullZcounts"
  -F, --format=ARG                   Time format to use on output
  -a, --add-offset=NUM               add NUM seconds to specified time
  -s, --sub-offset=NUM               subtract NUM seconds from specified time

Examples:
c:\time> timcvt -y "2004 9 9"

        Month/Day/Year                       1/9/2004
        Hour:Min:Sec                         00:00:09
        Modified Julian Date                 53013.0001041666
        GPSweek DayOfWeek SecOfWeek          228 5 432009.000000
        FullGPSweek Zcount                   1252 288006
        Year DayOfYear SecondOfDay           2004 9 9.000000
        Unix_sec Unix_usec                   1073606409 0
        FullZcount                           119825670

c:\time>timcvt -m "54327.65"

        Month/Day/Year                       8/15/2007
        Hour:Min:Sec                         15:36:00
        Modified Julian Date                 54327.6499999999
        GPSweek DayOfWeek SecOfWeek          416 3 315360.000000
        FullGPSweek Zcount                   1440 210240
        Year DayOfYear SecondOfDay           2007 227 56160.000000
        Unix_sec Unix_usec                   1187192160 0
        FullZcount                           218314048


c:\time>timcvt -c "9 9 2004" -a 86510

        Month/Day/Year                  9/10/2004
        Hour:Min:Sec                    00:01:50
        Modified Julian Date            53258.0012731482
        GPSweek DayOfWeek SecOfWeek     263 5 432110.000000
        FullGPSweek Zcount              1287 288074
        Year DayOfYear SecondOfDay      2004 254 110.000000
        Unix_sec Unix_usec              1094774510 0
        FullZcount                              138175818



---------------------------------------------------------------

Application: calgps (UNIX), calgps.exe (Windows)
Purpose:     To print a simple GPS calendar to the command line
Usage:       calgps (options)
Help:        calgps -h
Notes:       Calgps uses the system's current time to determine
             which months to print.

The program calgps behave much like the UNIX utility cal. It prints
one month of Julian days and GPS week numbers. The user can also
print a full year, either the current year or a given year.

The arguments are:
  -h, --help              Display argument list.
  -3, --three-months      Display last, this and next months.
  -y, --year              Display all months for the current year
  -Y, --specific-year=NUM Display all months for a given year

Examples:

c:\gpstk\apps\time\> calgps -3

user@server:/home/user/gpstk/apps/time%  ./calgps -Y 2004


timcvt and calgps are provided as part of the GPSTk open source project (www.gpstk.org).
GPSTk has been developed by Applied Research Laboratories, The University of
Texas at Austin, and is released under the LGPL.

