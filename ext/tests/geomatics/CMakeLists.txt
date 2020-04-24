set( SD ${GPSTK_TEST_DATA_DIR} )
set( TD ${GPSTK_TEST_OUTPUT_DIR} )

###############################################################################
# TEST geomatics applications: DDBase and DiscFix
###############################################################################

###############################################################################
# @todo - DDBase: Check that -h/--help is handled properly
###############################################################################
#add_test(NAME DDBase_CmdOpt_help
#         COMMAND ${CMAKE_COMMAND
#         -DTEST_PROG=$<TARGET_FILE:DDBase>
#         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
#         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
#         -P ${CMAKE_SOURCE_DIR}/core/tests/testhelp.cmake)

# ! ! ! WARNING ! ! !  DDBase currently only accepts command-line parameters
#                      with a very particular syntax:
#
# -fFILE       Accepted
# -f FILE      Rejected
# --fileFILE   Rejected
# --file FILE  Accepted
# --file=FILE  Rejected
#
# This unfortunate situation complicates the passing of parameters to the
# other cmake scripts - only -DARGS handles them adequately.  Thus, the tests
# below pass all parameters via ARGS with escaped spaces; SPARG{1-4} won't work.


###############################################################################
# DDBase: [-f|--file] <file>  Name of file containing more options
###############################################################################

# Missing options file
add_test(NAME DDBase_CmdOpt_f_missing
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:DDBase>
         -DARGS="-f missingFile"
         -P ${CMAKE_SOURCE_DIR}/core/tests/testfailexp.cmake)
set_property(TEST DDBase_CmdOpt_f_missing PROPERTY LABELS DDBase)

# Present but invalid options file
add_test(NAME DDBase_CmdOpt_f_invalid
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:DDBase>
         -DARGS="-f ${GPSTK_TEST_DATA_DIR}/test_input_ddbase.sp3"
         -P ${CMAKE_SOURCE_DIR}/core/tests/testfailexp.cmake)
set_property(TEST DDBase_CmdOpt_f_invalid PROPERTY LABELS DDBase)

# Valid options file - includes valid --ObsFile, --NavFile, --DT, --BeginTime,
# --EndTime, --EOPFile, --PosXYZ, --Fix, --PosPRS, and --BaseOut
add_test(NAME DDBase_CmdOpt_f_valid
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:DDBase>
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DNODIFF=TRUE
         -DTESTNAME=DDBase_CmdOpt_f_valid
         -DARGS=-f${GPSTK_TEST_DATA_DIR}/test_input_ddbase.opt_ok\ --ObsPath\ ${GPSTK_TEST_DATA_DIR}\ --NavPath\ ${GPSTK_TEST_DATA_DIR}\ --EOPPath\ ${GPSTK_TEST_DATA_DIR}
         -P ${CMAKE_SOURCE_DIR}/core/tests/testsuccexp.cmake)
set_property(TEST DDBase_CmdOpt_f_valid PROPERTY LABELS DDBase)


###############################################################################
# DDBase: --Log <file>  Name of output log file (ddbase.log)
###############################################################################

# Valid options file plus a valid log file
add_test(NAME DDBase_CmdOpt_log_valid
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:DDBase>
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DNODIFF=TRUE
         -DTESTNAME=DDBase_CmdOpt_log_valid
         -DARGS=-f${GPSTK_TEST_DATA_DIR}/test_input_ddbase.opt_ok\ --ObsPath\ ${GPSTK_TEST_DATA_DIR}\ --NavPath\ ${GPSTK_TEST_DATA_DIR}\ --EOPPath\ ${GPSTK_TEST_DATA_DIR}\ --Log\ ${GPSTK_TEST_OUTPUT_DIR}/test_output_ddbase.log
         -P ${CMAKE_SOURCE_DIR}/core/tests/testsuccexp.cmake)
set_property(TEST DDBase_CmdOpt_log_valid PROPERTY LABELS DDBase)

# Valid options file plus an invalid log file
add_test(NAME DDBase_CmdOpt_log_invalid
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:DDBase>
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DNODIFF=TRUE
         -DARGS=-f${GPSTK_TEST_DATA_DIR}/test_input_ddbase.opt_ok\ --ObsPath\ ${GPSTK_TEST_DATA_DIR}\ --NavPath\ ${GPSTK_TEST_DATA_DIR}\ --EOPPath\ ${GPSTK_TEST_DATA_DIR}\ --Log\ ${GPSTK_TEST_OUTPUT_DIR}/floop/derp/test_output_ddbase.log
         -P ${CMAKE_SOURCE_DIR}/core/tests/testfailexp.cmake)
set_property(TEST DDBase_CmdOpt_log_invalid PROPERTY LABELS DDBase)


###############################################################################
# DDBase: --ObsPath <path>      Path for input obs file(s) ()
###############################################################################

# Valid options file plus an invalid obs path
add_test(NAME DDBase_CmdOpt_obspath_invalid
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:DDBase>
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DNODIFF=TRUE
         -DARGS=-f${GPSTK_TEST_DATA_DIR}/test_input_ddbase.opt_ok\ --ObsPath\ ${GPSTK_TEST_DATA_DIR}/floop/derp\ --NavPath\ ${GPSTK_TEST_DATA_DIR}\ --EOPPath\ ${GPSTK_TEST_DATA_DIR}
         -P ${CMAKE_SOURCE_DIR}/core/tests/testfailexp.cmake)
set_property(TEST DDBase_CmdOpt_obspath_invalid PROPERTY LABELS DDBase)


###############################################################################
# DDBase: --ObsFile <name,id>   RINEX obs file name, and station label [repeatable] ()
###############################################################################

# Valid options file plus an invalid obs file
add_test(NAME DDBase_CmdOpt_obsfile_invalid
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:DDBase>
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DNODIFF=TRUE
         -DARGS=-f${GPSTK_TEST_DATA_DIR}/test_input_ddbase.opt_ok\ --ObsPath\ ${GPSTK_TEST_DATA_DIR} --NavPath\ ${GPSTK_TEST_DATA_DIR}\ --EOPPath\ ${GPSTK_TEST_DATA_DIR}\ --ObsFile\ missing_obs_file
         -P ${CMAKE_SOURCE_DIR}/core/tests/testfailexp.cmake)
set_property(TEST DDBase_CmdOpt_obsfile_invalid PROPERTY LABELS DDBase)


###############################################################################
# DDBase: --NavPath <dir>       Path of navigation file(s) ()
###############################################################################

# Valid options file plus an invalid nav path
add_test(NAME DDBase_CmdOpt_navpath_invalid
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:DDBase>
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DNODIFF=TRUE
         -DARGS=-f${GPSTK_TEST_DATA_DIR}/test_input_ddbase.opt_ok\ --ObsPath\ ${GPSTK_TEST_DATA_DIR} --NavPath\ ${GPSTK_TEST_DATA_DIR}/floop/derp\ --EOPPath\ ${GPSTK_TEST_DATA_DIR}\ --ObsFile\ missing_obs_file
         -P ${CMAKE_SOURCE_DIR}/core/tests/testfailexp.cmake)
set_property(TEST DDBase_CmdOpt_navpath_invalid PROPERTY LABELS DDBase)


###############################################################################
# DDBase: --NavFile <file>      Navigation (RINEX nav OR SP3) file name [repeatable] ()
###############################################################################

# Valid options file plus an invalid nav file
add_test(NAME DDBase_CmdOpt_navfile_invalid
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:DDBase>
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DNODIFF=TRUE
         -DARGS=-f${GPSTK_TEST_DATA_DIR}/test_input_ddbase.opt_ok\ --ObsPath\ ${GPSTK_TEST_DATA_DIR} --NavPath\ ${GPSTK_TEST_DATA_DIR}\ --EOPPath\ ${GPSTK_TEST_DATA_DIR}\ --NavFile\ missing_nav_file
         -P ${CMAKE_SOURCE_DIR}/core/tests/testfailexp.cmake)
set_property(TEST DDBase_CmdOpt_navfile_invalid PROPERTY LABELS DDBase)


###############################################################################
# DDBase: --EOPPath <dir>       Path of earth orientation file(s) ()
###############################################################################

# Valid options file plus an invalid eop path
add_test(NAME DDBase_CmdOpt_eoppath_invalid
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:DDBase>
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DNODIFF=TRUE
         -DARGS=-f${GPSTK_TEST_DATA_DIR}/test_input_ddbase.opt_ok\ --ObsPath\ ${GPSTK_TEST_DATA_DIR} --NavPath\ ${GPSTK_TEST_DATA_DIR}\ --EOPPath\ ${GPSTK_TEST_DATA_DIR}/floop/derp\ --ObsFile\ missing_obs_file
         -P ${CMAKE_SOURCE_DIR}/core/tests/testfailexp.cmake)
set_property(TEST DDBase_CmdOpt_eoppath_invalid PROPERTY LABELS DDBase)


###############################################################################
# DDBase: --EOPFile <file>      Earth orientation parameter (EOPP or IERS format) file [repeatable].
###############################################################################

# Valid options file plus an invalid eop file
add_test(NAME DDBase_CmdOpt_eopfile_invalid
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:DDBase>
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DNODIFF=TRUE
         -DARGS=-f${GPSTK_TEST_DATA_DIR}/test_input_ddbase.opt_ok\ --ObsPath\ ${GPSTK_TEST_DATA_DIR} --NavPath\ ${GPSTK_TEST_DATA_DIR}\ --EOPPath\ ${GPSTK_TEST_DATA_DIR}\ --EOPFile\ missing_eop_file
         -P ${CMAKE_SOURCE_DIR}/core/tests/testfailexp.cmake)
set_property(TEST DDBase_CmdOpt_eopfile_invalid PROPERTY LABELS DDBase)

# @todo - Valid IERS input file

# @todo - Invalid IERS input file

# @todo - If no EOP file is given, DDBase will search for a default IERS
#         format file in the current directory.] (finals.daily)
#


###############################################################################
# @todo - DDBase: Station configuration   [--Pos.. (1 only) MUST be given for each site]:
###############################################################################
# --PosXYZ <X,Y,Z,id>   Station position in ECEF coordinates in m,
#                         followed by a label identifying the station. ()
# --PosLLH <La,Lo,H,id> Station position in geodetic coordinates:
#                         Latitude(deg),Longitude(E,deg),Height(m),label ()
# --PosPRS <id>         Let position of station labelled <id> be set to the computed
#                         average pseudorange solution for that site. ()
###############################################################################


###############################################################################
# @todo - DDBase: --TropModel <trop,id> Use trop model <trop> for station <id>,
#                    choices are: 'Zero','Black','NewB','ModHop','ModHopH',
#                    'Saas' [cf. GPSTk] (Saas)
###############################################################################


###############################################################################
# @todo - DDBase: --Weather <T,P,H,id>  Weather parameters: Temperature(degC),
#                    Pressure(mbar), Humidity(%), followed by a label
#                    identifying the station. (20.0,1010.00,50.0)
###############################################################################


###############################################################################
# @todo - DDBase: --Fix <id>  Hold the station <id> fixed in estimation [repeatable] (don't)
###############################################################################


###############################################################################
## @todo - DDBase Configuration:
###############################################################################
# --noEstimate          Quit before performing the estimation. (don't)
# --Freq <L1|L2|L3>     Process L1, L2 or L3 [L1+L2] frequency data [L3 not validated] (L1)
# --nIter <n>           Maximum number of estimation iterations (5)
# --Converge <cl>       Convergence limit on RSS change in state in meters (5.00e-08)
# --FixBiases           Perform an extra, last iteration that fixes the phase biases (don't)
#
## Model, state elements, a priori constraints:
# --RZDnIntervals <n>   Number of (equal time) residual zenith delay intervals (0)
#                         [enter 0 to turn off estimation of RZD]
# --RZDtimeconst <tau>  Time constant in hours for multiple RZD intervals (2.00)
# --RZDsigma <sig>      A priori sigma in m for residual zenith delay (0.50)
# --Tight <ppm>         Tight a priori constraint, a fraction of baseline (0.0001)
# --Loose <ppm>         Loose a priori constraint, a fraction of baseline (0.1)
#
## Time limits:
# --BeginTime <arg>     Start time: arg = 'GPSweek,sow' OR 'YYYY,M,D,H,Min,Sec' ()
# --EndTime <arg>       End time: arg = 'GPSweek,sow' OR 'YYYY,M,D,H,Min,Sec' ()
#
## Satellite time table:
# --TimeTable <file>    Time table file name [if this option does not appear
#                          a time table will be computed and output to log file] ()
# --Ref <sat>           Use <sat> as 'reference' in DDs; don't use a timetable ()
#
## Data editing:
# --MinElev <elev>      Ignore data below elevation <elev> degrees, DDs only (10.00)
# --AntRotElev <elev>   Apply MinElev to antenna rotated in elevation by <elev> deg. ()
# --AntRotAz <az>       Apply MinElev to antenna rotated in azimuth by <az> deg. ()
# --MaxGap <n>          Maximum acceptable gap in data [number of --DT intervals] (10)
# --MinDDSeg <n>        Minimum acceptable length of DD data segment (50)
# --PhaseBiasReset <n>  Limit on pt-to-pt change in phase without reset, in cycles (10)
# --XSat <sat>          Exclude this satellite [repeatable] ()
# --DT <t>              Data time interval in sec, or decimate input data [REQUIRED] ()
#
## Pseudorange solution (PRS) configuration:
# --PRSniter <n>        PRS: Limit on number of iterations (10)
# --PRSconverge <cl>    PRS: Convergence limit in m (3.00e-07)
# --PRSrmsLimit <rms>   PRS: RMS residual limit in m (6.50)
# --PRSslopeLimit <sl>  PRS: RAIM residual limit in m (0.00)
# --PRSMinElev <elev>   PRS: Reject data below elevation <elev> degrees (10.00)
#
## Output files:
# --OutPath <dir>       Path for output files ()
# --RAWFileOut <file>   Filename for output of raw data ()
# --PRSFileOut <file>   Filename for output of pseudorange solution ()
# --CLKFileOut <file>   Filename for output of Rx clock bias and model ()
# --RDDFileOut <file>   Filename for output of raw DD data ()
# --DDDFileOut <file>   Filename for output of (edited) DD data ()
# --TDDFileOut <file>   Filename for output of triple difference data ()
# --DDRFileOut <file>   Filename for output of DD post-fit residuals ()
#
## Output misc:
# --BaseOut <id-id,x,y,z> Baseline to output; <id>s are station labels, '-' is
#                          required, <x,y,z> are optional baseline coordinates
#                          for comparison [repeatable] ()
# --validate            Read input and validate it, then quit (don't)
#


###############################################################################
# @todo - DiscFix: Check that -h/--help is handled properly
###############################################################################
#add_test(NAME DiscFix_CmdOpt_help
#         COMMAND ${CMAKE_COMMAND}
#         -DTEST_PROG=$<TARGET_FILE:DiscFix>
#         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
#         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
#         -P ${CMAKE_SOURCE_DIR}/core/tests/testhelp.cmake)

##############################################################################
# @todo - DiscFix: Check that all other command line options are handled properly
###############################################################################


###############################################################################
# Test EarthOrientation against SOFA example code
###############################################################################
add_test(NAME EarthOrientation_SOFA
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:test_EO_SOFA>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTNAME=EarthOrientation_SOFA
         -DTESTBASE=EarthOrientation_SOFA
         -DDIFF_ARGS=-l1\ -e1.e-12
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST EarthOrientation_SOFA PROPERTY LABELS Geomatics)

###############################################################################
# Test SolidEarth and OceanLoad Tides vs IERS software (cf OceanLoadTides.cpp)
###############################################################################
add_test(NAME EarthTides_IERS
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:test_tides>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTNAME=EarthTides_IERS
         -DTESTBASE=EarthTides_IERS
         -DARGS=--dt\ 360.\ --prec\ 6\ --solid\ --ocean\ --oceanfile\ ${GPSTK_TEST_DATA_DIR}/testocean.blq\ --oceansite\ ONSALA,REYKJAVIK\ --atm\ --atmfile\ ${GPSTK_TEST_DATA_DIR}/testatm.atl\ --atmsite\ reyk,onsa
         -DDIFF_ARGS=-l4\ -e1.e-5
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST EarthTides_IERS PROPERTY LABELS Geomatics)

###############################################################################
# Test JPL ephemeris 403 conversion
###############################################################################
set(JPLDIR ${CMAKE_CURRENT_SOURCE_DIR}/../../apps/geomatics/JPLeph/JPL/)
add_test(NAME JPL_403eph_conversion
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:convertSSEph>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTNAME=JPL_403eph_conversion
         -DTESTBASE=JPL_403eph_conversion
         -DARGS=--header\ ${JPLDIR}/header.403\ --data\ ${JPLDIR}/ascp1975.403,${JPLDIR}/ascp2000.403,${JPLDIR}/ascp2025.403\ --output\ SolarSystem1975to2050.403.bin\ --verbose
         -DDIFF_ARGS=-l50\ -z1\ -e1.e-16
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST JPL_403eph_conversion PROPERTY LABELS Geomatics)

###############################################################################
# Test JPL ephemeris 403 accuracy
###############################################################################
add_test(NAME JPL_403eph_accuracy
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:testSSEph>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTNAME=JPL_403eph_accuracy
         -DTESTBASE=JPL_403eph_accuracy
         -DARGS=--file\ SolarSystem1975to2050.403.bin\ --test\ ${JPLDIR}/testpo.403\ -d
         -DDIFF_ARGS=-l1\ -z1\ -e1.e-16
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST JPL_403eph_accuracy PROPERTY LABELS Geomatics)
set_property(TEST JPL_403eph_accuracy PROPERTY DEPENDS JPL_403eph_conversion)

###############################################################################
# Test JPL ephemeris 405 conversion
###############################################################################
add_test(NAME JPL_405eph_conversion
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:convertSSEph>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTNAME=JPL_405eph_conversion
         -DTESTBASE=JPL_405eph_conversion
         -DARGS=--header\ ${JPLDIR}/header.405\ --data\ ${JPLDIR}/ascp1980.405,${JPLDIR}/ascp2000.405,${JPLDIR}/ascp2020.405\ --output\ SolarSystem1975to2050.405.bin\ --verbose
         -DDIFF_ARGS=-l50\ -z1\ -e1.e-16
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST JPL_405eph_conversion PROPERTY LABELS Geomatics)

###############################################################################
# Test JPL ephemeris 405 accuracy
###############################################################################
add_test(NAME JPL_405eph_accuracy
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:testSSEph>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTNAME=JPL_405eph_accuracy
         -DTESTBASE=JPL_405eph_accuracy
         -DARGS=--file\ SolarSystem1975to2050.405.bin\ --test\ ${JPLDIR}/testpo.405\ -d
         -DDIFF_ARGS=-l1\ -z1\ -e1.e-16
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST JPL_405eph_accuracy PROPERTY LABELS Geomatics)
set_property(TEST JPL_405eph_accuracy PROPERTY DEPENDS JPL_405eph_conversion)

###############################################################################
# Test StatsFilters filters
###############################################################################
add_executable(StatsFilter_T StatsFilter_T.cpp)
target_link_libraries(StatsFilter_T gpstk)
add_test(StatsFilter StatsFilter_T)
set_property(TEST StatsFilter PROPERTY LABELS Geomatics)

###############################################################################
## Test dfix
################################################################################
add_test(NAME test_dfix_karr
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:dfix>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTNAME=test_dfix_karr
         -DTESTBASE=test_dfix_karr
         -DARGS=--obs\ ${GPSTK_TEST_DATA_DIR}/test_dfix_karr0880.ed.10o
         -DDIFF_ARGS=-l101\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST test_dfix_karr PROPERTY LABELS Geomatics)

################################################################################
add_test(NAME test_dfix_tower
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:dfix>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTNAME=test_dfix_tower
         -DTESTBASE=test_dfix_tower
         -DARGS=--obs\ ${GPSTK_TEST_DATA_DIR}/test_dfix_tower239.ed.15o\ --DC\ width=200,MinPts=100,MaxGap=100
         -DDIFF_ARGS=-l94\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST test_dfix_tower PROPERTY LABELS Geomatics)

################################################################################
add_test(NAME test_dfix_txau
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:dfix>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTNAME=test_dfix_txau
         -DTESTBASE=test_dfix_txau
         -DARGS=--obs\ ${GPSTK_TEST_DATA_DIR}/test_dfix_txau047.ed.12o\ --DC\ width=100,MinPts=50,MaxGap=50
         -DDIFF_ARGS=-l93\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST test_dfix_txau PROPERTY LABELS Geomatics)

################################################################################
add_test(NAME test_dfix_job4131
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:dfix>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTNAME=test_dfix_job4131
         -DTESTBASE=test_dfix_job4131
         -DARGS=--obs\ ${GPSTK_TEST_DATA_DIR}/test_dfix_job4131.ed.obs\ --obsID\ RC1*,RC2*,RL1*,RL2*
         -DDIFF_ARGS=-l93\ -z1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST test_dfix_job4131 PROPERTY LABELS Geomatics)

################################################################################
# Test Rinex3ObsFileLoader RINEX3.03 input
###############################################################################
add_test(NAME Rinex3ObsLoader_R303
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Rinex3ObsLoader_T>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTNAME=Rinex3ObsLoader_R303
         -DTESTBASE=Rinex3ObsLoader_R303
         -DARGS=${GPSTK_TEST_DATA_DIR}/Rinex3ObsLoader303.obs
         -DDIFF_ARGS=-l1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST Rinex3ObsLoader_R303 PROPERTY LABELS Geomatics)

###############################################################################
# Test Rinex3ObsFileLoader RINEX2.11 input
###############################################################################
add_test(NAME Rinex3ObsLoader_R211
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Rinex3ObsLoader_T>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTNAME=Rinex3ObsLoader_R211
         -DTESTBASE=Rinex3ObsLoader_R211
         -DARGS=${GPSTK_TEST_DATA_DIR}/Rinex3ObsLoader211.obs
         -DDIFF_ARGS=-l1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST Rinex3ObsLoader_R211 PROPERTY LABELS Geomatics)

###############################################################################
# Test Rinex3ObsFileLoader RINEX2.10 input
###############################################################################
add_test(NAME Rinex3ObsLoader_R210
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:Rinex3ObsLoader_T>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTNAME=Rinex3ObsLoader_R210
         -DTESTBASE=Rinex3ObsLoader_R210
         -DARGS=${GPSTK_TEST_DATA_DIR}/Rinex3ObsLoader210.obs
         -DDIFF_ARGS=-l1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST Rinex3ObsLoader_R210 PROPERTY LABELS Geomatics)

###############################################################################
add_executable(KalmanFilter_T KalmanFilter_T.cpp)
target_link_libraries(KalmanFilter_T gpstk)
add_test(KalmanFilter KalmanFilter_T)
set_property(TEST KalmanFilter PROPERTY LABELS Geomatics)

################################################################################


set (RSTATS_ARGS_1 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -bc -br -b2 -bw")
add_test(NAME rstats_1
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_1
  -DTESTBASE=rstats_1
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_1}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_1 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_2 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 -bc --beg 66600 --end 69000")
add_test(NAME rstats_2
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_2
  -DTESTBASE=rstats_2
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_2}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_2 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_3 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 -br --min 10. --max 40.")
add_test(NAME rstats_3
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_3
  -DTESTBASE=rstats_3
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_3}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_3 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_4 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --KS")
add_test(NAME rstats_4
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_4
  -DTESTBASE=rstats_4
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_4}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_4 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_5 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -bw")
add_test(NAME rstats_5
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_5
  -DTESTBASE=rstats_5
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_5}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_5 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_6 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 --brief")
add_test(NAME rstats_6
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_6
  -DTESTBASE=rstats_6
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_6}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_6 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_7 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -brw")
add_test(NAME rstats_7
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_7
  -DTESTBASE=rstats_7
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_7}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_7 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_8 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -br")
add_test(NAME rstats_8
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_8
  -DTESTBASE=rstats_8
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_8}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_8 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_9 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -bw")
add_test(NAME rstats_9
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_9
  -DTESTBASE=rstats_9
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_9}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_9 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_10 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -bc")
add_test(NAME rstats_10
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_10
  -DTESTBASE=rstats_10
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_10}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_10 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_11 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --wt 3 -b2")
add_test(NAME rstats_11
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_11
  -DTESTBASE=rstats_11
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_11}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_11 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_12 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --outs 0.5")
add_test(NAME rstats_12
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_12
  -DTESTBASE=rstats_12
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_12}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_12 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_13 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --plot")
add_test(NAME rstats_13
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_13
  -DTESTBASE=rstats_13
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DDIFF_PROG=$<TARGET_FILE:df_diff>
  -DDIFF_ARGS=-l1
  -DARGS=${RSTATS_ARGS_13}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_13 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_14 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --qplot --stdout")
add_test(NAME rstats_14
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_14
  -DTESTBASE=rstats_14
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_14}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_14 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_15 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --bin 30")
add_test(NAME rstats_15
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_15
  -DTESTBASE=rstats_15
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_15}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_15 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_16 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --sum")
add_test(NAME rstats_16
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_16
  -DTESTBASE=rstats_16
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_16}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_16 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_17 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --sum+")
add_test(NAME rstats_17
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_17
  -DTESTBASE=rstats_17
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_17}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_17 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_18 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --fit 5,65250,67700 --stdout")
add_test(NAME rstats_18
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_18
  -DTESTBASE=rstats_18
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_18}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_18 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_19 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --seq --label SEQS")
add_test(NAME rstats_19
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_19
  -DTESTBASE=rstats_19
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_19}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_19 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_20 "${SD}/SDexam01.txt -q -p 4 -x 1 -y 2 --disc 0.8,0")
add_test(NAME rstats_20
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_20
  -DTESTBASE=rstats_20
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_20}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_20 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_21 "${SD}/SDexam01.txt -q -p 5 -x 1 -y 2 --fdf 0.8")
add_test(NAME rstats_21
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_21
  -DTESTBASE=rstats_21
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_21}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_21 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_22 "${SD}/SDexam01.txt -q -p 5 -x 1 -y 2 --fdf2 0.8,0.02,2")
add_test(NAME rstats_22
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_22
  -DTESTBASE=rstats_22
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_22}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_22 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_23 "${SD}/SDexam01.txt -q -p 5 -x 1 -y 2 --wind 10")
add_test(NAME rstats_23
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_23
  -DTESTBASE=rstats_23
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_23}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_23 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_24 "${SD}/SDexam01.txt -q -p 5 -x 1 -y 2 --xwind 10")
add_test(NAME rstats_24
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_24
  -DTESTBASE=rstats_24
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_24}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_24 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_25 "${SD}/SDexam01.txt -q -p 5 -x 1 -y 2 --fix 4")
add_test(NAME rstats_25
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_25
  -DTESTBASE=rstats_25
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_25}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_25 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_26 "${SD}/SDexam01.txt -q -p 5 -x 1 -y 2 --wnj 1.e-4")
add_test(NAME rstats_26
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_26
  -DTESTBASE=rstats_26
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DDIFF_PROG=$<TARGET_FILE:df_diff>
  -DARGS=${RSTATS_ARGS_26}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_26 PROPERTY LABELS Geomatics)

set (RSTATS_ARGS_27 "${SD}/testfft.data -q -x 1 -y 9 -p 8 --fft 0.0034722")
add_test(NAME rstats_27
  COMMAND ${CMAKE_COMMAND}
  -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
  -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
  -DTESTNAME=rstats_27
  -DTESTBASE=rstats_27
  -DTEST_PROG=$<TARGET_FILE:rstats>
  -DARGS=${RSTATS_ARGS_27}
  -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST rstats_27 PROPERTY LABELS Geomatics)
