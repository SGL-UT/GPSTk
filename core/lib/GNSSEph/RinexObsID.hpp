//==============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#ifndef GPSTK_RINEXOBSID_HPP
#define GPSTK_RINEXOBSID_HPP

/**
 * @file RinexObsID.hpp
 * gpstk::RinexObsID - A specialization of ObsID that has only RINEX-specific
 * observation identifiers.
 */

#include "ObsID.hpp"
#include "RinexObsHeader.hpp"

namespace gpstk
{
      /// @todo determine if this really belongs with the RINEX files

      /// @ingroup FileHandling
      //@{

      /** Determine if the given ObsID is valid. If the input string
       * is 3 characters long, the system is unknown and the string is
       * tested with all RINEX If this string is 4 characters long,
       * the first character is the system character as described in
       * the Rinex 3 specification. */
   bool isValidRinexObsID(const std::string& id);

      /// Determine if the given ObsID is valid, for the given system
   bool isValidRinexObsID(const std::string& id, const char syschar);

      /** class RinexObsID is the set of ObsID's which are valid in
      * RINEX 3.03.  This class simply limits possible values of
      * ObsID, plus a little functionality.
      *
      * @note it depends critically on map validRinexTrackingCodes,
      * from class ObsID.
      *
      * This map[sys][freq] = valid codes; e.g.  valid['G'][1]="CSLXPWYMN"
      * Only exception is there is no pseudorange (C) on GPS L1/L2 N (codeless)
      * RINEX 3.03 document, section 5.1
      * sys  freq  code                    ObsID = sys+type+freq+code
      * GPS   L1   C,S,L,X,P,W,Y,M,N       G CLDS 1 CSLXPWYMN (but not C1N)
      *       L2   C,D,S,L,X,P,W,Y,M,N     G CLDS 2 CDSLXPWYMN (but not C2N)
      *       L5   I,Q,X                   G CLDS 5 IQX
      * GLO   G1   C,P                     R CLDS 1 CP
      *       G2   C,P                     R CLDS 2 CP
      * GAL   E1   A,B,C,X,Z               E CLDS 1 ABCXZ
      *       E5a  I,Q,X                   E CLDS 5 IQX
      *       E5b  I,Q,X                   E CLDS 7 IQX
      *       E5ab I,Q,X                   E CLDS 8 IQX
      *       E6   A,B,C,X,Z               E CLDS 6 ABCXZ
      * SBAS  L1   C                       S CLDS 1 C
      *       L5   I,Q,X                   S CLDS 5 IQX
      * BDS   B1   I,Q,X                   C CLDS 1 IQX
      *       B2   I,Q,X                   C CLDS 7 IQX
      *       B3   I,Q,X                   C CLDS 6 IQX
      * QZSS  L1   C,S,L,X,Z               J CLDS 1 CSLXZ
      *       L2   S,L,X                   J CLDS 2 SLX
      *       L5   I,Q,X                   J CLDS 5 IQX
      *       L6   S,L,X                   J CLDS 6 SLX
      * IRNSS L5   A,B,C,X                 I CLDS 5 ABCX
      *       L9   A,B,C,X                 I CLDS 9 ABCX
      */
   class RinexObsID : public ObsID
   {
   public:
         /// empty constructor, creates an invalid object
      RinexObsID() : ObsID() {};

         /// Explicit constructior
      RinexObsID(ObservationType ot, CarrierBand cb, TrackingCode tc)
            : ObsID(ot, cb, tc) {};
      
         /** Construct this object from the string specifier.
          * @param[in] strID The RINEX observation identifier to
          *   decode.  This must be a RINEX 3 ID, three or four
          *   characters in length.  Three character obs codes are
          *   assumed to be from GPS.  Four character obs codes use
          *   the first character for the system.
          * @param[in] version The RINEX version of the obs ID in
          *   strID.  This is used for oddball special cases like CC1*
          *   in RINEX 3.02, to make sure that the codes are properly
          *   interpreted.  When reading the obs ID from a RINEX
          *   header, one should use the header version here.  When
          *   interpreting command-line options or other contexts
          *   where a RINEX version is not specified, use
          *   Rinex3ObsBase::currentVersion.
          * @throw InvalidParameter
          */
      explicit RinexObsID(const std::string& strID, double version);

         /** Construct this object from the C-style string specifier.
          * @param[in] strID The RINEX observation identifier to
          *   decode.  This must be a RINEX 3 ID, three or four
          *   characters in length.  Three character obs codes are
          *   assumed to be from GPS.  Four character obs codes use
          *   the first character for the system.
          * @param[in] version The RINEX version of the obs ID in
          *   strID.  This is used for oddball special cases like CC1*
          *   in RINEX 3.02, to make sure that the codes are properly
          *   interpreted.  When reading the obs ID from a RINEX
          *   header, one should use the header version here.  When
          *   interpreting command-line options or other contexts
          *   where a RINEX version is not specified, use
          *   Rinex3ObsBase::currentVersion.
          * @throw InvalidParameter
          */
      explicit RinexObsID(const char* id, double version)
            : RinexObsID(std::string(id), version)
      {}

         /** Constructor from ObsID.
          * @throw InvalidParameter
          */
      RinexObsID(const ObsID& oid)
      {
         type=oid.type; band=oid.band; code=oid.code;
         std::string str(this->asString());
         if(!isValidRinexObsID(str)) {
            InvalidParameter ip("Invalid RinexObsID: " + str);
            GPSTK_THROW(ip);
         }
      }

         /** a conversion constructor, giving a fixed one-way mapping
          * from RINEX ver 2 obstypes to RinexObsIDs.
          * L1 -> L1P; P1 -> C1P; C1 -> C1C; S1 -> S1P; D1 -> D1P
          * L2 -> L2P; P2 -> C2P; C2 -> C2X; S2 -> S2P; D2 -> D2P
          * @deprecated RinexHeader does the conversion with more care
          * for tracking code, and does not use this routine. */
      RinexObsID(const RinexObsType& rot);

         /** This returns a representation of this object using the
          * observation codes described in section 5.1 of the Rinex 3
          * specification. Note that this always returns a three
          * character identifier so some information is lost because
          * some codes are shared between satellite systems.
          * @note This method defaults to using the obs ID
          *   representation defined in the RINEX version set in
          *   rinexVersion.
          * @see asString(double)
          * @see ObsID::rinexVersion */
      std::string asString() const
      { return asString(rinexVersion); }

         /** This returns a representation of this object using the
          * observation codes described in section 5.1 of the Rinex 3
          * specification. Note that this always returns a three
          * character identifier so some information is lost because
          * some codes are shared between satellite systems.
          * @param[in] version The RINEX format version to use when
          *   generating the string (e.g. 3.02 has different codes for
          *   BDS than other versions). */
      std::string asString(double version) const;

         /** This is used by Rinex3ObsHeader::getObsIndex to determine
          * a pseudo-equality that takes into account handling of
          * pseudo-observables like ionospheric delay and channel
          * numbers.
          * @return true if this and right are the same, ignoring
          *   tracking codes for ionospheric delay, and tracking codes
          *   and band for channel numbers. */
      bool equalIndex(const RinexObsID& right) const;

         // see ObsID for definition of validRinexTrackingCodes and
         // validRinexSystems.
         //static std::map<char, std::map<char, std::string> > validRinexTrackingCodes;

         // This string contains the system characters for all valid
         // RINEX systems.
         //static std::string validRinexSystems;

         /**
          * @throw Exception
          */
      static std::ostream& dumpCheck(std::ostream& s);

   }; // end class RinexObsID

      //@}

} // namespace gpstk

#endif
