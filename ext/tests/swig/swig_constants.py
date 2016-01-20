print "Verifying that the constants in GPSTk made it to python"

# Looks like the way ctest needs to run its scrips needs to have '' in the path
import sys
sys.path=['']+sys.path

import gpstk

# Only print a handfull since there are so many. Add more as we identify
# more that are useful
print gpstk.C_MPS
print gpstk.CA_CHIP_FREQ_GPS
print gpstk.L1_FREQ_GPS
print gpstk.L2_FREQ_GPS
print gpstk.PI
print gpstk.ZCOUNT_PER_DAY

# Make sure we throw an error when something _isn't_ there
try:
    print gpstk.foo_bar
    print "Error. This statement should not be reached"
    sys.exit(-1)
except AttributeError as e:
    pass
