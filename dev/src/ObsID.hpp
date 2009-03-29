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

#include "Exception.hpp"
#include "SatID.hpp"

namespace gpstk
{
   class ObsID;
   namespace StringUtils
   {
      /// convert this object to a string representation
      std::string asString(const ObsID& id);
      std::string asRinex3ID(const ObsID& id);
   }


   /// stream output for ObsID
   std::ostream& operator<<(std::ostream& s, const ObsID& p);


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
         otLast,      ///< used to extend this list
         otPlaceholder = otLast+1000
      };


      /// The frequency band this obs was collected from.
      enum CarrierBand
      {
         cbUnknown,
         cbL1,   ///< GPS L1, Galileo E2-L1-E1, SBAS L1
         cbL2,   ///< GPS L2
         cbL5,   ///< GPS L5, Galileo E5a, SBAS L5
         cbG1,   ///< Glonass G1
         cbG2,   ///< Glonass G2
         cbE5b,  ///< Galileo E5b
         cbE5ab, ///< Galileo E5a+b
         cbE6,   ///< Galileo E6
         cbL1L2, ///< Combined L1L2 (like an ionosphere free obs)
         cbLast, ///< Used to extend this list
         cbPlaceholder = cbLast+1000
      };


      /// The code used to collect the observation. Each of these should uniquely identify
      /// a code that was correlated against to track the signal.  While the notation
      /// generally follows section 5.1 of RINEX 3, due to ambiguities in that
      /// specification some extensions are made. Note that as concrete specifications
      /// for the codes are released, this list may need to be adjusted. Specifically,
      /// this lists assumes that the same I & Q codes will be used on all three of the
      /// Galileo carriers. If that is not true, more identifers need to be allocated
      enum TrackingCode
      {
         tcUnknown,
         tcCA,      ///< Legacy GPS civil code
         tcP,       ///< Legacy GPS precise code
         tcY,       ///< Encrypted legacy GPS precise code
         tcW,       ///< Encrypted legacy GPS precise code, with codeless Z mode tracking
         tcN,       ///< Encrypted legacy GPS precise code, with squaring codeless tracking
         tcD,       ///< Encrypted legacy GPS precise code, with other codeless tracking
         tcM,       ///< Modernized GPS military unique code
         tcC2M,     ///< Modernized GPS L2 civil M code
         tcC2L,     ///< Modernized GPS L2 civil L code
         tcC2LM,    ///< Modernized GPS L2 civil M+L combined tracking (such as Trimble NetRS, Septrentrio, and ITT)
         tcI5,      ///< Modernized GPS L5 civil in-phase
         tcQ5,      ///< Modernized GPS L5 civil quadrature
         tcIQ5,     ///< Modernized GPS L5 civil I+Q combined tracking

         tcGCA,     ///< Legacy Glonass civil signal
         tcGP,      ///< Legacy Glonass precise signal

         tcA,       ///< Galileo L1 PRS code
         tcB,       ///< Galileo OS/CS/SoL code
         tcC,       ///< Galileo Dataless code
         tcBC,      ///< Galileo B+C combined tracking
         tcABC,     ///< Galileo A+B+C combined tracking
         tcIE5,     ///< Galileo L5 I code
         tcQE5,     ///< Galileo L5 Q code
         tcIQE5,    ///< Galileo L5 I+Q combined tracking
         tcLast,    ///< Used to extend this list
         tcPlaceholder = tcLast+1000
      };

      /// empty constructor, creates an invalid object
      ObsID()
         : type(otUnknown), band(cbUnknown), code(tcUnknown) {};

      /// Explicit constructor
      ObsID(ObservationType ot, CarrierBand cb, TrackingCode tc)
         : type(ot), band(cb), code(tc) {};

      /// Constructor from a Rinex 3 style descriptor. If this string is 3 characters
      /// long, the system is assumed to be GPS. If this string is 4 characters long,
      /// the first character is the system designator as described in the Rinex 3
      /// specification.
      ObsID(const std::string& id) throw(InvalidParameter);
      ObsID(const char* id) throw(InvalidParameter)
      { *this=ObsID(std::string(id));};

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

      /// This returns a representation of this object using the observation
      /// codes described in section 5.1 of the Rinex 3 specification. Note that
      /// this always returns a three character identifier so some information
      /// is lost because some codes are shared between satellite systems.
      std::string asRinex3ID() const;

      /// Convenience output method
      virtual std::ostream& dump(std::ostream& s) const;

      /// Destructor
      virtual ~ObsID() {};

      /// The next three methods are deprecated
      static ObservationType newObservationType(const std::string& s){};
      static CarrierBand newCarrierBand(const std::string& s){};
      static TrackingCode newTrackingCode(const std::string& s){};

      // Extend the standard identifiers with a new Rinex 3 style identifier. If the
      // specified id is already defined, an exception is thrown and the existing
      // definitions are not touched. If not then each character of the specification
      // is examined and the new ones are created. The returned ObsID can then be
      // examined for the assigned values.
      static ObsID newID(const std::string& id,
                         const std::string& desc) throw(InvalidParameter);

      // Note that these are the only data members of objects of this class.
      ObservationType  type;
      CarrierBand      band;
      TrackingCode     code;

      /// These strings are for forming a somewhat verbose description
      static std::map< TrackingCode,    std::string > tcDesc;
      static std::map< CarrierBand,     std::string > cbDesc;
      static std::map< ObservationType, std::string > otDesc;

      /// These strings are used to translate this object to and from a
      /// rinex identifier
      static std::map< char, ObservationType> rinex2ot;
      static std::map< char, CarrierBand> rinex2cb;
      static std::map< char, TrackingCode> rinex2tc;
      static std::map< ObservationType, char > ot2Rinex;
      static std::map< CarrierBand, char > cb2Rinex;
      static std::map< TrackingCode, char> tc2Rinex;

   public:
      class Initializer
      {
      public:
         Initializer();
      };
      
      static Initializer singleton;

   }; // class ObsID

} // namespace gpstk
#endif   // OBSID_HPP
