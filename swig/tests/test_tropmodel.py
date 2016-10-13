#!/usr/bin/env python

import unittest, sys, os
sys.path.insert(0, os.path.abspath(".."))
from gpstk.test_utils import args,run_unit_tests
import gpstk

class TestTropModelsExist(unittest.TestCase):
    def test_TropModelsExist(self):
        tropMissing = 'Trop Model is missing'
        
        self.assertTrue(hasattr(gpstk, 'GCATTropModel'), tropMissing)
        self.assertTrue(hasattr(gpstk, 'GGHeightTropModel'), tropMissing)
        self.assertTrue(hasattr(gpstk, 'GGTropModel'), tropMissing)
        self.assertTrue(hasattr(gpstk, 'MOPSTropModel'), tropMissing)
        self.assertTrue(hasattr(gpstk, 'NBTropModel'), tropMissing)
        self.assertTrue(hasattr(gpstk, 'NeillTropModel'), tropMissing)
        self.assertTrue(hasattr(gpstk, 'SaasTropModel'), tropMissing)
        self.assertTrue(hasattr(gpstk, 'SimpleTropModel'), tropMissing)

if __name__ == '__main__':
    run_unit_tests()
