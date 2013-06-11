import subprocess
import timeconvert


fails = []
# test conditions should return TRUE if the test passed
default_condition = lambda x,y: x.split() == y.split()

def test(commands, expected='', name='', pass_condition=default_condition):
    print 'TEST:', name
    actual = subprocess.check_output(['python', 'timeconvert.py'] + commands)
    if not pass_condition(actual, expected): # whitespace not compared
        fails.append(name)
        print 'Test failure:'
        print 'Commands:', commands
        print 'Expected output:'
        print expected
        print 'Actual output:',
        print actual
        print ''
        print ''
    else:
        print '\tPASS'

test(['-A', '1370983571'],        
'        Month/Day/Year H:M:S            06/11/2013 20:46:11 GPS\n'
'        Modified Julian Date            56454.865405093 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     720 2  247571.000000\n'
'        FullGPSweek Zcount              1744 165047\n'
'        Year DayOfYear SecondOfDay      2013 162  74771.000000\n'
'        Unix: Second Microsecond        1370983571      0\n'
'        Zcount: 29-bit (32-bit)         377652407 (914523319)\n',
         'ANSI input')

test(['-c', '5 11 2010 6:15:59'],
'        Month/Day/Year H:M:S            05/11/2010 06:15:59 GPS\n'
'        Modified Julian Date            55327.261099537 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     559 2  195359.000000\n'
'        FullGPSweek Zcount              1583 130239\n'
'        Year DayOfYear SecondOfDay      2010 131  22559.000000\n'
'        Unix: Second Microsecond        1273558559      0\n'
'        Zcount: 29-bit (32-bit)         293207231 (830078143)\n',
         'civil input')

test(['-R', '05 06 1985 13:50:02'],
'        Month/Day/Year H:M:S            11/06/2010 13:00:00 GPS\n'
'        Modified Julian Date            55506.541666667 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     584 6  565200.000000\n'
'        FullGPSweek Zcount              1608 376800\n'
'        Year DayOfYear SecondOfDay      2010 310  46800.000000\n'
'        Unix: Second Microsecond        1289048400      0\n'
'        Zcount: 29-bit (32-bit)         306560992 (843431904)\n',
         'rinex input')

test(['-f', '158 200'],
'        Month/Day/Year H:M:S            01/16/1983 00:03:20 GPS\n'
'        Modified Julian Date            45350.002314815 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     158 0    200.000000\n'
'        FullGPSweek Zcount              158    133\n'
'        Year DayOfYear SecondOfDay      1983 016   200.000000\n'
'        Unix: Second Microsecond        411523400      0\n'
'        Zcount: 29-bit (32-bit)         82837637 (82837637)\n',
         'ws input')


test(['-o', '01 1379 500'],         
'        Month/Day/Year H:M:S            06/11/2006 00:08:20 GPS\n'
'        Modified Julian Date            53897.005787037 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     355 0    500.000000\n'
'        FullGPSweek Zcount              1379    333\n'
'        Year DayOfYear SecondOfDay      2006 162   500.000000\n'
'        Unix: Second Microsecond        1149984500      0\n'
'        Zcount: 29-bit (32-bit)         186122573 (722993485)\n',
         'ews input')

test(['--ews', '01 1379 500'], 
'        Month/Day/Year H:M:S            06/11/2006 00:08:20 GPS\n'
'        Modified Julian Date            53897.005787037 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     355 0    500.000000\n'
'        FullGPSweek Zcount              1379    333\n'
'        Year DayOfYear SecondOfDay      2006 162   500.000000\n'
'        Unix: Second Microsecond        1149984500      0\n'
'        Zcount: 29-bit (32-bit)         186122573 (722993485)\n',
         'ews-long input')

test(['-o 01 1379 500', '-a 7'], 
'        Month/Day/Year H:M:S            06/11/2006 00:08:27 GPS\n'
'        Modified Julian Date            53897.005868056 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     355 0    507.000000\n'
'        FullGPSweek Zcount              1379    338\n'
'        Year DayOfYear SecondOfDay      2006 162   507.000000\n'
'        Unix: Second Microsecond        1149984507      0\n'
'        Zcount: 29-bit (32-bit)         186122578 (722993490)\n',
         'ews input +7 seconds')

test(['-o 01 1379 500', '-s 19'], 
'        Month/Day/Year H:M:S            06/11/2006 00:08:01 GPS\n'
'        Modified Julian Date            53897.005567130 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     355 0    481.000000\n'
'        FullGPSweek Zcount              1379    320\n'
'        Year DayOfYear SecondOfDay      2006 162   481.000000\n'
'        Unix: Second Microsecond        1149984481      0\n'
'        Zcount: 29-bit (32-bit)         186122560 (722993472)\n',
         'ews input -19 seconds')

test(['-o 01 1379 500', '-a', '1', '2', '-s', '5', '6'], 
'        Month/Day/Year H:M:S            06/11/2006 00:08:12 GPS\n'
'        Modified Julian Date            53897.005694444 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     355 0    492.000000\n'
'        FullGPSweek Zcount              1379    328\n'
'        Year DayOfYear SecondOfDay      2006 162   492.000000\n'
'        Unix: Second Microsecond        1149984492      0\n'
'        Zcount: 29-bit (32-bit)         186122568 (722993480)\n',
         'ews input, +1,+2,-5,-6')

test(['-j', '2456455.361628'],    
'        Month/Day/Year H:M:S            06/11/2013 20:40:44 GPS\n'
'        Modified Julian Date            56454.861628000 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     720 2  247244.659200\n'
'        FullGPSweek Zcount              1744 164829\n'
'        Year DayOfYear SecondOfDay      2013 162  74444.659200\n'
'        Unix: Second Microsecond        1370983244 659200\n'
'        Zcount: 29-bit (32-bit)         377652189 (914523101)\n',
         'julian input')

test(['-m', '56454.857905304'],
'        Month/Day/Year H:M:S            06/11/2013 20:35:23 GPS\n'
'        Modified Julian Date            56454.857905304 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     720 2  246923.018266\n'
'        FullGPSweek Zcount              1744 164615\n'
'        Year DayOfYear SecondOfDay      2013 162  74123.018266\n'
'        Unix: Second Microsecond        1370982923  18266\n'
'        Zcount: 29-bit (32-bit)         377651975 (914522887)\n',
         'mjd input')

test(['-u 1470956546'],  
'        Month/Day/Year H:M:S            08/11/2016 23:02:26 GPS\n'
'        Modified Julian Date            57611.960023148 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     885 4  428546.000000\n'
'        FullGPSweek Zcount              1909 285697\n'
'        Year DayOfYear SecondOfDay      2016 224  82946.000000\n'
'        Unix: Second Microsecond        1470956546      0\n'
'        Zcount: 29-bit (32-bit)         464280577 (1001151489)\n',
         'unix input')

test(['--unixtime', '1470956546'],  
'        Month/Day/Year H:M:S            08/11/2016 23:02:26 GPS\n'
'        Modified Julian Date            57611.960023148 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     885 4  428546.000000\n'
'        FullGPSweek Zcount              1909 285697\n'
'        Year DayOfYear SecondOfDay      2016 224  82946.000000\n'
'        Unix: Second Microsecond        1470956546      0\n'
'        Zcount: 29-bit (32-bit)         464280577 (1001151489)\n',
         'unix input-long')

test(['-Z 1570956546'],
'        Month/Day/Year H:M:S            06/10/2037 07:02:27 GPS\n'
'        Modified Julian Date            65219.293368056 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     948 3  284547.000000\n'
'        FullGPSweek Zcount              2996 189698\n'
'        Year DayOfYear SecondOfDay      2037 161  25347.000000\n'
'        Unix: Second Microsecond        2128230147      0\n'
'        Zcount: 29-bit (32-bit)         497214722 (1570956546)\n',
         'Zcount input')

test(['-y 1994 24 10000'],
'        Month/Day/Year H:M:S            01/24/1994 02:46:40 GPS\n'
'        Modified Julian Date            49376.115740741 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     733 1  96400.000000\n'
'        FullGPSweek Zcount              733  64266\n'
'        Year DayOfYear SecondOfDay      1994 024  10000.000000\n'
'        Unix: Second Microsecond        759379600      0\n'
'        Zcount: 29-bit (32-bit)         384367370 (384367370)\n',
         'doy input')

test(['-y 1980 6 0'],
'        Month/Day/Year H:M:S            01/06/1980 00:00:00 GPS\n'
'        Modified Julian Date            44244.000000000 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     0 0      0.000000\n'
'        FullGPSweek Zcount              0      0\n'
'        Year DayOfYear SecondOfDay      1980 006     0.000000\n'
'        Unix: Second Microsecond        315964800      0\n'
'        Zcount: 29-bit (32-bit)         0 (0)\n',
         'doy input - gps epoch')

test(['-w 1023 604799'],        
'        Month/Day/Year H:M:S            01/16/1980 11:59:58 GPS\n'
'        Modified Julian Date            44254.499982639 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     1 3  302398.500000\n'
'        FullGPSweek Zcount              1 201599\n'
'        Year DayOfYear SecondOfDay      1980 016  43198.500000\n'
'        Unix: Second Microsecond        316871998 500000\n'
'        Zcount: 29-bit (32-bit)         725887 (725887)\n',
         'wz input - before epoch')

test(['-w 1024 0'],           
'        Month/Day/Year H:M:S            01/06/1980 00:00:00 GPS\n'
'        Modified Julian Date            44244.000000000 GPS\n'
'        GPSweek DayOfWeek SecOfWeek     0 0      0.000000\n'
'        FullGPSweek Zcount              0      0\n'
'        Year DayOfYear SecondOfDay      1980 006     0.000000\n'
'        Unix: Second Microsecond        315964800      0\n'
'        Zcount: 29-bit (32-bit)         0 (0)\n',
         'wz input - on epoch')

test(['-R', '05 06 1985 13:50:02', '-F', 'ms:%K, year:%Y'],
        'ms:1289048400, year:2010',
        'output format')

test(['-y 1980 5 86499'],
         'Input could not be parsed. Check the formatting and ensure that the input is in quotes.',
         'doy crash input - 1sec before gps epoch')

print ''
print 'Number of fails:', len(fails)
print 'Test fails:', fails
