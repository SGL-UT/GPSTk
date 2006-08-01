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
 * @file RinexEphemerisStore.cpp
 * Read & store Rinex formated ephemeris data
 */

#include "RinexEphemerisStore.hpp"
#include "MiscMath.hpp"
#include "ECEF.hpp"
#include "icd_200_constants.hpp"

using namespace gpstk::StringUtils;

namespace gpstk
{
   //-----------------------------------------------------------------------------
   //-----------------------------------------------------------------------------
   void RinexEphemerisStore::loadFile(const std::string& filename)
      throw(gpstk::FileMissingException)
   {
      try
      {
         RinexNavStream strm(filename.c_str());
         if (!strm)
         {
            FileMissingException e("File " + filename + " could not be opened.");
            GPSTK_THROW(e);
         }
      
         RinexNavHeader header;
         strm >> header;

         addFile(filename, header);

         RinexNavData rec;
         while(strm >> rec)
            addEphemeris(rec);
      }
      catch (gpstk::Exception& e)
      {
         GPSTK_RETHROW(e);
      }
   }  // end RinexEphemerisStore::load


   //--------------------------------------------------------------------------
   //--------------------------------------------------------------------------
   void RinexEphemerisStore::dump(short detail, std::ostream& s) const
   {
      s << "Dump of RinexEphemerisStore:" << std::endl;
      std::vector<std::string> fileNames = getFileNames();
      std::vector<std::string>::const_iterator f=fileNames.begin();
      for (f=fileNames.begin(); f!=fileNames.end(); f++)
         s << *f << std::endl;

      BCEphemerisStore::dump(detail, s);

   } // end of RinexEphemerisStore::dump

}  // namespace gpstk
