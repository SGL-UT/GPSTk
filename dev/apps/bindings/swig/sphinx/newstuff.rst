New Functions
==================


Time Functions
***************

These are largely helper functions that make some functionality a little easier or more succint.

.. py:function:: gpstk.now()
    :noindex:

Returns the current time (as defined by what SystemTime() returns) as a CommonTime.
Example: ::

    >>> print gpstk.now()
    2456490 72040524 0.000665000000000 UTC

    >>> print gpstk.CivilTime(gpstk.now())
    07/16/2013 19:59:25 UTC



.. py:function:: gpstk.timeSystem([str='Unknown'])
    :noindex:

Returns a gpstk.TimeSystem object given a string input. T
his is just a more succint way of creating a TimeSystem. ::

    >>> print gpstk.TimeSystem(gpstk.TimeSystem.GPS)
    GPS
    >>> print gpstk.timeSystem('GPS')
    GPS



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



The following functions use this pattern:

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

.. py:function:: gpstk.readYuma(filename[, lazy=False, filterfunction=lamba x: True])
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

