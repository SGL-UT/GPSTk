# this is a port of dev/src/examples/example2.cpp

import gpstk


def main():
    # Read in the rinex data
    header, data = gpstk.readRinex3Obs('bahr1620.04o')

    # Let's pretend we want to change something in the header
    # (otherwise this would be a two-line example!)
    outHeader = header
    outHeader.receiverOffset = 47

    # Now let's find the earliest observation time in the data
    earliest = gpstk.CommonTime.END_OF_TIME
    for d in data:
        if d.time < earliest:
            earliest = d.time

    print 'Earliest time found:', gpstk.CivilTime(earliest)

    # Now let's write it all back to a different file
    gpstk.writeRinex3Obs('bahr1620.04o', outHeader, data)


if __name__ == '__main__':
    main()
