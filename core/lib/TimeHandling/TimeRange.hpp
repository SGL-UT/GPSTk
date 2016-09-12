/**
 *   A utility class that contains a pair of CommonTime objects that
 *   specify the beginning and end of a time range.  
 */
#ifndef TIMERANGE_HPP
#define TIMERANGE_HPP

#include <map>
#include <string>

#include "CommonTime.hpp"
#include "Exception.hpp"
#include "StringUtils.hpp"


namespace gpstk
{
      /// @ingroup TimeHandling
      //@{

   class TimeRange
   {
   public:

      NEW_EXCEPTION_CLASS(TimeRangeException, Exception);
      
      typedef std::pair<CommonTime, CommonTime> DTPair;
   
      TimeRange();
      
      TimeRange(const CommonTime& startDT, 
                const CommonTime& endDT,
                const bool startInclusive=true, 
                const bool endInclusive=true ) throw(TimeRangeException);

         /// To cover potential use with RiseSetTimeList               
      TimeRange( DTPair dtPair,
                const bool startInclusive=true, 
                const bool endInclusive=true ) throw(TimeRangeException); 

         /// Copy construtor       
      TimeRange(const TimeRange& tr); 
     
      ~TimeRange( ) {};

         // Accessors
      CommonTime getStart() const { return start; }
      CommonTime getEnd() const { return end; }

      void set( const CommonTime& startDT, 
                const CommonTime& endDT,
                const bool startInclusive=true, 
                const bool endInclusive=true ) throw(TimeRangeException);

         /** Return true is testDT is within the TimeRange.  Whether
          * the boundaries are included is in accordance with the
          * indications provided in includeStartTime and
          * includeEndTime. */
      bool inRange( const CommonTime& testDT ) const; 

         /// Equivalence means all members are identical.
      bool operator==(const TimeRange& right) const;

         /** True if start/end of this object are both prior
          * to start of "right" */
      bool isPriorTo( const TimeRange& right ) const;

         /** @return true if this.start < right.end and 
          * this.end > right.start */
      bool overlaps( const TimeRange& right ) const;

         /** @return true if this.start >= right.start and 
          * this.end <= right.end */
      bool isSubsetOf( const TimeRange& right ) const;
      
         /** True if start/end of this object are both after the end
          * of "right" */
      bool isAfter( const TimeRange& right ) const;

         /** Formatted string input.
          * Assume string has 
          *  \li possible white space followed by 
          *  \li optional '[' or '(' (assume '['), 
          *  \li followed by a valid CommonTime string corresponding to fmt, 
          *  \li followed by a ','
          *  \li followed by a valid CommonTime string corresponding to fmt,
          *  \li followed by an optional ']' or ')' (assume ']').  */
      TimeRange& setToString( const std::string& str, 
                              const std::string& fmt)
         throw(TimeRangeException, 
               StringUtils::StringException);

         /// Formatted print
      std::string printf(const std::string formatArg="%02m/%02d/%02y %02H:%02M:%02S" ) const
        throw(gpstk::StringUtils::StringException);

         /// Dump method.   
      std::string dump(const std::string formatArg="%02m/%02d/%02y %02H:%02M:%02S" ) const;
        
   protected: 
      CommonTime start;
      CommonTime end;
      bool includeStartTime;
      bool includeEndTime; 

      void init(const CommonTime& startDT, 
                const CommonTime& endDT,
                const bool startInclusive=true, 
                const bool endInclusive=true ) throw(TimeRangeException);

   };

      //@}

}   // end namespace

#endif

