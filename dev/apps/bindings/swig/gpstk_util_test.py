import unittest
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
        r = ReferenceFrame(ReferenceFrames.Unknown)
        self.assertEqual('Unknown', r.__str__())

    def test_bad_input(self):
        r = ReferenceFrame('zzzzzzzzzzz')
        self.assertEqual('Unknown', r.__str__())

    def test_string_input(self):
        r = ReferenceFrame('PZ90')
        self.assertEqual('PZ90', r.__str__())

    def test_constant_input(self):
        r = ReferenceFrame(ReferenceFrames.WGS84)
        self.assertEqual('WGS84', r.__str__())


class SatID_test(unittest.TestCase):
    def test_validity(self):
        s = SatID(1, SatSystems.GPS)
        self.assertTrue(s.isValid())

    def test_invalid(self):
        s = SatID()  # creates an invalid SatID
        self.assertFalse(s.isValid())

    def test_str(self):
        a = SatID(3, SatSystems.Glonass)
        self.assertEqual('GLONASS 3', a.__str__())

        b = SatID(1, SatSystems.LEO)
        self.assertEqual('LEO 1', b.__str__())

        c = SatID(4)  # optional arg should be 'GPS'
        self.assertEqual('GPS 4', c.__str__())


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
        t = Triple(1, 2, 3)
        u = Triple(t)
        v = Triple(1, 2, 3)
        self.assertTrue(t == u)
        self.assertTrue(u == v)
        self.assertTrue(t == v)  # god help us if this fails

    def test_access(self):
        t = Triple(1.5, 2.0, -3.0)
        self.assertEqual(1.5, t[0])
        self.assertEqual(2.0, t[1])
        self.assertEqual(-3., t[2])

    def test_conversions(self):
        trip = Triple(1.5, 2.5, 3.5)
        tupl = (1.5, 2.5, 3.5)
        self.assertEqual(trip, makeTriple(tupl))
        self.assertEqual(tupl, makeTuple(trip))

    def test_operators(self):
        a = Triple(1.0, 2.0, 4.0)
        b = Triple(5.0, 6.0, 5.0)
        sum = Triple(6.0, 8.0, 9.0)
        self.assertEqual(sum, a + b)

    def test_methods(self):
        t = Triple(1.5, 2., -3.)
        u = Triple(10.0, 5, 2.0)
        self.assertEqual(15.25, t.dot(t))
        self.assertEqual(Triple(4, 6, 8), Triple(2, 3, 4).scale(2))
        self.assertEqual(3.905124837953327, t.mag())
        self.assertEqual(5.345455469884399, t.elvAngle(u))
        self.assertEqual(0.42837471052408865, t.cosVector(u))


class Position_test(unittest.TestCase):
    def test(self):
        p1 = Position(1.5, 6.2, 3.5)
        p2 = Position(1.0, 1.8, .5)
        self.assertEqual(5.348831648126533, range(p1, p2))

    def test_spherical_cartesian(self):
        orig = Triple(45, 30, 12)
        p = Position.convertSphericalToCartesian(orig)
        q = Position.convertCartesianToSpherical(p)
        expected = '(7.348469228349474, 4.242640687119164, 8.485281374238683)'
        self.assertEqual(expected, p.__str__())
        self.assertAlmostEqual(45, q[0])
        self.assertAlmostEqual(30, q[1])
        self.assertAlmostEqual(12, q[2])

    def test_cartesian_geodetic(self):
        a = PZ90Ellipsoid().a()
        eccSq = PZ90Ellipsoid().eccSquared()
        orig = Position(100000, 20000, 30000)
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
        orig = Position(40, 100, 2.5e5, CoordinateSystems.Geocentric)
        p = Position.convertGeocentricToGeodetic(orig, a, eccSq)
        q = Position.convertGeodeticToGeocentric(p, a, eccSq)
        self.assertAlmostEqual(44.90696703221949, p[0])
        self.assertAlmostEqual(100.0, p[1])
        self.assertAlmostEqual(-6118405.153409380, p[2])
        self.assertAlmostEqual(40.00000265961031, q[0])
        self.assertAlmostEqual(100, q[1])
        self.assertAlmostEqual(249998.49546297366, q[2])

    def test_cartesian_geocentric(self):
        p = Position.convertCartesianToGeocentric(Triple(4000, 5000, 7000))
        q = Position.convertGeocentricToCartesian(p)
        self.assertAlmostEqual(47.54984445710891, p[0])
        self.assertAlmostEqual(51.34019174590962, p[1])
        self.assertAlmostEqual(9486.832980505136, p[2])
        self.assertAlmostEqual(4000, q[0])
        self.assertAlmostEqual(5000, q[1])
        self.assertAlmostEqual(7000, q[2])

    def test_functions(self):
        system = CoordinateSystems.Cartesian
        ell = PZ90Ellipsoid()
        frame = ReferenceFrame(ReferenceFrames.PZ90)
        p = Position(10000, 150000, 200000, system, ell, frame)
        q = Position(20000, 160000, 190000, system, ell, frame)
        self.assertAlmostEqual(1.32756277187, q.elevation(p))
        self.assertAlmostEqual(86.18592516570916, p.getPhi())
        self.assertAlmostEqual(57.5141089193572, p.geodeticLatitude())
        self.assertAlmostEqual(10000., p.X())


class ObsID_test(unittest.TestCase):
    def test(self):
        o1 = ObsID(ObsID.otRange, ObsID.cbC6, ObsID.tcN)
        o2 = ObsID(ObservationTypes.Range, CarrierBands.C6, TrackingCodes.N)
        self.assertEqual('C6 GPSsquare pseudorange', o1.__str__())
        self.assertEqual(o1, o2)


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

    def test_vector_iter(self):
        v = std_vector_int()
        v.push_back(5)
        v.push_back(3)
        v.push_back(10)
        iterations = 0
        for x in v:
            if iterations == 0:
                self.assertEqual(5, x)
            elif iterations == 1:
                self.assertEqual(3, x)
            elif iterations == 2:
                self.assertEqual(10, x)
            else:
                self.fail()
            iterations += 1

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
        expected = 'x:(1.5, 2.5, 3.5), v:(500.0, 1000.0, -100.0)'
        self.assertEqual(expected, data.__str__())


class Xvt_test(unittest.TestCase):
    # TODO: this needs more consideration
    def test(self):
        data = Xvt()
        self.assertAlmostEqual(0, data.computeRelativityCorrection())
        self.assertAlmostEqual(0, data.getClockBias())


if __name__ == '__main__':
    unittest.main()
