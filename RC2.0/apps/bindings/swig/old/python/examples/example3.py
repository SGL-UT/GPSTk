#!/usr/bin/python

import sys
sys.path.append("..")
from gpstkPython import *

#if sys.argc < 2:
#    print "Required argument is a RINEX obs file."

#print "Name your PRN of interest (by number: 1 through 32: "
myprn = 5

gamma = (L1_FREQ / L2_FREQ) * (L1_FREQ / L2_FREQ);

try:
    print "Reading bahr1620.04o."

    # Declare RINEX observation file streams and data objects
    roffs = RinexObsStream("../../../../examples/bahr1620.04o")
    roh = RinexObsHeader()
    roe = RinexObsData()
    dataobj = RinexDatum()

    # Read the RinexHeader (this could be skipped).
    write(roffs, roh)

    # Print RINEX header to terminal screen
    roh.dump(cout)

    # Loop through epochs and process data for each.
    while write(roffs, roe):
        junk = DayTime_streamRead(cout, roe.time)

        # Make a GPSTk RinexPrn object for your PRN so you can search for it.
        prn = RinexPrn(myprn, systemGPS)

        # Check to see if your PRN is in view at this epoch (search for it)
        pointer = RinexPrnMap_find(roe, prn)
        
        if RinexPrnMap_compare(pointer, RinexPrnMap_end(roe)):
            print " PRN ", str(myprn), " not in view."
        else:
            dataobj = getPseudoRangeCode(roe, prn, RinexObsHeader.P1)
            dataobj2 = getPseudoRangeCode(pointer, RinexObsHeader.P1)
            if dataobj.data != dataobj2.data:
                print "dataobj.data != dataobj2.data"

            P1 = dataobj.data

            dataobj = getPseudoRangeCode(roe, prn, RinexObsHeader.P2)
            P2 = dataobj.data

            dataobj = getPseudoRangeCode(roe, prn, RinexObsHeader.L1)
            L1 = dataobj.data

            # Compute multipath
            mu = P1 -L1*(C_GPS_M/L1_FREQ) -2*(P1 -P2)/(1-gamma)
            print " PRN", myprn, "biased multipath", mu

    print "Read", roffs.recordNumber, "epochs.  Cheers."

except Exception:
    print "Exception"
