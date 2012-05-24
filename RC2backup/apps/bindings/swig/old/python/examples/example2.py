#!/usr/bin/python

import sys
sys.path.append("..")
from gpstkPython import *

rin = RinexObsStream('../../../../examples/bahr1620.04o')
rout = RinexObsStream('bahr1620.04o.new', ios_out_trunc())
head = RinexObsHeader()

write(rin, head)
rout.header = rin.header
read(rout, rout.header)

data = RinexObsData()
while write(rin, data):
    read(rout, data)
