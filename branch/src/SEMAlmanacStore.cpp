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
 * @file SEMAlmanacStore.cpp
 * Store SEM almanac information, and access by satellite and time
 */

#include "SEMAlmanacStore.hpp"

namespace gpstk
{

   void SEMAlmanacStore::loadFile(const std::string& filename)
      throw(FileMissingException)
   {
      try
      {
         SEMStream strm(filename.c_str());
         if (!strm)
         {
            FileMissingException e("File " + filename + " could not be opened.");
            GPSTK_THROW(e);
         }
         
         SEMHeader header;
         strm >> header;
         addFile(filename, header);

         SEMData rec;
         while(strm >> rec)
	 {
            //This is a fix to get the header and the data to share Toa and week.
	    //This should be fixed in the future
	    rec.Toa = header.Toa;
	    rec.week = header.week + 1024;
	    addAlmanac(AlmOrbit(rec));
         }	 
      }
      catch (Exception& e)
      {
         GPSTK_RETHROW(e);
      }  
   }
   
}
