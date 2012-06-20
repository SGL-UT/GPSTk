<TeXmacs|1.0.5>

<style|article>

<\body>
  <doc-data|<doc-title|Using RINEXPVT>|<doc-author-data|<author-name|R.
  Benjamin Harris>|<\author-address>
    Applied Research Laboratories,

    The University of Texas at Austin
  </author-address>|<author-email|pben@arlut.utexas.edu>>|<doc-date|February
  24, 2005>>

  <section|Overview>

  The <with|font-family|tt|rinexpvt> application is a GPSTk [1] based
  application that generates user positions from pseudoranges recorded in the
  RINEX [2] format. One user position, or PVT, is generated per epoch of
  observation. No smoothing is applied to the pseudoranges, nor are the
  solutions filtered. A number of error models are applied to the
  pseudoranges before the generating the position calcuation, such as
  atmospheric delay. The user can select an elevation mask for satellites.
  Also only healthy satellites, as defined by the ephemeris, are used in the
  solution. The user can control which pseudoranges are used, and some of the
  corrections applied to them.\ 

  <section|Synopsis>

  The user executes <with|font-family|tt|rinexpvt> from the command line. The
  processing performed by the <with|font-family|tt|rinexpvt> is specified
  through command line arguments. The full set of arguments is defined below.
  This list can be duplicated by running <with|font-family|tt|rinexpvt -h> on
  the command line.\ 

  <with|prog-language|shell|prog-session|default|<\session>
    <\input|shell] >
      ../rinexpvt -h
    </input>

    <\output>
      Usage: rinexpvt [OPTION] ...

      GPSTk PVT Generator

      \;

      This application generates user positions based on RINEX

      observations.

      \;

      NOTE: Although the -n and -p arguments appear as optional below,

      one of the two must be used. An ephemeris source must be specified.

      \;

      Required arguments:

      \ \ -o, --obs-file=ARG \ \ \ \ \ \ RINEX Obs File.

      \;

      Optional arguments:

      \ \ -d, --debug \ \ \ \ \ \ \ \ \ \ \ \ \ Increase debug level

      \ \ -v, --verbose \ \ \ \ \ \ \ \ \ \ \ Increase verbosity

      \ \ -h, --help \ \ \ \ \ \ \ \ \ \ \ \ \ \ Print help usage

      \ \ -n, --nav-file=ARG \ \ \ \ \ \ RINEX Nav File. Required for single
      frequency

      \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ ionosphere
      correction.

      \ \ -p, --pe-file=ARG \ \ \ \ \ \ \ SP3 Precise Ephemeris File. Repeat
      this for each

      \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ input file.

      \ \ -m, --met-file=ARG \ \ \ \ \ \ RINEX Met File.

      \ \ -t, --time-format=ARG \ \ \ Alternate time format string.

      \ \ -e, --enu=ARG \ \ \ \ \ \ \ \ \ \ \ Use the following as origin to
      solve for

      \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ East/North/Up
      coordinates, formatted as a string:

      \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ "X Y Z"

      \ \ -l, --elevation-mask=ARG Elevation mask (degrees)

      \ \ -s, --single-frequency \ \ Use only C1 (SPS)

      \ \ -d, --dual-frequency \ \ \ \ Use only P1 and P2 (PPS)

      \ \ -i, --ionosphere \ \ \ \ \ \ \ \ Do NOT correct for ionosphere
      delay.
    </output>

    \;
  </session>>

  <section|Detailed Description>

  <subsection|Observation Model>

  The user position is related to the pseudorange observation through the
  following formula [3, 4].

  \;

  <\equation>
    <with|mode|text|<with|mode|math|\<rho\>=<sqrt|(x<rsub|u>-x<rsub|s>)<rsup|2>+(y<rsub|u>-y<rsub|s>)<rsup|2>+(z<rsub|u>-z<rsub|s><rsup|<rsup|>>)<rsup|2>>><with|mode|math|><with|mode|math|+c\<delta\>t+t+i+\<nu\>+m+\<epsilon\><rsub|>>>
  </equation>

  where <tabular|<tformat|<table|<row|<cell|<with|mode|math|\<rho\>> is the
  pseudorange measurement>>|<row|<cell|<with|mode|math|x,
  ><with|mode|math|y>, and <with|mode|math|z> represent Cartesian
  coordinates>>|<row|<cell|<with|mode|math|u> is the user position at time of
  reception>>|<row|<cell|<with|mode|math|s> is the satellite position at the
  time of transmission>>|<row|<cell|<with|mode|math|c\<delta\>t> is the clock
  offset between the user and spacecraft clocks>>|<row|<cell|<with|mode|math|t>
  is the delay due to the troposphere>>|<row|<cell|<with|mode|math|i >is
  delay due to the ionosphere>>|<row|<cell|<with|mode|math|\<nu\>> is
  relativistic delay>>|<row|<cell|<with|mode|math|m> is multipath
  delay>>|<row|<cell|<with|mode|math|\<epsilon\>> is thermal measurement
  noise>>>>>

  <paragraph|>For each satellite in view at a given epoch, one independent
  relation can be formed. When the satellite position is known as a function
  of time, and atmospheric delays have been estimated, then unknown terms are
  user position, at <with|mode|math|x<rsub|s>, ><with|mode|math|y<rsub|s>>,
  and <with|mode|math|z<rsub|s>,>and the clock offset,
  <with|mode|math|c\<delta\>t>. If more than four such observations are
  available for a given epoch, then gradient search methods are be used in
  combination with least squares to solve for the user position [3, 4, 5].

  <subsection|Satellite Position Models>\ 

  Satellite positions are computed as a function of time by one of two
  methods. The first method applies modified Keplerian parameters found in
  the broadcast ephemeris, as defined by the ICD-GPS-200 [3, 4, 6]. The
  second method is by Lagrange interpolation of precise ephemerides [3].

  <subsection|Delay Models>

  Many of the delays found in Eq. 1 are modeled within
  <with|font-family|tt|rinexpvt>. The delay due to special relativity can be
  computed directly from satellite position and velocity. The troposphere
  delay can be estimated using meteorological observations. Finally, the
  ionosphere delay can be computed using additional range observation or
  using a reference model.\ 

  The net effect due to special relativistic delay is frequently modeled
  within receivers using the following equation [4, 6]

  <\equation>
    \<delta\>t<rprime|'> = 2<frac|<wide|r|\<vect\>>\<bullet\><wide|v|\<vect\>>|c<rsup|2>>
  </equation>

  where <with|mode|math|<wide|r|\<vect\>>> is the Earth centered, Earth fixed
  (ECEF) position vector, <with|mode|math|<wide|v|\<vect\>>> is the ECEF
  velocity vector and <with|mode|math|c> is the speed of light.\ 

  If observations from more than one frequency are available for an epoch,
  then the ionosphere delay <with|mode|math|i> is estimated using the
  following linear relationship [5].\ 

  <\equation>
    i\<approx\> <frac|f<rsub|2><rsup|2>|f<rsub|2><rsup|2>-f<rsub|1><rsup|2>>(P<rsub|1>-P<rsub|2>)
  </equation>

  where <with|mode|math|P<rsub|1> >is the pseudorange measurement on L1 and
  <with|mode|math|P<rsub|2>> is the pseudorange measurement on L2. If dual
  frequency measurements are not available, then ionosphere delay is
  estimated using the Klobuchar model [2].\ 

  The troposphere delay is estimated using meteorological observations. The
  modified Hopfield model is used within <with|font-family|tt|rinexpvt> to
  model this form of error.[7] If no actual weather measurements are
  provided, then a default weather condition is assumed: 20 degrees ceslius,
  1000 millibars and 50 percent humidity.

  <section|Examples and Usage Notes>

  This section contains a number of practical examples in the use of
  <with|font-family|tt|rinexpvt>. In each subsection there is a brief
  description of the desired processing, as well as a snapshot of a shell
  session as a demonstration. All of the example files used are distributed
  with the application.

  <subsection|Generating Positions in WGS 84 Coordinates>

  By default, <with|font-family|tt|rinexpvt> generates the user position in
  the Cartesian, WGS reference frame. The user needs only supply observations
  and a source of ephemerides. If a RINEX meteorological file is provided,
  then troposphere delays are modeled. Otherwise the troposphere delay is
  modeled using a standard temperature of 20 celsius, . The following example
  demonstrates the most basic processing provided by
  <with|font-family|tt|rinexpvt>.

  <with|prog-language|shell|prog-session|default|<\session>
    <\input|shell] >
      rinexpvt -o usno0200.05o -n brdc0200.05n -m usno0200.05m
    </input>

    <\output>
      2005 1 20 00 00 0.000000 1112192.67926 -4842951.98205 3985348.06329

      2005 1 20 00 00 30.000000 1112188.65938 -4842953.48346 3985351.48398

      2005 1 20 00 01 0.000000 1112189.48576 -4842957.45711 3985356.92698

      2005 1 20 00 01 30.000000 1112191.15384 -4842957.53284 3985355.11895

      2005 1 20 00 02 0.000000 1112191.1508 -4842955.89459 3985352.76549

      2005 1 20 00 02 30.000000 1112190.99828 -4842954.61737 3985352.3681

      2005 1 20 00 03 0.000000 1112189.92412 -4842954.29518 3985348.72842

      2005 1 20 00 03 30.000000 1112189.16937 -4842954.31307 3985351.92146

      2005 1 20 00 04 0.000000 1112191.62124 -4842955.6613 3985354.82972

      2005 1 20 00 04 30.000000 1112188.66733 -4842953.49038 3985348.04322
    </output>

    \;
  </session>>

  <subsection|Generating Positions in East/North/Up Coordinates>

  The user may wish to transform the results of the position calculation to
  local, or topocentric, reference frame. The new coordinates, still
  Cartesian, refer to the cardinal directions: East, North and Up. The
  positions calculated by <with|font-family|tt|rinexpvt> can be tranformed to
  a topocentric origin using the <with|font-family|tt|-e> option. The
  argument to this option is a single string, with three numerical entries
  for the origin of the topocentric system. Often, within RINEX observation
  files, the header entry ``APPROX POS XYZ'' is a recent surveyed origin of
  the receiver and forms a useful origin for a topocentric system. In the
  following example the topocentric transformation is applied to the results
  from the previous subsection.

  <with|prog-language|shell|prog-session|default|<\session>
    <\input|shell]>
      grep "APPROX " usno0200.05o
    </input>

    <\output>
      \ \ 1112189.9031 -4842955.0319 \ 3985352.2376
      \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ APPROX POSITION XYZ
    </output>

    <\input|shell]>
      rinexpvt -o usno0200.05o -n brdc0200.05n -m usno0200.05m -e
      "1112189.9031 -4842955.0319 \ 3985352.2376"\ 
    </input>

    <\output>
      2005 1 20 00 00 0.000000 -2.64323278089 1.39273684601 -4.44579107837

      2005 1 20 00 00 30.000000 0.675237880363 -0.413842977087 -1.86595014292

      2005 1 20 00 01 0.000000 0.74075583724 -1.74562915823 4.70500528956

      2005 1 20 00 01 30.000000 -0.514261859844 -0.427112579929 3.92260789052

      2005 1 20 00 02 0.000000 -0.798000760765 0.225433490228 1.2040383517

      2005 1 20 00 02 30.000000 -0.905046895282 -0.156961158353
      -0.0422947010004

      2005 1 20 00 03 0.000000 -0.144614169541 1.78733342562 -2.7520269223

      2005 1 20 00 03 30.000000 0.432346745093 -0.229710319547
      -0.872438081029

      2005 1 20 00 04 0.000000 -1.19640547889 -1.09032516564 2.40032451942

      2005 1 20 00 04 30.000000 0.670398976551 1.68417470062 -4.01206558988
    </output>

    \;
  </session>>

  <subsection|Generating Positions using Precise Ephemerides>

  Precise ephemerides may be substituted for broadcast ephemerides. The
  precise ephemerides must be in the SP-3 file format. In order to process a
  given period of RINEX observations, precise ephemerides must be utilized
  for times before and after that period in order to eliminate interpolation
  effects. For this reason the <with|font-family|tt|-p> option to specify a
  precise ephemeris can be repeated. The following example demonstrates the
  use of precise ephemerides.

  <with|prog-language|shell|prog-session|default|<\session>
    <\input|shell]>
      rinexpvt -o usno0200.05o -m usno0200.05m -e "1112189.9031 -4842955.0319
      \ 3985352.2376" -p nga13063.apc -p nga13064.apc -p nga13065.apc\ 
    </input>

    <\output>
      2005 1 20 00 00 0.000000 -2.08992486501 1.51632100425 -5.38603364386

      2005 1 20 00 00 30.000000 1.22849620836 -0.281974793322 -2.78857829356

      2005 1 20 00 01 0.000000 1.29400421219 -1.60589389988 3.7997452854

      2005 1 20 00 01 30.000000 0.03901809145 -0.279940424961 3.0344477269

      2005 1 20 00 02 0.000000 -0.141549865351 0.430550617004 0.613916181074

      2005 1 20 00 02 30.000000 -0.247842106971 0.0554617639326
      -0.61408409222

      2005 1 20 00 03 0.000000 0.513345520633 2.00662083505 -3.30594517769

      2005 1 20 00 03 30.000000 1.18397434384 0.173306575113 -1.80221181762

      2005 1 20 00 04 0.000000 -0.445369991941 -0.681710302654 1.48775750121

      2005 1 20 00 04 30.000000 1.26854804266 1.86268110989 -4.31594225054
    </output>

    \;
  </session>>

  <subsection|Emulating Standard Positioning Service (SPS) Performance>

  By default, <with|font-family|tt|rinexpvt> will attempt to form the best
  possible position for each epoch of observation. If dual frequency
  observations are applied, they are used. If for a single epoch, only C/A
  observations are available, then it is used. In order to perform solutions
  using only C/A, the <with|font-family|tt|-s> switch is available. The
  following is a demonstration of this switch.

  <with|prog-language|shell|prog-session|default|<\session>
    <\input|shell] >
      rinexpvt -o usno0200.05o -n brdc0200.05n -m usno0200.05m -s
    </input>

    <\output>
      2005 1 20 00 00 0.000000 1112192.36858 -4842952.68698 3985350.17084

      2005 1 20 00 00 30.000000 1112190.34546 -4842953.75694 3985351.57171

      2005 1 20 00 01 0.000000 1112191.29632 -4842954.16477 3985353.65599

      2005 1 20 00 01 30.000000 1112191.97305 -4842954.21052 3985353.96079

      2005 1 20 00 02 0.000000 1112191.47444 -4842954.60185 3985351.44099

      2005 1 20 00 02 30.000000 1112191.67217 -4842953.79149 3985352.74304

      2005 1 20 00 03 0.000000 1112192.35285 -4842953.76184 3985351.25908

      2005 1 20 00 03 30.000000 1112189.43589 -4842951.88681 3985348.73888

      2005 1 20 00 04 0.000000 1112190.55705 -4842953.10278 3985349.95615

      2005 1 20 00 04 30.000000 1112188.71119 -4842952.128 3985348.15393

      \;
    </output>
  </session>>

  <subsection|Emulating Precise Positioning Service (PPS) Performance>

  Similar to the option described in the previous subsection, there is an
  option to limit the solutions of <with|font-family|tt|rinexpvt> to those
  strictly derived from dual frequency observations. The following session
  demonstrates this switch.

  <with|prog-language|shell|prog-session|default|<\session>
    <\input|shell] >
      rinexpvt -o usno0200.05o -n brdc0200.05n -m usno0200.05m -d \ 
    </input>

    <\output>
      2005 1 20 00 00 0.000000 1112192.67926 -4842951.98205 3985348.06329

      2005 1 20 00 00 30.000000 1112188.65938 -4842953.48346 3985351.48398

      2005 1 20 00 01 0.000000 1112189.48576 -4842957.45711 3985356.92698

      2005 1 20 00 01 30.000000 1112191.15384 -4842957.53284 3985355.11895

      2005 1 20 00 02 0.000000 1112191.1508 -4842955.89459 3985352.76549

      2005 1 20 00 02 30.000000 1112190.99828 -4842954.61737 3985352.3681

      2005 1 20 00 03 0.000000 1112189.92412 -4842954.29518 3985348.72842

      2005 1 20 00 03 30.000000 1112189.16937 -4842954.31307 3985351.92146

      2005 1 20 00 04 0.000000 1112191.62124 -4842955.6613 3985354.82972

      2005 1 20 00 04 30.000000 1112188.66733 -4842953.49038 3985348.04322
    </output>

    \;
  </session>>

  <subsection|Customizing the Epoch Format>

  The GPSTk library supports conversion among a number of time formats. This
  conversion ability is provided to the end user of
  <with|font-family|tt|rinexpvt> in the form of the <with|font-family|tt|-t>
  command line switch and its argument, a string describing the time format.
  The GPSTk documentation to DayTime's printf method contains a full list of
  specifiers that can be used within the time format string. The following
  table summarizes some of these options.

  \;

  <\with|par-mode|center>
    <tabular|<tformat|<table|<row|<cell|%Y>|<cell|Four digit
    year>>|<row|<cell|%y>|<cell|Year modulo 100>>|<row|<cell|%m>|<cell|Month
    number>>|<row|<cell|%b>|<cell|Month name>>|<row|<cell|%d>|<cell|Day of
    month>>|<row|<cell|%S>|<cell|Second of minute>>|<row|<cell|%F>|<cell|Full
    GPS week>>|<row|<cell|%Z>|<cell|Z count>>|<row|<cell|%g>|<cell|Seconds of
    week>>|<row|<cell|%j>|<cell|D<\float|float|tbh>
      <big-table||>
    </float>ay of year>>|<row|<cell|%s>|<cell|Seconds of
    day>>|<row|<cell|%Q>|<cell|Modified Julian Date>>>>>
  </with>

  <\with|par-mode|center>
    Table. Time Format Specifiers
  </with>

  \;

  In the following example we see how to apply the format specifiers in the
  form of a string.

  <with|prog-language|shell|prog-session|default|<\session>
    <\input|shell]>
      rinexpvt -o usno0200.05o -n brdc0200.05n -m usno0200.05m -t "%F %g"\ 
    </input>

    <\output>
      1306 345600.000000 1112192.67926 -4842951.98205 3985348.06329

      1306 345630.000000 1112188.65938 -4842953.48346 3985351.48398

      1306 345660.000000 1112189.48576 -4842957.45711 3985356.92698

      1306 345690.000000 1112191.15384 -4842957.53284 3985355.11895

      1306 345720.000000 1112191.1508 -4842955.89459 3985352.76549

      1306 345750.000000 1112190.99828 -4842954.61737 3985352.3681

      1306 345780.000000 1112189.92412 -4842954.29518 3985348.72842

      1306 345810.000000 1112189.16937 -4842954.31307 3985351.92146

      1306 345840.000000 1112191.62124 -4842955.6613 3985354.82972

      1306 345870.000000 1112188.66733 -4842953.49038 3985348.04322
    </output>

    \;
  </session>>

  <section|References>

  <\enumerate-numeric>
    <item>The GPS Toolkit, GPSTk. Website: http://www.gpstk.org/.

    <item>RINEX: The Receiver Independent Exchange Format Version 2.10.
    Available on the web at http://www.ngs.noaa.gov/CORS/Rinex2.html.

    <item>Hofmann-Wellenhoff, B., Lichtenegger, H., and Collins, J.
    <with|font-shape|italic|Global Positioning Theory:>
    <with|font-shape|italic|Theory and Practice>, fifth ed. Springer-Verlag.
    2004.

    <item>Parkinson, Bradford W. and Spilker, James J., editors. Global
    Positioning Theory: Theory and Applications, Volume I. AIAA Press, 1996.

    <item>Borre, Kai and Strang, Gilbert. <with|font-shape|italic|Linear
    Algebra, Geodesy and GPS>. Wellesley-Cambridge Press, 1997.

    <item>The GPS Interface Control Document (ICD-GPS-200), which can be
    found at http://www.navcen.uscg.gov/ftp/policy/icd200/ICD200Cw1234.pdf.

    <item>Goad, C. C. and Goodman, L. ``A modified tropospheric refraction
    correction model.'' <with|font-shape|italic|Proceeding of the Annual
    American Geophysical Union Fall Meeting>, San Francisco, 1974.
  </enumerate-numeric>

  \;
</body>

<\references>
  <\collection>
    <associate|auto-1|<tuple|1|1>>
    <associate|auto-10|<tuple|4.2|3>>
    <associate|auto-11|<tuple|4.3|4>>
    <associate|auto-12|<tuple|4.4|4>>
    <associate|auto-13|<tuple|4.5|4>>
    <associate|auto-14|<tuple|4.6|5>>
    <associate|auto-15|<tuple|1|?>>
    <associate|auto-16|<tuple|5|5>>
    <associate|auto-2|<tuple|2|1>>
    <associate|auto-3|<tuple|3|2>>
    <associate|auto-4|<tuple|3.1|2>>
    <associate|auto-5|<tuple|1|2>>
    <associate|auto-6|<tuple|3.2|2>>
    <associate|auto-7|<tuple|3.3|2>>
    <associate|auto-8|<tuple|4|3>>
    <associate|auto-9|<tuple|4.1|3>>
    <associate|toc-1|<tuple|1|?>>
    <associate|toc-2|<tuple|2|?>>
    <associate|toc-3|<tuple|3|?>>
    <associate|toc-4|<tuple|4|?>>
    <associate|toc-5|<tuple|5|?>>
  </collection>
</references>

<\auxiliary>
  <\collection>
    <\associate|table>
      <tuple|normal||<pageref|auto-15>>
    </associate>
    <\associate|toc>
      <vspace*|1fn><with|font-series|<quote|bold>|math-font-series|<quote|bold>|1<space|2spc>Overview>
      <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-1><vspace|0.5fn>

      <vspace*|1fn><with|font-series|<quote|bold>|math-font-series|<quote|bold>|2<space|2spc>Synopsis>
      <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-2><vspace|0.5fn>

      <vspace*|1fn><with|font-series|<quote|bold>|math-font-series|<quote|bold>|3<space|2spc>Detailed
      Description> <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-3><vspace|0.5fn>

      <with|par-left|<quote|1.5fn>|3.1<space|2spc>Observation Model
      <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-4>>

      <with|par-left|<quote|6fn>| <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-5><vspace|0.15fn>>

      <with|par-left|<quote|1.5fn>|3.2<space|2spc>Satellite Position Models
      <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-6>>

      <with|par-left|<quote|1.5fn>|3.3<space|2spc>Delay Models
      <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-7>>

      <vspace*|1fn><with|font-series|<quote|bold>|math-font-series|<quote|bold>|4<space|2spc>Examples
      and Usage Notes> <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-8><vspace|0.5fn>

      <with|par-left|<quote|1.5fn>|4.1<space|2spc>Generating Positions in WGS
      84 Coordinates <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-9>>

      <with|par-left|<quote|1.5fn>|4.2<space|2spc>Generating Positions in
      East/North/Up Coordinates <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-10>>

      <with|par-left|<quote|1.5fn>|4.3<space|2spc>Generating Positions using
      Precise Ephemerides <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-11>>

      <with|par-left|<quote|1.5fn>|4.4<space|2spc>Emulating Standard
      Positioning Service (SPS) Performance
      <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-12>>

      <with|par-left|<quote|1.5fn>|4.5<space|2spc>Emulating Precise
      Positioning Service (PPS) Performance
      <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-13>>

      <with|par-left|<quote|1.5fn>|4.6<space|2spc>Customizing the Epoch
      Format <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-14>>

      <vspace*|1fn><with|font-series|<quote|bold>|math-font-series|<quote|bold>|5<space|2spc>References>
      <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-16><vspace|0.5fn>
    </associate>
  </collection>
</auxiliary>