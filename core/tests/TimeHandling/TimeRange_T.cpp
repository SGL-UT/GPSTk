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
   TimeRange_T()
   {
      eps = 1E-12;
   }
   ~TimeRange_T() {}

      //=======================================================================
      // Test for the TimeRange constructors
      //=======================================================================
   int constructorTest ( void )
   {
      TUDEF( "TimeRange", "Constructor" );
      std::string outputFormat = CivilTime().getDefaultFormat();


         //--------------------------------------------------------------------
         // Verify default constructor does not throw errors and
         // creates the expected object
         //--------------------------------------------------------------------
      try
      {
         TimeRange emptyConstructed;
         testFramework.assert(true, "Construction of empty TimeRange object worked",
                              __LINE__);
      }
      catch (...)
      {
         testFramework.assert(false, "Construction of empty TimeRange object failed",
                              __LINE__);
      }

      TimeRange emptyConstructed;
         // Verify default constructor sets the proper values (This is
         // important to verify the values in the copy constructor
         // below.)
      testFramework.assert(emptyConstructed.getStart() ==
                           gpstk::CommonTime::BEGINNING_OF_TIME,
                           "Start value for empty TimeRange is not the expected value", __LINE__);
      testFramework.assert(emptyConstructed.getEnd()   ==
                           gpstk::CommonTime::END_OF_TIME      ,
                           "End value for empty TimeRange is not the expected value"  , __LINE__);


         //--------------------------------------------------------------------
         // Verify copy constructor does not throw errors and creates
         // the expected object
         //--------------------------------------------------------------------
      try
      {
         TimeRange copyConstructed(emptyConstructed);
         testFramework.assert(true,
                              "Construction using TimeRange copy constructor worked", __LINE__);
      }
      catch (...)
      {
         testFramework.assert(false,
                              "Construction using TimeRange copy constructor failed", __LINE__);
      }

      TimeRange copyConstructed(emptyConstructed);
      testFramework.assert(emptyConstructed.getStart() ==
                           gpstk::CommonTime::BEGINNING_OF_TIME,
                           "Start value for copy constructed TimeRange is not the expected value",
                           __LINE__);
      testFramework.assert(emptyConstructed.getEnd()   ==
                           gpstk::CommonTime::END_OF_TIME      ,
                           "End value for copy constructed TimeRange is not the expected value"  ,
                           __LINE__);





         //--------------------------------------------------------------------
         // Verify CommonTime constructor does throws expected errors
         // and creates the expected object
         //--------------------------------------------------------------------
      CommonTime startEndpoint = gpstk::CivilTime(2011,1, 1, 0, 0,
                                 0.0).convertToCommonTime();
      CommonTime endEndpoint = gpstk::CivilTime(2011,1,31,23,59,
                               59.59).convertToCommonTime();
      bool beginningIncluded = true;
      bool endIncluded = false;

         // Verify CommonTime constructor does throw an error endpoint
         // when times are inverted
      try
      {
         TimeRange fourInputConstructed(endEndpoint,startEndpoint,beginningIncluded,
                                        endIncluded);
         testFramework.assert(false,
                              "CommonTime constructor allowed for the end time to be earlier than the start time",
                              __LINE__);
      }
      catch (Exception& e)
      {
         testFramework.assert(true, "CommonTime constructor threw the expected error",
                              __LINE__);
      }
      catch (...)
      {
         testFramework.assert(false,
                              "CommonTime constructor threw an unexpected error for when the end time is earlier than the start time",
                              __LINE__);
      }

         // Verify CommonTime constructor does not throw errors when
         // times are ok
      try
      {
         TimeRange fourInputConstructed(startEndpoint,endEndpoint,beginningIncluded,
                                        endIncluded);
         testFramework.assert(true,
                              "CommonTime constructor for valid data functioned properly", __LINE__);
      }
      catch (...)
      {
         testFramework.assert(false,
                              "CommonTime constructor for valid data functioned threw an error", __LINE__);
      }

         // Verify CommonTime constructor sets the proper values
         // (Implies that CommonTime == comparisons work)
      TimeRange fourInputConstructed(startEndpoint,endEndpoint,beginningIncluded,
                                     endIncluded);
      testFramework.assert(fourInputConstructed.getStart() == startEndpoint,
                           "CommonTime constructor did not set the start time properly", __LINE__);
      testFramework.assert(fourInputConstructed.getEnd() == endEndpoint    ,
                           "CommonTime constructor did not set the end time properly"  , __LINE__);


         //--------------------------------------------------------------------
         // Verify CommonTime constructor does throws expected errors
         // and creates the expected object
         //--------------------------------------------------------------------
      gpstk::TimeRange::DTPair inputPair, invertedPair;
      inputPair = std::make_pair(startEndpoint,endEndpoint);
      invertedPair = std::make_pair(endEndpoint,startEndpoint);

         // Verify DTpair constructor does throw an error when
         // endpoint times are inverted
      try
      {
         TimeRange threeInputConstructed(invertedPair,beginningIncluded, endIncluded);
         testFramework.assert(false,
                              "DTPair constructor allowed for the end time to be earlier than the start time",
                              __LINE__);
      }
      catch (Exception& e)
      {
         testFramework.assert(true, "DTPair constructor threw the expected error",
                              __LINE__);
      }
      catch (...)
      {
         testFramework.assert(false,
                              "DTPair constructor threw an unexpected error for when the end time is earlier than the start time",
                              __LINE__);
      }

         // Verify DTpair constructor does not throw errors with
         // proper inputs
      try
      {
         TimeRange threeInputConstructed(inputPair,beginningIncluded, endIncluded);
         testFramework.assert(true,
                              "DTPair constructor for valid data functioned properly", __LINE__);
      }
      catch (...)
      {
         testFramework.assert(false,
                              "DTPair constructor for valid data functioned threw an error", __LINE__);
      }

         // Verify DTpair constructor sets the proper values (Implies
         // that CommonTime == comparisons work)
      TimeRange threeInputConstructed(inputPair,beginningIncluded, endIncluded);
      testFramework.assert(threeInputConstructed.getStart() == startEndpoint,
                           "CommonTime constructor did not set the start time properly", __LINE__);
      testFramework.assert(threeInputConstructed.getEnd() == endEndpoint    ,
                           "CommonTime constructor did not set the end time properly"  , __LINE__);


      return testFramework.countFails();
   }

//=============================================================================
// Test for the inRange method
// If the target time occurs in the range, the method returns true.
// Additonal tests to ensure the endpoints respond properly depending
// on whether they are to be included in the range
//=============================================================================
   int inRangeTest ( void )
   {
      TUDEF( "TimeRange", "inRange" );


      CommonTime earlierThanRange = gpstk::CivilTime(2010,12,20, 0, 0,
                                    0.0 ).convertToCommonTime();
      CommonTime startEndpoint    = gpstk::CivilTime(2011, 1, 1, 0, 0,
                                    0.0 ).convertToCommonTime();
      CommonTime timeInRange      = gpstk::CivilTime(2011, 1,20, 0, 0,
                                    0.0 ).convertToCommonTime();
      CommonTime endEndpoint      = gpstk::CivilTime(2011, 1,31,23,59,
                                    59.59).convertToCommonTime();
      CommonTime laterThanRange   = gpstk::CivilTime(2011, 2,20, 0, 0,
                                    0.0 ).convertToCommonTime();

         //Create a TimeRange where both ends are included
      bool beginningIncluded = true;
      bool endIncluded = true;
      TimeRange bothEndsIncluded(startEndpoint, endEndpoint, beginningIncluded,
                                 endIncluded);

         //Create a TimeRange where both ends are excluded
      beginningIncluded = false;
      endIncluded = false;
      TimeRange bothEndsExcluded(startEndpoint, endEndpoint, beginningIncluded,
                                 endIncluded);

         //--------------------------------------------------------------------
         //Verify inRange for a TimeRange with both ends included
         //--------------------------------------------------------------------
      testFramework.assert(!bothEndsIncluded.inRange(earlierThanRange),
                           "inRange returned true for time before the TimeRange"                 ,
                           __LINE__);
      testFramework.assert( bothEndsIncluded.inRange(startEndpoint)   ,
                            "inRange returned false for the start time for an inclusive TimeRange",
                            __LINE__);
      testFramework.assert( bothEndsIncluded.inRange(timeInRange)     ,
                            "inRange returned false for time internal to the TimeRange"           ,
                            __LINE__);
      testFramework.assert( bothEndsIncluded.inRange(endEndpoint)     ,
                            "inRange returned false for the end time for an inclusive TimeRange"  ,
                            __LINE__);
      testFramework.assert(!bothEndsIncluded.inRange(laterThanRange)  ,
                           "inRange returned true for time after the TimeRange"                  ,
                           __LINE__);


         //--------------------------------------------------------------------
         // Verify inRange for a TimeRange with both ends included
         //--------------------------------------------------------------------
      testFramework.assert(!bothEndsExcluded.inRange(earlierThanRange),
                           "inRange returned true for time before the TimeRange"                 ,
                           __LINE__);
      testFramework.assert(!bothEndsExcluded.inRange(startEndpoint)   ,
                           "inRange returned true for the start time for an exclusive TimeRange" ,
                           __LINE__);
      testFramework.assert( bothEndsExcluded.inRange(timeInRange)     ,
                            "inRange returned false for time internal to the TimeRange"           ,
                            __LINE__);
      testFramework.assert(!bothEndsExcluded.inRange(endEndpoint)     ,
                           "inRange returned true for the end time for an exclusive TimeRange"   ,
                           __LINE__);
      testFramework.assert(!bothEndsIncluded.inRange(laterThanRange)  ,
                           "inRange returned true for time after the TimeRange"                  ,
                           __LINE__);

      return testFramework.countFails();
   }
//=============================================================================
// Test for the isPriorTo method
// Usage: referenceRange.isPriorTo(targetRange)
// If the reference range occurs completely before the target range, the method returns true
//=============================================================================
   int isPriorToTest ( void )
   {
      TUDEF( "TimeRange", "isPriorTo" );

         //Two time points before the reference TimeRange start endpoint
      CommonTime earlierThanRangeStart = gpstk::CivilTime(2010,12,20, 0, 0,
                                         0.0 ).convertToCommonTime();
      CommonTime earlierThanRangeEnd   = gpstk::CivilTime(2010,12,29, 0, 0,
                                         0.0 ).convertToCommonTime();
         //Reference TimeRange start endpoint
      CommonTime startEndpoint         = gpstk::CivilTime(2011, 1, 1, 0, 0,
                                         0.0 ).convertToCommonTime();
         //Two time points inside the reference TimeRange
      CommonTime timeInRangeStart      = gpstk::CivilTime(2011, 1,10, 0, 0,
                                         0.0 ).convertToCommonTime();
      CommonTime timeInRangeEnd        = gpstk::CivilTime(2011, 1,20, 0, 0,
                                         0.0 ).convertToCommonTime();
         //Reference TimeRange end endpoint
      CommonTime endEndpoint           = gpstk::CivilTime(2011, 1,31,23,59,
                                         59.59).convertToCommonTime();
         //Two time points after the reference TimeRange end endpoint
      CommonTime laterThanRangeStart   = gpstk::CivilTime(2011, 2,20, 0, 0,
                                         0.0 ).convertToCommonTime();
      CommonTime laterThanRangeEnd     = gpstk::CivilTime(2011, 2,27, 0, 0,
                                         0.0 ).convertToCommonTime();

         //Include endpoints for all checks
      bool beginningIncluded = true;
      bool endIncluded = true;

         //Create various TimeRanges
      TimeRange referenceTimeRange       (startEndpoint,         endEndpoint,
                                          beginningIncluded, endIncluded);
      TimeRange priorTimeRange           (earlierThanRangeStart, earlierThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange earlyOverlapTimeRange    (earlierThanRangeStart, timeInRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange interiorTimeRange        (timeInRangeStart,      timeInRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange lateOverlapTimeRange     (timeInRangeStart,      laterThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange completeOverlapTimeRange (earlierThanRangeStart, laterThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange afterTimeRange           (laterThanRangeStart,   laterThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange priorEdgeCase            (earlierThanRangeStart, startEndpoint,
                                          beginningIncluded, endIncluded);
      TimeRange priorEdgeCaseNoOverlap   (earlierThanRangeStart, startEndpoint,
                                          beginningIncluded, false      );
      TimeRange interiorEarlyEdge        (startEndpoint,         timeInRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange interiorLateEdge         (timeInRangeStart,      endEndpoint,
                                          beginningIncluded, endIncluded);
      TimeRange afterEdgeCase            (endEndpoint,           laterThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange afterEdgeCaseNoOverlap   (endEndpoint,           laterThanRangeEnd,
                                          false, endIncluded);

      std::string testMessageArray[13];
      testMessageArray[0]  =
         "isPriorTo returned true when the target TimeRange comes before the reference TimeRange";
      testMessageArray[1]  =
         "isPriorTo returned true when the target TimeRange overlaps the beginning of the reference TimeRange";
      testMessageArray[2]  =
         "isPriorTo returned true when the target TimeRange is interior to the reference TimeRange";
      testMessageArray[3]  =
         "isPriorTo returned true when the target TimeRange overlaps a later portion of the reference TimeRange";
      testMessageArray[4]  =
         "isPriorTo returned true when the reference TimeRange is interior to the target TimeRange";
      testMessageArray[5]  =
         "isPriorTo returned false when the target TimeRange is after the reference TimeRange";
      testMessageArray[6]  =
         "isPriorTo returned true when the target TimeRange ends at and includes the beginning of the reference TimeRange";
      testMessageArray[7]  =
         "isPriorTo returned true when the target TimeRange ends at but does not include the beginning of the reference TimeRange";
      testMessageArray[8]  =
         "isPriorTo returned true when the target TimeRange is interior to the reference TimeRange and shares a start value";
      testMessageArray[9]  =
         "isPriorTo returned true when the target TimeRange is interior to the reference TimeRange and shares an end value";
      testMessageArray[10] =
         "isPriorTo returned true when the target TimeRange starts at and includes the end of reference TimeRange";
      testMessageArray[11] =
         "isPriorTo returned false when the target TimeRange starts at but does not include the end of reference TimeRange";
      testMessageArray[12] =
         "isPriorTo returned true when the target TimeRange starts equals reference TimeRange";

      return testFramework.countFails();
   }

//=============================================================================
// Test for the overlaps method
// Usage: referenceRange.overlaps(targetRange)
// If the target range and reference range intersect at all the method is to return true.
//=============================================================================
   int overlapsTest ( void )
   {
      TUDEF( "TimeRange", "overlaps" );

         //Two time points before the reference TimeRange start endpoint
      CommonTime earlierThanRangeStart = gpstk::CivilTime(2010,12,20, 0, 0,
                                         0.0 ).convertToCommonTime();
      CommonTime earlierThanRangeEnd   = gpstk::CivilTime(2010,12,29, 0, 0,
                                         0.0 ).convertToCommonTime();
         //Reference TimeRange start endpoint
      CommonTime startEndpoint         = gpstk::CivilTime(2011, 1, 1, 0, 0,
                                         0.0 ).convertToCommonTime();
         //Two time points inside the reference TimeRange
      CommonTime timeInRangeStart      = gpstk::CivilTime(2011, 1,10, 0, 0,
                                         0.0 ).convertToCommonTime();
      CommonTime timeInRangeEnd        = gpstk::CivilTime(2011, 1,20, 0, 0,
                                         0.0 ).convertToCommonTime();
         //Reference TimeRange end endpoint
      CommonTime endEndpoint           = gpstk::CivilTime(2011, 1,31,23,59,
                                         59.59).convertToCommonTime();
         //Two time points after the reference TimeRange end endpoint
      CommonTime laterThanRangeStart   = gpstk::CivilTime(2011, 2,20, 0, 0,
                                         0.0 ).convertToCommonTime();
      CommonTime laterThanRangeEnd     = gpstk::CivilTime(2011, 2,27, 0, 0,
                                         0.0 ).convertToCommonTime();

         //Include endpoints for all checks
      bool beginningIncluded = true;
      bool endIncluded = true;

         //Create various TimeRanges
      TimeRange referenceTimeRange       (startEndpoint,         endEndpoint,
                                          beginningIncluded, endIncluded);
      TimeRange priorTimeRange           (earlierThanRangeStart, earlierThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange earlyOverlapTimeRange    (earlierThanRangeStart, timeInRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange interiorTimeRange        (timeInRangeStart,      timeInRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange lateOverlapTimeRange     (timeInRangeStart,      laterThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange completeOverlapTimeRange (earlierThanRangeStart, laterThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange afterTimeRange           (laterThanRangeStart,   laterThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange priorEdgeCase            (earlierThanRangeStart, startEndpoint,
                                          beginningIncluded, endIncluded);
      TimeRange priorEdgeCaseNoOverlap   (earlierThanRangeStart, startEndpoint,
                                          beginningIncluded, false      );
      TimeRange interiorEarlyEdge        (startEndpoint,         timeInRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange interiorLateEdge         (timeInRangeStart,      endEndpoint,
                                          beginningIncluded, endIncluded);
      TimeRange afterEdgeCase            (endEndpoint,           laterThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange afterEdgeCaseNoOverlap   (endEndpoint,           laterThanRangeEnd,
                                          false, endIncluded);

      std::string testMessageArray[13];
      testMessageArray[0]  =
         "overlaps returned true when the target TimeRange is completely before the reference TimeRange";
      testMessageArray[1]  =
         "overlaps returned false when the target TimeRange overlaps the earlier portion of the reference TimeRange";
      testMessageArray[2]  =
         "overlaps returned false when the target TimeRange is interior to the reference TimeRange";
      testMessageArray[3]  =
         "overlaps returned false when the target TimeRange overlaps a later portion of the reference TimeRange";
      testMessageArray[4]  =
         "overlaps returned false when the reference TimeRange is interior to the target TimeRange";
      testMessageArray[5]  =
         "overlaps returned true when the target TimeRange is after the reference TimeRange";
      testMessageArray[6]  =
         "overlaps returned false when the target TimeRange ends at and includes the beginning of the reference TimeRange";
      testMessageArray[7]  =
         "overlaps returned true when the target TimeRange ends at but does not include the beginning of the reference TimeRange";
      testMessageArray[8]  =
         "overlaps returned false when the target TimeRange is interior to the reference TimeRange and shares a start value";
      testMessageArray[9]  =
         "overlaps returned false when the target TimeRange is interior to the reference TimeRange and shares an end value";
      testMessageArray[10] =
         "overlaps returned false when the target TimeRange starts at and includes the end of reference TimeRange";
      testMessageArray[11] =
         "overlaps returned true when the target TimeRange starts at but does not include the end of reference TimeRange";
      testMessageArray[12] =
         "overlaps returned false when the target TimeRange starts equals reference TimeRange";


      testFramework.assert(!referenceTimeRange.overlaps(priorTimeRange)          ,
                           testMessageArray[0] , __LINE__);
      testFramework.assert( referenceTimeRange.overlaps(earlyOverlapTimeRange)   ,
                            testMessageArray[1] , __LINE__);
      testFramework.assert( referenceTimeRange.overlaps(interiorTimeRange)       ,
                            testMessageArray[2] , __LINE__);
      testFramework.assert( referenceTimeRange.overlaps(lateOverlapTimeRange)    ,
                            testMessageArray[3] , __LINE__);
      testFramework.assert( referenceTimeRange.overlaps(completeOverlapTimeRange),
                            testMessageArray[4] , __LINE__);
      testFramework.assert(!referenceTimeRange.overlaps(afterTimeRange)          ,
                           testMessageArray[5] , __LINE__);
      testFramework.assert( referenceTimeRange.overlaps(priorEdgeCase)           ,
                            testMessageArray[6] , __LINE__);
      testFramework.assert(!referenceTimeRange.overlaps(priorEdgeCaseNoOverlap)  ,
                           testMessageArray[7] , __LINE__);
      testFramework.assert( referenceTimeRange.overlaps(interiorEarlyEdge)       ,
                            testMessageArray[8] , __LINE__);
      testFramework.assert( referenceTimeRange.overlaps(interiorLateEdge)        ,
                            testMessageArray[9] , __LINE__);
      testFramework.assert( referenceTimeRange.overlaps(afterEdgeCase)           ,
                            testMessageArray[10], __LINE__);
      testFramework.assert(!referenceTimeRange.overlaps(afterEdgeCaseNoOverlap)  ,
                           testMessageArray[11], __LINE__);
      testFramework.assert( referenceTimeRange.overlaps(referenceTimeRange)      ,
                            testMessageArray[12], __LINE__);



      return testFramework.countFails();
   }

//=============================================================================
// Test for the isSubsetOf method
// Usage: referenceRange.isSubsetOf(targetRange)
// If the reference range is entirely within the target range, the method is to return to true.
//=============================================================================
   int isSubsetOfTest ( void )
   {
      TUDEF( "TimeRange", "isSubsetOf" );

         //Two time points before the reference TimeRange start endpoint
      CommonTime earlierThanRangeStart = gpstk::CivilTime(2010,12,20, 0, 0,
                                         0.0 ).convertToCommonTime();
      CommonTime earlierThanRangeEnd   = gpstk::CivilTime(2010,12,29, 0, 0,
                                         0.0 ).convertToCommonTime();
         //Reference TimeRange start endpoint
      CommonTime startEndpoint         = gpstk::CivilTime(2011, 1, 1, 0, 0,
                                         0.0 ).convertToCommonTime();
         //Two time points inside the reference TimeRange
      CommonTime timeInRangeStart      = gpstk::CivilTime(2011, 1,10, 0, 0,
                                         0.0 ).convertToCommonTime();
      CommonTime timeInRangeEnd        = gpstk::CivilTime(2011, 1,20, 0, 0,
                                         0.0 ).convertToCommonTime();
         //Reference TimeRange end endpoint
      CommonTime endEndpoint           = gpstk::CivilTime(2011, 1,31,23,59,
                                         59.59).convertToCommonTime();
         //Two time points after the reference TimeRange end endpoint
      CommonTime laterThanRangeStart   = gpstk::CivilTime(2011, 2,20, 0, 0,
                                         0.0 ).convertToCommonTime();
      CommonTime laterThanRangeEnd     = gpstk::CivilTime(2011, 2,27, 0, 0,
                                         0.0 ).convertToCommonTime();

         //Include endpoints for all checks
      bool beginningIncluded = true;
      bool endIncluded = true;

         //Create various TimeRanges
      TimeRange referenceTimeRange       (startEndpoint,         endEndpoint,
                                          beginningIncluded, endIncluded);
      TimeRange priorTimeRange           (earlierThanRangeStart, earlierThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange earlyOverlapTimeRange    (earlierThanRangeStart, timeInRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange interiorTimeRange        (timeInRangeStart,      timeInRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange lateOverlapTimeRange     (timeInRangeStart,      laterThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange completeOverlapTimeRange (earlierThanRangeStart, laterThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange afterTimeRange           (laterThanRangeStart,   laterThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange priorEdgeCase            (earlierThanRangeStart, startEndpoint,
                                          beginningIncluded, endIncluded);
      TimeRange priorEdgeCaseNoOverlap   (earlierThanRangeStart, startEndpoint,
                                          beginningIncluded, false      );
      TimeRange interiorEarlyEdge        (startEndpoint,         timeInRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange interiorLateEdge         (timeInRangeStart,      endEndpoint,
                                          beginningIncluded, endIncluded);
      TimeRange afterEdgeCase            (endEndpoint,           laterThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange afterEdgeCaseNoOverlap   (endEndpoint,           laterThanRangeEnd,
                                          beginningIncluded, false      );

      std::string testMessageArray[13];
      testMessageArray[0]  =
         "isSubsetOf returned true when the target TimeRange is completely before the reference TimeRange";
      testMessageArray[1]  =
         "isSubsetOf returned true when the target TimeRange overlaps the earlier portion of the reference TimeRange";
      testMessageArray[2]  =
         "isSubsetOf returned true when the target TimeRange is interior to the reference TimeRange";
      testMessageArray[3]  =
         "isSubsetOf returned true when the target TimeRange overlaps a later portion of the reference TimeRange";
      testMessageArray[4]  =
         "isSubsetOf returned false when the reference TimeRange is interior to the target TimeRange";
      testMessageArray[5]  =
         "isSubsetOf returned true when the target TimeRange is after the reference TimeRange";
      testMessageArray[6]  =
         "isSubsetOf returned true when the target TimeRange ends at and includes the beginning of the reference TimeRange";
      testMessageArray[7]  =
         "isSubsetOf returned true when the target TimeRange ends at but does not include the beginning of the reference TimeRange";
      testMessageArray[8]  =
         "isSubsetOf returned true when the target TimeRange is interior to the reference TimeRange and shares a start value";
      testMessageArray[9]  =
         "isSubsetOf returned true when the target TimeRange is interior to the reference TimeRange and shares an end value";
      testMessageArray[10] =
         "isSubsetOf returned true when the target TimeRange starts at and includes the end of reference TimeRange";
      testMessageArray[11] =
         "isSubsetOf returned true when the target TimeRange starts at but does not include the end of reference TimeRange";
      testMessageArray[12] =
         "isSubsetOf returned false when the target TimeRange starts equals reference TimeRange";


      testFramework.assert(!referenceTimeRange.isSubsetOf(priorTimeRange)          ,
                           testMessageArray[0] , __LINE__);
      testFramework.assert(!referenceTimeRange.isSubsetOf(earlyOverlapTimeRange)   ,
                           testMessageArray[1] , __LINE__);
      testFramework.assert(!referenceTimeRange.isSubsetOf(interiorTimeRange)       ,
                           testMessageArray[2] , __LINE__);
      testFramework.assert(!referenceTimeRange.isSubsetOf(lateOverlapTimeRange)    ,
                           testMessageArray[3] , __LINE__);
      testFramework.assert( referenceTimeRange.isSubsetOf(completeOverlapTimeRange),
                            testMessageArray[4] , __LINE__);
      testFramework.assert(!referenceTimeRange.isSubsetOf(afterTimeRange)          ,
                           testMessageArray[5] , __LINE__);
      testFramework.assert(!referenceTimeRange.isSubsetOf(priorEdgeCase)           ,
                           testMessageArray[6] , __LINE__);
      testFramework.assert(!referenceTimeRange.isSubsetOf(priorEdgeCaseNoOverlap)  ,
                           testMessageArray[7] , __LINE__);
      testFramework.assert(!referenceTimeRange.isSubsetOf(interiorEarlyEdge)       ,
                           testMessageArray[8] , __LINE__);
      testFramework.assert(!referenceTimeRange.isSubsetOf(interiorLateEdge)        ,
                           testMessageArray[9] , __LINE__);
      testFramework.assert(!referenceTimeRange.isSubsetOf(afterEdgeCase)           ,
                           testMessageArray[10], __LINE__);
      testFramework.assert(!referenceTimeRange.isSubsetOf(afterEdgeCaseNoOverlap)  ,
                           testMessageArray[11], __LINE__);
      testFramework.assert( referenceTimeRange.isSubsetOf(referenceTimeRange)      ,
                            testMessageArray[12], __LINE__);

      return testFramework.countFails();
   }


//=============================================================================
// Test for the isAfter method
// Usage: referenceRange.isAfter(targetRange)
// If the reference range is entirely after the target range, the method is to return to true.
//=============================================================================
   int isAfterTest ( void )
   {
      TUDEF( "TimeRange", "isAfter" );

         //Two time points before the reference TimeRange start endpoint
      CommonTime earlierThanRangeStart = gpstk::CivilTime(2010,12,20, 0, 0,
                                         0.0 ).convertToCommonTime();
      CommonTime earlierThanRangeEnd   = gpstk::CivilTime(2010,12,29, 0, 0,
                                         0.0 ).convertToCommonTime();
         //Reference TimeRange start endpoint
      CommonTime startEndpoint         = gpstk::CivilTime(2011, 1, 1, 0, 0,
                                         0.0 ).convertToCommonTime();
         //Two time points inside the reference TimeRange
      CommonTime timeInRangeStart      = gpstk::CivilTime(2011, 1,10, 0, 0,
                                         0.0 ).convertToCommonTime();
      CommonTime timeInRangeEnd        = gpstk::CivilTime(2011, 1,20, 0, 0,
                                         0.0 ).convertToCommonTime();
         //Reference TimeRange end endpoint
      CommonTime endEndpoint           = gpstk::CivilTime(2011, 1,31,23,59,
                                         59.59).convertToCommonTime();
         //Two time points after the reference TimeRange end endpoint
      CommonTime laterThanRangeStart   = gpstk::CivilTime(2011, 2,20, 0, 0,
                                         0.0 ).convertToCommonTime();
      CommonTime laterThanRangeEnd     = gpstk::CivilTime(2011, 2,27, 0, 0,
                                         0.0 ).convertToCommonTime();

         //Include endpoints for all checks
      bool beginningIncluded = true;
      bool endIncluded = true;

         //Create various TimeRanges
      TimeRange referenceTimeRange       (startEndpoint,         endEndpoint,
                                          beginningIncluded, endIncluded);
      TimeRange priorTimeRange           (earlierThanRangeStart, earlierThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange earlyOverlapTimeRange    (earlierThanRangeStart, timeInRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange interiorTimeRange        (timeInRangeStart,      timeInRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange lateOverlapTimeRange     (timeInRangeStart,      laterThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange completeOverlapTimeRange (earlierThanRangeStart, laterThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange afterTimeRange           (laterThanRangeStart,   laterThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange priorEdgeCase            (earlierThanRangeStart, startEndpoint,
                                          beginningIncluded, endIncluded);
      TimeRange priorEdgeCaseNoOverlap   (earlierThanRangeStart, startEndpoint,
                                          beginningIncluded, false      );
      TimeRange interiorEarlyEdge        (startEndpoint,         timeInRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange interiorLateEdge         (timeInRangeStart,      endEndpoint,
                                          beginningIncluded, endIncluded);
      TimeRange afterEdgeCase            (endEndpoint,           laterThanRangeEnd,
                                          beginningIncluded, endIncluded);
      TimeRange afterEdgeCaseNoOverlap   (endEndpoint,           laterThanRangeEnd,
                                          beginningIncluded, false      );


      std::string testMessageArray[13];
      testMessageArray[0]  =
         "isAfter returned false when the target TimeRange is completely before the reference TimeRange";
      testMessageArray[1]  =
         "isAfter returned true when the target TimeRange overlaps the earlier portion of the reference TimeRange";
      testMessageArray[2]  =
         "isAfter returned true when the target TimeRange is interior to the reference TimeRange";
      testMessageArray[3]  =
         "isAfter returned true when the target TimeRange overlaps a later portion of the reference TimeRange";
      testMessageArray[4]  =
         "isAfter returned true when the reference TimeRange is interior to the target TimeRange";
      testMessageArray[5]  =
         "isAfter returned true when the target TimeRange is after the reference TimeRange";
      testMessageArray[6]  =
         "isAfter returned true when the target TimeRange ends at and includes the beginning of the reference TimeRange";
      testMessageArray[7]  =
         "isAfter returned false when the target TimeRange ends at but does not include the beginning of the reference TimeRange";
      testMessageArray[8]  =
         "isAfter returned true when the target TimeRange is interior to the reference TimeRange and shares a start value";
      testMessageArray[9]  =
         "isAfter returned true when the target TimeRange is interior to the reference TimeRange and shares an end value";
      testMessageArray[10] =
         "isAfter returned true when the target TimeRange starts at and includes the end of reference TimeRange";
      testMessageArray[11] =
         "isAfter returned true when the target TimeRange starts at but does not include the end of reference TimeRange";
      testMessageArray[12] =
         "isAfter returned true when the target TimeRange starts equals reference TimeRange";


      testFramework.assert( referenceTimeRange.isAfter(priorTimeRange)          ,
                            testMessageArray[0] , __LINE__);
      testFramework.assert(!referenceTimeRange.isAfter(earlyOverlapTimeRange)   ,
                           testMessageArray[1] , __LINE__);
      testFramework.assert(!referenceTimeRange.isAfter(interiorTimeRange)       ,
                           testMessageArray[2] , __LINE__);
      testFramework.assert(!referenceTimeRange.isAfter(lateOverlapTimeRange)    ,
                           testMessageArray[3] , __LINE__);
      testFramework.assert(!referenceTimeRange.isAfter(completeOverlapTimeRange),
                           testMessageArray[4] , __LINE__);
      testFramework.assert(!referenceTimeRange.isAfter(afterTimeRange)          ,
                           testMessageArray[5] , __LINE__);
      testFramework.assert(!referenceTimeRange.isAfter(priorEdgeCase)           ,
                           testMessageArray[6] , __LINE__);
      testFramework.assert( referenceTimeRange.isAfter(priorEdgeCaseNoOverlap)  ,
                            testMessageArray[7] , __LINE__);
      testFramework.assert(!referenceTimeRange.isAfter(interiorEarlyEdge)       ,
                           testMessageArray[8] , __LINE__);
      testFramework.assert(!referenceTimeRange.isAfter(interiorLateEdge)        ,
                           testMessageArray[9] , __LINE__);
      testFramework.assert(!referenceTimeRange.isAfter(afterEdgeCase)           ,
                           testMessageArray[10], __LINE__);
      testFramework.assert(!referenceTimeRange.isAfter(afterEdgeCaseNoOverlap)  ,
                           testMessageArray[11], __LINE__);
      testFramework.assert(!referenceTimeRange.isAfter(referenceTimeRange)      ,
                           testMessageArray[12], __LINE__);


      return testFramework.countFails();
   }

//=============================================================================
// Test for the == Operator
// Usage: leftRange == rightRange
// If the left TimeRange is exactly (same start point, end point,
// and endpoint inclusions) return true. Otherwise false
//=============================================================================
   int equalsOperatorTest ( void )
   {
      TUDEF( "TimeRange", "OperatorEquivalence" );

         //Reference TimeRange endpoints
      CommonTime startPoint  = gpstk::CivilTime(2011, 1, 1, 0, 0,
                               0.0 ).convertToCommonTime();
      CommonTime endPoint    = gpstk::CivilTime(2011, 1,31,23,59,
                               59.59).convertToCommonTime();
      CommonTime anotherTime = gpstk::CivilTime(2011, 1,10, 0, 0,
                               0.0 ).convertToCommonTime();

         //Create various TimeRanges
      TimeRange referenceTimeRange(startPoint,  endPoint,    true,  true );
      TimeRange differentStart    (anotherTime, endPoint,    true,  true );
      TimeRange differentEnd      (startPoint,  anotherTime, true,  true );
      TimeRange noInitialPoint    (startPoint,  endPoint,    false, true );
      TimeRange noFinalPoint      (startPoint,  endPoint,    true,  false);
      TimeRange copiedTimeRange   (referenceTimeRange);

      testFramework.assert( !(referenceTimeRange == differentStart) ,
                            "Equivalence operator returned true when the start time is different"
                            , __LINE__ );
      testFramework.assert( !(referenceTimeRange == differentEnd)   ,
                            "Equivalence operator returned true when the end time is different"
                            , __LINE__ );
      testFramework.assert( !(referenceTimeRange == noInitialPoint) ,
                            "Equivalence operator returned true when the start time inclusion boolean is different",
                            __LINE__ );
      testFramework.assert( !(referenceTimeRange == noFinalPoint)   ,
                            "Equivalence operator returned true when the end time inclusion boolean is different"
                            , __LINE__ );
      testFramework.assert(  (referenceTimeRange == copiedTimeRange),
                             "Equivalence operator returned false when the time ranges are copies"
                             , __LINE__ );


      return testFramework.countFails();
   }

//=============================================================================
// Test for the set method
// Method changes the internal values of the TimeRange object
// Test that the interior attributes have changed.
//=============================================================================
   int setTest ( void )
   {
      TUDEF( "TimeRange", "set" );


         //Reference TimeRange endpoints
      CommonTime startPoint  = gpstk::CivilTime(2011, 1, 1, 0, 0,
                               0.0 ).convertToCommonTime();
      CommonTime endPoint    = gpstk::CivilTime(2011, 1,31,23,59,
                               59.59).convertToCommonTime();
      CommonTime anotherTime = gpstk::CivilTime(2011, 1,10, 0, 0,
                               0.0 ).convertToCommonTime();

         //Create various TimeRanges
      TimeRange referenceTimeRange(startPoint,  endPoint,    true,  true );
      TimeRange changedTimeRange;

         //--------------------------------------------------------------------
         //Verify set method functions and throws exceptions when it should
         //--------------------------------------------------------------------
      try
      {
         changedTimeRange.set(anotherTime, startPoint, true, true);
         testFramework.assert(false,
                              "set method allowed for the end time to be earlier than the start time",
                              __LINE__);
      }
      catch (Exception& e)
      {
         testFramework.assert(true, "set method threw the expected error", __LINE__);
      }
      catch (...)
      {
         testFramework.assert(false,
                              "set method threw an unexpected error for when the end time is earlier than the start time",
                              __LINE__);
      }


         //Verify set does not throw an exception when inputs are valid
      try
      {
         changedTimeRange.set(startPoint, endPoint, true, true);
         testFramework.assert(true, "set method for valid data functioned properly",
                              __LINE__);
      }
      catch (...)
      {
         testFramework.assert(false,
                              "set method for valid data functioned threw an error", __LINE__);
      }

      changedTimeRange.set(startPoint, endPoint, true, true);
      testFramework.assert( (changedTimeRange == referenceTimeRange),
                            "One of the values was not set properly", __LINE__ );

      return testFramework.countFails();
   }



//=============================================================================
// Test for the printf method
//=============================================================================
   int printfTest ( void )
   {
      TUDEF( "TimeRange", "printf" );


      std::stringstream printfOutputStream;

         //Reference TimeRange endpoints
      CommonTime startPoint  = gpstk::CivilTime(2011, 1, 1, 0, 0,
                               0.0 ).convertToCommonTime();
      CommonTime endPoint    = gpstk::CivilTime(2011, 1,31,23,59,
                               59.59).convertToCommonTime();

         //Create various TimeRanges
      TimeRange timeRangeArray[7];
      timeRangeArray[0].set(startPoint, endPoint, true,  true  );
      timeRangeArray[1].set(startPoint, endPoint, true,  false );
      timeRangeArray[2].set(startPoint, endPoint, false, true  );
      timeRangeArray[3].set(startPoint, endPoint, false, false );
      timeRangeArray[4].set(startPoint, endPoint, true,  true  );
      timeRangeArray[5].set(startPoint, endPoint, true,  true  );
      timeRangeArray[6].set(startPoint, endPoint, true,  true  );

      std::string testFmts[]=
      {
         "%Y %m %d %H %M %S",
         "%Y %m %d %H %M %S",
         "%Y %m %d %H %M %S",
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

      std::string testMessage = "Printed string did not match expected output";

      for (int i = 0; i < 7; i++)
      {
         printfOutputStream <<  timeRangeArray[i].printf(testFmts[i]);
         testFramework.assert( (printfOutputStream.str() == correctResults[i]),
                               testMessage, __LINE__ );
         printfOutputStream.str(std::string());
      }

      return testFramework.countFails();
   }

//=============================================================================
// Test for the dump method
//=============================================================================
   int dumpTest ( void )
   {
      TUDEF( "TimeRange", "dump" );

      std::stringstream dumpOutputStream;

         //Reference TimeRange endpoints
      CommonTime startPoint  = gpstk::CivilTime(2011, 1, 1, 0, 0,
                               0.0 ).convertToCommonTime();
      CommonTime endPoint    = gpstk::CivilTime(2011, 1,31,23,59,
                               59.59).convertToCommonTime();

         //Create various TimeRanges
      TimeRange timeRangeArray[7];
      timeRangeArray[0].set(startPoint, endPoint, true,  true  );
      timeRangeArray[1].set(startPoint, endPoint, true,  false );
      timeRangeArray[2].set(startPoint, endPoint, false, true  );
      timeRangeArray[3].set(startPoint, endPoint, false, false );
      timeRangeArray[4].set(startPoint, endPoint, true,  true  );
      timeRangeArray[5].set(startPoint, endPoint, true,  true  );
      timeRangeArray[6].set(startPoint, endPoint, true,  true  );

      std::string testFmts[]=
      {
         "%Y %m %d %H %M %S",
         "%Y %m %d %H %M %S",
         "%Y %m %d %H %M %S",
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

      std::string testMessage = "Printed string did not match expected output";

      for (int i = 0; i < 7; i++)
      {
         dumpOutputStream <<  timeRangeArray[i].dump(testFmts[i]);
         testFramework.assert( (dumpOutputStream.str() == correctResults[i]),
                               testMessage, __LINE__ );
         dumpOutputStream.str(std::string());
      }

      return testFramework.countFails();

   }
private:
   double eps;
};


int main() //Main function to initialize and run all tests above
{
   int errorTotal = 0;
   TimeRange_T testClass;

   errorTotal += testClass.constructorTest();
   errorTotal += testClass.inRangeTest();
   errorTotal += testClass.isPriorToTest();
   errorTotal += testClass.overlapsTest();
   errorTotal += testClass.isSubsetOfTest();
   errorTotal += testClass.isAfterTest();
   errorTotal += testClass.equalsOperatorTest();
   errorTotal += testClass.setTest();
   errorTotal += testClass.printfTest();
   errorTotal += testClass.dumpTest();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal; //Return the total number of errors
}

