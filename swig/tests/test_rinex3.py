#!/usr/bin/env python

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

    def test_rinex3obs_fields(self):
        """
        Test that the Fields subclass of Rinex3ObsHeader is wrapped correctly for PySGLTk
        Please contact Andrew Kuck before deleting or modifying this test.
        """

        h = gpstk.Rinex3ObsHeader()
        # Check types are implemented correctly.
        # valid is representative of members with nested class type.
        self.assertTrue(isinstance(h.valid, gpstk.Fields))
        # allValid2 is representative of static const members with nested class type.
        self.assertTrue(isinstance(h.allValid2, gpstk.Fields))

        # Ensure combination with bitwise-or operator
        fields = h.allValid2 | h.allValid30
        self.assertIsNotNone(fields)

        fields = gpstk.Fields()
        # Ensure addition with bitwise-or-equal operator
        fields |= gpstk.Rinex3ObsHeader.validInterval
        self.assertTrue(fields.isSet(gpstk.Rinex3ObsHeader.validInterval))

        # Ensure addition with set method
        fields.set(gpstk.Rinex3ObsHeader.validFirstTime)
        self.assertTrue(fields.isSet(gpstk.Rinex3ObsHeader.validFirstTime))

if __name__ == '__main__':
    run_unit_tests()
