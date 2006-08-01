#pragma ident "$Id$"


/**
 * @file RollingFile.cpp
 * Writes data to a file whose name is derived from a pattern and a nominal epoch. 
 * Class definitions.
 */

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

#include <iomanip>
#include <string>
#include "RollingFile.hpp"

namespace gpstk
{

   RollingFile::RollingFile(const std::string ifilespec) : 
         filespec(ifilespec), ofstrPtr(0)
   {   
   }
   
   RollingFile::~RollingFile(void)
   {
      if (ofstrPtr !=0)
      {
         *ofstrPtr << std::flush;
         delete ofstrPtr;
      }
      
   }

   std::ofstream& RollingFile::getStream(void) throw (Exception)
   {
      if (ofstrPtr==0)
      {
         Exception e("Stream accessed but not created.");
         GPSTK_THROW(e);
      }
      
      return *ofstrPtr;         
   }
   
   
   bool RollingFile::write(const std::string& msg, const DayTime& epoch)
   {
      using namespace std;
      bool openedNewFile = false;

      const string thisFilename=epoch.printf(filespec);

         // Case 1. No output file has ever been opened
      if (ofstrPtr==0) 
      {
         ofstrPtr = new ofstream(thisFilename.c_str(), ios::app);
         currentFilename = thisFilename;
         openedNewFile=true;
      }
      else 
      {
         // Case 2. This is the current open file. // Nothing to do for now
         if (currentFilename==thisFilename)
         {
         }
         else // Case 3. This is another file. Close the old one and open the new one.
         {
            delete ofstrPtr;
            ofstrPtr = new ofstream(thisFilename.c_str(), ios::app);
            currentFilename = thisFilename;
            openedNewFile=true;
         }
      }
      
      (*ofstrPtr) << msg << flush;
      
      return openedNewFile;
   };
   
   
} // namespace gpstk

