#!/usr/bin/env python

"""
A GPSTk example featuring some file input and processing to
create a plot with matplotlib.

Usage:

  python sem_plot.py

"""


import gpstk
import matplotlib.pyplot as plt


def main():
    # Read in data, strict=True makes dataSets a list rather than a generator:
    header, dataSets = gpstk.readSEM( gpstk.data.full_path('sem_data.txt'), strict=True)

    # Write the data back to a different file (their contents should match):
    gpstk.writeSEM( gpstk.data.full_path('sem_data.txt.new'), header, dataSets)

    # Read the orbit out of the data:
    orbit = dataSets[0].toAlmOrbit()  # alm orbit of first data point

    austin = gpstk.Position(30, 97, 0, gpstk.Position.Geodetic)  # Austin, TX

    starttime = gpstk.CommonTime()    # iterator time to start at
    starttime.setTimeSystem(gpstk.TimeSystem('GPS'))
    endtime = gpstk.CommonTime()  # end time, 1 day later (see below)
    endtime.setTimeSystem(gpstk.TimeSystem('GPS'))
    endtime.addDays(1)

    X = []
    Y = []

    # Step through a day, adding plot points:
    for t in gpstk.times(starttime, endtime, seconds=1000):
        xvt = orbit.svXvt(t)
        location = gpstk.Position(xvt.x)
        elevation = austin.elevation(location)
        X.append(t.getDays())
        Y.append(elevation)

    # Make the plot
    fig = plt.figure()
    fig.suptitle('Elevation of a GPS satellite throughout the day',
                 fontsize=14, fontweight='bold')
    ax = fig.add_subplot(111)
    ax.set_xlabel('Time (days)')
    ax.set_ylabel('Elevation (degrees)')
    plt.plot(X, Y, 'r')
    plt.show()


if __name__ == '__main__':
    main()
