#!/usr/bin/env python

import gpstk
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
        obs = gpstk.ObsID(gpstk.ObsID.otRange, gpstk.ObsID.cbAny, gpstk.ObsID.tcA)
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
        xvt = a.svXvt(a.getTimestamp())
        self.assertAlmostEqual(-5.285762998073685, xvt.x[0], places=3)
        self.assertAlmostEqual(-7.928259054425361, xvt.v[1], places=3)


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
        expected = ("Sys:mySys, PRN:1, Epoch:0000000 00000000 0.000000000000000 UNK, pos:(100, 200, 300), "
            "vel:(10, 20, 30), acc:(1, 2, 3), TauN:0, GammaN:0, MFTime:1, health:2, freqNum:3, ageOfInfo:1.1")
        self.assertEqual(expected, str(g))


class SP3Test(unittest.TestCase):
    def test_ephem_store(self):
        ephem = gpstk.SP3EphemerisStore()
        ephem.rejectBadPositions(True);
        ephem.rejectBadClocks(True);
        ephem.loadFile("sp3_data.txt");
        t = ephem.getInitialTime()
        t.addSeconds(60*120) # 2 hours into data
        s = gpstk.SatID(1)
        p = ephem.getPosition(s, t)
        self.assertAlmostEqual(17359099.884999998, p[0])
        self.assertAlmostEqual(-483028.55, p[1])
        self.assertAlmostEqual(-19921938.297000002, p[2])

    def test_stream(self):
        header, data = gpstk.readSP3('sp3_data.txt', strict=True)
        self.assertEqual(' IGS', header.agency)
        self.assertEqual(96, header.numberOfEpochs)
        dataPoint = data[15]
        self.assertEqual('P', dataPoint.RecType)
        # Checking length of typemap conversions for the arrays->lists
        self.assertEqual(3, len(dataPoint.x))
        self.assertEqual(4, len(dataPoint.sig))
        self.assertEqual(6, len(dataPoint.correlation))
        self.assertEqual(-20622.832361, dataPoint.x[0])
        self.assertEqual(0, dataPoint.sdev[0])


class SEMTest(unittest.TestCase):
    def test_stream(self):
        header, gen = gpstk.readSEM('sem_data.txt')
        data = list(gen)
        self.assertEqual(31, len(data))

    def test_stream_strict(self):
        header, data = gpstk.readSEM('sem_data.txt', strict=True)
        self.assertEqual(724, header.week)
        self.assertEqual(405504L, header.Toa)
        self.assertEqual(31, len(data))
        dataPoint = data[15]
        self.assertEqual(16, dataPoint.PRN)
        self.assertAlmostEqual(0.00711489, dataPoint.ecc)

    def test_almanac_store(self):
        s = gpstk.SEMAlmanacStore()
        s.loadFile('sem_data.txt')
        expected = ('x:(1.33149e+06, 1.54299e+07, -2.15873e+07), '
                    'v:(-2620.86, 793.126, 415.074), '
                    'clk bias:4.75373e-05, clk drift:0, relcorr:0')
        actual = str(s.getXvt(gpstk.SatID(1), s.getInitialTime()))
        self.assertEqual(expected, actual)


class YumaTest(unittest.TestCase):
    def test_stream(self):
        header, gen = gpstk.readYuma('yuma_data.txt')
        data = list(gen)
        self.assertEqual(30, len(data))

    def test_stream_lazy(self):
        header, data = gpstk.readYuma('yuma_data.txt', strict=True)
        self.assertEqual(30, len(data))
        dataPoint = data[10]
        self.assertAlmostEqual(0.0, dataPoint.AF1)
        self.assertEqual(11L, dataPoint.PRN)
        self.assertAlmostEqual(0.006191730499, dataPoint.ecc)
        self.assertEqual(377L, dataPoint.week)


class Rinex3ObsTest(unittest.TestCase):
    def test_stream(self):
        header, data = gpstk.readRinex3Obs('rinex3obs_data.txt', strict=True)
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

    def test_stream_lazy(self):
        header, gen = gpstk.readRinex3Obs('rinex3obs_data.txt', strict=False)
        data = list(gen)
        self.assertEqual(120, len(data))


class Rinex3NavTest(unittest.TestCase):
    def test_stream(self):
        header, data = gpstk.readRinex3Nav('rinex3nav_data.txt', strict=True)
        self.assertEqual('06/10/2004 00:00:26', header.date)
        self.assertEqual(166, len(data))
        dataPoint = data[165]
        self.assertAlmostEqual(5153.72985268, dataPoint.Ahalf)
        self.assertEqual(432000.0, dataPoint.Toc)

        # checks converstion to Engineering ephemeris and getting a xvt from it
        eng = dataPoint.toEngEphemeris()
        xvt = eng.svXvt(eng.getTransmitTime())
        self.assertAlmostEqual(4793694.728073199, xvt.x[0])
        self.assertAlmostEqual(2028.248716131878, xvt.v[1])


class RinexMetTest(unittest.TestCase):
    def test_stream(self):
        header, data = gpstk.readRinexMet('rinexmet_data.txt', strict=True)
        self.assertEqual('06/09/2004 23:58:58', header.date)
        self.assertEqual(96, len(data))
        dataPoint = data[95]
        d = dataPoint.getData()
        self.assertAlmostEqual(998.3, d[gpstk.RinexMetHeader.PR])
        self.assertAlmostEqual(30.8, d[gpstk.RinexMetHeader.TD])


class FICTest(unittest.TestCase):
    def test_almanac(self):
        isblock62 = (lambda x: x.blockNum == 62)
        header, data = gpstk.readFIC('fic_data.txt', filterfunction=isblock62, strict=True)
        self.assertEqual(96, len(data))
        g = gpstk.GPSAlmanacStore()
        for d in data:
            orbit = d.toAlmOrbit()
            g.addAlmanac(orbit)

        sat = gpstk.SatID(4, gpstk.SatID.systemGPS)
        sys = gpstk.TimeSystem(gpstk.TimeSystem.GPS)
        t = gpstk.CivilTime(2012, 11, 10, 2, 0, 0, sys)
        t = t.toCommonTime()
        xvt= g.getXvt(sat, t)
        self.assertAlmostEqual(6888490.4337890595, xvt.x[0])
        self.assertAlmostEqual(1036.1894772036476, xvt.v[1])

    def test_ephemeris(self):
        isblock9 = (lambda x: x.blockNum == 9)
        header, data = gpstk.readFIC('fic_data.txt', filterfunction=isblock9, strict=True)
        g = gpstk.GPSEphemerisStore()
        for d in data:
            ephem = gpstk.Rinex3NavData(d.toEngEphemeris())
            g.addEphemeris(ephem)

        sat = gpstk.SatID(4, gpstk.SatID.systemGPS)
        sys = gpstk.TimeSystem(gpstk.TimeSystem.GPS)
        t = gpstk.CivilTime(2012, 11, 10, 2, 0, 0, sys)
        t = t.toCommonTime()
        xvt= g.getXvt(sat, t)
        self.assertAlmostEqual(6887268.768807452, xvt.x[0])
        self.assertAlmostEqual(1036.3167828001287, xvt.v[1])


class MSCTest(unittest.TestCase):
    def test_stream(self):
        header, data = gpstk.readMSC('msc_data.txt')
        dataPoint = data.next()
        xvt = dataPoint.getXvt(dataPoint.time)
        self.assertAlmostEqual(-1111111.3059100616, xvt.x[0])
        self.assertAlmostEqual(0.0222, xvt.v[1])
        self.assertEqual('AAA    ', dataPoint.mnemonic)
        self.assertEqual(11111, dataPoint.station)

    def test_store(self):
        store = gpstk.MSCStore()
        store.loadFile('msc_data.txt')
        xvt = store.getXvt('11111', store.getInitialTime())
        self.assertAlmostEqual(-1111111.295, xvt.x[0])
        self.assertAlmostEqual(0.0222, xvt.v[1])


class GLONASS_test(unittest.TestCase):
    def test(self):
        pass
        # header, data = gpstk.readRinex3Nav('/home/jking/GLONASS/BEphemeris/thti2200.12n')
        # dataPoint = data.next()

        # ephem = dataPoint.toEngEphemeris()
        # time = ephem.getTransmitTime()
        # xvt = ephem.svXvt(time)
        # print time
        # print xvt

        # ephem = dataPoint.toGloEphemeris()
        # time = ephem.getEphemerisEpoch()
        # time.setTimeSystem(gpstk.TimeSystem('GLO'))
        # xvt = ephem.svXvt(time)
        # print time
        # print xvt



        # from glob import glob
        # files = glob('/home/jking/GLONASS/BEphemeris/*.12n')[:1]
        # g = gpstk.GloEphemerisStore()
        # for f in files:
        #     header, data = gpstk.readRinex3Nav(f)
        #     for d in data:
        #         g.addEphemeris(d)

        # print g


    # def test_dataobjects(self):
        # header, data = gpstk.readRinex3Nav('rinex3nav_data.txt')
        # g =
        # gloEphems = [d.toGloEphemeris() for d in data]
        # self.assertEqual(1, gloEphems[0].getPRNID())
        # print gloEphems[0]

        # engEphems = [d.toEngEphemeris() for d in data]
        # print engEphems[0]


    # def test_store(self):
    #     g = gpstk.GloEphemerisStore()
    #     header, data = gpstk.readRinex3Nav('/home/jking/GLONASS/BEphemeris/lhaz2950.12n')
    #     # header, data = gpstk.readRinex3Nav('rinex3nav_data.txt')
    #     for d in data:
    #         # print d
    #         g.addEphemeris(d)

        # print g
        # s = gpstk.SatID(7)
        # t = g.getInitialTime()
        # for i in range(10000):
        #     t.addSeconds(60*5)
        #     try:
        #         x= g.getXvt(s, t)
        #         print x
        #     except gpstk.exceptions.InvalidRequest:
        #         pass


if __name__ == '__main__':
    unittest.main()
