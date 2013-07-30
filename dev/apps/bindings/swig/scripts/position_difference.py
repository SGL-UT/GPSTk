#!/usr/bin/env python


import argparse
import gpstk
import matplotlib.pyplot as plt
import numpy as np
import sys


valid_types = ['rinexnav', 'rinex3nav', 'yuma', 'sp3', 'fic']


def triple2array(x):
    return np.array([x[0], x[1], x[2]])


# All data read functions should obey this contract:
#  1. Take filename and prn as parameters
#  2. Return an object that has a position function ((CommonTime) -> numpy.array)
#  3. Return an object that has a first_time function (() -> CommonTime)
#  4. Return an object that has a last_time (() -> CommonTime)
def rinexnav_data(filename, prn):
    header, data = gpstk.readRinexNav(filename, lazy=True)
    sat = gpstk.SatID(prn, gpstk.SatID.systemGPS)
    g = gpstk.GPSEphemerisStore()
    for d in data:
        if prn == d.PRNID:
            ephem = d.toEngEphemeris()
            g.addEphemeris(ephem)

    class rinexnav_holder:
        def __init__(self, gpsStore, satStore):
            self.gpsStore = gpsStore
            self.satStore = satStore
        def first_time(self):
            return self.gpsStore.getInitialTime()
        def last_time(self):
            return self.gpsStore.getFinalTime()
        def position(self, t):
            triple = self.gpsStore.getXvt(self.satStore, t).getPos()
            return triple2array(triple)
    return rinexnav_holder(g, sat)


def rinex3nav_data(filename, prn):
    header, data = gpstk.readRinex3Nav(filename, lazy=True)
    sat = gpstk.SatID(prn, gpstk.SatID.systemGPS)
    g = gpstk.GPSEphemerisStore()
    for d in data:
        if prn == d.PRNID:
            ephem = d.toEngEphemeris()
            g.addEphemeris(ephem)

    class rinex3nav_holder:
        def __init__(self, gpsStore, satStore):
            self.gpsStore = gpsStore
            self.satStore = satStore
        def first_time(self):
            return self.gpsStore.getInitialTime()
        def last_time(self):
            return self.gpsStore.getFinalTime()
        def position(self, t):
            triple = self.gpsStore.getXvt(self.satStore, t).getPos()
            return triple2array(triple)
    return rinex3nav_holder(g, sat)


def sp3_data(filename, prn):
    store = gpstk.SP3EphemerisStore()
    store.loadFile(filename)
    sat = gpstk.SatID(prn)

    class sp3_holder:
        def __init__(self, sp3Store, satStore):
            self.sp3Store = sp3Store
            self.satStore = satStore
        def first_time(self):
            return self.sp3Store.getPositionInitialTime(self.satStore)
        def last_time(self):
            return self.sp3Store.getPositionFinalTime(self.satStore)
        def position(self, t):
            triple = self.sp3Store.getPosition(self.satStore, t)
            return triple2array(triple)
    return sp3_holder(store, sat)


def fic_data(filename, prn):
    isblock9 = lambda x: x.blockNum == 9
    header, data = gpstk.readFIC(filename, lazy=True, filterfunction=isblock9)
    sat = gpstk.SatID(prn, gpstk.SatID.systemGPS)
    g = gpstk.GPSEphemerisStore()
    for d in data:
        ephem = d.toEngEphemeris()
        g.addEphemeris(ephem)

    class fic_holder:
        def __init__(self, gpsStore, satStore):
            self.gpsStore = gpsStore
            self.satStore = satStore
        def first_time(self):
            return self.gpsStore.getInitialTime()
        def last_time(self):
            return self.gpsStore.getFinalTime()
        def position(self, t):
            triple = self.gpsStore.getXvt(self.satStore, t).getPos()
            return triple2array(triple)
    return fic_holder(g, sat)



def yuma_data(filename, prn):
    header, data = gpstk.readYuma(filename, lazy=True)
    for d in data:
        if prn == d.PRN:
            orbit = d.toAlmOrbit()
            break

    class yuma_holder:
        def __init__(self, orbitStore):
            self.orbitStore = orbitStore
        def first_time(self):
            t = orbitStore.getTransmitTime()
            t.addSeconds(-60*60)  # 1 hour before
            return t
        def last_time(self):
            t = orbitStore.getTransmitTime()
            t.addSeconds(60*60)  # 1 hour after
            return t
        def position(self, t):
            triple = self.orbitStore.getPosition(self.satStore, t)
            return triple2array(triple)
    return yuma_holder(orbit)


# Director function for data reading:
def read_data(filetype, filename, prn):
    """Calls the appropriate position reader function based on the filetype."""
    func_name = filetype + '_data'
    possibles = globals().copy()
    possibles.update(locals())
    func = possibles.get(func_name)
    if func is None:
        raise Exception(func + ' is not an implemented function.')
    return func(filename, prn)


def main(args=sys.argv[1:]):
    help = {
        'prn_id':'The integer PRN ID you are interested in.',
        'filetype1':'Type for the first file.',
        'filename1':'File name for the first file.',
        'filetype2':'Type for the second file.',
        'filename2':'File name for the second file.',
        '-v':'Print output locations and error.',
        '-n':'Don\'t plot the file.',
        '-d':'Matplotlib will not connect calculated data points in the plot',
        '-t':'Timestep, in seconds, between plot points.'}

    program_description = ('This program takes 2 nav files and '
                            'provides a plot of the magnitude of '
                            'the difference of a given PRN ID.'
                            'Valid file types are ' + str(valid_types) + '.')

    parser = argparse.ArgumentParser(description=program_description)
    parser.add_argument('prn_id', type=int, help=help['prn_id'])
    parser.add_argument('filetype1', help=help['filetype1'])
    parser.add_argument('filename1', help=help['filename1'])
    parser.add_argument('filetype2', help=help['filetype2'])
    parser.add_argument('filename2', help=help['filename2'])
    parser.add_argument('-v', '--verbose', action="store_true", help=help['-v'])
    parser.add_argument('-n', '--noplot', action="store_true", help=help['-n'])
    parser.add_argument('-d', '--drawdots', action="store_true", help=help['-d'])
    parser.add_argument('-t', '--timestep', type=int, default=300, help=help['-t'])
    args = parser.parse_args(args)


    def check(filetype, filename):
        if not filetype in valid_types:
            print 'Invalid filetype:', filetype
            print 'Valid choices are:' + str(valid_types)
            print 'Use the -h or --help flags for more information.\n'
            sys.exit()
            try:
                with open('filename'): pass
            except IOError:
                print filename, 'cannot be read.\n'
                sys.exit()

    check(args.filetype1, args.filename1)
    check(args.filetype2, args.filename2)

    pos1 = read_data(args.filetype1, args.filename1, args.prn_id)
    pos2 = read_data(args.filetype2, args.filename2, args.prn_id)

    X = []  # list of x plot values
    Y = []  # list of y plot values
    start_time = max(pos1.first_time(), pos2.first_time())
    end_time = min(pos1.last_time(), pos2.last_time())

    t = start_time
    while t < end_time:
        t.addSeconds(args.timestep)
        try:
            p1 = pos1.position(t)
            p2 = pos2.position(t)
            error = np.linalg.norm(p1 - p2)  # euclidian distance
            X.append(t.getDays())
            Y.append(error)
            if args.verbose:
                print 'Time:', gpstk.CivilTime(t)
                print '\tPosition 1:', p1
                print '\tPosition 2:', p2
                print '\tError:', error
        except gpstk.exceptions.InvalidRequest:
            if args.verbose:
                print 'Can\'t use data at:', gpstk.CivilTime(t)

    if not args.noplot:
        fig = plt.figure()
        title = ('Error for PRN ' + str(args.prn_id)
               + ' starting ' + str(gpstk.CivilTime(start_time)))
        fig.suptitle(title, fontsize=14, fontweight='bold')
        ax = fig.add_subplot(111)
        ax.set_xlabel('Time (days)')
        ax.set_ylabel('Error (meters)')
        if args.drawdots:
            plt.plot(X, Y, 'ro')
        else:
            plt.plot(X, Y, 'r')
        plt.show()


if __name__ == '__main__':
    main()
