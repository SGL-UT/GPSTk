GPSTk Python Bindings
---------------------

Contents:
---------

* Introduction
* Building and Installing the Python GPSTk
* About the Python Bindings
* A Note on Python Versions
* Package Organization
* How to Use This Documentation
* Scope
* How some particular C++ structures are translated to Python
* New Functions

Introduction:
-------------

This documentation describes the architecture and design of the Python bindings of the 
GPS ToolKit (GPSTk), produced by the Space and Geophysics Laboratory (SGL) of Applied 
Research Laboratories, The University of Texas at Austin (ARL:UT).

Building and Installing the Python GPSTk:
-----------------------------------------

See the INSTALL.txt for details.


About the Python Bindings:
--------------------------

Though effort has been taken to reduce the C++-feeling when using these Python classes, it is 
impossible to completely purify the library without creating severe maintenance problems. Thus, 
the user must be aware that the GPSTk is primarily a C++ library and some things may require 
unusual semantics. It is recommended that you avoid using some of these unusual classes, such as 
gpstk.cpp.vector_double (std::vector<double>) as much as a possible. A much better replacement would 
be a numpy array.

Example of how C++ GPSTk reads and prints a Rinex3Obs file:

	|  // Create the input file stream
	| Rinex3ObsStream rin("rinex3obs_data.txt");
	|
	| // Read the RINEX header
	| Rinex3ObsHeader head;    // RINEX header object
	| rin >> head;
	| std::cout << head << std::endl;
	| 
	| // Loop over all data epochs
	| Rinex3ObsData data;   // RINEX data object
	| while (rin >> data)
	| {
	|    std::cout << data << std::endl;
	| }

Compare to the simpler Python GPSTk read process:

	| # read in the header and data
	| header, data = readRinex3Obs('rinex3obs_data.txt')
	| print header
	| for dataObject in data:
	|     print data


A Note on Python Versions:
--------------------------

These bindings were created with Python 2.7 in mind and that is how they are supported at the 
moment. It is likely Python 3.2 or 3.3 will be supported some time in the future.

The standard Python implementation, CPython, must be used. The wrapping makes use of the C Python 
API heavily.


Package Organization:
---------------------

Most classes and functions are in the gpstk namespace, but there are some exceptions.

	1. Constant values not inside a class are placed in the gpstk.constants submodule.

	2. GPSTk Exceptions are placed in the gpstk.exceptions submodule.

	3. Wrappers of C++ library structures (std::vector and std::map, which should only be used when necessary) 
		are placed in the gpstk.cpp submodule.


How to Use This Documentation:
------------------------------

The C++ API is the primary reference still, however much of it is embedded into the python source 
in the form of docstrings. This means you can use ipython to easily find out about classes and functions.

Changes and additions to the C++ core are noted in this documentation, as well as in the docstrings themselves.

A good first step is to browse the things available in the Quick Reference section. If you are coming from 
writing C++ GPSTk programs, you should be sure to read the section on how C++ structures have been changed 
for python.


Scope:
------

The GPSTk C++ library is the product of a large number of people over a sparse time period; as a result, the 
focus of the library has been somewhat lost. There are many non-GPS related utilities implemented that have 
been implemented more efficiently, robustly, and accurately by others. These bindings attempt to expose as little 
as needed to the programmer.

Prime examples of things NOT wrapped include (most of which are easily replace by scipy&numpy):

	* SpecialFunctions (e.g. the gamma function)
	* Stats, GaussianDistribution, etc.
	* Command Line utilities (python’s built-in argparse does the job better)
	* String utilities
	* Vector graphics utilities
	* Matrix class (trust us, you wouldn’t want to use a python wrap of that thing anyway!)


How some particular C++ structures are translated to Python:
------------------------------------------------------------

To wrap the library we used the Standard Wrapper Interface Generator (SWIG). Without SWIG, the amount of work 
to wrap the library would have made it completely infeasible. However, some elements of C++ do not have clear 
analogues in Python; as such, some compromises had to be made to allow for clean python programming without a 
complete rework of the well-established C++ API.

These are important to recognize because they present technical reasons why some things seem odd; the bindings 
were written in a way that had to be maintainable above all else. Any changes for the sake of the Python API 
could not duplicate code.

Namespaces:

	These don’t exist in python, so they are simply discarded. The GPSTk largely only uses the gpstk namespace, 
	which is roughly the gpstk package now. Exceptions include the submodules created for exceptions and constants.

	Note that SWIG is actually set to create a module called gpstk_pylib. To clean up the namespace and provide 
	more organization, there are __init__.py files that divide up the namespace (into gpstk, gpstk.constants, etc.) 
	and remove unwanted members. You can still access the raw wrapping through gpstk.gpstk_pylib, however. It is 
	strongly recommended you ignore the hidden components; they were hidden for a reason!

Enumerations:

	Many classes use enums extensively (see: ObsID), especially in their constructors to specify options. These 
	enums are simply wrapped to integer values. A enum value can be referred to by ClassName.ValueName. Note that 
	the name given to the whole enum is no long used.

Templates:

	Templates in the GPSTk were largely for 3 purposes:

		1. standard library containers (std::vector<T>, std::map<K,V>)

		2. accepting any numeric type (gpstk::Vector<T>)

		3. creating specialized class for use in inheritance 
			(such as how YumaAlmanacStore is a subclass of FileStore<YumaHeader>)

	Using the standard library containers of C++ is discouraged, so only a few were created for use.

	The only class that uses the numeric template type that is wrapped in gpstk::Vector, which only accepts floating 
	point numbers now.

	The templates in inheritance are not needed as they are not visible to the end-user.

Streams:

	Streams were used all over the GPSTk for input and output. Many classes had a dump or operator<< function that 
	could be used for string output, these have been replaced with the __str__ method.

	Streams were also used heavily for reading in Almanac and Ephemeris data from files. A more pythonic interface 
	was provided over these streams (the streams are now hidden) and is described at File I/O Functions.

Inner Classes:

	SWIG cannot wrap nested classes/structs (as of v2.0.7), so many of the inner classes and exceptions were simply 
	pulled into the gpstk namespace.

Standard Library Classes:

	Occasionally, you may need a std::vector or std::map for some purpose (i.e. a function takes them as a parameter). 
	They are included in a few common templated forms (string->char, etc.), but you should avoid the use of these whenever 
	possible.

	For the most part, a vector object has the same semantics as a list and a map object has the same semantics as a dict.

	When C++ functions deal with arrays, they are automatically converted (since the Python C API already uses arrays), 
	but when other containers are used, they must be explicitly converted. Conversion functions (such as seqToVector for 
	list->std::vector conversion) are defined in the cpp submodule.

	Warning: Reading documentation can get difficult in dealing with heavily-templated wrapped C++ code. Use these structures 
	as little as possible. If containers are small it might be wise to use the provided functions in gpstk.cpp to convert the 
	containers to native python lists/dicts.

Exceptions:

	Exceptions were tricky to get right. In general, most exceptions thrown by calling GPSTk routines should be caught in the 
	standard way in Python.

		| try:
		|     a = gpstk.someFunction()
		| except gpstk.exceptions.InvalidRequest:  # the most used exception in the GPSTk
		|     print 'Could not process data.'

	When an exception is thrown by the C++ code, it is propagated to the SWIG system which has code (written in Exception.i) 
	that either wraps the exception to one of the classes in gpstk.exceptions.

Arrays:

	SWIG typemaps have been written to automatically convert between C style arrays and Python lists. Any function that takes 
	a array parameter will accept a list and any array return will actually return a list.

	This conversion can be done efficiently since Python are already backed by C arrays.

Functions that modify a parameter that is passed by reference:

	Some C++ functions didn’t return a value, but just modify a non-const parameter. For example, from Position.hpp:

		| static void convertCartesianToGeocentric(const Triple& xyz, Triple& llr)
	
	Simple wrappers are added to the library that have a signature of:

		| static Triple convertCartesianToGeocentric(const Triple& xyz)

	Which behaves as expected when used in Python:

		>>> import gpstk
		>>> u = gpstk.Position(100, 200, 300)
		>>> v = gpstk.Position.convertCartesianToGeocentric(u)
		>>> print v
		(53.30077479951032, 63.434948822921655, 374.16573867739413)


New Functions:
--------------

Time Functions:

	These are largely helper functions that make some functionality a little easier or more succinct.

	gpstk.now([timeSystem=TimeSystem('UTC')])
	
		Returns the current time in the given TimeSystem. The default is in the UTC time system since the time is 
		derived from calling SystemTime().

		>>> print gpstk.now()
		2456490 72040524 0.000665000000000 UTC

	gpstk.times(starttime, endtime[, seconds=0.0, days=0])

		This returns a generator of CommonTime objects that starts at starttime and advances by the seconds and days 
		parameter each time. Special cases (no timestep parameters, negative parameters) are documented in the function’s 
		docstring and the quick reference section.

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

Position Functions:

	Some helpful functions for creating Position objects more easily (with keyword arguments) were also added:

	gpstk.cartesian([x=0.0, y=0.0, z=0.0, model=None, frame=ReferenceFrame('Unknown)'])

		>>> print gpstk.spherical(radius=5, theta=45, phi=45)
		45.00000000 deg 45.00000000 deg 5.0000 m

	gpstk.spherical([theta=0.0, phi=0.0, radius=0.0, model=None, frame=ReferenceFrame('Unknown')])

		Returns a Position in the Spherical coordinate system.

	gpstk.geodetic([latitude=0.0, longitude=0.0, height=0.0, model=None, frame=ReferenceFrame('Unknown')])

		Returns a Position in the Geodetic coordinate system.

	gpstk.geocentric([latitude=0.0, longitude=0.0, radius=0.0, model=None, frame=ReferenceFrame('Unknown')])

		Returns a Position in the Geocentric coordinate system.

	The next four functions are simply light wrappers over some relatively simple classes. The use of a entire class 
	objects was warranted in C++, but they are not neccessary for typical python usage.

	gpstk.moonPosition(time)
		
		Returns the current position (A gpstk.Triple) of the moon. This is a functional wrapper on the moonPosition class.

		>>> print gpstk.moonPosition(gpstk.now())
		(62424169.398472935, -365987646.51255625, -83100797.60563189)

	gpstk.sunPosition(time)
	
		Returns the current position (A gpstk.Triple) of the moon. This is a functional wrapper on the MoonPosition class.

	gpstk.poleTides(time, position, x, y)

		Returns the effect (a gpstk.Triple) of pole tides (meters) on the given position, in the Up-East-North (UEN) 
		reference frame. This is a functional wrapper on the (hidden) PoleTides class.

	gpstk.solidTides(time, position)
		
		Returns the effect (a gpstk.Triple) of solid Earth tides (meters) at the given position and epoch, in the 
		Up-East-North (UEN) reference frame. This is a functional wrapper on the (hidden) SolidTides class.

		>>> print gpstk.solidTides(gpstk.now(), gpstk.geodetic(latitude=29, longitude=95, height=0))
		(0.1601640329929359, -0.0031534542100034955, -0.03016846270875466)

File I/O Functions:

	To replace the stream-oriented paradigm for file i/o used in the C++ implementation, some thin wrapping was placed 
	over the Stream objects.

	Because the i/o strategy was uniform across all file types in C++, the strategy is similarly uniform for the python bindings.

	As an example, to read a SP3 file:

		>>> header, data = gpstk.readSP3('sp3_data.txt')
	
	And to write an SP3 file...

		>>> gpstk.writeSP3('sp3_data.txt.new', header, data)

	In this case, header is a SP3Header. Data is a list of SP3Data objects.

	By default, the data returned is a generator expression - which means it closely mimics the C++ stream in that it only 
	returns an object when it is needed. If you want a strictly evaluated list (all of the objects sit in memory) you can 
	use the strict=True keyword argument or just translate the generator to a list yourself using the list() initializer with the 
	generator as the argument.

	A common paradigm is to loop over all elements in the data and process them. As an example, to print all the data sets in a 
	RINEX 3 Nav file:

		| header, data = gpstk.readRinex3Nav('rinex3nav_data.txt')
		| for d in data:
    	|   print d

	You can also add a filter to the read on the data objects using the filterfunction keyword argument. By default, it simply 
	returns True, which includes all data objects in the output.

	For example, to get a generator of Rinex3NavData objects with only the PRNID of 3, you could use:

		>>> isPRN3 = (lambda x: x.PRNID == 3)

		>>> header, data = gpstk.readRinex3Nav('rinex3nav_data.txt', filterfunction=isPRN3)

		>>> print data.next()
		Sat: G03 TOE: 1274 367200.000 TOC: 1274 367200.000 codeflags:   1 L2Pflag:   0 IODC:  902 IODE:  134 HOWtime: 362376 FitInt:  4.000

		>>> print data.next()
		Sat: G03 TOE: 1274 374400.000 TOC: 1274 374400.000 codeflags:   1 L2Pflag:   0 IODC:  903 IODE:  135 HOWtime: 367206 FitInt:  4.000

		>>> print data.next()
		Sat: G03 TOE: 1274 381600.000 TOC: 1274 381600.000 codeflags:   1 L2Pflag:   0 IODC:  904 IODE:  136 HOWtime: 374406 FitInt:  4.000

	The following formats use this pattern (all read/write semantics are identical):

		* FIC
		* MSC
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