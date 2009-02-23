#pragma ident "$Id$"


/**
 * @file BELogEntry.hpp
 * Record the unique identifying information associated
 * with a Broadcast Ephemeris, allow it to be ordered (i.e. map support)
 * and provide a string output capability. 
 */

#ifndef GPSTK_BELOGENTRY_HPP
#define GPSTK_BELOGENTRY_HPP

//lgpl-license START
//lgpl-license END

//dod-release-statement START
//dod-release-statement END

#include "EngEphemeris.hpp"

namespace gpstk
{
   class BELogEntry
   {
   public:
         /// Default constructor
      BELogEntry( const gpstk::EngEphemeris ee );
      
         /// Destructor
      virtual ~BELogEntry() {}

      std::string getStr() const;
      gpstk::DayTime getHOW() const;
      void increment();
      unsigned long getKey() const;
      static const std::string header;
      
      protected:
         DayTime HOWSF1;
         DayTime Toe;
         int PRN_ID;
         int IODC;
         int count;
         unsigned long key;
         
   }; // class BELogEntry

} // namespace

#endif
