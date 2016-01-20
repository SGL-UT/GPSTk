#!/usr/env python

import test_utils as tu
import gpstk

# Only print a handfull since there are so many. Add more as we identify
# more that are useful
tu.class_name="constants"
tu.assert_equal(gpstk.C_MPS, 299792458.0)
tu.assert_equal(gpstk.CA_CHIP_FREQ_GPS, 1023000)
tu.assert_equal(gpstk.L1_FREQ_GPS, 1575420000)
tu.assert_equal(gpstk.L2_FREQ_GPS, 1227600000)
tu.assert_equal(gpstk.PI, 3.14159265359, 5e-13)
tu.assert_equal(gpstk.ZCOUNT_PER_DAY, 57600)

# Make sure we throw an error when something _isn't_ there
try:
    print gpstk.foo_bar
    tu.fail_test()
except AttributeError as e:
    tu.pass_test()

tu.finish()
