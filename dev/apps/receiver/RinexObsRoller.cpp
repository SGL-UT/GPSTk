#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/receiver/RinexObsRoller.cpp#1 $"

/**
 * @file RinexObsRoller.cpp
 * Distributes RINEX observations to a set of files.
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
#include <fstream>
#include <string>
#include "RinexObsRoller.hpp"
#include "FileUtils.hpp"


namespace gpstk
{

   inline bool fileAccessCheck(const std::string& filename, std::ios::openmode mode=std::ios::in)
   {
     std::fstream test(filename.c_str(), mode);
     return !test.fail();
   }
   
   RinexObsRoller::RinexObsRoller(const std::string& ifilespec, const RinexObsHeader& ihdrTemplate) : 
         filespec(ifilespec), ofstrPtr(0), headerTemplate(ihdrTemplate)
   {   
   }
   
   RinexObsRoller::~RinexObsRoller(void)
   {
      if (ofstrPtr !=0)
      {
         delete ofstrPtr;
      }
      
   }

   RinexObsStream& RinexObsRoller::getStream(void) throw (Exception)
   {
      if (ofstrPtr==0)
      {
         Exception e("Stream accessed but not created.");
         GPSTK_THROW(e);
      }
      
      return *ofstrPtr;         
   }
   
   
   bool RinexObsRoller::write(const RinexObsData& rod, const DayTime& epoch)
   {
      using namespace std;
      bool openedNewFile = false;

      const string thisFilename=epoch.printf(filespec);

         // Case 1. No output file has ever been opened since this program started
      if (ofstrPtr==0) 
      {
         bool existed=fileAccessCheck(thisFilename);
         ofstrPtr = new RinexObsStream(thisFilename.c_str(), 
                                       ios::out | ios::app);
         headerTemplate.firstObs = epoch;
         if (existed)
            ofstrPtr->header=headerTemplate;
         else 
            (*ofstrPtr) << headerTemplate;
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
            bool existed=fileAccessCheck(thisFilename);
            ofstrPtr = new RinexObsStream(thisFilename.c_str(), 
                                          ios::out);
            headerTemplate.firstObs = epoch;
            if (existed)
               ofstrPtr->header=headerTemplate;
            else 
               (*ofstrPtr) << headerTemplate;
            currentFilename = thisFilename;
            openedNewFile=true;
         }
      }
      
      *ofstrPtr << rod;

      return openedNewFile;
   };
   
   
} // namespace gpstk

