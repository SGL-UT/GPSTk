import unittest
import sys
from gpstk import *


class WGS84EllipsoidTest(unittest.TestCase):
    def test(self):
        e = WGS84Ellipsoid()
        self.assertAlmostEqual(299792458.0, e.c())
        self.assertTrue(issubclass(WGS84Ellipsoid, EllipsoidModel))


class PZ90EllipsoidTest(unittest.TestCase):
    def test(self):
        p = PZ90Ellipsoid()
        self.assertAlmostEqual(6378136.0, p.a())
        self.assertTrue(issubclass(WGS84Ellipsoid, EllipsoidModel))


class URATest(unittest.TestCase):
    def test(self):
        self.assertEqual(3, accuracy2ura(5.352))
        self.assertEqual(15, SV_ACCURACY_GPS_MAX_INDEX_VALUE)


class EngAlmanacTest(unittest.TestCase):
    def test(self):
        e = EngAlmanac()
        # we need better testing here; not convinced python longs
        # are being mapped to the native C longs needed
        suframe = [23222245L, 14111111324L, 4623626L, 33333531536L, 41261634L,
                   17845L, 6317L, 736162361L, 83163L, 91471L]
        e.addSubframe(suframe, 1)
        # e.dump()
        # e.getEcc(SatID(1))


class SP3EphemerisStoreTest(unittest.TestCase):
  def test_raw_data(self):
      s = SP3EphemerisStore()
      sat = SatID(1, SatID.systemGPS)
      time = CommonTime()
      time.addDays(10000)
      s.addPositionData(sat, time, Triple(50, -45, 20), Triple(1, 100, 5))
      s.addVelocityData(sat, time, Triple(1, 2, -10000), Triple(1, 100, 5))
      self.assertEqual(Triple(50000, -45000, 20000), s.getPosition(sat, time))
  def test_file_data(self):
      pass


class BrcKeplerOrbitTest(unittest.TestCase):
    def test(self):
        t1 = CommonTime()
        t2 = CommonTime()
        t2.addDays(1)
        t3 = CommonTime()
        t3.addSeconds(60)
        b = BrcKeplerOrbit('GPS', ObsID(ObsID.otRange, ObsID.cbC6, ObsID.tcN),
                           10, t1,
                           t2, t3,
                           5, True,
                           0.0, 0.0,
                           0.0, 0.0,
                           0.0, 0.0,
                           0.0, 0.0,
                           0.0, 1.1,
                           0.0, 0.0,
                           0.0, 0.0,
                           0.0, 1.2,
                           1.3, 0.0)
        self.assertEqual(1.1, b.getEcc())
        self.assertEqual(1.2, b.getW())
        self.assertEqual(1.3, b.getOmegaDot())


class AlmOrbitTest(unittest.TestCase):
    def test(self):
        a = AlmOrbit(0, 0, 0, 1.5, 0, 0, 0, 50.5, 0, 0, 0, 3000000L, 1, 2)
        self.assertEqual(0.0, a.getAF0())
        self.assertEqual(0L, a.getFullWeek())


class GPSAlmanacStoreTest(unittest.TestCase):
    def test(self):
        e = EngAlmanac()
        suframe = [023222245L, 14111111324L, 4623626L, 33333531536L,
                   4126166634L, 17845L, 6317L, 736162361L, 83163L, 91471L]
        e.addSubframe(suframe, 1)
        g = GPSAlmanacStore()
        g.addAlmanac(e)


class SEMTest(unittest.TestCase):
    def test_fileIO(self):
        header, data = readSEM('sem_plot_data.txt')
        self.assertEqual(724, header.week)
        self.assertEqual(405504L, header.Toa)
        self.assertEqual(32, len(data))
        dataPoint = data[15]
        self.assertEqual(16, dataPoint.PRN)
        self.assertAlmostEqual(0.00711489, dataPoint.ecc)


class YumaTest(unittest.TestCase):
    def test_fileIO(self):
        header, data = readYuma('yuma_data.txt')
        self.assertEqual(31, len(data))
        dataPoint = data[10]
        self.assertAlmostEqual(0.0, dataPoint.AF1)
        self.assertEqual(11L, dataPoint.PRN)
        self.assertAlmostEqual(0.006191730499, dataPoint.ecc)
        self.assertEqual(377L, dataPoint.week)


class Rinex3ObsTest(unittest.TestCase):
    def test_fileIO(self):
        header, data = readRinex3Obs('bahr1620.04o')
        self.assertEqual(120, len(data))


if __name__ == '__main__':
    unittest.main()
