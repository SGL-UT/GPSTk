#!/usr/bin/env python

import argparse
import gpstk
import matplotlib.pyplot as plt
import numpy as np
import sys


valid_types = ['rinexnav', 'rinex3nav', 'yuma', 'sp3', 'fic', 'sem']


def triple2Position(x):
    return gpstk.Position(x[0], x[1], x[2])


# All data read functions should obey this contract:
# 1. Take filename and prn as parameters
# 2. Return an object that has a position function ((CommonTime) -> numpy.array)
# 3. Return an object that has a first_time function (() -> CommonTime)
# 4. Return an object that has a last_time (() -> CommonTime)
def rinexnav_data(filename, prn):
    header, data = gpstk.readRinexNav(filename)
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
            return triple2Position(triple)
    return rinexnav_holder(g, sat)


def rinex3nav_data(filename, prn):
    header, data = gpstk.readRinex3Nav(filename)
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
            return triple2Position(triple)
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
            return triple2Position(triple)
    return sp3_holder(store, sat)


def yuma_data(filename, prn):
    header, data = gpstk.readYuma(filename)
    sat = gpstk.SatID(prn, gpstk.SatID.systemGPS)
    almanac = gpstk.GPSAlmanacStore()
    for d in data:
        if prn == d.PRN:
            orbit = d.toAlmOrbit()
            almanac.addAlmanac(orbit)

    class yuma_holder:
        def __init__(self, almanacStore, satStore):
            self.almanacStore = almanacStore
            self.satStore = satStore
        def first_time(self):
            return self.almanacStore.getInitialTime()
        def last_time(self):
            return self.almanacStore.getFinalTime()
        def position(self, t):
            triple = self.almanacStore.getXvt(self.satStore, t).getPos()
            return triple2Position(triple)
    return yuma_holder(almanac, sat)


def sem_data(filename, prn):
    header, data = gpstk.readSEM(filename)
    sat = gpstk.SatID(prn, gpstk.SatID.systemGPS)
    almanac = gpstk.GPSAlmanacStore()
    for d in data:
        if prn == d.PRN:
            orbit = d.toAlmOrbit()
            almanac.addAlmanac(orbit)

    class sem_holder:
        def __init__(self, almanacStore, satStore):
            self.almanacStore = almanacStore
            self.satStore = satStore
        def first_time(self):
            return self.almanacStore.getInitialTime()
        def last_time(self):
            return self.almanacStore.getFinalTime()
        def position(self, t):
            triple = self.almanacStore.getXvt(self.satStore, t).getPos()
            return triple2Position(triple)
    return sem_holder(almanac, sat)


# Director function for data reading:
def read_data(filetype, filename, prn):
    """Calls the appropriate position reader function based on the filetype."""
    func_name = filetype + '_data'
    possibles = globals().copy()
    possibles.update(locals())
    func = possibles.get(func_name)
    if func is None:
        raise NotImplementedError(func + ' is not an implemented function.')
    return func(filename, prn)


def main(args=sys.argv[1:]):
    program_description = ('This program takes 2 nav files and '
                            'provides a plot of the magnitude of '
                            'the difference of a given PRN ID.'
                            'Valid file types are ' + str(valid_types) + '.')

    parser = argparse.ArgumentParser(description=program_description)
    parser.add_argument('prn_id', type=int,
                        help='The integer PRN ID you are interested in.')
    parser.add_argument('filetype1', help='Type for the first file.')
    parser.add_argument('filename1', help='File name for the first file.')
    parser.add_argument('filetype2', help='Type for the second file.')
    parser.add_argument('filename2', help='File name for the second file.')
    parser.add_argument('-v', '--verbose', action="store_true",
                        help='Print output locations and error.')
    parser.add_argument('-n', '--noplot', action="store_true",
                        help='Don\'t plot the file.')
    parser.add_argument('-d', '--drawdots', action="store_true",
                        help='Matplotlib will not connect calculated data '
                        'points in the plot')
    parser.add_argument('-t', '--timestep', type=int, default=300,
                        help='Timestep, in seconds, between plot points.')
    parser.add_argument('-s', '--save', help='Save the image to <file>.')
    parser.add_argument('-f', '--format', default='%02H:%02M',
                        help='Format for x time ticks.')

    args = parser.parse_args(args)


    def timestr(t):
        return str(gpstk.CivilTime(t))

    def check(filetype, filename):
        if not filetype in valid_types:
            print 'Invalid filetype:', filetype
            print 'Valid choices are:' + str(valid_types)
            print 'Use the -h or --help flags for more information.\n'
            sys.exit()
            try:
                with open('filename'): pass
            except IOError as e:
                print e
                sys.exit(filename, 'cannot be read.\n')

    check(args.filetype1, args.filename1)
    check(args.filetype2, args.filename2)

    pos1 = read_data(args.filetype1, args.filename1, args.prn_id)
    pos2 = read_data(args.filetype2, args.filename2, args.prn_id)

    X = []  # list of x plot values
    Y = []  # list of y plot values

    starttime = max(pos1.first_time(), pos2.first_time())
    endtime = min(pos1.last_time(), pos2.last_time())

    if args.verbose:
        print (args.filename1 + ' ranges from ' + timestr(pos1.first_time())
              + ' to ' + timestr(pos1.last_time()))
        print (args.filename2 + ' ranges from ' + timestr(pos2.first_time())
              + ' to ' + timestr(pos2.last_time()))
        print 'Earliest time computable:', timestr(starttime)
        print 'Latest time computable:', timestr(endtime), '\n'

    sumErr = 0.0
    sumErrSq = 0.0
    n = 0
    maxErr = 0.0

    for t in gpstk.times(starttime, endtime, seconds=args.timestep):
        try:
            p1 = pos1.position(t)
            p2 = pos2.position(t)
            error = gpstk.range(p1, p2)
            maxErr = max(maxErr, error)
            X.append(t.getDays())
            Y.append(error)
            if args.verbose:
                sumErr += error
                sumErrSq += error*error
                n += 1
                print 'Time:', timestr(t)
                print '\tPosition 1:', p1
                print '\tPosition 2:', p2
                print '\tError:', error
        except gpstk.exceptions.InvalidRequest:
            if args.verbose:
                print 'Can\'t use data at:', timestr(t)

    if args.verbose and n > 0:
        print 'Arithmetic mean of error values: ', sumErr / n, 'm'
        print 'Root mean square of error values:', np.sqrt(sumErrSq / n), 'm'

    fig = plt.figure()
    title = ('Error for PRN ' + str(args.prn_id) + ' starting '
        + gpstk.CivilTime(starttime).printf('%02m/%02d/%04Y %02H:%02M:%02S'))

    fig.suptitle(title, fontsize=14, fontweight='bold')
    ax = fig.add_subplot(111)
    ax.text(0.90, 0.90, args.filetype1 + ': ' + args.filename1
        + '\n' + args.filetype2 + ': ' + args.filename2,
        verticalalignment='bottom', horizontalalignment='right',
        transform=ax.transAxes)
    ax.set_xlabel('Time')
    ax.set_ylabel('Error (meters)')
    if args.drawdots:
        plt.plot(X, Y, 'ro')
    else:
        plt.plot(X, Y, 'r')

    # sets the y scale
    plt.ylim([0, 2.0 * maxErr])

    # converts common time day (float) -> string
    def daytostring(x):
        t = gpstk.CommonTime()
        t.set(x)
        return gpstk.CivilTime(t).printf(args.format)

    # sets the text shown per-pixel when viewed interactively
    def format_coord(x, y):
        return 'x=' + daytostring(x) + ', y=%1.4f'%(y)
    ax.format_coord = format_coord

    # sets x ticks to use the daytostring text
    locs, labels = plt.xticks()
    for i in range(len(locs)):
        labels[i] = daytostring(locs[i])
    ax.set_xticklabels(labels)

    if not args.noplot:
        plt.show()

    if args.save is not None:
        fig.savefig(args.save)


def run(prn, filetype1, filename1, filetype2, filename2,
        verbose=False, noplot=False, savelocation=None,
        drawdots=False, timestep=300, format='%02H:%02M'):
    """
    Functional interface to the position_difference script for easier
    calling. See help on the script (python position_difference.py -h) for more.
    """
    commands = [str(prn), filetype1, filename1, filetype2, filename2,
               '-t', str(timestep), '-f', format]
    if verbose:
        commands.append('-v')
    if noplot:
        commands.append('-n')
    if savelocation is not None:
        commands.append('-s')
        commands.append(savelocation)
    if drawdots:
        commands.append('-d')
    main(commands)


if __name__ == '__main__':
    main()
