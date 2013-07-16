Introduction
==================

This documentaiton describes the architecture and design of the Python version
of the GPS ToolKit (GPSTk), produced by the Space and Geophysics Laboratory (SGL)
of Applied Research Laboratories, The University of Texas at Austin (ARL:UT).



About
**********
GPSTK is a library of support routines designed for use by programmers
developing applications that read and write (particularly in RINEX format),
process and otherwise manipulate GPS data. It is characterized by the following:
- Modern, ANSI C++; highly object oriented
- Uses templates, the Standard Template Library (STL) and iostreams classes
- Portable and platform independent
- Includes full implementation of the RINEX (Receiver Independent Exchange Format) I/O.


As the GPSTk is primarily a C++ library, it's API reference should still be used:
http://www.gpstk.org/doxygen/

Part of the goal is to turn this C++ code to the much simpler python code:

.. parsed-literal::
    // Create the input file stream
   Rinex3ObsStream rin("bahr1620.04o");

   // Read the RINEX header
   Rinex3ObsHeader head;    // RINEX header object
   rin >> head;

   // Loop over all data epochs
   Rinex3ObsData data;   // RINEX data object
   while (rin >> data)
   {
      // process data
   }

.. parsed-literal::
   # read in the data
   header, data = readRinex3Obs('bahr1620.04o')
   for dataObject in data:
       # process data



Bindings
**********
Though effort has been taken to reduce the C++-feeling when using these Python
classes, it is impossible to completely purify the library. Thus, the user
must be aware that the GPSTk is primarily a C++ library and some things
may require unusual semantics. It is recommended that you avoid using
some of these unusual classes, such as gpstk.cpp.vector_double (std::vector<double>)
as much as a possible. A much better replacement would be a numpy array.



Package Organization
************************
Most classes and functions are in the gpstk namespace, but there are some exceptions.

1. Constant values not inside a class are placed in the gpstk.constants submodule.
2. GPSTk Exceptions are placed in the gpstk.exceptions submodule.
3. Wrappers of C++ library structures (std::vector and std::map,
   which should only be used when necessary) are placed in the gpstk.cpp submodule.



How to Use Documentation
**************************
The C++ API is the primary reference still. Most of it is embedded into
the python source as docstrings however. This means you can use ipython
to easily find out about classes and functions.

Changes and additions to the C++ core are noted in this documentation,
as well as in the docstrings themselves.



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
* Matrix class (trust me, you wouldn't want to use a python wrap of that thing anyway!)



How some particular C++ structures are translated to Python
***************************************************************

To wrap the library we used the Standard Wrapper Interface Generator (SWIG).
Without SWIG, the amount of work to wrap the library would have made it completely infeasible.
However, some elements of C++ do not have clear analouges in Python; as such, some
compromises had to be made to allow for clean python programming without a complete
rework of the well-established C++ API.


**Namespaces:**

These don't exist in python, so they are simply discarded.
The GPSTk largely only uses the gpstk namespace, which is roughly the gpstk package now.
Exceptions include the submodules created for exceptions and constants.


**Enums:**

Many classes use enums extensively (see: ObsID), especially in their constructors
to specify options. These enums are simply wrapped to integer values. A enum
value can be referred to by ClassName.ValueName. Note that the name given to the
whole enum is no long used.

**Templates:**

Templates in the GPSTk were largely for 3 purposes:

* standard library containers (std::vector<T>, std::map<K,V>)
* accepting any numeric type (gpstk::Vector<T>)
* creating specialized class for use in inheiritance
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


**Inner Classes:**

SWIG cannot wrap nested classes/structs, so many of the
inner classes and exceptions were simply pulled into the gpstk namespace.


**Standard Library Classes:**

Occasionally, you may need a std::vector or std::map
for some purpose (i.e. a function takes them as a parameter). They are included
in a few common templated forms (string->char, etc.), but you should avoid
the use of these whenever possible.



**Functions that modify a parameter that is passed by reference**

Some C++ functions didn't return a value, but just modify a non-const parameter. For example, from Position.hpp:

.. parsed-literal::
    static void convertCartesianToGeocentric(const Triple& xyz, Triple& llr)

Simple wrappers are added to the library that have a signature of:

Part of the goal is to turn this C++ code to the much simpler python code:

.. parsed-literal::
    static Triple convertCartesianToGeocentric(const Triple& xyz)

Which behaves as expected when used in Python.
