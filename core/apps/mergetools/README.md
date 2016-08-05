mergetools - mergeRinObs, mergeRinNav, mergeRinMet
==================================================

These applications merge multiple RINEX observation, navigation, or meteorological data files
into a single coherent RINEX obs/nav/met file, respectively.

Usage:
------

### Required Arguments

Short Arg.| Long Arg.| Description

    -i    –input=ARG   An input RINEX observation file, can be repeated as many times as needed.
    -o    –output=ARG  Name for the merged output RINEX observation file. Any existing file with that
                         name will be overwritten.

### Required Arguments

Short Arg.| Long Arg.| Description

    -d    -debug       Increase debug level.
    -v    –verbose     Increase verbosity.
    -h    -help        Print help usage.

*mergeRinNav* and *mergeRinMet* have the same usage.

Examples:
---------

    > mergeRinObs -i arl280.06o -i arl2810.06o -o arl280-10.06o
    > mergeRinNav -i arl280.06n -i arl2810.06n -o arl280-10.06n
    > mergeRinMet -i arl280.06m -i arl2810.06m -o arl280-10.06m