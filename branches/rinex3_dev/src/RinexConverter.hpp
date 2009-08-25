#pragma ident "$Id: RinexConverter.hpp 2009-08-25 17:32:00 tvarney $"

//============================================================================//
//                                                                            //
//  This file is part of GPSTk, the GPS Toolkit.                              //
//                                                                            //
//  The GPSTk is free software; you can redistribute it and/or modify it      //
//  under the terms of the GNU Lesser General Public License as published by  //
//  the Free Software Foundation; either version 2.1 of the License, or any   //
//  later version.                                                            //
//                                                                            //
//  The GPSTk is distributed in the hope that it will be useful, but WITHOUT  //
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     //
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public      //
//  License for more details.                                                 //
//                                                                            //
//  You should have received a copy of the GNU Lesser General Public  License //
//  along with GPSTk; if not, write to the Free Software Foundation, Inc.,    //
//  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA                   //
//                                                                            //
//  Copyright 2009, The University of Texas at Austin                         //
//                                                                            //
//============================================================================//

/**
 * @file RinexConverter.hpp
 * Provide functions to convert RINEX 2.11 data types to the new RINEX 3.0 data
 * types. These functions produce converted products, and as such are not of the
 * same quality as those produced by actually filling the correct data structure
 * from the appropriate stream.
 */

#ifndef RINEX_TRANSLATOR_HPP
#define RINEX_TRANSLATOR_HPP

#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "Rinex3ObsData.hpp"
#include "Rinex3ObsHeader.hpp"

namespace gpstk
{
	
	/**
	 * This class provides functionallity to convert between the two working
	 * versions of the RINEX standard used by the toolkit, RINEX 2.11 and RINEX
	 * 3.0. In addition to this, several variables can be set or changed to
	 * change the default behavior of this class when converting between the two
	 * versions.
	 */
   class RinexConverter
   {
      public:
//============================================================================//
//                           Public Member Functions                          //
//============================================================================//
      /**
       * Takes data stored in a RINEX 2.11 observation data structure and stores
       * it in the equivalent RINEX 3.0 data structure.
       * 
       * @param dest The destination RINEX 3.0 data structure for the conversion.
       * @param src The source RINEX 2.11 data structure for the conversion.
       * 
       * @return If the conversion succeeded.
       */
      static bool convertToRinex3(Rinex3ObsData& dest, const RinexObsData& src,
                                    const RinexObsHeader& srcHead);
      /**
       * Takes data stored in a RINEX 2.11 observation header data structure and
       * stores it in the equivalent RINEX 3.0 data structure.
       * 
       * @param dest The destination RINEX 3.0 data structure for the conversion.
       * @param src The source RINEX 2.11 data structure for the conversion.
       * 
       * @return If the conversion succeeded.
       */
      static bool convertToRinex3(Rinex3ObsHeader& dest,
                                    const RinexObsHeader& src);
      
      /**
       * Takes data stored in the RINEX 3 observation data and stores it in the
       * RINEX 2 observation data passed to this function, using the header to
       * determine what information is present.
       * 
       * @param dest The destination RINEX 2.11 data structure for the conversion.
       * @param src The source RINEX 3.0 data structure for the conversion.
       * 
       * @return If the conversion succeeded.
       */
      static bool convertFromRinex3(RinexObsData& dest,
                                    const Rinex3ObsData& src,
                                    Rinex3ObsHeader& srcHead);
      /**
       * Takes data stored in the RINEX 3 observation header and stores it in
       * the RINEX 2 observation header passed to this function.
       * 
       * @param dest The destination RINEX 2.11 header for the conversion.
       * @param src The source RINEX 3.0 header for the conversion.
       * 
       * @return If the conversion succeeded.
       */
      static bool convertFromRinex3(RinexObsHeader& dest,
                                    Rinex3ObsHeader& src);
      
      /**
       * Resets the static options in this class to their defaults.
       */
      static void reset();
      
      /**
       * Checks the given obs type against an internal list of valid codes for
       * the GPS GNSS.
       * 
       * @param type The observation type to validate.
       * 
       * @return If the given observation type is a valid code for the GPS GNSS.
       */
      static bool validGPSCode(const RinexObsHeader::RinexObsType& type);
      /**
       * Checks the given obs type against an internal list of valid codes for
       * the Galileo GNSS.
       * 
       * @param type The observation type to validate.
       * 
       * @return If the given obs type is a valid code for the Galileo GNSS.
       */
      static bool validGalileoCode(const RinexObsHeader::RinexObsType& type);
      /**
       * Checks the given obs type against an internal list of valid codes for
       * the GLONASS GNSS.
       * 
       * @param type The observation type to validate.
       * 
       * @return If the given obs type is a valid code for the GLONASS GNSS.
       */
      static bool validGlonassCode(const RinexObsHeader::RinexObsType& type);
      /**
       * Checks teh given obs type against an internal list of valid codes for
       * the SBAS Geosynchronous GNSS.
       * 
       * @param type The observation type to validate.
       * 
       * @return If the given obs type is a valid code for the SBAS GNSS.
       */
      static bool validGEOCode(const RinexObsHeader::RinexObsType& type);
      /**
       * 
       */
      static int getR3IDUniqueValue(const std::string str);
//============================================================================//
//                             Public Data Members                            //
//============================================================================//
      /**
       * A mapping between the 2 character RINEX 2.11 observation codes and
       * their corresponding 3 character RINEX 3.0 observation codes.
       */
      typedef std::map<const char*, const char*> CodeMap;
      
         ///The mapping between RINEX 2.11 codes and RINEX 3.0 codes, can be set
         ///by the user at runtime to modify behavior of the conversion functions.
      static CodeMap obsMap;
      
         ///Fills all optional fields that have data.
      static bool fillOptionalFields;
         ///Keep any comments that were in the original file.
      static bool keepComments;
      
         ///The marker type to use for 2.11 -> 3.0 conversions.
      static std::string markerType;
      
//============================================================================//
      
      private:
//============================================================================//
//                          Private Member Functions                          //
//============================================================================//
      /**
       * Initalizes the observation map to contain default values.
       */
      static void initialize();
      /**
       * Sorts the values in the mapObsTypes member and sorts them into a list
       * such that there is a 1 to 1 correlation between all obs and their RINEX
       * 2.11 translation. The resulting list is stored in the obsTypeList
       * member of the header.
       * 
       * @param header The RINEX 3.0 observation header to work on.
       */
      static void sortRinex3ObsTypes(Rinex3ObsHeader& header);
      
         ///If the initialize() method has been called yet in this class.
      static bool initialized;
   };
}

#endif
