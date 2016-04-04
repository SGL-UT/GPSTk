#!/usr/env python

import unittest, sys, os
sys.path.insert(0, os.path.abspath(".."))
from gpstk.test_utils import args,run_unit_tests
import gpstk


class TestRinex3(unittest.TestCase):
    """Tests for the gpstk::Rinex3Obs file i/o"""

    def test_readRinex3Obs(self):
        """Test reading entire rinex obs file and spot check the data"""
        header, data = gpstk.readRinex3Obs( args.input_dir+"/arlm200a.15o", strict=True)

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
        """Test reading and writing back out a rinex obs file"""

        header, data = gpstk.readRinex3Obs( args.input_dir+"/arlm200a.15o", strict=True)
        # Now let's write it all back to a different file
        gpstk.writeRinex3Obs( args.output_dir+'/swig-arlm200a.15o', header, data)
        # should really difference the files here

if __name__ == '__main__':
    run_unit_tests()
