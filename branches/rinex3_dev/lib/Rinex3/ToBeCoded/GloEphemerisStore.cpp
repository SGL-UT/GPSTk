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
 * @file GloEphemerisStore.cpp
 * Read & store SP3 formated ephemeris data
 */

#include "GloEphemerisStore.hpp"
#include "MiscMath.hpp"
#include "ECEF.hpp"
#include "icd_200_constants.hpp"

using namespace gpstk::StringUtils;

namespace gpstk
{
   //-----------------------------------------------------------------------------
   //-----------------------------------------------------------------------------
   void GloEphemerisStore::loadFile(const std::string& filename)
      throw(FileMissingException)
   {
      try
      {
         SP3Stream strm(filename.c_str());
         if (!strm)
         {
            FileMissingException e("File " + filename + " could not be opened.");
            GPSTK_THROW(e);
         }
      
         SP3Header header;
         strm >> header;

         addFile(filename, header);

         /// If any file doesn't have the velocity data, clear the
         /// the flag indicating that there is any velocity data
         if (tolower(header.pvFlag) != 'v')
            haveVelocity = false;

         SP3Data rec;
         while(strm >> rec) {
            rec.version = header.version;
            addEphemeris(rec);
         }
      }
      catch (gpstk::Exception& e)
      {
         GPSTK_RETHROW(e);
      }
   }  // end GloEphemerisStore::load


   //--------------------------------------------------------------------------
   //--------------------------------------------------------------------------
   void GloEphemerisStore::dump(std::ostream& s, short detail)
      const throw()
   {
      s << "Dump of GloEphemerisStore:" << std::endl;
      std::vector<std::string> fileNames = getFileNames();
      std::vector<std::string>::const_iterator f=fileNames.begin();
      for (f=fileNames.begin(); f!=fileNames.end(); f++)
         s << *f << std::endl;
/*
  Add this back in when/if we add header info to the file store.
      while(fmi != fm.end()) {
         s << " File " << fmi->first << ", Times: " << fmi->second.time
            << " to " << (fmi->second.time+fmi->second.epochInterval*fmi->second.numberOfEpochs)
            << ", (" << fmi->second.numberOfEpochs
            << "  " << fmi->second.epochInterval << "sec intervals)." << std::endl;
         if(detail > 0) {
            s << "  Data used as input : " << fmi->second.dataUsed
               << "  Coordinate system : " << fmi->second.coordSystem << std::endl;
            s << "  Orbit estimate type : " << fmi->second.orbitType
               << "  Agency : " << fmi->second.agency << std::endl;
            s << "  List of satellite PRN/acc (" << fmi->second.svList.size()
               << " total) :\n";
            int i=0;
            std::map<short,short>::const_iterator it=fmi->second.svList.begin();
            while(it != fmi->second.svList.end()) {
               s << "  " << std::setw(2) << it->first << "/" << it->second;
               if(!(++i % 8)) s << std::endl;
               it++;
            }
            if(++i % 8) s << std::endl;
            s << "  Comments:\n";
            for(i=0; i<fmi->second.comments.size(); i++)
               s << "    " << fmi->second.comments[i] << std::endl;
            s << std::endl;
         }
         fmi++;
      }
*/
      TabularEphemerisStore::dump(s, detail);

   } // end of GloEphemerisStore::dump

}  // namespace gpstk
