#include "TimeRange.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk; 


namespace gpstk
{
   using namespace std;

   TimeRange::TimeRange():
      start(CommonTime::BEGINNING_OF_TIME),
      end(CommonTime::END_OF_TIME),
      includeStartTime(true),
      includeEndTime(true) 
      {} 

   TimeRange::TimeRange(
                const CommonTime& startDT, 
                const CommonTime& endDT,
                const bool startInclusive, 
                const bool endInclusive ) throw(TimeRangeException)
   {
      try
      {
         init( startDT, endDT, startInclusive, endInclusive );
      }
      catch (TimeRangeException tre)
      {
         tre.addText("Initializing from two CommonTime values.\n");
         GPSTK_RETHROW(tre);
      }
   }
   
   TimeRange::TimeRange( DTPair dtPair,
                const bool startInclusive, 
                const bool endInclusive ) throw(TimeRangeException)
   {
      try
      {
         init( dtPair.first, 
               dtPair.second,
               startInclusive,
               endInclusive );
      }
      catch (TimeRangeException tre)
      {
         tre.addText("Initializing from a CommonTime pair.\n");
         GPSTK_RETHROW(tre);
      }
   }

      // Common initialization method used by two constructors
   void TimeRange::init( const CommonTime& startDT, 
                    const CommonTime& endDT,
                    const bool startInclusive, 
                    const bool endInclusive ) throw(TimeRangeException)
   {
      if (endDT<startDT)
      {
         string tform = "%02m/%02d/%4Y %02H:%02M:%02S %Q";
         TimeRangeException tre;
         tre.addText("TimeRange() - Start time must be before end time.\n");
         string ts = "  start: ";
         ts += printTime(startDT,tform); 
         ts += "\n";
         tre.addText(ts);
         ts = "    end: ";
         ts += printTime(endDT,tform); 
         ts += "\n";
         tre.addText(ts); 
         GPSTK_THROW(tre);
      }
      
      start = startDT;
      end = endDT;
      includeStartTime = startInclusive;
      includeEndTime = endInclusive;      
   }

   void TimeRange::set( const CommonTime& startDT, 
                const CommonTime& endDT,
                const bool startInclusive, 
                const bool endInclusive ) throw(TimeRangeException)
   {
      try
      {
         init( startDT, endDT, startInclusive, endInclusive );
      }
      catch (TimeRangeException tre)
      {
         tre.addText("Called from TimeRange::set().\n");
         GPSTK_RETHROW(tre);
      }
   }

   
   TimeRange::TimeRange(const TimeRange& tr)
   {
      start = tr.start;
      end = tr.end;
      includeStartTime = tr.includeStartTime;
      includeEndTime = tr.includeEndTime;
   }

 
   bool TimeRange::inRange( const CommonTime& testDT ) const
   {
         // First, test for cases that don't involve equality
         // as it is expected most cases will fall in this category.
      if (testDT<start) return false;
      if (end<testDT) return false;
      if (start<testDT &&
          testDT < end) return true;

         // Last, test for the cases in which the test time is 
         // on a boundary condition.
         // To reach this code, it is already proven that
         // testDT must be equal to either start or end; however,
         // we haven't proven which.
      if (testDT==start && includeStartTime==true) return true;
      if (testDT==end && includeEndTime==true) return true;
      return false;
   }

      // True if all members are equal
   bool TimeRange::operator==(const TimeRange& right) const
   {
     if (start==right.start &&
         end==right.end &&
         includeStartTime==right.includeStartTime &&
         includeEndTime==right.includeEndTime ) return true;
     return false;
   }
   
      // True if start/end of this object are both prior
      // to start of "right"
      // Note that the constructor verifies start<end. 
   bool TimeRange::isPriorTo( const TimeRange& right ) const
   {
      if (end < right.start) return true;

         // Now handle two edge cases.  These are where
         // the edges of "this" and right align such that
         //   [  this )                     [ this ]
         //           [ right ]   - or -           (  right  ]
         // In these cases, "this" qualifies as being prior to right.
      if (end==right.start && 
          (!includeEndTime || !right.includeStartTime) ) return true;

      return false;
   }

      // True if this.start <= right.end and 
      //         this.end >= right.start   
   bool TimeRange::overlaps( const TimeRange& right ) const
   {
        // Check the simple case first
      if ( start<right.end &&
             end>right.start ) return true;

        // Then check edge cases.  There are two picky edge 
        // cases.  One where "this" leads right, both have boundaries
        // included and the end of "this" is aligned with the end of "right"
        //    [  this ]
        //            [ right ]
        // The second is similar, but reversed.
        //            [  this ]
        //    [ right ]
        //
      if (       includeEndTime && 
           right.includeStartTime && 
                 end==right.start ) return true;

      if (       includeStartTime && 
           right.includeEndTime && 
                 start==right.end ) return true;
             
      return false;
   }

      // True if this.start >= right.start and 
      //         this.end <= right.end   
   bool TimeRange::isSubsetOf( const TimeRange& right ) const
   {
      if (start>=right.start &&
            end<=right.end ) return true;
      return false;
   }
   
      // True if start/end of this object are both
      // after the end of "right"
   bool TimeRange::isAfter( const TimeRange& right ) const
   {
      if (start > right.end) return true;

         // Now handle two edge cases.  These are where
         // the edges of "this" and right align such that
         //            [  this ]                        ( this ]
         //    [ right )           - or -     [  right  ]
         // In these cases, "this" qualifies as being after right.
      if (start==right.end && 
          (!includeStartTime || !right.includeEndTime) ) return true;

      return false;
   }

      // Formatted input
   TimeRange& TimeRange::setToString( const string& str, 
                                      const string& fmt)
         throw(TimeRangeException, 
               StringUtils::StringException)
   {
         // Ignore leading whitespace.  
         // See if first non-whitespace character is '[' or '('
      std::string whitespace(" \t\n"); 
      std::string::size_type n = str.find_first_not_of(whitespace);
      includeStartTime = true;    // default case
      string leadChar = str.substr(n,1);
//      std::cout << "leadChar :'" << leadChar << "'" << endl;
      if (leadChar.compare("[")==0)
      {
         n++;       // Already set inclusion flag,
                    // before continuing to parse the line. 
      }
      else if (leadChar.compare("(")==0)
      {
         includeStartTime = false; 
         n++;
      }

         // Find separating comma indicating the end of the first CommonTime
      std::string::size_type endFirstTime = str.find(",", n); 
      std::string firstTime = str.substr(n, endFirstTime-n); 
      StringUtils::stripLeading(firstTime);
//      std::cout << "firstTime:'" << firstTime << "'" << endl;

      try
      {
         mixedScanTime(start, firstTime, fmt); 
      }
      catch(InvalidRequest& exc)
      {
         TimeRangeException tre(exc);
         tre.addText("TimeRange:Failure converting first time");
         GPSTK_THROW(tre);
      }
      catch(std::exception& exc)
      {
         TimeRangeException tre(exc.what());
         tre.addText("TimeRange:Failure converting first time");
         GPSTK_THROW(tre);
      }

//      std::cout << "Finished first time convert:"
//                << start.printf("%02m/%02m/%4Y %02H:%02M:%02S") << std::endl;
      
         // Find optional end inclusion definition.
      std::string::size_type endCharPos = str.find_first_of("])", endFirstTime+1);
      std::string::size_type endSecondTime = endCharPos;
      includeEndTime = true; 
      if (endCharPos!=std::string::npos)
      {
         endSecondTime--;
         string endChar = str.substr(endCharPos,1);
//         std::cout << "endChar: '" << endChar << "'" << endl;
         if (endChar.compare(")")==0) 
         {
            includeEndTime = false;
         }
      }
      
      std::string secondTime = str.substr( endFirstTime+1, endSecondTime-endFirstTime );
      StringUtils::stripLeading(secondTime);
///      std::cout << "secondTime: '" << secondTime << "'" << endl;
      try
      {
         mixedScanTime(end, secondTime, fmt);
      }
      catch(InvalidRequest& exc)
      {
         TimeRangeException tre(exc);
         tre.addText("TimeRange:Failure converting second time");
         GPSTK_THROW(tre);
      }
      catch(std::exception& exc)
      {
         TimeRangeException tre(exc.what());
         tre.addText("TimeRange:Failure converting second time");
         GPSTK_THROW(tre);
      }

         // The end time must be greater than the start time.  
      if (end<start)
      {
         TimeRangeException tre("Ending time is prior to beginning time");

         GPSTK_THROW(tre);
      }

      return *this; 
   }

      // Formatted output
   std::string TimeRange::printf(const std::string formatArg) const
        throw(gpstk::StringUtils::StringException)
   {
      string out;

      if (includeStartTime) out += "[";
       else out += "(";
      out += printTime(start, formatArg);
      out += ", ";
      out += printTime(end, formatArg);

      char includeEnd = ']';
      if (!includeEndTime) includeEnd = ')';
      out += includeEnd;
      
      return out;
   }


   std::string TimeRange::dump(const std::string formatArg ) const
   {
//      char includeStart = '[';
//      if (!includeStartTime) includeStart = '(';
      string out;

      if (includeStartTime) out += "[Start:";
       else out += "(Start:";
      out += printTime(start, formatArg);
      out += ", End: ";
      out += printTime(end, formatArg);

      char includeEnd = ']';
      if (!includeEndTime) includeEnd = ')';
      out += includeEnd;
      
      return out;
   }
}
