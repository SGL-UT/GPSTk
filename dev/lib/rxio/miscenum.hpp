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
//  Copyright 2007, The University of Texas at Austin
//
//============================================================================

/**
 * @file miscenum.hpp
 * This file contains miscellaneous enumerations that potentially have
 * global significance (as opposed to being relevant only to one class).
 */

#ifndef MISCENUM_HPP
#define MISCENUM_HPP

#include <string>

namespace gpstk
{
      /// An enumeration of the various Carrier Codes.
   enum CarrierCode
   {
      ccUnknown = 0,
      ccL1 = 1,
      ccL2 = 2,
      ccL5 = 5,
      ccMax
   };

      /// An enumeration of the various Range Codes.
   enum RangeCode 
   {
      rcUnknown  = 0,
      rcCA       = 1,
      rcPcode    = 2,
      rcYcode    = 3,
      rcCodeless = 4,
      rcCM       = 5,
      rcCL       = 6,
      rcMcode1   = 7,
      rcMcode2   = 8, // This should not be used
      rcCMCL     = 9,
      rcMax
   };

      /// An enumeration of the various Navigation Message Codes.
   enum NavCode
   {
      ncUnknown    = 0, ///< Unknown or uninitialized navigation message code.
      ncICD_200_2  = 1, ///< ICD-GPS-200 Data ID No 2.
      ncICD_200_4  = 2, ///< ICD-GPS-200 Data ID No 4.
      ncICD_700_M  = 3, ///< ICD-GPS-700 M-code.
      ncICD_705_L5 = 4, ///< ICD-GPS-705 L5 nav.
      ncMax
   };

      /** An enumeration of the External Frequency Status.  Relevant
       * to receivers using external timing sources. */
   enum ExternalFrequencyStatus
   {
      efsUnknown   = -1,
      efsNotLocked = 0,
      efsLocked    = 1
   };

   namespace StringUtils
   {
         /** Decode Carrier Code to a string.
          * @param cc the Carrier Code to convert.
          * @return the Carrier Code as a string.
          */
      inline std::string asString(const CarrierCode& cc) throw()
      {
         switch(cc)
         {
            case ccL1: return "L1";
            case ccL2: return "L2";
            case ccL5: return "L5";
         }
         return "BadCarrierCode";
      }

         /** Decode Range Code to a string.
          * @param rc the Range Code to convert.
          * @return the Range Code as a string.
          */
      inline std::string asString(const RangeCode& rc) throw()
      {
         switch(rc)
         {
            case rcCA:       return "CA";
            case rcPcode:    return "P";
            case rcYcode:    return "Y";
            case rcCodeless: return "Z";
            case rcCM:       return "CM";
            case rcCL:       return "CL";
            case rcMcode1:   return "M1";
            case rcMcode2:   return "M2";
            case rcCMCL:     return "CMCL";
         }
         return "BadRangeCode";
      }

         /** Decode Navigation Message Code to a string.
          * @param nc the Navigation Code to convert.
          * @return the Navigation Code as a string.
          */
      inline std::string asString(const NavCode& nc) throw()
      {
         switch(nc)
         {
            case ncICD_200_2:  return "ICD-GPS-200 Data ID No. 2";
            case ncICD_700_M:  return "ICD-GPS-700 M-code";
            case ncICD_705_L5: return "ICD-GPS-705 L5 nav";
            case ncICD_200_4:  return "ICD-GPS-200 Data ID No. 4";
         }
         return "BadNavCode";
      }

         /** Decode external frequency status to a string.
          * @param e the external frequency status to convert.
          * @return the Carrier Code as a string.
          */
      inline std::string asString(const ExternalFrequencyStatus& e)
         throw()
      {
         switch(e)
         {
            case efsNotLocked: return "NotLocked";
            case efsLocked:    return "Locked";
         }
         return "BadEFS";
      }
   } // namespace StringUtils

}
#endif // MISCENUM_HPP
