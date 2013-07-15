# this is a port of dev/src/examples/example2.cpp

import gpstk


def main():
    # Read in the rinex data
    header, data = gpstk.readRinex3Obs('rinex3obs_data.txt')

    # Let's pretend we want to change something in the header
    # (otherwise this would be a two-line example!)
    outHeader = header
    outHeader.receiverOffset = 47

    # Now let's find the earliest observation time in the data
    earliest = min(data, key=lambda dataPoint: dataPoint.time)

    print 'Earliest time found:', gpstk.CivilTime(earliest.time)

    # Now let's write it all back to a different file
    gpstk.writeRinex3Obs('rinex3obs_data.txt.new', outHeader, data)


if __name__ == '__main__':
    main()
