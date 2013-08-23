#!/usr/bin/env python

import unittest
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
        # subtracting 2 CommonTimes throws an InvalidRequest
        a = gpstk.CommonTime(gpstk.TimeSystem('GPS'))
        b = gpstk.CommonTime(gpstk.TimeSystem('GLO'))
        self.assertRaises(gpstk.exceptions.InvalidRequest, a.__sub__, b)

    def test_times_gen(self):
        start = gpstk.CommonTime()
        start.addSeconds(100.0)
        end = gpstk.CommonTime()
        end.addSeconds(900.0)
        times = gpstk.times(start, end, seconds=200.0)
        self.assertEqual(100.0, times.next().getSecondOfDay())
        self.assertEqual(300.0, times.next().getSecondOfDay())
        self.assertEqual(500.0, times.next().getSecondOfDay())
        self.assertEqual(700.0, times.next().getSecondOfDay())
        self.assertEqual(900.0, times.next().getSecondOfDay())
        self.assertRaises(StopIteration, times.next)

    def test_times_list(self):
        start = gpstk.CommonTime()
        start.addSeconds(100.0)
        end = gpstk.CommonTime()
        end.addSeconds(900.0)
        times = list(gpstk.times(start, end, seconds=200.0))
        self.assertEqual(100.0, times[0].getSecondOfDay())
        self.assertEqual(300.0, times[1].getSecondOfDay())
        self.assertEqual(500.0, times[2].getSecondOfDay())
        self.assertEqual(700.0, times[3].getSecondOfDay())
        self.assertEqual(900.0, times[4].getSecondOfDay())

        times = list(gpstk.times(start, end))
        self.assertEqual(2, len(times))
        self.assertEqual(times[0], start)
        self.assertEqual(times[1], end)


class ScanTimes(unittest.TestCase):
    def test_scanTime(self):
        def test(instr, fmt, expected='', raises=None):
            if raises is not None:
                self.assertRaises(raises, gpstk.scanTime, instr, fmt)
            else:
                self.assertEqual(expected, str(gpstk.scanTime(instr, fmt)))

        # ANSI
        test('10000', '%K', '2440588 10000000 0.000000000000000 UNK')
        test('100000000', '%x', raises=gpstk.exceptions.InvalidRequest)  # bad format
        test('1000000000000000000', '%K', raises=gpstk.exceptions.InvalidRequest)  # bad time

        # Civil
        test('01 24 1994 11:39:50', '%m %d %Y %H:%M:%f', '2449377 41990000 0.000000000000000 UNK')
        test('123461994', '%m %d %Y %H:%M:%f', raises=gpstk.exceptions.InvalidRequest)

        # RINEX
        test('01 24 1994 11:39:50', '%y %m %d %H %M %S', '2454603 39600000 0.000000000000000 UNK')
        test('05 06 1985 13:50:02', '%y %m %d %H %M %S', '2455507 46800000 0.000000000000000 UNK')

        # ews
        test('0 700 0', '%E %G %g', '2449145 00000000 0.000000000000000 GPS')

        # ws
        test('158 200', '%F %g', '2445351 00200000 0.000000000000000 GPS')

        # wz
        test('200 300', '%F %Z', '2444245 00450000 0.000000000000000 UNK')
        test('1023 604799', '%F %Z', '2444255 43198500 0.000000000000000 UNK')

        # z32
        test('1570956546', '%C', '2465220 25347000 0.000000000000000 UNK')

        # julian
        test('2456455.361628', '%J', '2456455 74444659 0.000200009482447 UNK')

        # mjd
        test('56454.857905304', '%Q', '2456455 74123018 0.000265603604959 UNK')

        # unix
        test('1470956546', '%U', '2457612 82946000 0.000000000000000 UNK')

        # doy
        test('1994 24 10000', '%Y %j %s', '2449377 10000000 0.000000000000000 UNK')


class Conversion_Tests(unittest.TestCase):
    def test(self):
        a = gpstk.ANSITime()
        a.scanf("1234567789", "%K")
        a.setTimeSystem(gpstk.TimeSystem('GLO'))
        b = a.toCommonTime()
        c = gpstk.MJD(b)
        d = gpstk.GPSWeekSecond(b)
        e = gpstk.CivilTime(b)
        f = gpstk.QZSWeekSecond(b)
        self.assertEqual('1234567789 GLO', str(a))
        self.assertEqual('2454876 84589000 0.000000000000000 GLO', str(b))
        self.assertEqual('54875.979039352 GLO', str(c))
        self.assertEqual('1518 516589.000 GPS', str(d))
        self.assertEqual('02/13/2009 23:29:49 GLO', str(e))
        self.assertEqual('1518 516589.000 QZS', str(f))


class TimeSystem_Tests(unittest.TestCase):
    def test_constructor_string(self):
        sys = gpstk.TimeSystem('GPS')
        self.assertEqual('GPS', str(sys))
        self.assertEqual(gpstk.TimeSystem.GPS, sys.getTimeSystem())

    def test_constructor_constant(self):
        sys = gpstk.TimeSystem(gpstk.TimeSystem.GPS)
        self.assertEqual('GPS', str(sys))
        self.assertEqual(gpstk.TimeSystem.GPS, sys.getTimeSystem())


class Time_Comparisons(unittest.TestCase):
    def test_standard_times(self):
        timeSystem = gpstk.TimeSystem('GPS')

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
        t2.addSeconds(2 * gpstk.constants.SEC_PER_DAY)  # add 2 days
        t2 += (2 * gpstk.constants.SEC_PER_DAY)  # add 2 more days
        t2 -= (1 * gpstk.constants.SEC_PER_DAY)  # subtract a day

        self.assertEqual(False, t1 < t2)
        self.assertEqual(True, t1 <= t2,)
        self.assertEqual(False, t1 > t2)
        self.assertEqual(True, t1 >= t2)
        self.assertEqual(True, t1 == t2)
        self.assertEqual(False, t1 != t2)


if __name__ == '__main__':
    unittest.main()
