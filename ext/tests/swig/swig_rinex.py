#!/usr/env python

from test_utils import data_dir,output_dir
import unittest
import gpstk

class TestRinex(unittest.TestCase):
    def test_readRinex3Obs(self):
        header, data = gpstk.readRinex3Obs( data_dir+"/arlm200a.15o", strict=True)

        # Find the earliest and latest observations
        # function for how to compare Rinex3ObsData objects for min/max functions:
        timeFunction = lambda self: self.time
        earliest = min(data, key=timeFunction)
        latest = max(data, key=timeFunction)

        self.assertEqual(
            gpstk.CivilTime(2015, 7, 19, 0, 0, 0, gpstk.TimeSystem(gpstk.TimeSystem.GPS)),
            gpstk.CivilTime(earliest.time))

        self.assertEqual(
            gpstk.CivilTime(2015, 7, 19, 0, 59, 30, gpstk.TimeSystem(gpstk.TimeSystem.GPS)),
            gpstk.CivilTime(latest.time))

    def test_writeRinex3Obs(self):
        header, data = gpstk.readRinex3Obs( data_dir+"/arlm200a.15o", strict=True)
        # Now let's write it all back to a different file
        gpstk.writeRinex3Obs( output_dir+'/writeRinex3Obs-arlm200a.15o', header, data)
        # should really difference the files here

if __name__ == '__main__':
    unittest.main()
