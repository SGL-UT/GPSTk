difftools - rmwdiff, rnwdiff, rowdiff
====================================

These applications difference RINEX observation, navigation, and meteorological data files.

Usage:
------

### Optional Arguments

Short Arg.| Long Arg.| Description

    -d    –debug                Increase debug level.
    -v    –verbose              Increase verbosity.
    -h    –help                 Print help usage.
    -l    –quit-on-first-error  Quit on the first error encountered.
    -t    –time=TIME            Start of time range to compare (Default = BOT.)
    -e    –end-time=TIME        End of time range to compare (Default = EOT.)

*rmwdiff usage: rmwdiff [options] <RINEX Met file> <RINEX Met file>*

*rnwdiff usage: rnwdiff [options] <RINEX Nav file> <RINEX Nav file>*

*rowdiff usage: rowdiff [options] <RINEX Obs file> <RINEX Obs file>*


Examples:
---------

    > rowdiff obs/s121001a.01o obs/s121001a.02o
          Comparing the following fields (other header data is ignored):
          C1 D1 D2 L1 L2 P1 P2
          <Dump of RinexObsData - time: 01 1 1 0 0 0.0000000 epochFlag: 0 numSvs: 11 clk offset:
          0.000000
          Sat G01 C1: 21623650.706/0/8 D1: -1740.071/0/8 D2: -1355.897/0/8 L1: -17390026.255/0/8
          L2: -13535827.656/0/8 P1: 21623650.392/0/8 P2: 21623657.569/0/8
          Sat G03 C1: 20805015.215/0/8 D1: -1654.577/0/8 D2: -1289.282/0/8 L1: -22641755.914/0/8
          L2: -17618096.770/0/8 P1: 20805015.003/0/8 P2: 20805021.105/0/8
          Sat G11 C1: 24129742.024/0/7 D1: 3245.246/0/7 D2: 2528.744/0/7 L1: -4672870.369/0/7
          L2: -3626228.611/0/7 P1: 24129741.782/0/7 P2: 24129750.888/0/7
          Sat G13 C1: 22087276.186/0/8 D1: 7.400/0/8 D2: 5.765/0/8 L1: -16451815.112/0/8
          L2: -12553265.286/0/8 P1: 22087276.610/0/8 P2: 22087282.441/0/8
          Sat G15 C1: 23463116.796/0/7 D1: -497.311/0/8 D2: -387.518/0/8 L1: -9031186.781/0/8
          L2: -7031551.474/0/8 P1: 23463116.213/0/8 P2: 23463124.003/0/8
          Sat G19 C1: 21324621.372/0/8 D1: 2187.448/0/8 D2: 1704.503/0/8 L1: -18645307.237/0/8
          L2: -14518504.343/0/8 P1: 21324621.390/0/8 P2: 21324628.098/0/8
          Sat G22 C1: 22350863.766/0/7 D1: -1204.472/0/8 D2: -938.550/0/8 L1: -12632952.524/0/8
          L2: -9804132.252/0/8 P1: 22350863.282/0/8 P2: 22350870.038/0/8
          Sat G25 C1: 24578217.445/0/7 D1: -3164.811/0/7 D2: -2466.069/0/7 L1: -3829204.504/0/7
          L2: -2958619.116/0/7 P1: 24578217.563/0/7 P2: 24578226.318/0/7
          Sat G27 C1: 23262592.158/0/7 D1: 2951.056/0/8 D2: 2299.519/0/8 L1: -9166691.680/0/8
          L2: -7120447.504/0/8 P1: 23262592.029/0/8 P2: 23262598.552/0/8
          Sat G28 C1: 21283503.220/0/8 D1: -585.103/0/8 D2: -455.924/0/8 L1: -17698942.286/0/8
          L2: -13775959.458/0/8 P1: 21283503.017/0/8 P2: 21283507.983/0/8
          Sat G31 C1: 20803601.031/0/8 D1: 878.855/0/8 D2: 684.823/0/8 L1: -22576510.085/0/8
          L2: -17577293.102/0/8 P1: 20803600.689/0/8 P2: 20803606.968/0/8
          ...

    > rnwdiff nav/s121001a.01n nav/s121001a.02n
          <PRN: 1 TOE: 2451911 07200000 0.000000000000000 Unknown TOC: 1095 93600.000 IODE: 136
          HOWtime: 86406
          <PRN: 3 TOE: 2451911 07200000 0.000000000000000 Unknown TOC: 1095 93600.000 IODE: 186
          HOWtime: 86406
          <PRN: 11 TOE: 2451911 07200000 0.000000000000000 Unknown TOC: 1095 93600.000 IODE: 18
          HOWtime: 86406
          <PRN: 13 TOE: 2451911 07200000 0.000000000000000 Unknown TOC: 1095 93600.000 IODE: 28
          HOWtime: 86406
          <PRN: 15 TOE: 2451911 07200000 0.000000000000000 Unknown TOC: 1095 93600.000 IODE: 226
          HOWtime: 86406
          <PRN: 19 TOE: 2451911 07200000 0.000000000000000 Unknown TOC: 1095 93600.000 IODE: 195
          HOWtime: 86406
          <PRN: 22 TOE: 2451911 07200000 0.000000000000000 Unknown TOC: 1095 93600.000 IODE: 135
          HOWtime: 86406
          <PRN: 25 TOE: 2451911 07200000 0.000000000000000 Unknown TOC: 1095 93600.000 IODE: 29
          HOWtime: 86406
          <PRN: 27 TOE: 2451911 07200000 0.000000000000000 Unknown TOC: 1095 93600.000 IODE: 66
          HOWtime: 86406
          <PRN: 28 TOE: 2451911 07200000 0.000000000000000 Unknown TOC: 1095 93600.000 IODE: 28
          HOWtime: 86406
          <PRN: 31 TOE: 2451911 07200000 0.000000000000000 Unknown TOC: 1095 93600.000 IODE: 91
          HOWtime: 86406
          <PRN: 8 TOE: 2451911 07200000 0.000000000000000 Unknown TOC: 1095 93600.000 IODE: 149
          HOWtime: 88716
          ...

    > rmwdiff met/412_001a.00m met/412_001a.01m
          Comparing the following fields (other header data is ignored):
          PR TD HR 
          < 2451545 00000000 0.000000000000000 Any
          PR 860.3
          TD 17.2
          HR 95.5
          < 2451545 00900000 0.000000000000000 Any
          PR 860.1
          TD 17.2
          HR 95.8
          < 2451545 01800000 0.000000000000000 Any
          PR 859.9
          TD 17.2
          HR 96
          < 2451545 02700000 0.000000000000000 Any
          PR 859.6
          TD 17.1
          HR 96.2

Notes:
------

Only the first error in each file is reported. The entire file is always checked regardless of time
options.