"""
A GPSTk example featuring some file input and processing to
create a plot with matplotlib.

Usage:

  python sem_plot.py

"""


import gpstk
import matplotlib.pyplot as plt


def main():
    # Read in data:
    header, dataSets = gpstk.readSEM('sem_data.txt')

    # Write the data back to a different file (their contents should match):
    gpstk.writeSEM('sem_data.txt.new', header, dataSets)

    # Rranslate the data to AlmOrbits:
    almOrbits = [data.toAlmOrbit() for data in dataSets]

    t = gpstk.CommonTime()    # iterator time
    t_f = gpstk.CommonTime()  # end time, 1 day later (see below)
    t_f.addDays(1)
    X = []
    Y = []

    # Step through a day, adding plot points:
    dt = 60  # time step, in seconds
    austin = gpstk.Position(30, 97, 0, gpstk.Position.Geodetic)  # Austin, TX
    while t < t_f:
        xvt = almOrbits[0].svXvt(t)  # the xvt of the first orbit
        location = gpstk.Position(xvt.x)
        elevation = austin.elevation(location)

        X.append(t.getSecondOfDay())
        Y.append(elevation)
        t.addSeconds(dt)

    # Make the plot
    fig = plt.figure()
    fig.suptitle('Elevation of a GPS satellite throughout the day',
                 fontsize=14, fontweight='bold')
    ax = fig.add_subplot(111)
    ax.set_xlabel('Time (seconds)')
    ax.set_ylabel('Elevation (degrees)')
    plt.plot(X, Y, 'r')
    plt.show()


if __name__ == '__main__':
    main()
