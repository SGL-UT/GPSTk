import unittest
import sys
# sys.path.append('../bin')
from gpstk import *


class TimeSystem_Tests(unittest.TestCase):
    def test_constructor_string(self):
        sys = TimeSystem('GPS')
        self.assertEqual('GPS', str(sys))
        self.assertEqual('GPS', sys.getTimeSystem())

    def test_constructor_constant(self):
        sys = TimeSystem(TimeSystem.GPS)
        self.assertEqual('GPS', str(sys))
        self.assertEqual('GPS', sys.getTimeSystem())


class Time_Comparisons(unittest.TestCase):
    def test_standard_times(self):
        timeSystem = TimeSystem(TimeSystem.GPS)

        t1 = UnixTime(1370983244, 659200)  # in 2013
        t1 = t1.convertToCommonTime()
        t1.setTimeSystem(timeSystem)

        t2 = CivilTime(2012, 11, 6, 20, 40, 400)  # in 2012
        t2 = t2.convertToCommonTime()
        t2.setTimeSystem(timeSystem)

        self.assertEqual(False, t1 < t2)
        self.assertEqual(False, t1 <= t2)
        self.assertEqual(True, t1 > t2)
        self.assertEqual(True, t1 >= t2)
        self.assertEqual(False, t1 == t2)
        self.assertEqual(True, t1 != t2)

    def test_equal_common_times(self):
        timeSystem = TimeSystem('GPS')
        t1 = CommonTime(timeSystem)
        t1.addDays(3)
        t2 = CommonTime(timeSystem)
        t2.addSeconds(60*60*24*3)
        self.assertEqual(False, t1 < t2)
        self.assertEqual(True, t1 <= t2,)
        self.assertEqual(False, t1 > t2)
        self.assertEqual(True, t1 >= t2)
        self.assertEqual(True, t1 == t2)
        self.assertEqual(False, t1 != t2)


if __name__ == '__main__':
    unittest.main()
