filetools - bc2sp3
==================

This application reads RINEX navigation file(s) and writes to SP3 (a or c) file(s).

Usage:
------

### Optional Arguments

Short Arg.| Long Arg.| Description

    –in       Read the input file (repeatable).
    –out      Name the output file. Default is sp3.out.
    –tb       Output beginning epoch; <time> = week, sec-of-week (earliest in input).
    –te       Output ending epoch; <time> = week, sec-of-week (latest in input).
    –outputC  Output version c (no correlation) (otherwise a).
    -msg      Add message as a comment to the output header (repeatable).
    –verbose  Output to screen: dump headers, data, etc.
    –help     Print this message and quit.

Examples:
---------

    > bc2sp3 --in nav/s121001a.00n --in nav/s121001a.01n --out bc2sp3.out --verbose
          Reading file nav/s121001a.00n
          Input---------------------------------- REQUIRED ----------------------------------
          Rinex Version 2.10, File type Navigation.
          Prgm: RinexNavWriter, Run: 11-08-01 0:31:01, By: NIMA
          (This header is VALID 2.11 Rinex.)
          ---------------------------------- OPTIONAL ----------------------------------
          Ion alpha is NOT valid
          Ion beta is NOT valid
          Delta UTC is NOT valid
          Leap seconds is NOT valid
          -------------------------------- END OF HEADER -------------------------------
          Reading file nav/s121001a.01n
          Input---------------------------------- REQUIRED ----------------------------------
          Rinex Version 2.10, File type Navigation.
          Prgm: RinexNavWriter, Run: 11-08-01 0:31:02, By: NIMA
          (This header is VALID 2.11 Rinex.)
          ---------------------------------- OPTIONAL ----------------------------------
          Ion alpha is NOT valid
          Ion beta is NOT valid
          Delta UTC is NOT valid
          Leap seconds is NOT valid
          -------------------------------- END OF HEADER -------------------------------
          SP3 Header: version SP3a containing positions and velocities.
          Time tag : 2000/01/01 0:14:44
          Timespacing is 900.00 sec, and the number of epochs is 208
          Data used as input : BCE
          Coordinate system : WGS84
          Orbit estimate type :
          Agency : ARL
          List of satellite PRN/accuracy (30 total) :
          G01/0 G02/0 G03/0 G04/0 G05/0 G06/0 G07/0 G08/0
          G09/0 G10/0 G11/0 G13/0 G14/0 G15/0 G16/0 G17/0
          G18/0 G19/0 G20/0 G21/0 G22/0 G23/0 G24/0 G25/0
          G26/0 G27/0 G28/0 G29/0 G30/0 G31/0
          Comments:
          End of SP3 header
          * G01 2000/01/01 0:14:44.000 = 1042/519284.000
          P G01 2000/01/01 0:14:44.000 = 1042/519284.000 X= 25704.923932
          Y= 1917.715173 Z= -6382.182137 C= 0.010948 sX= 0 sY= 0 sZ= 0 sC= 0 - - - -
          V G01 2000/01/01 0:14:44.000 = 1042/519284.000 X= 73.647819
          Y= 46.729037 Z= 302.940947 C= 0.000000 sX= 0 sY= 0 sZ= 0 sC= 0
          P G03 2000/01/01 0:14:44.000 = 1042/519284.000 X= 19615.286679
          Y= 13022.977045 Z= -12340.096622 C= 0.001460 sX= 0 sY= 0 sZ= 0 sC= 0 - - - -
          V G03 2000/01/01 0:14:44.000 = 1042/519284.000 X= -158.845279
          Y= -3.592649 Z= -256.800421 C= 0.000000 sX= 0 sY= 0 sZ= 0 sC= 0
          P G14 2000/01/01 0:14:44.000 = 1042/519284.000 X= 21304.591776
          Y= -7854.561000 Z= 13783.692368 C= -0.001147 sX= 0 sY= 0 sZ= 0 sC= 0 - - - -
          V G14 2000/01/01 0:14:44.000 = 1042/519284.000 X= -112.966658
          Y= 134.498918 Z= 250.863009 C= 0.000000 sX= 0 sY= 0 sZ= 0 sC= 0
          P G15 2000/01/01 0:14:44.000 = 1042/519284.000 X= 15085.444070
          Y= 12582.798439 Z= 17649.742134 C= 0.010795 sX= 0 sY= 0 sZ= 0 sC= 0 - - - -
          V G15 2000/01/01 0:14:44.000 = 1042/519284.000 X= 39.944949
          Y= 225.075281 Z= -191.841184 C= 0.000000 sX= 0 sY= 0 sZ= 0 sC= 0
          P G16 2000/01/01 0:14:44.000 = 1042/519284.000 X= 19460.508602
          Y= -17881.770281 Z= 1051.372781
          0C= -0.002944 sX= 0 sY= 0 sZ= 0 sC= 0 - - - -
          ...