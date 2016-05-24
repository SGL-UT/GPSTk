      #################################################

Version 2.9   Friday, May 20, 2016

   General modifications
   ---------------------

   - Significant progress resolving failed tests on Windows and Redhat
   - CPack update - RPM Packages and NSIS Windows installers added to package suite using build.sh
   - Added required files for Gitlab-CI builds
   - Updates to SWIG bindings, fully compiles on OSX
   - Addes initial set of Python/SWIG tests
   - Major Doxygen cleanup and overhaul
   - Cleanup of old code and documentation
   - This release includes new/modified source code comments for improved readability.
   - Various Bug and Compile Warning Fixes

   Modifications by Author
   ---------------------
   
Andrew Joplin (2):
      SunEarthSatGeometry: Added OrbitNormalAttitude function
      Added swig bindings for SunEarthSatGeometry

Anthony Hughes (4):
      Make output of END OF HEADER mandatory in reallyPutRecord.
      Handle NGA-specific event extension to SP3a during output; fixed SP3b SV ID format; added basic unit tests.
      Added a more correct and efficient addSubframe() variant that accepts the subframe as an array of uint32_t, thus avoiding several array copies.
      Fixed exception specifications for several methods that could throw despite throw().

Audric Terry (1):
      Several PRN/SVN updates to SVNumXRef.cpp

Brian Tolman (1):
      Update discontinuity corrector for GLONASS and improve algorithm

Bryan Parsons (67):
      Added Linux distro detection. RPM packages now build for Red Hat Linux distro.  NSIS support added for Windows installation.
      Erase garbage introduced when fixing merge conflicts.
      Merge Remote Master into Local Master. Updated v2.8.1 with new commit to update CMakeLists.txt file
      Merge branch 'issue_209' into 'master'
      Merge branch 'master' into cpack_rpm_nsis
      Merge branch 'SunEarthSatGeometry' into 'master'
      Merge branch 'cpack_rpm_nsis' into 'master'
      Adding warning flag back to silence SunOS warnings.
      Merge branch 'swig_tests' into 'master'
      Updating build_setup.sh to fix bug with SGLTk jbuild.sh
      Added root .gitlab-ci.yml file for gitlab ci integration with gitlab2
      Fixed formatting for .gitlab-ci.yml
      Initial addition of build code for command line Windows build by build.sh
      Updated build.sh to settle merge conflicts.
      Updated error in .gitlab-ci.yml script
      Updated gitlab-ci script to adjust for Windows cmd prompt command.
      Added tweaks to Windows build.sh command line support.
      Merge branch 'master' into build_windows
      Merge branch 'build_windows' into 'master'
      Merging Master into current gitlab-ci branch
      Updated gitlab-ci file for additional builds, added type: category as well.
      Commented out current platforms with no active runners.
      Updated .yml file to test staging.
      Adjusted .yml script for artifacts and dependencies.
      more tweaks to .yml file for proper syntax, after validating using CI Lint tool.
      Updating Artifact paths and Testing Debian and Opti022 runners.
      Removing opti022 code, testing debian build on Opti022.
      Commented out redhat runner code for now.
      Added cache option to .yml file.
      Testing cache function on windows only.
      Removed all dependency variables, testing ssh executor using imac osx coordinator.
      Tweak to code, testing new osx coordinator system level runners.
      Comments to .yml file
      Added all supported GPSTk platforms to .yml for testing with sudo osx coordinator and ssh scripts.
      Added deploy label to .yml file.
      Merge branch 'issue_250_NavID' into 'master'
      Adding CPACK License parameter for NSIS Windows installer build.
      Merge branch 'issue_249' into 'master'
      Merge branch 'master' into gitlab-ci
      Cleaned up and Updated .gitlab-ci.ym for testing on live gitlab-ci server.
      Updated SWIG CMakeLists.txt to ignore -std=c++ for Redhat Linux platforms.
      Merge branch 'gitlab-ci' into 'master'
      Fixed Overloading ambiguity error on Solaris, change pow(int,int) to pow(float,int)
      Merge branch 'issue_238' into 'master'
      Turning off SWIG bindings for Solaris Gitlab-CI Runner build.
      Merge branch 'master' into svnum_updates
      Merge branch 'svnum_updates' into 'master'
      Merge branch 'issue_236' into 'master'
      Explicit build folder for Gitlab-Ci redhat and osx builds
      Merge branch 'issue_249' into 'master'
      Updating Changelog and NEWS files with missing information.
      Updates to tests to reduce errors in Windows GPSTk build. Submitted by John Knudson.
      Merge branch 'windows_fix' into 'master'
      Merge branch 'splitWithDoubleQuotes' into 'master'
      Merge branch 'issue_251' into 'master'
      Added ifdef for Linux for unistd.h support after merging in Windows bug fixes.
      Commented out GPSWeekSecond tests dueto the delay of merging issue_248 branch.
      Updates for FileHunter and FileUtils tests for fixing Win32 bugs.  Spelling mistakes fixed in swig/PythonSetup.cmake.
      Merge branch 'GPSTk_RC29' into 'master'
      Merge branch '253-add-initialization-to-engephemeris-aodo' into 'master'
      Fixed various Windows build issues.  Added various #ifdef's to designate appropriate libraries between Windows and Linux.
      Merge branch 'windows_fixes' into 'master'
      Merge branch 'comment_fix_BAR' into 'master'
      Merge branch 'jcl01' into 'master'
      Silencing OSX Warnings.
      RinDump and RinEdit bug fixes, submitted by Brian Tolman.  2-3 important bugs fixed with 1-line fixes.
      Merge branch 'Rinex_Fixes_Tolman' into 'master'

Frederick Doe (1):
      Added initialization of AODO to EngEphemeris

Jessica Rosenquest (5):
      Modified
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Added include for compiling on debian.
      Merge branch 'master' into navfilter
      Added NavID.hpp, NavID.cpp, and NavID_T.cpp.

Jon C. Little (16):
      Removing files that should have not been part of the source tree. Part 1
      Removing files that should have not been part of the source tree. Part 2
      Merge branch 'master' into issue_209_python_ctest
      Hacked Rinex3ObsHeaderTouchHeaderMerge to do something on Rinex3Obs and made the tests pass
      Reworking swig builds
      Fixing bug where the wrong python libraries were bein found
      Moving python config to the swig dir
      Finally compiling on OSX
      Finally compiling the swig bindings on OSX
      Merge branch 'swig_tests' of repositories.arlut.utexas.edu:sgl/gpstk into swig_tests
      Big refactoring of the swig python module.     Its almost what it should be now.
      Fixing some test failures.
      Got rid of installing the example python programs and added installing a couple python apps
      Simplifying specifying the files to populate the module with
      Making test_utils work for sgltk
      Merge branch 'master' into swig_tests

Jon Little (34):
      Initial commit of adding swig bindings to ctest
      Now has first cut at the first python/swig tests
      Now with a code that parallels the C++ tests
      Fixed the working dir so we don't polute the source dir
      Refactored tests to use python unittest module
      trying to settle on a test file naming scheme
      Now with some doc strings
      adding testing of time classes
      Fixed a bug where build.sh won't work on all linux platforms
      Merge branch 'master' into issue_209
      Fixed a build bug on OS X
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Moved all the build dirs to a single subdirectory.
      Merge branch 'master' into file_cleanup
      Merge branch 'issue_233' into 'master'
      Nicer reporting of test failures at the end of the run
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Merge branch 'file_cleanup' into 'master'
      Merge branch 'master' into issue_209
      Refactored to use argparse to communicate input dir, output dir, and other stuff to the python tests
      Now verifies that the swig module is loaded from the requested location
      Settig the appropriate time system on GPSWeekZcount
      Now supports writing a rinex obs file from scratch
      Fixed bug when tests are not run.
      Fixed another bug when tests are not run.
      Merge branch 'master' into issue_68_novatel
      Merge branch 'issue_68_novatel' into 'master'
      Moving TestUtil into the gpstk namespace...
      Merge branch '237-TestUtil' into 'master'
      test_utils was not telling cmake it was failing ...
      Better comments about satellite id
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Adding .gitattributes to normalize line endings
      Introduce end-of-line normalization

Nick Fitzsimmons (2):
      New Test Case to Display GPSWeekSecond Conversion Bug
      Update to TestUtil

Steve Johnson (3):
      Add StringUtils::splitWithDoubleQuotes(...)
      Merge branch 'master' into splitWithDoubleQuotes
      Fix redhat, solaris, osx broken builds

anthony (1):
      Merge branch 'issue_232_sp3_nga' into 'master'

johnk (103):
      Doxygen overhaul part 1
      Doxygen overhaul part 2
      Allow indpendent execution of doxygen (i.e. outside of build.sh) and tweak formatting
      Doxygen overhaul part 3
      Doxygen overhaul part 4
      Doxygen overhaul part 5
      Doxygen overhaul part 6
      Doxygen overhaul part 7
      Merge remote-tracking branch 'origin/master' into issue_207
      Merge remote-tracking branch 'origin/master' into issue_207
      Doxygen overhaul part 8
      Doxygen overhaul part 9
      Change Doxygen time group label
      Doxygen overhaul part 10
      First pass at a modular nav data filter
      Fix some bugs that arise from using combined nav filters
      Remove debug output
      Correct a comment
      Doxygen stuff
      more doxygen
      Add NavFilterMgr example
      Add a minimalistic pseudo-code example
      Fix doxygen formatting issue
      Merge remote-tracking branch 'origin/master' into navfilter
      Merge remote-tracking branch 'origin/master' into issue_207
      Doxygen-ify comments and some code reformatting
      Fix up example
      Turn off most graphs by default
      Remove waitLength(), add finalize(), implement LNavCrossSourceFilter
      First cut at testing multi-epoch filters and finalize()
      Merge branch 'issue_207' into 'master'
      Merge remote-tracking branch 'origin/master' into navfilter
      Fix NavFilterMgr::finalize which was failing to copy data out
      Add preconditions documentation to LNAV filters.     Add time stamp to NavFilterKey and use it in LNavCrossSourceFilter.     Properly accept and reject messages in LNavCrossSourceFilter.     Add ability to accept/reject nav messages in bulk.     Properly handle rejects in NavFilterMgr::finalize.     Much more documentation and additional examples.
      Add an ephemeris aggregating filter and test it
      Merge branch 'navfilter' into 'master'
      Fix includes for cross-platform usability
      Merge branch 'navfilter' into 'master'
      Doxygen overhaul part 1
      Doxygen overhaul part 2
      Allow indpendent execution of doxygen (i.e. outside of build.sh) and tweak formatting
      Doxygen overhaul part 3
      Doxygen overhaul part 4
      Doxygen overhaul part 5
      Doxygen overhaul part 6
      Doxygen overhaul part 7
      Doxygen overhaul part 8
      Doxygen overhaul part 9
      Change Doxygen time group label
      Doxygen overhaul part 10
      First pass at a modular nav data filter
      Fix some bugs that arise from using combined nav filters
      Remove debug output
      Correct a comment
      Doxygen stuff
      more doxygen
      Add NavFilterMgr example
      Add a minimalistic pseudo-code example
      Fix doxygen formatting issue
      Doxygen-ify comments and some code reformatting
      Fix up example
      Turn off most graphs by default
      Remove waitLength(), add finalize(), implement LNavCrossSourceFilter
      First cut at testing multi-epoch filters and finalize()
      Fix NavFilterMgr::finalize which was failing to copy data out
      Add preconditions documentation to LNAV filters.     Add time stamp to NavFilterKey and use it in LNavCrossSourceFilter.     Properly accept and reject messages in LNavCrossSourceFilter.     Add ability to accept/reject nav messages in bulk.     Properly handle rejects in NavFilterMgr::finalize.     Much more documentation and additional examples.
      Add an ephemeris aggregating filter and test it
      Fix includes for cross-platform usability
      Use psrinfo instead of kstat to get core count in solaris
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      add a newline to the end of TabularSatStore.hpp
      add extern "C" to callback functions to eliminate warning messages
      Remove resolved @todo item/comment
      Fix template name resolution issue
      Merge branch 'issue_230' into 'master'
      Changed xorChecksum to not use host word types
      clarify return value comment
      Merge remote-tracking branch 'origin/master' into issue_233
      Add some NMCT validity time methods to EngNav
      Modify NMCT functions per conversations with Brent and add tests
      Merge remote-tracking branch 'origin/master' into navfilter
      Export time of transmission in getNMCTValidity and add a wrapper class for all this time data
      Merge branch 'navfilter' into 'master'
      Generally it's a good idea to initialize data members of a class in its constructor
      Add namespace scope the lack of which was breaking solaris builds
      Why does only solaris studio require namespace for find? Why do none of the compilers require namespace for copy() and inserter()?
      Move navfilter code into core
      Merge branch 'navfilter' into 'master'
      clean up RinexNav and add more test macros
      Try to make the code readable
      Reformat EngEphemeris and fix to handle toc != toe
      Change TUCMPFILE macro so the filenames can be easily copypasted into a diff command line
      Try to make RinexNavData times match the spec
      Merge remote-tracking branch 'origin/master' into issue_247
      Initialize *all* data members of RinexNavData.     Test and fix support for negative transmission times.
      format/comment
      more optimal implementation of checkParity
      Merge branch 'issue_247' into 'master'
      reformat
      make operator==() constant
      Add range/phase std dev obs types
      Add fixes for obs ID mapping and ValidType::operator==
      Merge remote-tracking branch 'origin/master' into issue_251

renfrob (19):
      Initial commit for issue_238 changes
      Finishing up PackedNavBits_T.cpp
      Tweaking test cases
      Initial commit for new OrbData class tree and OrbSysStore
      OrbSysStore sufficient to support UTC Offset analysis
      Tweaking debug support
      Improvements to find()
      Adding UTC offset modulo leap second
      Adding more GPS almanac capability
      Refactor OrbSysStore to better support multi-GNSS
      Adding an additional dump method
      øMerge branch 'master' into issue_249
      Integrating NavID into OrbSysStore
      Adding missing return
      Merging master into branch.  Moving PackedNavBits from ext to core
      Adding operator< to PackedNavBits
      Removing obsolete tests
      Add new findList() method
      Fixed incorrect comment


      #################################################

Version 2.8.1   Wednesday, January 27, 2016

   General modifications
   ---------------------
   - Patch for MS VC bug fix and code cleanup

   Modifications by Author
   ---------------------

Brian Tolman (3):
      move function definitions into a cpp file
      fix for MS VC 11 2012
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk

Bryan Parsons (1):
      GPSTk v2.8.1 Patch

Jessica Rosenquest (12):
      Updated SVNumXRef to reflect change from CommonTime to TimeRange.
      update CommonTime to TimeRange
      Initial working version of test program.
      Updated isConsistent() test to incorporate TimeRange.
      Added test for NAVSTARIDAvailable.
      killing xSVNumXRef.cpp
      New Program SVNumXRefDUMP
      deleting previous overlap test, adding tests for all functions in SVNumXRef.cpp
      complete test program for SVNumXRef
      Modified whitespace, removed redundant output, and added Copyright/ARL statements.
      Fixing Jon's nuisance errors
      Merge branch 'master' into my_br_issue

Jon Little (9):
      Making swig not complain about nested/unknown classes
      Now builds swig w/o any warnings
      Better error reporting
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Removing some debug output
      Merge branch 'master' into jessie_build
      Added couple files to work get to compile w/o errors
      Merge branch 'master' into jessie_build
      Merge branch 'my_br_issue' into 'master'

rosenquest (1):
      Merge branch 'jessie_build' into 'master'


      #################################################

Version 2.8   Friday, January 15, 2016

   General modifications
   ---------------------
   - Significant progress resolving failed tests on Linux, Solaris, Redhat, and OSX
   - New and updated unit tests for Core code base
   - Additional test programs and scripts for application testing
   - Some applications migrated from Ext to Core
   - Application testing for Core applications
   - Fixes to eliminate data corruption during encode/decode
   - Build script updates
   - CDash Scripts removed from repository
   - Updated in-source documentation
   - Code and old test clean-up
   - This release includes new/modified source code comments for improved readability.
   - Various Bug and Compile Warning Fixes

   Modifications by Author
   ---------------------
Anthony Hughes (6):
      Added unit tests for all CommandOption classes - all but 3 are passing.
      Merge branch 'master' into issue_107_commandoption_tests
      Improved parameter checking in addOption(); check results from whichOne().
      Tweaked regular expression for source code that is ignored during packaging.
      Fixed file modes - removed executable flag (755 -> 644).
      Tried to remove undefined behaviors in printable(); fixed a corner case.

Audric Terry (1):
      Minor update to SVNumXRef.cpp to include the recent launching of SV73/PRN10.

Brian Tolman (2):
      increase array length in incomplete gamma; chi squared now good to 12000 dof
      bugs in TwoSampleStats and rstats

Bryan Parsons (10):
      VS 2015 compiler flags added, VS 2011 compiler flags removed from root CMakeLists.txt.
      Bug fixes to build and test build errors for VS2015 support.
      Merge branch 'master' into ID1_VS2015_Support
      Merge branch 'ID1_VS2015_Support' into 'master'
      Updated Comments in BuildSetup.cmake. Test push for CDash Continuous builds.
      Added <cmath> to Rinex3NavHeader for fabs (absolute value) function support.
      Migrated timeconvert and poscvt from /ext to /core.  Migrated testfailexp.cmake, testhelp.cmake, and testsuccexp.cmake from /ext to /core.
      Migrated RinDump, PRSolve to core with initial tests.  Migrated Rinex3EphemerisStore, Namelist, GloEphemerisStore, and PRSolution to core, tests still need to be added.
      Added preliminary .ctest script for RinDump test comparison utilizing df_diff.  Added data for PRSolve tests.
      Added new .cmake test files for PRSolve and RinDump.  New testsuccdiff.cmake that utilizes df_diff program for comparison.  Updated df_diff to incorporate omitting eof lines.

Jon C. Little (38):
      Updating documentation and adding how to test apps.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      trying to get the C++ to render correctly.
      Wordsmithing testing docs.
      Killing DOS EOLs
      Fixing some compile warnings under clang
      Merge branch 'issue_208_unit_test_fixes' of repositories.arlut.utexas.edu:sgl/gpstk into issue_208_unit_test_fixes
      Fixing path expanstion to use custom install paths
      Merge remote-tracking branch 'refs/remotes/origin/issue_208_unit_test_fixes' into issue_208_unit_test_fixes
      Merge branch 'master' into issue_208_unit_test_fixes
      Stopping reading on eof and not just throwing an exception
      Fixing a scope issue that was casuing a warning under clang
      Making the df_test an install target
      Refactoring and fixing the Householder tests
      Merge branch 'issue_208_unit_test_fixes' of repositories.arlut.utexas.edu:sgl/gpstk into issue_208_unit_test_fixes
      Fixing lots of warnings under clang
      Merge branch 'issue_208_unit_test_fixes' of repositories.arlut.utexas.edu:sgl/gpstk into issue_208_unit_test_fixes
      Cholesky tests now working
      Now a working SVD test!
      Now a working SVD test!
      Merge branch 'issue_208_unit_test_fixes' of repositories.arlut.utexas.edu:sgl/gpstk into issue_208_unit_test_fixes
      Fixed a bug in the Load function.
      Refactored a little bit of TestUtils and rewrote Stats_T
      Removing dependancy on TestUtil from SVD_T.cpp
      Merge remote-tracking branch 'origin/master' into issue_208_unit_test_fixes
      Now working after BT fixed the underlying class.
      Changing order of operations to cause a segfault.
      Adding back in test for capacity
      Fixing test for capacity
      Removing debug output
      Allowing rinex2 files with extra spaces at the end of the lines past 80 characters
      Partial rewrite of Rine3Obs_T to teset better.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Fixed and incorret index into consts array
      Added better debugging under clang/OS X
      Fixing bug in SP3 data lookup.
      Removing execute bit on new files

Jon Little (33):
      Merge branch 'issue_204' into 'master'
      Merge branch 'issue_155' into 'master'
      Build directory should not be borked when on a tag or off a branch
      Forgot to remove a debugging code
      Merge branch 'issue_205' into 'master'
      Merge branch 'issue_33' into 'master'
      Merge branch 'issue_14' into 'master'
      Adding in namespaces specifiers to make gcc happy.
      Merge branch 'issue_34' into 'master'
      Still working on formatting
      This discusses more than library testing now...
      Merge branch 'issue_215' into 'master'
      Fixing a namespace specifier to make swig happy
      Merge remote-tracking branch 'origin/issue_208_unit_test_fixes' into issue_208_unit_test_fixes
      Merge branch 'master' into issue_208_unit_test_fixes
      Merge branch 'issue_208_unit_test_fixes' into 'master'
      App to help difference data files
      Merge branch 'issue_208_unit_test_fixes' of repositories.arlut.utexas.edu:sgl/gpstk into issue_208_unit_test_fixes
      was using the wrong abs
      Removed throwing of exceptions on errors in reading
      Adding some needed includes
      Adding types to Matrix_SVD_T
      Forgot a program...
      Merge branch 'issue_208_unit_test_fixes' of repositories.arlut.utexas.edu:sgl/gpstk into issue_208_unit_test_fixes
      Fixed bug in SVD tests.
      Removing a file used to help debug SVD tests. All tests are in Matrix_SVD_T
      Forgot to remove building of SVD_T from CMakeLists on previous commit
      Removing non-sensical accessor and assoicated test
      Removing old test files
      Rewriting test to use new macros.
      removing old files and attrs
      removing old files and attrs
      Merge branch 'issue_224' into 'master'

Joseph Voss (3):
      Setting CommonTime objects from RinexNavData to have a default time system of GP
      Uncommenting the fix to issue 155
      Minor fix the the Matrix SVD test

anthony (1):
      Merge branch 'issue_107_commandoption_tests' into 'master'

johnk (130):
      commented out switch causing build.sh to ignore test results
      removed unused overloaded initialize method
      Modify setFromInfo y token to match POSIX
      Reformatted to remove tab characters.     Fixed copy-pasta issues.     Fixed test that used setInternal with seconds instead of milliseconds.
      Fix incorrect formatting in printError.     Fix test that was erroneously using the wrong format spec.     Reformat the GPSWeekSecond_T to remove tabs.
      Distinguish between month abbreviation and full name when scanning.     Add translation method for month names and abbreviations.     Allow a 29-bit zcount to be specified with a full week.     Fix some memory leaks.     Fix 2-digit year support.     Fix issue where additional specs (e.g. time system) were being ignored.     Reformat TimeString_T.cpp.
      Reformatting
      Process %y in POSIX-compatible fashion
      Fixed test expectations for inSameTimeBlock
      Throw exception for front/back on empty data vector
      Merge branch 'issue_49' into 'master'
      Merge remote-tracking branch 'origin/master' into issue_208_unit_test_fixes
      Rewrite FileSpec::sortList to work as intended.     Make TimeTag verify that the entire format string has been properly parsed.     Comment out (with @todo) tests for unimplemented FileSpec features.     Added additional tests using GPSWeekZcount in lieu of SOW capability (precision).
      Make sure a test that is expected to fail has a reasonable chance of failing
      Throw an exception when setFilter is called with an empty filter.     Rewrite FileHunter::find which was relying on DayTime behavior of throwing exceptions for incomplete time specs.     Throw an exception when FileHunter is constructed with empty file spec.     Fix FileHunter_T to not fail if a test directory already exists.     Correct expected exception class in FileHunter_T tests.     Correct issues with FileHunter_T tests not matching file specs and file names.
      Comment out unimplemented preciseRho test
      Fix what looks to have been a copypasta error (copying from meters2cyclesTest to cycles2metersTest)
      Change CRC24Q definition so that the final XOR value is 24-bits.     Change the documentation for computeCRC to make it clear len is in bytes.     Replace the existing CRC tests with a reasonable set.
      EngAlmanac_T seems to have been incorrect as to what a correct value for WNLSF was (see @todo)
      removed some debug output
      Move TFASSERT macro into TestUtil.hpp
      Make sure the text used for comparison is of the same format before and after
      Reformat for readability
      Changed one test to use data comparison rather than dumped strings
      Add supported version checking when reading RINEX MET headers.     Fix dump method to use supplied ostream instead of cout.     Correct names of test input files.     Turn on stream exceptions where expected.
      Fix issues with header formatting and testing thereof.
      Reformat/untabify for readability.
      Use macros in a few places
      Fix attempts to use EOF as genuine data in tests
      Fix attempts to use EOF as genuine data in tests
      Merge remote-tracking branch 'origin/master' into issue_208_unit_test_fixes
      Try to make rmwdiff look like it did before CommonTime
      Add a definition for the data path for testing
      First cut set of tests for rmwdiff app
      Add a couple more tests for rmwdiff
      Add tests for timeconvert and tweak it so the output matches version 1.6
      Add tests for rnwdiff.     Add stable methods for text representation of RINEX MET and NAV.
      Initialize Triple values to zero
      Add a compare function for Rinex3ObsHeader to check for differences between headers.     Change RinEdit to exit with 0 on success rather than number of files, which looks like an error.     Started a tool for comparing RINEX headers.     Started a RinEdit test.
      Add exit codes to distinguish between help and option errors
      Merge branch 'issue_216' into 'master'
      Merge remote-tracking branch 'origin/master' into issue_208_unit_test_fixes
      Change rinheaddiff to use updated exit code stuff
      Modify r*wdiff to use frameworks exit codes.     Fix up the RinEdit tests.     Add rowdiff tests.
      Add RINEX 3 obs tests for rinheaddiff
      add rinheaddiff tests for RINEX 2 OBS
      Merge remote-tracking branch 'origin/issue_208_Brent' into issue_208_unit_test_fixes
      Fixed cmake syntax issues causing tests to incorrectly pass
      Merge remote-tracking branch 'origin/master' into issue_208_unit_test_fixes
      add tests for poscvt
      Merge branch 'issue_208_unit_test_fixes' of repositories.arlut.utexas.edu:sgl/gpstk into issue_208_unit_test_fixes
      tiniest of beginings for mpsolve testing
      Add support for RINEX nav and met to rinheaddiff.     Add tests for RINEX merge tools.
      Untabify
      Split RinexDatum into its own files instead of having the exact same     class appear in both RinexObsData and Rinex3ObsData.     Put parsing and encoding code into RinexDatum and take it out of Rinex3ObsData.     Include the ability to preserve headers across read/write.     Preserve spaces in data across read/write.     Add a round-trip test in Rinex3Obs_T.
      Bunch of changes to try to make Rinex3ObsHeader code more readable.     Make sure all data members get initialized in constructor and clear().     Add documentation of all the header fields and what-not.     Minor changes to test input, reordering header lines to match output.
      Merge remote-tracking branch 'origin/master' into issue_208_unit_test_fixes
      Merge remote-tracking branch 'remotes/origin/issue_212' into issue_208_unit_test_fixes
      Add comments that explain what headerExceptionTest is actually doing, and do it.     Comment out "tests" of unimplemented features.
      Comment out testing of unimplemented version conversion feature
      Update Rinex3ObsHeader documentation.     Comment out most of the Rinex3Clock_T tests as Rinex3Clock* is largely unimplemented.
      Add RinexClock* tests and clean up some of the other rinex tests
      Replace unimplemented RINEX3 clock with partially implemented from RINEX directory
      Migrate project RINEX clock implementation into the gpstk
      Migrate project RINEX clock implementation into the gpstk
      Minor fixes to RINEX clock handling and new tests
      Remove invalid tests
      Make test macro names consistent.     Comment out unimplemented AlmOrbit_T::svXvtTest and update tests to use macros.
      Change Rinex3ObsHeader to not reset PisY flag.     Add ability to do file compares in TestUtil with regex ignore.     Make RinSum testable.     Add RinSum test for --ycode option.
      Merge branch 'issue_208_unit_test_fixes' of repositories.arlut.utexas.edu:sgl/gpstk into issue_208_unit_test_fixes
      add calgps tests
      Create tests for bc2sp3
      Specify input files without option and update tests
      Process exceptions thrown by CommandOptionParser in BasicFramework.     Add two more space-embedded arguments to testsuccexp.cmake.
      Use shorter variable names
      Merge branch 'issue_208_unit_test_fixes' of repositories.arlut.utexas.edu:sgl/gpstk into issue_208_unit_test_fixes
      remove deprecated config macros
      clean up TestUtil
      Clean up TestUtil and TimeRange_T
      Add binary file comparison to TestUtil
      Add missing test file that broken FFStream obscured
      Add file error exit codes
      Use init methods in streams to avoid issues with virtual methods in constructors.     Also a fair bit of beautification.
      Update core stream classes to avoid virtualization issues
      Reformatting and fixing inappropriate use of doxygen @defgroup
      Never ever put using statements in include files
      Fix namespace issues that show up on linux but not solaris
      Kludge because g++ raises exceptions in tellg() when eofbit is set.  Also fix some related test issues.
      Clear output stream status bits between tests
      remove tests of twiddle using floating point types, and add a long long test
      Make sure seconds-of-day are printed out in fixed notation
      Don't attempt to write to a closed stream
      Rename variables that are conflicting with precompiler macros
      namespace scoping issue for gcc on solaris
      Test that breaks encodeVar under solaris
      Resolve type ambiguity
      Portable type casting
      Reimplement encodeVar/decodeVar to avoid data corruption when using floating point types
      Make Matrix tests compile under solaris
      Exception class tests
      Fix decodeVar bug
      Replace dodgy byte swapping functions in BinUtils with less dodgy ones
      Reformatting
      Reformatting
      Reformatting
      Add little-endian decode methods.  Remove twiddle tests.
      Update BINEX support to use new BinUtils functions
      Change subframe word type from environment-dependent 'long' to consistent 'uint32_t'
      Mild reformatting, and add implementation for parent abstract method
      Incomplete updates to Novatel support using new BinUtils
      Remove old Novatel code
      Add some missing functions for binary encode/decode
      Fix some bugs in BinUtils and write tests that exercise both BinUtils and FFBinaryStream
      Merge remote-tracking branch 'origin/issue_208_unit_test_fixes' into issue_224
      Change a couple static_cast to reinterpret to make gcc shut up.  Use const where appropriate in endian translations.
      Merge remote-tracking branch 'origin/issue_208_unit_test_fixes' into issue_224
      Fix a typo
      Merge remote-tracking branch 'origin/issue_208_unit_test_fixes' into issue_224
      Ignore RinDump output that includes file names
      Merge remote-tracking branch 'origin/issue_208_unit_test_fixes' into issue_224
      Merge branch 'issue_224' into 'master'
      Merge remote-tracking branch 'origin/master' into issue_221
      Change minor release to 8
      Rename BinUtils functions to avoid problems with compilers/systems that use compiler macros to define byte swapping functions
      Test everything that's in BinUtils
      Allow for different quantization when translating ireg to SNR
      Merge remote-tracking branch 'origin/master' into issue_226
      Merge remote-tracking branch 'origin/master' into issue_221
      Merge branch 'issue_221' into 'master'
      Merge branch 'issue_226' into 'master'

renfrob (11):
      Changes to OrbElemStore to allow multiple orbit/clock with same epoch time
      Updates to Rinex v2.11 to v3 obs type mappings
      Extending RinSum to provide option to assuming P means Y
      Adding Galileo and SBAS obs type conversions
      Changes to address issue 215
      Fixing TimeRange test and merge conflicst
      Changes to allow branch to build on Linux
      Updated truth files to support RinexEphemerisStore_T
      Changes to make RinexEphemerisStore_T work correctly.
      More cleaning of RinexEphemerisStore_T
      Fixing weekrollover test issue in RinexNavData


      #################################################

Version 2.7   Friday, October 16, 2015

   General modifications
   ---------------------
   - Supported Platform updated
      - Solaris Sparc no longer supported
      - XCode no longer supported
   - Substantial Clean-up and Updates to build.sh script
   - Python bindings now work under Linux, Solaris, & Mac
   - CTest Infrastructure Complete - all platforms
      - C++ test code tested and works on all platforms
      - Linux, Solaris, OSx , Windows (VS2012)
      - This include tests for library
   - CDash Scripts added to repository
   - More Unit Tests code added for Core
   - Bug fixes for RINEX tools
   - Sinex Bug Fix for Solaris x86
   - SVNumXRef moved from /ext/lib/Misc to /core/lig/GNSSCore
   - Additional in-source documentation
   - This release includes new/modified source code comments for improved readability.
   - Various Bug and Compile Warning Fixes

   Modifications by Author
   ---------------------
Anthony Hughes (14):
      Ensure that SinexBlock contents are subclasses of Sinex::DataType.
      Updated BINEX classes with latest MSN changes: typedefs, bugfixes, and tweeks.
      Added GPSTK_ASSERT macro.
      Added CommandOptionWithDecimalArg.
      Added const versions of encodeVar and decodeVar; added some inline directives; fixed a decodeVar unit test error.
      Added back memory.h after accidentally removing it.
      Made getFitInterval static; added isValid and isDataSet.
      Removed broken arbitrary location encodeVar().
      Added public accessors for ObsID.
      Added CommandOptionNof.
      Added an index parameter to CommandOption::getOrder; changed 'order' attribute to a vector.
      Added an edit method to IonoModelStore.
      Added utility methods to EngNav: sv2page(), sfpage2svid(), and zcount2page().
      Added comparison operators for GPSWeekSecond to guide implicit type conversions.

Audric Terry (6):
      Updating SVNumXRef
      Moved SVNumXRef from gpstk/ext/ to gpstk/core/ and added its test program to gpstk/core/tests/GNSSCore/.
      Updated SVNumXRef.cpp to include SVN72/PRN08 relationship and changed a comment in xSVumXRef.cpp
      Added end date to SVN40/PRN10 in SVNumXRef.cpp
      fixed conditional statement
      Updated SVNumXRef.cpp to include SVN36/PRN10 relationship.

Brian Tolman (24):
      fix bug introduced in previous commit
      add leap second
      Add sparse matrix package to Geomatics, add diagDopy to Matrix, small changes
      fix for windows
      fix for windows
      small change to increase efficiency
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      avoid duplicate file name
      refine sparse matrix classes and add test program
      remove until a windows version works
      subtle bug arises when no reference position supplied
      fix and rename strip() that respects quotes
      fix test for splitWithQuotes
      handle repeated empty fields in splitWithQuotes
      Fixed bug introduced by changing int to size_t everywhere
      Add option to find millisecond clock adjusts
      bug when string-without-trailer is shorter than trailer
      update and repair several things in RINEX tools
      not handling R ver.2 obs types correctly in warnings
      another bug
      add option to output in TEC units
      add some functionality
      bugs in decimate (--TN) and delete sats (--DS)
      bug fix in inverseUT() for 1x1 matrix, other small additions

Bryan Parsons (14):
      Merge branch 'RC26' into 'master'
      Merge branch 'SVNumXRef' into 'master'
      Merge branch 'Rinex_Merger' into 'master'
      Added initial files for CDash support
      Merging CDash Integration feature branch into 'master'
      Tweaks to Mac CDash scripts.
      Tweaking Linux and Solaris nightly CDash scripts.
      Removing CDash scripts from the GPSTk repository.
      Merge branch 'issue_201_removescripts' into 'master'
      Spelling Tweaks in build.sh.
      Updated documents with new refactored build scripts and for RC27 release.
      Resolving merge issues.
      Minor tweak to setup.py.in to match new documentation name.
      Merge branch 'issue_197_refactor_build' into 'master'

Jason Vestuto (8):
      Added test for zero input args to build.sh, and if found, call the usage() fucntion to print the help to the screen.
      Updates to force building gpstk/ext when the python bindings are built.
      Updates to comments for Python build configuration files.
      Fixed PYTHON_VERSION variable names for custom python build config.
      Custom python build config changes
      SWIG, minor bug fix to doc.i created by CMake that caused a compiler error on OSX.
      Merge branch 'master' into issue_121
      CMake Python Config, python version changed to match that of the shared test host.

Jon C. Little (2):
      So we can ignore all build directotries...
      Still getting the hostname right on all platforms

Jon Little (24):
      Better diagnostics for the config mode
      Code now compiles under solaris studio 12.4 with --std=c++11 option
      Letting the default C++ ABI be used by CC.     This can be overridden by specifying -DCMAKE_CXX_FLAGS=-std=c++11 on the cmake invocation
      Merge branch 'master' into issue_185_c++11
      Fixing the test apps
      Removing depricated tests for the MSC class
      Fixing bugs found by clang
      Merge remote-tracking branch 'origin' into issue_185_c++11
      Merge branch 'master' into issue_185_c++11
      Merge branch 'issue_185_c++11' into 'master'
      First cut at making complete out of tree build
      Now build through packages on Linux
      Now builds and bundles on Darwin
      Fixed problem with finding hostname under solaris
      Now can do a joint build with sgltk
      Updated PRSolve and Rinex3 support to allow the user to specify P code data is really Y code data.
      Added back some reporting on tests
      Making ctest work on multiple platforms.
      Merge remote-tracking branch 'origin/master' into issue_197_refactor_build
      Fixing the var for the python executable. I really don't know how this worked before :-/
      Merge remote-tracking branch 'origin/issue_197_refactor_build' into issue_197_refactor_build
      Fixing problem with building the python package.
      Merge remote-tracking branch 'origin' into issue_197_refactor_build
      Merge remote-tracking branch 'origin/issue_197_refactor_build' into issue_197_refactor_build

Joseph Voss (35):
      Adding EngAlmanac Test     - Uses classes from AlmanacDataGenerator to parse gps almanac files, creates comparison data and generates subframes for testing     - Other subframes necessary are generated by hand, see comments in EngAlmanac_T     - Debugging getUTC method, returns certain values out of range
      Corrected binary sv id for subframe 4 page 25 which prevented the page from being stored
      Refactored the get method tests
      Finishing EngAlmanac_T     - Conversion of WNlsf from the binary subframe to an integer value is     incorrect due to an error in EngNav's convertXbit method. This method     implicitly signs the values passed to it, causing an error for unsigned values     - Conversion of DN from bthe binary subframe to an integer value is     incorrect. The IS-GPS-200D states that the DN value is subframe 4 page 18     is right justified, but the gpstk treats it as left-justified     - Resolved error in EngAlmanac_T UTC parameters     - Closes 95
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Minor fixes to EngAlmanac test, DN test now passes
      Initial commit of ObsRngDev Test     - Working on initialization of ObsRngDev Object using Rinex Ephemeris Store     - Using Rinex file stream for generator of ObsRngDev pseudorange data     - Need to use Rinex to get receiver position and CommonTime object
      Generated all the data necessary to construct the ORDS, but need new data files
      Successfully generated ObsRngDev objects, completed basic test     - RinexNavData time system set to GPS by default, bad. How else can time system be set?
      Successfully created one IonoModel object using EngAlmanac, but too much overhead. Rewriting to generate IonoModelStore entirely by hand
      Factoring tests into fail counters, 91 tests count as one     - Added basic initialization tests for single frequency constructors     - Completed get function testing
      Decided to remove the RinexObs dependencies and generate Ords by hand     Ran into invalid time system issues, can't find where the error is, so making test much simpler     Committing to backup the existing test before the purge of RinexObs
      Refactoring
      Added calculation testing for every instance of ObsRngDev
      Merge branch 'issue_89' into 'master'
      Resetting a library file that was editted for testing
      Fixed the way that floats are compared in ObsRngDev_T
      Minor fixes to Matrix_SVD test (on the right branch this time)
      Replaced CPPUNIT with TestUtil for ReferenceFrame_T     - closes #73
      Merging Rinex3ClockData      - Implemented dump member function      - Implemented reallyPutRecord member function      - Uses RinexSatID to support GNSS
      Updating Rinex3ClockData with the RinexClockData public members
      Initial commit of Rinex3Obs_T and all depending test files
      Commenting out modifier function in Rinex3ClockData that broke the build
      Fixed error within the NumberHeaderRecordsToBeWritten
      Initial commit of Rinex3Nav_T and all dependent test files
      Reverting back to current GPSTk Rinex3Clock class for testing
      Initial commit of Rinex3Clock_T and all dependent test data files
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk into Rinex_Merger
      Minor changes to fix failing tests
      Added Rinex version conversion tests to Rinex3Obs_T & Rinex3Nav_T
      Merge branch 'issue_73'
      Replaced hard-coded file seperator with a cross-platform call to getFileSep
      Updated all RINEX tests to use a platform independent file seperator
      Minor fix to errors that broke the build
      Removing test from CMakeFile that's not in the GPSTK repo

Josh King (1):
      SVNumXRef updates

Kevin Kraatz (3):
      set firstObs TimeSytem to VERSION/TYPE if unset
      add header edit for REC #/TYPE/VERS and ANT #
      Fix writing of marker number

Nick Fitzsimmons (1):
      Update to CommonTime_T comments.

Scot Nelsen (4):
      Standardize variable names and versions numbering used in CMake build generation.
      Add ext binaries to the list of targets in the gpstkTargets file.
      Rework PythonSetup.cmake to import Python config through either system or custom config.
      Remove extra python configurationfrom swig/CMakeLists.txt and ensure Python Interp and Libs versions match.

Scotland Nelsen (4):
      Standardize on "GPSTK" as the CMake project name.
      Missed a usage of projectConfig.cmake.in in the CMakeLists.txt file.
      Merge branch 'iss43'
      Update swig/CMakeLists.txt to use non-deprecated python paths.

Steve Johnson (1):
      Rename 'small' and 'big' variables to avoid conflict with Qt when building on Windows

anthony (11):
      Merge branch 'issue_180_binex' into 'master'
      Merge branch 'issue_181_assert' into 'master'
      Merge branch 'issue_182_decimalarg' into 'master'
      Merge branch 'issue_183_binutils' into 'master'
      Merge branch 'issue_184_engeph' into 'master'
      Merge branch 'issue_187_getObsID' into 'master'
      Merge branch 'issue_188_CommandOptionNof' into 'master'
      Merge branch 'issue_189_getOrder' into 'master'
      Merge branch 'issue_191' into 'master'
      Merge branch 'issue_192_engnav' into 'master'
      Merge branch 'issue_195_GPSWeekSecond_Comparison' into 'master'

aterry (1):
      Merge branch 'svnumref' into 'master'

kolensky (1):
      Merge branch 'issue_198_prsolve_rinex2' into 'master'

renfrob (16):
      Added flag to PackedNavBits - Fully backward compatible
      Extending OrbElem class hierarchy to cover GLONASS
      Adding isSameData() method to OrbElemBase
      Refactor OrbElemStore classes for greater commonality
      Adding receiver ID option to PackedNavBits
      Polymorphic behavior requires pointers.
      Fix to addPackedBit( ) method.
      Adding Rinex3Nav output options
      Adding Sign & Mag capability to PackedNavBits
      Adding OrbElemStore.getSatList()
      Add method to override fit interval limit
      Tweak boundary limits
      Changes to support development of sgltk/apps/filetools/rnwcompare
      Adding an overlooked modification
      Fixing missing comma in CNAV raw output
      Tweak to dump() output format.

unknown (1):
      fixes for windows

vestuto (2):
      Merge branch 'issue_121' into 'master'
      Merge branch 'issue_185_c++11' into 'master'





	#################################################



Version 2.6   Tuesday, May 12, 2015

   General modifications
   ---------------------
   - CTest Infrastructure Complete - Unix, Linux, Mac, Xcode
   - C++ & Python test code tested and works - Linux Only
   - Suite of Unit-Tests added for Core
   - Some Code Migrated from Ext to Core
   - Updated build script
      - Python Install
      - Build Source and Binary Tarball
   - This release includes new/modified source code comments for improved readability.
   - Various Bug and Compile Warning Fixes 

   Modifications by Author
   ---------------------
Adam Loggins (1):
      fixing bug for updating markerNumber in the RinexHeader update in ExecuteEditCmd

Andrew Joplin (3):
      STLTemplates.i: several ObsID templates
      STLTemplates.i: added wrapping for ValidType types
      Install codeDump

Andrew Kuck (1):
      Added a new example script demonstrating RINEX Observation File writing.

Anthony Hughes (40):
      Refactored the BINEX types tests to use the TestUtils stuff and to read test input from files.
      Updated some comments.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Added filters for build output and for OS/X file system droppings.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Refactored more BINEX tests to use TestUtils.
      Ignore Sublime Text temp files
      Adjusted (downward) test reporting granularity, avoided using of assert(), added fail text.
      Added a fail message parameter to assert() which defaults to "none".  Pass constant strings by reference.  Improved code formatting consistency.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Improved test framework usage and test failure output; did code cleanup.
      Refactored in response to TestUtil refactor.
      Refactored in response to TestUtil refactoring.
      Refactored in response to TestUtil refactoring.
      Refactored in response to TestUtil refactoring.
      Updated test count logic for Update/Extract primitive tests.
      Fixed backwards logic in Binex_Types tests and tweaked more test count logic in UpdateExtract.
      Added tests for FileSpec.
      Added FileUtils tests.
      Fixed some error handling issues in setFilter() when file spec type is invalid or missing.
      Added FileHunter test category.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Added FileHunter tests.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Test data for the FileHunter tests.
      Changed FileHunter test setup to generate expected files and directories; tweaks.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Added FileFilter tests.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Fixed undefined behaviour in unique() when operating on an empty data set.  Fixed some distracting code formatting issues.
      Added FileFilter tests and fixed some FileHunter test typos.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Added a missing include for std::map.
      Added tests for FileStore.
      Removed deprecated BINEX tests.

Brian Tolman (6):
      Small changes, and move Namelist to Geomatics with SRI
      Improve robustness of inversion, plus small fixes
      Fix a bug and add 3 functions useful for eclipsing satellites
      restore Namelist in lib/Geomatics
      small addition
      update version number

Bryan Parsons (40):
      Submitted by Brian Tolman - Fix for Clau specific PRSolve Bug. Problem - SPS algorithm in PRSolution.cpp in lib/Geomatics failed to converge due to garbage in the matrix problem.
      Submitted by Brian Tolman - PRSplot.pl update
      RefTime/TimeHandling Lib folders have migrated from Ext to Core.  First round of Unit Tests have been pulled from /tets folder for usage.
      Merge RefTime/TimeHandling migration to UpStream Master changes.
      Targetted MSN Merger Classes, with tests, have migrated from /ext and /tests to /core and /core/tests
      Current tests have been consolidated, from Nick and /tests folder, into the current test framework.  Placeholder tests have been added and designated with an *, with some tests still needing attention due to build issues.
      TimeTag class bug fix - Nick.  Ext -> Core file movement.
      Pull down from gitlab repository.
      Updated core test class names, updated ANSITime_T for new UnitTest class.
      Moved Clocktools apps from SGLTk to GPSTk
      Updated CivilTime_T to utilize new TestUtils class, as well as minor fixes.
      Additions to CivilTime
      Removed SMODF files from the GPSTk.  Added TimeTag_T test placeholder.
      Changes made to CommonTime_T tests.
      Added/Removed test Placeholders to match updated chart, cleaned up *'s
      Resolving merge conflict from upstream changes.
      Removing BDSEphemeris test placeholder, as it was added by mistake.
      Removed BDSEphemeris Test
      Merging Upstream changes to local branch.
      Hide build/ directory from git. External submission by Github user jonyrock of Research & Engineering Center of Saint-Petersburg Electrotechnical University (REC-SPb-ETU)
      Updated minimum required version of CMake. External submission by Github user jonyrock of REC-SPb-ETU.
      Removed all commented out add_test() commands from ext app CMakeLists.
      Per #144 - Added two new options to the build.sh script: -g for building binary Gzip tarball and -s for building source Gzip tarball.  BuildSetup.cmake, INSTALL.txt, CMakeLists.txt also updated for this change.
      Per #146 - Added Testing document to root GPSTk.
      Merge upstream changes on master to RC26 feature branch.
      GPSTk Documentation update for RC v2.6
      Minor fixes for tests that failed or passed warnings on Mac and Windows.
      Hide build/ directory from git. External submission by Github user jonyrock of Research & Engineering Center of Saint-Petersburg Electrotechnical University (REC-SPb-ETU)
      Updated minimum required version of CMake. External submission by Github user jonyrock of REC-SPb-ETU.
      Removed all commented out add_test() commands from ext app CMakeLists.
      Per #144 - Added two new options to the build.sh script: -g for building binary Gzip tarball and -s for building source Gzip tarball.  BuildSetup.cmake, INSTALL.txt, CMakeLists.txt also updated for this change.
      Per #146 - Added Testing document to root GPSTk.
      GPSTk Documentation update for RC v2.6
      Minor fixes for tests that failed or passed warnings on Mac and Windows.
      Additional minor fixes for warnings and errors under Windows Visual Studio 2012.
      Resolving merge conflict, pushing out to feature branch RC26
      Added Python documentation to source tree.  Additional tweaks to other documentation in preparation for RC2.6.
      Cleaning up unused variables and semantics in /Core tests.
      Resolving merge conflicts when pushing up to feature branch RC26.
      Removing pragmas from migrated code flagged by Dirty Word Search.

Jason Vestuto (113):
      Fixed example so that path to needed data file is not hard-coded, but instead, determined by a module method.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Python bindings and documentation, fixed docstring generator script:     - corrected file paths to match current gpstk file tree structure.     - removed all instances of magic numbers, using os.path methods instead.     - removed linux file seperators, and used os.path.sep instead.     - improved comments for clarity.
      Python bindings, documentation, fixed copyright date in conf.py
      Overhaul of build.sh and clean up of top-level CMakeLists.txt files for C++ and Python.
      Build script, added test for source files when building /ext or /core. If *.cpp files not found, an error is printed to the screen, and the script exits.
      Build script, fixed white-space problems and ordering of documentation processing.
      Sphinx documentation, updates to the build instructions to account for recent changes to the GPSTk source file tree structure and the build.sh build script.
      Updated build.sh script:     - fixed comments about install paths.     - moved determination of install paths above echo and tests of those paths.     - changed the file names of the documentation archives for clarity.
      Build script, replaced tabs with spaces.
      Decoupling of build and install processes:     - modified build.sh, adding new input flag "-b" to do "build only, no install"     - modified build.sh, adding an explicity $build_root variable     - modified build.sh, so that $build_root is /tmp/gpstk/build for non-user installs     - modified swig/CMakeLists.txt, changing the python package file copy commands from install targets into custom commands dependent on the swig module build target that builds the binding library _gpstk.so, so that the fiel tree gets recopied every time the library gets rebuilt.     - successfully tested separate build and install to user and systme paths on FAI Debian VM.
      Updates to INSTALL.txt and build.sh to account for recent changes to the automated build framework.
      Updated build script, adding num_threads as an input parameter.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Updates to INSTALL.txt instructions to account for recent changes.
      Initial overhaul of RINEX Obs test cases, including the following changes:
      Initial commit of TestUtil utility class for use with unit testing of GPSTk.
      Updates to Rinex_Obs_T test cases to demonstrate the use of TestUtil class.
      Cleaning up comments.
      Updates to TimeUtil class, including the following:
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Moved /tests to /ext/tests and updated /ext/CMake file accordingly.
      Removing add_test( tests ) from top-level CMake file so that the one in core is not ignored.
      Fixed constructor signature for TestUtil class, correcting const string& syntax.
      Testing, overahuls of Rinex Obs and Rinex Nav tests so as to use the new testUtil class.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Tests, refactored fileEqualTest() method out of separate Rinex file test apps, and moved it into the TestUtil class.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Fixing bug in core/tests/TimeHandling CMakeList files introduced in commit 5788d72a.
      Renamed Rinex FileHandling test files to match class names, per convention.
      Updated tests RinexObs_T and RinexNav_T, including:
      Renaming Rinex_Met_T file to RinexMet_T to follow test app naming conventions.
      RinexMetHeader class, corrected scope problem for sensorType and sensorPosType:
      RinexMet test class, first pass at complete overhaul and clean up:
      Testing, RinexMet_T class, cleanup:
      Testing, FileHandling build script, added "-Wall" flag to gcc builds.
      Testing, Rewrote Rinex_T test app to follow current working test app style conventions for GPSTk.
      Initial addition of SINEX file handling support:
      SINEX, added include of stdint.h in SinexBase.hpp:
      Testing, renamed sinex_test.cpp to Sinex_ReadWrite_T.cpp to follow GPSTk conventions.
      Sweeping away the last remnants of the old CPPUNIT republic.
      Updated build.sh to capture accurate measure of unit test results.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Initial addition of RINEX3 data files for use in automated testing.
      Testing, cleaned up implemetation of testLine in TestUtil.hpp to follow conventions followed elsewhere in the class.
      Initial addition of sinex data files for use in automated testing.
      Testing, Fixed source method name in print statement for Vector_T testing.
      Testing, renamed Rinex_Clock_T.cpp to RinexClock_T.cpp to follow GPSTk test file name conventions.
      Added capability to pass build-config-dependent variables into the C++ source:
      Build Framework, preparaing ground work for refactoring build config.
      Testing, Updates to Rinex test apps to use test data file paths
      Testing, test data file migration for Rinex tests.     - This applies the new conventions in test data naming and location     - Updated all file paths to use the new top-level path $ gpstk_root/data     - Updated all file names to follow new data file naming conventions.     - New convention is to store all data files in $gpstk_root/data rather than spread across the source tree.     - New convention for file names is 5 tokens delimited by underscores.     - Obviously, all these conventions are subject to change in the future if everyone hates them.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk     - Resolved Conflict in core/tests/FileHandling/CMakeLists.txt     - Removed all instances of "^M" that some windows user slopped into the file.     - Please use dos2unix or manually remove all instances of "^M" before future commits.     - Linux/Unix uses 0xA for a newline character. Windows uses a combination of two characters: 0xD 0xA. 0xD is the carriage return character. Most editors will display 0xD as "^M" and any file diffs on Linux will show them too.
      Updates to build_config.h.in to add C++ style methods for paths.
      Build config, made all functions inline.
      Build, added getPathBuild method, cleaned up get methods for test data and temp paths.
      Testing, update TestUtil implementation to adapt to changes to build_config.h
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Updated build framework, including addition of install prefix input and handling.
      Building, bug fixes and updates for path and log file handling in build.sh, including:     - Removed initialization of build_root, and added initialization of system and use specific build roots.     - Added logic so that the system_build_root is the default, but user_build_root is used if -u flag is used.     - Removed build_root assignment syntax from help function usage()     - Added string formatting to echoed config values     - Added more detailed treatment of build and install paths, testing for their existence, trying to create them if not present, and exiting the process entirely if they cannot be created.     - Added the use of the tee command to calls to cmake, make, make install, and ctest so that all stdout and stderr are both sent to the screen and to new log files, which are saved to the build_root path.
      Minor changes to variable names in build.sh for clairty.
      Build, bug fixes for ctest file names and path test help comment updates
      Updates to INSTALL.txt     - Updated information on using the build.sh script     - Updated information on using the CMake files directly     - Added example of how to link against libgpstk in your C++ apps
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Test Framework, Updates to TestUtil class     - added verbosity data member, with default 1     - added method setTestLine() that can accept an int or string     - modified constructor so as to strip off path from input test file name, e.g., __FILE__ returns a full path, but we don't need it     - added failMessage data member, which is reset to an empty string by next()     - added a setFailMessage() method     - overloaded setFailMessage() so that it can optionaly take a line number as a second input parameter, as either a string or an int, e.g. __LINE__     - overloaded fail() method so that it can optionally take a failMessage string and a line number, e.g. __LINE__     - overloaded failTest() method so that it can optionally take a failMessage string and a line number, e.g. __LINE__     - modified print() method to print "key=value" pairs instead of just values     - modified print() method to test for failBit and verbosity and to optionally print failMessage key=value pair     - added getTolerance() method     - changed log keyword from "TestOutput" to "GpstkTest"     - updated build.sh script to accomodate changes to test output log file
      Testing, modified RinexMet_T.cpp to demonstrate use of newly updated TestUtil class.
      Test Framework, TestUtil, fixed default values for failMessage.
      Build script, added flags to control python binding process. Default off.     - modified build.sh, changing -p flag to set build_python=True     - modified build.sh, adding -P flag to accept input python executable path     - modified build.sh, wrapping all relevant blocks with test for build_python=1     - modified build.sh, passing "-DBUILD_PYTHON=ON" to cmake     - modified CMakeLists.txt, adding an optional switch BUILD_PYTHON, default OFF     - modified CMakelists.txt, wrapping the add_subdirectory(swig) with a test for BUILD_PYTHON
      Build, Updated build.sh comments and echo/print statement text for clarity.
      Build and Test, updated build.sh to compute percent passed for test categories.
      Build, minor updates to test log parsing, additional test keywords counted.
      Testing, overhaul to TestUtil class, rewritten CommonTime_T to demonstrate use.     - The design of TestUtil was found to be internally inconsistent and confusing     - The methods were trying to follow two different approaches:       (1) small simple methods, forcing many calls per test case       (2) large methods wrapping small methods, allowing fewer calls per test case     - The problem was all were public methods, and the two design approaches were       not independent or complete, so you had to use a mixture of both.     - Decision was made to make the interface almost entirely based on a new assert()       which wraps and calls all needed smaller methods, and to change all the smaller       wrapped methods from public to private, and deleting other methods that tried       to be both large and small, represented the worst of the mixed design.     - TestUtil, assert() updated to take failMessage and lineNumber, and call next()     - TestUtil, methods deprecated/removed include the following:       init(), passTest(), failTest(), assert(bool, string="none")     - TestUtil, methods changed from public to private:       print(), pass(), fail(), next(), undo()     - Complete rewrite of CommonTime_T.cpp to both use the new TestUtil as       the new design intends, and to demonstrate preferred code style for test apps,       including use of white-space and comment block delimiters. All test descriptions       were moved to the failMessage inputs to assert() but can still be searched.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Testing, updates to TestUtil.assert() to finish off this overhaul     - removed assert(bool) signature so as to force test developers       to always give a message and line number, e.g. using __LINE__       to prevent assert(true) from givin n context and having the wrong file line     - changed provate data member failMessage to testMessage     - changed public method setFailMessage() to setTestMessage()     - changed assert() method, adding calls to setTestMessage()       and setTestLine() not just for cases where fail() is called     - Deleted comment blocks fo old deprecated methods     - updated CommonTime_T.cpp calls to assert(true) so they are       passed a testmessage and __LINE__     - updated conventions of test formatting used as test_message       passed to assert() calls to demonstrate a possible convention.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Testing, updated to Rinex_T to use new TestUtil.assert() interface.     - refactored assert message strings     - added __LINE__ input for all assert calls     - added test description string input for all assert calls     - replaced all pass(), fail(), passTest(), failTest() calls with assert() calls.
      Testing, updated to RinexNav_T to use new TestUtil.assert() interface.     - refactored assert message strings     - added __LINE__ input for all assert calls     - added test description string input for all assert calls     - replaced all pass(), fail(), passTest(), failTest() calls with assert() calls.     - removed all calls to next()     - fixed indentation and try/catch blocks
      Testing, updated to RinexObs_T to use new TestUtil.assert() interface.     - refactored assert message strings     - added __LINE__ input for all assert calls     - added test description string input for all assert calls     - replaced all pass(), fail(), passTest(), failTest() calls with assert() calls.     - removed all calls to next()     - fixed indentation and try/catch blocks
      Build, reordered add_subdirectory() directives so that the stuff that breaks is lower on the list, allowing the stuff that builds to be built and not skipped.
      Testing, bug fixes to RinexNav_T
      Testing, updates to RinexMet_T to use new TestUtil.assert() interface.
      Build and Test, Updates to CmakeLists.txt files     - commented out CMake add_test() for broken test apps unilt they can be fixed     - changed order of some CMake add tests to push broken stuff to bottom     - added category for BinUtils to build.sh to report test results
      Build script, replaced all command substitution backticks/graves with the more modern POSIX form, so value=`foo` changes to value=$(foo)
      Updates to makeDir() in FileUtils.hpp to remove SunOS specific libraries.
      Build script bug fix, install paths
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Testing, Cleaning out RinexNav "ext/tests" that have been replaced by new tests in "core/tests"
      Testing, Cleaning out RinexObs "ext/tests" that have been replaced by new tests in "core/tests"
      Testing, Cleaning out RinexMet "ext/tests" that have been replaced by new tests in "core/tests"
      Testing, removing old redundant unused Rinex tests.
      Build, refactored build configuration from CMakeLists.txt to BuildSetup.cmake
      Build, simplify CMakeLists.txt to reduce scatter of /ext and /core conditionals
      Python bindings, addition of PythonSetup.cmake for manual configuration.
      Python bindings, fixes to CMake cache types for python config template.
      Testing, cleaning old ext test files
      Testing, removal of an old copy of a header that also lives in the core lib.     - test apps shall test only the current version of the headers     - test apps shall not test headers in the source tree, but only in the install tree     - this version of StringUtils.hpp is an old copy of the one in core/lib     - it had apparently been copied into the path with/for the test app that needed it.     - the effect was that the test was testing an old version of the class, not the current version     - as such, the test was made worse than simply dead code, it was misleading.     - code difference as compared to the current version is the removal of method x2int()     - x2int() was in the past replaced by x2uint() which does not strip off the "0x" part of HEX strings.
      Testing, cleaning old test files, SEM
      FileDirProc, fixed map interator problem in FileStore
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Utilities, added std:: scoping to sqrt()
      Build, additional debug message() calls added to the BuildSetup
      Install, fixed install target for include files, which was broken 2015-MAR-05 by commit aca11538.
      Build, updated CMake variabel names for lists of source and include files to be consistent throughout the top level CMakeLists.txt
      Build, Updated test summary list to add missing test app names and sort alphabetically.
      Build, platform-dependent build config refactored, SunOS build improved.     - refactored shared/static flags, compiler flags, and Win32 IDE flags     - changed SunOS linker flags so that everything now builds and installs on x86 SunOS 5.10     - Tests compile and execute on Sun0S 5.10     - ctest calls to test apps run now when called from temp build directory     - need to verify cmake/ctest version numbers match SunOS and Linux platforms.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      SWIG, wrapped nested structs/classes in Rinex headers with #ifndef SWIG
      Tests, removal of all old code which has already been moved to core/tests.
      Tests, migrated IonoModel_T test to new framework.
      Tests, added basic structure for test class ObsClockModel_T
      Build, modified project CMakeLists.txt library build target to explicitly encorporate include files as part of the gpstk library build target in the add_library() directive. This fixes some project issues for IDEs.
      Testing, addition of implementation of test methods for testing constructors and getSvMode method. Removal of other methods not fully developed.
      Testing, additions in build script and CMake files needed to execute test app for ObsClockModel.

Johnathan York (1):
      build.sh: Use bash arithmetic instead of assuming 'bc' installed for num_cores

Jon Little (12):
      Cleaning up unused dirs.
      Making swig build part of the gpstk cmake tree
      Needed to move the doc dir out of the source tree
      Now installs .i files in include dir
      Removing old jam/make detritus
      Fixed a couple places where variable should have been used to reference package name
      Fixing last couple file paths
      Being pedantic about grammar.
      Removed calling of isPrint in printTime to make it faster
      Merge branch '141-DiscFix-Slowdown' into 'master'
      Indention/tabs had become messed up. Fixed
      Now should not throw errors when printing times that aren't representable in all systems.

Joseph Voss (43):
      Initial commit of modified time case tests
      debugging, JulianDate_T has every printf error, MDT_T error with eps. Testing to see if eps needed
      JulianDate and MDJ finished debugging, eps unecessary for MDJ
      Finished debugging GPSWeekZcount
      Finished debugging GPSWeekZcount
      Cleaned up comments
      Rewrote TimeConverters_T, but it isn't built by cmake. Why?
      Working on converting YDS
      YDSTime_T completed with initialization test
      UnixTime_T completed, cleaned up comments in YDSTime_T
      First attempt at rewrite of CommonTime_T
      Completed GPSWeekSecond_T, and cleaned up GPSWeekZcount_T
      Debugging GPSWeekSecond_T
      Debugging CommonTime_T
      Merging in TimeTest changes     Merge branch 'timetests'
      Rewrote Vector_T in new testing standard
      Modified CMake file allowing for Vector_T to build
      Rewrote Matrix_T to new test standard, and fixed errors in Vector_T. Matrix_T will not build due to unknown errors
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Rewrote PolyFit_T, Fixed compiler error for Matrix.hpp
      Rewrote RACRotation_T, Stats_T, Stats_TwoSampleStats_T, & Triple_T
      Fixed redirection operator bug in Xvt
      Added Xvt.cpp test
      Adding GPSTk copyright headers
      Fixed bug in ValidType.hpp, added convhelp and ValidType tests
      Added SP3SatID_T and SatID_T tests
      Changed #include from a cpp file to a hpp
      Added WxObsMap_T and updated the respective CMake file
      Moved file necessary for ObsID class to the core library
      Added test for ObsID class
      Minor spelling fix
      Testing, added BinUtils_T, minor edits to BinUtils.hpp
      Tests, Updated GNSSCore, Math, and Utilties tests in line with TestUtil changes
      Finishing migration of BinUtils test from ext to core
      Adding AlmOrbit_T, Timesystem_T, and Stl_helpers_T
      Minor bug fix in Position_T to fix failing tests
      Minor bug fix in the CMake build files
      Refactoring Matrix_T, adding Matrix Function Tests     - Moved old Matrix test to core, refactored that test into multiple Matrix Function tests     - Matrix_T.cpp converted to header file for Matrix_T class, Matrix_T_*.cpp files now run Matrix_T tests     - Currently the Cholesky, Householder, and SVD tests FAIL     	- Cholesky factorization returns incorrect L, and U matrices. L * U doesn't = A     	- Housefolder decomp fails in one instance where entire column returned is incorrect     	- SVD function tested by verifying U * S * V^T = A, but the matrix returned is nonequivalent due to column negation and other errors
      Testing, adding EngEphemeris and EngNav tests. Minor spelling fix in EngEphemeris_T     - fixParity function in EngNav fails, fixParity calls computeParity but doesn't pass     the knownUpright bool. See issue 147     - EngEphemeris test differs slightly from the TestUtil standard. Many of the EngEphemeris     functions store ephemeris data in different ways. To test this, a set of tests are put in     a function and are called to verify the storage method.     - Data for these tests was reconstructed from the RINEX Nav file     data/test_input_rinex_nav_FilterTest2.99n     - Minor spelling fix in EngEphemeris.hpp     - closes #96, closes #97
      Added a default bool input to fixParity, passes to computeParity
      Testing, Minor changes to AlmOrbit test
      Merge branch 'issue_147' into 'master'

Josh King (5):
      Fixing time system issue caused from declarations in helmertTransform
      Fix so that codeDump can run correctly for prn 0 (minor mod)
      Updated version of SVNumXRef.cpp
      Corrected SVNumXRef.cpp
      Updated PackedNavBits.hpp & .cpp

Nick Fitzsimmons (74):
      Commiting the new test framework infrastructure. The testFramework class is located in core/lib/TestFramework/ .
      Reverted the changes to the scanning functionality of the Time classes. The standard is now that the P option will be read as a string. Updating many tests to new infrastructure. Stats, Stats_TwoSampleStats, and GPSWeekSecond all FAIL. Awaiting further instruction to finalize the changes necessary to have them PASS.
      Committing changes to Rinex_Obs Testing.
      Merging Jason's changes to Rinex_Obs Tests.     Merge remote-tracking branch 'origin/master'
      Changes to the TimeHandling tests so that they now match the new test class naming scheme (from xCLASS to CLASS_T) and adding the additional tests to ANSITime_T. Also a new test method has been added to CivilTime_T. Finally, the testFramework.hpp file is being removed in favor of TestUtil.hpp.
      Update for TestUtil.hpp to include the three extra methods to change a string in the output and the passTest and failTest methods. ANSITime_T.cpp has been updated to reflect these changes.
      Updated core/lib/GNSSEph/RinexEphemerisStore.cpp so that the loadFile method will open the file before reading from it. Updated TestUtil.hpp so that passTest() and failTest() will increment the test counter properly. Updated CivilTime_T.cpp so to remove a couple of redundant tests.
      I broke the build. It is now fixed. RinexEphemerisStore::loadFile will no longer use the is_open method derived from the (great-grand)parent fstream class. The new method is to use a simple !RinexNavStream_Object to denote whether the file has been opened.
      Adding minor comment changes to ANSITime_T and CivilTime_T. Also adding a completed RinexEphemerisStore_T. This new test has many failures associated with it. These failures are documented on GPSTk-MSNTk merger Test Bug list. Also some small functionality has been removed from GPSEphemeris in that the dumpHeader method will no longer rely on the SVNumXRef class in ext.
      Commit to clean up a number of small issues:       1) core/tests/CMakeLists.txt now only includes directories where GPSTk tests have been written. Be sure to update this list as you move to the skipped directories.       2) core/tests/Math/Vector_T.cpp did not compile. This has been fixed.       3) core/tests/TimeHandling/CivilTime_T.cpp and core/tests/TimeHandling/YDSTime_T.cpp have been updated to fail as per the bug page.
      Forgot to cleanup the GNSSEph directory and remove some comments as the test was being built. Also removing IonoModel_T so that the tests will build on non cppunit machines when cloned.
      Commit changes the following:
      Commit to add the SP3EphemerisStore_T.cpp test. Removed all stored output and comparison files associated with this test as they are no longer necessary.
      Pulling changes which include the addition of Sinex.
      Adding core/tests/Utilities/StringUtils_T.cpp tests.
      When trying to build with the SunSolaris compiler, there are issues.     These issues stem from the use of __func__ which CC does not recognize.     The changes are:
      Commit for test result filtering and new additions:     1) All test result output which contained additional commas have been revised.     2) RinexEphemerisStore_T test results now correctly reflect which of the parent        classes to RinexEphemerisStore is actually under test.     3) Added TimeRange.hpp and TimeRange.cpp from MSNTk to core/lib/TimeHandling     4) Added an unfinished test for TimeRange to core/tests/TimeHandling
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Build broke in last commit. Updating the CMakeLists.txt file to reflect the change fixes the build.
      Commiting the completed TimeRange_T.cpp file.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Updates to correct some errors in building using the SunSolaris compilers.     1)core/lib/Math/Stats.hpp now uses sqrt( (long double) ) castings to get the highest accuracy.       Note that the sqrt result is then cast back to whatever type the template is using.     2) core/lib/Utilities/ValidType.hpp now defaults the value variable to zero when using the default constructor.     3) core/tests/TimeHandling/UnixTime_T.cpp was not running its initialization test.     4) core/tests/Math/Triple_T.cpp now uses proper floating point comparisons in assertions.
      RinexEphemerisStore_T.cpp has been updated to use the newest file path conventions. Also the files related to this test have been moved to the data directory.
      SP3EphemerisStore_T.cpp has been updated to the new file path conventions. Its inputs have been moved to the data directory.
      Edits to the files Joseph added. Mainly comments. Also deleted unused files in core/tests/Math/ which were remnants of CPPUNIT.
      Adding the test for SystemTime.
      Investigated why WxObsMap_T.cpp fails on the SunSolaris compiler.     Found that the failing comparisons were comparing two single precision floating     point values by using the absolute value of their difference and enforcing that     this difference be less than a tolerance. That tolerance was set to 1E-12, a     value appropriate only for double precision. This tolerance was changed to 1E-7.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Updating StringUtils_T so that it will work on all platforms
      I have been corrected.     The hexDataDump test has been split into two separate     methods. hexDataDumpTest will now test only for     formatting of the output. This means that any characters,     like end-of-file or end-of-line, which are not common     across all platforms will be replaced by alphanumeric ones.     The second test, hexToAsciiTest, will leave those characters     in to ensure that they are handled properly. This test will     need a revision so that an independent method (not using gpstk)     of converting those hex values to ASCII is used for the comparison     checks.
      Use of captain showed a bug in Triple_T.cpp and Triple.cpp.     In the azimuth angle test you cannot get an azimuth angle if you     are referencing a vector the is parallel to the direction of the origin.     There is a built in test in Triple.cpp to check for this but the tolerance     on it was set to 1E-16 which is too small, especially when adding two     numbers which are on the order of computational round off error. This     tolerance has been raised to match the other test in the azAngle method     (This check to check is to ensure the observer position is NOT along     the z axis.).
      Adding additional files to core/lib/GNSSCore from ext:     1) geometry.hpp - constants file which is needed by Triple, EphemerisRange, and HelmertTransform.     2) gps_constants.hpp - Needed by core/ClockModel and core/GNSSEph files.     3) compass_constants.hpp - Moving prior to merge with GNSSconstants.hpp
      Moving files to end Rinex 3 dependencies on ext:     1) *Ephemeris.hpp (or .cpp) - These files will are used in Rinex 3     2) PZ90Ellipsoid.hpp - Needed by GloEphemeris.hpp     3) SVNumXRef.hpp - Needed by BDSEphemeris.cpp, EngEphemeris.cpp, and GPSEphemeris.cpp
      3rd set of file moves from ext to core:     1) BrcKeplerOrbit.hpp and BrcClockCorrection.hpp - needed by EngEphemeris.hpp     2) ClockSatStore.hpp and PositionSatStore.hpp - needed by SP3EphemerisStore.hpp     3) TabularSatStore.hpp - needed by ClockSatStore.hpp and PositionSatStore.hpp     4) Xv.hpp - Needed by the BrcKeplerOrbit.hpp
      Final set of moves for ending core's dependency on ext     Moved the remainder of the Utilities in ext to core. CommandLine.cpp depends on     a couple of these files, but after speaking with Bryan a decision was made to     move all the Utilities from ext to core.
      Moving Apps Out of Core
      Build of Core C++ Library No Longer Requires Ext
      Removal of Core Dependence on SVNumXRef
      Migration of geometry to GNSSconstants
      Migrated compass_constants to GNSSconstants
      Removed Dependencies on Xv
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Forgot to add the modifications to the source files in the last commit.
      Previous Changes Broke The Build
      Xv.hpp is now Deprecated
      Edits to Tests
      Fix for Matrix_T.cpp. The checks with the vectors now compare with the proper values.
      Updated the failing Rinex tests to have fail messages.
      Brought RinexEphemerisStore_T.cpp to the new testing standard.
      ANSITime_T.cpp has been migrated to the new TestUtil standard.
      Fixed CivilTime_T to work with the new standard. Also found I was dissatisfied with the organization of the TimeTag child class tests. Updated ANSITime_T to reflect those changes.
      TimeTag Child Class Tests Reworked
      More Updates to TimeHandling
      Minor Test Fixes
      More Test Updates
      Test Labels
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      TimeTag Child Class Tests Update and Fixed SatID_T
      Final Fixes for TestUtil Update
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Removing Some Pre-existing tests
      Removal of More Pre-existing Tests
      Pre-existing Test Migration Changes
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Pre-existing Test Migration Change
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Finished Cleanup of GPSZcount Test
      Fixes for Test App Ambiguities on SunSolaris
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Removing RACRotation from ext/tests as it has already been migrated.
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Migration of DayTime Test Seeds
      Update for reference in case mdpscreen needs to be custom built again.
      Removed the mdpscreen additions as these should be left in SGLTk. Just wanted a reference point should mdpscreen need to be built without SGLTk again.

Scot Nelsen (6):
      CMakeLists.txt: added gpstk_VERSION_* variables and condensed recursive file collections.
      CMakeLists.txt: include GNUInstallDirs module for standard directory definitions.
      swig/CMakeLists.txt: PYTHON_INSTALL_PREFIX properly defaults to CMAKE_INSTALL_PREFIX
      Unified definition of version (major, minor, patch) variables.
      Generate package import cmake file for use by other cmake projects.
      Correct CMakeLists.txt so it tries to remove getopt.h from the correct list.

Steve Johnson (5):
      Add StringUtils::splitOnWhitespace(...) method and tests
      Add a few more splitOnWhitespace tests
      Clean up debug output.  Add empty string test cases to StringUtils::splitOnWhitespace
      Comment out broken assignment operator that breaks Windows build
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk

Tom Gaussiran (4):
      Fixed a portion of the code where the same call was issued twice.
      Actually took out the line that were just commented out in the previous modification.
      huh?
      Hacked out the code which writes the log file. This will need to be handeled more elegantly but it does provide a 20x speed up.

renfrob (5):
      Adding split field unpacking to PackedNavBits
      Generalized PackedNavBits.matchBits()
      Tweaking BDS track codes
      Fix to subtle bug in rationalize( )
      Enhancement to provide more flexible output

vestuto (2):
      Merge branch 'issue_70' into 'master'
      Merge branch 'issue_86' into 'master'




	#################################################



Version 2.5   Friday, October 10, 2014

   General modifications
   ---------------------
   - Python Bindings Installation Package
   - GPSTk Script Changes
      - Tweaks and Clean-up
      - Build and Install C++ Lib/Apps
      - Build and Install Python Lib 
   - This release includes new/modified source code comments for improved readability.
   - Rearrangement and clean-up of various documentation
   - Various Bug and Compile Warning Fixes (Thanks Richard Penney!)

   Modifications by Author
   ---------------------
	Andrew Joplin (1):
	      SVNumXRef.cpp: typo fixes

	Bryan Parsons (11):
	      INSTALL file spelling fix
	      Merged changes from Richard Penney's Github fork.
	      Merging Internal changes into External branch
	      Updated test_gpstk_util test script to handle updated PI value in GNSSconstants.hpp
	      Mac OSX fixes for GCC/Clang and Mac OSX preparing for v2.5 release.
	      MS VS2012 fixes preparing for v2.5 release.
	      Merging MacOSX and Windows fixes into one branch
	      Merge Windows and MacOSX changes into master branch
	      Solaris Sparc changes for v2.5 release.
	      Pulling changes from Gitlab Repo
	      Pulling changes from Gitlab repo

	Jason Vestuto (49):
	      Build framework bug fixes: Corrected instances of sgltk to gpstk in CMake input file, and added creation of build directory for the python bindings.
	      Restoring a header file that is required SGL toolkits dependent upon GPSTk. May need to migrate this header to the other toolkit.
	      Disambiguation of multiple headers with the same file name by removal of one and updating comments of the other.
	      Removal of mistakenly restored header that has been migrated to SGLTk.
	      Added a missing mkdir for the python install directory.
	      Overhaul of python bindings build and install using CMake.      
	      	- Overhaul of both CMake files and build script to build python bindings by linking to libgpstk.so rather than recompiling GPSTK C++ library source files.      
	      	- Updated both the CMake files and build script to not export environment variables, while still allowing user-defined environment to be used to set the install path.      
	      	- Added a test of $PATH and $LD_LIBRARY_PATH, such that the user is warned if neither contain the gpstk install path.      
	      	- Added a CMake Find module to allow CMake process for the python module build to find the GPSTK install rather than hard-coding relative files paths.      
	      	- Modified the python module CMakeLists.txt, replacing the gpstk source file list with a find directive, and adding install directives to place _gpstk.os and gpstk.py in site-packages.      
	      	- Updated the build script to follow the new pattern of having the bindings link against a pre-built libgpstk.so rather than recompiling *.cpp files in the GPSTK library.
	      Removal of duplicate header file entries in gpstk_swig.hpp.
	      Several bug fixes for the default GPSTk install path handling.      
	      	- Fixed a typo in the build script which misnamed gpstk_install path variable name, causing the default to be ignored.      
	      	- Fixed a logic error in the CMake Find module so that the python bindings properly accept the default gpstk install path.      
	      	- Added the default gpstk_install path in the cmake call for the python bindings, so that an environment variable does not need to be set for the python module to build.
	      Changed default install path for the python extension module.      
	      	- Previous path was a /gpstk sub-directory under ~/.local/lib/<python-version>/site-packages      
	      	- sys.path does not know about sub-directories under site-packages, so removed the trailing /gpstk and just install to site-packages directly.
	      Updated CMake find directive for PythonLibs to force python2 vs python3.
	      Changed the python extension module install back to using a gpstk subdirectory. Have added the required module __init__.py file that was missing from the last revision.
	      Updates to both test and example scripts to account for namespace changes in the python extension module.      
	      	- Fixed references attributes under gpstk.constants, gpstk.exceptions, gpstk.cpp      
	      	- Fixed import statements referencing same.      
	      	- Improved usage and example usage statements in docstrings of example scripts
	      Updated SWIG interface file by adding an %extend directive to add the __getitem__() method to gpstk::Triple so that it could support indexing as seen in the python extension module. This fixes about a dozen otherwise broken test cases in test.py.
	      Updated the SWIG interface file to remove all the "_swigregister" pollution seen in the gpstk python extension module namespace.
	      Updated SWIG interface file, adding a missing include of the Triple.i secondary SWIG input file. Parts of a previous fix, needed because of that missing include, were moved from gpstk.i to Triple.i.
	      Initial commit of a distribution install package for the python extension module using distutils. Needs further testing.
	      Restructuring python package file tree for distutils installs.     
	      	- Moved /data and /test directories into /gpstk package dir and added init files     
	      	- Added methods to gpstk.data init file, including full_path() to get file paths and load_data() load data files as byte strings attributes.     
	      	- Added method to gpstk.test, run_all(), to allow all test cases to be run from within python.     
	      	- Updated test cases that require input data file reads to call gpstk.data.full_path() method to determine data file path after install.     
	      	- Updated setup.py for distutils to treat /data and /test and sub-packages     
	      	- Updated setup.py for distutils to correctly associate files _gpstk.so and gpstk.py with the gpstk package at install time.     
	      	- Updated both the MANIFEST.in and README.txt to reflect package file tree changes.
	      Updated python extension package example to use gpstk.data method to determine full file path of data file after install.
	      Update to build script to account for changes in python package install using distutils. Addition of optional methods in CMake files to be further developed so that one can build and install with CMake, or just build with CMake and install with distutils outside of CMake, both in a consistent way.
	      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
	      Updated python package CMake install options.     
	      	- Removed optional install methods that were used for previous tests.     
	      	- Preserved two CMake options: (1) to build and (2) to build and install
	      Python bindings, CMake install options clean up:     
	      	- added hooks to script_gpstk.sh to trigger sphinx documentation generation.     
	      	- cleaned up CMake/distutils install options so that either CMake does it or the user has to call "python setup.py install" manually.     
	      	- updated the --prefix flag in CMake since "~/.local" and CMAKE_INSTALL_PREFIXare not treated in the same way nor generate the results on install.     
	      	- Refactored test of users PYTHON install in CMake files.
	      Python bindings, update to distutils setup files, package init, and CHANGES file.     
	      	- moved setup.py.in to the same pat as the CMakeLists.txt file     
	      	- Removed all references to distutils.Extension since the wrapper code is compiled with CMake, not distutils.     
	      	- Left the setup.py in the install_package path, even though it is overwritten every time you use the CMake install option. If not delivered with the package, other install and package options would not be available without a cmake install, even though cmake is not required after the build.     
	      	- removed unused and broken load_data() method from gpstk.data __init__.py file     
	      	- added comment to previously empty CHANGES.txt
	      Python bindings, deleted unused __init__.py file in the swig directory.
	      Python bindings, clean up of file tree.     
	      	- moving but preserving some data files for fic and msc     
	      	- removing duplicate copies of example and test files found outside of the install package.
	      Python bindings, updates to unit tests in gpstk.test     
	      	- corrected the expected output of the gpstk.scanTime() test for the ( '158 200', '%F %g') case     
	      	- added a new unit test for gpstk.CivilTime for date '01 06 1980'
	      Python bindings, fixed namespace bug by changing gpstk.exceptions to gpstk
	      Python bindings, clean up of some includes in the context of trying to determine why SWIG wraps and presents some implicit conversion/constructor operators for various ephemeris objects from C++ and not others.
	      Python bindings, updates to install package meta-data files.
	      Python bindings, removal of user-specific name in example file paths.
	      Python bindings, updated test package so that individual test modules can be run as scripts or from the python interpreter as e.g. gpstk.test.run_util()
	      Python bindings, removal of files no longer used.
	      Python bindings, addition of scripts used to test python package installs. For use by devlopers, not for distribution.
	      Python bindings, updated script_gpstk to create a ZIP file of the html documentation output from Sphinx and place it in the install_package/docs directory for package distribution.
	      Python bindings, correction of RINEX data file names.     
	      	- Testing revealed the data files rinex3obs_data.txt and rinex3nav_data.txt to be RINEX2 format.     
	      	- Corrected file names and all references to them in the test and data modules.
	      Python bindings, grooming of package meta-data files, including additional markdown formatting.
	      Python bindings, updtes to Sphinx RST documentation files to account for recent changes to the python bindings build and install package.
	      Python bindings, corrections to RST formatting in Sphinx documentation.
	      Updated top-level project README for clarity, including a table of contents, more explicit references to other documentation files within the source file tree, and references to the python bindings package.
	      Python bindings, removal of unused scripts, and addition of draft code block for creating source distribution packages for the python package in script_gpstk.sh
	      Python bindings, removal of unused scripts, and addition of draft code block for creating source distribution packages for the python package in script_gpstk.sh
	      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
	      GPSTk documentation, updates to README and addition of full GPL/LGPL licensing files needed. LGPL depends on the GPL, so we need to distribute copies of both with the source code.
	      GPSTk build script, minor updates to white-space formatting.
	      GPSTk build script, updated script header comments.
	      GPSTk Licensing, pleased copies of the full GPL and LGPL amendments in the gpstk/dev directory to ensure that the any portion of the file tree that we might distribute include the full license documents. In the future, we may have the package install scripts simply copy the top-level documents.
	      GPSTk install, moving dev/INSTALL to dev/INSTALL.txt to avoid conflicts with things that are not case-sensitive when you try to create an dev/install/ subdirectory
	      GPSTk project documentation, changing all top-level documentation file names to be consistent with the pattern FILENAME.txt
	      GPSTk project documentation, more renames to follow consistent pattern of FILENAME.txt

	Jon C. Little (1):
	      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk

	Jon Little (21):
	      Adding descriptive output
	      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
	      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
	      Added the 8-bit week number to the output
	      getSatHealth now checks to see that an ephemeris is found before calling isHealthy
	      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
	      Refactor of script_gpstk.sh to work with current paths correctly along with pruning of some diagnostics. Python build now uses the source tree and not the install tree
	      python gpstk module install is now done from cmake/make with reasonable defaults
	      Now honors the CMAKE_INSTALL_PREFIX definition if the gpstk environment variable isn't defined
	      Examples now use the proper rinex 3 obs identitiers.
	      Removing files that are no longer used. See ObsID for similiar functionality
	      Adding a couple more classes to wrap
	      Adding support for ObsEpoch and related classes
	      An example of walking RINEX data in python and converting that to ObsEpoch structures
	      Moved includes and ignores back to upper level .i file
	      Moved includes and ignores back to upper level .i file
	      Two files to start moving the bindings build to play nicely with other modules.
	      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
	      Swig doesn't support long doubles so the MJD interface just uses a double now. This will cause some loss of precision but...
	      Added a reasonable hash to SatID and ObsID to allow objects to be use as keys in dicts and sets
	      Fixed bug where the wrong field was used in the ObsID hash

	Josh King (5):
	      added the dump method to SVNumXRef that checks for overlap
	      small edit
	      fixed health error
	      added SVN 68 launch
	      SVNumXRef update

	Richard Penney (8):
	      Added .gitignore file & fixed permissions.
	      Improved const-correctness of SatPass etc.
	      Patched const-accessor methods of SatPass.
	      Miscellaneous minor tidying.
	      Removed '#pragma ident $Id$'
	      Fixed various compiler warnings from g++ -Wall
	      Fixed more compiler warnings from g++ -Wall
	      Fixed yet more compiler warnings from g++ -Wall

	Supraj Prakash (5):
	      SWIG Python Bindings comments added
	      Created scripts to build C++ and Python GPSTk individually and allow for changes in build and install locations; Fixed errors in Python example 2
	      Spelling error fix in gpstk_pylib.i
	      Made syntax fixes and suppressed Python Build Directory Already Exists message
	      Removed the pure C++ library and pure Python library build scripts

	Thomas Joseph Garcia (4):
	      STLTemplate.i: added template for rinexObsID and vector_rinexObsID to support writing gpstk Rinex files in python.
	      STLTemplates.i,FileIO.i: Added functionality to write a vector of Rinex3ObsData objects in C++ through a python call.
	      Merge branch 'master' of https://repositories.arlut.utexas.edu/sgl/gpstk
	      STLTemplates.i: Added some additional typemaps to support Rinex2 reading in Python.

	renfrob (1):
	      Fix glodump to only dump GLONASS from multi-GNSS nav files



	#################################################



Version 2.4   Tuesday, April 22, 2014

   General modifications
   ---------------------
   - Core & Ext Folder Structure implemented
      - Core Folder
         - Location of Core GPSTk Code base, fully reviewed and tested.
         - Currently, no code resides in this directory, as code review and testing are currently ongoing.
      - Ext Folder
         - GPSTk Developer Sandbox Code base
         - Code that is in development, prototyped, or does not meet criteria for Core Code base.
   - Python Bindings Relocated and Repaired
   - Doxygen build Repaired
   - New Initial CMake Testing framework
      - Currently in Development
      - No actual tests are being utilized at this time, only the framework
   - New GPSTk Script
      - Build and Install GPSTk Code
         - Automatically Identifies and Utilizes multiple processors for increased build speed.
         - Can be set to build only the Core Code base (Currently broken, as no code resides in Core folder)
      - Build Doxygen Documentation
      - Build Python Bindings
      - Utilize Test Framework
   - Code Header's have been standardized and updated to include GNU LGPL v3.0
   - This release includes new/modified source code comments for improved readability.
   - Various Bug Fixes

   Modifications by Author
   ---------------------
Andrew Joplin (4):
      Added EphemerisRange.hpp to python bindings
      Added EphReader.hpp to python bindings
      Added FFStreamError to python bindings
      EphReader.hpp: forgot an include

Brent Renfro (14):
      Fixes for Odd IGS brdc aggregate files.
      fixing SatSystem bug in GPSEphemeriStore. updating bc2sp3.cpp
      Adding a printTerse message
      Correction to rationalize()
      Change BDSEphemeris begin/end valid based on exmaination of operationel data
      Correct error in printTerse output
      Tweaks to BDS ephemeris based on processing 2013 data
      Improved error message on exception
      Fixed GEO position determination
      Merge branch 'master' of repositories.arlut.utexas.edu:sgl/gpstk
      Add velocities to BDSEphemeris.cpp
      Update to status as of 4/14/2014
      Improving optional debug printout
      Correcting fit Interval to fit duration

Bryan Parsons (14):
      Phase 1 of GPSTk to SGLTk Migration, with Python CMake updated
      Base CMakeLists.txt clean-up
      CMakeLists.txt tweak
      GPSTk CMake build/install automation script, modified version of SGLTk script.
      script change
      Script update
      RinEdit bug fix
      Code updated from submitted patches
      Moved python bindings folder to main GPSTk folder, fixed Python build code and re-added gpstk_builder.py, updated and fixed Doxygen, added doxygen and python option to main script_cmake.py
      Deprecated setup.py in favor of cmake and shell script
      Script bug fix
      Script updated with additional options - doxygen, python, and test.  Headers have been updated for all applicable code to reflect updated LGP License 3.0.
      Mac OSX Maverick Tweaks
      Additional Mac OSX tweaks

Jon Little (5):
      Fixing the file modes. The execute bit got set on basically every file. Someone's development environment is messed up
      Added some clumsy cmake magic to prevent installing getopt.h on gcc systems
      Hopefully made the getop.h handling a little smarter
      Continuing to make things build on solaris with older gcc
      It appears that some variants of cmake are sensitive to the ordering of target_link_libraries



      
	#################################################



Version 2.3   Thursday, December 5, 2013

   General modifications
   ---------------------
   - GPSTk Library merger & Directory Restructure
      - Core Library and Auxiliary Libraries have been merged into single GPSTk Library
        - Core Library code (src) have been merged into lib directory
        - Code has been reorganized into function-oriented directories
   - CMake is now the build system of the GPSTk
      - Jam and Autotools build files have been deprecated and removed
   - Linux 32-bit & Cygwin are no longer supported platforms
   - Bug fixes for multiple applications
   - This release includes new/modified source code comments for improved readability.
   - The next planned release, GPSTk v2.4, will be a major upgrade that will introduce a new testing infrastructure, as well as further code reorganization.

   Modifications by Author
   ---------------------
Brent Renfro (4):
      Fixing end-of-fit interval error
      Fixed range bound error. URA=15 was not allowed but is valid
      Fixing TimeSystem problem in compSatVis
      Adding SDOP calculation to compSatVis

Brian Tolman (11):
      DAT record should not be verbose
      Restore begin and end time for GPS, fix PRSolve bug.
      MARKER TYPE is not a required header record
      Thanks to Felipe Nievinski
      Correct a few small bugs in Rinextools, and clean up many unused variables
      RinDump: added Glonass nav, changed MW to WLC and added NLC
      Small omission from last commit
      Fix seg fault when ephemeris is not found, replace with throw
      Satisfy some compilers
      Remove throw where it should not be
      Remove stray # in headless mode

Bryan Parsons (15):
      Update Windows Installer file
      Copied Readme to additional location for Gitlab use
      Additional tweaks for Rinex files
      Minor modification to RinDump
      CMake Linux Tweak
      Bug fix for WhereSat
      Major merge and reorganization of src and lib, new lib folder contains function oriented folder structure.  Jam and autotools files have been removed.  CMake now recursively searches through lib for source files, and no longer needs CMakeLists per folder.
      Updating previous commit to new lib
      Small Rinextools changes
      More Wheresat bug fixes
      Update Python files to match source code restructure
      Tweaks to windows RC2.3 build
      Tweak for Solaris Sparc RC2.3 build
      Tweaks for MacOSX RC2.3 build
      Tweaks in preparation for RC2.3 Release

Jon C. Little (1):
      Removing the exec bit from a readme

Jon Little (3):
      Fixing some file modes
      To encourage _not_ poluting the source tree, don't ignore the transient files that cmake creates and uses
      Merge branch 'master' of repositories.arlut.utexas.edu:bparsons/gpstk

Sean Lewis (5):
      Added python binding test runner.
      Updated Python test cases to match changes to C++ time classes.
      Python binding built fixed (source file list repaired).
      Python gpstk_builder.py script default install location fixed.
      Developer Documentation added for python bindings.




	#################################################



Version 2.2   Friday, September 6, 2013

   General modifications
   ---------------------
   - Fully functional CMake cross-platform build system has been finished and tested for the GPSTk.
   		- Using CMake, the GPSTk now supports IDEs in addition to current supported platforms.
   		- Newly supported IDEs:
   			- Visual Studio 2010 IDE (32-bit)
   			- Visual Studio 2012 IDE (32-bit & 64-bit)
   			- XCode (32-bit & 64-bit)
   - Basic RINEX 3.02 Framework has been added, still developmental and subject to change
   - Basic Python Interface Bindings have been added, still developmental and subject to change 
   		- Additional Information: http://www.gpstk.org/pythondoc/
   - Minor modifications to Jamfiles, Makefiles, and source code 
     for clean compilation (no warnings or errors) & compatibility with the latest 
	 Cygwin, Windows 7 VS, Solaris 10 Sparc, and Mac OX X Lion (10.7.5).
   - The next planned release, GPSTk v2.3, will be a minor upgrade that will introduce a new function-oriented dirctory structure.
   - This release includes new/modified source code comments for improved
     readability.

   Modifications by Author
   ---------------------
	Brent Renfro (17):
	      First change to capture/crack CNAV from MDP
	      New classes to crack CNAV messages into engineering units.
	      Updated SVNumXRef to reflect launch of SVN 66 and decommision of SVN35
	      Improvements to support CNAV data tests
	      =Clarifying label in PackedNavBits.dump()
	      =Clarifying label in PackedNavBits.dump() - AGAIN
	      Resolving a merge issue (we hope)
	      Refined validity calculations, improved dumpTerse()
	      Removing dead code
	      Fixing low-rate-of-occurrence errors in CNAV crackers
	      Adding L2CPhasing flag to CNAV class
	      Fixing output format and returning #bits used
	      Adjusting multi-row rollover
	      Adding CNAV pos/vel changes
	      Adding formatted output to GloEphemeris
	      Fixing problem with lack of epoch time in CNavGGTO
	      Adding new utility to dump GLOANSS Rinex nav files

	Brian Tolman (22):
	      Fix the dangling-else problem on MacOSx for logstream, and undo unnecessary added brackets from commit b7ef28941
	      Clarify Lagrange interpolation routines and documentation.
	      Small changes to refine and correct SP3 interpolation
	      Add several new systems to TimeSystem and Time classes.
	      Modify ObsID, adding several new entries, for Rinex3.02
	      Fix bug in LagrangeInterpolation; thanks Sara Farese.
	      Fix 2 bugs in getEpoch and scanTime
	      One file omitted in my last commit
	      Fix bug in scanTime when scanning into undefined time.
	      Fix missing return value.
	      Fix bug in WeekSecond related to GPS epoch in TimeConstants.
	      Minor fixes to go along with previous commit.
	      Improve SVD convergence and fix bug in Householder.
	      Fix bug in TimeSystemCorr, trivial change to WeekSecond classes.
	      Move leap second correction into the TimeSystemCorr::correct.
	      size_t is unsigned; changing index from int to size_t is not always a good idea
	      Redesign orbit-based ephemerides and time systems for R3.02
	      Changes omitted in the last commit
	      Undoing this leads to a seg fault
	      Remove all reference to RinexNav in OrbitEph classes.
	      Give each xxxWeekSecond their own format specifiers.
	      Small changes to complete the examples


	Bryan Parsons (30):
	      Added commit id for RC2.1
	      Changes to fix compile issues from prior commits
	      Minor fix submitted by Anthony Hughes - Replace EllipsoidModel with const GeoidModel
	      Reverting convhelp.hpp back to original code after commit change
	      Code Cleanup: Removed DebugUtils class and all references
	      Code Cleanup: Removed DebugUtils from repo
	      Build system fixes
	      Major CMake Build System Changes for Linux OS
	      Additional Build System Tweaks
	      Cmake Build Changes for Cygwin
	      Build System cleanup for recent commits
	      Small CMake adjustment
	      CMake Build Changes for VS10 (32-bit) & VS12 (32-|64-bit)
	      CMake adjustment
	      Another small fix for CMake Build
	      More CMake tweaks
	      CMake Build Changes for Solaris Sparc
	      Fixing Mac OSX error from prior commit.  Initialization of function is required in TabularSatStore.hpp for Clang compiler
	      Jamfile edited to reflect removal and addition of new files
	      Brackets added to if statements to remove dangling else warning in Clang
	      Additional tweaks to LOG file and brackets for dangling else warnings in Clang
	      Major Cmake Build Changes for Mac OSX     
	      Rinexpvt design flaw patch
	      MDPtools patch - Brian Tolman
	      CMake Adjustments for Linux and Solaris
	      CMake Mac OSX tweaks
	      CMake tweaks for added files
	      Cygwin CMake tweaks
	      CMake Visual Studio tweaks
	      CMake XCode clean-up and tweaks

	Jon Little (8):
	      Added a variation of ComputeAtTransmitTime that estimates pseudorange as opposed to requiring it
	      Ignoring what should be ignored
	      Ignoring what should be ignored
	      I'm not bparsons
	      Fixing file modes
	      Removed TypeID.hpp, since its now in procframe
	      Moving TypeID back here
	      more fixage of file perms

	Josh King (1):
	      Fixing MFtime from short to long

	Sean Lewis (118):
	      Initial work on time module and timeconvert python port
	      Improved python binding for time module
	      Finished implementation of gpstk_time python bindings and test programs
	      Simplified time comparision test syntax with eval()
	      gpstk_time bindings formatting changes
	      More work on python bindings - gpstk_util module created
	      ObsID implemented in python bindings - inner class Initializer exported to ObsIDInitializer
	      Python modules collapsed into single gpstk module
	      Deleted old, unused SWIG binding files
	      Deleted rest of old/unused SWIG bindings
	      TimeString.scanTime moved to gpstk.i
	      Undefined methods commented out
	      Fixed const-related bug in FileStore
	      Reorganized python SWIG bindings to multiple files but a single module
	      CivilTime comment error fixed
	      Minor changes to let SWIG process library
	      Improved python testing/enum semantics
	      Vector zero-releated members removed for SWIG compilation
	      Python binding structure reworked to a single module
	      Simplified Python constructor/enum mappings
	      Python binding directory structure reworked and documentation added
	      Improved Python documentation system
	      Added distutils system to build python module
	      Removed math utilities from python bindings
	      Moved inner-class exceptions into global namespace so SWIG can wrap them
	      Python test coverage improved
	      EngEphemeris updated to deal with non-nested exception change
	      Added typemaps, fixed broken ignores
	      Removed CMake build for Python bindings
	      More python binding testings
	      Added timeconvert module to python bindings
	      Removed doc.py step of python build process
	      Python doxygen creates full index now
	      Fixes to python binding of Position class
	      Removed timecony from python examples
	      Added SEM support to python
	      Python setup generates docs now
	      Updated references to exceptions moved to gpstk namespace
	      Added ObsIDInitializer to build system
	      Temporary fix to python setup script
	      Added dummy doc file for python
	      Python build switched back to CMake
	      Documentation typo fixed in SystemTime
	      Some constant initialization changed to let SWIG wrap it
	      Python timeconvert moved to examples
	      Wrapped AstronomicalFunctions for python
	      Minor documentation errors fixed
	      Headers removed from python CMakeLists
	      Cleaned up python SWIG files
	      Undefined method in OrbElem removed.
	      Duplicate definition in TabularSatStore removed
	      Added string/concatentation support for the python vector
	      Python File IO and __str__ support improved
	      Python SEM file IO/plot example added
	      Python examples and testing updated
	      SWIG things added to gitignore
	      Minor style changes to python tests/examples
	      Fixed SEMData::dump to output to the correct stream.
	      Reworked python build to hide certain objects
	      Cleaned up python examples/tests
	      Added more python ignores
	      Python Rinex3Obs support added
	      Python module_builder now builds to user-given target
	      Improved python build script
	      Rinex3ObsData::RinexDatum pulled into gpstk namespace so SWIG can wrap it.
	      Updated a reference to RinexDatum
	      Updated another reference to RinexDatum
	      More file support added for python
	      Duplicate method signature removed
	      Rinex3ObsHeader inner classes moved to gpstk namespace.
	      Added module system to python bindings
	      Added more __str__ and vector support for Python
	      Added RinexUtilities functions for python
	      Removed an undefined function in RinexUtilities.hpp
	      Cleaned up some python tests and renamed test files.
	      Pulled out RinexObsHeader inner classes so SWIG can wrap them.
	      Pulled out Rinex3NavHeader  inner classes so SWIG can wrap them.
	      Pulled out Rinex3ClockHeader/RinexObsData inner classes so SWIG can wrap them.
	      Updated python examples
	      Added python wrappers for RinexMet clases
	      RinexMetHeader inner classes pulled out so SWIG can wrap them.
	      Sphinx documentation system added for python bindings.
	      Python test cleanup for lazy file reading.
	      Renamed python documentation files and added more sphinx docs.
	      Wrote more python examples and documentation.
	      Fixed Rinex3MetData data access in Python.
	      Distutils release/packaging added for python bindings.
	      Updated file location of gpstk_builder.py script.
	      Updated python documentation and examples.
	      Cleaned up STL python wrappers and improved auto-gen doc pages.
	      Fixed off-by-one file IO errors in python; added FIC support.
	      Added position difference script and added file input filter parameter.
	      Added typemaps for C arrays -> Python lists.
	      Added 4th example for Python bindings.
	      Fixed python distutils build by adding gpstk header file.
	      Updated Python bindings to use new time class structure.
	      Added missing Exception for AlmOrbit::svXvt.
	      Cleaned up Python exception handling.
	      Exception specifiers removed from time-related files.
	      Added python testing for CommonTime.
	      Extraneous backslashes fixed in CivilTime.
	      Updated python time test code and added python gpstk exception support.
	      Minor fixes to python testing.
	      Fixed some slightly broken Python tests.
	      Updated python distutils build and minor fixes.
	      Python binding files cleaned up.
	      Added appropriate git ignores to sphinx documentation.
	      Refactored Python binding files to eliminate header duplication.
	      Added Python wrappers for Moon/Sun positioning and Pole/Solid Tides.
	      Fixed some broken exception specifiers for Brc classes.
	      Minor fixes to python build and position difference script.
	      Added MSC file bindings for Python.
	      Cleaned up Python examples.
	      Added python wrappers for some constant arrays in GPS_URA.hpp.
	      Updated python bindings to use new Ephemeris & EphemerisStore classes.
	      Python API changes to FileIO and time API.
	      Python binding formatting changes.
	      Python support for Expression evaluation added. 


   	#################################################



Version 2.1   Tuesday, June 25, 2013

   General modifications
   ---------------------
   - Version 2.1 is comprised of SVN revision number 3143 to Git revision number 4daf3c5 
   - A number of modifications were made to Jamfiles, Makefiles, and source code 
     for clean compilation (no warnings or errors) & compatibility with the latest 
	 Cygwin, Windows 7 VS, Solaris 10 Sparc, and Mac OX X Lion (10.7.5).
   - Base CMake files have been added for future CMake support, still in development
     stages.
   - The next planned release, GPSTk v2.2, will be a minor upgrade that will introduce
     a new fully functioning CMake cross-platform build system and deprecation of some
     older supported platforms.	 
   - This release includes new/modified source code comments for improved
     readability.

   
   Library modifications
   ---------------------
   - Added to dev
   
		dev/CMakeLists.txt
			Added first CMake files to src, lib, apps, and examples	
		
		dev/.gitignore
			added .gitignore
		   
   - Modified in dev
		
		dev/Jamrules
		    Code Changes to fix Solaris Sparc Warnings
			Fixed Make build files, code format cleanup, TimeSystem error
			Add ruls for vs2012 to get rid of 'regex.lib' on windows.
		
		dev/Makefile.am
			Fixed Make build files, code format cleanup, TimeSystem error
			
		dev/config.guess
		dev/config.sub
		dev/configure.ac
		dev/depcomp
		dev/install-sh
		dev/ltmain.sh
		dev/missing
			Fixed Make build files, code format cleanup, TimeSystem error
			
   - Added to src
   
		dev/src/CMakeLists.txt
			Added first CMake files to src and lib.
   
   - Modified in src   
   
		dev/src/Makefile.am
			Fixed Make build files, code format cleanup, TimeSystem error
			
		dev/src/Jamfile
			jam install wasn't installing GNSSconstants.hpp. Now it will

		dev/src/GPSOrbElemStore.cpp
		dev/src/GPSOrbElemStore.hpp
		dev/src/Exception.cpp
		dev/src/Vector.hpp
		dev/src/FICFilterOperators.hpp
		dev/src/PRSolution.cpp
		dev/src/CommandLine.cpp
		dev/src/PowerSum.hpp
		dev/src/HelmertTransform.cpp
		dev/src/ReferenceFrame.cpp
		dev/src/LinearClockModel.cpp
		dev/src/GPSOrbElemStore.cpp
		dev/src/CNAVEphemeris.cpp
		dev/src/CommandLine.cpp
		dev/src/EngEphemeris.cpp
		dev/src/AntexReader.cpp
		dev/src/CommandLine.cpp
		dev/src/Expression.cpp
		dev/src/ObsID.cpp
		dev/src/Rinex3EphemerisStore.hpp
		dev/src/Rinex3NavHeader.cpp
		dev/src/RinexMetHeader.hpp
		dev/src/TabularSatStore.hpp
		dev/src/CommandLine.cpp
		dev/src/BinexData.cpp
		dev/src/TypeID.hpp
		dev/src/TabularSatStore.hpp		
			Code Changes to fix various Warnings on various supported platforms 

		dev/src/TypeID.cpp
			Add a new TypeID::transmit for POD.
			
		dev/src/BrcClockCorrection.hpp
		dev/src/BrcKeplerOrbit.hpp
		dev/src/EngEphemeris.hpp
		dev/src/GalEphemeris.hpp
			Commented out unused functions for Python development efforts 
			
		dev/src/OrbElemLNav.cpp
		dev/src/OrbElemRinex.cpp
		dev/src/OrbElemFIC9.cpp
			Fix health bit format
			Mod to round endValid UP to appropriate value when Toe offset
				
		dev/src/SVPCodeGen.cpp
			Fixed end of week bug for PRNs 38-210.
			
		dev/src/TimeString.cpp
			Fixed a bug where scanTime would not process a %f specifier correctly
			
		dev/src/GNSSconstants.hpp
		dev/src/ObsID.cpp
		dev/src/ObsRngDev.cpp
		dev/src/ObsRngDev.hpp
			Paramaterizing ObsRngDev to allow for other than L1-L2 dual frequency iono corrections
			
		dev/src/DebugUtils.hpp
			Adding support for ashtech smoothing

		dev/src/Rinex3NavData.cpp
		dev/src/Rinex3NavData.hpp
			Added constructor to allow conversions between OrbElem and Rinex3NavData objects
			
		dev/src/StringUtils.hpp
			We can use 'std::regex' instead of 'gnu regex' for visual studio 2012!
			
		dev/src/Matrix.hpp
		dev/src/MatrixOperators.hpp
		dev/src/Vector.hpp
			1)Fix the constructor Matrix(row,col,vector) to make it consist with operator'=' and all the other Matrix constructors;
			2)Enable 'Vector || Vector' to get a left to right matrix;
			3)Add a operator '<<' for 'Vector', and user can construct a vector as the following:
			  Vector v; v << 1 << 2 << 3 << 4 << 5 << 6;
			  	
		dev/src/FICFilterOperators.hpp
		dev/src/FileFilter.hpp
			Add blockNum tests to Block 62 & 162 unique methods in FICFilterOperators to fix seg fault in mergeFIC.
			
		dev/src/MemoryUtils.hpp
		dev/src/RinexUtilities.cpp
			Fix some expression 'GPSTK_THROW(Exception("..."))' for it COULD NOT get the exception location properly!
			
		dev/src/GPSOrbElemStore.cpp
		dev/src/GPSOrbElemStore.hpp
		dev/src/OrbElem.hpp
		dev/src/OrbElemICE.hpp
		dev/src/OrbElemLNav.hpp
		dev/src/OrbElemRinex.hpp	
		dev/src/OrbElemICE.cpp
		dev/src/OrbElemLNav.cpp
		dev/src/OrbElemRinex.cpp
		dev/src/OrbElemCNAV.cpp
		dev/src/OrbElemCNAV2.cpp
		dev/src/OrbElemICE.cpp
			Bug fixes from synthetic data testing
			Added OrbElemICE and OrbElemCNAV2 to accomandate the new CNAV2 navigation message coming soon.
			Added OrbElemCNAV to accomodate new CNAV message coming soon. 
			OrbElemRinex.hpp: Added loadData( Rinex3NavData ) to fix compile error.
			
		dev/src/AntexReader.cpp
		dev/src/MoonPosition.cpp
		dev/src/SunPosition.cpp
			Fix problems for PPP examples.
			
		dev/src/TimeSystem.cpp
			Add the TimeSystem string for COM(BeiDou Time,BDT)
			
		dev/src/ReferenceFrame.cpp
		dev/src/ReferenceFrame.hpp
			fixed a segmentation fault when initializing static data members.
			
   - Deleted from src
   
		none

   - Added to lib

		dev/lib/CMakeLists.txt
		dev/lib/geodyn/CMakeLists.txt
		dev/lib/geomatics/CMakeLists.txt
		dev/lib/procframe/CMakeLists.txt
		dev/lib/rxio/CMakeLists.txt
		dev/lib/vdraw/CMakeLists.txt
		dev/lib/vplot/CMakeLists.txt
			Added first CMake files to src and lib.
   
   - Modified in lib
		
		dev/lib/geodyn/Makefile.am
		dev/lib/geomatics/Makefile.am
		dev/lib/procframe/Makefile.am
		dev/lib/rxio/Makefile.am
		dev/lib/vdraw/Makefile.am
		dev/lib/vplot/Makefile.am  
			Fixed Make build files, code format cleanup, TimeSystem error
		
		dev/lib/geomatics/SRIMatrix.hpp
		dev/lib/procframe/GeneralConstraint.cpp
		dev/lib/vplot/LinePlot.cpp
		dev/lib/vplot/ScatterPlot.cpp
		dev/lib/geomatics/AntexHeader.cpp
		dev/lib/vdraw/TextStyle.cpp
		dev/lib/geomatics/AntexHeader.cpp
		dev/lib/rxio/FFIdentifier.cpp
		dev/lib/rxio/miscenum.hpp
		dev/lib/rxio/ATSData.hpp
		dev/lib/rxio/AshtechALB.hpp
		dev/lib/rxio/AshtechData.hpp
			Code Changes to fix various Warnings on various supported platforms
		
		dev/lib/geodyn/IERSConventions.cpp
			Updates Leap History Map and UTC Timesystem initializations to prevent 
			comparison exceptions.
			
		dev/lib/rxio/MDPObsEpoch.cpp
			Fixed bug in reading epochs
			
		dev/lib/rxio/ObsUtils.cpp
		dev/lib/rxio/ObsUtils.hpp
		dev/lib/rxio/RinexConverters.cpp
		dev/lib/rxio/miscenum.hpp		
			Adding support for ashtech smoothing
			Now supports removing smoothing from ashtech data
		
		dev/lib/rxio/FFIdentifier.cpp
			Correctly detects when a file is not SP3
			
		
			
   - Deleted from lib   
   
		none

   Application modifications
   -------------------------
   - Added to apps
   
		dev/apps/CMakeLists.txt
		dev/apps/DataAvailability/CMakeLists.txt
		dev/apps/MDPtools/CMakeLists.txt
		dev/apps/Rinextools/CMakeLists.txt
		dev/apps/checktools/CMakeLists.txt
		dev/apps/clocktools/CMakeLists.txt
		dev/apps/converters/CMakeLists.txt
		dev/apps/differential/CMakeLists.txt
		dev/apps/difftools/CMakeLists.txt
		dev/apps/filetools/CMakeLists.txt
		dev/apps/geomatics/CMakeLists.txt
		dev/apps/geomatics/JPLeph/CMakeLists.txt
		dev/apps/geomatics/cycleslips/CMakeLists.txt
		dev/apps/geomatics/kalman/CMakeLists.txt
		dev/apps/geomatics/relposition/CMakeLists.txt
		dev/apps/geomatics/robust/CMakeLists.txt
		dev/apps/ionosphere/CMakeLists.txt
		dev/apps/mergetools/CMakeLists.txt
		dev/apps/multipath/CMakeLists.txt
		dev/apps/performance/CMakeLists.txt
		dev/apps/positioning/CMakeLists.txt
		dev/apps/receiver/CMakeLists.txt
		dev/apps/reszilla/CMakeLists.txt
		dev/apps/rfw/CMakeLists.txt
		dev/apps/swrx/CMakeLists.txt
		dev/apps/time/CMakeLists.txt
		dev/apps/visibility/CMakeLists.txt
			Added the apps to the CMake build.
			
		dev/apps/reszilla/.gitignore
			.gitignore added
		
		dev/apps/filetools/RinexElvStrip.cpp
			Add a tool to strip RINEX obs data by elevation
		
   - Modified in apps

		dev/apps/filetools/Jamfile
			Add a tool to strip RINEX obs data by elevation
			
		dev/apps/DataAvailability/Makefile.am
		dev/apps/MDPtools/Makefile.am
		dev/apps/ObsArrayEvaluator/Makefile.am
		dev/apps/Rinextools/Makefile.am
		dev/apps/checktools/Makefile.am
		dev/apps/clocktools/Makefile.am
		dev/apps/converters/Makefile.am
		dev/apps/differential/Makefile.am
		dev/apps/difftools/Makefile.am
		dev/apps/filetools/Makefile.am
		dev/apps/geomatics/JPLeph/Makefile.am
		dev/apps/geomatics/cycleslips/Makefile.am
		dev/apps/geomatics/kalman/Makefile.am
		dev/apps/geomatics/relposition/Makefile.am
		dev/apps/geomatics/robust/Makefile.am
		dev/apps/ionosphere/Makefile.am
		dev/apps/mergetools/Makefile.am
		dev/apps/multipath/Makefile.am
		dev/apps/performance/Makefile.am
		dev/apps/positioning/Makefile.am
		dev/apps/qa/Makefile.am
		dev/apps/receiver/Makefile.am
		dev/apps/reszilla/Makefile.am
		dev/apps/rfw/Makefile.am
		dev/apps/swrx/Makefile.am
		dev/apps/time/Makefile.am
		dev/apps/visibility/Makefile.am
			Fixed Make build files, code format cleanup, TimeSystem error

		dev/apps/clocktools/ORDPhaseParser.cpp
		dev/apps/clocktools/dallandev.cpp
		dev/apps/clocktools/ffp.cpp
		dev/apps/clocktools/mallandev.cpp
		dev/apps/clocktools/nallandev.cpp
		dev/apps/clocktools/oallandev.cpp
		dev/apps/clocktools/ohadamarddev.cpp
		dev/apps/clocktools/pff.cpp
		dev/apps/clocktools/rmoutlier.cpp
		dev/apps/clocktools/scale.cpp
		dev/apps/clocktools/tallandev.cpp
		dev/apps/geomatics/robust/rstats.cpp
		dev/apps/positioning/rinexpvt.cpp
		dev/apps/rfw/DeviceStream.hpp
		dev/apps/swrx/CodeGenerator.hpp
		dev/apps/visibility/compSatVis.cpp
		dev/apps/visibility/compStaVis.cpp
		dev/apps/MDPtools/mdp2fic.cpp
		dev/apps/Rinextools/RinDump.cpp
		dev/apps/Rinextools/RinEdit.cpp
		dev/apps/Rinextools/RinNav.cpp
		dev/apps/Rinextools/RinSum.cpp
		dev/apps/geomatics/JPLeph/testSSEph.cpp
		dev/apps/positioning/PRSolve.cpp
		dev/apps/Rinextools/RinSum.cpp
		dev/apps/clocktools/dallandev.cpp
		dev/apps/clocktools/mallandev.cpp
		dev/apps/clocktools/nallandev.cpp
		dev/apps/clocktools/oallandev.cpp
		dev/apps/clocktools/ohadamarddev.cpp
		dev/apps/clocktools/tallandev.cpp
		dev/apps/swrx/gpsSim.cpp		
		dev/apps/swrx/IQStream.cpp
		dev/apps/reszilla/OrdEngine.cpp
		dev/apps/reszilla/ddGen.cpp
			Code Changes to fix various Warnings on various supported platforms 
				
		dev/apps/reszilla/OrdApp.cpp
		dev/apps/reszilla/OrdEngine.cpp
		dev/apps/reszilla/OrdEngine.hpp
		dev/apps/reszilla/ordGen.cpp
		dev/apps/reszilla/ordPlot
			ordGen now handles two frequency combos other than L1-L2 better. 
			Fixing some obs combos
			
		dev/apps/Rinextools/RinDump.cpp
			Add GPS Nav support and debiasing
			
		dev/apps/converters/ash2mdp.cpp
			Adding support for ashtech smoothing
			Now supports removing smoothing from ashtech data
			Fixed a week rollover problem
			
		dev/apps/MDPtools/SummaryProc.cpp
		dev/apps/MDPtools/SummaryProc.hpp
		dev/apps/MDPtools/TrackProc.cpp
		dev/apps/MDPtools/TrackProc.hpp
			Removing a 12 channel limit
			
		dev/apps/reszilla/ddrecord.py
		dev/apps/reszilla/ordPlot
			Adding ability to parse stats records
			
		dev/apps/reszilla/ordStats.cpp
			doesn't segfault when no data is provided
			
		dev/apps/visibility/compStaVis.cpp
			Fixed CompStaVis debug mesage time display error
			
		dev/apps/difftools/DiffFrame.hpp
		dev/apps/difftools/ficdiff.cpp
			Added notification for missing input files
			
		dev/apps/geomatics/relposition/ReadRawData.cpp
			Fixed reference to global SolutionEpoch variable in ReadRawData.
		
		dev/apps/geomatics/relposition/CommandInput.cpp		
			Time system mismatch fix.
		
		dev/apps/geomatics/relposition/ProcessRawData.cpp
			Fix for DDBase in ProcessRawData.cpp - CommonTime TimeSystem mismatch corrected.
		
		dev/apps/clocktools/ffp.cpp
		dev/apps/clocktools/pff.cpp
			Added usage information and examples for ffp.cpp and pff.cpp
		
		dev/apps/mergetools/mergeFIC.cpp
			Add blockNum tests to Block 62 & 162 unique methods in FICFilterOperators to fix seg fault in mergeFIC.
		
		dev/apps/MDPtools/mdp2rinex.cpp
		dev/apps/MDPtools/mdpEdit.cpp
		dev/apps/MDPtools/mdpScrubber.cpp
			Fixed a bug in filtering by PRN number
			
		dev/apps/visibility/compSatVis.cpp
			fixed a time printing error in the output.
			
		dev/apps/converters/ash2xyz.cpp
			fixed a bug where a cleared vector wasn't removed from the map which contains it. 
			
		dev/apps/filetools/ficfica.cpp
			Added basic invalid input file error handling and output dialog to ficfica app
			
		dev/apps/geomatics/cycleslips/DiscFix.cpp
			Fixed a logic bug in ignoring other satellite systems
			
		dev/apps/reszilla/DDEpoch.cpp
		dev/apps/reszilla/PhaseResidual.cpp
			Corrected bug in computing triple difference.

		
   - Deleted from apps
   
		dev/apps/filetools/sp32bc.cpp
			Removed sp32bc from repository			
   
   Example modifications
   -------------------------   
   - Added to examples
   
		dev/examples/CMakeLists.txt
			Added examples to the CMake build.

   - Modified in examples  
   
		/Makefile.am
		    Code changes to fix remaining Windows error & warnings
	
		dev/examples/Jamfile
		    Examples folder Make/Jam file changes

		dev/examples/example14.cpp
			Added examples to the CMake build.
		 
   
   - Deleted from examples  
   
		none

	Testing modifications
   -------------------------   
   - Added to tests

		dev/tests/GPSOrbElemStore/xTestRationalize.cpp
			Adding a program that specifically tests GPSOrbElemStore::rationalize()
   
   - Modified in tests
   
		dev/tests/Makefile.am
			 Fixed Make build files, code format cleanup, TimeSystem error
   
   - Deleted from tests
   
		none
   
	Reference modifications
   -------------------------     

		ref/usersguide/CalcDOPs.tex
		ref/usersguide/CalcDOPs.tex.backup
		ref/usersguide/ConstellationList.tex
		ref/usersguide/ConstellationList.tex.backup
		ref/usersguide/DOPcalc.tex
		ref/usersguide/DOPcalc.tex.backup
		ref/usersguide/DiscFix.tex
		ref/usersguide/DiscFix.tex.backup
		ref/usersguide/FICcheck.tex
		ref/usersguide/FICcheck.tex.backup
		ref/usersguide/IonoBias.tex
		ref/usersguide/NavMerge.tex
		ref/usersguide/PRSolve.tex
		ref/usersguide/PRSolve.tex.backup
		ref/usersguide/README.txt
		ref/usersguide/RINcheck.tex
		ref/usersguide/RINdiff.tex
		ref/usersguide/ResCor.tex
		ref/usersguide/RinDump.tex
		ref/usersguide/RinEdit.tex
		ref/usersguide/RinNav.tex
		ref/usersguide/RinNav.tex.backup
		ref/usersguide/RinSum.tex
		ref/usersguide/RinSum.tex.backup
		ref/usersguide/RinexDump.tex
		ref/usersguide/RinexNav.tex
		ref/usersguide/TECMaps.tex
		ref/usersguide/WhereSat.tex
		ref/usersguide/WhereSat.tex.backup
		ref/usersguide/ashconv.tex
		ref/usersguide/ashconv.tex.backup
		ref/usersguide/ats2mdp.tex
		ref/usersguide/bc2sp3.tex
		ref/usersguide/bc2sp3.tex.backup
		ref/usersguide/bibtexall
		ref/usersguide/bibunits.sty
		ref/usersguide/blankpg.tex
		ref/usersguide/blankpg.tex.backup
		ref/usersguide/calgps.tex
		ref/usersguide/calgps.tex.backup
		ref/usersguide/cmd.tex
		ref/usersguide/colortbl.dtx
		ref/usersguide/colortbl.ins
		ref/usersguide/colortbl.sty
		ref/usersguide/compSatStaVis.tex
		ref/usersguide/compSatStaVis.tex.backup
		ref/usersguide/convcoordtime.tex
		ref/usersguide/convcoordtime.tex.backup
		ref/usersguide/daa.tex
		ref/usersguide/daa.tex.backup
		ref/usersguide/ephdiff.tex
		ref/usersguide/ephdiff.tex.backup
		ref/usersguide/ephsum.tex
		ref/usersguide/ephsum.tex.backup
		ref/usersguide/fancyvrb.dtx
		ref/usersguide/fancyvrb.ins
		ref/usersguide/fancyvrb.sty
		ref/usersguide/fic2rin.tex
		ref/usersguide/fic2rin.tex.backup
		ref/usersguide/ficconv.tex
		ref/usersguide/ficconv.tex.backup
		ref/usersguide/ficdiff.tex
		ref/usersguide/findMoreThan12.tex
		ref/usersguide/gpsfileforms.tex
		ref/usersguide/gpsnutshell.tex
		ref/usersguide/gpstk-user-reference.pdf
		ref/usersguide/gpstk-user-reference.tex
		ref/usersguide/gpstk-user-referenceNewRender.pdf
		ref/usersguide/gpstk.bib
		ref/usersguide/gpstk.bib.back
		ref/usersguide/gpstk.bib.bkp
		ref/usersguide/gpstklogo.jpg
		ref/usersguide/introduction.tex
		ref/usersguide/make.out
		ref/usersguide/makescr
		ref/usersguide/manual.tex
		ref/usersguide/mdpconv.tex
		ref/usersguide/mdptool.tex
		ref/usersguide/mergeFIC.tex
		ref/usersguide/mergeRin.tex
		ref/usersguide/navdmp.tex
		ref/usersguide/navsum.tex
		ref/usersguide/novaRinex.tex
		ref/usersguide/perltex
		ref/usersguide/perltex.sty
		ref/usersguide/perltexout.out
		ref/usersguide/poscvt.tex
		ref/usersguide/poscvt.tex.backup
		ref/usersguide/recurse.sh
		ref/usersguide/reszilla.tex
		ref/usersguide/rinexpvt.tex
		ref/usersguide/rtAshtech.tex
		ref/usersguide/sec2_2.tex
		ref/usersguide/sp32bc.tex
		ref/usersguide/sp3version.tex
		ref/usersguide/src/bibunits.sty
		ref/usersguide/src/texput.lgpl
		ref/usersguide/src/texput.log
		ref/usersguide/svgnam.def
		ref/usersguide/svvis.tex
		ref/usersguide/svvis.tex.backup
		ref/usersguide/tex.out
		ref/usersguide/timeconvert.tex
		ref/usersguide/toolkitportrait.pdf
		ref/usersguide/toolkitportrait.tex
		ref/usersguide/toolkitportrait2.tex
		ref/usersguide/vecsol.tex
		ref/usersguide/x11nam.def
		ref/usersguide/xcolor.dtx
		ref/usersguide/xcolor.ins
		ref/usersguide/xcolor.lox
		ref/usersguide/xcolor.pro
		ref/usersguide/xcolor.sty
		ref/usersguide/xcolor1.tex
		ref/usersguide/xcolor2.tex
		ref/usersguide/xcolor3.tex
		ref/usersguide/xcolor4.tex
			Updated Users Guide to version 3.0 of Rinex, added more usage 
			examples for applications that were lacking them.
   
	#################################################

	
	
   Version 2.0   Thursday, 24 May, 2012

   General modifications
   ---------------------

   - Version 2.0 was branched from dev/ on revision 2537, merged back into the 
     trunk with revision 3140.

   Library modifications
   ---------------------

   - Added to src

       PZ90Ellipsoid.hpp 
       OrbElemStore.hpp       
       GloEphemerisStore.cpp
       GloEphemerisStore.hpp
       GloFreqIndex.cpp
       GloFreqIndex.hpp
       GloRecord.cpp
       GloRecord.hpp
       HelmertTransform.cpp
       HelmertTransform.hpp
       GalEphemeris.cpp
       GalEphemeris.hpp
       GalEphemerisStore.cpp
       GalEphemerisStore.hpp
       GPSAlmanacStore.cpp
       GPSAlmanacStore.hpp
       ReferenceFrame.cpp
       ReferenceFrame.hpp
       Rinex3ClockBase.hpp
       Rinex3ClockData.cpp
       Rinex3ClockData.hpp
       Rinex3ClockHeader.cpp
       Rinex3ClockHeader.hpp
       Rinex3ClockStream.hpp
       Rinex3EphemerisStore.cpp
       Rinex3EphemerisStore.hpp
	   Added handling of time transformations in mixed system processing.

       Rinex3NavBase.hpp
       Rinex3NavData.cpp
       Rinex3NavData.hpp
       Rinex3NavFilterOperators.hpp
       Rinex3NavHeader.cpp
       Rinex3NavHeader.hpp
	  Added handling of time transformations in mixed system processing.

       Rinex3NavStream.hpp
       Rinex3ObsBase.hpp
       Rinex3ObsData.cpp
       Rinex3ObsData.hpp
	   The new 'Rinex3ObsData' method was modified and renamed to improve consistency
	   with the former GPSTk version, and a new method was added. 

       Rinex3ObsFilterOperators.hpp
       Rinex3ObsHeader.cpp
       Rinex3ObsHeader.hpp
       Rinex3ObsStream.hpp
       RinexConverter.cpp
       RinexConverter.hpp
       TimeSystem.cpp
       TimeSystem.hpp
       TimeSystemCorr.hpp
	   Move TimeSystem correction out of Rinex3NavHeader and into its own class,
	   preparatory to implementing time system conversions and doing real mixed-system
	   applications.

       TypeID.cpp
       TypeID.hpp
	   Added all RINEX 3.01 observable types.

       SVNumXRef.cpp
           Updated PRN-SVN numbers through 2011 abd added TimeSystem definition.

       Xt.cpp
       Xt.hpp
       icd_glo_constants.hpp
       icd_glo_freqindex.hpp
       icd_gps_constants.hpp
       MSCStore.cpp
       PRSolution.cpp
       RinexNavFilterOperators.hpp
           Hand-merged src/ directories of the trunk and Rinex-3 branch.

       RinexGloNavData.[h|c]pp
          Added Rinex-2 GLONASS Navigation files data parsers with 
	  documentation.

       RK4VehicleModel.[h|c]pp
	  Added a class that implements a simple model of a vehicle based on 
	  the Runge-Kutta-4 numerical integration algorithm.

      GlonassSatelliteModel.[h|c]pp
	  Added a class that implements a simple model of a GLONASS satellite,
	  inheriting from the 'RK4VehicleModel' class.

      GloBrcEphemerisStore.[h|c]pp
	  Added working version of a class to compute GLONASS satellite 
	  positions based on broadcast ephemeris.

      FIRDifferentiator5thOrder.[h|c]pp
      FilterBase.hpp
	  Added FIR differentiator classes to RC2.0.

      ARBase.[h|c]pp
      ARLambda.[h|c]pp
      ARMLambda.[h|c]pp
      ARSimple.[h|c]pp
	  Ambiguity resolution classes added.

      BRCClockCorrection.[h|c]pp
      BrcKeplerOrbit.[h|c]pp
      Xv.hpp
      EngEphemeris.[h|c]pp
	  Added legacy navigation message classes and modified CNAV/CNAV2
	  classes.

      CNAV2EphClock.[h|c]pp
      CNAVClock.[h|c]pp
      CNAVEphemeris.[h|c]pp
      PackedNavBits.[h|c]pp
	  Added new classes for CNAV and CNAV2 navigation messages.

      GNSSconstants.hpp
	  Condenses constants from all GNSS including GPS, GALILEO, and COMPASS.

   - Modified in src
       
       ANSITime.cpp
       ANSITime.hpp
       AlmOrbit.cpp
       AstronomicalFunctions.hpp
       CivilTime.cpp
       CivilTime.hpp
       CommandOptionParser.hpp
       CommonTime.cpp
       CommonTime.hpp
       DayTime.cpp
       DayTime.hpp
       ECEF.cpp
       ECEF.hpp
       EngEphemeris.cpp
       EngEphemeris.hpp
       EngNav.cpp
       EphemerisRange.cpp 
       EphemerisRange.hpp
       Exception.hpp
       Expression.cpp
       ExtractLC.hpp
       ExtractPC.hpp
       FICData109.cpp
       FICData162.cpp
       FICData62.cpp
       FICData9.cpp
       GPSEllipsoid.hpp
       GPSEphemerisStore.cpp
       GPSEphemerisStore.hpp
       GPSGeoid.hpp
       GPSWeek.cpp
       GPSWeek.hpp
       GPSWeekSecond.cpp
       GPSWeekSecond.hpp
       GPSWeekZcount.cpp
       GPSWeekZcount.hpp
       Geodetic.cpp
       Geodetic.hpp
       IonexData.cpp
       IonexData.hpp
       IonexStore.hpp
       IonoModel.cpp
       IonoModel.hpp
       JulianDate.cpp
       JulianDate.hpp
       MJD.cpp
       MJD.hpp
       MOPSWeight.cpp
       MOPSWeight.hpp
       MSCData.[h|c]pp
       MSCStore.cpp
       MSCStore.hpp
       MatrixBase.hpp
       MoonPosition.hpp
       ObsRngDev.cpp
       ObsRngDev.hpp
       OceanLoading.hpp
       PRSolution.cpp
       PRSolution.hpp
       PoleTides.hpp
       Position.cpp
       Position.hpp
       RACRotation.cpp
       RinexEphemerisStore.cpp
       RinexEphemerisStore.hpp
       RinexMetBase.hpp
       RinexMetData.cpp
       RinexMetData.hpp
       RinexMetFilterOperators.hpp
       RinexMetHeader.cpp
       RinexMetHeader.hpp
       RinexMetStream.hpp
       RinexNavData.cpp
       RinexNavData.hpp
       RinexNavFilterOperators.hpp
       RinexNavHeader.cpp
       RinexObsData.cpp
       RinexObsData.hpp
       RinexObsHeader.cpp
       RinexObsHeader.hpp
       RinexUtilities.cpp
       RinexUtilities.hpp
       RungeKutta4.cpp
       SEMData.cpp
       SEMHeader.cpp
       SP3Base.hpp
       SP3Data.cpp
       SP3Data.hpp
       SP3EphemerisStore.cpp
       SP3EphemerisStore.hpp
       SP3Header.cpp
       SP3Header.hpp
       SP3SatID.hpp
       SP3Stream.hpp
       SimpleIURAWeight.cpp
       SimpleIURAWeight.hpp
       SimpleKalmanFilter.cpp
       SimpleKalmanFilter.hpp
	  Removed the SuperKalmanFilter class and combined the features into
	  SimpleKalmanFilter.

       SolidTides.hpp
       SpecialFunctions.hpp
       SunPosition.hpp
       SystemTime.cpp
       SystemTime.hpp
       TabularEphemerisStore.cpp
       TabularEphemerisStore.hpp
       TimeConstants.hpp
       TimeTag.cpp
       TimeTag.hpp
       TropModel.cpp
       TropModel.hpp
       UnixTime.cpp
       UnixTime.hpp 
       Xvt.cpp 
       Xvt.hpp 
       XvtStore.hpp
       YDSTime.cpp
       YDSTime.hpp
       YumaAlmanacStore.cpp
       YumaAlmanacStore.hpp
       YumaData.cpp
       convhelp.hpp
          Hand-merged src/ directories of the trunk and Rinex-3 branch.

       FFTextStream.hpp
       RinexGloNavHeader.cpp
       RinexGloNavHeader.hpp
       RinexGloNavStream.hpp
          Propagating some of Dagoberto's changes/additions in /src.

       GPSAlmanacStore.[h|c]pp
          Adjusted some functions in GPSAlmanacStore so that it and 
	  YumaAlmanacStore aren't abstract classes.

       RinexGlonavHeader.[h|c]pp
          'RinexGloNavHeader' is now CommonTime-based, plus minor improvements.

       PZ90Ellipsoid.hpp
          Minor updates to the last GLONASS ICD v5.1 (defined in table 3.2)
	  to update it to PZ90.02 ellipsoid.
          Improvements in the documentation, and the addition of the 
	  J20 parameter.

       RinexGloNavData.cpp
	  Corrected errors when reading the 3rd line of a GLONASS
	  broadcast data record.

       RungeKutta4.cpp
	  Changes made in order to make 'RungeKutta4' handle negative steps.

       SP3Header.hpp
       SP3Stream.hpp
	  Minor modifications to silence compiler warnings.

       ObsID.hpp
       Rinex3ObsHeader.[h|c]pp
       RinexSatID.hpp
       SatID.hpp
	  Partial update to RINEX 3.01. 
	  Cf. http://igscb.jpl.nasa.gov/igscb/data/format/rinex301.pdf, 
	  particularly section 9. Updated the Obs header for new records. 
	  Added Compass to RinexSatID and SatID; someone needs to update 
	  ObsID based on Table 5.Did not implement 9.1 in ObsData, 
	  nor attempt to implement 9.2, 9.4, 9.6-8.

       SP3EphemerisStore.[h|c]pp
	  Modified SP3EphemerisStore to use either the SP3 clocks or
	  Rinex clock files.  Added classes for Rinex clock files RinexClock*;
	  these are very similar to, and will replace Rinex3Clock* classes.

       FileStore.hpp
       GPSEphemerisStore.hpp
       GloBrcEphemerisStore.cpp
       GloEphemerisStore.cpp
	  Modified FileStore to stand alone, and modify the ephemeris store
	  classes to contain FileStore(s) rather than inherit FileStore.
	  Redesign the tabular ephemeris store classes based on 
	  TabularSatStore; eventually TabularEphemerisStore will be deleted. 
	  Add classes ClockSatStore and PositionSatStore, which are 
	  standalone functional stores, derived from TabularSatStore, 
	  which are not associated with any file type.Added 'findEphemeris()' methods to
	  'GloEphemerisStore' in order to keep consistency with 'GPSEphemerisStore' and
 	  'GalEphemerisStore' classes.

       AlmOrbit.cpp
       ENUUtil.cpp
       EngEphemeris.cpp
       EngEphemeris.hpp
          Removed protection on EngEphemeris variables to make it easier to convert from
 		  Rinex3NavData to EngEphemeris objects.

       EphemerisRange.cpp
       EphemerisRange.hpp
       GPSAlmanacStore.hpp
       GalEphemeris.cpp
       GloRecord.cpp
       GloRecord.hpp
       IonoModel.cpp
       MSCData.cpp
       MSCData.hpp
       NEDUtil.cpp
       ObsRngDev.cpp
       ObsRngDev.hpp
       OrbElemStore.hpp
       PRSolution.cpp
       RACRotation.cpp
       Rinex3ObsHeader.cpp
       RinexNavFilterOperators.hpp
       RinexUtilities.cpp
       RinexUtilities.hpp
       SP3EphemerisStore.cpp
       SP3EphemerisStore.hpp
       StringUtils.hpp
       TropModel.cpp
       Xt.cpp
       Xt.hpp
       Xvt.cpp
       Xvt.hpp
       XvtStore.hpp
	  Change slightly XvtStore.hpp; this affects the interface of all 
	  ephemeris store class.  Modify Xvt to separate 
	  the relativity correction from the clock bias, and rename dtime 
	  clkbias, and ddtime clkdrift; modify all of src and some apps to 
	  account for this change.

       ClockSatStore.cpp
       ClockSatStore.hpp
       PositionSatStore.cpp
       PositionSatStore.hpp
       RinexClockBase.hpp
       RinexClockData.cpp
       RinexClockData.hpp
       RinexClockHeader.cpp
       RinexClockHeader.hpp
       RinexClockStore.hpp
       RinexClockStream.hpp
       TabularSatStore.hpp
	  Redesign the tabular ephemeris store
          classes based on TabularSatStore; eventually TabularEphemerisStore
          will be deleted. Add classes ClockSatStore and PositionSatStore,
          which are standalone functional stores, derived from TabularSatStore,
          which are not associated with any file type.  

       IonoModel.hpp
       IonoModelStore.[h|c]pp
	  Converted from Geodetic (deprecated) to Position, and adapted to
	  CommonTime.

       ObsID.[h|c]pp
       Rinex3ObsHeader.cpp
       ModeledPseudorangeBase.hpp
	  Files adapted to new classes from obsoleted classes in RC2.0.

       GloFreqIndex.[h|c]pp
       RinexConverter.cpp
       SP3EphemerisStore.hpp
       SystemTime.hpp
	  Windows fixes.

       AntexReader.[h|c]pp
	  Silence some warnings issued by MS compilers regarding the length
	  of map names.

       Rinex3ObsHeader.cpp
       SP3EphemerisStore.[h|c]pp
       TabularSatStore.hpp
	  Added a flag to reject predicted ephemeris data (ultrarapid IGS);
	  fixed error omitting system character from obsid, and other minor
	  improvements.

       StringUtils.hpp
	  Added split() function, fixed warnings related to comparisons
	  between signed and unsigned integer expressions.

       icd_gps_constants.hpp
	  Added L5_MULT constant.

       Rinex3NavData.[h|c]pp
	  MFtime must be an int, short is too short.

       Rinex3NavHeader.[h|c]pp
	  Add CORR TO TIME SYS record and support for version 3.01.

       Rinex3ObsData.[h|c]pp
	  Add dump() that uses the header.
	 
       RinexClockStore.hpp
       ClockSatStore.hpp
       SP3EphemerisStore.[h|c]pp
       TabularSatStore.hpp
	  Remove nominal time step as member data, replacing with a 'compute
	  timestep (sat)' function.

       TabularEphemerisStore.hpp
	  Fixed dummy methods which were not returning the expected types.

      PreciseRange.[h|c]pp
	  Fix for new definitions in Xvt; remove functions here to 
	  compute rel. corr.

      MatrixBase.hpp
	  Removed macros that were commented out and forgotten.

      RinexNavData.cpp
      SP3Data.hpp
      SP3Stream.hpp
	  Removed unused 'headerRead' from SP3Stream.  

      SP3Header.cpp
	  SP3Header was not saving the header to stream in reallyPutRecord'.

      BrcClockCorrection.[h|c]pp
      BrcKeplerOrbit.[h|c]pp
      EngEphemeris.[h|c]pp
      ObsID.hpp
	  Added modified navigation message classes.

      Epoch.[h|c]pp
      MJD.hpp
	  Templates must be defined in .hpp.

       MSCStore.[h|c]pp
          'getXvt' and 'getXt' require reference in calling arguments to match
          XvtStore interface.

       ObsID.cpp
	  Added a description for otNavMsg.

       AlmOrbit.[h|c]pp
       Antenna.hpp
       AntexReader.[h|c]pp
       AstronomicalFunctions.[h|c]pp
       BinexData.cpp
       ClockModel.hpp
       ClockSatStore.[h|c]pp
       CodeBuffer.[h|c]pp
       CommandOptionWithPositionArg.cpp
       CommandOptionWithTimeArg.[h|c]pp
       DCBDataReader.hpp
       EngAlmanac.[h|c]pp
       EngEphemeris.cpp
       Epoch.hpp
       EpochClockModel.hpp
       EpochDataStore.[h|c]pp
       FICData.[h|c]pp
       FICData.hpp
       FICData109.cpp
       FICData162.cpp
       FICData62.cpp
       FICFilterOperators.hpp
       FileFilterFrame.hpp
       FileFilterFrameWithHeader.hpp
       FileHunter.[h|c]pp
       FileSpec.[h|c]pp
       GalEphemeris.cpp
       IonexData.[h|c]pp
       IonexHeader.[h|c]pp
       IonexStore.[h|c]pp
       IonoModel.hpp
       LinearClockModel.[h|c]pp
       Logger.[h|c]pp
       MSCData.[h|c]pp
       MSCData.hpp
       MoonPosition.[h|c]pp
       ORDEpoch.hpp
       ObsEpochMap.hpp
       ObsRngDev.[h|c]pp
       OceanLoading.[h|c]pp
       OceanLoading.hpp
       PoleTides.[h|c]pp
       Position.[h|c]pp
       PositionSatStore.[h|c]pp
       RTFileFrame.hpp
       RinexClockData.[h|c]pp
       RinexClockHeader.cpp
       RinexClockStore.hpp
       RinexUtilities.cpp
       SEMAlmanacStore.[h|c]pp
       SEMAlmanacStore.hpp
       SMODFData.[h|c]pp
       SVExclusionList.[h|c]pp
       SVNumXRef.[h|c]pp
       SVPCodeGen.[h|c]pp
       SatDataReader.[h|c]pp
       SatDataReader.hpp
       SolidTides.[h|c]pp
       StringUtils.hpp
       SunPosition.[h|c]pp
       TabularSatStore.hpp
       TimeNamedFileStream.hpp
       WxObsMap.[h|c]pp
       EngEphemeris.[h|c]pp
       GPSAlmanacStore.[h|c]pp
       GPSEphemerisStore.[h|c]pp
       GalEphemeris.[h|c]pp
       GalEphemerisStore.[h|c]pp
       GloEphemerisStore.[h|c]pp
       HelmertTransform.[h|c]pp
       MSCStore.[h|c]pp
       OrbElemStore.hpp
       ReferenceFrame.hpp
       SP3EphemerisStore.[h|c]pp
       TabularEphemerisStore.hpp
       Xvt.hpp
       XvtStore.hpp
	  Removed dependence of src directory from any instances of Xt.

       GloEphemerisStore.[h|c]pp
	  Updated and corrected GloEphemerisStore.

       EngEphemeris.cpp
	  Added modifications (Mendiola).

       BrcKeplerOrbit.[h|c]pp
       icd_gps_constants.hpp
	  Added modifications to navigation message classes.

       RinexObsHeader.cpp
	  Commented out a debugging line.

       Xvt.[h|c]pp
	  Added preciseRho function which was originally in the now 
	  obsoleted class, Xt.

       Rinex3NAVData.cpp
	  Added Geosync or SBAS system.

       RinexGloNavData.[h|c]pp
       RinexGloNavHeader.[h|c]pp
       GloEphemerisStore.[h|c]pp
       RK4VehicleModel.[h|c]pp
	  Fixed ID expansion.

       AlmOrbit.cpp
       AstronomicalFunctions.hpp
       BrcClockCorrection.cpp
       BrcKeplerOrbit.cpp
       CNAV2EphClk.cpp
       CNAVClock.cpp
       CNAVEphemeris.cpp
       EngEphemeris.cpp
       EngNav.cpp
       EphemerisRange.cpp
       Expression.cpp
       ExtractLC.hpp
       ExtractPC.hpp
       FICData109.cpp
       FICData162.cpp
       FICData62.cpp
       FICData9.cpp
       GPSEllipsoid.hpp
       GPSGeoid.hpp
       GalEphemeris.cpp
       IonexStore.hpp
       IonoModel.cpp
       MOPSWeight.hpp
       MoonPosition.hpp
       ObsRngDev.cpp
       OceanLoading.hpp
       PRSolution.[h|c]pp
       PackedNavBits.cpp
       PoleTides.hpp
       Position.cpp
       Rinex3NavData.cpp
       RinexEphemerisStore.cpp
       RinexNavData.cpp
       SEMData.cpp
       SEMHeader.cpp
       SimpleIURAWeight.hpp
       SolidTides.hpp
       SpecialFunctions.hpp
       SunPosition.hpp
       TabularEphemerisStore.hpp
       TropModel.[h|c]pp
       Xvt.[h|c]pp
       YumaData.cpp
       icd_glo_freqindex.hpp
       BrcClockCorrection.hpp
       BrcKeplerOrbit.hpp
       GloFreqIndex.cpp
	  Changed constants found within source files to match new constants
	  in GNSSconstants.hpp.

       Xvt.[h|c]pp
	  Moved constants (GNSSconstants.hpp) from Xvt.hpp to Xvt.cpp and
	  updated this class.

       GalEphemerisStore.hpp
	  Implemented two small pure virtual functions.

       BrcClockCorrection.cpp
       EphemerisRange.cpp
       Expression.cpp
       GNSSconstants.hpp
       GPSEllipsoid.hpp
       GPSGeoid.hpp
       IonoModel.cpp
       PRSolution.cpp
       Rinex3ClockData.hpp
       Rinex3ClockHeader.cpp
       Xvt.hpp
	  Changed 'C_GPS_MPS' instances to 'C_MPS'.

       ANSITime.[h|c]pp
       ARBase.[h|c]pp
       ARLambda.[h|c]pp
       ARMLambda.[h|c]pp
       ARSimple.[h|c]pp
       AlmOrbit.[h|c]pp
       Antenna.[h|c]pp
       AntexReader.[h|c]pp
       AstronomicalFunctions.[h|c]pp
       BLQDataReader.hpp
       Bancroft.[h|c]pp
       BaseDistribution.hpp
       BasicFramework.[h|c]pp
       BinUtils.[h|c]pp
       BinexData.[h|c]pp
       BinexFilterOperators.hpp
       BinexStream.hpp
       BivarStats.hpp
       BrcClockCorrection.[h|c]pp
       BrcKeplerOrbit.[h|c]pp
       CNAV2EphClk.[h|c]pp
       CNAVClock.[h|c]pp
       CNAVEphemeris.[h|c]pp
       CheckPRData.hpp
       Chi2Distribution.[h|c]pp
       CivilTime.[h|c]pp
       ClockModel.hpp
       CodeBuffer.[h|c]pp
       CommandOption.[h|c]pp
       CommandOptionParser.[h|c]pp
       CommandOptionWithCommonTimeArg.[h|c]pp
       CommandOptionWithPositionArg.[h|c]pp
       CommandOptionWithTimeArg.[h|c]pp
       CommonTime.[h|c]pp
       ConfDataReader.[h|c]pp
       DCBDataReader.[h|c]pp
       DOP.[h|c]pp
       DayTime.[h|c]pp
       ENUUtil.[h|c]pp
       EllipsoidModel.hpp
       EngAlmanac.[h|c]pp
       EngEphemeris.[h|c]pp
       EngNav.[h|c]pp
       EphemerisRange.[h|c]pp
       Epoch.[h|c]pp
       EpochClockModel.hpp
       EpochDataStore.[h|c]pp
       Exception.[h|c]pp
       Expression.[h|c]pp
       ExtractC1.hpp
       ExtractCombinationData.hpp
       ExtractD1.hpp
       ExtractD2.hpp
       ExtractData.hpp
       ExtractL1.hpp
       ExtractL2.hpp
       ExtractLC.hpp
       ExtractP1.hpp
       ExtractP2.hpp
       ExtractPC.hpp
       FFBinaryStream.hpp
       FFData.[h|c]pp
       FFStream.[h|c]pp
       FFStreamError.hpp
       FFTextStream.hpp
       FICAStream.hpp
       FICBase.hpp
       FICData.[h|c]pp
       FICData109.[h|c]pp
       FICData162.[h|c]pp
       FICData62.[h|c]pp
       FICData62.hpp
       FICData9.[h|c]pp
       FICFilterOperators.hpp
       FICHeader.[h|c]pp
       FICStream.hpp
       FICStreamBase.hpp
       FIRDifferentiator5thOrder.[h|c]pp
       FileFilter.hpp
       FileFilterFrame.hpp
       FileFilterFrameWithHeader.hpp
       FileHunter.[h|c]pp
       FileSpec.[h|c]pp
       FileStore.hpp
       FileUtils.hpp
       FilterBase.hpp
       GNSSconstants.hpp
       GPSAlmanacStore.[h|c]pp
       GPSEllipsoid.hpp
       GPSEphemerisStore.[h|c]pp
       GPSGeoid.hpp
       GPSWeek.[h|c]pp
       GPSWeekSecond.[h|c]pp
       GPSWeekZcount.[h|c]pp
       GPSZcount.[h|c]pp
       GalEphemeris.[h|c]pp
       GalEphemerisStore.[h|c]pp
       GaussianDistribution.[h|c]pp
       GenXSequence.[h|c]pp
       GeoidModel.hpp
       GloEphemerisStore.[h|c]pp
       GloFreqIndex.[h|c]pp
       HelmertTransform.[h|c]pp
       IonexData.[h|c]pp
       IonexStore.hpp
       IonexStream.hpp
       IonoModel.[h|c]pp
       IonoModelStore.[h|c]pp
       JulianDate.[h|c]pp
       LinearClockModel.[h|c]pp
       Logger.hpp
       LoopedFramework.[h|c]pp
       MJD.[h|c]pp
       MOPSWeight.[h|c]pp
       MSCData.[h|c]pp
       MSCStore.[h|c]pp
       MSCStream.hpp
       MainAdapter.hpp
       MathBase.hpp
       Matrix.hpp
       MatrixBase.hpp
       MatrixBaseOperators.hpp
       MatrixFunctors.hpp
       MatrixImplementation.hpp
       MatrixOperators.hpp
       MiscMath.hpp
       ModeledPseudorangeBase.hpp
       MoonPosition.hpp
       NEDUtil.[h|c]pp
       ORDEpoch.hpp
       ObsClockModel.[h|c]pp
       ObsEpochMap.[h|c]pp
       ObsID.[h|c]pp
       ObsRngDev.[h|c]pp
       OceanLoading.[h|c]pp
       OrbElemStore.hpp
       PCodeConst.hpp
       PRSolution.[h|c]pp
       PZ90Ellipsoid.hpp
       PackedNavBits.[h|c]pp
       PoleTides.hpp
       PolyFit.hpp
       Position.[h|c]pp
       RACRotation.[h|c]pp
       RK4VehicleModel.[h|c]pp
       RTFileFrame.hpp
       ReferenceFrame.[h|c]pp
       Rinex3ClockBase.[h|c]pp
       Rinex3ClockData.[h|c]pp
       Rinex3ClockHeader.[h|c]pp
       Rinex3ClockStream.hpp
       Rinex3NavBase.hpp
       Rinex3NavData.[h|c]pp
       Rinex3NavFilterOperators.hpp
       Rinex3NavHeader.[h|c]pp
       Rinex3NavStream.hpp
       Rinex3ObsBase.hpp
       Rinex3ObsData.[h|c]pp
       Rinex3ObsFilterOperators.hpp
       Rinex3ObsHeader.[h|c]pp
       Rinex3ObsStream.hpp
       RinexConverter.[h|c]pp
       RinexEphemerisStore.[h|c]pp
       RinexGloNavData.[h|c]pp
       RinexGloNavHeader.[h|c]pp
       RinexGloNavHeader.hpp
       RinexGloNavStream.hpp
       RinexMetBase.hpp
       RinexMetData.[h|c]pp
       RinexMetFilterOperators.hpp
       RinexMetHeader.[h|c]pp
       RinexMetStream.hpp
       RinexNavBase.[h|c]pp
       RinexNavData.[h|c]pp
       RinexNavFilterOperators.hpp
       RinexNavHeader.[h|c]pp
       RinexNavStream.hpp
       RinexObsBase.hpp
       RinexObsData.[h|c]pp
       RinexObsFilterOperators.hpp
       RinexObsHeader.[h|c]pp
       RinexObsID.[h|c]pp
       RinexObsStream.hpp
       RinexSatID.[h|c]pp
       RinexUtilities.[h|c]pp
       RungeKutta4.[h|c]pp
       SMODFData.[h|c]pp
       SMODFStream.hpp
       SP3Base.hpp
       SP3Data.[h|c]pp
       SP3EphemerisStore.[h|c]pp
       SP3Header.[h|c]pp
       SP3SatID.[h|c]pp
       SP3Stream.hpp
       SVExclusionList.[h|c]pp
       SVNumXRef.[h|c]pp
       SVPCodeGen.[h|c]pp
       SatDataReader.[h|c]pp
       SatID.hpp
       SimpleIURAWeight.[h|c]pp
       SimpleKalmanFilter.hpp
       SolidTides.hpp
       SolverBase.hpp
       SourceID.hpp
       SpecialFunctions.[h|c]pp
       Stats.hpp
       StringUtils.hpp
       StudentDistribution.[h|c]pp
       SunPosition.[h|c]pp
       SuperKalmanFilter.[h|c]pp
       SystemTime.[h|c]pp
       TabularEphemerisStore.hpp
       TimeConstants.hpp
       TimeConverters.[h|c]pp
       TimeNamedFileStream.hpp
       TimeString.[h|c]pp
       TimeSystem.[h|c]pp
       TimeTag.[h|c]pp
       Triple.[h|c]pp
       TropModel.[h|c]pp
       TypeID.[h|c]pp
       UnixTime.[h|c]pp
       ValidType.hpp
       Vector.hpp
       VectorBase.[h|c]pp
       VectorBaseOperators.hpp
       VectorOperators.hpp
       WGS84Ellipsoid.hpp
       WGS84Geoid.hpp
       WeightBase.hpp
       WxObsMap.[h|c]pp
       X1Sequence.[h|c]pp
       X2Sequence.[h|c]pp
       Xt.[h|c]pp
       Xv.hpp
       Xvt.[h|c]pp
       XvtStore.hpp
       YDSTime.[h|c]pp
       YumaAlmanacStore.[h|c]pp
       convhelp.hpp
       geometry.hpp
       gps_constants.hpp
       icd_200_constants.hpp
       icd_glo_constants.hpp
       icd_glo_freqindex.hpp
       icd_gps_constants.hpp
       stl_helpers.hpp
	  Ran svn propset for ID and for eol-style on all files.

   - Deleted from src

       TabularEphemerisStore.cpp
         This class now obsoleted; functions reimplemented in new Ephemeris 
	 Store classes (SP3).

       ECEF.[h|c]pp
       Geodetic.[h|c]pp
	  Removed ECEF and Geodetic (which are deprecated), and 
	  modified code to use Position instead.

       EngEphemeris.cpp
	  Added modifications (Mendiola).

   - Added to lib

       /geomatics/AntennaStore.hpp
       /geomatics/KalmanFilter.hpp
       /geomatics/OceanLoading.cpp
       /geomatics/OceanLoading.hpp
       /geomatics/SolidEarthTides.cpp
       /geomatics/SolidEarthTides.hpp
       /geomatics/SpecialFunctions.hpp
       /geomatics/singleton.hpp
          Added new files to lib/geomatics from Rinex-3 branch.

       /procframe/ConstraintSystem.[h|c]pp
       /procframe/Variable.[h|c]pp
	  Added 'ConstraintSystem' class to RC2.0, as well as the appropriate
	  changes in 'Variable' class.

       /procframe/GeneralEquations.[h|c]pp
	  Added class 'GeneralEquations' to RC2.0.  The original 
	  version had very long inline methods, which are not recommended; 
	  therefore, a 'GeneralEquations.cpp' file was added.

       /procframe/Differentiator.[h|c]pp
	  Added 'Differentiator' class to RC2.0.

       /procframe/EquationSystem.[h|c]pp
       /procframe/GeneralConstraint.[h|c]pp
       /procframe/SolverGeneral.[h|c]pp
	  Added 'GeneralConstraint' class, as well as additions to related 
	  classes.

   - Modified in lib

       /geomatics/SunEarthSatGeometry.cpp
       /geomatics/PhaseWindup.cpp
       /geomatics/PreciseRange.cpp
       /geomatics/SatPass.cpp
       /geomatics/SolarPosition.cpp
       /geomatics/DiscCorr.cpp
       /geomatics/EarthOrientation.cpp
       /geomatics/GeodeticFrames.cpp
       /rxio/AshtechMBEN.cpp
       /rxio/EphReader.cpp
       /rxio/FFIdentifier.cpp
       /rxio/MDPPVTSolution.hpp
       /rxio/NovatelData.cpp
       /rxio/ObsUtils.cpp
          Merged the trunk and Rinex-3 version of lib/rxoi and lib/geomatics.

       /geodyn/IERS.cpp
       /geomatics/PhaseWindup.cpp
       /geomatics/PreciseRange.cpp
       /procframe/CorrectCodeBiases.cpp
       /procframe/DataStructures.hpp
       /procframe/IonexModel.cpp
       /procframe/LinearCombinations.hpp
       /procframe/ModeledReferencePR.hpp
         Renaming some instances of icd_gps_constants.

       /rxio/EphReader.cpp
       /rxio/FFIdentifier.cpp
       /rxio/MDPPVTSolution.hpp
	  Modified library to reflect more meaningful constants' names in 
	  XvtStore.

       /procframe/DataHeaders.[h|c]pp
       /procframe/DataStructures.[h|c]pp
	  Changed to CommonTime.

       /procframe/ProcessingClass.hpp
          The 'getIndex()' method and its associated operators were deleted 
	  from 'ProcessingClass'. This feature made developing more complex, 
	  it potentially limited the compilation on parallel machines, and 
	  it seems that no one was using it.

       /procframe/BasicModel.cpp
       /procframe/BasicModel.hpp
       /procframe/ComputeDOP.cpp
       /procframe/ComputeDOP.hpp
       /procframe/CodeKalmanSolver.cpp
       /procframe/CodeKalmanSolver.hpp
       /procframe/CodeSmoother.cpp
       /procframe/CodeSmoother.hpp
       /procframe/DataHeaders.cpp
       /procframe/DataHeaders.hpp
       /procframe/DataStructures.cpp
       /procframe/DataStructures.hpp
       /procframe/ComputeCombination.cpp
       /procframe/ComputeCombination.hpp
       /procframe/ComputeIURAWeights.cpp
       /procframe/ComputeIURAWeights.hpp
       /procframe/ComputeIonoModel.cpp
       /procframe/ComputeIonoModel.hpp
       /procframe/ComputeLC.cpp
       /procframe/ComputeLC.hpp
       /procframe/ComputeLI.cpp
       /procframe/ComputeLI.hpp
       /procframe/ComputeLdelta.cpp
       /procframe/ComputeLdelta.hpp
       /procframe/ComputeLinear.cpp
       /procframe/ComputeLinear.hpp
       /procframe/ComputeMelbourneWubbena.cpp
       /procframe/ComputeMelbourneWubbena.hpp
       /procframe/ComputeIonoModel.cpp
       /procframe/ComputeIonoModel.hpp
       /procframe/ComputeMOPSWeights.cpp
       /procframe/ComputeMOPSWeights.hpp
       /procframe/ComputeSimpleWeights.[h|c]pp
       /procframe/ComputePC.cpp
       /procframe/ComputePC.hpp
       /procframe/ComputePI.cpp
       /procframe/ComputePI.hpp
       /procframe/ComputePdelta.cpp
       /procframe/ComputePdelta.hpp
       /procframe/ComputeSatPCenter.cpp
       /procframe/ComputeSatPCenter.hpp
       /procframe/ComputeTropModel.cpp
       /procframe/ComputeTropModel.hpp
       /procframe/ComputeWindUp.cpp
       /procframe/ComputeWindUp.hpp
       /procframe/ConvertC1ToP1.cpp
       /procframe/ConvertC1ToP1.hpp
       /procframe/CorrectCodeBiases.cpp
       /procframe/CorrectCodeBiases.hpp
       /procframe/CorrectObservables.cpp
       /procframe/CorrectObservables.hpp
       /procframe/Decimate.cpp
       /procframe/Decimate.hpp
       /procframe/DeltaOp.cpp
       /procframe/DeltaOp.hpp
       /procframe/DoubleOp.cpp
       /procframe/DoubleOp.hpp
       /procframe/Dumper.cpp
       /procframe/Dumper.hpp
       /procframe/EclipsedSatFilter.cpp
       /procframe/EclipsedSatFilter.hpp
       /procframe/Equation.cpp
       /procframe/Equation.hpp
       /procframe/GravitationalDelay.cpp
       /procframe/GravitationalDelay.hpp
       /procframe/BasicModel.hpp
       /procframe/IonexModel.cpp
       /procframe/IonexModel.hpp
       /procframe/Keeper.cpp
       /procframe/Keeper.hpp
       /procframe/LICSDetector.cpp
       /procframe/LICSDetector.hpp
       /procframe/LICSDetector2.cpp
       /procframe/LICSDetector2.hpp
       /procframe/ModeledPR.cpp
       /procframe/ModeledPR.hpp
       /procframe/ModeledReferencePR.cpp
       /procframe/ModeledReferencePR.hpp
       /procframe/MWCSDetector.cpp
       /procframe/MWCSDetector.hpp
       /procframe/ModelObs.cpp
       /procframe/ModelObs.hpp
       /procframe/ModelObsFixedStation.cpp
       /procframe/ModelObsFixedStation.hpp
       /procframe/NablaOp.cpp
       /procframe/NablaOp.hpp
       /procframe/OneFreqCSDetector.cpp
       /procframe/OneFreqCSDetector.hpp
       /procframe/PCSmoother.cpp
       /procframe/PCSmoother.hpp
       /procframe/PhaseCodeAlignment.cpp
       /procframe/PhaseCodeAlignment.hpp
       /procframe/ProblemSatFilter.cpp
       /procframe/ProblemSatFilter.hpp
       /procframe/ProcessingList.cpp
       /procframe/ProcessingList.hpp
       /procframe/ProcessingVector.cpp
       /procframe/ProcessingVector.hpp
       /procframe/Pruner.cpp
       /procframe/Pruner.hpp
       /procframe/RequireObservables.cpp
       /procframe/RequireObservables.hpp
       /procframe/SatArcMarker.cpp
       /procframe/SatArcMarker.hpp
       /procframe/SimpleFilter.cpp
       /procframe/SimpleFilter.hpp
       /procframe/SolverGeneral.cpp
       /procframe/SolverGeneral.hpp
       /procframe/SolverLMS.cpp
       /procframe/SolverLMS.hpp
       /procframe/SolverPPP.cpp
       /procframe/SolverPPP.hpp
       /procframe/SolverPPPFB.cpp
       /procframe/SolverPPPFB.hpp
       /procframe/SolverWMS.cpp
       /procframe/SolverWMS.hpp
       /procframe/Variable.cpp
       /procframe/Variable.hpp
       /procframe/XYZ2NED.cpp
       /procframe/XYZ2NED.hpp
       /procframe/XYZ2NEU.cpp
       /procframe/XYZ2NEU.hpp
       /procframe/ModeledReferencedPR.[h|c]pp
       /procframe/Synchronize.[h|c]pp
	  Procframe library adapted to new RC2.0 from obsoleted classes.

       /rxio/AshtechData.hpp
       /rxio/AshtechMBEN.hpp
       /rxio/ATSData.cpp
       /rxio/ATSData.hpp
       /rxio/ObsUtils.cpp
       /rxio/EphReader.cpp
       /rxio/EphReader.hpp
       /rxio/Jamfile
       /rxio/MDPHeader.cpp
       /rxio/MDPHeader.hpp
       /rxio/MDPObsEpoch.cpp
       /rxio/MDPPVTSolution.cpp
       /rxio/MDPPVTSolution.hpp
       /rxio/MDPSelftestStatus.cpp
       /rxio/MDPSelftestStatus.hpp
       /rxio/NovatelData.cpp
       /rxio/ObsReader.cpp
       /rxio/ObsUtils.cpp
       /rxio/RinexConverters.cpp
	  Converted rxio library from obsoleted DayTime class, and resolved
	  consequential compile errors.

       /geomatics/AntexData.cpp
       /geomatics/DiscCorr.cpp
       /geomatics/SatPass.cpp
       /geomatics/EarthOrientation.cpp
       /geomatics/GeodeticFrames.[h|c]pp
       /geomatics/PhaseWindup.[h|c]pp
       /geomatics/PreciseRange.[h|c]pp
       /geomatics/SolarPosition.[h|c]pp
       /geomatics/SolarSystem.[h|c]pp
       /geomatics/SolidEarthTides.[h|c]pp
       /geomatics/SunEarthSatGeometry.[h|c]pp	  
	  Converted geomatics library from obsoleted DayTime class, and resolved
	  consequential compile errors.

       /geomatics/SolarSystem.[h|c]pp
	  MJD_TO_JD in TimeConstants is the same (same sign) as 
	  DayTime::JD_TO_MJD.

       /rxio/EphReader.cpp
	  Updated reader for new SP3EphemerisStore.

      /geomatics/GSatID.hpp
	  Added COMPASS.

       /geodyn/ForceModel.hpp
       /geodyn/IERS.cpp
       /geodyn/IERS.hpp
       /geodyn/PvtStore.cpp
       /geodyn/PvtStore.hpp
       /geodyn/ReferenceFrames.cpp
       /geodyn/ReferenceFrames.hpp
       /geodyn/SatOrbitPropagator.cpp
       /geodyn/SatOrbitPropagator.hpp
       /geodyn/SatOrbitStore.cpp
       /geodyn/SatOrbitStore.hpp
       /geodyn/Spacecraft.cpp
       /geodyn/Spacecraft.hpp
       /geodyn/SunForce.cpp
       /geodyn/EarthOceanTide.cpp
       /geodyn/HarrisPriesterDrag.cpp
       /geodyn/Msise00Drag.cpp
       /geodyn/SatOrbitStore.cpp
       /geodyn/SphericalHarmonicGravity.cpp
       /geodyn/UTCTime.cpp
       /geodyn/UTCTime.hpp
          Converted geomatics library from obsoleted DayTime class, and 
	  resolved consequential compile errors.

       /geodyn/IERS.cpp
       /geomatics/DiscCorr.cpp
       /geomatics/EarthOrientation.cpp
       /geomatics/GeodeticFrames.cpp
       /geomatics/PhaseWindup.cpp
       /geomatics/PreciseRange.cpp
       /geomatics/SatPass.cpp
       /geomatics/SolarPosition.cpp
       /geomatics/SunEarthSatGeometry.cpp
       /procframe/BasicModel.cpp
       /procframe/ComputeIonoModel.cpp
       /procframe/ComputeLdelta.cpp
       /procframe/ComputeLdelta.hpp
       /procframe/ComputeMelbourneWubbena.cpp
       /procframe/ComputePdelta.cpp
       /procframe/ComputePdelta.hpp
       /procframe/ConvertC1ToP1.cpp
       /procframe/CorrectCodeBiases.cpp
       /procframe/DataStructures.cpp
       /procframe/DataStructures.hpp
       /procframe/IonexModel.cpp
       /procframe/LinearCombinations.cpp
       /procframe/LinearCombinations.hpp
       /procframe/ModelObsFixedStation.cpp
       /procframe/ModeledReferencePR.cpp
       /procframe/ModeledReferencePR.hpp
       /rxio/AshtechMBEN.cpp
       /rxio/MDPNavSubframe.cpp
       /rxio/MDPObsEpoch.cpp
       /rxio/NovatelData.cpp
       /rxio/ObsUtils.cpp
           Adapting apps from icd_gps_constants.hpp and icd_glo_constants.hpp to
           include constants specified by GNSS tags in GNSSconstants.hpp.

       /geomatics/DiscCorr.cpp
       /geomatics/PreciseRange.cpp
       /geomatics/SatPass.cpp
       /procframe/BasicModel.cpp
       /procframe/ConstraintSystem.hpp
       /procframe/ConvertC1ToP1.cpp
       /procframe/CorrectCodeBiases.cpp
       /procframe/IonexModel.cpp
       /procframe/LinearCombinations.cpp
       /procframe/ModelObsFixedStation.cpp
       /procframe/ModeledReferencePR.cpp
       /rxio/NovatelData.cpp
       /rxio/ObsUtils.cpp
	  Changed instances of 'C_GPS_MPS' to 'C_MPS'.

       /geodyn/SatOrbitStore.cpp
	  Adapting library to CommonTime methods from obsoleted DayTime.
   Applications modifications
   -------------------------

   - Added to apps

       /Rinextools/Rin3Sum.cpp
       /Rinextools/Rinex3Dump.cpp
       /converters/RinexConvert.cpp
       /converters/RinexConvert.hpp
       /positioning/PRSolveR3.cpp
         Added apps from Rinex-3 branch that are hand-merged with trunk.

       /geomatics/JPLeph/JPL
       /geomatics/JPLeph/JPL/ascp1950.403
       /geomatics/JPLeph/JPL/ascp1975.403
       /geomatics/JPLeph/JPL/ascp2000.403
       /geomatics/JPLeph/JPL/ascp2025.403
       /geomatics/JPLeph/JPL/header.403
       /geomatics/JPLeph/JPL/testpo.403
	  Added JPL data for 403 solar system ephemeris.

   - Modified in apps

       /Rinextools/EditRinex.cpp
       /Rinextools/NavMerge.cpp
       /Rinextools/ResCor.cpp
       /Rinextools/RinexDump.cpp
       /Rinextools/RinexEditor.cpp
       /Rinextools/RinexEditor.hpp
       /Rinextools/RinNav.cpp
       /Rinextools/RinSum.cpp
	   Added visibility plot and table to RinSum output
       /converters/ash2xyz.cpp
       /DataAvailability/DataAvailabilityAnalyzer.cpp
       /differential/vecsol.cpp
       /geomatics/relposition/Synchronization.cpp
       /geomatics/relposition/constants.hpp
       /ionosphere/IonoBias.cpp
       /ionosphere/VTECMap.cpp
       /ionosphere/VTECMap.hpp
       /positioning/posInterp.cpp
       /positioning/rinexpvt.cpp
       /positioning/rinexpvt.hpp
       /positioning/PRSolve.cpp
       /receiver/AshtechMessage.cpp
       /reszilla/DDEpoch.hpp
       /reszilla/ordStats.cpp
       /swrx/CCReplica.hpp
       /swrx/EMLTracker.hpp
       /swrx/RX.cpp
       /swrx/codeDump.cpp
       /swrx/corltr.cpp
       /swrx/gpsSim.cpp
       /swrx/position.cpp
       /swrx/tracker.cpp
       /swrx/trackerMT.cpp
       /visibility/WhereSat.cpp
       /visibility/compSatVis.cpp
       /visibility/compStaVis.cpp
       /visibility/compSatVis.cpp
       /visibility/compStaVis.cpp
       /visibility/CalcDOPs.cpp
       /visibility/findMoreThan12.cpp
       /swrx/simpleNav.cpp
       /multipath/mpsolve.cpp
          Hand-merging the trunk and Rinex-3 branch applications.

       /reszilla/OrdEngine.[h|c]pp
       /reszilla/ddGen.cpp
       /reszilla/ordGen.cpp
	  Changing 'GPSGeoid.hpp' to 'GPSEllipsoid.hpp'.

       /MDPtools/MDPProcessors.[h|c]pp
       /MDPtools/SummaryProc.cpp
       /MDPtools/TrackProc.cpp
       /MDPtools/mdpEdit.cpp
       /reszilla/SvElevationMap.[h|c]pp
       /reszilla/ordStats.cpp
       /receiver/rtAshtech.cpp
       /MDPtools/NavProc.cpp
       /multipath/mpsim.cpp
       /ionosphere/TECMaps.cpp
       /checktools/CheckFrame.hpp
       /converters/ash2mdp.cpp
       /DataAvailability/DataAvailabilityAnalyzer.[h|c]pp
       /filetools/bc2sp3.cpp
       /filetools/EphSum.cpp
       /filetools/RinexThin.cpp
       /filetools/fic2rin.cpp
       /filetools/navdmp.cpp
       /filetools/navsum.cpp
       /filetools/sp32bc.cpp
       /filetools/sp3version.cpp
       /geomatics/cycleslips/DiscFix.cpp
       /geomatics/JPLeph/convertSSEph.cpp
       /geomatics/JPLeph/testSSEph.cpp
       /performance/ConstellationDefinition.cpp
       /performance/ConstellationDefinition.hpp
       /performance/ConstellationList.cpp
       /performance/ConstellationSet.[h|c]pp
	   Changed Daytime objects to CommonTime and YDSTime objects.

       /geomatics/relposition/ComputeRAIMSolution.cpp
       /geomatics/relposition/Configure.cpp
       /geomatics/relposition/DDBase.[h|c]pp
       /geomatics/relposition/DataStructures.cpp
       /geomatics/relposition/DoubleDifference.cpp
       /geomatics/relposition/EditDDs.cpp
       /geomatics/relposition/EditRawDataBuffers.cpp
       /geomatics/relposition/EphemerisImprovement.cpp
       /geomatics/relposition/Estimation.cpp
       /geomatics/relposition/ProcessRawData.cpp
       /geomatics/relposition/ReadObsFiles.cpp
       /geomatics/relposition/ReadRawData.cpp
       /geomatics/relposition/Synchronization.cpp
       /geomatics/relposition/Timetable.cpp
       /geomatics/relposition/ddmerge.cpp
       /geomatics/robust/rstats.cpp
       /geomatics/relposition/ClockModel.cpp
       /geomatics/relposition/ComputeRAIMSolution.cpp
       /geomatics/relposition/DDBase.cpp
       /geomatics/relposition/DoubleDifference.cpp
       /geomatics/relposition/EditDDs.cpp
       /geomatics/relposition/Estimation.cpp
       /geomatics/relposition/ProcessRawData.cpp
       /geomatics/relposition/ReadObsFiles.cpp
       /geomatics/relposition/ReadRawData.cpp
       /geomatics/relposition/Synchronization.cpp
       /geomatics/relposition/Timetable.cpp
       /MDPtools/NavProc.cpp
       /qa/obsrip.cpp
       /geomatics/JPLeph/convertSSEph.cpp
       /geomatics/JPLeph/testSSEph.cpp
       /converters/ash2mdp.cpp
       /converters/novaRinex.cpp
       /geomatics/relposition/Configure.cpp
       /geomatics/relposition/DDBase.cpp
       /geomatics/relposition/DDBase.hpp
       /geomatics/relposition/ReadObsFiles.cpp
       /geomatics/relposition/ReadRawData.cpp
       /geomatics/relposition/Timetable.cpp
       /geomatics/relposition/ddmerge.cpp
       /swrx/RX.cpp
       /swrx/position.cpp
       /swrx/simpleNav.cpp
       /positioning/PRSolve.cpp
       /Rinextools/EditRinex.cpp
       /Rinextools/ResCor.cpp
       /Rinextools/Rin3Sum.cpp
       /Rinextools/RinSum.cpp
       /rfw/rfw.cpp
       /positioning/PRSolve.cpp
       /positioning/posInterp.cpp
       /positioning/rinexpvt.[h|c]pp
       /positioning/posInterp.cpp
       /differential/vecsol.cpp
       /difftools/rmwdiff.cpp
       /difftools/rnwdiff.cpp
       /difftools/rowdiff.cpp
       /filetools/EphSum.cpp
       /filetools/RinexThin.cpp
       /filetools/bc2sp3.cpp
       /filetools/fic2rin.cpp
       /filetools/navdmp.cpp
       /filetools/navsum.cpp
       /filetools/sp32bc.cpp
       /geomatics/cycleslips/DiscFix.cpp
       /geomatics/relposition/Configure.cpp
       /geomatics/relposition/DDBase.cpp
       /geomatics/relposition/Estimation.cpp
       /geomatics/relposition/ReadObsFiles.cpp
       /geomatics/relposition/ReadRawData.cpp
	  Adapting apps directory to CommonTime methods from obsoleted DayTime.

       /MDPtools/NavProc.cpp
       /Rinextools/EditRinex.cpp
       /Rinextools/Rin3Sum.cpp
       /visibility/VisSupport.cpp
       /visibility/VisSupport.hpp
       /visibility/compSatVis.cpp
       /visibility/compStaVis.cpp
       /visibility/findMoreThan12
          Modified library to reflect more meaningful constants' names in 
	  XvtStore.

      /filetools/bc2sp3.cpp
      /filetools/sp3version.cpp
	  Updated for new SP3 design.

      /DataAvailability/DataAvailabilityAnalyzer.cpp
      /difftools/DiffFrame.hpp
      /geomatics/relposition/CommandInput.[h|c]pp
      /geomatics/relposition/DataOutput.cpp
      /geomatics/relposition/DataStructures.hpp
      /geomatics/relposition/ReadObsFiles.cpp 
      /geomatics/relposition/Estimation.cpp
      /ionosphere/IonoBias.cpp
      /ionosphere/TECMaps.cpp
      /ionosphere/VTECMap.[h|c]pp
      /MDPtools/BELogEntry.[h|c]pp
      /MDPtools/MDPProcessors.cpp
      /MDPtools/NavProc.cpp
      /MDPtools/ScreenProc.cpp
      /MDPtools/SubframeProc.cpp
      /MDPtoos/SummaryProc.cpp
      /MDPtools/TrackProc.[h|c]pp
      /MDPtools/mdp2rinex.cpp
      /MDPtools/UniqueAlmStore.[h|c]pp
      /MDPtools/mdp2fic.cpp
      /MDPtools/mdp2rinex.cpp
      /MDPtools/mdpEdit.cpp
      /converters/ash2mdp.cpp
      /converters/ash2xyz.cpp
      /converters/ats2mdp.cpp
      /converters/novaRinex.cpp
      /performance/ConstellationDefinition.cpp
      /performance/ConstellationList.cpp
      /performance/ConstellationList.[h|c]pp
      /Rinextools/ResCor.cpp
      /Rinextools/RinexDump.cpp
      /receiver/rtAshtech.cpp
      /reszilla/OrdEngine.[h|c]pp
      /reszilla/DDEpoch.cpp
      /reszilla/OrdApp.cpp
      /reszilla/OrdEngine.cpp
      /reszilla/ordGen.cpp
      /reszilla/ordLinEst.cpp
      /reszilla/PhaseCleaner.cpp
      /reszilla/PhaseResidual.[h|c]pp
      /reszilla/SvElevationMap.cpp
      /reszilla/ddGen.cpp
      /reszilla/ordClock.cpp
      /reszilla/ordEdit.cpp
      /reszilla/ordLinEst.cpp
      /reszilla/ordStats.cpp
      /reszilla/RobustLinearEstimator.[h|c]pp
      /mergetools/mergeRinMet.cpp
      /mergetools/mergeRinNav.cpp
      /mergetools/mergeRinObs.cpp
      /multipath/ObsArray.cpp
      /multipath/mpsolve.cpp
      /multipath/testObsArray.cpp
      /ObsArrayEvaluator/ObsArray.[h|c]pp
      /ObsArrayEvaluator/oae.cpp
      /positioning/PRSolve.cpp
      /positiong/rinexpvt.cpp
      /receiver/AshtechMessage.[h|c]pp
      /performance/ConstellationDefinition.cpp
      /performance/ConstellationList.cpp
      /performance/ConstellationSet.[h|c]ppi
      /swrx/PCodeGenerator.hpp
      /swrx/simpleNav.cpp
      /visibility/CalcDOPs.cpp
      /visibility/DOPcalc.cpp
      /visibility/VisSupport.[h|c]pp
      /visibility/WhereSat.cpp
      /visibility/compSatVis.cpp
      /visibility/compStaVis.cpp
      /visibility/findMoreThan12.cpp
      /visibility/svvis.cpp
	  Converting apps from obsoleted DayTime class, and resolving
	  consequential compile errors.

      /reszilla/CycleSlipList.[h|c]pp
	  Added StringUtils.hpp.

      /swrx/PCodeGenerator.hpp
	  Changed GPSZcount and GPSZcountFloor calls from DayTime to
	  Epoch member functions.

      /MDPtools/mdp2fic.cpp
	  Added RinexConverters header to mdp2fic.cpp.

       /Rinextools/ResCor.cpp
       /Rinextools/Rin3Sum.cpp
       /Rinextools/RinSum.cpp
       /converters/ash2xyz.cpp
       /differential/vecsol.cpp
       /geomatics/relposition/Synchronization.cpp
       /geomatics/relposition/constants.hpp
       /ionosphere/IonoBias.cpp
       /ionosphere/VTECMap.cpp
       /ionosphere/VTECMap.hpp
       /multipath/mpsim.cpp
       /performance/ConstellationDefinition.cpp
       /performance/ConstellationList.cpp
       /positioning/PRSolve.cpp
       /positioning/PRSolveR3.cpp
       /positioning/posInterp.cpp
       /positioning/rinexpvt.cpp
       /positioning/rinexpvt.hpp
       /receiver/AshtechMessage.cpp
       /reszilla/DDEpoch.cpp
       /reszilla/DDEpoch.hpp
       /reszilla/OrdApp.cpp
       /reszilla/PhaseCleaner.cpp
       /reszilla/PhaseCleaner.hpp
       /reszilla/ordLinEst.cpp
       /reszilla/ordStats.cpp
       /swrx/CCReplica.hpp
       /swrx/EMLTracker.hpp
       /swrx/RX.cpp
       /swrx/SVSource.hpp
       /swrx/acquire.cpp
       /swrx/codeDump.cpp
       /swrx/corltr.cpp
       /swrx/gpsSim.cpp
       /swrx/position.cpp
       /swrx/tracker.cpp
       /swrx/trackerMT.cpp
       /visibility/CalcDOPs.cpp
       /visibility/DOPcalc.cpp
       /visibility/compSatVis.cpp
       /visibility/compStaVis.cpp
	  Adapting apps from icd_gps_constants.hpp and icd_glo_constants.hpp to
	  include constants specified by GNSS tags in GNSSconstants.hpp.

       /Rinextools/ResCor.cpp
       /Rinextools/Rin3Sum.cpp
       /Rinextools/RinSum.cpp
       /Rinextools/Rinex3Dump.cpp
       /converters/ash2xyz.cpp
       /differential/vecsol.cpp
       /geomatics/relposition/Synchronization.cpp
       /geomatics/relposition/constants.hpp
       /positioning/PRSolve.cpp
       /positioning/PRSolveR3.cpp
       /positioning/rinexpvt.cpp
       /receiver/AshtechMessage.cpp
       /reszilla/DDEpoch.cpp
       /reszilla/OrdApp.cpp
       /reszilla/PhaseCleaner.[h|c]pp
       /reszilla/ordLinEst.cpp
       /reszilla/ordStats.cpp
       /swrx/RX.cpp
       /swrx/position.cpp
	  Changed instances of 'C_GPS_MPS' to 'C_MPS'.

   - Deleted from apps

       /RinexPlot/
	  Removed obsoleted app.

   Test modifications
   ------------------

   - Added to tests

       /gpsNavMsg/makefile
       /gpsNavMsg/xBrcClockCorrection.cpp
       /gpsNavMsg/xBrcKeplerOrbit.cpp
	  Test code for legacy navigation messages added.

       /HelmertTransform/HelmertTransform.[h|c]pp
       /HelmertTransform/xHelmertTransform.[h|c]pp
       /HelmertTransform/xHelmertTransformM.cpp
       /ReferenceFrame/ReferenceFrame.[h|c]pp
       /ReferenceFrame/xReferenceFrame.[h|c]pp
       /ReferenceFrame/xReferenceFrameM.cpp
       /SP3EphemerisStore/SP3EphemerisStore.[h|c]pp
	  Added new tests from UnitTests in Rinex-3 branch to tests 
	  directory in RC2.0.

       /RinexNav/xRinexTest.cpp
	  Added new tests for the EngEphemeris cast operator, found in 	
	  src/Rinex3NavData.cpp

       /gpsNavMsg/xCNAV2EphClk.cpp
       /gpsNavMsg/xCNAV2EphClockPNB.cpp
       /gpsNavMsg/xCNAVClock.cpp
       /gpsNavMsg/xCNAVClockPNB.cpp
       /gpsNavMsg/xCNAVEphemeris.cpp
       /gpsNavMsg/xCNAVEphemerisPNB.cpp
       /gpsNavMsg/xPackedNavBits.cpp
	  Added test cases for new CNAV classes.

       /gpsNavMsg/Checks/AfterHalfweekchange.txt
       /gpsNavMsg/Checks/BeforeHalfweekchange.txt
       /gpsNavMsg/Checks/BrcClockCorrection_Test_June_16_2011_4pm.txt
       /gpsNavMsg/Checks/BrcKeplerOrbit_Test_June_16_2011_4pm.txt
       /gpsNavMsg/Checks/ClockCorrTest.txt
       /gpsNavMsg/Checks/OrbTest.txt
       /gpsNavMsg/Checks/PackedNavBits_Output.txt
       /gpsNavMsg/Checks/xBrcClkCorr_output_01.txt
       /gpsNavMsg/Checks/xBrcKeplerOrbit_withsetFrame.out.txt
       /gpsNavMsg/Checks/xCEC.txt
       /gpsNavMsg/Checks/xCNAVClock_Output.txt
       /gpsNavMsg/Checks/xCNAVEphemeris_Output.txt
       /gpsNavMsg/Checks/xPackedNavBits_Output.txt
       /gpsNavMsg/Logs/BrcClkCorr_Output
       /gpsNavMsg/Logs/BrcClkCorr_Truth
       /gpsNavMsg/Logs/BrcKeplerOrbit_Output
       /gpsNavMsg/Logs/BrcKeplerOrbit_Truth
       /gpsNavMsg/Logs/CNAV2PNB_Output
       /gpsNavMsg/Logs/CNAV2PNB_Truth
       /gpsNavMsg/Logs/CNAVClockPNB_Output
       /gpsNavMsg/Logs/CNAVClockPNB_Truth
       /gpsNavMsg/Logs/CNAVClock_Output
       /gpsNavMsg/Logs/CNAVClock_Truth
       /gpsNavMsg/Logs/CNAVEphemerisPNB_Output
       /gpsNavMsg/Logs/CNAVEphemerisPNB_Truth
       /gpsNavMsg/Logs/CNAVEphemeris_Output
       /gpsNavMsg/Logs/CNAVEphemeris_Truth
       /gpsNavMsg/Logs/EngEphemeris_Output
       /gpsNavMsg/Logs/EngEphemeris_Truth
       /gpsNavMsg/Logs/Output
       /gpsNavMsg/Logs/PackedNavBits_Output
       /gpsNavMsg/Logs/PackedNavBits_Truth
       /gpsNavMsg/Logs/Truth
       /gpsNavMsg/xBrcClockCorrectiongpsNavMsg.cpp
       /gpsNavMsg/xBrcClockCorrectiongpsNavMsg.hpp
       /gpsNavMsg/xBrcKeplerOrbitgpsNavMsg.cpp
       /gpsNavMsg/xBrcKeplerOrbitgpsNavMsg.hpp
       /gpsNavMsg/xCNAV2PNBgpsNavMsg.cpp
       /gpsNavMsg/xCNAV2PNBgpsNavMsg.hpp
       /gpsNavMsg/xCNAV2gpsNavMsg.cpp
       /gpsNavMsg/xCNAV2gpsNavMsg.hpp
       /gpsNavMsg/xCNAVClockPNBgpsNavMsg.cpp
       /gpsNavMsg/xCNAVClockPNBgpsNavMsg.hpp
       /gpsNavMsg/xCNAVClockgpsNavMsg.cpp
       /gpsNavMsg/xCNAVClockgpsNavMsg.hpp
       /gpsNavMsg/xCNAVEphemerisPNBgpsNavMsg.cpp
       /gpsNavMsg/xCNAVEphemerisPNBgpsNavMsg.hpp
       /gpsNavMsg/xCNAVEphemerisgpsNavMsg.cpp
       /gpsNavMsg/xCNAVEphemerisgpsNavMsg.hpp
       /gpsNavMsg/xEngEphemerisgpsNavMsg.cpp
       /gpsNavMsg/xEngEphemerisgpsNavMsg.hpp
       /gpsNavMsg/xPackedNavBitsgpsNavMsg.cpp
       /gpsNavMsg/xPackedNavBitsgpsNavMsg.hpp
       /gpsNavMsg/xgpsNavMsgM.cpp
	  Added test programs for CNAV/CNAV2 classes utilizing the cppunit
	  test module.


	/SP3EphemerisStore/Checks
   	/SP3EphemerisStore/Checks/findXvt1.txt
   	/SP3EphemerisStore/Checks/findXvt15.txt
   	/SP3EphemerisStore/Checks/findXvt31.txt
   	/SP3EphemerisStore/DumpData.txt
   	/SP3EphemerisStore/Logs
   	/SP3EphemerisStore/Logs/DumpData.txt
   	/SP3EphemerisStore/Logs/findXvt1.txt
   	/SP3EphemerisStore/Logs/findXvt15.txt
   	/SP3EphemerisStore/Logs/findXvt31.txt
   	/SP3EphemerisStore/SP3
   	/SP3EphemerisStore/SP3/apc01000
   	/SP3EphemerisStore/SP3/igs09000.sp3
   	/SP3EphemerisStore/igs09000.sp3
   	/SP3EphemerisStore/xSP3EphemerisStore.cpp
   	/SP3EphemerisStore/xSP3EphemerisStore.hpp
   
   - Modified in tests

       /ANSITime/xANSITime.[h|c]pp
       /CivilTime/xCivilTime.[h|c]pp
       /CommonTime/xCommonTime.cpp
       /JulianDate/xJulianDate.[h|c]pp
       /MJD/xMJD.[h|c]pp
       /UnixTime/xUnixTime.[h|c]pp
       /YDSTime/xYDSTime.[h|c]pp
       /SEM/SEMData.cpp
       /SEM/SEMHeader.[h|c]pp
       /SEM/SEMStream.hpp
       /SEM/StringUtils.hpp
       /Yuma/YumaData.cpp
       /Yuma/YumaData.cpp
	  Merged UnitTests modifications from Rinex-3 into tests directory of RC2.0.

       /RinexEphemerisStore/xRinexEphemerisStore.cpp
	  Adapting test suite to CommonTime methods from obsoleted DayTime.

       /IonoModel/xIonoModel.cpp
       /MSC/xMSC.cpp
       /RACRotation/xRACRotation.cpp
       /RinexEphemerisStore/xRinexEphemerisStore.cpp
       /RinexMet/xRinexMet.cpp
       /RinexNav/Logs/RinexDump
       /RinexNav/xRinexNav.cpp
       /RinexObs/xRinexObs.cpp
       /UnixTime/xUnixTime.cpp
       /Yuma/YumaData.cpp
	  Resolved compile errors in /tests directory.

        DayTime/xDayTimeInc.hpp
        gpsNavMsg/xCNAV2EphClockPNB.cpp
        gpsNavMsg/xCNAVClockPNB.cpp
        gpsNavMsg/xCNAVEphemerisPNB.cpp
        gpsNavMsg/xPackedNavBits.cpp
        gpsNavMsg/xPackedNavBitsgpsNavMsg.cpp
        SEM/SEMBase.hpp
        SEM/SEMData.[h|c]pp
        SEM/SEMHeader.[h|c]pp
        SEM/SEMStream.hpp
        SEM/StringUtils.hpp
        Yuma/YumaBase.hpp
        Yuma/YumaData.[h|c]pp
        Yuma/YumaStream.hpp
           Adapting test suite to new GNSSconstants.hpp.

	
	   

      /RACRotation/xRACRotation.cpp
      /SEM/SEMData.cpp
      /SEM/SEMHeader.cpp
      /Yuma/YumaData.cpp
      /oldtests/DayTimeIncrementTest.cpp
      /oldtests/DayTimeIncrememntTest2.cpp
	  Changed instances icd_200 constants to adaptations in 
	  GNSSconstants.hpp.

   Examples modifications
   ----------------------

   - Modified in examples

       example1.cpp
       example2.cpp
       example3.cpp
       example4.cpp
	  Converted examples which completely relied on DayTime class.

       example3.cpp
       example4.cpp
	  Changed instances of icd_200 constants to adaptations in GNSSconstants.hpp

       example3.cpp
	  Changed C_GPS_MPS to C_MPS.

       example10.cpp
       example14.cpp
       example4.cpp
       example5.cpp
       example6.cpp
       example7.cpp
       example8.cpp
       example9.cpp
	  The examples directory has been modified to reflect obsoletion of 
	  DayTime class and the consolidation of GNSSconstants.  Expect changes 
	  to examples 8,9, and 14 which instantiate non-existent SP3EphemerisStore 
	  member functions to quality-check the precise ephemeris data for data 
	  gaps and too wide intervals.

       example3.cpp
       example4.cpp
	  Changed 'C_GPS_MPS' to 'C_MPS'.
        #################################################


Version 1.7   Friday, 8 October, 2010

   General modifications
   ---------------------

   - The majority of version 1.7 is comprised of revision number 2476;
     branching took place under revision 2476 from dev/ to branches/RC1.7/
   - A number of modifications were made to Jamfiles and Makefiles, and some
     tweaks to code for clean compilation, for compatibility with the latest
     Cygwin, Windows 7 VS, Solaris and Mac Snow Leopard (OSX 6.6).  Release 
     during the week of 21 February 2011.
   - Major additions to dev/apps/bindings/octave for a SWIG-based interface to
     the MATLAB-like language Octave. SWIG-based Octave wrapper added for most
     library classes. Additions listed below, modifications not involving new
     functionality omitted in this summary.
   - Major addition of dev/lib/geodyn library. Additions listed below,
     modifications not involving new functionality omitted in this summary.
   - Essential documentation and minimal examples were added for various 
     graphics formats in /ref/graphics/.
   - This release includes new/modified source code comments for improved
     readability.
   - Cleaned up and added comments to various examples to make them more
     useful.
   - The next planned release, GPSTk v2.0, will be a major upgrade and will
     merge the RINEX 3 development branch back into the trunk; 
     GPSTk v1.7 is expected to be the last 1.x release.
  

   Library modifications
   ---------------------
   - Added to src
       
       ConfDataWriter.[h|c]pp
          Added ConfDataWriter to work with ConfDataReader written years ago.

       DCBDataReader.[h|c]pp
          Added a new class DCBDataReader to handle DCB data from CODE.

       SuperKalmanFilter.[h|c]pp
          Class SuperKalmanFilter modified from SimpleKalmanFilter with added
          methods for precise orbit determination. Robust adaptive filter
          feature to be added later.

       EpochDataStore.[h|c]pp
          Added a class to handle interpolatable time series data.

   - Modified in src

       README
          Cleaned up examples and added 'example14.cpp'.

       Stats.hpp
          Normalization error in Variance, and bug in operator +=. Later mod
          made change more robust, esp. for non-fp types, and improved doc.
          Also mod to fix bug in median() and added similar for std::vector.

       VectorBase.hpp
          Fixed to make sure unary minus does not return an l-value.

       MatrixBase.hpp
       VectorBase.hpp
          Update to have the unary minus operator be a const function that 
          returns a const value.(ref: Solter, Kleper,"Professional C++", p.439)

       ObsID.cpp
          Fixed an incorrect comment.

       FICData.[h|c]pp
          Added a method to describe each field for each block. Also fixed
          navdmp so that date filtering works. Data are filtered based upon the
          transmission rather than the epoch date; related to SourceForge
          Bug #1196179.

       FICData.cpp
          Fixed bug in display of almanac time. Also modified appearance of
          SF4, SF5 data.

       FICFilterOperators.hpp
          Modified appearance of SF4, SF5 data. Also fixed navdmp so that date
          filtering works. Data are filtered based upon the transmission rather
          than the epoch date; related to SourceForge Bug #1196179. Also fixed
          a small bug by adding an argument to the command line to specify 
	  which block to convert in the RINEX file (if blank, default=9).

       Position.cpp
          Fixed bug related to converting between Geocentric and Geodetic
          systems. Longitude was always set to 0 in both cases.
          SourceForge Bug ID #1499586. Also fixed bug related to converting a
          position string with leading spaces to a Position.  SourceForge Bug
          ID #1530502.

       StringUtils.hpp
          Fixed namespace issue: strtod, strtoul, strtol declared in cstdlib
          but this was not included in StringUtils causing an error on some
          platforms. Also fixed formatting issue. Also modified to eliminate
          definition conflict with Windows "max" function.

       TypeID.[h|c]pp
          New types added to handle common derivatives. Also added derivatives
          for phase observations and some combinations; added corresponding
          TypeID's for the GRoup And PHase Ionospheric Combinations (GRAPHIC).
          Also new TypeIDs added w/ability to manage new TypeIDs w/string name.
          Added extra qualification on member class and std space for 'pair'
          to fix compilation errors. Also added a new TypeID 'codeBias' used to
          estimate slant TEC and separate interfrequency bias. Also added new
          "wide lane" TypeIDs for use in fast ambiguity resolution, as well as 
          'WL2' and related combinations. TypeIDs added to support 'spaceborne'
          data processing. TypeIDs added for GPS precise code observation 
          derivative.

       AntexReader.[h|c]pp
          Fixed problem with AntexReader pointed to by Yan Wei.

       MiscMath.hpp
          Added function to compute the second derivative of Lagrange
          interpolation. Also changed operator name 'and' to '&&'.

       TabularEphemerisStore.[h|c]pp
          Added method to compute the acceleration of the indicated object in 
          ECEF coordinates. Also private class members changed to protected to
          allow inheritance.

       BinUtils.[h|c]pp
          Added the CRC for GPS CNAV.

       MatrixFunctors.hpp
          Made changes so that PRSolve turns on flags to reject bad SP3 data;
          Improved Householder algorithm.

       RinexsObsData.[h|c]pp
       RinexObsFilterOperators.hpp
          Added ability to handle blank epoch times.

       GPSEphemerisStore.cpp
          Fixed problem with ending effectivity of table.

       AlmOrbit.cpp
       EngAlmanac.cpp
          Fixed the output to not throw away data.

       EngAlmanac.[h|c]pp
          Added EngAlmanac::getSVConfig() to enable access to SV configuration.
          Implemented missing definition for EngAlmanac::get6bitHealth().

       PRSolution.cpp
          Fixed RAIM algorithm to continue when it fails to converge if there's
          still room to do combinations (i.e., still possible to reject
          satellites). Also modified to save the covariance matrix
          corresponding to the best solution.

       SVNumXRef.cpp
          Updated class for launch of SVN50/PRN05 on 8/17/09. Also updated for
          launch of SVN62/PRN25 on 5/28/10. Two more cross-reference updates
          by B. Renfro.

       MSCData.cpp
          Updated to support new data format which allows for greater precision
          in specifying station positions. New format described on GPSTk wiki
          at: http://gpstk.arlut.utexas.edu/bin/view/Documentation/
          MonitorStationCoordinatesFileFormat. Change related to 1/7/09 e-mail 
          from B. Renfro to gpstk developers e-mail list re these classes. Also
          fixed to properly read records with fractional dates in the epoch
          time in the old MSC format.

       MSCStore.cpp
          Replacing a tab with " ".

       SatID.hpp
          Fixed output operator. Added a convertSatelliteSystemtoString()
          function.

       EllipsoidModel.hpp
       GPSEllipsoid.hpp
       SP3EphemerisStore.hpp
       StringUtils.hpp
       TabularEphemerisStore.hpp
       WGS84Ellipsoid.hpp
          Geomatics update.

       RinexSatID.[h|c]pp
          Fixed minor formatting issue; identical to Rinex3 branch (.hpp only).
          Also removed invalid mixed system type.

       CommandOption.hpp
          Changed to look in current directory for getopt.h (for MSVC 9.0).

       CommandOptionParser.cpp
          Modified to allow clean compile with MSVC 2010 re std::min and 
          std::max.

       TropModel.[h|c]pp
          Added weather calculation by a standard atmospheric model; also std
          name space for pow and exp.

       EngEphemeris.hpp
          Edited comment prior to getFitInterval().

       ConfDataWriter.[h|c]pp
       DCBDataReader.[h|c]pp
          Added 'pragma ident', fixing compilation errors.

       ConfDataReader.cpp
          Bug fix for MSVC 2010 (skip blank line).

       SpecialFunctions.[h|c]pp
          Added double precision input to factorial calculation.

       MatrixOperators.hpp
          Added a 'diag' operator to get the diagonal matrix of a square one.

       GPSAlmanacStore.[h|c]pp
          Added ability to position based on most recently received.

       EpochDataStore.[h|c]pp
          Added class 'PvtStore' to handle interpolatable position velocity and
          clock, assistance for class 'SatOrbitStore'. Added class
          'SatOrbitStore'. Fixed compiler error re nested template argument
          list.

       RinexObsHeader.[h|c]pp
          Corrected to permit Mixed file system.

       BasicFramework.[h|c]pp
          Modified to allow initialization of the BasicFramework with a command
          line string.

       SEMData.cpp
          Minor formatting fixes to SEMData::dump().

       SEMHeader.[h|c]pp
          Fixed SEMHeader::reallyGetRecord() in SEMHeader.cpp -- now properly
          sets the header and headerRead members of the stream. Static data 
          members added for more versatile handling of 10-bit GPS weeks found
          in files.

       YumaData.[h|c]pp
          Added static data members for more versatile handling of 10-bit GPS
          weeks found in files.

   - Added to lib

       /procframe/EquationSystem.[h|c]pp
       /procframe/SolverGeneral.[h|c]pp
          Added classes 'EquationSystem' and 'SolverGeneral'. These classes
          combine to create a run-time programmable solver.

       /geomatics/SunEarthSatGeometry.[h|c]pp
          Added SunEarthGeometry class.

       /geomatics/SolarSystem.[h|c]pp
          Added SolarSystem class.

       /geomatics/logstream.hpp
          Mods/updates/bug fixes to PhaseWindup.

       /geomatics/AntexBase.hpp
       /geomatics/AntexData.[h|c]pp
       /geomatics/AntexHeader.[h|c]pp
       /geomatics/AntexStream.hpp
       /geomatics/SRIMatrix.hpp
       /geomatics/SolarPosition.[h|c]pp
          Geomatics update.

       /procframe/CorrectCodeBiases.[h|c]pp
          Added new class 'CorrectCodeBiases' to work with GDS and correct code
          biases for P1 P2 P3 (PC), P4 (PI), P6 (MW), C1 and C1-based 
          combinations.

       /procframe/DoubleOp.[h|c]pp
          Added new class 'DoubleOp' to calculate DD data.

       /procframe/NetworkObsStreams.[h|c]pp
          Added new class 'NetworkObsStreams' to handle a set of RINEX
          observation files and provide a graceful way to process network data.

       /geodyn/
       /geodyn/Jamfile
       /geodyn/ReadMe.txt
       /geodyn/ASConstant.hpp
       /geodyn/EarthBody.[h|c]pp
       /geodyn/EarthOceanTide.[h|c]pp
       /geodyn/EarthPoleTide.[h|c]pp
       /geodyn/EarthSolidTide.[h|c]pp
       /geodyn/EquationOfMotion.hpp
       /geodyn/ForceModel.hpp
       /geodyn/ForceModelList.[h|c]pp
       /geodyn/IERS.[h|c]pp
       /geodyn/Integrator.hpp
       /geodyn/KeplerOrbit.[h|c]pp
       /geodyn/ReferenceFrames.[h|c]pp
       /geodyn/RungeKuttaFehlberg.[h|c]pp
       /geodyn/Spacecraft.[h|c]pp
       /geodyn/UTCTime.[h|c]pp
       /geodyn/AtmosphericDrag.[h|c]pp
       /geodyn/CiraExponentialDrag.[h|c]pp
       /geodyn/EGM96GravityModel.[h|c]pp
       /geodyn/HarrisPriesterDrag.[h|c]pp
       /geodyn/JGM3GravityModel.[h|c]pp
       /geodyn/MoonForce.[h|c]pp
       /geodyn/Msise00Drag.[h|c]pp
       /geodyn/RelativityEffect.[h|c]pp
       /geodyn/SolarRadiationPressure.[h|c]pp
       /geodyn/SphericalHarmonicGravity.[h|c]pp
       /geodyn/SunForce.[h|c]pp
       /geodyn/LEOSatOrbit.[h|c]pp
       /geodyn/NavSatOrbit.[h|c]pp
       /geodyn/SatOrbit.[h|c]pp
       /geodyn/SatOrbitPropagator.[h|c]pp
       /geodyn/PvtStore.[h|c]pp
       /geodyn/SatOrbitStore.[h|c]pp
          New library 'geodyn' added

   - Modified in lib

       /procframe/Dumper.cpp
          Corrected mistake when printing SourceID information.

       /procframe/LinearCombinations.[h|c]pp
          Minor improvements in class 'LinearCombinations'. Also added objects
          to compute the GRAPHIC combinations. Also added 'p1Prefit' linear
          combination, plus a remark about TGD. Corrected mixing 'c1Prefit' 
          with 'p1Prefit'. Comment added for 'instC1' component. Added
          'l1Prefit' linear combination, and the multipath corresponding 
          coefficient. Also fixed comment error and added combinations to aid
          fast ambiguity resolution. Added 'WL2' and related combinations.
          Reverted LC 'formal' wavelength to its proper value.

       /procframe/DataStructures.[h|c]pp
          Added new data structures and methods to ease handling of multi-epoch
          multi-receiver data. Also added new method
          'gnssDataMap::insertValue()'.

       /procframe/DataStructures.cpp
       /procframe/EquationSystem.hpp
          Changed to make it work with Microsoft Visual C++ 9.0. Also changed
          operator 'and' to '&&'.

       /procframe/BasicModel.[h|c]pp
          Now 'BasicModel' also inserts satellite velocity into GDS. In 
          BasicModel.cpp, store receiver position and velocity in the 
          gnssRinex object.

       /procframe/LICSDetector2.hpp
       /procframe/MWCSDetector.hpp
       /procframe/PhaseCodeAlignment.hpp
       /procframe/BasicModel.hpp
          Documentation improvements.

       /procframe/NablaOp.[h|c]pp
          Improvements to 'NablaOp' class. Also added important warnings to
          method 'setRefSat()' of class 'NablaOp'.

       /procframe/SolverPPPFB.cpp
          Bug fix.

       /procframe/Equation.[h|c]pp
       /procframe/EquationSystem.[h|c]pp
       /procframe/Variable.[h|c]pp
          Added new features for 'SolverGeneral' and its related equation
          system: type index Variable, and different equation for different
          satellite. Added include file '#include <iterator>' for MSVC 2010.

       /procframe/CorrectCodeBiases.[h|c]pp
       /procframe/DoubleOp.[h|c]pp
          Added end of lines to several files in order to suppress some
          compilation warnings. Fixed wrong property value in 
          CorrectCodeBiases.

       /procframe/ComputeSatPCenter.cpp
          Force nadir angle slightly larger than 14 deg to 14 to avoid throwing
          exception.

       /procframe/NetworkObsStreams.cpp
          Added some missing 'std::' prefixes, which were preventing proper
          compilation in ANSI C++ compilers.

       /procframe/DataStructures.[h|c]pp
          Added stream output generator for gnssRinex.

       /procframe/ComputeIonoModel.[h|c]pp
          New class added to get nominal value for ionospheric delay with
          klobuchar or grid TEC (ionospheric maps).

       /rxio/FFIdentifier.[h|c]pp
          Added small routine to identify field with strings.

       /rxio/FFIdentifier.cpp
       /rxio/ObsEphReaderFramework.cpp
          Fixed what the robustification of the MDP reader broke.

       /rxio/FFIdentifier.cpp
       /rxio/EphReader.[h|c]pp
          FFIdentifier now properly identifies SEM files. EphReader can now 
          modify 10-bit weeks within SEM and Yuma almanac files.

       /rxio/MDPHeader.cpp
       /rxio/MDPObsEpoch.cpp
       /rxio/ObsUtils.cpp
       /rxio/miscenum.hpp
          Fixed some nav IDs. Also modified MDPHeader and MDPObsEpoch to remove
          limits on how far into a file to read before giving up. Also 
          MDPHeader now dumps the current message if there's a CRC error. Also 
          fixed MDPHeader include for actual location of file.

       /rxio/ObsReader.[h|c]pp
          Added obsInterval and obsIntervalConfidence.

       /rxio/RinexConverters.cpp
          Cleaned up lli/ssi code.

       /rxio/NovatelData.[h|c]pp
          Added malformed message and large NovatelData records handling to 
          novaRinex (associated changes).

       /geomatics/RobustStats.hpp
          Fixed include dependency using <cmath>.

       /geomatics/PhaseWindup.[h|c]pp
          Mods/updates/bugfixes to PhaseWindup, plus minor mod to namespace.
          Also modified to remove round()- not supported by all compilers, 
          notably Windows.

       /geomatics/DiscCorr.cpp
       /geomatics/DiscCorr.hpp
       /geomatics/EarthOrientation.cpp
       /geomatics/EarthOrientation.hpp
       /geomatics/GeodeticFrames.cpp
       /geomatics/GeodeticFrames.hpp
       /geomatics/Jamfile
       /geomatics/Namelist.cpp
       /geomatics/PhaseWindup.cpp
       /geomatics/PhaseWindup.hpp
       /geomatics/PreciseRange.cpp
       /geomatics/PreciseRange.hpp
       /geomatics/RobustStats.cpp
       /geomatics/RobustStats.hpp
       /geomatics/SRI.cpp
       /geomatics/SRI.hpp
       /geomatics/SRIFilter.cpp
       /geomatics/SRIFilter.hpp
       /geomatics/SatPass.cpp
       /geomatics/SatPass.hpp
       /geomatics/SolarSystem.cpp
       /geomatics/SolarSystem.hpp
       /geomatics/SunEarthSatGeometry.cpp
       /geomatics/SunEarthSatGeometry.hpp
       /geomatics/logstream.hpp
          Geomatics update.

       /geomatics/DiscCorr.cpp
          Cleaned up old-style line returns.

       /vdraw/ViewerManager.cpp
          Added pretty coding format.

       /vdraw/EPSImage.cpp
          Added 'gsview32' to the ViewManager - works under Windows.

       /vplot/Splitter.cpp
          Force conversion of '0' to 'list<Path>' to make it work with MSVC
          2010.


   Application modifications
   -------------------------
   - Added to apps
	
       /bindings/octave/gpstk.i
       /bindings/octave/gpstk_ANSITime.i
       /bindings/octave/gpstk_AllanDeviation.i
       /bindings/octave/gpstk_AlmOrbit.i
       /bindings/octave/gpstk_Antenna.i
       /bindings/octave/gpstk_AstronomicalFunctions.i
       /bindings/octave/gpstk_BLQDataReader.i
       /bindings/octave/gpstk_Bancroft.i
       /bindings/octave/gpstk_BaseDistribution.i
       /bindings/octave/gpstk_BasicFramework.i
       /bindings/octave/gpstk_BinUtils.i
       /bindings/octave/gpstk_BinexData.i
       /bindings/octave/gpstk_BinexFilterOperators.i
       /bindings/octave/gpstk_BinexStream.i
       /bindings/octave/gpstk_BivarStats.i
       /bindings/octave/gpstk_CheckPRData.i
       /bindings/octave/gpstk_ClockModel.i
       /bindings/octave/gpstk_CodeBuffer.i
       /bindings/octave/gpstk_CommandOptionParser.i
       /bindings/octave/gpstk_CommonTime.i
       /bindings/octave/gpstk_ConfDataReader.i
       /bindings/octave/gpstk_DOP.i
       /bindings/octave/gpstk_DayTime.i
       /bindings/octave/gpstk_ENUUtil.i
       /bindings/octave/gpstk_EllipsoidModel.i
       /bindings/octave/gpstk_Epoch.i
       /bindings/octave/gpstk_Triple.i
       /bindings/octave/gpstk_Zcount.i
       /bindings/octave/gpstk_convhelp.i
       /bindings/octave/gpstk_exception.i
       /bindings/octave/gpstk_orbitgroup.i
       /bindings/octave/gpstk_position.i
       /bindings/octave/build.bash (precursor to makefile)
          Beginnings of a SWIG-based interface to 
	  (the MATLAB-like language) Octave.

       /bindings/octave/Makefile
       /bindings/octave/Makefile.old (from rev 2016)
	  New Makefile for python, perl and octave bindings.

       /bindings/octave/gpstk_Expression.i
       /bindings/octave/gpstk_ExtractC1.i
       /bindings/octave/gpstk_ExtractCombinationData.i
       /bindings/octave/gpstk_ExtractD1.i
       /bindings/octave/gpstk_ExtractD2.i
       /bindings/octave/gpstk_ExtractData.i
       /bindings/octave/gpstk_ExtractL1.i
       /bindings/octave/gpstk_ExtractL2.i
       /bindings/octave/gpstk_ExtractLC.i
       /bindings/octave/gpstk_ExtractP1.i
       /bindings/octave/gpstk_ExtractP2.i
       /bindings/octave/gpstk_ExtractPC.i
       /bindings/octave/gpstk_FFBinaryStream.i
       /bindings/octave/gpstk_FFData.i
       /bindings/octave/gpstk_FFStream.i
       /bindings/octave/gpstk_FFStreamError.i
       /bindings/octave/gpstk_FFTextStream.i
       /bindings/octave/gpstk_FICAStream.i
       /bindings/octave/gpstk_FICBase.i
       /bindings/octave/gpstk_FICDataSet.i
       /bindings/octave/gpstk_FICFilterOperators.i
       /bindings/octave/gpstk_FICHeader.i
       /bindings/octave/gpstk_FICStream.i
       /bindings/octave/gpstk_FICStreamBase.i
       /bindings/octave/gpstk_FileFilter.i
       /bindings/octave/gpstk_FileFilterFrame.i
       /bindings/octave/gpstk_FileFilterFrameWithHeader.i
       /bindings/octave/gpstk_FileHunter.i
       /bindings/octave/gpstk_FileSpec.i
       /bindings/octave/gpstk_FileStore.i
       /bindings/octave/gpstk_FileUtils.i
       /bindings/octave/gpstk_GPSAlmanacStore.i
       /bindings/octave/gpstk_GPSEllipsoid.i
       /bindings/octave/gpstk_GPSEphemerisStore.i
       /bindings/octave/gpstk_GPSGeoid.i
       /bindings/octave/gpstk_GPSWeek.i
       /bindings/octave/gpstk_GPSWeekSecond.i
       /bindings/octave/gpstk_GPSWeekZcount.i
       /bindings/octave/gpstk_GPSZcount.i
       /bindings/octave/gpstk_GaussianDistribution.i
       /bindings/octave/gpstk_GenXSequence.i
       /bindings/octave/gpstk_Geodetic.i
       /bindings/octave/gpstk_GeoidModel.i
       /bindings/octave/gpstk_InOutFramework.i
       /bindings/octave/gpstk_IonexBase.i
       /bindings/octave/gpstk_IonexData.i
       /bindings/octave/gpstk_IonexHeader.i
       /bindings/octave/gpstk_IonexStore.i
       /bindings/octave/gpstk_IonexStream.i
       /bindings/octave/gpstk_IonoModel.i
       /bindings/octave/gpstk_IonoModelStore.i
       /bindings/octave/gpstk_JulianDate.i
       /bindings/octave/gpstk_LinearClockModel.i
       /bindings/octave/gpstk_LoopedFramework.i
       /bindings/octave/gpstk_MJD.i
       /bindings/octave/gpstk_MOPSWeight.i
       /bindings/octave/gpstk_MSCBase.i
       /bindings/octave/gpstk_MSCData.i
       /bindings/octave/gpstk_MSCHeader.i
       /bindings/octave/gpstk_MSCStore.i
       /bindings/octave/gpstk_MSCStream.i
       /bindings/octave/gpstk_Matrix.i
       /bindings/octave/gpstk_MatrixBase.i
       /bindings/octave/gpstk_MatrixBaseOperators.i
       /bindings/octave/gpstk_MatrixFunctors.i
       /bindings/octave/gpstk_MatrixImplementation.i
       /bindings/octave/gpstk_MiscMath.i
       /bindings/octave/gpstk_ModeledPseudorangeBase.i
       /bindings/octave/gpstk_MoonPosition.i
       /bindings/octave/gpstk_NEDUtil.i
       /bindings/octave/gpstk_ORDEpoch.i
       /bindings/octave/gpstk_ObsClockModel.i
       /bindings/octave/gpstk_ObsEpochMap.i
       /bindings/octave/gpstk_ObsID.i
       /bindings/octave/gpstk_ObsRngDev.i
       /bindings/octave/gpstk_OceanLoading.i
       /bindings/octave/gpstk_PCodeConst.i
       /bindings/octave/gpstk_PRSolution.i
       /bindings/octave/gpstk_PoleTides.i
       /bindings/octave/gpstk_PolyFit.i
       /bindings/octave/gpstk_Position.i
       /bindings/octave/gpstk_PowerSum.i
       /bindings/octave/gpstk_RACRotation.i
       /bindings/octave/gpstk_RTFileFrame.i
       /bindings/octave/gpstk_RinexEphemerisStore.i
       /bindings/octave/gpstk_RinexMetBase.i
       /bindings/octave/gpstk_RinexMetData.i
       /bindings/octave/gpstk_RinexMetFilterOperators.i
       /bindings/octave/gpstk_RinexMetHeader.i
       /bindings/octave/gpstk_RinexMetStream.i
       /bindings/octave/gpstk_RinexNavBase.i
       /bindings/octave/gpstk_RinexNavData.i
       /bindings/octave/gpstk_RinexNavFilterOperators.i
       /bindings/octave/gpstk_RinexNavHeader.i
       /bindings/octave/gpstk_RinexNavStream.i
       /bindings/octave/gpstk_RinexObsBase.i
       /bindings/octave/gpstk_RinexObsData.i
       /bindings/octave/gpstk_RinexObsFilterOperators.i
       /bindings/octave/gpstk_RinexObsHeader.i
       /bindings/octave/gpstk_RinexObsID.i
       /bindings/octave/gpstk_RinexObsStream.i
       /bindings/octave/gpstk_RinexSatID.i
       /bindings/octave/gpstk_RinexUtilities.i
       /bindings/octave/gpstk_RungeKutta4.i
       /bindings/octave/gpstk_SEMAlmanacStore.i
       /bindings/octave/gpstk_SEMBase.i
       /bindings/octave/gpstk_SEMData.i
       /bindings/octave/gpstk_SEMHeader.i
       /bindings/octave/gpstk_SEMStream.i
       /bindings/octave/gpstk_SMODFData.i
       /bindings/octave/gpstk_SMODFStream.i
       /bindings/octave/gpstk_SP3Base.i
       /bindings/octave/gpstk_SP3Data.i
       /bindings/octave/gpstk_SP3EphemerisStore.i
       /bindings/octave/gpstk_SP3Header.i
       /bindings/octave/gpstk_SP3SatID.i
       /bindings/octave/gpstk_SP3Stream.i
       /bindings/octave/gpstk_SVExclusionList.i
       /bindings/octave/gpstk_SVNumXRef.i
       /bindings/octave/gpstk_SVPCodeGen.i
       /bindings/octave/gpstk_SatDataReader.i
       /bindings/octave/gpstk_SimpleIURAWeight.i
       /bindings/octave/gpstk_SimpleKalmanFilter.i
       /bindings/octave/gpstk_SolidTides.i
       /bindings/octave/gpstk_SolverBase.i
       /bindings/octave/gpstk_SourceID.i
       /bindings/octave/gpstk_SpecialFunctions.i
       /bindings/octave/gpstk_Stats.i
       /bindings/octave/gpstk_StringUtils.i
       /bindings/octave/gpstk_StudentDistribution.i
       /bindings/octave/gpstk_SunPosition.i
       /bindings/octave/gpstk_SystemTime.i
       /bindings/octave/gpstk_TabularEphemerisStore.i
       /bindings/octave/gpstk_TimeConstants.i
       /bindings/octave/gpstk_TimeConverters.i
       /bindings/octave/gpstk_TimeNamedFileStream.i
       /bindings/octave/gpstk_TimeString.i
       /bindings/octave/gpstk_TimeTag.i
       /bindings/octave/gpstk_TropModel.i
       /bindings/octave/gpstk_TypeID.i
       /bindings/octave/gpstk_UnixTime.i
       /bindings/octave/gpstk_ValidType.i
       /bindings/octave/gpstk_VectorBase.i
       /bindings/octave/gpstk_VectorBaseOperators.i
       /bindings/octave/gpstk_VectorOperators.i
       /bindings/octave/gpstk_WGS84Ellipsoid.i
       /bindings/octave/gpstk_WGS84Geoid.i
       /bindings/octave/gpstk_WeightBase.i
       /bindings/octave/gpstk_WxObsMap.i
       /bindings/octave/gpstk_X1Sequence.i
       /bindings/octave/gpstk_X2Sequence.i
       /bindings/octave/gpstk_Xvt.i
       /bindings/octave/gpstk_XvtStore.i
       /bindings/octave/gpstk_YDSTime.i
       /bindings/octave/gpstk_YumaAlmanacStore.i
       /bindings/octave/gpstk_YumaBase.i
       /bindings/octave/gpstk_YumaData.i
       /bindings/octave/gpstk_YumaHeader.i
       /bindings/octave/gpstk_YumaStream.i
       /bindings/octave/gpstk_geometry.i
       /bindings/octave/gpstk_gps_constants.i
       /bindings/octave/gpstk_icd_200_constants.i
       /bindings/octave/gpstk_stl_helpers.i
       /bindings/octave/gpstk_Chi2Distribution.i
       /bindings/octave/gpstk_CivilTime.i
       /bindings/octave/gpstk_SatID.i
       /bindings/octave/STREAMER.i
       /bindings/octave/common.i
          SWIG-based Octave wrapper for most library classes.

       /bindings/octave/ubuntu-notes.txt
          Added detailed notes on building SWIG interfaces for ubuntu.

       /bindings/swig (from /bindings/octave:2072)
          Establishing a general interface using SWIG.

       /bindings/swig/old
       /bindings/swig/old/Daytime.i (from /bindings:2072)
       /bindings/swig/old/Exception.i 
	  (from /bindings:2072)
       /bindings/swig/old/FFTextStream.i 
	  (from /bindings:2072)
       /bindings/swig/old/GPSZcount.i 
	  (from /bindings:2072)
       /bindings/swig/old/README (from /bindings:2072)
       /bindings/swig/old/RinexObsStream.i
	  (from /bindings:2072)
       /bindings/swig/old/common.i (from /bindings:2072)
       /bindings/swig/old/gpstk.i (from /bindings:2072)
       /bindings/swig/old/java (from /bindings/java:2072)
       /bindings/swig/old/perl (from /bindings/perl:2072)
       /bindings/swig/old/python
          (from /bindings/python:2072)
       /bindings/swig/old/tcl (from /bindings/tcl:2072)
          Partial move of old SWIG code and final reorg of old SWIG
	  interfaces.
	
       /bindings/octave/gpstk_OctaveUtils.i
       /bindings/octave/gpstk_OctaveUtilsFile.i
          Additions for new FFData.i

       /MDPtools/SubframeProc.[c|h]pp
          Added files needed for MDP processing.

       /bindings/swig/RinexDatum.i
          Added SWIG interface to handle data taken under A-S
          appropriately.

       /bindings/swig/octexample1.m
       /bindings/swig/octexample2.m
       /bindings/swig/readRinexObsFast.m
          Update to readRinexFast.m

       /multipath/mpsim.cpp
          Added skeleton for ground multipath simulation tool.

       /multipath/DenseBinnedStats.hpp
          Added upgrade to mpsolve, includes bias resolution routines.

       /geomatics/JPLeph
       /geomatics/JPLeph/Jamfile
       /geomatics/JPLeph/convertSSEph.cpp
       /geomatics/JPLeph/testSSEph.cpp
          Added conversion and test programs for JPL ephemeris (class
          SolarSystem in lib/geomatics)
	
   - Modified in apps

       /MDPTools/NavProc.[h|c]pp
          Improved accouting of nav subframes.

       /difftools/ficdiff.cpp
          Added a diagnostic mode to give more details when two files differ.

       /MDPTools/Histogram.hpp
       /MDPTools/NavProc.cpp
          More cleanup on the accounting of errors.

       /converters/ash2mdp.cpp
       /converters/ats2mdp.cpp
          Working snapshot.

       /positioning/PRSolve.cpp
          Fixed flag-setting to reject bad SP3 data; improved Householder
          algorithm. Fixed missing bracket and cleaned up per GPSTk coding 
          standards.

       /positioning/rinexpvt.[h|c]pp
          Fixed namespace issue. strtod, strtoul and strtol are declared in
          cstdlib but this wasn't included in StringUtils causing an error on
          some platforms. Also, added Yuma and SEM capability.

       /MDPtools/MDPProcessors.[h|c]pp
          Moved header checks to after the entire message has been read.

       /bindings/swig/common.i
       /bindings/swig/gpstk_RinexObsHeader.i
       /bindings/swig/readRinexObsFast.m
          readRinexObsFast.m update.

       /geomatics/relposition/ElevationMask.cpp
       /geomatics/relposition/Synchronization.cpp
          Modifications to accommodate lib/geomatics changes.

       /Rinextools/ResCor.cpp
          Fixed bug to allow ResCor to handle GLO satellites properly;
          wavelength-dependent derived quantities are probably incorrect.

       /MDPtools/MDPProcessors.cpp
       /MDPtools/NavProc.cpp
       /MDPtools/mdptool.cpp
          Cleaned up handling of freshness counts. Added crc checking for
          CNAV data to NavProc.cpp.

       /multipath/ObsArray.cpp
          Fixed to handle data taken under A-S appropriately.

       /filetools/navsum.cpp
          Fixed navsum indexing error, and added compressed summary output
          option (-f).

       /filetools/navdmp.cpp
          Fixed navdmp so that datefiltering works now. The data are filtered 
          based upon the transmission rather than the epoch date. Related to 
          SourceForge bug #1196179.

       /filetools/fic2rin.cpp
          Added an argument to fic2rin. Extra argument is --9 | --109 | --all
          which will either convert block 9, block 109, or block 9 and 109 data
          to the RINEX file. Formerly converting block 9 was the default, and 
          still is. Argument must be specified at end of parameter list. If
          left blank, defaults to block 9 only.


       /MDPtools/mdpEdit.cpp
          Added a more useful prn exclusion/inclusion mechanism.

       /visibility/DiscreteVisibleCounts.cpp
       /visibility/compStaVis.cpp
          Fixed problem with percentage statistics. Fixed table header.
          Improved initialization.

       /visibility/svvis.cpp
          Added an option to svvis to display its data by PRN instead of by 
          epoch. Later added options to add a tabular display and specify that
          10-bit weeks in ephemeris sources are "recent".

       /ObsArrayEvaluator/ObsArray.cpp
       /ObsArrayEvaluator/oae.cpp
       /ObsArrayEvaluator/oaePlot
          Robustification of processing.

       /DataAvailability/DataAvailabilityAnalyzer.cpp
       /ObsArrayEvaluator/ObsArray.cpp
          Modified DataAvailabilityAnalyzer and ObsArray to handle ObsReader
          changes. DataAvailabilityAnalyzer now accounts for all missing
          epochs when start and stop times are specified. Some messages output
          by DataAvailabilityAnalyzer reworked.

       /DataAvailability/DataAvailabilityAnalyzer.[h|c]pp
          Added a command-line option for specification of the observation
          interval. Also modified accounting for missing data epochs and fixed
          some errors in the summary when all epochs are missed.

       /Rinextools/RinSum.cpp
          Added antenna offset to output, added option to output lsit of gaps,
          fixed some minor bugs and improved the code.

       /Rinextools/RinexEditor.cpp
          Accompanying fix to RinexObsHeader to permit Mixed file system.

       /reszilla/ordPlot
          Implemented the --warts option fully in ordPlot.

       /reszilla/ordClock.cpp
       /reszilla/ordPlot
          Fixed plotting output with only warts. Increased the clock tolerance
          for marking of wonky ord clocks.

       /reszilla/ordStats.cpp
          Added median absolute deviation and median to output.

       /reszilla/ordGen.cpp
          Now chooses the last entry for a given station in an MSC file.

       /MDPtools/Jamfile
       /MDPtools/MDPProcessors.[h|c]pp
       /MDPtools/mdptool.cpp
          Made the mdpscreen build conditional (Jamfile). Added a fast mode for
          converting to a new file.

       /geomatics/cycleslips/DiscFix.cpp
       /geomatics/relposition/ClockModel.cpp
       /geomatics/relposition/CommandInput.cpp
       /geomatics/relposition/CommandInput.hpp
       /geomatics/relposition/DDBase.cpp
       /geomatics/relposition/DDBase.hpp
       /geomatics/relposition/DataOutput.cpp
       /geomatics/relposition/DataStructures.hpp
       /geomatics/relposition/EditDDs.cpp
       /geomatics/relposition/ElevationMask.cpp
       /geomatics/relposition/Estimation.cpp
       /geomatics/relposition/ReadObsFiles.cpp
       /geomatics/relposition/ReadRawData.cpp
       /geomatics/relposition/StochasticModels.cpp
       /geomatics/relposition/Synchronization.cpp
       /geomatics/relposition/ddmerge.cpp
       /geomatics/robust/rstats.cpp
       /positioning/PRSolve.cpp
       /positioning/posInterp.cpp
          Geomatics update.

       /geomatics/cycleslips/DiscFix.cpp
          Fixed DayTime constructor (constant) v. constant issue, plus most
          C->C++ endl.

       /geomatics/relposition/CommandInput.cpp
          Fixed SourceForge bug #2876038. Bug occurred when using multiple
          fixed stations for DDBase. More info on SourceForge bug tracker site.

       /geomatics/relposition/Configure.cpp
          Minor change so that weather data is interpolated from tabular values
          and scaled for height properly when using the NewB tropo model.

       /geomatics/relposition/DDBase.cpp
          Updated version string and date for release.

       /converters/novaRinex.cpp
          Open temporary file on Windows in env(TEMP) directory; necessary for
          Vista. Later added fix to address breakage related to changes in 
          /dev/src/RinexSatID (rev2319). However, fix only allows GPS system
          type. Later fix added malformed message and large NovatelData records
          handling.

       /difftools/rowdiff.cpp
          Modified to: indicate to the user if the two files contain different
          data types; output to the screen the data types contained in each
          file; indicate to the user if no differences were found.

       /filetools/EphSum.cpp
          Corrected reported end of effectivity times.

       /multipath/SparseBinnedStats.hpp
       /multipath/ValarrayUtils.[h|c]pp
       /multipath/mpsolve.cpp
       /multipath/tstSparseBinnedStats.cpp
          Major upgrade to mpsolve. Added bias resolution routines and cleaned
          up code.

       /multipath/ObsArray.cpp
       /multipath/mpsolve.cpp
          Now mpsolve uses robust statistic to detect and remove data affected
          by cycle slips. Also can process a directory of files and can make
          surface plots of its solutions. Paper is in the works. Also, mpsolve
          user can edit out days when processing large directories of data.
          Also added zero mean assumption to add final de-bias/constraint on
          DFM processing. Disabled position solution consistency check. Also in
          mpsolve fixed two bugs: one for the DFM method when no intersections 
          were found, one for the DFM method when a pass exists that is less
          than two points.

       /multipath/mpsim.cpp
          Added capability to generate random Gaussian numbers. Comment
          corrected.

       /multipath/ObsArray.[h|c]pp
          Major bug fix.
         

   Example modifications
   -------------------------
   - Added to examples

       jplde405
          JPL De405 binary file 1980-2040 for forthcoming geodyn example.

       acor1480.08o
       scoa1480.08o
       sfer1480.08o
       tlse1480.08o
          Added RINEX files.

       example14.cpp
       popconf.txt
          Added 'example14.cpp': an implementation of the 'Precise Orbits
          Positioning' (POP) processing strategy.

       example15.cpp
          Added example for RobustStats and Random Number generation in 
          geomatics library.       

   - Modified in examples
         
       pppconf.txt
          Minor change in configuration file.

       OCEAN-GOT00.dat
       PRN_GPS
       igs05.atx
          Updated auxiliary files.

       Jamfile
       Makefile.am
          Changed for addition of 'example14.cpp': an implementation of 
          the 'Precise Orbits Positioning' (POP) processing strategy. Also 
          changed for addition of example15.
	

   Test Modifications
   -------------------------
   - Added to tests

       /Stats/xStats.cpp
          Updated/improved testing in Stats test class.
	
       /Vector directory
       /Vector/Jamfile
       /Vector/makefile
       /Vector/Vector.hpp
       /Vector/xVector.[h|c]pp
       /Vector/xVectorM.cpp
          Tests added for the Vector class. Vector class was failing due
          to an issue with unary minus method.

       /Matrix directory
       /Matrix/Jamfile
       /Matrix/makefile
       /Matrix/Matrix.hpp
       /Matrix/xMatrix.[h|c]pp
       /Matrix/xMatrixM.cpp
	  Tests added for the Matrix class.

   - Modified in tests

       Jamfile
       Makefile.am
       /Vector/makefile
       /Vector/xVector.[h|c]pp
       /Vector/xVectorM.cpp
       /Matrix/xMatrix.[h|c]pp
          Updated Vector and Matrix test classes.


	#################################################

	
Version 1.6   Tuesday, June 19, 2009

   General modifications
   ---------------------

   - Majority of version 1.6 comprised of revision number 1962, branching
     took place under revision 1962 from dev/ to branches/RC1.6/
   - There is now a Rinex3 development branch. This release does not include
     support for Rinex3 but we hope to include it in the future.
   - Major improvements in readability of code by adding new comments and
     rearraging a lot of code
   - Added the LGPL to serveral files from which it was missing.
   

   Library modifications
   ---------------------
   - Additions to src
         AllanDeviation.hpp
            Compute the overlapping Allan variance of the phase data provided.
         Antenna.[h|c]pp
            This class encapsulates the data related to GNSS antennas according
            to IGS standards.
         AntexReader.[h|c]pp
            This is a class to read and parse antenna data in Antex file
            format.
         BaseDistribution.hpp
            This is a base class for statistical distributions.
         Chi2Distribution.[h|c]pp
            This class implements the Chi-square distribution.
         ConfDataReader.[h|c]pp
            This is a class to parse and manage configuration data files.
         EllipsoidModel.hpp
            This is a copy of GeoidModel.hpp with the proper name.
         ENUUtil.[h|c]pp
            Converts from Cartesian in XZY to East-North-Up (ENU).
         Epoch.[h|c]pp
            Time representation class for common time formats, including GPS.
         GaussianDistribution.[h|c]pp
            This class implements the Gaussian (or normal) distribution.
         GPSEllipsoid.hpp
            This is a copy of GPSGeoid.hpp with the proper name.
         GPSEphemerisStore.[h|c]pp
            Store GPS broadcast ephemeris information.
         GPSWeek.[h|c]pp
            This class is an abstract base class inherited by other
            GPS-time-related classes.
         InOutFramework.cpp
            Framework for programs that take a single type of input data and
            output a single stream of output.
         IonexBase.hpp
            Does not served any purpose other than to make readable inheritance
            diagrams.
         IonexData.[h|c]pp
            Models a IONEX Data Record.
         IonexHeader.[h|c]pp
            Models the header for a IONEX file.
         IonexStore.[h|c]pp
            Reads and stores Ionosphere maps.
         IonexStream.hpp
            Provides access to IONEX files.
         NEDUtil.[h|c]pp
            Converts from Cartesian in XZY to North-East-Down (NED).
         PowerSum.[h|c]pp
            Computes the power sum of a list of numbers.
         SP3EphemerisStore.[h|c]pp
            Interface to read SP3 files into a TabularEphemerisStore.
         SpecialFunctions.[h|c]pp
            Contains handy special functions: gamma, erf, erfc, etc.
         StudentDistribution.[h|c]pp
            This class implements the t-Student distribution.
         TabularEphemerisStore.[h|c]pp
            Store a tabular list of Xvt data (such as a table of precise
            ephemeris data in an SP3 file) and compute Xvt from this table.
         WGS84Ellipsoid.hpp
            This is a copy of WGS84Geoid.hpp with the proper name.

   - Modified in src
         CommonTime.[h|c]pp
            Added operator<<() and asString() functions as well as a couple
            bug fixes.
         GPSWeekSecond.cpp
            Fix for 32-bit round off error during day of week calculation.
         FFTextStream.hpp
            Fixing a bug when a line of input has more than 256 characters. It
            now throws an exception when a line has more than 256 characters.
         MatrixBase.hpp
            Fixed unary minux operations to return an lvalue.
         MJD.cpp
            Fixed a floating-point rounding error problem which could cause
            exceptions to be thrown when converting to CommonTime.
         ObsID.[h|c]pp
            Fixed an error associated with string values for ValueTypes 'Last'
            and 'Placeholder' not being defined.
         SP3EphemerisStore.[h|c]pp
            Modified in order to implement rejection of satellites with bad or
            absent positional values or clocks. Feature disabled by default.
         StringUtils.hpp
            Improved error reporting and slimmed formattedPrint(). Fixed
            possible "undefined" behavior in matches(). Corrected a bug in
            translate() which allowed index 'inpos' to access 'outputChars'
            beyond its length.
         SVNumXRef.[h|c]pp
            Modified to account for new data since the last update and to allow
            a single SVN to have multiple PRNs over time.
         TimeTag.hpp
            Throw exceptions on invalid conversions and added a printError()
            function.
         TypeID.[h|c]pp
            Fixed and error associated with string values for ValueTypes 'Last'
            and 'Placeholder' not being defined.

   - Added in lib
         - /vdraw
            vdraw is a modification to vplot and much of the code in vdraw was
            moved over from vplot. This library allows drawing shapes and text
            in a vector format and outputting them to files.
         - /geomatics/SRIleastSquares.[h|c]pp
            Implements a general least squares algorithm using SRI, including
            weighted, linear or linearized, robust or sequential algorithms.
         - /procframe/CodeKalmanSolver.[h|c]pp
            Computes the code-based solution using a simple Kalman solver.
         - /procframe/ComputeDOP.[h|c]pp
            Computes the usual DOP values: GDOP, PDOP, TDOP, HDOP and VDOP.
         - /procframe/ComputeSatPCenter.[h|c]pp
            Computes the satellite antenna phase correction, in meters.
         - /procframe/Decimate.[h|c]pp
            Decimates GNSS Data Structures data.
         - /procframe/Dumper.[h|c]pp
            Dumps the values inside a GNSS Data Structure (GDS), and therefore
            is meant to be used with the GDS objects found in DataStructures.
         - /procframe/EclipsedSatFilter.[h|c]pp
            Filters out satellites that are eclipsed by Earth shadow.
         - /procframe/GravitationalDelay.[h|c]pp
            Computes the delay in the signal due to the gravity field.
         - /procframe/IonexModel.[h|c]pp
            Computes the main values related to a given GNSS IONEX model, i.e.,
            TEC value, ionospheric mapping function, slant ionospheric delay.
         - /procframe/Keeper.[h|c]pp
            This class examines a GNSS Data Structure (GDS) and keeps only
            specific values according to their TypeIDs.
         - /procframe/LICSDetector2.[h|c]pp
            Detects cycle slips using LI observables
         - /procframe/PhaseCodeAlignment.[h|c]pp
            Aligns phase with code measurements.
         - /procframe/ProcessingList.[h|c]pp
            Stores ProcessingClass objects in a list.
         - /procfrane/Pruner.[h|c]pp
            This class examines a GNSS Data Structure (GDS) and prunes specific
            values according to their TypeIDs.
         - /procframe/RequireObservables.[h|c]pp
            Filters out satellites that don't have the required observations.
         - /procframe/SatArcMarker.[h|c]pp
            Keeps track of satellite arcs caused by cycle slips.
         - /procframe/SolverPPP.[h|c]pp
            Computes the Precise Point Positioning (PPP) solution using Kalman
            solver that combines ionosphere-free code and phase measurements.
         - /procframe/SolverPPPFB.[h|c]pp
            Computes the Precise Point Positioning (PPP) solution using Kalman
            solver that combines ionosphere-free code and phase measurements
            and has forwards backwards mode.
         - /procframe/Synchronize.[h|c]pp
            Synchronizes two GNSS Data Structures data streams.
         - /rxio/ObsEphReaderFramework.[h|c]pp
            Object intened to be created in place of a CommandOptionParser.
         - /vplot/Axis.[h|c]pp
            Used to draw a simple axis.
         - /vplot/AxisStyle.hpp
            Used to describe an Axis's style.
         - /vplot/LinePlot.[h|c]pp
            Used to draw a lineplot.
         - /vplot/Plot.[h|c]pp
            Basic plot information.
         - /vplot/plottypes.hpp
            Defines plot types.
         - /vplot/ScatterPlot.[h|c]pp
            Used to draw a scatter plot.
         - /vplot/SeriesList.[h|c]pp
            Used to maintain series information for some types of plots.
         - /vplot/Splitter.[h|c]pp
            Used to help with splitting sets of points.
         - /vplot/SurfacePlot.[h|c]pp
            Used to draw a surface plot.

   - Changes in lib
         - /geomatics
            Major changes to the entire library. Adding new features as
            described in DDBase.cpp
         - /procframe/Equation.hpp
            Added '<' operator.
         - /procframe/LICSDetector.cpp
            Simpler method to compute 'currentDeltaT' is now used.
         - /procframe/MWCSDetector.cpp
            Simpler method to compute 'currentDeltaT' is now used.
         - /procframe/Variable.hpp
            Added '<' operator.

   Application modifications
   -------------------------
   - Added in apps
         - /clocktools
            Implements clock time-domain frequency stability metrics as well as
            data editing, noise identification, and plotting routines. Allows
            for basic clock stability analyses and is interoperable with other
            GPSTk programs. Run from the command line with output piped to
            other GPSTk programs.
         - /ObsArrayEvaluator
            Refactoring of mpsolve whic is the GPSTk Multipath Environment
            Evaluator. Computes statistical model of a dual frequency multipath
            combination. The model is a function of azimuth and/or elevation.
         - /performance
            Determines the slot numbers of satellites given input files. Also
            determines if satellites are in or out of the Base24 constellation
            at any given time.
         - /filetools/EphSum.cpp
            Summary of all ephemerides relevant to the day for a PRN.
         - /geomatics/relposition/StochasticModels.cpp
            Stochastic model for measurement errors used in estimation.
         - /geomatics/robust/lsfilt.cpp
            Read the data in one column of a file, and output as least-squares.
            filtered version of the data.
         - /swrx/acquire.cpp
            FFT based acquisition for GPS L1 band.
         - /swrx/hilbert.cpp
            Hilbert transform on samples from output file generated by SiGe
            SE4110L.
         - /swrx/position.cpp
            Position solution.
         - /swrx/RX.cpp
            Integrates position solutions into the tracking algorithm.
         - /swrx/trackerMT.cpp
            Parallel tracker for multiple PRNs.
         - /visibility/CalcDOPs.cpp
            Compute and display satellite visibility information.
         - /visibility/DiscreteVisibleCounts.[h|c]pp
            Discrete counts of how many SVs/stations are visible to a
            station/SV.
         - /visibility/DOPcalc.cpp
            Computes several dilution of precision (DOP) parameters.

   - Changes in apps
         - /geomatics
            Major changes to the entire application. Added new features in
            order to match the geomatics library changes.
         - /MDPtools/SummaryProc.[h|c]pp
            Better accounting of subframe errors.
         - /positioning/PRSolve.cpp
            Added option to process L1 only, L2 only, or iono-free.

   Example modifications
   -------------------------
   - Added in examples
         example8.cpp
            This program shows how to use GNSS Data Structures (GDS) to obtain
            "Precise Point Positioning" (PPP).
         example9.cpp
            This program shows how to use GNSS Data Structures (GDS) and other
            classes to build a reasonable complete application that computes
            "Precise Point Positioning" (PPP).
         example10.cpp
            This program shows how to use GNSS Data Structures (GDS) to obtain
            phase-based differential positioning (single-differences).
         example11.cpp
            Example of GPSTk's built-in postscript plotting routines.
         example12.cpp
            This is a example program to demonstrate some of the functionality
            of the ObsID class. The intent is to use ObsID as a key in a STL
            map of gps data. ObsID supports identifying the data in a manner
            that is similiar but can extend the Rinex 3 specification.
         example13.cpp
            Example of native surface plots.
   - Changes in examples
         example7.cpp
            Added example useage of code-based kalman filtering. Example 7 has
            also become a showcase for several of the new classes added since
            the last release.
   - Cleaned up and added comments to various examples to make them more useful


	#################################################

Version 1.5   Tuesday, March 11, 2008

	General modifications
	---------------------

	- Majority of version 1.5 comprised of revision number 1115, 
	  branching took place under revision 1115 from dev/ to RC1.5/

	- A large amount of work went into ensuring that the GPSTk built
	  on all supported platforms using both jam and make. A place has 
	  been set aside on the TWiki to inform developers on which platforms
	  the Tk has been built successfully.
	  http://www.gpstk.org/bin/view/Development/SourceStatus

	- The Tk now builds successfully on Mac OSX using jam and make

	- Added a library /dev/lib/procframe/ (from Processing Framework)
	  under which all pre-existing classes related to GNSS Data 
	  Structures (GDS) have been moved out of /src/.

	      + lib/procframe/CodeSmoother.[h|c]pp
	                      ComputeCombination.[h|c]pp
			      ComputeIURAWeights.[h|c]pp
			      ComputeLC.[h|c]pp
			      ComputeLI.[h|c]pp
			      ComputeLdelta.[h|c]pp
			      ComputeMOPSWeights.[h|c]pp
			      ComputeMelbourneWubbena.[h|c]pp
			      ComputePC.[h|c]pp
			      ComputePI.[h|c]pp
			      ComputePdelta.[h|c]pp
			      ComputeWindUp.[h|c]pp
			      DataHeaders.[h|c]pp
			      DataStructures.[h|c]pp
			      DeltaOp.[h|c]pp
			      LICSDetector.[h|c]pp
			      MWCSDetector.[h|c]pp
			      ModelObs.[h|c]pp
			      ModelObsFixedStation.[h|c]pp
			      ModeledPR.[h|c]pp
			      ModeledReferencePR.[h|c]pp
			      NablaOp.[h|c]pp
			      OneFreqCSDetector.[h|c]pp
			      PCSmoother.[h|c]pp
			      ProcessingClass.hpp
			      ProcessingVector.[h|c]pp
			      SimpleFilter.[h|c]pp
			      SolverLMS.[h|c]pp
			      SolverWMS.[h|c]pp

	- Integrated a new regex that is LGPL instead of GPL

	- GCC 3.3.5 chokes on the use of __attribute__ - avoid using it
	  unless we're compiling under GCC 4.x

	- Updates to the INSTALL and README including additional website 
	  information, information about new lib directory, an update of
	  the build instructions, and an additional plug for help and 
	  instruction available on the website

	- Added a script to parse apps for user manual
	        /apps/Wikify.pl

	Library modifications
	---------------------

	- Added src/BLQDataReader.[h|c]pp
	        Reads and parses ocean tides harmonics data in BLQ file format.
		Ocean loading displacement models usually use the ocean tide 
		harmonics to compute station biases due to this effect.
		You may find this data in 
		http://www.oso.chalmers.se/~loading/
	
	- Added src/ComputeWindUp.[h|c]pp
	        Computes the wind-up effect on the phase observables in radians.
	
	- Added src/MoonPosition.[h|c]pp
	        Computes the approximate position of the Moon in ECEF.

	- Added src/OceanLoading.[h|c]pp
	        Models station discplacement caused by ovean tides loading

	- Added src/PoleTides.[h|c]pp
	        Computes pole tides according to IERS conventions.

	- Added src/SatDataReader.[h|c]pp
	        Reads and parses satellite data from PRN_GPS-like files. JPL
		provides files called PRN_GPS with important satellite 
		information for some precise GPS data processing algorithms.
		You may find this file in 
		ftp://sideshow.jpl.nasa.gov:/pub/gipsy_products/gipsy_params
	
	- Added src/SolidTides.[h|c]pp
	        Computes the solid tide effect at a given position and epoch
		based on the Williams model (1970).

	- Added lib/rxio/NovatelData.[h|c]pp
	        lib/rxio/NovatelDtream.hpp
		Moved Novatel data parsing into the rxio library
	
	- Added lib/procframe/BasicModel.[h|c]pp
	        Computes the basic parts of a GNSS model. It is a more modular 
		alternative to classes such as ModelObs.

	- Added lib/procframe/ComputeLinear.[h|c]pp
	        ComputeLinear is a GDS-enabled class that allows you to
		compute several previously defined linear combinations in
		one pass and in a flexible way.
	
	- Added lib/procframe/ComputeTropModel.[h|c]pp
	        ComputeTropModel is a GDS-enabled class that computes and
		inserts the main values related to a given GNSS 
		tropospheric model. 
	
	- Added lib/procframe/CorrectObservables.[h|c]pp
	        Corrects observables from effects such as antenna 
		eccentricity, differences in phase centers, offsets due
		to tide effects, etc.

	- Added lib/procframe/Equation.[h|c]pp
	        Used to define and handle an equation when dealing with GDS.
	
	- Added lib/procframe/LinearCombinations.[h|c]pp
	        Defines handy linear conmbinations of GDS data.

	- Added lib/procframe/StochasticModel.[h|c]pp
	        Simple implementations of common stochastic models
		used in Kalman filters.
	
	- Added lib/procframe/Variable.[h|c]pp
	        Used to define and handle a variable when dealing with GDS

	- Improved lib/procframe/ComputeMOPSWeights.[h|c]pp
		More robust when ionospheric information is missing for GDS.
	
	- Improved lib/rxio/MDPNavSubframe.cpp
	           lib/rxio/RinexConverters.cpp
		The above classes now have better LLI computation.

	- Improved src/AlmOrbit.cpp
	           src/SEMAlmanacStore.[h|c]pp
	        Added GPS epoch handling option.
	
	- Improved src/AstronomicalFunctions.[h|c]pp
	           src/SunPosition.[h|c]pp
	        Improvements: Use of static members, definition of 
		constants, etc.

	- Improved src/EngEphemeris.[h|c]pp
	           src/EngNav.cpp
		   src/GPSEphemerisStore.[h|c]pp
	        Added AODO to FIC bit cracking. Added means to access stored
		eph moaps by SV.
	
	- Improved src/MSCData.[h|c]pp
	           src/MSCStore.[h|c]pp
	        Promoted getXvt from MSCStore to MSCData. 
		Old MSCStore::getXvt(string, DayTime) still there. Now 
		overloaded with MSCStore::getXvt(unsigned long, DayTime)

	- Improved src/SP3Data.[h|c]pp
	        Added a version guessing test to support SP3 better.
	
	- Improved src/TropModel.[h|c]pp
	        Added NeillTropModel, a useful model implementing Neill
		mapping functions and used in deodetic software such as 
		JPLs Gipsy/OASIS.
	
	- Improved src/YumaAlmanacStore.[h|c]pp
	        Added time_of_interest option to aid with 10-bit week problem.

	
	Application modifications
	-------------------------

	- Added apps/converters/ash2mdp.cpp
	        ash2mdp moved from apps/receiver/ to /apps/converters/

	- Added apps/converters/ash2xyz.cpp
	        Takes a stream of Ashtech observation and ephemeris data 
		and outputs SV positions and ionospheric corrections. Added
		an exponential filter to iono error and rate calculation.
	
	- Added apps/DataAvailability/DataAvailabilityAnalyzer.[h|c]pp
	        Includes the per-obs changes in the raw statistics.
	
	- Added apps/MDPtools/mdpscreen.cpp
	        mdpscreen moved back into MDPtools, but only being built 
		conditionally.

	- Added apps/MDPtools/mdpEdit.cpp
	        mdpEdit: filters messages based on record number.

	- Added apps/time/generateCalenders.bash
	        Added a simple script to generate los of calendars at once
		using calgps.

	- Improved apps/checktools/CheckFrame.hpp
	        Fixed CheckFrame to return 1 if any erors are encountered.
		Added a command-line-option to CheckFrom to exit on first
		error.

	- Improved apps/reszilla/OrdEngine.cpp
	        The trop model throws erros when the humidity is over 100.
		Instead of sending it values over 100, just send 100.

	- Improved apps/reszilla/OrdPlot.cpp
	        Added a % data autoranger and smoothing on the ords.
	
	- Improved apps/rfw/rfw.cpp
	        Added command line option that sends strings to the 
		source periodically.

	- Improved apps/swrx/PCodeGenerator.hpp
	        GCC 3.3.5 chokes on the use of __attribute__ - avoid using it
		unless we're compiling under GCC 4.x

	- Improved apps/time/calgps.cpp
	        Calgps now in graphical output.

	- Improved apps/visibility/WhereSat.cpp
	        + Got rid of "MATLAB-suitable" output option.
		+ Results now output in a tabular format only.
		+ Can process more than one SV, or all SVs.
		+ Added time format option.
		+ Output clock correction in ms, not seconds.
		+ Better exception handling.
	
	- Improved apps/MDPtools/
	        To allow the Tk to build under all platforms MDPTool was
		removed from all non UNIX based system builds, namely Windows.

	- Improved apps/receiver/
	        Compilation disabled becasue of severe linking problems.

	- Improved apps/reszilla/
	        Changed Install process to allow python scripts to be 
		installed without a hickup under Windows.


	#################################################

Version 1.4   Tuesday, December 18, 2007

	General modifications
	---------------------

	- Majority of version 1.4 comprised of revision number 863 
	  branching took place under revision 864 from dev/ to RC1.4/
	
	- Major refactoring of the Ephemeris/Almanac classes. This change may 
	  break some apps. To fix them the following translations are useful:
	
              Old                           |        New
    -------------------------------------------------------------------------------
    gpstk::EphemerisStore		    | gpstk::XvtStore<gpstk::SatID>
    gpstk::BCEphemerisStore		    | gpstk::GPSEphemerisStore
    gpstk::AlmanacStore                     | gpstk::GPSAlmanacStore
    gpstk::EphemerisStore::NoEphemerisFound | gpstk::InvalidRequest
    eph.getSatXvt(sat, time)	            | eph.getXvt(sat, time)
    eph.getSatXvt(prn, time)		    | eph.getXvt(SatID(prn, SatID::systemGPS), time)
    eph.dump(0, cout)			    | eph.dump(cout, 0)


        - Added an updated LaTeX source for table describing apps of v1.3, 
	        as well as a list of classes in the gpstk libraries 
	        (sorted & categorized) found in ref/glance/

	- Added LeTex source for the GPSTk paper at the ION-GNSS-2006

	- Added a library /dev/lib/vplot/ to create drawing primitives in SVG 
	        and postscript formats. Intended to support complex, fully 
	        customizable visualizations on GNSS analyses. Supported only 
	        in jam for now,	but with the hope of support in make.

	- Improved version control 
	  * set gpstk package version to 1.3 in the autoconf build world
	  * set the current shared library major version to 13, so gpstk 1.3
	    delivers lib{gpstk,rxio,mdplib,...}.so.13.0
	  * now allows jam and make to build shared libraries with the 
	    appropriate versions
	  * causes jam to install headers to $PREFIX/include/gpstk to prevent 
	    header namespace collision. This behaviour is consistent with the 
	    "make" build.

	- Moved apps/geomatics/lib library to dev/lib/geomatics

	Library modifications
	---------------------

	- Added src/InOutFramework.hpp
	        New framework that is useful for a program that processes one 
		input and produces one output. 
	
	- Added src/ModelObs.[h|c]pp
	        ModelObs class is a GDS-optimized version ModeledPR
	
	- Added src/GPSAlmanacStore.[h|c]pp
	        Part of the refactoring of the Ephemeris/Almanac classes
		A migration path has been provided for BCEPhemerisStore, 
		EphemerisStore, and AlmanacStore
	
	- Added src/GPSEphemerisStore.[h|c]pp
	        Part of the refactoring of the Ephemeris/Almanac classes
		A migration path has been provided for BCEPhemerisStore, 
		EphemerisStore, and AlmanacStore

	- Added src/XvtStore.hpp
	        Part of the refactoring of the Ephemeris/Almanac classes
		A migration path has been provided for BCEPhemerisStore, 
		EphemerisStore, and AlmanacStore
	
	- Added src/ProcessingClass.hpp
	        ProcessingClass is an abstract base class for all objects 
		processing GNSS Data Structures. This step furthers the GNSS 
		Data Structures (GDS) processing paradigm clearly separating 
		the GDS and the objects that process them.

	- Added src/ProcessingList.[h|c]pp
	        ProcessingList allows to create run-time defined and modified 
		lists of processing objects to be applied on GNSS data 
		structures. This class furthers the paradigm of GNSS data 
		processing as an assembly line.

	- Added src/ProcessingVector.[h|c]pp
	        ProcessingVector allows to create run-time defined and modified
		std::vectors of processing objects to be applied on GNSS data
		structures. This class furthers the paradigm of GNSS data
		processing as an assembly line.

	- Added src/SimpleFilter.cpp
	        SimpleFilter class was modified in order to inherit from 
	        ProcessingClass.
	
	- Added lib/vplot/Helper.hpp
	        A helper class for creating VGImage pointer for a file name 
		using the extension.

	- Added src/ComputeLC.cpp
	        src/ComputeLI.cpp
		src/ComputeMelbourneWubbena.cpp
		src/ComputePC.cpp
		src/OneFreqCSDetector.cpp
		src/LICSDetector.cpp
		src/MWCSDetector.cpp
		src/CodeSmoother.cpp
		src/ComputeLdelta.cpp
		src/ComputePI.cpp
		src/CoputePdelta.cpp
		src/PCSmoother.cpp
		src/ComputeIURAWeights.cpp
		src/ComputeMOPSWeights.cpp
		The above classes were added to inherit from ProcessingClass

	- Improved src/XYZ2NED.[h|c]pp
	           src/NablaOP.[h|c]pp
		   src/DeltaOP.[h|c]pp
		The above classes were modified to inherit from ProcessingClass

	- Improved lib/rxio/AshtechData.[h|c]pp
	           lib/rxio/AshtechMBEN.[h|c]pp
		   lib/rxio/AshtechPBEN.[h|c]pp
		   lib/rxio/MDPHeader.hpp
		   lib/rxio/MDPObsEpoch
		   lib/rxio/ObsUtils.cpp
		   lib/rxio/ash2mdp.cpp
	        Added checksum checking to ashtech data, and made the parsing
		a bit more robust
	
	- Improved lib/rxio/FFIdentifier.[h|c]pp
	        FFIdentifier no londer uses iostream exceptions to determine
		file type

	- Improved lib/rxio/EphReader.[h|c]pp 
	        Added support for YUMA and SEM streams

	- Improved src/SatID.hpp
	        Added systemUderDefined to SatId and corresponding change 
	        in example7.cpp


	Application modifications
	-------------------------

	- Added apps/visibility/svvis.cpp
	        Computes when satellites are visible at a given point 
		on the earth

	- Improved apps/DataAvailability/DataAvailabilityAnalyzer.[h|c]pp
	        Added a position option
	
	- Improved apps/receiver/rfw.cpp
	        Added new input option??????
	
	- Improved apps/visibility/findMoreThan12.cpp
	        Added SV azmith to output

	- Improved apps/time/calgps.cpp
	        Added ability to draw a calender

	

Version 1.3   Monday, July 16, 2007

	General modifications
	---------------------

	- Most of version 1.3 comprised of revision number 701

	- gpstk1.3/apps/time/calgps.cpp was pulled from revision nember 707

	- The current msi for the windows package may not update the 
	        previous version installations
	
	- Added examples/example7.cpp & associated RINEX data files
	        This program shows 10 different ways to process GPS data using
		"GNSS data structures". The "GNSS Data Structures" paradigm can 
	        process GPS code-based data based on Vectors, Matrices 
		and other objects.
	
	- Added examples/example6.cpp
	        Shows a rather minimalist way to process GPS data using GNSS Data
		Structures

	- Improved src/icd_200_constants.hpp
	        Fixed RSVCLK value	

	- GPSTk is now able to build under the MS Visual Studio 2005 (Express
	        Edition)

	- A simulator has been added but is not part of the binary build, but
	        can be found in the source tarball under gpstk/apps/swrx

	Library modifications
	---------------------

	- Added src/SimpleKalmanFilter.[h|c]pp
	        Implements a version of the Kalman filter, based in Vectors
		and Matrices - not yet GNSS data structures-enabled

	- Added src/NablaOp.[h|c]pp
	        Applies differences in satellite-related data to the GNSS
		data structures (a.k.a. GDS)

	- Added src/TypeID.[h|c]pp
	        Index able to represent any type of observation, correction,
		model parameter or other data value of interest 

	- Added src/DataHeaders.[h|c]pp
	        Adds several headers to be used with the GNSS data structures
		classes

	- Added DataStructures.[h|c]pp
	        Set of data structures to be used by other GPSTk classes

	- Added src/DeltaOp.[h|c]pp
	        Applies differences on ground-related data to the GNSS data
		structures

	- Added src/ComputeCompination.hpp
	        Base class to ease computing of data for the GNSS data structures

	- Added src/ComputePC.hpp
	        Eases computing the PC combination for GNSS data structures

	- Added src/ComputeLC.hpp
	        Eases computing the LC combination for GNSS data structures

	- Added src/ComputeLI.hpp
	        Eases computing the LI combination for GNSS data structures

	- Added src/ComputeLdelta.hpp
	        Eases computing the Ldelta combination for GNSS data structures

	- Added src/ComputePdelta.hpponly
	        Eases computing the Pdelta combination for GNSS data structures

	- Added src/ComputePI.hpp
	        Eases computing the PI combination for GNSS data structures

	- Added src/ComputeMelbourneWubbena.hpp
	        Eases computing the ComputeMelbourneWubbena combination for 
		GNSS data structures

	- Added src/ComputeIURAWeights.hpp
	        Computes satellites weights based on URA Index for GNSS data
		structures

	- Added src/ComputeMOPSWeights.hpp
	        Computes satellites weights based on Appendix J of MOPS C
		(RTCA/DO-229C), for GNSS data structures

	- Added src/SimpleFilter.hpp
	        Filters satellites with observables grossly out of bounds, 
		intended for GNSS data structures

	- Added src/OneFreqCSDetector.hpp
	        Detects cycle slips using observables in just one frequency

	- Added src/LICSDetector.hpp
	        Detects cycle slips using LI observables

	- Added src/MWCSDetector.hpp
	        Detects cycle slips using the Melbourne-Wubbena combination

	- Added src/CodeSmoother.hpp
	        Smoothes a given code observable using the corresponding phase 
		observable

	- Added src/PCsmoother.hpp
	        Smoothes PC code observables using the corresponding LC phase
		observables

	- Added src/XYZ2NEU.[h|c]pp
	        Reference frame change from ECEF XYZ to topocentric North-East-Up

	- Added src/XYZ2NED.[h|c]pp
	        Reference frame change from ECEF XYZ to topocentric North-East-Down

	- Added src/TimeNamedFileStream.hpp
	        Simple time-based file name stream

	- Improved src/ModeledReferencePR.[c|h]pp
	        Adapted to work with GNSS data structures

	- Improved src/ModeledPR.[h|c]pp
	        Adapted to work with GNSS data structures

	- Improved src/TropModel.hpp
	        Added method MOPSTropModel::setAllParameters()

	- Improved src/SolverLMS.[h|c]pp
	        Adapted to work with GNSS data structures

	- Improved src/SolverWMS.[h|c]pp
	        Adapted to work with GNSS data structures

	- Improved src/icd_200_constants.hpp
	        Added data regarding L5, L6, L7 and L8

	- Improved src/RinexHeader.[h|c]pp
	        Can now process RINEX2.11 navigation files

	- Improved src/icd_200_constants.hpp
	        Fixed RSVCLK

	- Improved src/SP3Data.cpp
	        Modified the reader to be tolerant of short PG lines

	- Improved src/RinexUtilitties.[h|c]pp
	        Added a sort routine 

	- Improved src/EngAlmanac.[h|c]pp
	        Routing added that performs a simple sanity check on the almanac
	
	Application modifications
	-------------------------

	- Improved apps/differential/vecsol.cpp
	        Along with other submissions allows the toolkit to build under
		the MS Visual Studio 2005

	- Improved apps/Rinextools.[h|c]pp
	        Increased robustness and added multiple input file capabilities

	- Improved all tools to process input files in time order:
	        apps/Rinextools/ResCor.cpp
		apps/Rinextools/RinSum.cpp
		apps/Rinextools/RinexDump.cpp
		apps/positioning/PRSolve.cpp

Version 1.2   Monday, November 6, 2006
	
	General modifications
	---------------------
	
	- Moved from Perforce to a Subversion repository located at 
		https://svn.sourceforge.net/svnroot/gpstk
	
	- GPSTk library testing is now being implemented in dev/tests
		These unit tests for the library currently cover over
		40% of the code with an average of 95% coverage for 
		tested classes.  Tests use CppUnit and Perl scripts.
		See test documentation for more information.
	
	- Added examples/example5.cpp
		An example that shows how to use some very useful high level
		GPSTk classes for positioning
			
	- Added Python support to the GPSTk
	
	- Added capability for FileHunter to work under Window and Cygwin
	
	Library modifications
	---------------------
	
	- Added RINEX version 2.11 support for GPS Navigation files
	
	- Added the following prototype time classes:
		src/CommonTime.[h|c]pp
		src/YDSTime.[h|c]pp
		src/CivilTime.[h|c]pp
		src/GPSEpochWeekSecond.[h|c]pp
		src/GPSWeekSecond.[h|c]pp
		src/GPSWeekZcount.[h|c]pp
		src/JulianDate.[h|c]pp
		src/MJD.[h|c]pp
		src/UnixTime.[h|c]pp
		
	- Added src/TimeConstants.hpp
		Time constants
	
	- Added src/TimeConverters.[c|h]pp
		Time converstion routines
	
	- Added src/Bancroft.[h|c]pp 
		Class gets an initial guess of GPS receiver's position
		
	- Added src/SVExclusionList.[h|c]pp
	
	- Added and implemented src/SatID.hpp
		Class which encapsulates Satellite Identification information
	
	- Added src/ModeledPR.[h|c]pp
		Encapsulates computation of modeled pseudoranges of a mobile
		receiver
		
	- Added src/SimpleIURAWeight.[h|c]pp
		Assigns weights to satellites based on their URA Index
	 
	- Added src/ModeledPseudorangeBase.hpp
		An abstract base class for modeled pseudoranges
	
	- Added src/WeightBase.hpp
		An abstract base class for algorithms assigning weights to satellites
			
	- Improved src/Matrix.hpp and src/Vector.hpp
		Added unary minus operator, CholeskyCrout class and inverseChol method
	
	- Improved src/Vector.hpp
		Added add() method
		
	- Improved src/Stats.hpp 
		Better handling of weighted input
		
	- Improved src/Tropmodel.[h|c]pp
		Added GCAT tropospheric model
		Added MOPSTropModel
		
	- Improved src/Position.[h|c]pp 
		Added methods elevationGeodetic() and azimuthGeodetic()
	
	- Improved src/TabularEphemerisStore.[h|c]pp
		Added method hasTGD()
		
	- Improved src/BCEphemerisStore.[h|c]pp
		Added method getTGD() to return the Total Group Delay of the SV
	
	- Improved src/EphemerisRange.[h|c]pp
		Added methods elevationGeodeitc() and azimuthGeodetic().
		Changed RelativityCorrection() to improved speed and precision
	
	- Added src/BinexData.[c|h]pp BinexFilterOperators.hpp BinexStream.hpp
		Added BINEX code
	
	Application modifications
	-------------------------
	
	- Added apps/positioning/poscvt
		A position converter modeled after timecvt
		
	- Added apps/geomatics/relposition/DDBase
		A network positioning application that uses double
		differenced carrier phases
	
	- Added apps/geomatics/kalman
		Tools for Kalman filtering
		
	- Added apps/geomatics/robust
		Tool for robust statistics
	
	- Added apps/geomatics/????
		Added geodetic reference frames
		
	- Complete rewrite and move of apps/cycleslips/DiscFix
	  Located now in apps/geomatics/cycleslips/DiscFix
	  	DiscFix is now much more object-oriented, faster, more robust
		and more accurate.  Significant changes have been made to the
		algorithm.
	
	- Added apps/visibility/wheresat
		A tool for calculating SV position, azimuth and
		elevation from a navigation file
	
	- Added apps/converters/novaRinex
		A NovAtel binary to Rinex converter
	
	- Improved apps/filetools/navdmp
		Added -r (RINEX) input capability
	
	- Added apps/qa/Expression
		A fully functional expression interpreter
	
	- Added apps/qa/obsrip
		A utility which transforms RINEX files into linear combos, ect.
	
	- Added functionality to apps/multipath/ObsArray
		Can track observations by PRN and by epoch.  Also can insert
		GPS constants into Expressions.
		
	- Added apps/positioning/posInterp
		This application interpolates positions in a RINEX file, increasing
		the data rate by an integer factor.  An example that uses both
		posInterp and PRSolve is included.
	
	- Added apps/visibility/findMoreThan12
		A new tools which given an ephemeris source, finds times when there are
		more than 12 SVs simultaneously above a specified elevation angle.
	
	- Improved apps/positioning/rinexpvt
		Added optional log file to output
		Added option to set observation rate to other than 30s
		
	
Version 1.1   Friday, January 7, 2005

	General modifications
	---------------------

	- Added patches to support more compilers:
	     GNU compiler version 3.4
	     GNU compiler under AIX
	     Sun ONE Studio 8

	- GNU make/configure capabilities have been added. Since this
	  is a new feature, your mileage may vary. The makefiles
	  work on systems with newer autoconf toolchain versions.

	- More examples were added to gpstk/examples.
	  These examples are described in the website.

	- Major revamp of the website.
	
	Library modifications
	---------------------

	- P code generator was allocating over 20 megabytes in
	  processes that use the shared library. Now the 20 megabytes
	  are not allocated until the P code generator is initialized
	  by the user.
	
	- Bug fix: FileSpec::extractDayTime() now initializes
	  DayTimes generated from file name to have zero hours,
	  zero minutes, zero seconds, etc.
	
	- Bug fix: DayTime::setGPS now more robust when accepting
	  a year number as a hint to disambiguate which full GPS
	  week to use.
	
	- Added more DayTime tests.
	
	- Bug fix: DayTime user-defined tolerance for comparisons now is
	  working.

	- Bug fix: DayTime::setGPS(short week, long zcount, short year)
	  now works across a year rollover.
	
	- Renamed WxObsData::find(...) to getWxObservation(...) to remove
	  any confusion that might develop over having the same method
	  name as the STL.
	
	- Bug fix: satellite combinations in RAIM sometimes incorrect when 
	  a user-marked satellite was present.

	- Some Vector routines were throwing Exception instead of 
	  VectorException.

	- Added Vector-Matrix concatentation operators.

	- Added a sub-Vector constructor.

	- Modification to Householder decomposition routine.
	
	- Various fixes to MatrixOperators.

	- Updates to FIC routines.

	- Implemented azimuth and elevation functions in Position, making 
	  use of the functions inherited from Triple. Modified EphemerisRange
	  to use Position, as well as TropModel and ResCor; this eliminated 
	  a bug in the azimuth() in EphemerisRange and means that now there 
	  is only one implementation of az and el. Added a test of the new 
	  routines in positiontest. Also added a Position constructor from Xvt.

	- Bugs in DayTime::setToString() - %f was missing (!) and %S 
	  was acting as %f should; date was trashed when time was set, 
	  and vice versa, and year input was not properly handled.

	- Implemented ionospheric model types other than linear.

	- Cleaned up weather data handling (e.g., error checks).

	
	Application modifications
	-------------------------
	
	apps/difftools

	- Unified code using a common class.


	apps/filetools

	- Improvments to navdmp.

	- New utility called rinexthin for subsampling RINEX observation
	  files.

	
	apps/binding
	
	- This  new project provides GPSTk bindings in other languages
	  has been added. This capability is in an alpha state.
	  Currently there are a limited set of bindings for
	  Tcl/Tk, Python and Octave.

	
	apps/RinexEditor

	-  Bug fix: 'delete all' command was being deleted prematurely.


	apps/RinexPlot

	- Improved documentation.

	- Various tweaks.

	- Plotting improvements.


	apps/ionosphere

	- Various improvements.










	
