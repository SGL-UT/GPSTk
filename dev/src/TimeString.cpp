#pragma ident "$Id$"



//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

#include "TimeString.hpp"

#include "ANSITime.hpp"
#include "CivilTime.hpp"
#include "GPSEpochWeekSecond.hpp"
#include "GPSWeekSecond.hpp"
#include "GPSWeekZcount.hpp"
#include "GPSZcount29.hpp"
#include "GPSZcount32.hpp"
#include "JulianDate.hpp"
#include "MJD.hpp"
#include "UnixTime.hpp"
#include "YDSTime.hpp"

#include "TimeConverters.hpp"
#include "TimeConstants.hpp"

namespace gpstk
{
   std::string printTime( const TimeTag& t,
                          const std::string& fmt )
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         std::string rv( fmt );
         
            // First, try to print using 't' itself.
         rv = t.printf( rv );
         
            // Next, run it through all the other TimeTag classes.
         return printTime( t.convertToCommonTime(), rv );

      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }

   std::string printTime( const CommonTime& t,
                          const std::string& fmt )
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         std::string rv( fmt );

            // Convert to each TimeTag type and run its printf using rv.
            // The printf functions test to see if they can print any part
            // of the requested format.  If they can, they do, and if not, 
            // simply return without doing any work.
         rv = ANSITime( t ).printf( rv );
         rv = CivilTime( t ).printf( rv );
         rv = GPSEpochWeekSecond( t ).printf( rv );
         rv = GPSWeekSecond( t ).printf( rv );
         rv = GPSWeekZcount( t ).printf( rv );
         rv = GPSZcount32( t ).printf( rv );
         rv = GPSZcount29( t ).printf( rv );
         rv = JulianDate( t ).printf( rv );
         rv = MJD( t ).printf( rv );
         rv = UnixTime( t ).printf( rv );
         rv = YDSTime( t ).printf( rv );
      
         return rv;
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }
   
      /// Fill the TimeTag object \a btime with time information found in
      /// string \a str formatted according to string \a fmt.
   void scanTime( TimeTag& btime,
                  const std::string& str,
                  const std::string& fmt )
      throw( gpstk::InvalidRequest,
             gpstk::StringUtils::StringException )
   {
      try
      {
            // Get the mapping of character (from fmt) to value (from str).
         TimeTag::IdToValue info;
         TimeTag::getInfo( str, fmt, info );
         
         if( btime.setFromInfo( info ) )
         {
            return;
         }
         
            // Convert to CommonTime, and try to set using all formats.
         CommonTime ct( btime.convertToCommonTime() );
         scanTime( ct, str, fmt );

            // Convert the CommonTime into the requested format.
         btime.convertFromCommonTime( ct );
      }
      catch( gpstk::InvalidRequest& ir )
      {
         GPSTK_RETHROW( ir );
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }
   
   void scanTime( CommonTime& t,
                  const std::string& str,
                  const std::string& fmt )
      throw( gpstk::InvalidRequest,
             gpstk::StringUtils::StringException )
   {
      try
      {
         using namespace gpstk::StringUtils;

            // Get the mapping of character (from fmt) to value (from str).
         TimeTag::IdToValue info;
         TimeTag::getInfo( str, fmt, info );
         
            // These indicate which information has been found.
         bool hmjd( false ), hsow( false ), hweek( false ), hfullweek( false ),
            hdow( false ), hyear( false ), hmonth( false ), hday( false ),
            hzcount( false ), hdoy( false ), hzcount29( false ), 
            hzcount32( false ), hhour( false ), hmin( false ), hsec( false ),
            hsod( false ), hunixsec( false ), hunixusec( false ), 
            hepoch( false ), hansi( false ), hjulian( false );

            // These are to hold data that no one parses.
         int idow( 0 );
         
         for( TimeTag::IdToValue::iterator itr = info.begin();
              itr != info.end(); itr++ )
         {
            switch( itr->first )
            {
               case 'Q':
                  hmjd = true;
                  break;

               case 'Z':
                  hzcount = true;
                  break;

               case 's':
                  hsod = true;
                  break;

               case 'g':
                  hsow = true;
                  break;

               case 'w':
                  idow = asInt( itr->second );
                  hdow = true;
                  break;

               case 'G':
                  hweek = true;
                  break;

               case 'F':
                  hfullweek = true;
                  break;

               case 'j':
                  hdoy = true;
                  break;

               case 'b':
               case 'B':
                  hmonth = true;
                  break;

               case 'Y':
               case 'y':
                  hyear = true;
                  break;

               case 'a':
               case 'A':
               {
                  hdow = true;
                  std::string thisDay = firstWord( itr->second );
                  lowerCase(thisDay);
                  if (isLike(thisDay, "sun.*")) idow = 0;
                  else if (isLike(thisDay, "mon.*")) idow = 1;
                  else if (isLike(thisDay, "tue.*")) idow = 2;
                  else if (isLike(thisDay, "wed.*")) idow = 3;
                  else if (isLike(thisDay, "thu.*")) idow = 4;
                  else if (isLike(thisDay, "fri.*")) idow = 5;
                  else if (isLike(thisDay, "sat.*")) idow = 6;
                  else
                  {
                     hdow = false;
                  }
               }
               break;
                  
               case 'm':
                  hmonth = true;
                  break;

               case 'd':
                  hday = true;
                  break;

               case 'H':
                  hhour = true;
                  break;

               case 'M':
                  hmin = true;
                  break;

               case 'S':
                  hsec = true;
                  break;

               case 'f':
                  hsec = true;
                  break;

               case 'U':
                  hunixsec = true;
                  break;

               case 'u':
                  hunixusec = true;
                  break;
                  
               case 'c':
                  hzcount29 = true;
                  break;

               case 'C':
                  hzcount32 = true;
                  break;

               case 'J':
                  hjulian = true;
                  break;
                  
               case 'K':
                  hansi = true;
                  break;
                  
               case 'E':
                  hepoch = true;
                  break;

               default:
               {
                     // do nothing
               }
               break;

            };
         }

         if( hyear )
         {
            if( hmonth && hday )
            {
               CivilTime tt;
               tt.setFromInfo( info );
               if( hsod )
               {
                  convertSODtoTime( asDouble( info['s'] ), 
                                    tt.hour, tt.minute, tt.second );
               }
               t = tt.convertToCommonTime();
               return;
            }
            else  // use YDSTime as default
            {
               YDSTime tt;
               tt.setFromInfo( info );
               if( hhour && hmin && hsec )
               {
                  tt.sod = convertTimeToSOD( asInt( info['H'] ), 
                                             asInt( info['M'] ), 
                                             asDouble( info['S'] ) );
               }
               t = tt.convertToCommonTime();
               return;
            }

         } // end of if( hyear )

         if( hzcount32 )
         {
            GPSZcount32 tt;
            tt.setFromInfo( info );
            t = tt.convertToCommonTime();
            return;
         }

         if( hepoch )
         {
            if( hzcount29 )
            {
               GPSZcount29 tt;
               tt.setFromInfo( info );
               t = tt.convertToCommonTime();
               return;
            }

            if( hweek )
            {
               GPSEpochWeekSecond tt;
               tt.setFromInfo( info );
               if( hdow && !hsow )
               {
                  tt.sow = asInt( info['w'] ) * SEC_PER_DAY;
                  if( hsod )
                  {
                     tt.sow += asDouble( info['s'] );
                  }
                  else if( hhour && hmin && hsec )
                  {
                     tt.sow += convertTimeToSOD( asInt( info['H'] ), 
                                                 asInt( info['M'] ), 
                                                 asDouble( info['S'] ) );
                  }
               }
               t = tt.convertToCommonTime();
               return;

            } // end of if( hweek )

         } // end of if( hepoch )
         
         if( hfullweek )
         {
            if( hzcount )
            {
               GPSWeekZcount tt;
               tt.setFromInfo( info );
               t = tt.convertToCommonTime();
               return;
            }
            
            GPSWeekSecond tt;
            tt.setFromInfo( info );
            if( hdow && !hsow )
            {
               tt.sow = asInt( info['w'] ) * SEC_PER_DAY;
               if( hsod )
               {
                  tt.sow += asDouble( info['s'] );
               }
               else if( hhour && hmin && hsec )
               {
                  tt.sow += convertTimeToSOD( asInt( info['H'] ), 
                                              asInt( info['M'] ), 
                                              asDouble( info['S'] ) );
               }
            }
            t = tt.convertToCommonTime();
            return;
         } // end of if( hfullweek )

         if( hmjd )
         {
            MJD tt;
            tt.setFromInfo( info );
            t = tt.convertToCommonTime();
            return;
         }

         if( hjulian )
         {
            JulianDate tt;
            tt.setFromInfo( info );
            t = tt.convertToCommonTime();
            return;
         }

         if( hansi )
         {
            ANSITime tt;
            tt.setFromInfo( info );
            t = tt.convertToCommonTime();
            return;
         } 
         
         if( hunixsec || hunixusec )
         {
            UnixTime tt;
            tt.setFromInfo( info );
            t = tt.convertToCommonTime();
            return;
         }

         InvalidRequest ir("Incomplete time specification for readTime");
         GPSTK_THROW( ir );
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }   

   void mixedScanTime( CommonTime& t,
                       const std::string& str,
                       const std::string& fmt )
      throw( gpstk::InvalidRequest,
             gpstk::StringUtils::StringException )
   {
      try
      {
         using namespace gpstk::StringUtils;

            // Get the mapping of character (from fmt) to value (from str).
         TimeTag::IdToValue info;
         TimeTag::getInfo( str, fmt, info );
         
            // These indicate which information has been found.
         bool hsow( false ), hweek( false ), hfullweek( false ),
            hdow( false ), hyear( false ), hmonth( false ), hday( false ),
            hzcount( false ), hdoy( false ), hzcount29( false ), 
            hhour( false ), hmin( false ), hsec( false ),
            hsod( false ), hepoch( false ), hunixsec( false ),
            hunixusec( false );

            // MJD, Julian Date, ANSI time, Unix time, and 32-bit Zcounts
            // are treated as stand-alone types and are not mixed with others
            // if detected.
         
            // These variables will hold the values for use later.
         double isow, isod, isec;
         int iweek, ifullweek, idow, iyear, imonth, iday, izcount, idoy,
            izcount29, ihour, imin, iepoch, iunixsec, iunixusec;
         
         for( TimeTag::IdToValue::iterator itr = info.begin();
              itr != info.end(); itr++ )
         {
            switch( itr->first )
            {
               case 'Q':
                  t = MJD( asLongDouble(itr->second) );
                  return;

               case 'J':
                  t = JulianDate( asLongDouble(itr->second) );
                  return;
                  
               case 'C':
                  t = GPSZcount32( asInt(itr->second) );
                  return;

               case 'K':
                  t = ANSITime( asInt(itr->second) );
                  return;
                  
               case 'U':
               case 'u':
               {
                  UnixTime tt;
                  tt.setFromInfo( info );
                  t = tt.convertToCommonTime();
                  return;
               }
               break;

               case 'Z':
                  hzcount = true;
                  izcount = asInt(itr->second);
                  break;

               case 's':
                  hsod = true;
                  isod = asDouble(itr->second);
                  break;

               case 'g':
                  hsow = true;
                  isow = asDouble(itr->second);
                  break;

               case 'w':
                  idow = asInt(itr->second);
                  hdow = true;
                  break;

               case 'G':
                  hweek = true;
                  iweek = asInt(itr->second);
                  break;

               case 'F':
                  hfullweek = true;
                  ifullweek = asInt(itr->second);
                  break;

               case 'j':
                  hdoy = true;
                  idoy = asInt(itr->second);
                  break;

               case 'b':
               case 'B':
                  hmonth = true;
                  imonth = asInt(itr->second);
                  break;

               case 'Y':
               case 'y':
                  hyear = true;
                  iyear = asInt(itr->second);
                  break;

               case 'a':
               case 'A':
               {
                  hdow = true;
                  std::string thisDay = firstWord( itr->second );
                  lowerCase(thisDay);
                  if (isLike(thisDay, "sun.*")) idow = 0;
                  else if (isLike(thisDay, "mon.*")) idow = 1;
                  else if (isLike(thisDay, "tue.*")) idow = 2;
                  else if (isLike(thisDay, "wed.*")) idow = 3;
                  else if (isLike(thisDay, "thu.*")) idow = 4;
                  else if (isLike(thisDay, "fri.*")) idow = 5;
                  else if (isLike(thisDay, "sat.*")) idow = 6;
               }
               break;
                  
               case 'm':
                  hmonth = true;
                  imonth = asInt(itr->second);
                  break;

               case 'd':
                  hday = true;
                  iday = asInt(itr->second);
                  break;

               case 'H':
                  hhour = true;
                  ihour = asInt(itr->second);
                  break;

               case 'M':
                  hmin = true;
                  imin = asInt(itr->second);
                  break;

               case 'S':
                  hsec = true;
                  isec = asDouble(itr->second);
                  break;

               case 'f':
                  hsec = true;
                  isec = asDouble(itr->second);
                  break;

               case 'c':
                  hzcount29 = true;
                  izcount29 = asInt(itr->second);
                  break;

               case 'E':
                  hepoch = true;
                  iepoch = asInt(itr->second);
                  break;

               default:
                     // do nothing
                  break;

            };
         }


            // We'll copy this time to 't' after all of the processing.
         CommonTime ct;
         
            // Go through all of the types in order of least precise to most
            // precise.
         if( hepoch ) 
         {
            GPSEpochWeekSecond tt(ct);
            tt.epoch = iepoch;
            ct = tt.convertToCommonTime();
         }
         
         if( hyear )
         {
            YDSTime tt(ct);
            tt.year = iyear;
            ct = tt.convertToCommonTime();
         }
 
         if( hmonth )
         {
            CivilTime tt(ct);
            tt.month = imonth;
            ct = tt.convertToCommonTime();
         }

         if( hfullweek )
         {
            GPSWeekSecond tt(ct);
            tt.week = ifullweek;
            ct = tt.convertToCommonTime();
         }
         
         if( hweek )
         {
            GPSEpochWeekSecond tt(ct);
            tt.week = iweek;
            ct = tt.convertToCommonTime();
         }
         
         if( hdow )
         {
            GPSWeekSecond tt(ct);
            tt.sow = static_cast<double>(idow) * SEC_PER_DAY;
            ct = tt.convertToCommonTime();
         }
         
         if( hday )
         {
            CivilTime tt(ct);
            tt.day = iday;
            ct = tt.convertToCommonTime();
         }
         
         if( hdoy )
         {
            YDSTime tt(ct);
            tt.doy = idoy;
            ct = tt.convertToCommonTime();
         }
         
         if( hzcount29 )
         {
            GPSZcount29 tt(ct);
            tt.zcount = izcount29;
            ct = tt.convertToCommonTime(); 
         }

         if( hzcount )
         {
            GPSWeekZcount tt(ct);
            tt.zcount = izcount;
            ct = tt.convertToCommonTime();
         }

         if( hhour )
         {
            CivilTime tt(ct);
            tt.hour = ihour;
            ct = tt.convertToCommonTime();
         }

         if( hmin )
         {
            CivilTime tt(ct);
            tt.hour = imin;
            ct = tt.convertToCommonTime();
         }
         
         if( hsow )
         {
            GPSWeekSecond tt(ct);
            tt.sow = isow;
            ct = tt.convertToCommonTime();
         }
         
         if( hsod )
         {
            YDSTime tt(ct);
            tt.sod = isod;
            ct = tt.convertToCommonTime();
         }

         if( hsec )
         {
            CivilTime tt(ct);
            tt.second = isec;
            ct = tt.convertToCommonTime();
         }
         
         t = ct;
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }   

} // namespace gpstk
