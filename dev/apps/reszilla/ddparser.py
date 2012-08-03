#!/usr/bin/env python

import re
import datetime
from matplotlib import dates

# This parses the 'raw' output lines of ddGen
ddr_re = re.compile("(?P<time>\d{4} +\d{1,3} \d{2}:\d{2}:\d{2})(?P<fsec>\.\d) +"
                    "(?P<carrier>L\d) (?P<code>\S+) +(?P<otype>\S+) +"
                    "(?P<sv1>\d{1,2}) +(?P<sv2>\d{1,2}) +"
                    "(?P<elev1>\d+\.\d+) +(?P<elev2>\d+\.\d+) +"
                    "(?P<dd>-?\d+\.\d+) +(?P<hh>\d\d)")

cslip_re = re.compile(">c +"
                      "(?P<time>\d{4} +\d{1,3} \d{2}:\d{2}:\d{2})(?P<fsec>\.\d) +"
                      "(?P<sv1>\d{1,2}) +(?P<sv2>\d{1,2}) +"
                      "(?P<carrier>L\d) (?P<code>\S+) +(?P<otype>\S+) +"
                      "(?P<dd>\-?\d+\.\d+) +"
                      "(?P<elev1>\d+\.\d+) +(?P<elev2>\d+\.\d+) +"
                      "(?P<pre>\d+) +(?P<post>\d+) +(?P<gap>\d+\.\d)")

class DDRecord(object):
    debug = 0
    
    def __init__(self, rtype=None, time=None, carrier=None, code=None, otype=None,
                 dd=None, sv1=None, sv2=None, el1=None, el2=None, health=None):
        """Stores the data from a 'record' in a line of ddGen output.
        each record can be one of several types, indicated by the rtype:
        rtype='ddr'    double diference residual
        rtype='cs'     cycle slip
        rtype='stats'  statistics
        """
        self.type = rtype      # 'ddr', 'cs', 'stats'
        self.time = time       # gregorian date (i.e. one that can be given to matplotlib.date_plot()
        self.carrier = carrier # 'L1', 'L2'...
        self.code = code       # 'GPSC/A', 'GPSY', 'GPSP', ...
        self.otype = otype     # 'pseudorange', 'phase', 'doppler', ...
        self.dd = dd           # the residual, or size of cycle slip, or variance, or bias on Arc
        self.sv1 = sv1         # reference SV
        self.sv2 = sv2         # target SV
        self.el1 = el1         # elevation of the reference SV, or start of statistic bin, degrees
        self.el2 = el2         # elevation of the target SV, or end of statistic bin, degrees
        self.health = health   # health of sv2

    def __str__(self):
        return "{} {} {}".format(self.type, self.time, self.dd)

    @classmethod
    def reader(cls, file):
        """Generates DDRecords from a ddGen output file"""
        sec_per_day = 86400.0
        for line in file:
            line = line.strip()
            if cls.debug>1: print line
            if len(line)==0: continue
            if line[0] =='#' and line[0:6] != "# Arc:": continue

            for rtype,cre in [['ddr',ddr_re], ['cs',cslip_re], ['arc',arc_re]]:
                c = cre.match(line)
                if c is not None: break
            if c is None: continue
            if cls.debug: print rtype,c.groupdict()

            rec = DDRecord()
            rec.type = rtype
            rec.time = dates.date2num(
                datetime.datetime.strptime(
                    c.group('time'),  "%Y %j %H:%M:%S"))
            + float(c.group('fsec'))/sec_per_day
            rec.carrier = c.group('carrier')
            rec.code = c.group('code')
            rec.otype = c.group('otype')
            rec.dd = float(c.group('dd'))
            rec.sv1 = int(c.group('sv1'))
            rec.sv2 = int(c.group('sv2'))
            if rtype == 'ddr' or rtype == 'cs':
                rec.el1 = float(c.group('elev1'))
                rec.el2 = float(c.group('elev2'))
            if rtype == "ddr":
                rec.health = c.group('hh')

            yield rec
#

if __name__ == "__main__":
    import sys
    import argparse
    parser = argparse.ArgumentParser(description="Plots the output of ddGen.")

    parser.add_argument("input_file", metavar='fn', nargs='?', type=argparse.FileType('r'), default=sys.stdin,
                        help="Input data file, defaults to stdin.",)
    parser.add_argument("-d", "--debug", default=0, dest="debug", action="count",
                      help="Increase the debug.")

    args = parser.parse_args()
    DDRecord.debug = args.debug
    for r in DDRecord.reader(args.input_file):
        print r

