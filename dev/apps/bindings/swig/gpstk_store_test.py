import unittest
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
        self.assertEqual(15, SV_ACCURACY_GPS_MAX_INDEX_VALUE)


class EngAlmanacTest(unittest.TestCase):
    def test(self):
        pass
        # TODO: move inner exception macros outside in c++ source


        # TODO: EngAlmanac testing
        e = EngAlmanac()
        # e.getEcc(SatID(1))


# class SP3EphemerisStoreTest(unittest.TestCase):
#   def test_raw_data(self):
#       s = SP3EphemerisStore()
#       sat = SatID(1, SatSystems.GPS)
#       time = CommonTime()
#       time.addDays(10000)
#       s.addPositionData(sat, time, Triple(50.0, -45.5, 20), Triple(1.0, 100.0, 5.0))
#       s.addVelocityData(sat, time, Triple(1.0, 2.0, -10000.0), Triple(1.0, 100.0, 5.0))
#       self.assertEqual(Triple(50000, -45500, 20000), s.getPosition(sat, time))
#   def test_file_data(self):
#       pass


if __name__ == '__main__':
    unittest.main()
