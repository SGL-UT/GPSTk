#!/usr/env python

from test_utils import data_dir,output_dir
import unittest
import gpstk

class TestSystemTime(unittest.TestCase):
    def test_SystemTime(self):
        systime = gpstk.SystemTime()
        print systime

class TestCommonTime(unittest.TestCase):
    """CommonTime tests"""
    def test_CommonTime(self):
        """CommonTime constructor tests"""
        t=gpstk.CommonTime(1980, 1, 6, 0, 0, 0, gpstk.TimeSystem(gpstk.TimeSystem.GPS))


        systime = gpstk.SystemTime()
        comtime = systime.toCommonTime()

if __name__ == '__main__':
    unittest.main()
