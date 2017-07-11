ftp://ssd.jpl.nasa.gov/pub/eph/planets
*******************************************************************************

JPL PLANETARY AND LUNAR EPHEMERIDES : Export Information     [29 November 2011]

*******************************************************************************

INTRODUCTION
------------

JPL planetary ephemerides are generally created to support spacecraft missions
to the planets. Selected ephemerides are recommended for more general use, 
in particular DE405 which is the basis for the Astronomical Almanac, 
and DE421, which is the latest JPL ephemeris with fully consistent treatment 
of planetary and lunar laser ranging data (Folkner et al 2009).
Other ephemerides generated for specific mission requirements may be available
for use but usually without documentation available to outside users.

The JPL planetary ephemerides are saved as files of Chebyshev polynomials fit 
to the Cartesian positions and velocities of the planets, Sun, and Moon, 
typically in 32-day intervals. The positions are integrated in astronomical 
units (AU), but with polynomials stored in units of kilometers. The integration
time units are days of barycentric dynamical time (TDB). The value of the 
astronomical unit is estimated based from measurements of planetary orbits 
using the Gassian gravitational constant k as adopted by the International 
Astronomical Union. The mass parameter (GM) of the Sun is related to the 
estimated AU by the relation GMsun = k*k/AU. Thus a change in the value of the
AU corresponds to a change in the GM of the Sun in SI units. 
Since the ephemeris fits are sensitive to the GM of the Sun, each ephemeris 
generally has a different estimate of the AU. The mass parameters (GM) of the 
planets are best determined by tracking of spacecraft in orbit about or 
encountering the planets. Each ephemeris uses the best estimates of the 
planetary GM values available at the time.

Most JPL planetary ephemeris files include Chebyshev polynomials fit to 
the lunar libration angles, which are integrated along with the planetary 
positions. Many ephemeris files also a fit to the 1980 IAU nutation series. 
While the 1980 IAU nutation seriesis not current, it is maintained in the 
files for backward compatibility.


ACCESSING THE JPL PLANETARY EPHEMERIDES
------------------------------------------

For users who need positions of planets at a few specific time, 
JPL's interactive website and telnet service, "Horizons", provides a wide 
variety of astronomical information including planetary positions from the 
planetary ephemeris DE405, at web site  http://ssd.jpl.nasa.gov/?horizons .


For users who need the capability of looking up the positions of planets at many
times or as needed for other computations, the recommended means of reading the
ephemerides is through use of the SPICE software toolkit, 
http://naif.jpl.nasa.gov/naif/index.html. 
The SPICE software is supported in several languages (including C, Fortran, 
and Matlab) on multiple platforms and compilers. The SPICE software reads JPL 
planetary ephemerides in a machine-independent binary format (kernals) which 
are available from the SPICE web site and by anonymous ftp from 
ftp://ssd.jpl.nasa.gov/pub/eph/planets/bsp .


The JPL planetary ephemerides are also available in an ASCII format which are 
converted by the user to machine-dependent binary files for reading. 
The ASCII files are created in blocks of 20 or more years. Later ephemerides 
tend to be stored in longer blocks. When converted into binary format, 
the files can be merged to span a longer time if desired. 
The ASCII files are available by anonymous ftp from 
ftp://ssd.jpl.nasa.gov/pub/eph/planets/ascii/ .
The ascii file format is briefly described in the file
ftp://ssd.jpl.nasa.gov/pub/eph/planets/ascii/ascii_format.txt .

The machine-dependent binary files differ mainly in the binary representation
of numbers. The 'little-endian' representation is used by Intel x86, IA64 or 
Alpha based processors (and their clones). Some binary ephemeris files are 
available in this format from ftp://ssd.jpl.nasa.gov/pub/eph/planets/Linux . 
The 'big endian' representation is used by PowerPC and SPARC processors. 
Some binary files in this format are available from 
ftp://ssd.jpl.nasa.gov/pub/eph/planets/SunOS .

Fortran programs for converting the ASCII files and for merging and reading
the binary files are available by anonymous ftp from 
ftp://ssd.jpl.nasa.gov/pub/eph/planets/fortran . 
These Fortran programs generally require some tailoring by the user. 
Instructions for using these programs are available for downloading from 
ftp://ssd.jpl.nasa.gov/pub/eph/planets/fortran/userguide.txt . 
Other programs for reading these formats have been developed independently by 
outside programmers, and not suported directly by JPL, are listed at 
ftp://ssd.jpl.nasa.gov/pub/eph/planets/other_readers.txt .


AVAILABLE EPHEMERIDES
---------------------

DE102 : Created September 1981; includes nutations but not librations.
        Referred to the dynamical equator and equinox of 1950.
        Covers JED 1206160.5 (-1410 APR 16) to JED 2817872.5 (3002 DEC 22).

DE200 : Created September 1981; includes nutations but not librations.
        Referred to the dynamical equator and equinox of 2000.
        Covers JED 2305424.5 (1599 DEC 09)  to  JED 2513360.5 (2169 MAR 31).
        This ephemeris was used for  the Astronomical Almanac from 1984 to 2003.
        (See Standish, 1982 and Standish, 1990).

DE202 : Created October 1987; includes nutations and librations.
        Referred to the dynamical equator and equinox of 2000.
        Covers JED 2414992.5 (1899 DEC 04) to  JED 2469808.5 (2050 JAN 02).

DE403 : Created May 1993; includes nutations and librations.
        Referred to the International Celestial Reference Frame.
        Covers JED 2305200.5 (1599 APR 29) to  JED 2524400.5 (2199 JUN 22).
        Fit to planetary and lunar laser ranging data.
        (See Folkner et al. 1994).

DE405 : Created May 1997; includes both nutations and librations.
        Referred to the International Celestial Reference Frame.
        Covers JED 2305424.50  (1599 DEC 09)  to  JED 2525008.50  (2201 FEB 20)

DE406 : Created May 1997; includes neither nutations nor librations.
        Referred to the International Celestial Reference Frame.
        Spans JED 0624976.50 (-3001 FEB 04) to 2816912.50 (+3000 MAY 06)

        This is the same integration as DE405, with the accuracy of the 
        interpolating polynomials has been lessened to reduce file size
        for the longer time span covered by the file.

DE410 : Created April 2003; includes nutations and librations.
        Referred to the International Celestial Reference Frame.
        Covers JED 2415056.5 (1900 FEB 06) to JED 2458832.5 (2019 DEC 15).
        Ephemeris used for Mars Exploration Rover navigation.

DE413 : Created November 2004; includes nutations and librations.
        Referred to the International Celestial Reference Frame.
        Covers JED 2414992.5, (1899 DEC 04) to JED 2469872.5 (2050 MAR 07).

        Created to update the orbit of Pluto to aid in planning
        for an occultation of a relatively bright star by Charon on 11 July 2005.

DE414 : Created May 2005; includes nutations and librations.
        Covers JED 2414992.5, (1899 DEC 04) to JED 2469872.5 (2050 MAR 07).

        Fit to ranging data from MGS and Odyssey through 2003.
        (See Konopliv et al., 2006.)

DE418 : Created August 2007; includes nutations and librations.
        Covers JED 2414864.5 (1899 JUL 29) to JED 2470192.5 (2051 JAN 21)

DE421 : Created Feb 2008; includes nutations and librations.
        Referred to the International Celestial Reference Frame.
        Covers JED 2414864.5 (1899 JUL 29) to JED 2471184.5 (2053 OCT 09)

        Fit to planetary and lunar laser ranging data.
        (See Folkner et al., 2009)

DE422 : Created September 2009; includes nutations and librations.
        Referred to the International Celestial Reference Frame.
        Covers JED 625648.5, (-3000 DEC 07) to JED 2816816.5, (3000 JAN 30).

        Intended for the MESSENGER mission to Mercury.
        Extended integration time to serve as successor to DE406.
        Fit to ranging data from MGS and Odyssey through 2003.
        (See Konopliv et al., 2010.)

DE423 : Created February 2010; includes nutations and librations.
        Referred to the International Celestial Reference Frame  version 2.0.
        Covers JED 2378480.5, (1799 DEC 16) to JED  2524624.5, (2200 FEB 02).

        Intended for the MESSENGER mission to Mercury.

REFERENCES
----------

Folkner et al., Astronomy and Astrophysics v. 287, pp. 279-289, 1994.

Folkner et al., JPL Interplanetary Network Progress Report 24-178, 2009
   http://ipnpr.jpl.nasa.gov/progress_report/42-178/178C.pdf

Konopliv et al., Icarus v. 182, pp. 23-26, 2006.

Konopliv et al., Icarus v. 211, pp. 401-428, 2011.

Newhall, X X, Standish, E.M. and Williams, J.G.: 1983, "DE102: a numerically 
   integrated ephemeris of the Moon and planets spanning forty-four centuries",
   Astronomy & Astrophysics, vol. 125, pp. 150-167.

Standish, E.M.: 1982, "Orientation of the JPL Ephemerides, DE200/LE200, to 
   the Dynamical Equinox of J2000", Astronomy & Astrophysics, vol. 114, 
   pp. 297-302.

 Standish, E.M.: 1990, "The Observational Basis for JPL's DE200, the 
   planetary ephemeris of the Astronomical Almanac", Astronomy & Astrophysics,
   vol. 233, pp. 252-271.

 Standish, E.M., Newhall, X X, Williams, J.G. and Folkner, W.F.: 1995, 
   "JPL Planetary and Lunar Ephemerides, DE403/LE403", JPL IOM 314.10-127.

 Standish, E.M.: 1998, "JPL Planetary and Lunar Ephemerides,
   DE405/LE405",JPL IOM  312.F-98-048. 

ASSISTANCE
----------

Questions may be addressed to;

William Folkner; JPL m/s 301-150; Pasadena, CA  91109
TEL: 818-354-0443                   FAX: 818-393-7631
e-mail: William.Folkner@jpl.nasa.gov                 
