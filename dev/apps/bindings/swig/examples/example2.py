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
    header, data = gpstk.readRinex3Obs('rinex3obs_data.txt')

    # Let's pretend we want to change something in the header
    # (otherwise this would be a two-line example!)
    outHeader = header
    outHeader.receiverOffset = 47

    # Now let's find the earliest and latest observations
    earliest = min(data, key=lambda dataPoint: dataPoint.time)
    latest = max(data, key=lambda dataPoint: dataPoint.time)

    print 'Earliest time found:', gpstk.CivilTime(earliest.time)
    print 'Latest time found:  ', gpstk.CivilTime(latest.time)

    # Now let's write it all back to a different file
    gpstk.writeRinex3Obs('rinex3obs_data.txt.new', outHeader, data)


if __name__ == '__main__':
    main()
