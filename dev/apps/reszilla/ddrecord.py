#!/usr/bin/env python

import re
import datetime
from matplotlib import dates

class DDRecord(object):
    sec_per_day = 86400.0
    debug = 0

    
    def __init__(self,carrier=None, code=None, otype=None):
        """Stores the data from a 'record' in a line of ddGen output."""

        self.carrier = carrier # 'L1', 'L2'...
        self.code = code       # 'GPSC/A', 'GPSY', 'GPSP', ...
        self.otype = otype     # 'pseudorange', 'phase', 'doppler', ...

    def __str__(self):
        return "{} {} {}".format(self.carrier, self.code, self.otype)

    @classmethod
    def reader(cls, file):
        """Generates DDRecords from a ddGen output file"""
        for line in file:
            line = line.strip()
            if DDRecord.debug>1: print line
            if len(line)==0: continue
            if line[0] =='#' and line[0:6] != "# Arc:": continue

            for rec in [DDres(), DDcs(), DDarc(), DDstat()]:
                if rec.parse(line):
                    yield rec

class DDstat(DDRecord):
    """Stores the values from a statistics line output of ddGen"""
    sre = re.compile(">s +"
                     "(?P<carrier>\S+) +"
                     "(?P<code>\S+) +"
                     "(?P<otype>\S+) +"
                     "(?P<el1>\d{1,2})-"
                     "(?P<el2>\d{1,2}) +"
                     "(?P<mad>[0-9.]+) +"
                     "(?P<median>[0-9-+.e]+) +"
                     "(?P<dde>\d+) +"
                     "(?P<sve>\d+) +"
                     "(?P<kurt>[0-9.]+) +"
                     "(?P<jumps>\d+)")

    def __init__(self, time=None, carrier=None, code=None, otype=None, el1=None, el2=None,
                 mad=None, median=None, dde=None, sve=None, kurt=None, jumps=None):
        self.carrier = carrier # L1, L2, L5, etc
        self.code = code       # GPSC/A, GPSY, ...
        self.otype = otype     # obs type (pseudorange, phase, doppler)
        self.el1 = el1         # elevation of the reference SV, or start of statistic bin, degrees
        self.el2 = el2         # elevation of the target SV, or end of statistic bin, degrees
        self.mad = mad
        self.median = median
        self.dde = dde
        self.sve = sve
        self.kurt = kurt
        self.jumps = jumps

    def __str__(self):
        s = DDRecord.__str__(self)
        return "stats: {} {:2d}-{:2d} {} {}".format(
            s, self.el1, self.el2, self.mad, self.dde)

    def parse(self, line):
        c = DDstat.sre.match(line)
        if c is None:
            return False
        self.carrier = c.group('carrier')
        self.code = c.group('code')
        self.otype = c.group('otype')
        self.el1 = int(c.group('el1'))
        self.el2 = int(c.group('el2'))
        self.mad = float(c.group('mad'))
        self.median = float(c.group('median'))
        self.dde = int(c.group('dde'))
        self.sve = int(c.group('sve'))
        self.kurt = float(c.group('kurt'))
        self.jumps = int(c.group('jumps'))
        return True
#

class DDres(DDRecord):
    """Stores the values from a raw residual line output of ddGen"""

    # This parses the 'raw' output lines of ddGen
    cre = re.compile("(?P<time>\d{4} +\d{1,3} \d{2}:\d{2}:\d{2})(?P<fsec>\.\d) +"
                        "(?P<carrier>L\d) (?P<code>\S+) +(?P<otype>\S+) +"
                        "(?P<sv1>\d{1,2}) +(?P<sv2>\d{1,2}) +"
                        "(?P<elev1>\d+\.\d+) +(?P<elev2>\d+\.\d+) +"
                        "(?P<dd>-?\d+\.\d+) +(?P<hh>\d\d)")
    
    def __init__(self, time=None, dd=None, sv1=None, sv2=None, el1=None, el2=None, health=None):
        self.time = time       # gregorian date (i.e. one that can be given to matplotlib.date_plot()
        self.dd = dd           # the residual, or size of cycle slip, or variance, or bias on Arc
        self.sv1 = sv1         # reference SV
        self.sv2 = sv2         # target SV
        self.el1 = el1         # elevation of the reference SV, or start of statistic bin, degrees
        self.el2 = el2         # elevation of the target SV, or end of statistic bin, degrees
        self.health = health   # health of sv2

    def __str__(self):
        s = DDRecord.__str__(self)
        return "ddr:{} {} {}".format(s, self.time, self.dd)

    def parse(self, line):
        c = DDres.cre.match(line)
        if c is None:
            return False
        self.time = dates.date2num(
            datetime.datetime.strptime(
                c.group('time'),  "%Y %j %H:%M:%S"))
        + float(c.group('fsec'))/DDRecord.sec_per_day
        self.carrier = c.group('carrier')
        self.code = c.group('code')
        self.otype = c.group('otype')
        self.sv1 = int(c.group('sv1'))
        self.sv2 = int(c.group('sv2'))
        self.el1 = float(c.group('elev1'))
        self.el2 = float(c.group('elev2'))
        self.health = c.group('hh')
        self.dd = float(c.group('dd'))
        return True
#

class DDcs(DDRecord):
    """Stores the values from a cycle slip line output of ddGen"""

    cre = re.compile(">c +"
                     "(?P<time>\d{4} +\d{1,3} \d{2}:\d{2}:\d{2})(?P<fsec>\.\d) +"
                     "(?P<sv1>\d{1,2}) +(?P<sv2>\d{1,2}) +"
                     "(?P<carrier>L\d) (?P<code>\S+) +(?P<otype>\S+) +"
                     "(?P<cs>\-?\d+\.\d+) +"
                     "(?P<elev1>\d+\.\d+) +(?P<elev2>\d+\.\d+) +"
                     "(?P<pre>\d+) +(?P<post>\d+) +(?P<gap>\d+\.\d)")

    def __init__(self, time=None, cs=None, sv1=None, sv2=None, el1=None, el2=None, health=None):
        self.time = time       # gregorian date (i.e. one that can be given to matplotlib.date_plot()
        self.cs = cs           # size of cycle slip in cycles
        self.sv1 = sv1         # reference SV
        self.sv2 = sv2         # target SV
        self.el1 = el1         # elevation of the reference SV, or start of statistic bin, degrees
        self.el2 = el2         # elevation of the target SV, or end of statistic bin, degrees

    def __str__(self):
        s = DDRecord.__str__(self)
        return "cs:{} {} {}".format(s, self.time, self.cs)

    def parse(self, line):
        c = DDcs.cre.match(line)
        if c is None:
            return False
        self.time = dates.date2num(
            datetime.datetime.strptime(
                c.group('time'),  "%Y %j %H:%M:%S"))
        + float(c.group('fsec'))/DDRecord.sec_per_day
        self.carrier = c.group('carrier')
        self.code = c.group('code')
        self.otype = c.group('otype')
        self.sv1 = int(c.group('sv1'))
        self.sv2 = int(c.group('sv2'))
        self.el1 = float(c.group('elev1'))
        self.el2 = float(c.group('elev2'))
        self.cs = float(c.group('cs'))
        return True
#

class DDarc(DDRecord):
    """Stores the values from a cycle slip line output of ddGen"""

    cre = re.compile("# Arc: +"
                        "(?P<t0>\d{2}:\d{2}:\d{2})(?P<fsec0>\.\d) +\- +"
                        "(?P<t1>\d{2}:\d{2}:\d{2})(?P<fsec1>\.\d) +"
                        "SVs:(?P<sv1>\d{1,2})-(?P<sv2>\d{1,2}) +"
                        "(?P<carrier>L\d) (?P<code>\S+) +(?P<otype>\S+) +"
                        "N:(?P<n>\d+) +"
                        "bias:(?P<bias>\-?\d+\.\d+) +"
                        "(sdev:(?P<sdev>\d+\.\d+))?")

    def __init__(self, t0=None, t1=None, sv1=None, sv2=None, bias=None, sdev=None):
        self.t0 = t0
        self.t1 = t1
        self.sv1 = sv1
        self.sv2 = sv2
        self.bias = bias
        self.sdev = sdev

    def __str__(self):
        s = DDRecord.__str__(self)
        return "bias:{} {} {}".format(s, self.bias, self.sdev)

    def parse(self, line):
        c = DDarc.cre.match(line)
        if c is None:
            return False

        self.t0 = dates.date2num(
            datetime.datetime.strptime(
                c.group('t0'),  "%H:%M:%S"))
        + float(c.group('fsec0'))/DDRecord.sec_per_day
        self.carrier = c.group('carrier')
        self.code = c.group('code')
        self.otype = c.group('otype')
        self.sv1 = int(c.group('sv1'))
        self.sv2 = int(c.group('sv2'))
        self.bias = float(c.group('bias'))
        return True
#

if __name__ == "__main__":
    import sys
    import argparse
    parser = argparse.ArgumentParser(description="Reads the output of ddGen.")

    parser.add_argument("input_file", metavar='fn', nargs='?', type=argparse.FileType('r'), default=sys.stdin,
                        help="Input data file, defaults to stdin.",)
    parser.add_argument("-d", "--debug", default=0, dest="debug", action="count",
                      help="Increase the debug.")

    args = parser.parse_args()
    DDRecord.debug = args.debug
    for r in DDRecord.reader(args.input_file):
        print r

