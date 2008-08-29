#pragma ident "$Id$"

/**
 * @file ObsID.hpp
 * gpstk::ObsID - navigation system, receiver, and file specification
 * independent representation of the types of observation data that can
 * be collected.  This class is analogous to the RinexObsType class that
 * is used to represent the observation codes in a RINEX file. It is
 * intended to support at least everything in section 5.1 of the RINEX 3
 * specifications.
 */

#ifndef OBSID_HPP
#define OBSID_HPP

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


#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>


namespace gpstk
{
   class ObsID
   {
   public:
         /// The type of observation.
      enum ObservationType
      {
         otUnknown,
         otRange,     ///< pseudorange, in meters
         otPhase,     ///< accumulated phase, in meters
         otDoppler,   ///< Doppler, in Hz
         otSNR,       ///< Signal strength, in dB-Hz
         otSSI,       ///< Signal Strength Indicator (kinda a rinex thing)
         otLLI,       ///< Loss of Lock Indicator (another rinex thing)
         otTrackLen,  ///< Number of continuous epochs of 'good' tracking
         otLast,      ///< used to extend this...
         otPlaceholder = otLast+1000
      };

         /// The frequency band this obs was collected from.
      enum CarrierBand
      {
         cbUnknown,
         cbL1,   ///< GPS L1, Galileo E2-L1-E1, SBAS L1
         cbL2,   ///< GPS L2
         cbL5,   ///< GPS L5, Galileo E5a, SBAS L5
         cbL1L2, ///< Combined L1L2 (like an ionosphere free obs)
         cbG1,   ///< Glonass G1
         cbG2,   ///< Glonass G2
         cbE5b,  ///< Galileo E5b
         cbE5ab, ///< Galileo E5a+b
         cbE6,   ///< Galileo E6
         cbLast, ///< Used to extend this...
         cbPlaceholder = cbLast+1000
      };

         /// The code used to collect the observation. This generally follows
         /// the attributes in section 5.1 of RINEX 3. Note that these need to
         /// be interpreted in conjunction with the satellite system that
         /// is in the svid variable.
      enum TrackingCode
      {
         tcUnknown,
         tcCA,      ///< Legacy civil code
         tcP,       ///< Legacy precise code
         tcY,       ///< Encrypted legacy precise code
         tcW,       ///< Encrypted legacy precise code, with codeless Z mode tracking
         tcN,       ///< Encrypted legacy precise code, with codeless tracking
         tcM,       ///< Encrypted modernized precise code
         tcC2M,     ///< L2 civil M code
         tcC2L,     ///< L2 civil L code
         tcC2LM,    ///< L2 civil M+L combined tracking (such as Trimble NetRS, Septrentrio, and ITT)
         tcI5,      ///< L5 civil in-phase
         tcQ5,      ///< L5 civil quadrature
         tcIQ5,     ///< L5 civil I+Q combined tracking
         tcA,       ///< Galileo L1 PRS code
         tcB,       ///< Galileo OS/CS/SoL code
         tcC,       ///< Galileo Dataless code
         tcBC,      ///< Galileo B+C combined tracking
         tcABC,     ///< Galileo A+B+C combined tracking
         tcLast,    ///< Used to extend this...
         tcPlaceholder = tcLast+1000
      };

         /// empty constructor, creates an invalid object
      ObsID()
         : type(otUnknown), band(cbUnknown), code(tcUnknown) {};

         /// Explicit constructor
      ObsID(ObservationType ot, CarrierBand cb, TrackingCode tc)
         : type(ot), band(cb), code(tc) {};

         /// Equality requires all fields to be the same
      virtual bool operator==(const ObsID& right) const;

         /// This ordering is somewhat arbitrary but is required to be able
         /// to use an ObsID as an index to a std::map. If an application needs
         /// some other ordering, inherit and override this function.
      virtual bool operator<(const ObsID& right) const;

      bool operator!=(const ObsID& right) const
      { return !(operator==(right)); };

      bool operator>(const ObsID& right) const
      {  return (!operator<(right) && !operator==(right)); };

      bool operator<=(const ObsID& right) const
      { return (operator<(right) || operator==(right)); };

      bool operator>=(const ObsID& right) const
      { return !(operator<(right)); };


         /// Convenience output method
      virtual std::ostream& dump(std::ostream& s) const;

         /// Return true if this is a valid ObsID. Basically just
         /// checks that none of the enums are undefined
      virtual bool isValid() const;

         /// Destructor
      virtual ~ObsID() {};


         /** Static method to add new ObservationType's
          * @param s      Identifying string for the new ObservationType
          */
      static ObservationType newObservationType(const std::string& s);

         /** Static method to add new CarrierBand's
          * @param s      Identifying string for the new CarrierBand
          */
      static CarrierBand newCarrierBand(const std::string& s);

         /** Static method to add new TrackingCode's
          * @param s      Identifying string for the new TrackingCode
          */
      static TrackingCode newTrackingCode(const std::string& s);


      ObservationType  type;
      CarrierBand      band;
      TrackingCode     code;

      static std::map< TrackingCode,    std::string > tcStrings;
      static std::map< CarrierBand,     std::string > cbStrings;
      static std::map< ObservationType, std::string > otStrings;

   public:
      class Initializer
      {
      public:
         Initializer();
      };

      static Initializer singleton;

   }; // class ObsID


   namespace StringUtils
   {
         /// convert this object to a string representation
      std::string asString(const ObsID& p);
   }


      /// stream output for ObsID
   std::ostream& operator<<(std::ostream& s, const ObsID& p);


} // namespace gpstk
#endif   // OBSID_HPP
