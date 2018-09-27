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
#include "OrbAlm.hpp"

namespace gpstk
{   
   OrbAlm::OrbAlm(): OrbElemBase()
   { }

   void OrbAlm::dumpHeader(std::ostream& s) const
         throw( gpstk::InvalidRequest )
   {
     if (!dataLoaded())
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      s << "**************************************************************" << std::endl
        << " ORB/CLK ALMANAC PARAMETERS FOR " << subjectSV
        << std::endl;

      std::string tform("test");
      
         // To do, generalize for all systems
      if (subjectSV.system==SatID::systemIRNSS)
	 tform = "%02m/%02d/%Y %03j %02H:%02M:%02S  %7.0s  %4O %6.0g  %P";
      else 
	 tform = "%02m/%02d/%Y %03j %02H:%02M:%02S  %7.0s  %4F %6.0g  %P";
      
      s << std::endl
        << "              MM/DD/YYYY DOY HH:MM:SS      SOD  WWWW    SOW\n";
      s << "Transmit   :  "
        << printTime(beginValid,tform) << std::endl;
      s << "Orbit Epoch:  "
        << printTime(ctToe,tform) << std::endl;
      s << std::endl; 
   }
}
