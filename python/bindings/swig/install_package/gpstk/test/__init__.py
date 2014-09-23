"""The GPS Toolkit, python extension package - test module."""


from test_gpstk_time import *
from test_gpstk_util import *
from test_gpstk_store import *
from test_gpstk_timeconvert import *

def run_all():
    import unittest
    suite = unittest.TestLoader().loadTestsFromModule( test_gpstk_time )
    suite.addTest( unittest.TestLoader().loadTestsFromModule( test_gpstk_timeconvert ) )
    suite.addTest( unittest.TestLoader().loadTestsFromModule( test_gpstk_util ) )
    suite.addTest( unittest.TestLoader().loadTestsFromModule( test_gpstk_store ) )
    unittest.TextTestRunner(verbosity=2).run( suite )    
