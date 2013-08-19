#pragma ident "$Id$"

/**
 * @file TimeSystemCorr.hpp
 * Encapsulate time system corrections, defined by header of RINEX 3 navigation file,
 * including RINEX 2, and used to convert CommonTime between systems.
 */

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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
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

#ifndef GPSTK_TIMESYSTEMCORRECTION_INCLUDE
#define GPSTK_TIMESYSTEMCORRECTION_INCLUDE

#include "GNSSconstants.hpp"
#include "CommonTime.hpp"
#include "GPSWeekSecond.hpp"
#include "CivilTime.hpp"

namespace gpstk {

      /// Time System Corrections as defined in the RINEX version 3 Navigation header.
   class TimeSystemCorrection
   {
   public:
         /// Supported time system correction types, cf. RINEX version 3 spec.
      enum CorrType
      {
         Unknown=0,
         GPUT,    ///< GPS  to UTC using A0, A1
         GAUT,    ///< GAL  to UTC using A0, A1
         SBUT,    ///< SBAS to UTC using A0, A1, incl. provider and UTC ID
         GLUT,    ///< GLO  to UTC using A0 = -TauC , A1 = 0
         GPGA,    ///< GPS  to GAL using A0 = A0G   , A1 = A1G
         GLGP,    ///< GLO  to GPS using A0 = -TauGPS, A1 = 0
         QZGP,    ///< QZS  to GPS using A0, A1
         QZUT,    ///< QZS  to UTC using A0, A1
         BDUT,    ///< BDS  to UTC using A0, A1
         BDGP    ///< BDS  to GPS using A0, A1  // not in RINEX
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

      void fromString(const std::string str)
      {
         std::string STR(gpstk::StringUtils::upperCase(str));
              if(STR == std::string("GPUT")) type = GPUT;
         else if(STR == std::string("GAUT")) type = GAUT;
         else if(STR == std::string("SBUT")) type = SBUT;
         else if(STR == std::string("GLUT")) type = GLUT;
         else if(STR == std::string("GPGA")) type = GPGA;
         else if(STR == std::string("GLGP")) type = GLGP;
         else if(STR == std::string("QZGP")) type = QZGP;
         else if(STR == std::string("QZUT")) type = QZUT;
         else if(STR == std::string("BDUT")) type = BDUT;
         else if(STR == std::string("BDGP")) type = BDGP;
         else {
            Exception e("Unknown TimeSystemCorrection type: " + str);
            GPSTK_THROW(e);
         }
      }

         /// Return readable string version of CorrType
      std::string asString() const
      {
         switch(type) {
            case GPUT: return std::string("GPS to UTC"); break;
            case GAUT: return std::string("GAL to UTC"); break;
            case SBUT: return std::string("SBAS to UTC");
               break;
            case GLUT: return std::string("GLO to UTC"); break;
            case GPGA: return std::string("GPS to GAL"); break;
            case GLGP: return std::string("GLO to GPS"); break;
            case QZGP: return std::string("QZSS to GPS"); break;
            case QZUT: return std::string("QZSS to UTC"); break;
            case BDUT: return std::string("BDS to UTC"); break;
            case BDGP: return std::string("BDS to GPS"); break;
            default:   return std::string("ERROR"); break;
         }
      }

         /// Return 4-char string version of CorrType
      std::string asString4() const
      {
         switch(type) {
            case GPUT: return std::string("GPUT"); break;
            case GAUT: return std::string("GAUT"); break;
            case SBUT: return std::string("SBUT"); break;
            case GLUT: return std::string("GLUT"); break;
            case GPGA: return std::string("GPGA"); break;
            case GLGP: return std::string("GLGP"); break;
            case QZGP: return std::string("QZGP"); break;
            case QZUT: return std::string("QZUT"); break;
            case BDUT: return std::string("BDUT"); break;
            case BDGP: return std::string("BDGP"); break;
            default:   return std::string("ERROR"); break;
         }
      }

         /// dump
      void dump(std::ostream& s) const
      {
         s << "Time system correction for " << asString4() << ": "
            << asString() << std::scientific << std::setprecision(12);
         switch(type) {
            case TimeSystemCorrection::GPUT:
               s << ", A0 = " << A0 << ", A1 = " << A1
                 << ", RefTime = week/sow " << refWeek << "/" << refSOW;
               break;
            case TimeSystemCorrection::GAUT:
               s << ", A0 = " << A0 << ", A1 = " << A1
                 << ", RefTime = week/sow " << refWeek << "/" << refSOW;
               break;
            case TimeSystemCorrection::SBUT:
               s << ", A0 = " << A0 << ", A1 = " << A1
                 << ", RefTime = week/sow " << refWeek << "/" << refSOW
                 << ", provider " << geoProvider << ", UTC ID = " << geoUTCid;
               break;
            case TimeSystemCorrection::GLUT:
               s << ", -TauC = " << A0
                 << ", RefTime = week/sow " << refWeek << "/" << refSOW;
               break;
            case TimeSystemCorrection::GPGA:
               s << ", A0G = " << A0 << ", A1G = " << A1
                 << ", RefTime = week/sow " << refWeek << "/" << refSOW;
               break;
            case TimeSystemCorrection::GLGP:
               s << ", TauGPS = " << A0 << " sec, RefTime = yr/mon/day "
                 << refYr << "/" << refMon << "/" << refDay;
               break;
            case TimeSystemCorrection::QZGP:
               s << ", A0 = " << A0 << ", A1 = " << A1
                 << ", RefTime = week/sow " << refWeek << "/" << refSOW;
               break;
            case TimeSystemCorrection::QZUT:
               s << ", A0 = " << A0 << ", A1 = " << A1
                 << ", RefTime = week/sow " << refWeek << "/" << refSOW;
               break;
            case TimeSystemCorrection::BDUT:
               s << ", A0 = " << A0 << ", A1 = " << A1
                 << ", RefTime = week/sow " << refWeek << "/" << refSOW;
               break;
            case TimeSystemCorrection::BDGP:
               s << ", A0 = " << A0 << ", A1 = " << A1
                 << ", RefTime = week/sow " << refWeek << "/" << refSOW;
               break;
            default:
               break;
         }
         //s << std::endl;
      }

         /// Equal operator
      inline bool operator==(const TimeSystemCorrection& tc)
      { return tc.type == type; }

         /// Less than operator - required for map.find()
      inline bool operator<(const TimeSystemCorrection& tc)
      { return tc.type < type; }

      /// Given a time in one system (fromTime), compute the correction between
      /// the given system and the target system (toTime.system),
      /// and apply it to fromTime, placing the result in the target toTime.
      /// i.e. toTime = (const)fromTime + correction(from => to).
      ///
      /// In addition to TimeSystemCorr, must account for leap seconds:
      /// Let dtLS == getLeapSeconds()-19
      /// NB. GPS = TAI-19sec and so GPS-UTC = getLeapSeconds()-19 == dtLS.
      /// NB. GLO = UTC = GPS - dtLS
      /// NB. GLO is actually UTC(SU) Moscow
      /// NB. GAL = GPS = UTC + dtLS
      /// NB. BDT = GPS - 15 = UTC + dtLS - 15
      /// NB. BDT is actually UTC(NTSC) China
      ///
      /// @param CommonTime fromTime, on input is a time in the given system,
      ///     unchanged on output.
      /// @param CommonTime toTime, on input this defines the target system and
      ///     the value is ignored; on output the value is set to "given time plus
      ///     correction" and the system is left as the target system.
      /// @return true if successful, false if the input systems are not those
      ///     that can be converted by this TimeSystemCorr object.
      /// @throw Exception if this object has not been defined.
      bool convertSystem(const CommonTime& fromTime, CommonTime& toTime) const
      {
         CivilTime ct;
         double dtLS;
         switch(type) {
            case GPUT:
               // NB. GPS = TAI-19sec and so GPS-UTC = getLeapSeconds()-19 == dtLS.
               ct = CivilTime(fromTime);
               dtLS = TimeSystem::getLeapSeconds(ct.year, ct.month, ct.day) - 19.;
               // ------------------------------------------------- GPS => UTC
               if(fromTime.getTimeSystem() == TimeSystem::GPS &&
                    toTime.getTimeSystem() == TimeSystem::UTC)
               {
                  // dt = fromTime - refTime
                  GPSWeekSecond gpsws(refWeek,refSOW);
                  CommonTime refTime(gpsws.convertToCommonTime());
                  refTime.setTimeSystem(TimeSystem::GPS);
                  double dt(fromTime - refTime);

                  toTime = fromTime - dtLS + A0 + A1*dt;
                  toTime.setTimeSystem(TimeSystem::UTC);

                  return true;
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

                  toTime = fromTime + dtLS - A0 - A1*dt;
                  toTime.setTimeSystem(TimeSystem::GPS);

                  return true;
               }
               break;

            case GAUT:
               // NB. GAL = UTC + dtLS
               ct = CivilTime(fromTime);
               dtLS = TimeSystem::getLeapSeconds(ct.year, ct.month, ct.day) - 19.;
               // ------------------------------------------------- GAL => UTC
               if(fromTime.getTimeSystem() == TimeSystem::GAL &&
                    toTime.getTimeSystem() == TimeSystem::UTC)
               {
                  // dt = fromTime - refTime
                  GPSWeekSecond gpsws(refWeek,refSOW);
                  CommonTime refTime(gpsws.convertToCommonTime());
                  refTime.setTimeSystem(TimeSystem::GAL);
                  double dt(fromTime - refTime);

                  toTime = fromTime - dtLS + A0 + A1*dt;
                  toTime.setTimeSystem(TimeSystem::UTC);

                  return true;
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

                  toTime = fromTime + dtLS - A0 - A1*dt;
                  toTime.setTimeSystem(TimeSystem::GAL);

                  return true;
               }
               break;

            case SBUT:
               // ------------------------------------------------- SBAS => UTC
               if(fromTime.getTimeSystem() == TimeSystem::UTC &&
                    toTime.getTimeSystem() == TimeSystem::UTC)
               {
                  // TBD
                  // depends on provider and UT ID
                  Exception e("TimeSystemCorr SBAS <=> UTC has not been implemented");
                  GPSTK_THROW(e);
               }
               break;

            case GLUT:
               // ------------------------------------------------- GLO => UTC
               if(fromTime.getTimeSystem() == TimeSystem::GLO &&
                    toTime.getTimeSystem() == TimeSystem::UTC)
               {
                  toTime = fromTime + A0;
                  toTime.setTimeSystem(TimeSystem::UTC);
                  return true;
               }
               // ------------------------------------------------- UTC => GLO
               else if(fromTime.getTimeSystem() == TimeSystem::UTC &&
                         toTime.getTimeSystem() == TimeSystem::GLO)
               {
                  toTime = fromTime - A0;
                  toTime.setTimeSystem(TimeSystem::GLO);
                  return true;
               }
               break;

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

                  return true;
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

                  return true;
               }
               break;

            case GLGP:
               // NB. GLO = UTC = GPS - dtLS
               ct = CivilTime(fromTime);
               dtLS = TimeSystem::getLeapSeconds(ct.year, ct.month, ct.day) - 19.;
               // ------------------------------------------------- GPS => GLO
               if(fromTime.getTimeSystem() == TimeSystem::GPS &&
                    toTime.getTimeSystem() == TimeSystem::GLO)
               {
                  toTime = fromTime - dtLS + A0;
                  toTime.setTimeSystem(TimeSystem::GLO);
                  return true;
               }
               // ------------------------------------------------- GLO => GPS
               else if(fromTime.getTimeSystem() == TimeSystem::GLO &&
                         toTime.getTimeSystem() == TimeSystem::GPS)
               {
                  toTime = fromTime + dtLS - A0;
                  toTime.setTimeSystem(TimeSystem::GPS);
                  return true;
               }
               break;

            case QZGP:
               // ------------------------------------------------- GPS => QZS
               if(fromTime.getTimeSystem() == TimeSystem::GPS &&
                    toTime.getTimeSystem() == TimeSystem::QZS)
               {
                  toTime = fromTime;
                  toTime.setTimeSystem(TimeSystem::QZS);

                  return true;
               }
               // ------------------------------------------------- UTC => GPS
               else if(fromTime.getTimeSystem() == TimeSystem::UTC &&
                         toTime.getTimeSystem() == TimeSystem::GPS)
               {
                  toTime = fromTime;
                  toTime.setTimeSystem(TimeSystem::GPS);

                  return true;
               }
               break;

            case QZUT:
               // ------------------------------------------------- QZS => UTC
               if(fromTime.getTimeSystem() == TimeSystem::QZS &&
                    toTime.getTimeSystem() == TimeSystem::UTC)
               {
                  // dt = fromTime - refTime
                  GPSWeekSecond gpsws(refWeek,refSOW);
                  CommonTime refTime(gpsws.convertToCommonTime());
                  refTime.setTimeSystem(TimeSystem::QZS);
                  double dt(fromTime - refTime);

                  toTime = fromTime + A0 + A1*dt;
                  toTime.setTimeSystem(TimeSystem::UTC);

                  return true;
               }
               // ------------------------------------------------- UTC => QZS
               else if(fromTime.getTimeSystem() == TimeSystem::UTC &&
                         toTime.getTimeSystem() == TimeSystem::QZS)
               {
                  // dt = fromTime - refTime
                  GPSWeekSecond gpsws(refWeek,refSOW);
                  CommonTime refTime(gpsws.convertToCommonTime());
                  refTime.setTimeSystem(TimeSystem::UTC);
                  double dt(fromTime - refTime);

                  toTime = fromTime - A0 - A1*dt;
                  toTime.setTimeSystem(TimeSystem::QZS);

                  return true;
               }
               break;

            case BDUT:
               // NB. BDT = GPS - 15 = UTC + dtLS - 15
               ct = CivilTime(fromTime);
               dtLS = TimeSystem::getLeapSeconds(ct.year, ct.month, ct.day) - 19.;
               // ------------------------------------------------- BDS => UTC
               if(fromTime.getTimeSystem() == TimeSystem::BDS &&
                    toTime.getTimeSystem() == TimeSystem::UTC)
               {
                  toTime = fromTime - dtLS + 15. + A0 - A1;
                  toTime.setTimeSystem(TimeSystem::UTC);
                  return true;
               }
               // ------------------------------------------------- UTC => BDS
               else if(fromTime.getTimeSystem() == TimeSystem::UTC &&
                         toTime.getTimeSystem() == TimeSystem::BDS)
               {
                  toTime = fromTime + dtLS - 15. - A0 + A1;
                  toTime.setTimeSystem(TimeSystem::BDS);
                  return true;
               }
               break;

            case BDGP:
               // NB. BDT = GPS - 15 = UTC + dtLS - 15
               //ct = CivilTime(fromTime);
               //dtLS = TimeSystem::getLeapSeconds(ct.year, ct.month, ct.day) - 19.;
               // ------------------------------------------------- GPS => BDS
               if(fromTime.getTimeSystem() == TimeSystem::GPS &&
                    toTime.getTimeSystem() == TimeSystem::BDS)
               {
                  toTime = fromTime - 15. + A0;
                  toTime.setTimeSystem(TimeSystem::BDS);
                  return true;
               }
               // ------------------------------------------------- BDS => GPS
               else if(fromTime.getTimeSystem() == TimeSystem::BDS &&
                         toTime.getTimeSystem() == TimeSystem::GPS)
               {
                  toTime = fromTime + 15. - A0;
                  toTime.setTimeSystem(TimeSystem::GPS);
                  return true;
               }
               break;

            default:
               Exception e("TimeSystemCorrection is not defined.");
               GPSTK_THROW(e);
               break;
         }

         //Exception e(string("Cannot convert time systems: input is ")
         //            + fromTime.getTimeSystem().asString() + string(" => ")
         //            + toTime.getTimeSystem().asString()
         //            + string(", but conversion object is ") + asString());
         //GPSTK_THROW(e);
         return false;
      }

   }; // End of class 'TimeSystemCorrection'

};    // end namespace

#endif // GPSTK_TIMESYSTEMCORRECTION_INCLUDE
