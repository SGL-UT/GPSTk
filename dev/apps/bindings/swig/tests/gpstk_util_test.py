import unittest
import sys
from gpstk import *


class GPS_consants_test(unittest.TestCase):
    def test_constants(self):
        self.assertEqual(32, MAX_PRN)
        self.assertEqual(32, MAX_PRN_GPS)


class Geometry_test(unittest.TestCase):
    def test_constants(self):
        self.assertEqual(1.7453292519943e-2, DEG_TO_RAD)
        self.assertEqual(57.295779513082, RAD_TO_DEG)


class ReferenceFrame_test(unittest.TestCase):
    def test_unknown(self):
        r = ReferenceFrame()
        self.assertEqual('Unknown', str(r))

    def test_string_input(self):
        r = ReferenceFrame(ReferenceFrame.PZ90)
        self.assertEqual('PZ90', str(r))

    def test_constant_input(self):
        r = ReferenceFrame(ReferenceFrame.WGS84)
        self.assertEqual('WGS84', str(r))


class SatID_test(unittest.TestCase):
    def test_validity(self):
        s = SatID(1, SatID.systemGPS)
        self.assertTrue(s.isValid())

    def test_invalid(self):
        s = SatID()  # creates an invalid SatID (id=-1)
        self.assertFalse(s.isValid())

    def test_str(self):
        a = SatID(3, SatID.systemGlonass)
        self.assertEqual('GLONASS 3', str(a))

        b = SatID(1, SatID.systemLEO)
        self.assertEqual('LEO 1', str(b))

        c = SatID(4)  # optional arg should be SatSystems.GPS
        self.assertEqual('GPS 4', str(c))


class GNSSconstants_test(unittest.TestCase):
    def test_constants(self):
        self.assertEqual(PI, 3.1415926535898)
        self.assertEqual(OSC_FREQ_GPS, 10.23e6)
        self.assertEqual(L6_FREQ_GAL, 1278.75e6)

    def test_functions(self):
        self.assertEqual(4, getLegacyFitInterval(15, 27))
        self.assertEqual(0.190293672798, getWavelength(SatID(1, 1), 1))
        self.assertEqual(1.2833333333333334, getBeta(SatID(1, 1), 1, 2))
        self.assertEqual(0.6469444444444448, getAlpha(SatID(1, 1), 1, 2))


class Triple_test(unittest.TestCase):
    def test_copy_constructor(self):
        t = Triple(1.0, 2.0, 3.0)
        u = Triple(t)
        v = Triple(1.0, 2.0, 3.0)
        self.assertTrue(t == u)
        self.assertTrue(u == v)

    def test_access(self):
        t = Triple(1.5, 2.0, -3.0)
        self.assertEqual(1.5, t[0])
        self.assertEqual(2.0, t[1])
        self.assertEqual(-3.0, t[2])

    def test_conversions(self):
        trip = Triple(1.5, 2.5, 3.5)
        tupl = (1.5, 2.5, 3.5)
        self.assertEqual(trip, makeTriple(tupl))
        self.assertEqual(tupl, makeTuple(trip))

    def test_operators(self):
        a = Triple(1.0, 2.0, 4.0)
        b = Triple(5.0, 6.0, 5.0)
        #      +   --------------
        c = Triple(6.0, 8.0, 9.0)
        self.assertEqual(c, a + b)

    def test_methods(self):
        t = Triple(1.5, 2.0, -3.0)
        u = Triple(10.0, 5.0, 2.0)
        self.assertEqual(15.25, t.dot(t))
        self.assertEqual(Triple(4.0, 6.0, 8.0), Triple(2.0, 3.0, 4.0).scale(2.0))
        self.assertEqual(3.905124837953327, t.mag())
        self.assertEqual(5.345455469884399, t.elvAngle(u))
        self.assertEqual(0.42837471052408865, t.cosVector(u))


class Position_test(unittest.TestCase):
    def test(self):
        p1 = Position(1.5, 6.2, 3.5)
        p2 = Position(1.0, 1.8, 0.5)
        self.assertEqual(5.348831648126533, rangeBetween(p1, p2))

    def test_spherical_cartesian(self):
        orig = Triple(45.0, 30.0, 12.0)
        p = Position.convertSphericalToCartesian(orig)
        q = Position.convertCartesianToSpherical(p)
        expected = '(7.348469228349474, 4.242640687119164, 8.485281374238683)'
        self.assertEqual(expected, str(p))
        self.assertAlmostEqual(45.0, q[0])
        self.assertAlmostEqual(30.0, q[1])
        self.assertAlmostEqual(12.0, q[2])

    def test_cartesian_geodetic(self):
        a = PZ90Ellipsoid().a()
        eccSq = PZ90Ellipsoid().eccSquared()
        orig = Position(100000.0, 20000.0, 30000.0)
        p = Position.convertCartesianToGeodetic(orig, a, eccSq)
        q = Position.convertGeodeticToCartesian(p, a, eccSq)
        self.assertAlmostEqual(25.33498527029081, p[0])
        self.assertAlmostEqual(11.30993247402015, p[1])
        self.assertAlmostEqual(-6269217.08416736, p[2])
        self.assertAlmostEqual(99999.26269737557, q[0])
        self.assertAlmostEqual(19999.85253947465, q[1])
        self.assertAlmostEqual(29999.83821484564, q[2])

    def test_geocentric_geodetic(self):
        a = PZ90Ellipsoid().a()
        eccSq = PZ90Ellipsoid().eccSquared()
        orig = Position(40.0, 100.0, 2.5e5, Position.Geocentric)
        p = Position.convertGeocentricToGeodetic(orig, a, eccSq)
        q = Position.convertGeodeticToGeocentric(p, a, eccSq)
        self.assertAlmostEqual(44.90696703221949, p[0])
        self.assertAlmostEqual(100.0, p[1])
        self.assertAlmostEqual(-6118405.153409380, p[2])
        self.assertAlmostEqual(40.00000265961031, q[0])
        self.assertAlmostEqual(100.0, q[1])
        self.assertAlmostEqual(249998.49546297366, q[2])

    def test_cartesian_geocentric(self):
        orig = Triple(4000.0, 5000.0, 7000.0)
        p = Position.convertCartesianToGeocentric(orig)
        q = Position.convertGeocentricToCartesian(p)
        self.assertAlmostEqual(47.54984445710891, p[0])
        self.assertAlmostEqual(51.34019174590962, p[1])
        self.assertAlmostEqual(9486.832980505136, p[2])
        self.assertAlmostEqual(orig[0], q[0])
        self.assertAlmostEqual(orig[1], q[1])
        self.assertAlmostEqual(orig[2], q[2])

    def test_functions(self):
        system = Position.Cartesian
        ell = PZ90Ellipsoid()
        frame = ReferenceFrame(ReferenceFrame.PZ90)
        p = Position(10000.0, 150000.0, 200000.0, system, ell, frame)
        q = Position(20000.0, 160000.0, 190000.0, system, ell, frame)
        self.assertAlmostEqual(1.32756277187, q.elevation(p))
        self.assertAlmostEqual(86.18592516570916, p.getPhi())
        self.assertAlmostEqual(57.5141089193572, p.geodeticLatitude())
        self.assertAlmostEqual(10000.0, p.X())


class ObsID_test(unittest.TestCase):
    def test(self):
        o1 = ObsID(ObsID.otRange, ObsID.cbC6, ObsID.tcN)
        self.assertEqual('C6 GPSsquare pseudorange', str(o1))


class Vector_test(unittest.TestCase):
    def test_standard_double(self):
        v = vector(5, 3.0)  # 3 3 3 3 3
        self.assertAlmostEqual(3.0, v[0])
        self.assertAlmostEqual(3.0, v[1])
        self.assertAlmostEqual(3.0, v[2])
        self.assertAlmostEqual(3.0, v[3])
        self.assertAlmostEqual(3.0, v[4])

    def test_concatenate(self):
        u = vector(1, 2.5)
        v = vector(1, 3.5)
        u.concatenate(v)
        self.assertAlmostEqual(2.5, u[0])
        self.assertAlmostEqual(3.5, u[1])
        self.assertAlmostEqual(3.5, v[0])  # v should be unchanged

    def test_string(self):
        v = vector(5, 3.0)
        self.assertEqual('3, 3, 3, 3, 3', str(v))

    def test_from_stdvector_double(self):
        tmp = std_vector_double(5)
        tmp[0] = 1.23
        tmp[1] = 2.34
        tmp[2] = 3.45
        tmp[3] = 4.56
        tmp[4] = 5.67
        v = vector(tmp)
        self.assertAlmostEqual(1.23, v[0])
        self.assertAlmostEqual(2.34, v[1])
        self.assertAlmostEqual(3.45, v[2])
        self.assertAlmostEqual(4.56, v[3])
        self.assertAlmostEqual(5.67, v[4])

    def test_iter(self):
        v = vector(3, 2.5)
        i = 0
        for x in v:
            self.assertAlmostEqual(v[i], x)
            i += 1


class std_template_test(unittest.TestCase):
    def test_vector(self):
        v = std_vector_int()
        v.push_back(5)
        v.push_back(3)
        v.push_back(10)
        self.assertEqual(5, v[0])
        self.assertEqual(3, v[1])
        self.assertEqual(10, v[2])
        v = std_vector_double()
        v.push_back(1.5)
        v.push_back(2.5)
        self.assertEqual(1.5, v[0])
        self.assertEqual(2.5, v[1])
        self.assertEqual(2, len(v))

    def test_vector_iter(self):
        v = std_vector_int()
        v.push_back(5)
        v.push_back(3)
        v.push_back(10)
        i = 0
        for x in v:
            self.assertEqual(v[i], x)
            i += 1

    def test_map(self):
        map = map_int_char()
        map[1] = 'A'
        map[100] = 'z'
        self.assertEqual('A', map[1])
        self.assertEqual('z', map[100])
        map = map_string_int()
        map['key1'] = 123
        map['key2'] = 321
        self.assertEqual(123, map['key1'])
        self.assertEqual(321, map['key2'])

    def test_map_iter(self):
        map = map_int_string()
        map[5] = 'five'
        map[6] = 'six'
        list = []
        for key in map:
            list.append(key)
        self.assertEqual([5,6], sorted(list))

    def test_map_len(self):
        map = map_int_string()
        map[5] = 'five'
        map[6] = 'six'
        self.assertEqual(2, len(map))


class convhelp_test(unittest.TestCase):
    def test(self):
        self.assertAlmostEqual(32.0, cel2far(0))
        self.assertAlmostEqual(121.1, cel2far(49.5))


class Xv_test(unittest.TestCase):
    def test(self):
        data = Xv()
        data.x = Triple(1.5, 2.5, 3.5)
        data.v = Triple(500, 1000, -100)
        self.assertEqual(1.5, data.x[0])
        expected = 'x:(1.5, 2.5, 3.5), v:(500, 1000, -100)'
        self.assertEqual(expected, str(data))


class Xvt_test(unittest.TestCase):
    def test(self):
        data = Xvt()
        data.x = Triple(1000.0, 2000.0, 1500.0)
        data.v = Triple(50.0, 25.0, -500.0)
        data.clkbias = 0.0001
        data.clkdrift = 0.05
        data.relcorr = 0.83
        data.frame = ReferenceFrame(ReferenceFrame.WGS84)
        self.assertAlmostEqual(0.0001, data.getClockBias())

        expected = 1.446445072869704e-11
        self.assertAlmostEqual(expected, data.computeRelativityCorrection())

        expected = ('x:(1000, 2000, 1500), v:(50, 25, -500), clk bias:0.0001, '
                    'clk drift:0.05, relcorr:1.44645e-11')
        self.assertEqual(expected, str(data))


class AstronomicalFunctions_test(unittest.TestCase):
    def test_functions(self):
        c = CommonTime()
        c.addSeconds(12345678)
        self.assertAlmostEqual(10.934294925420545, UTC2SID(c))

    def test_constants(self):
        self.assertAlmostEqual(1.49597870e11, AU_CONST)
        self.assertAlmostEqual(0.0174532925199432957692369, D2R)
        self.assertAlmostEqual(9.80665,  EarthGrav)


if __name__ == '__main__':
    unittest.main()
