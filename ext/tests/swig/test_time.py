#!/usr/env python

from test_utils import args, run_unit_tests
import unittest
import time
from gpstk import CommonTime,SystemTime,CivilTime,BDSWeekSecond,GPSWeekSecond,GPSWeekZcount,TimeSystem
from gpstk import JulianDate, MJD, UnixTime, YDSTime

class TestSystemTime(unittest.TestCase):
    def test_SystemTime(self):
        t0 = SystemTime()
        time.sleep(0.5)
        t1 = SystemTime()
        dt = t1.toCommonTime() - t0.toCommonTime()
        self.assertAlmostEqual(dt, 0.5, places=1)

class TestTimeTags(unittest.TestCase):
    def test_CivilTime(self):
        t0 = CivilTime(1980, 1, 6, 0, 0, 0, TimeSystem(TimeSystem.GPS))
        self.assertEqual(str(t0), '01/06/1980 00:00:00 GPS')
        ct = t0.toCommonTime()
        self.assertEqual(str(ct), '2444245 00000000 0.000000000000000 GPS')
        t1 = CivilTime(ct)
        self.assertEqual(str(t1), str(t0))

    def test_BDSWeekSecond(self):
        t0 = BDSWeekSecond( 0, 0)
        self.assertEqual(str(t0), "0 0.000000 BDT")
        t1 = CivilTime(t0.toCommonTime())
        self.assertEqual(str(t1), "01/01/2006 00:00:00 BDT")
        t2 = BDSWeekSecond(t1.toCommonTime())
        self.assertEqual(str(t2), str(t0))

    def test_GPSWeekSecond(self):
        t0 = GPSWeekSecond(1023, 86400*7-1)
        self.assertEqual(str(t0), "1023 604799.000000 GPS")
        t1 = GPSWeekSecond(t0.toCommonTime())
        self.assertEqual(str(t1), "1023 604799.000000 GPS")
        t3 = GPSWeekSecond(t0.toCommonTime()+1)
        self.assertEqual(str(t3), "1024 0.000000 GPS")
        t4 = CivilTime(t3.toCommonTime())
        self.assertEqual(str(t4), "08/22/1999 00:00:00 GPS")

    def test_GPSWeekZcount(self):
        t0 = GPSWeekZcount(1023, (2*86400/3)*7-1)
        self.assertEqual(str(t0), "1023 403199 GPS")
        t1 = GPSWeekZcount(t0.toCommonTime())
        self.assertEqual(str(t1), "1023 403199 GPS")
        t2 = GPSWeekZcount(t0.toCommonTime()+1.5)
        self.assertEqual(str(t2), "1024 000000 GPS")
        t3 = CivilTime(t2.toCommonTime())
        self.assertEqual(str(t3), "08/22/1999 00:00:00 GPS")

    def test_JulianDate(self):
        t0 = JulianDate(0.0)
        self.assertEqual(str(t0), "0.000000 UNK")
        t1 = CivilTime(t0.toCommonTime())
        # See http://aa.usno.navy.mil/data/docs/JulianDate.php
        self.assertEqual(str(t1), "01/01/-4713 12:00:00 UNK")
        t2 = JulianDate(2444244.5)
        t3 = CivilTime(t2.toCommonTime())
        self.assertEqual(str(t3), "01/06/1980 00:00:00 UNK")

    def test_MJD(self):
        t0 = MJD(50449)
        t1 = CivilTime(t0.toCommonTime())
        self.assertEqual(str(t1), "01/01/1997 00:00:00 UNK")

    def test_UnixTime(self):
        t0 = UnixTime(315964800)
        t1 = CivilTime(t0.toCommonTime())
        self.assertEqual(str(t1), "01/06/1980 00:00:00 UNK")
        t2 = CivilTime(2038, 1, 19, 3, 14, 7.999999)
        t3 = UnixTime(t2.toCommonTime())
        self.assertEqual(str(t3), "2147483647 999999 UNK")

    def test_YDSTime(self):
        t0 = YDSTime(1980, 6, 0.0)
        t1 = CivilTime(t0.toCommonTime())
        self.assertEqual(str(t1), "01/06/1980 00:00:00 UNK")

if __name__ == '__main__':
    run_unit_tests()
