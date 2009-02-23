#!/usr/bin/python

import sys
sys.path.append("..")
from gpstkPython import *

try:
    time = DayTime()

    print 'Hello world!'
    print 'The current GPS week is', time.GPSfullweek()

    print 'The day of the GPS week is', time.GPSday()

    print 'The seconds of the GPS week is', time.GPSsecond()

except Exception:
    print 'Error'


