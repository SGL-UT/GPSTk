     Andrew Kuck <kuck@arlut.utexas.edu> (7):
           Keep all debian files as artifacts.
           Update build.sh to create python site-packages directory if needed.
           Add workaround for possible undocumented SWIG bug
           Support older versions of SWIG
           Automated tagged release updates.
           Manual update to release notes.
           Removed merges to master.

     Brian Tolman <btolman@arlut.utexas.edu> (3):
           correct decimation, add func to dump one epoch RINEX data, and add many explanatory comments
           correct Geomatics code, add output-RINEX-version option to tools
           bug in handling * in wanted obs types

     John Knutson <johnk@arlut.utexas.edu> (68):
           Deprecate SVNumXRef, replacing it with SatMetaDataStore
           Add comments describing CSV columns
           Change SVN field in SatMetaData to string to better support non-GPS GNSSes
           Add support for signal information association to satellite metadata
           Add OrbAlmStore tests for computeXvt and getSVHealth with unhealthy satellites
           Add tests to verify how effectivity is used by computeXvt, getXvt and getXvt_WithinValidity
           Clean up ObsID_T.cpp
           Add translations between enums and strings for ObsID, and test
           Clean up NavID_T.cpp
           Fix wonky indentation
           Add asString/asNavType methods to mirror what I did to ObsID
           Add more detail to exception message when encountering corrupt ephemerides
           Add nested classes back to Rinex3ObsHeader in SWIG environment for experiment
           Change RINEX 3 obs header field presence indicator from integer to class
           Migrate Yuma almanac code and tests to core
           Migrate SEM almanac code and tests to core
           Remove unused tests for code that no longer exists
           Clear junk from ext/tests/PowerSum and modify pstst.cpp to use TestUtil
           Migrate PowerSum code and tests to core
           Remove cruft
           Save test output files as artifacts when test stages fail
           Copy corrected test reference data from other issue 420 branch
           try again
           Reorder columns in CSV to system,svn,prn
           Change satellite record end time handling to be exlusive (< rather than <=)
           Fix comments in CSV to reflect changes in column order
           Make SatMetaDataStore::loadData return false if invalid records are encountered
           Trying to fix timeconvert Julian date test for windows
           Still trying to fix timeconvert Julian date test for windows
           Windows just can't convert the specific time used in the test case for timeconvert Julian date, so try to make a test case that will succeed
           Adjust the Julian date to fit in Windows' fake long double
           Still trying to find a JD/MJD that windows doesn't fail at
           Fix tests I broke and fix tests that were already broken
           Rename reference data to match actual contents
           Handle RINEX encoding and decoding of GEO and QZSS satellite identifiers
           Make sure the constructors validate the input
           Handle SP3 encoding and decoding of QZSS satellite identifiers
           Kludge to allow invalid IGS data files
           Make MARKER TYPE not required, which is in violation of RINEX 3, but eliminates the pain of dealing with all those invalid IGS files
           Remove the #ifndef directives that were commented out
           Moved windows extension removal from BasicFramework into CommandOptionParser which parses argv[0] directly
           add tests for rinex obs id decoding (commented out ones that currently fail)
           Fix what appears to have been a copypasta error resulting in incorrect tracking code mapping
           Add support for the codes recently added to the signal decoder ring in toolkit_docs
           Add support for RINEX 3 channel and ionospheric delay pseudo-observable.
           Update RinDump --typehelp for new codes and pseudo-observables.
           Add kludges in Rinex3ObsHeader for the one-off handling of BDS 1IQX in 3.02
           Integrate most of the rest of the changes from issue_397_RINEX304
           Change getWavelength, getAlpha, getBeta to use SatelliteSystem instead of unnecessary SatID.
           Resolve problems with python because of changes to consts
           Add an asString() method to RinexObsID that takes a version number to override the internally stored version and make Rinex3ObsHeader use this method so that the written header has the correct obs IDs.
           Additional documentation to help clarify the rinex version handling WRT ObsID.
           Update RinDump tests to include the new rinex version output
           Test handling of wildcards in ObsID
           Update Doxyfile to latest version.
           More documentation and examples
           Fix comments in examples that were copy-pasted and incorrect
           windows doesn't like forced NaNs
           Change position coordinate system conversion methods to return a reference
           Update invalid rinex header message to better reflect the newer enum usage
           Moving HelmertTransform tests
           Remove useless tests for 11 year old API
           remove ersatz test code for BrcClockCorrection and add new automated tests that perform a similar function
           Remove old BrcKeplerOrbit tests
           Use df_diff for Yuma_T test
           Remove ancient tests for classes that no longer exist
           Add comment to Rinex3ObsHeader/Data indicating the phase shift concern
           Add comments explaining Debian 7 problems

     Quang Duong <qduong@arlut.utexas.edu> (3):
           Declare via SWIG's exceptionclass directive that gpstk::Exception is an Exception
           Replace using syntax with typedef
           Resolve Rinex3ObsHeader nested class/struct issues

     Taben Malik <malik@arlut.utexas.edu> (3):
           Reintroduced usage of gpstk_typemaps.i
           Added swig support for type checking array arguments of overloaded functions
           Modified swig test to show swig support of Python lists to C arrays

