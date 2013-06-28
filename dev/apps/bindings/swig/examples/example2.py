# this is a translation of dev/src/examples/example2.cpp

from gpstk import *


# Create the input file stream
rin = Rinex3ObsStream('bahr1620.04o')

# Create the output file stream

print(dir(rin))

#       // Read the RINEX header
# Rinex3ObsHeader head;    #RINEX header object
#    rin >> head;
#    rout.header = rin.header;
#    rout << rout.header;

#       // Loop over all data epochs
#    Rinex3ObsData data;   //RINEX data object
#    while (rin >> data)
#    {
#      rout << data;
#    }

#    exit(0);

# }
