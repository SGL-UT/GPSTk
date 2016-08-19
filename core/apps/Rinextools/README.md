Rinextools - RinDump
====================

The application reads a RINEX file and dumps the obervation types in columns. Output is to
the screen, with one time tag and one satellite per line.

Usage:
------

### Optional Arguments

Short Arg.| Long Arg.| Description

          –pos            Output only positions from aux headers; sat and obs are ignored.
    -n    –num            Make output purely numeric (no header, no system char on sats).
          –format <file>  Output times in CommonTime format (Default: %4F %10.3g).
          –file <file>    RINEX observation file; this option may be repeated.
          –obs <obs>      RINEX observation type, found in file header.
          –sat <sat>      RINEX satellite ID (e.g. G31 for GPS PRN 31).
    -h    –help           Print this and quit.

RinDump usage: *RinDump [-n] <rinex obs file> [<satellite(s)> <obstype(s)>]*

The optional argument -n tells RinDump its output should be purely numeric.

Examples:
---------

    > RinDump algo1580.06o 3 4 5
        # Rinexdump file: algo1580.06o Satellites: G03 G04 G05 Observations: ALL
        # Week GPS_sow Sat L1 L S L2 L S C1 L S
        1378 259200.000 G03 -3843024.647 0 3 -2994560.443 0 1 23796436.087 0 0
        1378 259230.000 G03 -3954052.735 0 3 -3081075.654 0 2 23775308.750 0 0
        1378 259260.000 G03 -4064994.465 0 2 -3167523.561 0 3 23754197.617 0 0
        . . .
        P2 L S P1 L S S1 L S S2 L S
        23796439.457 0 0 23796436.350 0 0 21.100 0 0 11.000 0 0
        23775311.168 0 0 23775308.182 0 0 22.100 0 0 17.800 0 0
        23754199.648 0 0 23754196.550 0 0 17.000 0 0 18.600 0 0
        . . .
        3.16.4 Notes
        MATLAB and Octave can read the purely numeric output.


-----------------------------------------------------------------------------------------------------------------------

Rinextools - RinEdit
====================

The application opens and reads RINEX observation files(s) (version 2.x or 3.x), applies editing commands,
and write out the modified RINEX data to RINEX 2.11 or 3.x file(s).

Usage:
------

```
RinEdit [options] [editing commands]
  Options:
# RINEX input and output files
 --IF <fn>         Input RINEX observation file name [repeat] ()
 --ID <p>          Path of input RINEX observation file(s) ()
 --OF <fn>         Output RINEX obs files [also see --OF <f,t> below] [repeat] ()
 --OD <p>          Path of output RINEX observation file(s) ()
# Other file I/O
 --file <fn>       Name of file containing more options [#->EOL = comment] [repeat] ()
 --log <fn>        Output log file name ()
 --ver2            Write out RINEX version 2 (don't)
# Help
 --verbose         Print extra output information (don't)
 --debug           Print debug output at level 0 [debug<n> for level n=1-7] (-1)
 --help            Print this syntax page, and quit (don't)
# ------ Editing commands ------
# RINEX header modifications (arguments with whitespace must be quoted)
 --HDp <p>         Set header 'PROGRAM' field to <p> ()
 --HDr <rb>        Set header 'RUN BY' field to <rb> ()
 --HDo <obs>       Set header 'OBSERVER' field to <obs> ()
 --HDa <a>         Set header 'AGENCY' field to <a> ()
 --HDx <x,y,z>     Set header 'POSITION' field to <x,y,z> (ECEF, m) ()
 --HDm <m>         Set header 'MARKER NAME' field to <m> ()
 --HDn <n>         Set header 'MARKER NUMBER' field to <n> ()
 --HDj <n>         Set header 'REC #' field to <n> ()
 --HDk <t>         Set header 'REC TYPE' field to <t> ()
 --HDl <v>         Set header 'REC VERS' field to <v> ()
 --HDs <n>         Set header 'ANT #' field to <n> ()
 --HDt <t>         Set header 'ANT TYPE' field to <t> ()
 --HDh <h,e,n>     Set header 'ANTENNA OFFSET' field to <h,e,n> (Ht,East,North) ()
 --HDc <c>         Add 'COMMENT' <c> to the output header [repeat] ()
 --HDdc            Delete all comments [not --HDc] from input header (don't)
 --HDda            Delete all auxiliary header data (don't)
# Time related [t,f are strings, time t conforms to format f; cf. gpstk::Epoch.]
# Default t(f) is 'week,sec-of-week'(%F,%g) OR 'y,m,d,h,m,s'(%Y,%m,%d,%H,%M,%S)
 --OF <f,t>        At RINEX time <t>, close output file and open another named <f> ()
 --TB <t[:f]>      Start time: Reject data before this time ([Beginning of dataset])
 --TE <t[:f]>      Stop  time: Reject data after this time ([End of dataset])
 --TT <dt>         Tolerance in comparing times, in seconds (0.00)
 --TN <dt>         If dt>0, decimate data to times = TB + N*dt [sec, w/in tol] (0.00)
# In the following <SV> is a RINEX satellite identifier, e.g. G17 R7 E22 R etc.
#              and <OT> is a 3- or 4-char RINEX observation code e.g. C1C GL2X S2N
# Delete cmds; for start(stop) cmds. stop(start) time defaults to end(begin) of data
#     and 'deleting' data for a single OT means it is set to zero - as RINEX requires.
 --DA <t>          Delete all data at a single time <t> [repeat] ()
 --DA+ <t>         Delete all data beginning at time <t> [repeat] ()
 --DA- <t>         Stop deleting at time <t> [repeat] ()
 --DO <OT>         Delete RINEX obs type <OT> entirely (incl. header) [repeat] ()
 --DS <SV>         Delete all data for satellite <SV> [SV may be char]
 --DS <SV,t>       Delete all data for satellite <SV> at single time <t> [repeat] ()
 --DS+ <SV,t>      Delete data for satellite <SV> beginning at time <t> [repeat] ()
 --DS- <SV,t>      Stop deleting data for sat <SV> beginning at time <t> [repeat] ()
 --DD <SV,OT,t>    Delete a single RINEX datum(SV,OT) at time <t> [repeat] ()
 --DD+ <SV,OT,t>   Delete all RINEX data(SV,OT) starting at time <t> [repeat] ()
 --DD- <SV,OT,t>   Stop deleting RINEX data(SV,OT) at time <t> [repeat] ()
 --SD <SV,OT,t,d>  Set data(SV,OT) to value <d> at single time <t> [repeat] ()
 --SS <SV,OT,t,s>  Set SSI(SV,OT) to value <s> at single time <t> [repeat] ()
 --SL <SV,OT,t,l>  Set LLI(SV,OT) to value <l> at single time <t> [repeat] ()
 --SL+ <SV,OT,t,l> Set all LLI(SV,OT) to value <l> starting at time <t> [repeat] ()
 --SL- <SV,OT,t,l> Stop setting LLI(SV,OT) to value <l> at time <t> [repeat] ()
# Bias cmds: (BD cmds apply only when data is non-zero, unless --BZ)
 --BZ              Apply BD command even when data is zero (i.e. 'missing') (don't)
 --BS <SV,OT,t,s>  Add the value <s> to SSI(SV,OT) at single time <t> [repeat] ()
 --BL <SV,OT,t,l>  Add the value <l> to LLI(SV,OT) at single time <t> [repeat] ()
 --BD <SV,OT,t,d>  Add the value <d> to data(SV,OT) at single time <t> [repeat] ()
 --BD+ <SV,OT,t,d> Add the value <d> to data(SV,OT) beginning at time <t> [repeat] ()
 --BD- <SV,OT,t,d> Stop adding the value <d> to data(SV,OT) at time <t> [repeat] ()
```
 
Examples:
---------

# Combine two files
`$ RinEdit --IF acor1480.08o --IF areq015o.10o --OF out.12o --verbose`

# Delete all galileo and GPS L5 observations from a v2.11 file and write a v2.11 file
`$ RinEdit --IF acor1480.08o --IF areq015o.10o --OF out.12o --ver2 -DS E --DO GL5X --DO GC5X --DO GD5X --DO GS5X`



-----------------------------------------------------------------------------------------------------------------------

Rinextools - RinSum
===================

The application reads a RINEX file and summarizes its content.

Usage:
------

### Optional Arguments

Short Arg.| Long Arg.| Description

    -i    –input    Input file name(s).
    -f              File containing more options.
    -o    –output   Output file name.
    -p    –path     Path for input file(s).
    -R    –Replace  Replace header with full one.
    -s    –sort     Sort the PRN/Obs table on begin time.
    -g    –gps      Print times in the PRN/Obs table as GPS times.
          –gaps     Print a table of gaps in the data, assuming specified interval dt.
          –start    Start time: <time> is ’GPSweek,sow’ OR ’YYYY,MM,DD,HH,Min,Sec’.
          –stop     Stop time: <time> is ’GPSweek,sow’ OR ’YYYY,MM,DD,HH,Min,Sec’.
    -b    –brief    Produce a brief (6-line) summary.
    -h    –help     Print syntax and quit.
    -d    –debug    Print debugging information.

Examples:
---------

    >RinSum obs/s051001a.04o
        # RinSum, part of the GPS Toolkit, Ver 3.3 1/31/12, Run 2012/07/17 11:12:32
        +++++++++++++ RinSum summary of Rinex obs file obs/s051001a.04o +++++++++++++
        ---------------------------------- REQUIRED ----------------------------------
        Rinex Version 2.10, File type Observation, System G (GPS).
        Prgm: GFW - ROW, Run: 12/31/2003 23:59:53, By: NIMA
        Marker name: 85405, Marker type: .
        Observer : Monitor Station, Agency: NIMA
        Rec#: 1, Type: ZY12, Vers:
        Antenna # : 85405, Type : AshTech Geodetic 3
        Position (XYZ,m) : (3633910.6680, 4425277.7563, 2799862.8708).
        Antenna Delta (HEN,m) : (0.0000, 0.0000, 0.0000).
        Wavelength factor L1: 1 L2: 1
        GPS Observation types (9):
        Type #01 (L1P) L1 GPSP phase
        Type #02 (L2P) L2 GPSP phase
        Type #03 (C1C) L1 GPSC/A pseudorange
        Type #04 (C1P) L1 GPSP pseudorange
        Type #05 (C2P) L2 GPSP pseudorange
        Type #06 (D1P) L1 GPSP doppler
        Type #07 (D2P) L2 GPSP doppler
        Type #08 (S1P) L1 GPSP snr
        Type #09 (S2P) L2 GPSP snr
        Time of first obs 2004/01/01 00:00:00.000 Unknown
        (This header is VALID)
        ---------------------------------- OPTIONAL ----------------------------------
        Marker number : 85405
        Signal Strenth Unit =
        Comments (1) :
        Data are thinned (not smoothed) 30s. observations
        -------------------------------- END OF HEADER --------------------------------
        Reading the observation data...
        Computed interval 30.00 seconds.
        Computed first epoch: 2004/01/01 00:00:00 = 1251 4 345600.000
        Computed last epoch: 2004/01/01 23:59:30 = 1251 4 431970.000
        Computed time span: 23h 59m 30s = 86370 seconds.
        Computed file size: 3785956 bytes.
        There were 2880 epochs (100.00% of 2880 possible epochs in this timespan) and 0 inline header blocks.
        Summary of data available in this file: (Spans are based on times and interval)
        System G = GPS:
        Sat\OT: L1P L2P C1C C1P C2P D1P D2P S1P S2P Span Begin time - End time
        G01 945 945 942 945 945 945 945 945 945 2880 2004/01/01 00:00:00 - 2004/01/01 23:59:30
        G02 911 906 889 911 906 911 906 911 906 2880 2004/01/01 00:00:00 - 2004/01/01 23:59:30
        G03 872 869 855 872 869 872 869 872 869 2433 2004/01/01 00:42:00 - 2004/01/01 20:58:00
        G04 914 908 884 914 908 914 908 914 908 2880 2004/01/01 00:00:00 - 2004/01/01 23:59:30
        G05 785 785 781 785 785 785 785 785 785 860 2004/01/01 07:19:30 - 2004/01/01 14:29:00
        G06 890 890 885 890 890 890 890 890 890 947 2004/01/01 09:31:00 - 2004/01/01 17:24:00
        G07 735 735 735 735 735 735 735 735 735 735 2004/01/01 03:51:00 - 2004/01/01 09:58:00
        G08 924 923 916 924 923 924 923 924 923 974 2004/01/01 00:17:00 - 2004/01/01 08:23:30
        G09 665 665 659 665 665 665 665 665 665 1310 2004/01/01 06:34:30 - 2004/01/01 17:29:00
        G10 947 943 937 947 943 947 943 947 943 1407 2004/01/01 02:44:00 - 2004/01/01 14:27:00
        G11 699 696 657 699 696 699 696 699 696 2880 2004/01/01 00:00:00 - 2004/01/01 23:59:30
        G13 890 888 875 890 888 890 888 890 888 2880 2004/01/01 00:00:00 - 2004/01/01 23:59:30
        G14 700 700 698 700 700 700 700 700 699 702 2004/01/01 16:14:00 - 2004/01/01 22:04:30
        G15 903 903 898 903 903 903 903 903 903 959 2004/01/01 13:08:00 - 2004/01/01 21:07:00
        G16 982 982 969 982 982 982 982 982 982 2880 2004/01/01 00:00:00 - 2004/01/01 23:59:30
        G17 738 738 737 738 738 738 738 738 738 766 2004/01/01 08:20:30 - 2004/01/01 14:43:00
        G18 773 772 765 773 772 773 772 773 772 873 2004/01/01 12:43:30 - 2004/01/01 19:59:30
        G20 789 789 771 789 789 789 789 789 789 2880 2004/01/01 00:00:00 - 2004/01/01 23:59:30
        G21 930 930 921 930 930 930 930 930 930 966 2004/01/01 11:02:00 - 2004/01/01 19:04:30
        G22 793 793 793 793 793 793 793 793 793 793 2004/01/01 15:15:30 - 2004/01/01 21:51:30
        G23 877 877 888 877 877 877 877 877 877 904 2004/01/01 14:52:00 - 2004/01/01 22:23:30
        G24 675 675 667 675 675 675 675 675 675 1394 2004/01/01 01:29:30 - 2004/01/01 13:06:00
        G25 652 651 647 652 651 652 651 652 651 1412 2004/01/01 12:07:30 - 2004/01/01 23:53:00
        G26 927 927 920 927 927 927 927 927 927 1411 2004/01/01 04:47:30 - 2004/01/01 16:32:30
        G27 954 954 951 954 954 954 954 954 954 2880 2004/01/01 00:00:00 - 2004/01/01 23:59:30
        G28 882 881 876 882 881 882 881 882 881 914 2004/01/01 01:41:30 - 2004/01/01 09:18:00
        G29 885 884 870 885 884 885 884 885 884 1419 2004/01/01 03:57:30 - 2004/01/01 15:46:30
        G30 701 701 700 701 701 701 701 701 701 749 2004/01/01 09:27:30 - 2004/01/01 15:41:30
        G31 973 970 962 973 970 973 970 973 970 2541 2004/01/01 00:32:00 - 2004/01/01 21:42:00
        TOTAL 24311 24280 24048 24311 24280 24311 24280 24311 24279
        
