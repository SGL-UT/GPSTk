//============================================================================
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

/**
 * @file RinexDatum.hpp
 * Define a class for containing data points from RINEX OBS data.
 */

#ifndef RINEXDATUM_HPP
#define RINEXDATUM_HPP

#include <string>

namespace gpstk
{
      /// @ingroup FileHandling
      //@{

      /// Storage for single RINEX OBS data measurements
   class RinexDatum
   {
   public:
         /// Initialize data to default values
      RinexDatum();

         /** Parse a RINEX OBS datum string into data members
          * @param[in] str a RINEX-formatted datum, must be 16
          *   characters in length.
          * @throw AssertionFailure if str.length() != 16 */
      RinexDatum(const std::string& str);

         /** Parse a RINEX OBS datum string into data members
          * @param[in] str a RINEX-formatted datum, must be 16
          *   characters in length.
          * @throw AssertionFailure if str.length() != 16 */
      void fromString(const std::string& str);

         /// Turn this datum into a RINEX OBS formatted string
      std::string asString() const;

      double data;    ///< The actual data point.
      bool dataBlank; ///< True if the data is blank in the file
      short lli;      ///< See the RINEX Spec. for an explanation.
      bool lliBlank;  ///< True if the lli is blank in the file
      short ssi;      ///< See the RINEX Spec. for an explanation.
      bool ssiBlank;  ///< True if the ssi is blank in the file
   };

      //@}

} // namespace gpstk

#endif // RINEXDATUM_HPP
