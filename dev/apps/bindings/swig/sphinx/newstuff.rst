New Functions
==================


Time Functions
**********************************

These are largely helper functions that make some functionality a little easier or more succint.

.. py:function:: gpstk.now([timeSystem=TimeSystem('UTC')])
    :noindex:

Returns the current time in the given TimeSystem. The default is in the UTC time system since the time is
derived from calling SystemTime(). ::

    >>> print gpstk.now()
    2456490 72040524 0.000665000000000 UTC

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

.. autofunction:: gpstk.geocentric(latitude=0.0, longitude=0.0, radius=0.0, model=None, frame=ReferenceFrame('Unknown'))
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

If you want a true stream behavior (i.e. lazy evaluation) for data input,
you can use the optional lazy keyword argument for the read methods,
which will return a generator of data objects instead of a list.

For example: ::

    >>> header, data = gpstk.readYuma('yuma_data.txt', lazy=True)

    >>> print type(data)
    <type 'generator'>

    >>> print data.next()
    PRN = 1
    week = 377
    SV_health = 0
    ecc = 0.006598
    Toa = 319488
    i_offset = 0.0468284
    OMEGAdot = -7.50888e-09
    Ahalf = 5153.68
    OMEGA0 = -0.564433
    w = -1.78141
    M0 = 0.466845
    AF0 = 8.96454e-05
    AF1 = 3.63798e-12
    xmit_time = 0

You can use add a filter to the read on the data objects using the filterfunction keyword argument.
By default, it simply returns True, which includes all data objects in the output.

For example, to get a generator of Rinex3NavData objects with only the PRNID of 3, you could use: ::

    >>> isPRN3 = (lambda x: x.PRNID == 3)

    >>> header, data = gpstk.readRinex3Nav('rinex3nav_data.txt', lazy=True, filterfunction=isPRN3)

    >>> print data.next()
    Sat: G03 TOE: 1274 367200.000 TOC: 1274 367200.000 codeflags:   1 L2Pflag:   0 IODC:  902 IODE:  134 HOWtime: 362376 FitInt:  4.000

    >>> print data.next()
    Sat: G03 TOE: 1274 374400.000 TOC: 1274 374400.000 codeflags:   1 L2Pflag:   0 IODC:  903 IODE:  135 HOWtime: 367206 FitInt:  4.000

    >>> print data.next()
    Sat: G03 TOE: 1274 381600.000 TOC: 1274 381600.000 codeflags:   1 L2Pflag:   0 IODC:  904 IODE:  136 HOWtime: 374406 FitInt:  4.000



The following functions use this pattern:

.. py:function:: gpstk.readFIC(filename[, lazy=False, filterfunction=lamba x: True, filterfunction=lamba x: True])
    :noindex:

.. py:function:: gpstk.readRinex3Clock(filename[, lazy=False, filterfunction=lamba x: True, filterfunction=lamba x: True])
    :noindex:

.. py:function:: gpstk.readRinex3Nav(filename[, lazy=False, filterfunction=lamba x: True])
    :noindex:

.. py:function:: gpstk.readRinex3Obs(filename[, lazy=False, filterfunction=lamba x: True])
    :noindex:

.. py:function:: gpstk.readRinexClock(filename[, lazy=False, filterfunction=lamba x: True])
    :noindex:

.. py:function:: gpstk.readRinexMet(filename[, lazy=False, filterfunction=lamba x: True])
    :noindex:

.. py:function:: gpstk.readRinexNav(filename[, lazy=False, filterfunction=lamba x: True])
    :noindex:

.. py:function:: gpstk.readRinexObs(filename[, lazy=False, filterfunction=lamba x: True])
    :noindex:

.. py:function:: gpstk.readSEM(filename[, lazy=False, filterfunction=lamba x: True])
    :noindex:

.. py:function:: gpstk.readSP3(filename[, lazy=False, filterfunction=lamba x: True])
    :noindex:

.. py:function:: gpstk.readMSC(filename[, lazy=False, filterfunction=lamba x: True])
    :noindex:

.. py:function:: gpstk.readYuma(filename[, lazy=False, filterfunction=lamba x: True])
    :noindex:

.. py:function:: gpstk.writeFIC(filename, header, data)
    :noindex:

.. py:function:: gpstk.writeRinex3Clock(filename, header, data)
    :noindex:

.. py:function:: gpstk.writeRinex3Nav(filename, header, data)
    :noindex:

.. py:function:: gpstk.writeRinex3Obs(filename, header, data)
    :noindex:

.. py:function:: gpstk.writeRinexClock(filename, header, data)
    :noindex:

.. py:function:: gpstk.writeRinexMet(filename, header, data)
    :noindex:

.. py:function:: gpstk.writeRinexNav(filename, header, data)
    :noindex:

.. py:function:: gpstk.writeRinexObs(filename, header, data)
    :noindex:

.. py:function:: gpstk.writeSEM(filename, header, data)
    :noindex:

.. py:function:: gpstk.writeSP3(filename, header, data)
    :noindex:

.. py:function:: gpstk.writeYuma(filename, header, data)
    :noindex:

.. py:function:: gpstk.writeMSC(filename, header, data)
    :noindex:
