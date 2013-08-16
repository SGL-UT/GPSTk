Introduction
==================

This documentation describes the architecture and design of the Python bindings
of the GPS ToolKit (GPSTk), produced by the Space and Geophysics Laboratory (SGL)
of Applied Research Laboratories, The University of Texas at Austin (ARL:UT).



About the GPSTk
*****************
The GPSTK is a library of support routines designed for use by programmers
developing applications that read and write (particularly in RINEX format),
process and otherwise manipulate GPS data. It is characterized by the following:

- Modern, ANSI C++; highly object oriented
- Uses templates, the Standard Template Library (STL) and iostreams classes
- Portable and platform independent
- Includes full implementation of the RINEX (Receiver Independent Exchange Format) I/O.


As the GPSTk is primarily a C++ library, it's API reference should still be used:
http://www.gpstk.org/doxygen/




Getting the Python GPSTk
****************************
Building the GPSTk from source can be a fairly involved process, see :ref:`build_label`
for the details.




About the Python Bindings
********************************************
Though effort has been taken to reduce the C++-feeling when using these Python
classes, it is impossible to completely purify the library without creating severe
maintenance problems. Thus, the user must be aware that the GPSTk is primarily a
C++ library and some things may require unusual semantics. It is recommended that
you avoid using some of these unusual classes, such as gpstk.cpp.vector_double (std::vector<double>)
as much as a possible. A much better replacement would be a numpy array.

Example of how C++ GPSTk reads and prints a Rinex3Obs file:

.. parsed-literal::
    // Create the input file stream
   Rinex3ObsStream rin("rinex3obs_data.txt");

   // Read the RINEX header
   Rinex3ObsHeader head;    // RINEX header object
   rin >> head;
   std::cout << head << std::endl;

   // Loop over all data epochs
   Rinex3ObsData data;   // RINEX data object
   while (rin >> data)
   {
      std::cout << data << std::endl;
   }

Compare to the simpler Python GPSTk read process:

.. parsed-literal::
   # read in the header and data
   header, data = readRinex3Obs('rinex3obs_data.txt')
   print header
   for dataObject in data:
       print data



A Note on Python Versions
*****************************************
These bindings were created with Python 2.7 in mind and that is how they are supported
at the moment. It is likely Python 3.2 or 3.3 will be supported some time in the future.

The standard Python implementation, CPython, must be used. The wrapping makes use
of the C Python API heavily.




Package Organization
************************
Most classes and functions are in the gpstk namespace, but there are some exceptions.

1. Constant values not inside a class are placed in the gpstk.constants submodule.
2. GPSTk Exceptions are placed in the gpstk.exceptions submodule.
3. Wrappers of C++ library structures (std::vector and std::map,
   which should only be used when necessary) are placed in the gpstk.cpp submodule.



How to Use This Documentation
**************************************
The C++ API is the primary reference still, however much of it is embedded into
the python source in the form of docstrings. This means you can use ipython
to easily find out about classes and functions.

Changes and additions to the C++ core are noted in this documentation,
as well as in the docstrings themselves.

A good first step is to browse the things available in the :ref:`quickref_label`
section. If you are coming from writing C++ GPSTk programs, you should be sure to read
the section on how C++ structures have been changed for python.



Scope
**********
The GPSTk C++ library is the product of a large number of people over a
sparse time period; as a result, the focus of the library has been somewhat
lost. There are many non-GPS related utilities implemented that have been
implemented more efficiently, robustly, and accurately by others.
These bindings attempt to expose as little as needed to the programmer.

Prime examples of things **NOT wrapped** include (most of which are easily replace by scipy&numpy):

* SpecialFunctions (e.g. the gamma function)
* Stats, GaussianDistribution, etc.
* Command Line utilities (python's built-in argparse does the job better)
* String utilities
* Vector graphics utilities
* Matrix class (trust us, you wouldn't want to use a python wrap of that thing anyway!)



How some particular C++ structures are translated to Python
***************************************************************

To wrap the library we used the Standard Wrapper Interface Generator (SWIG).
Without SWIG, the amount of work to wrap the library would have made it completely infeasible.
However, some elements of C++ do not have clear analogues in Python; as such, some
compromises had to be made to allow for clean python programming without a complete
rework of the well-established C++ API.


**Namespaces:**

These don't exist in python, so they are simply discarded.
The GPSTk largely only uses the gpstk namespace, which is roughly the gpstk package now.
Exceptions include the submodules created for exceptions and constants.

Note that SWIG is actually set to create a module called gpstk_pylib. To clean up the namespace
and provide more organization, there are __init__.py files that divide up the
namespace (into gpstk, gpstk.constants, etc.) and remove unwanted members.
You can still access the raw wrapping through gpstk.gpstk_pylib, however. It is
strongly recommended you ignore the hidden components; they were hidden for a reason!


**Enums:**

Many classes use enums extensively (see: ObsID), especially in their constructors
to specify options. These enums are simply wrapped to integer values. A enum
value can be referred to by ClassName.ValueName. Note that the name given to the
whole enum is no long used.

**Templates:**

Templates in the GPSTk were largely for 3 purposes:

* standard library containers (std::vector<T>, std::map<K,V>)
* accepting any numeric type (gpstk::Vector<T>)
* creating specialized class for use in inheritance
    (such as how YumaAlmanacStore is a subclass of FileStore<YumaHeader>)

Using the standard library containers of C++ is discouraged,
so only a few were created for use.

The only class that uses the numeric template type that is wrapped
in gpstk::Vector, which only accepts floating point numbers now.

The templates in inheritance are not needed as they are not visible to the end-user.



**Streams:**

Streams were used all over the GPSTk for input and output.
Many classes had a dump or operator<< function that could be used for
string output, these have been replaced with the __str__ method.

Streams were also used heavily for reading in Almanac and Ephemeris data from files.
A more pythonic interface was provided over these streams (the streams are now hidden)
and is described at :ref:`fileio_label`.


**Inner Classes:**

SWIG cannot wrap nested classes/structs (as of v2.0.7), so many of the
inner classes and exceptions were simply pulled into the gpstk namespace.


**Standard Library Classes:**

Occasionally, you may need a std::vector or std::map
for some purpose (i.e. a function takes them as a parameter). They are included
in a few common templated forms (string->char, etc.), but you should avoid
the use of these whenever possible.

For the most part, a vector object has the same semantics as a list and a
map object has the same semantics as a dict.


When C++ functions deal with arrays, they are automatically converted
(since the Python C API already uses arrays),
but when other containers are used, they must be explicitly converted.
Conversion functions (such as seqToVector for list->std::vector conversion)
are defined in the cpp submodule.


.. warning::
    Reading documentation can get difficult in dealing with heavily-templated
    wrapped C++ code. Use these structures as little as possible.
    If containers are small it might be wise to use the provided functions in
    gpstk.cpp to convert the containers to native python lists/dicts.




**Exceptions:**

Exceptions were tricky to get right. In general, most exceptions thrown by
calling GPSTk routines should be caught in the standard way in Python. ::

    try:
        a = gpstk.someFunction()
    except gpstk.exceptions.InvalidRequest:  # the most used exception in the GPSTk
        print 'Could not process data.'

When an exception is thrown by the C++ code, it is propogated to the SWIG
system which has code (written in Exception.i) that either wraps the exception
to one of the classes in gpstk.exceptions.


**Arrays:**

SWIG typemaps have been written to automatically convert between C style arrays
and Python lists. Any function that takes a array parameter will accept a list
and any array return will actually return a list.

This conversion can be done efficently since Python are already backed by C arrays.


**Functions that modify a parameter that is passed by reference:**

Some C++ functions didn't return a value, but just modify a non-const parameter.
For example, from Position.hpp:

.. parsed-literal::
    static void convertCartesianToGeocentric(const Triple& xyz, Triple& llr)

Simple wrappers are added to the library that have a signature of:

.. parsed-literal::
    static Triple convertCartesianToGeocentric(const Triple& xyz)

Which behaves as expected when used in Python: ::

    >>> import gpstk
    >>> u = gpstk.Position(100, 200, 300)
    >>> v = gpstk.Position.convertCartesianToGeocentric(u)
    >>> print v
    (53.30077479951032, 63.434948822921655, 374.16573867739413)

