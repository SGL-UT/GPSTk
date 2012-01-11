#pragma ident "$Id$"

/**
 * @file TimeSystemCorr.hpp
 * Encapsulate time system corrections, defined by header of RINEX 3 navigation file,
 * including RINEX 2, and used to convert CommonTime between systems.
 */

#ifndef GPSTK_TIMESYSTEMCORRECTION_INCLUDE
#define GPSTK_TIMESYSTEMCORRECTION_INCLUDE

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

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

#include "CommonTime.hpp"
#include "GPSWeekSecond.hpp"

namespace gpstk {

      /// Time System Corrections
   class TimeSystemCorrection
   {
   public:
         /// Supported time system correction types
      enum CorrType
      {
         Unknown=0,
         GPUT,    ///< GPS  to UTC using A0, A1
         GAUT,    ///< GAL  to UTC using A0, A1
         SBUT,    ///< SBAS to UTC using A0, A1, incl. provider and UTC ID
         GLUT,    ///< GLO  to UTC using A0 = -TauC , A1 = 0
         GPGA,    ///< GPS  to GAL using A0 = A0G   , A1 = A1G
         GLGP     ///< GLO  to GPS using A0 = -TauGPS, A1 = 0
      };

         //// Member data
      CorrType type;
      double A0, A1;
      long refWeek,refSOW;       ///< reference time for polynominal (week,sow)
      long refYr,refMon,refDay;  ///< reference time (yr,mon,day) for RINEX ver 2 GLO
      std::string geoProvider;   ///< string 'EGNOS' 'WAAS' or 'MSAS'
      int geoUTCid;              ///< UTC Identifier [0 unknown, 1=UTC(NIST),
                                 ///<  2=UTC(USNO), 3=UTC(SU), 4=UTC(BIPM),
                                 ///<  5=UTC(Europe), 6=UTC(CRL)]

         /// Empty constructor
      TimeSystemCorrection() : type(Unknown) { }

         /// Constructor from string
      TimeSystemCorrection(std::string str) { this->fromString(str); }

         /// Return readable string version of CorrType
      std::string asString() const throw()
      {
         switch(type) {
            case GPUT: return std::string("GPS to UTC (A0,A1)"); break;
            case GAUT: return std::string("GAL to UTC (A0,A1)"); break;
            case SBUT: return std::string("SBAS to UTC (A0, A1, provider, UTC ID)");
               break;
            case GLUT: return std::string("GLO to UTC (TauC)"); break;
            case GPGA: return std::string("GPS to GAL (A0G,A1G)"); break;
            case GLGP: return std::string("GLO to GPS (TauGPS)"); break;
         }
      }

         /// Return 4-char string version of CorrType
      std::string asString4() const throw()
      {
         switch(type) {
            case GPUT: return std::string("GPUT"); break;
            case GAUT: return std::string("GAUT"); break;
            case SBUT: return std::string("SBUT"); break;
            case GLUT: return std::string("GLUT"); break;
            case GPGA: return std::string("GPGA"); break;
            case GLGP: return std::string("GLGP"); break;
         }
      }


      void fromString(const std::string str) throw(Exception)
      {
         std::string STR(gpstk::StringUtils::upperCase(str));
              if(STR == std::string("GPUT")) type = GPUT;
         else if(STR == std::string("GAUT")) type = GAUT;
         else if(STR == std::string("SBUT")) type = SBUT;
         else if(STR == std::string("GLUT")) type = GLUT;
         else if(STR == std::string("GPGA")) type = GPGA;
         else if(STR == std::string("GLGP")) type = GLGP;
         else {
            Exception e("Unknown TimeSystemCorrection type: " + str);
            GPSTK_THROW(e);
         }
      }

         /// Equal operator
      inline bool operator==(const TimeSystemCorrection& tc)
      { return tc.type == type; }

         /// Less than operator - required for map.find()
      inline bool operator<(const TimeSystemCorrection& tc)
      { return tc.type < type; }

         /// Compute the correction from the time system that is given (fromTime)
         /// to the time system of the target (toTime), and
         /// apply it to the given time (fromTime) to yield the target time (toTime);
         /// i.e. toTime = fromTime + correction(from => to).
         /// @param fromTime CommonTime, on input is a time in the "from" system,
         ///     unchanged on output.
         /// @param toTime CommonTime, on input is in the "to" system, on output is
         ///     set to "fromTime" plus the correction, in the "to" system.
         /// @throw Exception of this object has not been defined, or if conversion is
         ///      not available (this object is not a conversion "from" => "to").
      void convertSystem(const CommonTime& fromTime, CommonTime& toTime) const
         throw(Exception)
      {
         switch(type) {
            case GPUT:
               // ------------------------------------------------- GPS => UTC
               if(fromTime.getTimeSystem() == TimeSystem::GPS &&
                    toTime.getTimeSystem() == TimeSystem::UTC)
               {
                  // dt = fromTime - refTime
                  GPSWeekSecond gpsws(refWeek,refSOW);
                  CommonTime refTime(gpsws.convertToCommonTime());
                  refTime.setTimeSystem(TimeSystem::GPS);
                  double dt(fromTime - refTime);

                  toTime = fromTime + A0 + A1*dt;
                  toTime.setTimeSystem(TimeSystem::UTC);

                  return;
               }
               // ------------------------------------------------- UTC => GPS
               else if(fromTime.getTimeSystem() == TimeSystem::UTC &&
                         toTime.getTimeSystem() == TimeSystem::GPS)
               {
                  // dt = fromTime - refTime
                  GPSWeekSecond gpsws(refWeek,refSOW);
                  CommonTime refTime(gpsws.convertToCommonTime());
                  refTime.setTimeSystem(TimeSystem::UTC);
                  double dt(fromTime - refTime);

                  toTime = fromTime - A0 - A1*dt;
                  toTime.setTimeSystem(TimeSystem::GPS);

                  return;
               }

            case GAUT:
               // ------------------------------------------------- GAL => UTC
               if(fromTime.getTimeSystem() == TimeSystem::GAL &&
                    toTime.getTimeSystem() == TimeSystem::UTC)
               {
                  // dt = fromTime - refTime
                  GPSWeekSecond gpsws(refWeek,refSOW);
                  CommonTime refTime(gpsws.convertToCommonTime());
                  refTime.setTimeSystem(TimeSystem::GAL);
                  double dt(fromTime - refTime);

                  toTime = fromTime + A0 + A1*dt;
                  toTime.setTimeSystem(TimeSystem::UTC);

                  return;
               }
               // ------------------------------------------------- UTC => GAL
               else if(fromTime.getTimeSystem() == TimeSystem::UTC &&
                         toTime.getTimeSystem() == TimeSystem::GAL)
               {
                  // TBD
                  // dt = fromTime - refTime
                  GPSWeekSecond gpsws(refWeek,refSOW);
                  CommonTime refTime(gpsws.convertToCommonTime());
                  refTime.setTimeSystem(TimeSystem::UTC);
                  double dt(fromTime - refTime);

                  toTime = fromTime - A0 - A1*dt;
                  toTime.setTimeSystem(TimeSystem::GAL);

                  return;
               }

            case SBUT:
               // ------------------------------------------------- SBAS => UTC
               if(fromTime.getTimeSystem() == TimeSystem::UTC &&
                    toTime.getTimeSystem() == TimeSystem::UTC)
               {
                  // TBD
                  // depends on provider and UT ID
                  // make it throw until implemented  return;
               }

            case GLUT:
               // ------------------------------------------------- GLO => UTC
               if(fromTime.getTimeSystem() == TimeSystem::GLO &&
                    toTime.getTimeSystem() == TimeSystem::UTC)
               {
                  toTime = fromTime + A0;
                  toTime.setTimeSystem(TimeSystem::UTC);
                  return;
               }
               // ------------------------------------------------- UTC => GLO
               else if(fromTime.getTimeSystem() == TimeSystem::UTC &&
                         toTime.getTimeSystem() == TimeSystem::GLO)
               {
                  toTime = fromTime - A0;
                  toTime.setTimeSystem(TimeSystem::GLO);
                  return;
               }

            case GPGA:
               // ------------------------------------------------- GPS => GAL
               if(fromTime.getTimeSystem() == TimeSystem::GPS &&
                    toTime.getTimeSystem() == TimeSystem::GAL)
               {
                  // dt = fromTime - refTime
                  GPSWeekSecond gpsws(refWeek,refSOW);
                  CommonTime refTime(gpsws.convertToCommonTime());
                  refTime.setTimeSystem(TimeSystem::GPS);
                  double dt(fromTime - refTime);

                  toTime = fromTime + A0 + A1*dt;
                  toTime.setTimeSystem(TimeSystem::GAL);

                  return;
               }
               // ------------------------------------------------- GAL => GPS
               else if(fromTime.getTimeSystem() == TimeSystem::GAL &&
                         toTime.getTimeSystem() == TimeSystem::GPS)
               {
                  // dt = fromTime - refTime
                  GPSWeekSecond gpsws(refWeek,refSOW);
                  CommonTime refTime(gpsws.convertToCommonTime());
                  refTime.setTimeSystem(TimeSystem::GAL);
                  double dt(fromTime - refTime);

                  toTime = fromTime - A0 - A1*dt;
                  toTime.setTimeSystem(TimeSystem::GPS);

                  return;
               }

            case GLGP:
               // ------------------------------------------------- GPS => GLO
               if(fromTime.getTimeSystem() == TimeSystem::GPS &&
                    toTime.getTimeSystem() == TimeSystem::GLO)
               {
                  toTime = fromTime + A0;
                  toTime.setTimeSystem(TimeSystem::GLO);
                  return;
               }
               // ------------------------------------------------- GLO => GPS
               else if(fromTime.getTimeSystem() == TimeSystem::GLO &&
                         toTime.getTimeSystem() == TimeSystem::GPS)
               {
                  toTime = fromTime - A0;
                  toTime.setTimeSystem(TimeSystem::GPS);
                  return;
               }

            default:
               Exception e("TimeSystemCorrection is not defined.");
               GPSTK_THROW(e);
         }

         Exception e(string("Cannot convert time systems: input is ")
                     + fromTime.getTimeSystem().asString() + string(" => ")
                     + toTime.getTimeSystem().asString()
                     + string(", but conversion object is ") + asString());
         GPSTK_THROW(e);
      }

   }; // End of class 'TimeSystemCorrection'

};    // end namespace

#endif // GPSTK_TIMESYSTEMCORRECTION_INCLUDE
