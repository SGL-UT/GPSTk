import gpstk
import sys
import unittest


class EllipsoidTests(unittest.TestCase):
    def test_WGS84(self):
        e = gpstk.WGS84Ellipsoid()
        self.assertAlmostEqual(299792458.0, e.c())
        self.assertTrue(issubclass(gpstk.WGS84Ellipsoid, gpstk.EllipsoidModel))

    def test_PZ90(self):
        p = gpstk.PZ90Ellipsoid()
        self.assertAlmostEqual(6378136.0, p.a())
        self.assertTrue(issubclass(gpstk.WGS84Ellipsoid, gpstk.EllipsoidModel))


class URATest(unittest.TestCase):
    def test(self):
        self.assertEqual(3, gpstk.accuracy2ura(5.352))
        self.assertEqual(15, gpstk.constants.SV_ACCURACY_GPS_MAX_INDEX_VALUE)


class BrcKeplerOrbitTest(unittest.TestCase):
    def test(self):
        t1 = gpstk.CommonTime()
        t2 = gpstk.CommonTime()
        t2.addDays(1)
        t3 = gpstk.CommonTime()
        t3.addSeconds(60)
        obs = gpstk.ObsID(gpstk.ObsID.otRange, gpstk.ObsID.cbC6, gpstk.ObsID.tcN)
        b = gpstk.BrcKeplerOrbit('GPS', obs, 10, t1, t2, t3,
                           5, True, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                           0.0, 1.1, 0.0, 0.0, 0.0, 0.0, 0.0, 1.2, 1.3, 0.0)
        self.assertEqual(1.1, b.getEcc())
        self.assertEqual(1.2, b.getW())
        self.assertEqual(1.3, b.getOmegaDot())


class AlmOrbitTest(unittest.TestCase):
    def test(self):
        a = gpstk.AlmOrbit(1, 1.0, 2.0, 1.5, 3.0, 0, 0, 50.5, 0, 0, 0, 3000000L, 1, 2)
        self.assertEqual(0.0, a.getAF0())
        self.assertEqual(0L, a.getFullWeek())
        xvt = a.svXvt(gpstk.CommonTime())
        self.assertAlmostEqual(-17.959049543400, xvt.x[0])
        self.assertAlmostEqual(9442.96597037813, xvt.v[1])


class GPSAlmanacStoreTest(unittest.TestCase):
    def test(self):
        e = gpstk.EngAlmanac()
        suframe = [023222245L, 14111111324L, 4623626L, 33333531536L,
                   4126166634L, 17845L, 6317L, 736162361L, 83163L, 91471L]
        e.addSubframe(suframe, 1)
        g = gpstk.GPSAlmanacStore()
        g.addAlmanac(e)


class GloEphemerisTest(unittest.TestCase):
    def test_ephemeris(self):
        g = gpstk.GloEphemeris()
        g.setRecord('mySys', 1, gpstk.CommonTime(), gpstk.Triple(100, 200, 300),
            gpstk.Triple(10, 20, 30), gpstk.Triple(1, 2, 3),
            0.0,  0.0, 1, 2, 3, 1.1, 1.0)
        expected = ("Sys:mySys, PRN:1\nEpoch:0000000 00000000 0.000000000000000 "
            "Unknown, pos:(100, 200, 300)\nvel:(10, 20, 30), acc:(1, 2, 3)\n"
            "TauN:0, GammaN:0\nMFTime:1, health:2\nfreqNum:3, ageOfInfo:1.1")
        self.assertEqual(expected, str(g))


class GPSEphemerisStoreTest(unittest.TestCase):
    def test(self):
        e = gpstk.EngEphemeris()
        g = gpstk.GPSEphemerisStore()
        # help(g)
        # g.addEphemeris(e)


class SP3Test(unittest.TestCase):
    def test_fileIO(self):
        pass

    def test_almanac_store(self):
      s = gpstk.SP3EphemerisStore()
      sat = gpstk.SatID(1, gpstk.SatID.systemGPS)
      time = gpstk.CommonTime()
      time.addDays(10000)
      s.addPositionData(sat, time, gpstk.Triple(50, -45, 20), gpstk.Triple(1, 100, 5))
      s.addVelocityData(sat, time, gpstk.Triple(1, 2, -10000), gpstk.Triple(1, 100, 5))
      self.assertEqual(gpstk.Triple(50000, -45000, 20000), s.getPosition(sat, time))


class SEMTest(unittest.TestCase):
    def test_fileIO(self):
        header, data = gpstk.readSEM('sem_data.txt', lazy=False)
        self.assertEqual(724, header.week)
        self.assertEqual(405504L, header.Toa)
        self.assertEqual(32, len(data))
        dataPoint = data[15]
        self.assertEqual(16, dataPoint.PRN)
        self.assertAlmostEqual(0.00711489, dataPoint.ecc)

    def test_fileIO_lazy(self):
        header, gen = gpstk.readSEM('sem_data.txt', lazy=True)
        data = [x for x in gen]
        self.assertEqual(32, len(data))

    def test_almanac_store(self):
        s = gpstk.SEMAlmanacStore()
        s.loadFile('sem_data.txt')
        expected = ('x:(1.33149e+06, 1.54299e+07, -2.15873e+07), '
                    'v:(-2620.86, 793.126, 415.074), '
                    'clk bias:4.75373e-05, clk drift:0, relcorr:0')
        actual = str(s.getXvt(gpstk.SatID(1), s.getInitialTime()))
        self.assertEqual(expected, actual)


class YumaTest(unittest.TestCase):
    def test_fileIO(self):
        header, data = gpstk.readYuma('yuma_data.txt', lazy=False)
        self.assertEqual(31, len(data))
        dataPoint = data[10]
        self.assertAlmostEqual(0.0, dataPoint.AF1)
        self.assertEqual(11L, dataPoint.PRN)
        self.assertAlmostEqual(0.006191730499, dataPoint.ecc)
        self.assertEqual(377L, dataPoint.week)

    def test_fileIO_lazy(self):
        header, gen = gpstk.readYuma('yuma_data.txt', lazy=True)
        data = [x for x in gen]
        self.assertEqual(31, len(data))


class Rinex3ObsTest(unittest.TestCase):
    def test_fileIO(self):
        header, data = gpstk.readRinex3Obs('rinex3obs_data.txt', lazy=False)
        self.assertEqual(0L, header.numSVs)
        self.assertEqual('NATIONAL IMAGERY AND MAPPING AGENCY', header.agency)
        self.assertEqual(120, len(data))
        dataPoint = data[0]
        datum = dataPoint.getObs(gpstk.SatID(4), header.getObsIndex("C1"))
        self.assertAlmostEqual(24236698.057, datum.data)
        self.assertEqual(0, dataPoint.clockOffset)
        expectedTime = gpstk.CommonTime()
        expectedTime.set(2453167)
        expectedTime.setTimeSystem(gpstk.TimeSystem(gpstk.TimeSystem.GPS))
        self.assertEqual(expectedTime, dataPoint.time)

    def test_fileIO_lazy(self):
        header, gen = gpstk.readRinex3Obs('rinex3obs_data.txt', lazy=True)
        data = [x for x in gen]
        self.assertEqual(120, len(data))


class Rinex3NavTest(unittest.TestCase):
    def test_fileIO(self):
        header, data = gpstk.readRinex3Nav('rinex3nav_data.txt')
        self.assertEqual('06/10/2004 00:00:26', header.date)
        self.assertEqual(166, len(data))
        dataPoint = data[165]
        self.assertEqual(5153.72985268, dataPoint.Ahalf)
        self.assertEqual(432000.0, dataPoint.Toc)


class RinexMetTest(unittest.TestCase):
    def test_fileIO(self):
        header, data = gpstk.readRinexMet('rinexmet_data.txt')
        self.assertEqual('06/09/2004 23:58:58', header.date)
        self.assertEqual(96, len(data))
        dataPoint = data[95]
        d = dataPoint.getData()
        self.assertAlmostEqual(998.3, d[gpstk.RinexMetHeader.PR])
        self.assertAlmostEqual(30.8, d[gpstk.RinexMetHeader.TD])


if __name__ == '__main__':
    unittest.main()
