#!/usr/env python

from test_utils import data_dir,output_dir
import unittest
import gpstk

class TestConstants(unittest.TestCase):
    # Test a subset of the constants since there are so many.
    # Add more as we mature the testing.
    def testConstants(self):
        self.assertEqual(gpstk.C_MPS, 299792458)
        self.assertEqual(gpstk.CA_CHIP_FREQ_GPS, 1023000)
        self.assertEqual(gpstk.L1_FREQ_GPS, 1575420000)
        self.assertEqual(gpstk.L2_FREQ_GPS, 1227600000)
        self.assertAlmostEqual(gpstk.PI, 3.14159265359, places=12)
        self.assertEqual(gpstk.ZCOUNT_PER_DAY, 57600)
        with self.assertRaises(AttributeError):
            print gpstk.foo.bar

if __name__ == '__main__':
    unittest.main()
