"""The GPS Toolkit, python extension package - test module."""

import unittest
import test_gpstk_store
import test_gpstk_time
import test_gpstk_timeconvert
import test_gpstk_util

def run_all():
    """Run all unit tests defined in gpstk.test module"""
    suite = unittest.TestLoader().loadTestsFromModule( test_gpstk_store )
    suite.addTest( unittest.TestLoader().loadTestsFromModule( test_gpstk_time ) )
    suite.addTest( unittest.TestLoader().loadTestsFromModule( test_gpstk_timeconvert ) )
    suite.addTest( unittest.TestLoader().loadTestsFromModule( test_gpstk_util ) )
    unittest.TextTestRunner(verbosity=2).run( suite )    

def run_store():
    """Run only the store tests defined in gpstk.test module"""
    suite = unittest.TestLoader().loadTestsFromModule( test_gpstk_store )
    unittest.TextTestRunner(verbosity=2).run( suite ) 

def run_time():
    """Run only the time tests defined in gpstk.test module"""
    suite = unittest.TestLoader().loadTestsFromModule( test_gpstk_time )
    unittest.TextTestRunner(verbosity=2).run( suite ) 

def run_timeconvert():
    """Run only the timeconvert tests defined in gpstk.test module"""
    suite = unittest.TestLoader().loadTestsFromModule( test_gpstk_timeconvert )
    unittest.TextTestRunner(verbosity=2).run( suite ) 

def run_util():
    """Run only the util tests defined in gpstk.test module"""
    suite = unittest.TestLoader().loadTestsFromModule( test_gpstk_util )
    unittest.TextTestRunner(verbosity=2).run( suite )

__all__ = ['run_all','run_store','run_time','run_timeconvert','run_util']
