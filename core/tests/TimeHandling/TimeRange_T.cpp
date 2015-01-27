#pragma ident "$Id: //depot/msn/main/code/shared/gpstk/test/TimeRangeTest.cpp#5 $"

#include <iostream>
#include <sstream>
#include <string>
#include "TestUtil.hpp"
#include "CivilTime.hpp"
#include "BasicFramework.hpp"
#include "TimeRange.hpp"
#include "CommonTime.hpp"
#include "Exception.hpp"
using namespace std;
using namespace gpstk;


class TimeRange_T
{
	public:
		TimeRange_T() {eps = 1E-12;}
		~TimeRange_T() {}

/* =========================================================================================================================
	Test for the TimeRange constructors
========================================================================================================================= */
		int constructorTest ( void )
		{
			TestUtil testFramework( "TimeRange", "Constructor", __FILE__, __LINE__ );
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
			gpstk::TimeRange::DTPair inputPair, invertedPair;
			inputPair = std::make_pair(startEndpoint,endEndpoint);
			invertedPair = std::make_pair(endEndpoint,startEndpoint);			
			bool beginningIncluded = true;
			bool endIncluded = false;

//--------------TimeRange_ _6 - Verify CommonTime constructor does throw an error endpoint when times are inverted
			try {TimeRange fourInputConstructed(endEndpoint,startEndpoint,beginningIncluded, endIncluded); testFramework.failTest();}
			catch (Exception& e) {testFramework.passTest();}
			catch (...) {testFramework.failTest();}

//--------------TimeRange_ _7 - Verify CommonTime constructor does not throw errors when times are ok
			try {TimeRange fourInputConstructed(startEndpoint,endEndpoint,beginningIncluded, endIncluded); testFramework.passTest();}
			catch (...) {testFramework.failTest();}
			TimeRange fourInputConstructed(startEndpoint,endEndpoint,beginningIncluded, endIncluded);

//--------------TimeRange_ _8 - Verify CommonTime constructor sets the proper values (Implies that CommonTime == comparisons work)
			testFramework.assert( (fourInputConstructed.getStart() == startEndpoint) &&
				              (fourInputConstructed.getEnd() == endEndpoint) );
			testFramework.next();	

//--------------TimeRange_ _9 - Verify DTpair constructor does throw an error when endpoint times are inverted
			try {TimeRange threeInputConstructed(invertedPair,beginningIncluded, endIncluded); testFramework.failTest();}
			catch (Exception& e) {testFramework.passTest();}
			catch (...) {testFramework.failTest();}
		
//--------------TimeRange_ _10 - Verify DTpair constructor does not throw errors
			try {TimeRange threeInputConstructed(inputPair,beginningIncluded, endIncluded); testFramework.passTest();}
			catch (...) {testFramework.failTest();}
			TimeRange threeInputConstructed(inputPair,beginningIncluded, endIncluded);

//--------------TimeRange_ _11 - Verify DTpair constructor sets the proper values (Implies that CommonTime == comparisons work)
			testFramework.assert( (threeInputConstructed.getStart() == startEndpoint) &&
				              (threeInputConstructed.getEnd() == endEndpoint) );
			testFramework.next();	

			return testFramework.countFails();
		}

/* =========================================================================================================================
	Test for the inRange method
	If the target time occurs in the range, the method returns true.
	Additonal tests to ensure the endpoints respond properly depending on whether they are to be included in the range
========================================================================================================================= */
		int inRangeTest ( void )
		{
			TestUtil testFramework( "TimeRange", "inRange", __FILE__, __LINE__ );
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

//--------------TimeRange_ _2 - Verify inRange returns true for the starting endpoint when both ends are included
			testFramework.assert( bothEndsIncluded.inRange(startEndpoint) );
			testFramework.next();

//--------------TimeRange_ _3 - Verify inRange returns true for time inside the range when both ends are included
			testFramework.assert( bothEndsIncluded.inRange(timeInRange) );
			testFramework.next();

//--------------TimeRange_ _4 - Verify inRange returns true for the later endpoint when both ends are included
			testFramework.assert( bothEndsIncluded.inRange(endEndpoint) );
			testFramework.next();

//--------------TimeRange_ _5 - Verify inRange returns false time after then time range when both ends are included
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
/* =========================================================================================================================
	Test for the isPriorTo method
	Usage: referenceRange.isPriorTo(targetRange)
	If the reference range occurs completely before the target range, the method returns true
========================================================================================================================= */
		int isPriorToTest ( void )
		{
			TestUtil testFramework( "TimeRange", "isPriorTo", __FILE__, __LINE__ );
			testFramework.init();
			//Two time points before the reference TimeRange start endpoint
			CommonTime earlierThanRangeStart = gpstk::CivilTime(2010,12,20, 0, 0, 0.0 ).convertToCommonTime();	
			CommonTime earlierThanRangeEnd   = gpstk::CivilTime(2010,12,29, 0, 0, 0.0 ).convertToCommonTime();
			//Reference TimeRange start endpoint
			CommonTime startEndpoint         = gpstk::CivilTime(2011, 1, 1, 0, 0, 0.0 ).convertToCommonTime();
			//Two time points inside the reference TimeRange
			CommonTime timeInRangeStart      = gpstk::CivilTime(2011, 1,10, 0, 0, 0.0 ).convertToCommonTime();
			CommonTime timeInRangeEnd        = gpstk::CivilTime(2011, 1,20, 0, 0, 0.0 ).convertToCommonTime();
			//Reference TimeRange end endpoint
			CommonTime endEndpoint           = gpstk::CivilTime(2011, 1,31,23,59,59.59).convertToCommonTime();
			//Two time points after the reference TimeRange end endpoint
			CommonTime laterThanRangeStart   = gpstk::CivilTime(2011, 2,20, 0, 0, 0.0 ).convertToCommonTime();
			CommonTime laterThanRangeEnd     = gpstk::CivilTime(2011, 2,27, 0, 0, 0.0 ).convertToCommonTime();

			//Include endpoints for all checks
			bool beginningIncluded = true;
			bool endIncluded = true;

			//Create various TimeRanges
			TimeRange referenceTimeRange       (startEndpoint,         endEndpoint,         beginningIncluded, endIncluded);
			TimeRange priorTimeRange           (earlierThanRangeStart, earlierThanRangeEnd, beginningIncluded, endIncluded);
			TimeRange earlyOverlapTimeRange    (earlierThanRangeStart, timeInRangeEnd,      beginningIncluded, endIncluded);
			TimeRange interiorTimeRange        (timeInRangeStart,      timeInRangeEnd,      beginningIncluded, endIncluded);
			TimeRange lateOverlapTimeRange     (timeInRangeStart,      laterThanRangeEnd,   beginningIncluded, endIncluded);
			TimeRange completeOverlapTimeRange (earlierThanRangeStart, laterThanRangeEnd,   beginningIncluded, endIncluded);
			TimeRange afterTimeRange           (laterThanRangeStart,   laterThanRangeEnd,   beginningIncluded, endIncluded);
			TimeRange priorEdgeCase            (earlierThanRangeStart, startEndpoint,       beginningIncluded, endIncluded);
			TimeRange priorEdgeCaseNoOverlap   (earlierThanRangeStart, startEndpoint,       beginningIncluded, false      );
			TimeRange interiorEarlyEdge        (startEndpoint,         timeInRangeEnd,      beginningIncluded, endIncluded);
			TimeRange interiorLateEdge         (timeInRangeStart,      endEndpoint,         beginningIncluded, endIncluded);
			TimeRange afterEdgeCase            (endEndpoint,           laterThanRangeEnd,   beginningIncluded, endIncluded);
			TimeRange afterEdgeCaseNoOverlap   (endEndpoint,           laterThanRangeEnd,   beginningIncluded, false      );

//--------------TimeRange_ _1 - Verify isPriorTo returns false when the target range is completely before the reference range
			testFramework.assert( !referenceTimeRange.isPriorTo(priorTimeRange) );
			testFramework.next();

//--------------TimeRange_ _2 - Verify isPriorTo returns false when the target range overlaps the earlier portion of the reference range
			testFramework.assert( !referenceTimeRange.isPriorTo(earlyOverlapTimeRange) );
			testFramework.next();

//--------------TimeRange_ _3 - Verify isPriorTo returns false when the target range is interior to the reference range
			testFramework.assert( !referenceTimeRange.isPriorTo(interiorTimeRange) );
			testFramework.next();

//--------------TimeRange_ _4 - Verify isPriorTo returns false when the target range overlaps a later portion of the reference range
			testFramework.assert( !referenceTimeRange.isPriorTo(lateOverlapTimeRange) );
			testFramework.next();

//--------------TimeRange_ _5 - Verify isPriorTo returns false when the reference range is interior to the target range
			testFramework.assert( !referenceTimeRange.isPriorTo(completeOverlapTimeRange) );
			testFramework.next();

//--------------TimeRange_ _6 - Verify isPriorTo returns true when the target range is after the reference range
			testFramework.assert( referenceTimeRange.isPriorTo(afterTimeRange) );
			testFramework.next();

//--------------TimeRange_ _7 - Verify isPriorTo returns false when the target range ends at and includes the beginning of the reference range
			testFramework.assert( !referenceTimeRange.isPriorTo(priorEdgeCase) );
			testFramework.next();

//--------------TimeRange_ _8 - Verify isPriorTo returns false when the target range ends at but does not include the beginning of the reference range
			testFramework.assert( !referenceTimeRange.isPriorTo(priorEdgeCaseNoOverlap) );
			testFramework.next();

//--------------TimeRange_ _9 - Verify isPriorTo returns false when the target range is interior to the reference range and shares a start value
			testFramework.assert( !referenceTimeRange.isPriorTo(interiorEarlyEdge) );
			testFramework.next();

//--------------TimeRange_ _10 - Verify isPriorTo returns false when the target range is interior to the reference range and shares an end value
			testFramework.assert( !referenceTimeRange.isPriorTo(interiorLateEdge) );
			testFramework.next();

//--------------TimeRange_ _11 - Verify isPriorTo returns false when the target range starts at and includes the end of reference range
			testFramework.assert( !referenceTimeRange.isPriorTo(afterEdgeCase) );
			testFramework.next();

//--------------TimeRange_ _12 - Verify isPriorTo returns true when the target range starts at but does not include the end of reference range
			testFramework.assert( referenceTimeRange.isPriorTo(afterEdgeCaseNoOverlap) );
			testFramework.next();

//--------------TimeRange_ _13 - Verify isPriorTo returns false when the target range starts equals reference range
			testFramework.assert( !referenceTimeRange.isPriorTo(referenceTimeRange) );
			testFramework.next();
		

			return testFramework.countFails();
		}

/* =========================================================================================================================
	Test for the overlaps method
	Usage: referenceRange.overlaps(targetRange)
	If the target range and reference range intersect at all the method is to return true.
========================================================================================================================= */
		int overlapsTest ( void )
		{
			TestUtil testFramework( "TimeRange", "overlaps", __FILE__, __LINE__ );
			testFramework.init();
			//Two time points before the reference TimeRange start endpoint
			CommonTime earlierThanRangeStart = gpstk::CivilTime(2010,12,20, 0, 0, 0.0 ).convertToCommonTime();	
			CommonTime earlierThanRangeEnd   = gpstk::CivilTime(2010,12,29, 0, 0, 0.0 ).convertToCommonTime();
			//Reference TimeRange start endpoint
			CommonTime startEndpoint         = gpstk::CivilTime(2011, 1, 1, 0, 0, 0.0 ).convertToCommonTime();
			//Two time points inside the reference TimeRange
			CommonTime timeInRangeStart      = gpstk::CivilTime(2011, 1,10, 0, 0, 0.0 ).convertToCommonTime();
			CommonTime timeInRangeEnd        = gpstk::CivilTime(2011, 1,20, 0, 0, 0.0 ).convertToCommonTime();
			//Reference TimeRange end endpoint
			CommonTime endEndpoint           = gpstk::CivilTime(2011, 1,31,23,59,59.59).convertToCommonTime();
			//Two time points after the reference TimeRange end endpoint
			CommonTime laterThanRangeStart   = gpstk::CivilTime(2011, 2,20, 0, 0, 0.0 ).convertToCommonTime();
			CommonTime laterThanRangeEnd     = gpstk::CivilTime(2011, 2,27, 0, 0, 0.0 ).convertToCommonTime();

			//Include endpoints for all checks
			bool beginningIncluded = true;
			bool endIncluded = true;

			//Create various TimeRanges
			TimeRange referenceTimeRange       (startEndpoint,         endEndpoint,         beginningIncluded, endIncluded);
			TimeRange priorTimeRange           (earlierThanRangeStart, earlierThanRangeEnd, beginningIncluded, endIncluded);
			TimeRange earlyOverlapTimeRange    (earlierThanRangeStart, timeInRangeEnd,      beginningIncluded, endIncluded);
			TimeRange interiorTimeRange        (timeInRangeStart,      timeInRangeEnd,      beginningIncluded, endIncluded);
			TimeRange lateOverlapTimeRange     (timeInRangeStart,      laterThanRangeEnd,   beginningIncluded, endIncluded);
			TimeRange completeOverlapTimeRange (earlierThanRangeStart, laterThanRangeEnd,   beginningIncluded, endIncluded);
			TimeRange afterTimeRange           (laterThanRangeStart,   laterThanRangeEnd,   beginningIncluded, endIncluded);
			TimeRange priorEdgeCase            (earlierThanRangeStart, startEndpoint,       beginningIncluded, endIncluded);
			TimeRange priorEdgeCaseNoOverlap   (earlierThanRangeStart, startEndpoint,       beginningIncluded, false      );
			TimeRange interiorEarlyEdge        (startEndpoint,         timeInRangeEnd,      beginningIncluded, endIncluded);
			TimeRange interiorLateEdge         (timeInRangeStart,      endEndpoint,         beginningIncluded, endIncluded);
			TimeRange afterEdgeCase            (endEndpoint,           laterThanRangeEnd,   beginningIncluded, endIncluded);
			TimeRange afterEdgeCaseNoOverlap   (endEndpoint,           laterThanRangeEnd,   beginningIncluded, false      );

//--------------TimeRange_ _1 - Verify overlaps returns false when the target range is completely before the reference range
			testFramework.assert( !referenceTimeRange.overlaps(priorTimeRange) );
			testFramework.next();

//--------------TimeRange_ _2 - Verify overlaps returns true when the target range overlaps the earlier portion of the reference range
			testFramework.assert( referenceTimeRange.overlaps(earlyOverlapTimeRange) );
			testFramework.next();

//--------------TimeRange_ _3 - Verify overlaps returns true when the target range is interior to the reference range
			testFramework.assert( referenceTimeRange.overlaps(interiorTimeRange) );
			testFramework.next();

//--------------TimeRange_ _4 - Verify overlaps returns true when the target range overlaps a later portion of the reference range
			testFramework.assert( referenceTimeRange.overlaps(lateOverlapTimeRange) );
			testFramework.next();

//--------------TimeRange_ _5 - Verify overlaps returns true when the reference range is interior to the target range
			testFramework.assert( referenceTimeRange.overlaps(completeOverlapTimeRange) );
			testFramework.next();

//--------------TimeRange_ _6 - Verify overlaps returns false when the target range is after the reference range
			testFramework.assert( !referenceTimeRange.overlaps(afterTimeRange) );
			testFramework.next();

//--------------TimeRange_ _7 - Verify overlaps returns true when the target range ends at and includes the beginning of the reference range
			testFramework.assert( referenceTimeRange.overlaps(priorEdgeCase) );
			testFramework.next();

//--------------TimeRange_ _8 - Verify overlaps returns false when the target range ends at but does not include the beginning of the reference range
			testFramework.assert( !referenceTimeRange.overlaps(priorEdgeCaseNoOverlap) );
			testFramework.next();

//--------------TimeRange_ _9 - Verify overlaps returns true when the target range is interior to the reference range and shares a start value
			testFramework.assert( referenceTimeRange.overlaps(interiorEarlyEdge) );
			testFramework.next();

//--------------TimeRange_ _10 - Verify overlaps returns true when the target range is interior to the reference range and shares an end value
			testFramework.assert( referenceTimeRange.overlaps(interiorLateEdge) );
			testFramework.next();

//--------------TimeRange_ _11 - Verify overlaps returns true when the target range starts at and includes the end of reference range
			testFramework.assert( referenceTimeRange.overlaps(afterEdgeCase) );
			testFramework.next();

//--------------TimeRange_ _12 - Verify overlaps returns false when the target range starts at but does not include the end of reference range
			testFramework.assert( !referenceTimeRange.overlaps(afterEdgeCaseNoOverlap) );
			testFramework.next();

//--------------TimeRange_ _13 - Verify overlaps returns true when the target range starts equals reference range
			testFramework.assert( referenceTimeRange.overlaps(referenceTimeRange) );
			testFramework.next();
		

			return testFramework.countFails();
		}

/* =========================================================================================================================
	Test for the isSubsetOf method
	Usage: referenceRange.isSubsetOf(targetRange)
	If the reference range is entirely within the target range, the method is to return to true.
========================================================================================================================= */
		int isSubsetOfTest ( void )
		{
			TestUtil testFramework( "TimeRange", "isSubsetOf", __FILE__, __LINE__ );
			testFramework.init();
			//Two time points before the reference TimeRange start endpoint
			CommonTime earlierThanRangeStart = gpstk::CivilTime(2010,12,20, 0, 0, 0.0 ).convertToCommonTime();	
			CommonTime earlierThanRangeEnd   = gpstk::CivilTime(2010,12,29, 0, 0, 0.0 ).convertToCommonTime();
			//Reference TimeRange start endpoint
			CommonTime startEndpoint         = gpstk::CivilTime(2011, 1, 1, 0, 0, 0.0 ).convertToCommonTime();
			//Two time points inside the reference TimeRange
			CommonTime timeInRangeStart      = gpstk::CivilTime(2011, 1,10, 0, 0, 0.0 ).convertToCommonTime();
			CommonTime timeInRangeEnd        = gpstk::CivilTime(2011, 1,20, 0, 0, 0.0 ).convertToCommonTime();
			//Reference TimeRange end endpoint
			CommonTime endEndpoint           = gpstk::CivilTime(2011, 1,31,23,59,59.59).convertToCommonTime();
			//Two time points after the reference TimeRange end endpoint
			CommonTime laterThanRangeStart   = gpstk::CivilTime(2011, 2,20, 0, 0, 0.0 ).convertToCommonTime();
			CommonTime laterThanRangeEnd     = gpstk::CivilTime(2011, 2,27, 0, 0, 0.0 ).convertToCommonTime();

			//Include endpoints for all checks
			bool beginningIncluded = true;
			bool endIncluded = true;

			//Create various TimeRanges
			TimeRange referenceTimeRange       (startEndpoint,         endEndpoint,         beginningIncluded, endIncluded);
			TimeRange priorTimeRange           (earlierThanRangeStart, earlierThanRangeEnd, beginningIncluded, endIncluded);
			TimeRange earlyOverlapTimeRange    (earlierThanRangeStart, timeInRangeEnd,      beginningIncluded, endIncluded);
			TimeRange interiorTimeRange        (timeInRangeStart,      timeInRangeEnd,      beginningIncluded, endIncluded);
			TimeRange lateOverlapTimeRange     (timeInRangeStart,      laterThanRangeEnd,   beginningIncluded, endIncluded);
			TimeRange completeOverlapTimeRange (earlierThanRangeStart, laterThanRangeEnd,   beginningIncluded, endIncluded);
			TimeRange afterTimeRange           (laterThanRangeStart,   laterThanRangeEnd,   beginningIncluded, endIncluded);
			TimeRange priorEdgeCase            (earlierThanRangeStart, startEndpoint,       beginningIncluded, endIncluded);
			TimeRange priorEdgeCaseNoOverlap   (earlierThanRangeStart, startEndpoint,       beginningIncluded, false      );
			TimeRange interiorEarlyEdge        (startEndpoint,         timeInRangeEnd,      beginningIncluded, endIncluded);
			TimeRange interiorLateEdge         (timeInRangeStart,      endEndpoint,         beginningIncluded, endIncluded);
			TimeRange afterEdgeCase            (endEndpoint,           laterThanRangeEnd,   beginningIncluded, endIncluded);
			TimeRange afterEdgeCaseNoOverlap   (endEndpoint,           laterThanRangeEnd,   beginningIncluded, false      );

//--------------TimeRange_ _1 - Verify isSubsetOf returns false when the target range is completely before the reference range
			testFramework.assert( !referenceTimeRange.isSubsetOf(priorTimeRange) );
			testFramework.next();

//--------------TimeRange_ _2 - Verify isSubsetOf returns false when the target range overlaps the earlier portion of the reference range
			testFramework.assert( !referenceTimeRange.isSubsetOf(earlyOverlapTimeRange) );
			testFramework.next();

//--------------TimeRange_ _3 - Verify isSubsetOf returns false when the target range is interior to the reference range
			testFramework.assert( !referenceTimeRange.isSubsetOf(interiorTimeRange) );
			testFramework.next();

//--------------TimeRange_ _4 - Verify isSubsetOf returns false when the target range overlaps a later portion of the reference range
			testFramework.assert( !referenceTimeRange.isSubsetOf(lateOverlapTimeRange) );
			testFramework.next();

//--------------TimeRange_ _5 - Verify isSubsetOf returns true when the reference range is interior to the target range
			testFramework.assert( referenceTimeRange.isSubsetOf(completeOverlapTimeRange) );
			testFramework.next();

//--------------TimeRange_ _6 - Verify isSubsetOf returns false when the target range is after the reference range
			testFramework.assert( !referenceTimeRange.isSubsetOf(afterTimeRange) );
			testFramework.next();

//--------------TimeRange_ _7 - Verify isSubsetOf returns false when the target range ends at and includes the beginning of the reference range
			testFramework.assert( !referenceTimeRange.isSubsetOf(priorEdgeCase) );
			testFramework.next();

//--------------TimeRange_ _8 - Verify isSubsetOf returns false when the target range ends at but does not include the beginning of the reference range
			testFramework.assert( !referenceTimeRange.isSubsetOf(priorEdgeCaseNoOverlap) );
			testFramework.next();

//--------------TimeRange_ _9 - Verify isSubsetOf returns false when the target range is interior to the reference range and shares a start value
			testFramework.assert( !referenceTimeRange.isSubsetOf(interiorEarlyEdge) );
			testFramework.next();

//--------------TimeRange_ _10 - Verify isSubsetOf returns false when the target range is interior to the reference range and shares an end value
			testFramework.assert( !referenceTimeRange.isSubsetOf(interiorLateEdge) );
			testFramework.next();

//--------------TimeRange_ _11 - Verify isSubsetOf returns false when the target range starts at and includes the end of reference range
			testFramework.assert( !referenceTimeRange.isSubsetOf(afterEdgeCase) );
			testFramework.next();

//--------------TimeRange_ _12 - Verify isSubsetOf returns false when the target range starts at but does not include the end of reference range
			testFramework.assert( !referenceTimeRange.isSubsetOf(afterEdgeCaseNoOverlap) );
			testFramework.next();

//--------------TimeRange_ _13 - Verify isSubsetOf returns true when the target range starts equals reference range
			testFramework.assert( referenceTimeRange.isSubsetOf(referenceTimeRange) );
			testFramework.next();
		

			return testFramework.countFails();
		}


/* =========================================================================================================================
	Test for the isAfter method
	Usage: referenceRange.isAfter(targetRange)
	If the reference range is entirely after the target range, the method is to return to true.
========================================================================================================================= */
		int isAfterTest ( void )
		{
			TestUtil testFramework( "TimeRange", "isAfter", __FILE__, __LINE__ );
			testFramework.init();
			//Two time points before the reference TimeRange start endpoint
			CommonTime earlierThanRangeStart = gpstk::CivilTime(2010,12,20, 0, 0, 0.0 ).convertToCommonTime();	
			CommonTime earlierThanRangeEnd   = gpstk::CivilTime(2010,12,29, 0, 0, 0.0 ).convertToCommonTime();
			//Reference TimeRange start endpoint
			CommonTime startEndpoint         = gpstk::CivilTime(2011, 1, 1, 0, 0, 0.0 ).convertToCommonTime();
			//Two time points inside the reference TimeRange
			CommonTime timeInRangeStart      = gpstk::CivilTime(2011, 1,10, 0, 0, 0.0 ).convertToCommonTime();
			CommonTime timeInRangeEnd        = gpstk::CivilTime(2011, 1,20, 0, 0, 0.0 ).convertToCommonTime();
			//Reference TimeRange end endpoint
			CommonTime endEndpoint           = gpstk::CivilTime(2011, 1,31,23,59,59.59).convertToCommonTime();
			//Two time points after the reference TimeRange end endpoint
			CommonTime laterThanRangeStart   = gpstk::CivilTime(2011, 2,20, 0, 0, 0.0 ).convertToCommonTime();
			CommonTime laterThanRangeEnd     = gpstk::CivilTime(2011, 2,27, 0, 0, 0.0 ).convertToCommonTime();

			//Include endpoints for all checks
			bool beginningIncluded = true;
			bool endIncluded = true;

			//Create various TimeRanges
			TimeRange referenceTimeRange       (startEndpoint,         endEndpoint,         beginningIncluded, endIncluded);
			TimeRange priorTimeRange           (earlierThanRangeStart, earlierThanRangeEnd, beginningIncluded, endIncluded);
			TimeRange earlyOverlapTimeRange    (earlierThanRangeStart, timeInRangeEnd,      beginningIncluded, endIncluded);
			TimeRange interiorTimeRange        (timeInRangeStart,      timeInRangeEnd,      beginningIncluded, endIncluded);
			TimeRange lateOverlapTimeRange     (timeInRangeStart,      laterThanRangeEnd,   beginningIncluded, endIncluded);
			TimeRange completeOverlapTimeRange (earlierThanRangeStart, laterThanRangeEnd,   beginningIncluded, endIncluded);
			TimeRange afterTimeRange           (laterThanRangeStart,   laterThanRangeEnd,   beginningIncluded, endIncluded);
			TimeRange priorEdgeCase            (earlierThanRangeStart, startEndpoint,       beginningIncluded, endIncluded);
			TimeRange priorEdgeCaseNoOverlap   (earlierThanRangeStart, startEndpoint,       beginningIncluded, false      );
			TimeRange interiorEarlyEdge        (startEndpoint,         timeInRangeEnd,      beginningIncluded, endIncluded);
			TimeRange interiorLateEdge         (timeInRangeStart,      endEndpoint,         beginningIncluded, endIncluded);
			TimeRange afterEdgeCase            (endEndpoint,           laterThanRangeEnd,   beginningIncluded, endIncluded);
			TimeRange afterEdgeCaseNoOverlap   (endEndpoint,           laterThanRangeEnd,   beginningIncluded, false      );

//--------------TimeRange_ _1 - Verify isAfter returns true when the target range is completely before the reference range
			testFramework.assert( referenceTimeRange.isAfter(priorTimeRange) );
			testFramework.next();

//--------------TimeRange_ _2 - Verify isAfter returns false when the target range overlaps the earlier portion of the reference range
			testFramework.assert( !referenceTimeRange.isAfter(earlyOverlapTimeRange) );
			testFramework.next();

//--------------TimeRange_ _3 - Verify isAfter returns false when the target range is interior to the reference range
			testFramework.assert( !referenceTimeRange.isAfter(interiorTimeRange) );
			testFramework.next();

//--------------TimeRange_ _4 - Verify isAfter returns false when the target range overlaps a later portion of the reference range
			testFramework.assert( !referenceTimeRange.isAfter(lateOverlapTimeRange) );
			testFramework.next();

//--------------TimeRange_ _5 - Verify isAfter returns false when the reference range is interior to the target range
			testFramework.assert( !referenceTimeRange.isAfter(completeOverlapTimeRange) );
			testFramework.next();

//--------------TimeRange_ _6 - Verify isAfter returns false when the target range is after the reference range
			testFramework.assert( !referenceTimeRange.isAfter(afterTimeRange) );
			testFramework.next();

//--------------TimeRange_ _7 - Verify isAfter returns false when the target range ends at and includes the beginning of the reference range
			testFramework.assert( !referenceTimeRange.isAfter(priorEdgeCase) );
			testFramework.next();

//--------------TimeRange_ _8 - Verify isAfter returns true when the target range ends at but does not include the beginning of the reference range
			testFramework.assert( referenceTimeRange.isAfter(priorEdgeCaseNoOverlap) );
			testFramework.next();

//--------------TimeRange_ _9 - Verify isAfter returns false when the target range is interior to the reference range and shares a start value
			testFramework.assert( !referenceTimeRange.isAfter(interiorEarlyEdge) );
			testFramework.next();

//--------------TimeRange_ _10 - Verify isAfter returns false when the target range is interior to the reference range and shares an end value
			testFramework.assert( !referenceTimeRange.isAfter(interiorLateEdge) );
			testFramework.next();

//--------------TimeRange_ _11 - Verify isAfter returns false when the target range starts at and includes the end of reference range
			testFramework.assert( !referenceTimeRange.isAfter(afterEdgeCase) );
			testFramework.next();

//--------------TimeRange_ _12 - Verify isAfter returns false when the target range starts at but does not include the end of reference range
			testFramework.assert( !referenceTimeRange.isAfter(afterEdgeCaseNoOverlap) );
			testFramework.next();

//--------------TimeRange_ _13 - Verify isAfter returns false when the target range starts equals reference range
			testFramework.assert( !referenceTimeRange.isAfter(referenceTimeRange) );
			testFramework.next();
		
			return testFramework.countFails();
		}

/* =========================================================================================================================
	Test for the equalsOperator method
	Usage: referenceRange.isAfter(targetRange)
	If the reference range is entirely after the target range, the method is to return to true.
========================================================================================================================= */
		int equalsOperatorTest ( void )
		{
			TestUtil testFramework( "TimeRange", "equalsOperator", __FILE__, __LINE__ );
			testFramework.init();

			//Reference TimeRange endpoints
			CommonTime startPoint  = gpstk::CivilTime(2011, 1, 1, 0, 0, 0.0 ).convertToCommonTime();
			CommonTime endPoint    = gpstk::CivilTime(2011, 1,31,23,59,59.59).convertToCommonTime();
			CommonTime anotherTime = gpstk::CivilTime(2011, 1,10, 0, 0, 0.0 ).convertToCommonTime();

			//Create various TimeRanges
			TimeRange referenceTimeRange(startPoint,  endPoint,    true,  true );
			TimeRange differentStart    (anotherTime, endPoint,    true,  true );
			TimeRange differentEnd      (startPoint,  anotherTime, true,  true );
			TimeRange noInitialPoint    (startPoint,  endPoint,    false, true );
			TimeRange noFinalPoint      (startPoint,  endPoint,    true,  false);
			TimeRange copiedTimeRange   (referenceTimeRange);			

//--------------TimeRange_ _1 - Verify == returns false when the start time is different
			testFramework.assert( !(referenceTimeRange == differentStart) );
			testFramework.next();

//--------------TimeRange_ _2 - Verify == returns false when the end time is different
			testFramework.assert( !(referenceTimeRange == differentEnd) );
			testFramework.next();

//--------------TimeRange_ _3 - Verify == returns false when the start time inclusion boolean is different
			testFramework.assert( !(referenceTimeRange == noInitialPoint) );
			testFramework.next();

//--------------TimeRange_ _4 - Verify == returns false when the end time inclusion boolean is different
			testFramework.assert( !(referenceTimeRange == noFinalPoint) );
			testFramework.next();

//--------------TimeRange_ _5 - Verify == returns true when the time ranges are copies
			testFramework.assert( (referenceTimeRange == copiedTimeRange) );
			testFramework.next();

			return testFramework.countFails();
		}

/* =========================================================================================================================
	Test for the set method
	Method changes the internal values of the TimeRange object
	Test that the interior attributes have changed.
========================================================================================================================= */
		int setTest ( void )
		{
			TestUtil testFramework( "TimeRange", "set", __FILE__, __LINE__ );
			testFramework.init();

			//Reference TimeRange endpoints
			CommonTime startPoint  = gpstk::CivilTime(2011, 1, 1, 0, 0, 0.0 ).convertToCommonTime();
			CommonTime endPoint    = gpstk::CivilTime(2011, 1,31,23,59,59.59).convertToCommonTime();
			CommonTime anotherTime = gpstk::CivilTime(2011, 1,10, 0, 0, 0.0 ).convertToCommonTime();

			//Create various TimeRanges
			TimeRange referenceTimeRange(startPoint,  endPoint,    true,  true );
			TimeRange changedTimeRange;

//--------------TimeRange_ _1 - Verify set throws an exception if the time range provided is inverted
			try {changedTimeRange.set(anotherTime, startPoint, true, true); testFramework.failTest();}
			catch (Exception& e) {testFramework.passTest();}
			catch (...) {testFramework.failTest();}

//--------------TimeRange_ _2 - Verify set does not throw an exception when inputs are valid
			try {changedTimeRange.set(startPoint, endPoint, true, true); testFramework.passTest();}
			catch (...) {testFramework.failTest();}

			changedTimeRange.set(startPoint, endPoint, true, true);
//--------------TimeRange_ _3 - Verify set input all four values correctly.
			testFramework.assert( (changedTimeRange == referenceTimeRange) );
			testFramework.next();

			changedTimeRange.set(anotherTime, endPoint, true, true);
//--------------TimeRange_ _4 - Verify set input all four values correctly.
			testFramework.assert( !(changedTimeRange == referenceTimeRange) );
			testFramework.next();

			changedTimeRange.set(startPoint, anotherTime, true, true);
//--------------TimeRange_ _5 - Verify set input all four values correctly.
			testFramework.assert( !(changedTimeRange == referenceTimeRange) );
			testFramework.next();

			changedTimeRange.set(startPoint, endPoint, false, true);
//--------------TimeRange_ _6 - Verify set input all four values correctly.
			testFramework.assert( !(changedTimeRange == referenceTimeRange) );
			testFramework.next();

			changedTimeRange.set(startPoint, endPoint, true, false);
//--------------TimeRange_ _7 - Verify set input all four values correctly.
			testFramework.assert( !(changedTimeRange == referenceTimeRange) );
			testFramework.next();

			return testFramework.countFails();
		}



/* =========================================================================================================================
	Test for the printf method
========================================================================================================================= */
		int printfTest ( void )
		{
			TestUtil testFramework( "TimeRange", "printf", __FILE__, __LINE__ );
			testFramework.init();

			std::stringstream printfOutputStream;

			//Reference TimeRange endpoints
			CommonTime startPoint  = gpstk::CivilTime(2011, 1, 1, 0, 0, 0.0 ).convertToCommonTime();
			CommonTime endPoint    = gpstk::CivilTime(2011, 1,31,23,59,59.59).convertToCommonTime();

			//Create various TimeRanges
			TimeRange includeBoth (startPoint, endPoint, true,  true  );
			TimeRange includeStart(startPoint, endPoint, true,  false );
			TimeRange includeEnd  (startPoint, endPoint, false, true  );
			TimeRange excludeBoth (startPoint, endPoint, false, false );

			std::string testFmts[]=
			{
				"%Y %m %d %H %M %S",
				"%02m/%02d/%02y %02H:%02M:%02S",
				"%02b %02d, %04Y %02H:%02M:%02S",
				"%Y %j %s"
			};


			std::string correctResults[]=
			{
				"[2011 1 1 0 0 0, 2011 1 31 23 59 59]",
				"[2011 1 1 0 0 0, 2011 1 31 23 59 59)",
				"(2011 1 1 0 0 0, 2011 1 31 23 59 59]",
				"(2011 1 1 0 0 0, 2011 1 31 23 59 59)",
				"[01/01/11 00:00:00, 01/31/11 23:59:59]",
				"[Jan 01, 2011 00:00:00, Jan 31, 2011 23:59:59]",
				"[2011 1 0.000000, 2011 31 86399.590000]"
			};


			printfOutputStream <<  includeBoth.printf(testFmts[0]);
//--------------TimeRange_ _1 - Verify printf prints correctly
			testFramework.assert( (printfOutputStream.str() == correctResults[0]) );
			testFramework.next();
			printfOutputStream.str(std::string());

			printfOutputStream << includeStart.printf(testFmts[0]);
//--------------TimeRange_ _2 - Verify printf prints correctly
			testFramework.assert( (printfOutputStream.str() == correctResults[1]) );
			testFramework.next();
			printfOutputStream.str(std::string());

			printfOutputStream << includeEnd.printf(testFmts[0]);
//--------------TimeRange_ _3 - Verify printf prints correctly
			testFramework.assert( (printfOutputStream.str() == correctResults[2]) );
			testFramework.next();
			printfOutputStream.str(std::string());

			printfOutputStream << excludeBoth.printf(testFmts[0]);
//--------------TimeRange_ _4 - Verify printf prints correctly
			testFramework.assert( (printfOutputStream.str() == correctResults[3]) );
			testFramework.next();
			printfOutputStream.str(std::string());

			printfOutputStream << includeBoth.printf(testFmts[1]);
//--------------TimeRange_ _5 - Verify printf prints correctly
			testFramework.assert( (printfOutputStream.str() == correctResults[4]) );
			testFramework.next();
			printfOutputStream.str(std::string());

			printfOutputStream << includeBoth.printf(testFmts[2]);
//--------------TimeRange_ _6 - Verify printf prints correctly
			testFramework.assert( (printfOutputStream.str() == correctResults[5]) );
			testFramework.next();
			printfOutputStream.str(std::string());

			printfOutputStream << includeBoth.printf(testFmts[3]);
//--------------TimeRange_ _7 - Verify printf prints correctly
			testFramework.assert( (printfOutputStream.str() == correctResults[6]) );
			testFramework.next();



			return testFramework.countFails();
		}

/* =========================================================================================================================
	Test for the dump method
========================================================================================================================= */
		int dumpTest ( void )
		{
			TestUtil testFramework( "TimeRange", "dump", __FILE__, __LINE__ );
			testFramework.init();

			std::stringstream printfOutputStream;

			//Reference TimeRange endpoints
			CommonTime startPoint  = gpstk::CivilTime(2011, 1, 1, 0, 0, 0.0 ).convertToCommonTime();
			CommonTime endPoint    = gpstk::CivilTime(2011, 1,31,23,59,59.59).convertToCommonTime();

			//Create various TimeRanges
			TimeRange includeBoth (startPoint, endPoint, true,  true  );
			TimeRange includeStart(startPoint, endPoint, true,  false );
			TimeRange includeEnd  (startPoint, endPoint, false, true  );
			TimeRange excludeBoth (startPoint, endPoint, false, false );

			std::string testFmts[]=
			{
				"%Y %m %d %H %M %S",
				"%02m/%02d/%02y %02H:%02M:%02S",
				"%02b %02d, %04Y %02H:%02M:%02S",
				"%Y %j %s"
			};


			std::string correctResults[]=
			{
				"[Start:2011 1 1 0 0 0, End: 2011 1 31 23 59 59]",
				"[Start:2011 1 1 0 0 0, End: 2011 1 31 23 59 59)",
				"(Start:2011 1 1 0 0 0, End: 2011 1 31 23 59 59]",
				"(Start:2011 1 1 0 0 0, End: 2011 1 31 23 59 59)",
				"[Start:01/01/11 00:00:00, End: 01/31/11 23:59:59]",
				"[Start:Jan 01, 2011 00:00:00, End: Jan 31, 2011 23:59:59]",
				"[Start:2011 1 0.000000, End: 2011 31 86399.590000]"
			};


			printfOutputStream <<  includeBoth.dump(testFmts[0]);
//--------------TimeRange_ _1 - Verify dump prints correctly
			testFramework.assert( (printfOutputStream.str() == correctResults[0]) );
			testFramework.next();
			printfOutputStream.str(std::string());

			printfOutputStream << includeStart.dump(testFmts[0]);
//--------------TimeRange_ _2 - Verify dump prints correctly
			testFramework.assert( (printfOutputStream.str() == correctResults[1]) );
			testFramework.next();
			printfOutputStream.str(std::string());

			printfOutputStream << includeEnd.dump(testFmts[0]);
//--------------TimeRange_ _3 - Verify dump prints correctly
			testFramework.assert( (printfOutputStream.str() == correctResults[2]) );
			testFramework.next();
			printfOutputStream.str(std::string());

			printfOutputStream << excludeBoth.dump(testFmts[0]);
//--------------TimeRange_ _4 - Verify dump prints correctly
			testFramework.assert( (printfOutputStream.str() == correctResults[3]) );
			testFramework.next();
			printfOutputStream.str(std::string());

			printfOutputStream << includeBoth.dump(testFmts[1]);
//--------------TimeRange_ _5 - Verify dump prints correctly
			testFramework.assert( (printfOutputStream.str() == correctResults[4]) );
			testFramework.next();
			printfOutputStream.str(std::string());

			printfOutputStream << includeBoth.dump(testFmts[2]);
//--------------TimeRange_ _6 - Verify dump prints correctly
			testFramework.assert( (printfOutputStream.str() == correctResults[5]) );
			testFramework.next();
			printfOutputStream.str(std::string());

			printfOutputStream << includeBoth.dump(testFmts[3]);
//--------------TimeRange_ _7 - Verify dump prints correctly
			testFramework.assert( (printfOutputStream.str() == correctResults[6]) );
			testFramework.next();



			return testFramework.countFails();
		}
	private:
		double eps;
};


int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	TimeRange_T testClass;

	check = testClass.constructorTest();
	errorCounter += check;

	check = testClass.inRangeTest();
	errorCounter += check;

	check = testClass.isPriorToTest();
	errorCounter += check;

	check = testClass.overlapsTest();
	errorCounter += check;

	check = testClass.isSubsetOfTest();
	errorCounter += check;

	check = testClass.isAfterTest();
	errorCounter += check;

	check = testClass.equalsOperatorTest();
	errorCounter += check;

	check = testClass.setTest();
	errorCounter += check;

	check = testClass.printfTest();
	errorCounter += check;

	check = testClass.dumpTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}

