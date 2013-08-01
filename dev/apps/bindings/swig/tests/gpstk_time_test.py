import unittest
import sys
import gpstk


class TimeSystem_Tests(unittest.TestCase):
    def test_constructor_string(self):
        sys = gpstk.TimeSystem('GPS')
        self.assertEqual('GPS', str(sys))
        self.assertEqual(gpstk.TimeSystem.GPS, sys.getTimeSystem())

    def test_constructor_constant(self):
        sys = gpstk.TimeSystem(gpstk.TimeSystem.GPS)
        self.assertEqual('GPS', str(sys))
        self.assertEqual(gpstk.TimeSystem.GPS, sys.getTimeSystem())

    def test_constructor_factory(self):
        sys = gpstk.timeSystem('GPS')
        self.assertEqual('GPS', str(sys))
        self.assertEqual(gpstk.TimeSystem.GPS, sys.getTimeSystem())
        self.assertRaises(ValueError, gpstk.timeSystem, 'badinput')


class Time_Comparisons(unittest.TestCase):
    def test_standard_times(self):
        timeSystem = gpstk.TimeSystem(gpstk.TimeSystem.GPS)

        t1 = gpstk.UnixTime(1370983244, 659200)  # in 2013
        t1 = t1.toCommonTime()
        t1.setTimeSystem(timeSystem)

        t2 = gpstk.CivilTime(2012, 11, 6, 20, 40, 400)  # in 2012
        t2 = t2.toCommonTime()
        t2.setTimeSystem(timeSystem)

        self.assertEqual(False, t1 < t2)
        self.assertEqual(False, t1 <= t2)
        self.assertEqual(True, t1 > t2)
        self.assertEqual(True, t1 >= t2)
        self.assertEqual(False, t1 == t2)
        self.assertEqual(True, t1 != t2)

    def test_equal_common_times(self):
        timeSystem = gpstk.TimeSystem('GPS')
        t1 = gpstk.CommonTime(timeSystem)
        t1.addDays(3)
        t2 = gpstk.CommonTime(timeSystem)
        t2.addSeconds(60*60*24*3)
        self.assertEqual(False, t1 < t2)
        self.assertEqual(True, t1 <= t2,)
        self.assertEqual(False, t1 > t2)
        self.assertEqual(True, t1 >= t2)
        self.assertEqual(True, t1 == t2)
        self.assertEqual(False, t1 != t2)


if __name__ == '__main__':
    unittest.main()
