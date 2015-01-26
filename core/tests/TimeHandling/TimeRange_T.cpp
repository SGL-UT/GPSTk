#pragma ident "$Id: //depot/msn/main/code/shared/gpstk/test/TimeRangeTest.cpp#5 $"

#include <iostream>
#include "TestUtil.hpp"
#include "CivilTime.hpp"
#include "BasicFramework.hpp"
#include "TimeRange.hpp"
#include "CommonTime.hpp"

using namespace std;
using namespace gpstk;


class TimeRange_T
{
	public:
		TimeRange_T() {eps = 1E-12;}
		~TimeRange_T() {}

		int constructorTest ( void )
		{
			TestUtil testFramework( "ANSITime", "Constructor", __FILE__, __LINE__ );
			testFramework.init();

			std::string outputFormat = CivilTime().getDefaultFormat();
//--------------TimeRange_ _1 - Verify default constructor does not throw errors
			try {TimeRange emptyConstructed; testFramework.passTest();}
			catch (...) {testFramework.failTest();}
			TimeRange emptyConstructed;

//--------------TimeRange_ _2 - Verify default constructor sets the proper values (This is important to verify the values in the copy constructor below.)
			testFramework.assert( (emptyConstructed.getStart() == gpstk::CommonTime::BEGINNING_OF_TIME) &&
				              (emptyConstructed.getEnd() == gpstk::CommonTime::END_OF_TIME) );
			testFramework.next();

//--------------TimeRange_ _3 - Verify copy constructor does not throw errors
			try {TimeRange copyConstructed(emptyConstructed); testFramework.passTest();}
			catch (...) {testFramework.failTest();}
			TimeRange copyConstructed(emptyConstructed);

//--------------TimeRange_ _4 - Verify copy constructor sets the proper values
			testFramework.assert( (copyConstructed.getStart() == gpstk::CommonTime::BEGINNING_OF_TIME) &&
				              (copyConstructed.getEnd() == gpstk::CommonTime::END_OF_TIME) );
			testFramework.next();	

		//Variables for input into constuctors with multiple inputs
			CommonTime startEndpoint = gpstk::CivilTime(2011,1, 1, 0, 0,0.0).convertToCommonTime();
			CommonTime endEndpoint = gpstk::CivilTime(2011,1,31,23,59, 59.59).convertToCommonTime();
			gpstk::TimeRange::DTPair inputPair;
			inputPair = std::make_pair(startEndpoint,endEndpoint);
			bool beginningIncluded = true;
			bool endIncluded = false;

//--------------TimeRange_ _5 - Verify CommonTime constructor does not throw errors
			try {TimeRange fourInputConstructed(startEndpoint,endEndpoint,beginningIncluded, endIncluded); testFramework.passTest();}
			catch (...) {testFramework.failTest();}
			TimeRange fourInputConstructed(startEndpoint,endEndpoint,beginningIncluded, endIncluded);

//--------------TimeRange_ _6 - Verify CommonTime constructor sets the proper values (Implies that CommonTime == comparisons work)
			testFramework.assert( (fourInputConstructed.getStart() == startEndpoint) &&
				              (fourInputConstructed.getEnd() == endEndpoint) );
			testFramework.next();	
		
//--------------TimeRange_ _7 - Verify DTpair constructor does not throw errors
			try {TimeRange threeInputConstructed(inputPair,beginningIncluded, endIncluded); testFramework.passTest();}
			catch (...) {testFramework.failTest();}
			TimeRange threeInputConstructed(inputPair,beginningIncluded, endIncluded);

//--------------TimeRange_ _8 - Verify DTpair constructor sets the proper values (Implies that CommonTime == comparisons work)
			testFramework.assert( (threeInputConstructed.getStart() == startEndpoint) &&
				              (threeInputConstructed.getEnd() == endEndpoint) );
			testFramework.next();	

			return testFramework.countFails();
		}

		int inRangeTest ( void )
		{
			TestUtil testFramework( "ANSITime", "inRange", __FILE__, __LINE__ );
			testFramework.init();

			CommonTime earlierThanRange = gpstk::CivilTime(2010,12,20, 0, 0, 0.0 ).convertToCommonTime();	
			CommonTime startEndpoint    = gpstk::CivilTime(2011, 1, 1, 0, 0, 0.0 ).convertToCommonTime();
			CommonTime timeInRange      = gpstk::CivilTime(2011, 1,20, 0, 0, 0.0 ).convertToCommonTime();
			CommonTime endEndpoint      = gpstk::CivilTime(2011, 1,31,23,59,59.59).convertToCommonTime();
			CommonTime laterThanRange   = gpstk::CivilTime(2011, 2,20, 0, 0, 0.0 ).convertToCommonTime();

		//Create a TimeRange where both ends are included
			bool beginningIncluded = true;
			bool endIncluded = true;
			TimeRange bothEndsIncluded(startEndpoint, endEndpoint, beginningIncluded, endIncluded);
		
		//Create a TimeRange where both ends are excluded
			beginningIncluded = false;
			endIncluded = false;
			TimeRange bothEndsExcluded(startEndpoint, endEndpoint, beginningIncluded, endIncluded);

//--------------TimeRange_ _1 - Verify inRange returns false for time earlier than the range given when both ends are included
			testFramework.assert( !bothEndsIncluded.inRange(earlierThanRange) );
			testFramework.next();

//--------------TimeRange_ _2 - Verify inRange returns true for earlier endpoint when both ends are included
			testFramework.assert( bothEndsIncluded.inRange(startEndpoint) );
			testFramework.next();

//--------------TimeRange_ _3 - Verify inRange returns true for time inside the range when both ends are included
			testFramework.assert( bothEndsIncluded.inRange(timeInRange) );
			testFramework.next();

//--------------TimeRange_ _4 - Verify inRange returns true for later endpoint when both ends are included
			testFramework.assert( bothEndsIncluded.inRange(endEndpoint) );
			testFramework.next();

//--------------TimeRange_ _5 - Verify inRange returns false for time later than the range given when both ends are excluded
			testFramework.assert( !bothEndsIncluded.inRange(laterThanRange) );
			testFramework.next();

//--------------TimeRange_ _6 - Verify inRange returns false for time earlier than the range given when both ends are excluded
			testFramework.assert( !bothEndsExcluded.inRange(earlierThanRange) );
			testFramework.next();

//--------------TimeRange_ _7 - Verify inRange returns false for earlier endpoint when both ends are excluded
			testFramework.assert( !bothEndsExcluded.inRange(startEndpoint) );
			testFramework.next();

//--------------TimeRange_ _8 - Verify inRange returns true for time inside the range when both ends are excluded
			testFramework.assert( bothEndsExcluded.inRange(timeInRange) );
			testFramework.next();

//--------------TimeRange_ _9 - Verify inRange returns false for later endpoint when both ends are excluded
			testFramework.assert( !bothEndsExcluded.inRange(endEndpoint) );
			testFramework.next();

//--------------TimeRange_ _10 - Verify inRange returns false for time later than the range given when both ends are excluded
			testFramework.assert( !bothEndsExcluded.inRange(laterThanRange) );
			testFramework.next();

			return testFramework.countFails();
		}

		int priorToTest ( void )
		{
			TestUtil testFramework( "ANSITime", "priorTo", __FILE__, __LINE__ );
			testFramework.init();

			CommonTime earlierThanRangeStart = gpstk::CivilTime(2010,12,20, 0, 0, 0.0 ).convertToCommonTime();	
			CommonTime earlierThanRangeEnd   = gpstk::CivilTime(2010,12,29, 0, 0, 0.0 ).convertToCommonTime();
	
			CommonTime startEndpoint         = gpstk::CivilTime(2011, 1, 1, 0, 0, 0.0 ).convertToCommonTime();

			CommonTime timeInRangeStart      = gpstk::CivilTime(2011, 1,10, 0, 0, 0.0 ).convertToCommonTime();
			CommonTime timeInRangeEnd        = gpstk::CivilTime(2011, 1,20, 0, 0, 0.0 ).convertToCommonTime();

			CommonTime endEndpoint           = gpstk::CivilTime(2011, 1,31,23,59,59.59).convertToCommonTime();

			CommonTime laterThanRangeStart   = gpstk::CivilTime(2011, 2,20, 0, 0, 0.0 ).convertToCommonTime();
			CommonTime laterThanRangeEnd     = gpstk::CivilTime(2011, 2,27, 0, 0, 0.0 ).convertToCommonTime();

		//Create a TimeRange where both ends are included
			bool beginningIncluded = true;
			bool endIncluded = true;
			TimeRange bothEndsIncluded(startEndpoint, endEndpoint, beginningIncluded, endIncluded);

			return testFramework.countFails();
		}
	private:
		double eps;
};


/*

	// Test comparison operators 
	TimeRange testRange = tr11;// Exercise copy constructor;
	TimeRange comp[6];			// Default constructor should set all these to begining/end of time.
	string compDefs[6] = { "Range completely after comparison span", 
								  "Front overlap",
								  "End overlap",
								  "Complete overlap",
								  "Interior overlap",
								  "Range completely before comparison span" };
	CommonTime priorStart = CivilTime(2010, 1, 1, 0, 0, 0.0);
	CommonTime priorEnd = CivilTime(2010, 2, 1, 0, 0, 0.0);							 
	comp[0].set( priorStart, priorEnd, true, true );

	CommonTime middle = CivilTime(2011, 1, 15, 0, 0, 0.0);
	comp[1].set( priorStart, middle, true, true );

	CommonTime afterStart = CivilTime(2011, 3, 1, 0, 0, 0.0);
	CommonTime afterEnd = CivilTime( 2011, 3, 31, 0, 0, 0.0);
	comp[5].set( afterStart, afterEnd, true, true ); 

	comp[2].set( middle, afterEnd, true, true );

	comp[3].set(priorEnd, afterStart, true, true);

	CommonTime interiorStart = CivilTime(2011,1,10,0,0,0.0);
	CommonTime interiorEnd = CivilTime(2011,1,20,0,0,0.0);
	comp[4].set( interiorStart, interiorEnd, true, true);

	string tform2("%02m/%02d/%02y");
	cout << "Comparion Operator Results" << endl;
	cout << "!				 Value				 !				  CompTo				 ! isPriorTo() ! Overlaps()  ! isAfter() ! Test case"  << endl;
	for (int i1=0;i1<6;i1++)
	{
		cout << testRange.dump(tform2) << " ! " << comp[i1].dump(tform2);
		cout << " !		 "  << testRange.isPriorTo(comp[i1]); 
		cout << "	  !		"  << testRange.overlaps(comp[i1]); 
		cout << "		!	  "  << testRange.isAfter(comp[i1]); 
		cout << "	  ! "  << compDefs[i1] << endl;
	}

	cout << "- - - setToString( )/printf( ) tests - - - " << endl;;

	// TimeRange setToString() test cases
	std::string testStrings[] = 
	{
		// Y/m/d H:M:S 
		"[2012 1 1 0 0 0.0, 2012 1 31 23 59 59.9]", // Inclusive
		"(2012 1 1 0 0 0.0, 2012 1 31 23 59 59.9)", // Exclusive
		"2012 1 1 0 0 0.0, 2012 1 31 23 59 59.9",// Inclusive default
		"(2012 1 1 0 0 0.0, 2012 1 31 23 59 59.9]", // Exclusive/Inclusive
		"[2012 1 1 0 0 0.0, 2012 1 31 23 59 59.9)", // Inclusive/Exclusive
		
		// Year, DOY, SOD
		"[2012 001 0.0, 2012 031 86399.]",
		"[2011 360 0.0,  2011 365 84599.0]",
		"(2003 1 42300.0, 2003 180 42300.0)",
		
		// Exception cases
		// string = "%Y %m %d %H %M %S"
		"[2012 1 31 23 59 59.9, 2012 1 1 0 0 0.0]", // Out of order
		"[2012 1 1 0 0 0.0,@ 2012 1 31 23 59 59.9]", // Invalid character
		"[2012 1 1 ! 0 0 0.0, 2012 1 31 $ 23 59 59.9]" // Invalid character
																  // DayTime.setToTime( ) processes
																  // this, returns the wrong answer
																  // and does NOT throw.	Why? 
	};

	std::string testFmts[]=
	{
		"%Y %m %d %H %M %S",
		"%Y %m %d %H %M %S",
		"%Y %m %d %H %M %S",
		"%Y %m %d %H %M %S",
		"%Y %m %d %H %M %S",
		"%Y %j %s",
		"%Y %j %s",
		"%Y %j %s",
		"%Y %m %d %H %M %S",
		"%Y %m %d %H %M %S",
		"%Y %m %d %H %M %S"
	};
	int numTests = 11; 
	
	for (int i=0;i<numTests;++i)
	{
		try
		{
			cout << "Testing '" << testStrings[i] << "'" << endl;
			testRange.setToString(testStrings[i],testFmts[i]);
			cout << "Result: '" << testRange.printf(testFmts[i]) << "'" << endl;
		}
		catch(gpstk::InvalidRequest exc)
		{
			cout << "Caught an InvalidRequest" << endl;
			cout << exc << endl;
		}
		catch(StringUtils::StringException exc)
		{
			cout << "Caught a StringException" << endl;
			cout << exc << endl;	
		}
		catch(TimeRange::TimeRangeException exc)
		{
			cout << "Caught a TimeRangeException" << endl;
			cout << exc << endl;
		}
	}

	cout << "Test Processing Complete." << endl;
}
*/

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	TimeRange_T testClass;

	check = testClass.constructorTest();
	errorCounter += check;

	check = testClass.inRangeTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}

