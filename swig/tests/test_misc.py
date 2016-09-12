#!/usr/bin/env python

import unittest, sys, os
sys.path.insert(0, os.path.abspath(".."))
from gpstk.test_utils import args,run_unit_tests
import gpstk


class ReferenceFrame_test(unittest.TestCase):
    def test_unknown(self):
        r = gpstk.ReferenceFrame()
        self.assertEqual('Unknown', str(r))

    def test_string_input(self):
        r = gpstk.ReferenceFrame('PZ90')
        self.assertEqual('PZ90', str(r))

    def test_constant_input(self):
        r = gpstk.ReferenceFrame(gpstk.ReferenceFrame.WGS84)
        self.assertEqual('WGS84', str(r))


class SatID_test(unittest.TestCase):
    def test_validity(self):
        s = gpstk.SatID(1, gpstk.SatID.systemGPS)
        self.assertTrue(s.isValid())

    def test_invalid(self):
        s = gpstk.SatID()  # creates an invalid SatID (id=-1)
        self.assertFalse(s.isValid())

    def test_str(self):
        a = gpstk.SatID(3, gpstk.SatID.systemGlonass)
        self.assertEqual('GLONASS 3', str(a))

        b = gpstk.SatID(1, gpstk.SatID.systemLEO)
        self.assertEqual('LEO 1', str(b))

        c = gpstk.SatID(4)  # optional arg should be SatID.systemGPS
        self.assertEqual('GPS 4', str(c))


class Triple_test(unittest.TestCase):
    def test_copy_constructor(self):
        t = gpstk.Triple(1.0, 2.0, 3.0)
        u = gpstk.Triple(t)
        v = gpstk.Triple(1.0, 2.0, 3.0)
        self.assertTrue(t == u)
        self.assertTrue(u == v)

    def test_access(self):
        t = gpstk.Triple(1.5, 2.0, -3.0)
        self.assertEqual(1.5, t[0])
        self.assertEqual(2.0, t[1])
        self.assertEqual(-3.0, t[2])

    def test_operators(self):
        a = gpstk.Triple(1.0, 2.0, 4.0)
        b = gpstk.Triple(5.0, 6.0, 5.0)
        #      +   --------------
        c = gpstk.Triple(6.0, 8.0, 9.0)
        self.assertEqual(c, a + b)

    def test_methods(self):
        t = gpstk.Triple(1.5, 2.0, -3.0)
        u = gpstk.Triple(10.0, 5.0, 2.0)
        self.assertEqual(15.25, t.dot(t))

        expected = gpstk.Triple(4.0, 6.0, 8.0)
        self.assertEqual(expected, gpstk.Triple(2.0, 3.0, 4.0).scale(2.0))

        self.assertAlmostEqual(3.905124837953327, t.mag())
        self.assertAlmostEqual(5.345455469884399, t.elvAngle(u))
        self.assertAlmostEqual(0.42837471052408865, t.cosVector(u))


class Position_test(unittest.TestCase):
    def test(self):
        p1 = gpstk.Position(1.5, 6.2, 3.5)
        p2 = gpstk.Position(1.0, 1.8, 0.5)
        self.assertAlmostEqual(5.348831648126533, gpstk.range(p1, p2))

    def test_spherical_cartesian(self):
        orig = gpstk.Triple(45.0, 30.0, 12.0)
        p = gpstk.Position.convertSphericalToCartesian(orig)
        q = gpstk.Position.convertCartesianToSpherical(p)
        self.assertAlmostEqual(45.0, q[0], places=4)
        self.assertAlmostEqual(30.0, q[1], places=4)
        self.assertAlmostEqual(12.0, q[2], places=4)

    def test_cartesian_geodetic(self):
        a = gpstk.PZ90Ellipsoid().a()
        eccSq = gpstk.PZ90Ellipsoid().eccSquared()
        orig = gpstk.Position(100000.0, 20000.0, 30000.0)
        p = gpstk.Position.convertCartesianToGeodetic(orig, a, eccSq)
        q = gpstk.Position.convertGeodeticToCartesian(p, a, eccSq)
        self.assertAlmostEqual(25.33498527029081, p[0], places=4)
        self.assertAlmostEqual(11.30993247402015, p[1], places=4)
        self.assertAlmostEqual(-6269217.08416736, p[2], places=4)
        self.assertAlmostEqual(99999.26269737557, q[0], places=4)
        self.assertAlmostEqual(19999.85253947465, q[1], places=4)
        self.assertAlmostEqual(29999.83821484564, q[2], places=4)

    def test_geocentric_geodetic(self):
        a = gpstk.PZ90Ellipsoid().a()
        eccSq = gpstk.PZ90Ellipsoid().eccSquared()
        orig = gpstk.Position(40.0, 100.0, 2.5e5, gpstk.Position.Geocentric)
        p = gpstk.Position.convertGeocentricToGeodetic(orig, a, eccSq)
        q = gpstk.Position.convertGeodeticToGeocentric(p, a, eccSq)
        self.assertAlmostEqual(44.90696703221949, p[0], places=4)
        self.assertAlmostEqual(100.0, p[1], places=4)
        self.assertAlmostEqual(-6118405.153409380, p[2], places=4)
        self.assertAlmostEqual(40.00000265961031, q[0], places=4)
        self.assertAlmostEqual(100.0, q[1], places=4)
        self.assertAlmostEqual(249998.49546297366, q[2], places=4)

    def test_cartesian_geocentric(self):
        orig = gpstk.Triple(4000.0, 5000.0, 7000.0)
        p = gpstk.Position.convertCartesianToGeocentric(orig)
        q = gpstk.Position.convertGeocentricToCartesian(p)
        self.assertAlmostEqual(47.54984445710891, p[0], places=4)
        self.assertAlmostEqual(51.34019174590962, p[1], places=4)
        self.assertAlmostEqual(9486.832980505136, p[2], places=4)
        self.assertAlmostEqual(orig[0], q[0], places=4)
        self.assertAlmostEqual(orig[1], q[1], places=4)
        self.assertAlmostEqual(orig[2], q[2], places=4)

    def test_functions(self):
        system = gpstk.Position.Cartesian
        ell = gpstk.PZ90Ellipsoid()
        frame = gpstk.ReferenceFrame(gpstk.ReferenceFrame.PZ90)
        p = gpstk.Position(10000.0, 150000.0, 200000.0, system, ell, frame)
        q = gpstk.Position(20000.0, 160000.0, 190000.0, system, ell, frame)
        self.assertAlmostEqual(1.32756277187, q.elevation(p))
        self.assertAlmostEqual(86.18592516570916, p.getPhi())
        self.assertAlmostEqual(57.5141089193572, p.geodeticLatitude())
        self.assertAlmostEqual(10000.0, p.X())

    def test_helpers(self):
        p = gpstk.cartesian(100, 200, 300)
        self.assertEqual(gpstk.Position.Cartesian, p.getCoordinateSystem())
        p = gpstk.spherical(45, 60, 100000, model=gpstk.PZ90Ellipsoid())
        self.assertEqual(gpstk.Position.Spherical, p.getCoordinateSystem())
        p = gpstk.geodetic(frame=gpstk.ReferenceFrame('WGS84'))
        self.assertEqual(gpstk.Position.Geodetic, p.getCoordinateSystem())
        p = gpstk.geocentric(latitude=60, radius=10000)
        self.assertEqual(gpstk.Position.Geocentric, p.getCoordinateSystem())


class GPS_URA_test(unittest.TestCase):
    def test(self):
        self.assertEqual(15, gpstk.SV_ACCURACY_GPS_MAX_INDEX_VALUE)
        self.assertEqual(9, gpstk.accuracy2ura(100.1))
        self.assertEqual(0.0, gpstk.sv_accuracy_gps_min_index(0))
        self.assertEqual(2.4, gpstk.sv_accuracy_gps_min_index(1))
        self.assertRaises(gpstk.IndexOutOfBoundsException, gpstk.sv_accuracy_gps_min_index, 16)


class ObsID_test(unittest.TestCase):
    def test(self):
        o1 = gpstk.ObsID(gpstk.ObsID.otRange, gpstk.ObsID.cbAny, gpstk.ObsID.tcA)
        self.assertEqual('AnyBand GALA pseudorange', str(o1))


class std_template_test(unittest.TestCase):
    def test_vector(self):
        v = gpstk.vector_int()
        v.push_back(5)
        v.push_back(3)
        v.push_back(10)
        self.assertEqual(5, v[0])
        self.assertEqual(3, v[1])
        self.assertEqual(10, v[2])
        v = gpstk.vector_double()
        v.push_back(1.5)
        v.push_back(2.5)
        self.assertEqual(1.5, v[0])
        self.assertEqual(2.5, v[1])
        self.assertEqual(2, len(v))

    def test_vector_iter(self):
        v = gpstk.vector_int()
        v.push_back(5)
        v.push_back(3)
        v.push_back(10)
        i = 0
        for x in v:
            self.assertEqual(v[i], x)
            i += 1

    def test_vector_conversions(self):
        def same_seq(seq1, seq2):
            self.assertEqual(len(seq1), len(seq2))
            for i in range(len(seq1)):
                self.assertEqual(seq1[i], seq2[i])

        list = [1.1, 2.2, 3.3]
        v = gpstk.seqToVector(list)
        self.assertIsInstance(v, gpstk.vector_double)
        same_seq(list, v)

        list = [1.1, 2.2, 3.3]
        v = gpstk.seqToVector(list, outtype='vector_double')
        self.assertIsInstance(v, gpstk.vector_double)
        same_seq(list, v)

        list = ['bar!', 'foo?']
        v = gpstk.seqToVector(list)
        self.assertIsInstance(v, gpstk.vector_string)
        same_seq(list, v)

        v = gpstk.vector_int()
        v.push_back(3)
        v.push_back(5)
        list = gpstk.vectorToSeq(v)
        same_seq(list, v)

        list = [1.1, 2.2, 3.3]
        self.assertRaises(TypeError, gpstk.seqToVector, list, 'vector_doesnotexist')

        list = [1, 2.2, 'c']  # mismatching types not allowed
        self.assertRaises(TypeError, gpstk.seqToVector, list)

        list = [1000L, 2000L]  # PyLongs are not templated
        self.assertRaises(TypeError, gpstk.seqToVector, list)

    def test_map(self):
        map = gpstk.map_int_char()
        map[1] = 'A'
        map[100] = 'z'
        self.assertEqual('A', map[1])
        self.assertEqual('z', map[100])
        map = gpstk.map_string_int()
        map['key1'] = 123
        map['key2'] = 321
        self.assertEqual(123, map['key1'])
        self.assertEqual(321, map['key2'])

    def test_map_iter(self):
        map = gpstk.map_int_string()
        map[5] = 'five'
        map[6] = 'six'
        list = []
        for key in map:
            list.append(key)
        self.assertEqual([5,6], sorted(list))

    def test_map_len(self):
        map = gpstk.map_int_string()
        map[5] = 'five'
        map[6] = 'six'
        self.assertEqual(2, len(map))

    def test_map_conversions(self):
        def same(a, b):
            self.assertEqual(len(a), len(b))
            for x in a:
                self.assertEqual(a[x], b[x])

        d = {1: 'A', 2: 'B', 3: 'C'}
        m = gpstk.dictToMap(d)
        same(d, m)
        self.assertIsInstance(m, gpstk.map_int_string)
        same(d, gpstk.mapToDict(m))

        d = {'A': 1.1, 'B': 2.2, 'C': 3.3}
        m = gpstk.dictToMap(d)
        same(d, m)
        self.assertIsInstance(m, gpstk.map_string_double)
        same(d, gpstk.mapToDict(m))

        d = {'A': 1, 'B': 1.1}
        self.assertRaises(TypeError, gpstk.dictToMap, d)


class convhelp_test(unittest.TestCase):
    def test(self):
        self.assertAlmostEqual(32.0, gpstk.cel2far(0.0))
        self.assertAlmostEqual(0.0, gpstk.far2cel(32.0))
        self.assertAlmostEqual(121.1, gpstk.cel2far(49.5))


class Xv_test(unittest.TestCase):
    def test(self):
        data = gpstk.Xv()
        data.x = gpstk.Triple(1.5, 2.5, 3.5)
        data.v = gpstk.Triple(500, 1000, -100)
        self.assertEqual(1.5, data.x[0])
        expected = 'x:(1.5, 2.5, 3.5), v:(500, 1000, -100)'
        self.assertEqual(expected, str(data))


class Xvt_test(unittest.TestCase):
    def test(self):
        data = gpstk.Xvt()
        data.x = gpstk.Triple(1000.0, 2000.0, 1500.0)
        data.v = gpstk.Triple(50.0, 25.0, -500.0)
        data.clkbias = 0.0001
        data.clkdrift = 0.05
        data.relcorr = 0.83
        data.frame = gpstk.ReferenceFrame(gpstk.ReferenceFrame.WGS84)
        self.assertAlmostEqual(0.0001, data.getClockBias())

        expected = 1.446445072869704e-11
        self.assertAlmostEqual(expected, data.computeRelativityCorrection())

        expected = ('x:(1000, 2000, 1500), v:(50, 25, -500), clk bias:0.0001, '
                    'clk drift:0.05, relcorr:1.44645e-11')
        self.assertEqual(expected, str(data))


class AstronomicalFunctions_test(unittest.TestCase):
    def test_functions(self):
        c = gpstk.CommonTime()
        c.addSeconds(12345678)
        self.assertAlmostEqual(10.934294925420545, gpstk.UTC2SID(c))

    def test_constants(self):
        self.assertAlmostEqual(1.49597870e11, gpstk.AU_CONST)
        self.assertAlmostEqual(0.0174532925199432957692369, gpstk.D2R)
        self.assertAlmostEqual(9.80665,  gpstk.EarthGrav)


class Positioning_test(unittest.TestCase):
    def test_moon(self):
        t = gpstk.CivilTime(2000).toCommonTime()
        # object way:
        pos = gpstk.MoonPosition().getPosition(t)
        self.assertAlmostEqual(-89651219.03579193, pos[0])
        # functional way:
        pos = gpstk.moonPosition(t)
        self.assertAlmostEqual(-89651219.03579193, pos[0])

    def test_sun(self):
        t = gpstk.CivilTime(2000).toCommonTime()
        # object way:
        pos = gpstk.SunPosition().getPosition(t)
        self.assertAlmostEqual(-136909966557.8461 , pos[0], places=3)
        # functional way:
        pos = gpstk.sunPosition(t)
        self.assertAlmostEqual(-136909966557.8461 , pos[0], places=3)


class Tides_test(unittest.TestCase):
    def test_pole_tides(self):
        t = gpstk.CivilTime(2000).toCommonTime()
        p = gpstk.Position(1000.0, 2000.0, 3000.0)
        x = 5.0
        y = 10.0
        trip = gpstk.poleTides(t, p, x, y)
        self.assertAlmostEqual(-0.03128457731297798, trip[0])

    def test_solid_tides(self):
        t = gpstk.CivilTime(2000).toCommonTime()
        p = gpstk.Position(1000.0, 2000.0, 3000.0)
        trip = gpstk.solidTides(t, p)
        self.assertAlmostEqual(-2.2479508782610997e-15, trip[0])


class Expression_test(unittest.TestCase):
    def test_class(self):
        e = gpstk.Expression('1 + 1')
        self.assertAlmostEqual(2.0, e.evaluate())

        e = gpstk.Expression('1.12*(4.0/2.0 - 0.1)')
        self.assertAlmostEqual(2.128, e.evaluate())

        e = gpstk.Expression('1 + 2*x')
        e.set('x', 5.5)
        self.assertAlmostEqual(12.0, e.evaluate())

        e = gpstk.Expression('L1 + L2')
        e.setGPSConstants();
        self.assertAlmostEqual(2803020000.0, e.evaluate())

    def test_function(self):
        self.assertAlmostEqual(2.0, gpstk.eval('1 + 1'))
        self.assertAlmostEqual(2.128, gpstk.eval('1.12*(4.0/2.0 - 0.1)'))
        self.assertAlmostEqual(12.0, gpstk.eval('1 + 2*x', x=5.5))
        self.assertAlmostEqual(2803020000.0, gpstk.eval('L1 + L2'))

        e = gpstk.Expression('1 + 2*x')
        self.assertAlmostEqual(12.0, gpstk.eval(e, x=5.5))
 

if __name__ == '__main__':
    run_unit_tests()
