import unittest
from gpstk import *

class gps_consants_test(unittest.TestCase):
	def test(self):
		self.assertEqual(32, MAX_PRN)
		self.assertEqual(32, MAX_PRN_GPS)

class geometry_test(unittest.TestCase):
	def test(self):
		self.assertEqual(1.7453292519943e-2, DEG_TO_RAD)
		self.assertEqual(57.295779513082, RAD_TO_DEG)

class SatID_test(unittest.TestCase):
	def test_SatelliteSystemEnum(self):
		self.assertEqual(SatelliteSystems['GPS'],         SatID.systemGPS)
		self.assertEqual(SatelliteSystems['Galileo'],     SatID.systemGalileo)
		self.assertEqual(SatelliteSystems['Glonass'],     SatID.systemGlonass)
		self.assertEqual(SatelliteSystems['Geosync' ],    SatID.systemGeosync)
		self.assertEqual(SatelliteSystems['LEO'],         SatID.systemLEO)
		self.assertEqual(SatelliteSystems['Transit'],     SatID.systemTransit)
		self.assertEqual(SatelliteSystems['Compass'], 	SatID.systemCompass)
		self.assertEqual(SatelliteSystems['Mixed'],       SatID.systemMixed)
		self.assertEqual(SatelliteSystems['UserDefined'], SatID.systemUserDefined)
		self.assertEqual(SatelliteSystems['Unknown'],     SatID.systemUnknown)
	def test_normal(self):
		s = SatID(1, SatelliteSystems['GPS'])
		self.assertTrue(s.isValid())
	def test_invalid(self):
		s = SatID() # creates an invalid SatID
		self.assertFalse(s.isValid())
	def test_str(self):
		s = SatID(5, SatelliteSystems['Galileo'])
		self.assertEqual('Galileo 5', s.__str__())
	def test_helper_factory(self):
		a = makeSatID(3, 'Glonass')
		b = makeSatID(1, 'LEO')
		c = makeSatID(4)
		self.assertEqual('GLONASS 3', a.__str__())
		self.assertEqual('LEO 1', b.__str__())
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
	def test(self):
		t = Triple(1.5, 2., -3.)
		self.assertEqual(1.5*1.5 + 2.*2. + 3.*3., t.dot(t))
		self.assertEqual(1.5, t[0])
		self.assertEqual(Triple(6.,8.,9.), Triple(1.,2.,4.)+Triple(5.,6.,5.))
		self.assertEqual(Triple(4,6,8), Triple(2,3,4).scale(2))
		self.assertEqual(Triple(1.5, 2.5, 3.5), makeTriple((1.5, 2.5, 3.5)))
		self.assertEqual((1.5, 2.5, 3.5), makeTuple(makeTriple((1.5, 2.5, 3.5))))

class Position_test(unittest.TestCase):
	def test(self):
		p1 = Position(1.5, 6.2, 3.5)
		p2 = Position(1.0, 1.8, .5)
		self.assertEqual(5.348831648126533, range(p1, p2))
		p3 = Position(1.1, 1.2, 1.3, CoordinateSystems['Unknown']) # making sure coordinatesystems dict is built
	def test_spherical_cartesian(self):
		p = Position.convertSphericalToCartesian(Triple(45,30,12))
		q = Position.convertCartesianToSpherical(p)
		self.assertEqual('(7.348469228349474, 4.242640687119164, 8.485281374238683)', p.__str__())
		self.assertAlmostEqual(45, q[0])
		self.assertAlmostEqual(30, q[1])
		self.assertAlmostEqual(12, q[2])
	def test_cartesian_geocentric(self):
		p = Position.convertCartesianToGeocentric(Triple(4000,5000,7000))
		q = Position.convertGeocentricToCartesian(p)
		self.assertAlmostEqual(47.54984445710891, p[0])
		self.assertAlmostEqual(51.34019174590962, p[1])
		self.assertAlmostEqual(9486.832980505136, p[2])
		self.assertAlmostEqual(4000, q[0])
		self.assertAlmostEqual(5000, q[1])
		self.assertAlmostEqual(7000, q[2])
	def test_functions(self):
		p = Position(10000, 150000, 200000, CoordinateSystems['Cartesian'], PZ90Ellipsoid(), makeReferenceFrame('PZ90'))
		q = Position(20000, 160000, 190000, CoordinateSystems['Cartesian'], PZ90Ellipsoid(), makeReferenceFrame('PZ90'))
		self.assertAlmostEqual(1.32756277187, q.elevation(p))
		self.assertAlmostEqual(86.18592516570916, p.getPhi())
		self.assertAlmostEqual(57.5141089193572, p.geodeticLatitude())
		self.assertAlmostEqual(10000., p.X())

class ObsID_test(unittest.TestCase):
	def setUp(self):
		# four equivalent ways of making a ObsID
		self.o1 = ObsID(ObsID.otRange, ObsID.cbC6, ObsID.tcN)
		self.o2 = ObsID(ObservationTypes['Range'], CarrierBands['C6'], TrackingCodes['N'])
		self.o3 = makeObsID('Range', 'C6', 'N')
		self.o4 = makeObsID(carrierBand='C6', observationType='Range', trackingCode='N')
	def test_equality(self):
		self.assertEqual(self.o1, self.o2)
		self.assertEqual(self.o2, self.o3)
		self.assertEqual(self.o3, self.o4)
	def test_translate(self):
		self.assertEqual('C6 GPSsquare pseudorange', self.o1.__str__())


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
		self.assertAlmostEqual(49.5, far2cel(cel2far(49.5)))

class Xv_test(unittest.TestCase):
	def test(self):
		data = Xv()
		data.x = Triple(1.5, 2.5, 3.5)
		data.v = Triple(500,1000,-100)
		self.assertEqual(1.5, data.x[0])
		self.assertEqual(3.5, data.x[2])
		self.assertEqual(1000, data.v[1])
		self.assertEqual("x:(1.5, 2.5, 3.5), v:(500.0, 1000.0, -100.0)", data.__str__())

class Xvt_test(unittest.TestCase):
	def test(self):
		data = Xvt()
		self.assertAlmostEqual(0, data.computeRelativityCorrection())
		self.assertAlmostEqual(0, data.getClockBias())


if __name__ == '__main__':
	unittest.main()
