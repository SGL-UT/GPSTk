import unittest
import sys
import gpstk


class CommonTime_Tests(unittest.TestCase):
    def test(self):
        a = gpstk.CommonTime()
        a.addDays(1234)
        b = gpstk.CommonTime(a)
        b.addSeconds(123.4)
        c = b - a
        self.assertAlmostEqual(1234.0, a.getDays())
        self.assertEqual('0001234 00000000 0.000000000000000 UNK', str(a))
        self.assertAlmostEqual(1234.0014282407408, b.getDays())
        self.assertEqual('0001234 00123400 0.000000000000000 UNK', str(b))
        self.assertAlmostEqual(123.4, c)

    def test_exception(self):
        a = gpstk.CommonTime(gpstk.timeSystem('GPS'))
        b = gpstk.CommonTime(gpstk.timeSystem('GLO'))
        self.assertRaises(RuntimeError, a.__sub__, b)


class Conversion_Tests(unittest.TestCase):
    def test(self):
        a = gpstk.ANSITime()
        a.scanf("1234567789", "%K")
        a.setTimeSystem(gpstk.timeSystem('GLO'))
        b = a.toCommonTime()
        c = gpstk.MJD(b)
        d = gpstk.GPSWeekSecond(b)
        e = gpstk.CivilTime(b)
        f = gpstk.QZSWeekSecond(b)
        self.assertEqual('1234567789 GLO', str(a))
        self.assertEqual('2454876 84589000 0.000000000000000 GLO', str(b))
        self.assertEqual('54875.979039352 GLO', str(c))
        self.assertEqual('1518 516589.000 GLO', str(d))
        self.assertEqual('02/13/2009 23:29:49 GLO', str(e))
        self.assertEqual('1518 516589.000 GLO', str(f))


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
