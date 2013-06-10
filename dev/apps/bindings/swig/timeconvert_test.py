import subprocess
import timeconvert

def test(commands, expected, name=''):
    print 'TEST:', name
    actual = subprocess.check_output(['python', 'timeconvert.py'] + commands)
    if expected.split() != actual.split(): # whitespace not compared
        print 'Test failure:'
        print 'Commands:', commands
        print 'Expected output:'
        print expected
        print 'Actual output:'
        print actual
    else:
        print '\tPASS'
    print ''
    print ''

test(['-o \"01 1379 500\"'], 
'        Month/Day/Year H:M:S             10/26/1986 00:08:20 GPS\n'
'        ModifiedJulianDate               46729.005787037 GPS\n'
'        GPSweek DayOfWeek SecOfWeek      355 0    500.000000\n'
'        FullGPSweek Zcount               355    333\n'
'        Year DayOfYear SecondOfDay       1986 299   500.000000\n'
'        Unix: Second Microsecond         530669300      0\n'
'        Zcount: 29-bit (32-bit)          186122573 (186122573)\n',
         'ews input')

test(['-o \"01 1379 500\"', '-a 7'], 
'        Month/Day/Year H:M:S             10/26/1986 00:08:27 GPS\n'
'        ModifiedJulianDate               46729.005868056 GPS\n'
'        GPSweek DayOfWeek SecOfWeek      355 0    507.000000\n'
'        FullGPSweek Zcount               355    338\n'
'        Year DayOfYear SecondOfDay       1986 299   507.000000\n'
'        Unix: Second Microsecond         530669307      0\n'
'        Zcount: 29-bit (32-bit)          186122578 (186122578)\n',
         'ews input +7 seconds')

test(['-o \"01 1379 500\"', '-s 19'], 
'        Month/Day/Year H:M:S             10/26/1986 00:08:01 GPS\n'
'        ModifiedJulianDate               46729.005567130 GPS\n'
'        GPSweek DayOfWeek SecOfWeek      355 0    481.000000\n'
'        FullGPSweek Zcount               355    320\n'
'        Year DayOfYear SecondOfDay       1986 299   481.000000\n'
'        Unix: Second Microsecond         530669281      0\n'
'        Zcount: 29-bit (32-bit)          186122560 (186122560)\n',
         'ews input -19 seconds')



