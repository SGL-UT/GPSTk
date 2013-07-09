import gpstk
import matplotlib.pyplot as plt


def main():
    # Read in data:
    header, dataSets = gpstk.readSEMFile('sem_plot_data.txt')

    # Write the data back to a different file (their contents should match):
    gpstk.writeSEMFile('sem_plot_data.txt.new', header, dataSets)

    # Rranslate the data to AlmOrbits:
    almOrbits = [data.toAlmOrbit() for data in dataSets]

    # Step through a day, adding plot points:
    X = []
    Y = []
    t = gpstk.CommonTime()
    t_f = gpstk.CommonTime()
    t_f.addDays(1)

    while t < t_f:
        xvt = almOrbits[18].svXvt(t)
        austin = gpstk.Position(30, 97, 0, gpstk.Position.Geodetic)  # Austin, TX
        location = gpstk.Position(xvt.x)
        elevation = austin.elevation(location)

        X.append(t.getSecondOfDay())
        Y.append(elevation)
        t.addSeconds(60*30)

    # Make the plot
    fig = plt.figure()
    fig.suptitle('Elevation of a GPS satellite throughout the day',
                 fontsize=14, fontweight='bold')
    ax = fig.add_subplot(111)
    ax.set_xlabel('Time (seconds)')
    ax.set_ylabel('Elevation (degrees)')
    plt.plot(X, Y, 'ro')
    plt.show()


if __name__ == '__main__':
    main()
