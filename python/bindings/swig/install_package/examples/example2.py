#!/usr/bin/env python

"""
A GPSTk example with some simple time classes and functions.

This is a semi-port of the older C++ example2.cpp. The original
only imported and rewrote the same file. But that would be
completely trivial for python.

Usage:
  python example2.py

"""


import gpstk


def main():
    # Read in the rinex data
    header, data = gpstk.readRinex3Obs( gpstk.data.full_path('rinex2obs_data.txt'), strict=True)

    # Let's pretend we want to change something in the header
    # (otherwise this would be a two-line example!)
    header.receiverOffset = 47

    # Now let's find the earliest and latest observations
    # function for how to compare Rinex3ObsData objects for min/max functions:
    timeFunction = lambda self: self.time
    earliest = min(data, key=timeFunction)
    latest = max(data, key=timeFunction)

    print 'Earliest time found:', gpstk.CivilTime(earliest.time)
    print 'Latest time found:  ', gpstk.CivilTime(latest.time)

    # Now let's write it all back to a different file
    gpstk.writeRinex3Obs( 'rinex3obs_data.txt.new', header, data)


if __name__ == '__main__':
    main()
