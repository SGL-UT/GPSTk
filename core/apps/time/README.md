time - calgps
=============

This application generates a dual GPS and Julian calendar to either stdout or to a graphics
file. The arguments and format are inspired by the UNIX ‘cal’ utility. With no arguments, the
current argument is printed. The last and next month can also be printed. Also, the current
or any given year can be printed.

Usage:
------

### Optional Arguments

Short Arg.| Long Arg.| Description

    -h    –help               Generates help output.
    -3    –three-months       Prints a GPS calendar for the previous, current, and next month.
    -y    –year               Prints a GPS calendar for the entire current year.
    -Y    –specific-year=NUM  Prints a GPS calendar for the entire specified year.
    -p    –postscript=ARG     Generates a postscript file.
    -s    –svg=ARG            Generates an SVG file.
    -e    –eps=ARG            Generates an encapsulated postscript file.
    -v    –view               Try to launch an appropriate viewer for the file.
    -n    –no-blurb           Suppress GPSTk reference in graphic output.

Examples:
---------

    > calgps -3
        Jun 2011
        1638 1-152 2-153 3-154 4-155
        1639 5-156 6-157 7-158 8-159 9-160 10-161 11-162
        1640 12-163 13-164 14-165 15-166 16-167 17-168 18-169
        1641 19-170 20-171 21-172 22-173 23-174 24-175 25-176
        1642 26-177 27-178 28-179 29-180 30-181
        Jul 2011
        1642 1-182 2-183
        1643 3-184 4-185 5-186 6-187 7-188 8-189 9-190
        1644 10-191 11-192 12-193 13-194 14-195 15-196 16-197
        1645 17-198 18-199 19-200 20-201 21-202 22-203 23-204
        1646 24-205 25-206 26-207 27-208 28-209 29-210 30-211
        1647 31-212
        . . .

Notes:
---------

If multiple options are given only the first is considered.



-----------------------------------------------------------------------------------------------------------------------

time - timeconvert
==================

This application allows the user to convert between time formats associated with GPS. Time
formats include: civilian time, Julian day of year and year, GPS week and seconds of week, Z
counts, and Modified Julian Date (MJD).

Usage:
------

### Optional Arguments

Short Arg.| Long Arg.| Description

    -d    –debug             Increase debug level.
    -v    –verbose           Increase verbosity.
    -h    –help              Print help usage.
    -A    –ansi=TIME         “ANSI-Second”.
    -c    –civil=TIME        “Month(numeric) DayOfMonth Year Hour:Minute:Second
    -R    –rinex-file=TIME   “Year(2-digit) Month(numeric) DayOfMonth Hour Minute Second”.
    -o    –ews=TIME          “GPSEpoch 10bitGPSweek SecondOfWeek”.
    -f    –ws=TIME           “FullGPSWeek SecondOfWeek”.
    -w    –wz=TIME           “FullGPSWeek Zcount”.
          –z29=TIME          “29bitZcount”.
    -Z    –z32=TIME          “32bitZcount”.
    -j    –julian=TIME       “JulianDate”.
    -m    –mjd=TIME          “ModifiedJulianDate”.
    -u    –unixtime=TIME     “UnixSeconds UnixMicroseconds”.
    -y    –doy=TIME          “Year DayOfYear SecondsOfDay”.
          –input-format=ARG  Time format to use on input.
          –input-time=ARG    Time to be parsed by ”input-format” option.
    -F    –format=ARG        Time format to use on output.
    -a    –add-offset=NUM    Add NUM seconds to specified time.
    -s    –sub-offset=NUM    Subtract NUM seconds from specified time.

Examples:
---------

Convert RINEX file time.

    > timeconvert -R "05 06 1985 13:50:02"
        Month/Day/Year H:M:S 11/06/2010 13:00:00
        Modified Julian Date 55506.541666667
        GPSweek DayOfWeek SecOfWeek 584 6 565200.000000
        FullGPSweek Zcount 1608 376800
        Year DayOfYear SecondOfDay 2010 310 46800.000000
        Unix: Second Microsecond 1289048400 0
        Zcount: 29-bit (32-bit) 306560992 (843431904)
        Convert ews time.
        timeconvert -o "01 1379 500"
        Month/Day/Year 1/25/2026
        Hour:Min:Sec 00:08:20
        Modified Julian Date 61065.005787037
        GPSweek DayOfWeek SecOfWeek 355 0 500.000000
        FullGPSweek Zcount 2403 333
        Year DayOfYear SecondOfDay 2026 25 500.000000
        Unix_sec Unix_usec 1769299700 0
        Zcount: 29-bit (32-bit) 186122573 (1259864397)
        
Notes:
---------

If no arguments are given it will convert the current time to all