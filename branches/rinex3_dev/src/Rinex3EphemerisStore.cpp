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
 * @file Rinex3EphemerisStore.cpp
 * Read and store Rinex3 formated ephemeris data
 */

#include <string>

#include "Rinex3EphemerisStore.hpp"
#include "Rinex3NavStream.hpp"

namespace gpstk
{
   /// load the given Rinex file
   void Rinex3EphemerisStore::loadFile(const std::string& filename) 
     throw( FileMissingException )
   {
      try
      {

         Rinex3NavStream strm(filename.c_str());
         if (!strm)
         {
            FileMissingException e("File " +filename+ " could not be opened.");
            GPSTK_THROW(e);
         }

         Rinex3NavHeader header;
         strm >> header;

         addFile(filename, header);

         Rinex3NavData rec;
         while(strm >> rec)
         {
            if (rec.satSys=="G") // Ephemeris and clock are valid, then add them
               GPSstore.addEphemeris(rec);
            else if (rec.satSys=="R") // Ephemeris and clock are valid, then add them
               GLOstore.addEphemeris(rec);
            //if (rec.satSys=="E") // Ephemeris and clock are valid, then add them
               //GALstore.addEphemeris(rec);
         }

      }
      catch (gpstk::Exception& e)
      {
         GPSTK_RETHROW(e);
      }

   }  // End of method 'SP3EphemerisStore::loadFile()'


   void Rinex3EphemerisStore::dump( std::ostream& s,
                                    short detail    )
      const throw()
   {
     GPSstore.dump(s, detail);
     GLOstore.dump(s, detail);
      //GALstore.dump();
   }

}  // namespace
