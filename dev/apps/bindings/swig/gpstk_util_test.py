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
	def setUp(self):
		self.normal = SatID(1, SatelliteSystems['systemGPS'])
		self.invalid = SatID() # calls default (invalid) constructor
	def test_SatelliteSystemEnum(self):
		self.assertEqual(SatelliteSystems['systemGPS'],         SatID.systemGPS)
		self.assertEqual(SatelliteSystems['systemGalileo'],     SatID.systemGalileo)
		self.assertEqual(SatelliteSystems['systemGlonass'],     SatID.systemGlonass)
		self.assertEqual(SatelliteSystems['systemGeosync' ],    SatID.systemGeosync)
		self.assertEqual(SatelliteSystems['systemLEO'],         SatID.systemLEO)
		self.assertEqual(SatelliteSystems['systemTransit'],     SatID.systemTransit)
		self.assertEqual(SatelliteSystems['systemCompass'], 	SatID.systemCompass)
		self.assertEqual(SatelliteSystems['systemMixed'],       SatID.systemMixed)
		self.assertEqual(SatelliteSystems['systemUserDefined'], SatID.systemUserDefined)
		self.assertEqual(SatelliteSystems['systemUnknown'],     SatID.systemUnknown)
	def test_normal(self):
		self.assertTrue(self.normal.isValid())
	def test_invalid(self):
		self.assertFalse(self.invalid.isValid())

class GNSSconstants_test(unittest.TestCase):
	def test_constants(self):
		self.assertEqual(PI, 3.1415926535898)
		self.assertEqual(OSC_FREQ_GPS, 10.23e6)
		self.assertEqual(L6_FREQ_GAL, 1278.75e6)
	def test_functions(self):
		self.assertEqual(4, getLegacyFitInterval(15, 27)) 
		self.assertEqual(0.190293672798, getWavelength(SatID(1, 1), 1))

class Triple_test(unittest.TestCase):
	def test(self):
		t = Triple(1.5, 2., -3.)
		self.assertEqual(2.25+4.+9., t.dot(t))
		self.assertEqual(1.5, t[0])
		self.assertEqual(Triple(6.,8.,9.), Triple(1.,2.,4.)+Triple(5.,6.,5.))
		self.assertEqual(Triple(4,6,8), Triple(2,3,4).scale(2))
		self.assertEqual(Triple(1.5, 2.5, 3.5), makeTriple((1.5, 2.5, 3.5)))
		self.assertEqual((1.5, 2.5, 3.5), makeTuple(makeTriple((1.5, 2.5, 3.5))))

class SpecialFunctions_test(unittest.TestCase):
	def test(self):
		self.assertAlmostEqual(24.0, gamma(5))
		self.assertAlmostEqual(2.3632718012073, gamma(-1.5))
		self.assertAlmostEqual(0.8427007929497, erf(1))
		self.assertAlmostEqual(0.9661051464753, erf(1.5))

class Position_test(unittest.TestCase):
	def test(self):
		p1 = Position(1.5, 6.2, 3.5)
		p2 = Position(1.0, 1.8, .5)
		self.assertEqual(5.348831648126533, range(p1, p2))
		p3 = Position(1.1, 1.2, 1.3, CoordinateSystems['Unknown']) # making sure coordinatesystems dict is built

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
		self.assertEqual(self.o1.__str__() , 'C6 GPSsquare pseudorange')

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


if __name__ == '__main__':
	unittest.main()
