New Functions
==================

Time Functions
**********************************

These are largely helper functions that make some functionality a little easier or more succinct.

.. py:function:: gpstk.now([timeSystem=TimeSystem('UTC')])
    :noindex:

Returns the current time in the given TimeSystem. The default is in the UTC time system since the time is
derived from calling SystemTime(). ::

    >>> print gpstk.now()
    2456490 72040524 0.000665000000000 UTC


.. py:function:: gpstk.times(starttime, endtime[, seconds=0.0, days=0])
    :noindex:

This returns a generator of CommonTime objects that starts at starttime and advances
by the seconds and days parameter each time. Special cases (no timestep parameters,
negative parameters) are documented in the function's docstring and the quick reference section. ::

    >>> start = gpstk.now()
    >>> # wait a few seconds...
    >>> end = gpstk.now()
    >>> times = list(gpstk.times(start, end, seconds=1.0))
    >>> for t in times:
    ...     print t

    2456527 60045625 0.000057000000000 UTC
    2456527 60046625 0.000057000000000 UTC
    2456527 60047625 0.000057000000000 UTC
    2456527 60048625 0.000057000000000 UTC
    2456527 60049625 0.000057000000000 UTC
    2456527 60050625 0.000057000000000 UTC
    2456527 60051625 0.000057000000000 UTC
    2456527 60052625 0.000057000000000 UTC
    2456527 60053625 0.000057000000000 UTC



Position Functions
**********************************
Some helpful functions for creating Position objects more easily (with keyword arguments) were also added:

.. py:function:: gpstk.cartesian([x=0.0, y=0.0, z=0.0, model=None, frame=ReferenceFrame('Unknown)'])
    :noindex:

    >>> print gpstk.spherical(radius=5, theta=45, phi=45)
    45.00000000 deg 45.00000000 deg 5.0000 m

.. autofunction:: gpstk.spherical([theta=0.0, phi=0.0, radius=0.0, model=None, frame=ReferenceFrame('Unknown')])
    :noindex:

.. autofunction:: gpstk.geodetic([latitude=0.0, longitude=0.0, height=0.0, model=None, frame=ReferenceFrame('Unknown')])
    :noindex:

.. autofunction:: gpstk.geocentric([latitude=0.0, longitude=0.0, radius=0.0, model=None, frame=ReferenceFrame('Unknown')])
    :noindex:

The next four functions are simply light wrappers over some relatively simple classes.
The use of a entire class objects was warranted in C++, but they are not neccessary
for typical python usage.

.. autofunction:: gpstk.moonPosition
    :noindex:

    >>> print gpstk.moonPosition(gpstk.now())
    (62424169.398472935, -365987646.51255625, -83100797.60563189)


.. autofunction:: gpstk.sunPosition
    :noindex:

.. autofunction:: gpstk.poleTides
    :noindex:

.. autofunction:: gpstk.solidTides
    :noindex:

    >>> print gpstk.solidTides(gpstk.now(), gpstk.geodetic(latitude=29, longitude=95, height=0))
    (0.1601640329929359, -0.0031534542100034955, -0.03016846270875466)



.. _fileio_label:

File I/O Functions
*******************

To replace the stream-oriented paradigm for file i/o used in the C++
implementation, some thin wrapping was placed over the Stream objects.

Because the i/o strategy was uniform across all file types in C++, the
strategy is similarly uniform for the python bindings.

As an example, to read a SP3 file: ::

    >>> header, data = gpstk.readSP3('sp3_data.txt')


And to write an SP3 file... ::

    >>> gpstk.writeSP3('sp3_data.txt.new', header, data)


In this case, header is a SP3Header. Data is a list of SP3Data objects.

By default, the data returned is a generator expression - which means
it closely mimics the C++ stream in that it only returns an object when it is
needed. If you want a strictly evaluated list (all of the objects sit in memory)
you can use the strict=True keyword argument or just translate the generator to
a list yourself using the list() initializer with the generator as the argument.

A common paradigm is to loop over all elements in the data and process them.
As an example, to print all the data sets in a RINEX 3 Nav file: ::

    header, data = gpstk.readRinex3Nav('rinex3nav_data.txt')
    for d in data:
        print d



You can also add a filter to the read on the data objects using the filterfunction keyword argument.
By default, it simply returns True, which includes all data objects in the output.

For example, to get a generator of Rinex3NavData objects with only the PRNID of 3, you could use: ::

    >>> isPRN3 = (lambda x: x.PRNID == 3)

    >>> header, data = gpstk.readRinex3Nav('rinex3nav_data.txt', filterfunction=isPRN3)

    >>> print data.next()
    Sat: G03 TOE: 1274 367200.000 TOC: 1274 367200.000 codeflags:   1 L2Pflag:   0 IODC:  902 IODE:  134 HOWtime: 362376 FitInt:  4.000

    >>> print data.next()
    Sat: G03 TOE: 1274 374400.000 TOC: 1274 374400.000 codeflags:   1 L2Pflag:   0 IODC:  903 IODE:  135 HOWtime: 367206 FitInt:  4.000

    >>> print data.next()
    Sat: G03 TOE: 1274 381600.000 TOC: 1274 381600.000 codeflags:   1 L2Pflag:   0 IODC:  904 IODE:  136 HOWtime: 374406 FitInt:  4.000


The following formats use this pattern (all read/write semantics are identical):

* Rinex3Clock
* Rinex3Nav
* Rinex3Obs
* RinexClock
* RinexMet
* RinexNav
* RinexObs
* SEM
* SP3
* Yuma

Miscellaneous
****************************

Several of the constants in GPS_URA.hpp were C-style arrays. These could not be converted
directly to Python lists (without code duplication), so functions were added that gave
access to the underlying array.

A C++ call to gpstk::SV_ACCURACY_GPS_MIN_INDEX[3] would become a Python statement of gpstk.constants.sv_accuracy_gps_min_index(3)

