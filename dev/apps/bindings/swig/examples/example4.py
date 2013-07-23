"""
...

Usage:
  python example4.py <RINEX Obs file> <RINEX Nav file> [<RINEX Met file>]'

For example:
  python example4.py rinex3obs_data.txt rinex3nav_data.txt rinex3met_data.txt

"""


import gpstk
import sys


def main(args=sys.argv[1:]):


    if not (2 <= len(args) <= 3):
        print 'This program requires at least 2 parameters:'
        print 'Usage: <RINEX Obs file> <RINEX Nav file> [<RINEX Met file>]'
        sys.exit()

    rinexobs_filename = args[0]
    rinexnav_filename = args[1]
    if len(args) == 3:
        rinexmet_filename = args[2]
    else:
        rinexmet_filename = None


    bcestore = gpstk.GPSEphemerisStore()
    # TODO exception:
    if True:
        navHeader, navData = gpstk.readRinex3Nav(rinexnav_filename)
        for navDataObj in navData:
            ephem = navDataObj.toEngEphemeris()
            # print type(ephem)
            bcestore.addEphemeris(gpstk.EngEphemeris())  # TODO: causes a TypeError

        bcestore.SearchNear()

        metData = []
        if rinexmet_filename is not None:
            metHeader, metData = gpstk.readRinex3Met(rinexmet_filename)

        obsHeader, obsData = gpstk.readRinex3Obs(rinexobs_filename)

        # The following lines fetch the corresponding indexes for some
        # observation types we are interested in. Given that old-style
        # observation types are used, GPS is assumed.
        try:
            indexP1 = obsHeader.getObsIndex('P1')
        except:
            print 'The observation files has no P1 pseudoranges.'
            sys.exit()

        try:
            indexP2 = obsHeader.getObsIndex('P2')
        except:
            indexP2 = -1

        for d in obsData:
            pass








    # gpstk.constants.GAMMA_GPS




if __name__ == '__main__':
    main()
