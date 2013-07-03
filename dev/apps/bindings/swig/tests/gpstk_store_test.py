import unittest
import sys
# sys.path.append('../bin')
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
        suframe = [023222245L,14111111324L,4623626L,33333531536L,4126166634L,17845L,6317L,736162361L,83163L,91471L]
        e.addSubframe(suframe, 1)
        # e.dump()
        # e.getEcc(SatID(1))


class SP3EphemerisStoreTest(unittest.TestCase):
  def test_raw_data(self):
      s = SP3EphemerisStore()
      sat = SatID(1, SatID.systemGPS)
      time = CommonTime()
      time.addDays(10000)
      s.addPositionData(sat, time, Triple(50.0, -45.5, 20), Triple(1.0, 100.0, 5.0))
      s.addVelocityData(sat, time, Triple(1.0, 2.0, -10000.0), Triple(1.0, 100.0, 5.0))
      self.assertEqual(Triple(50000, -45500, 20000), s.getPosition(sat, time))
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


if __name__ == '__main__':
    unittest.main()
