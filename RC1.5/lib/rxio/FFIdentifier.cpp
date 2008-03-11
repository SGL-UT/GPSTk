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

#include <string>
#include <iostream>
#include <iomanip>

#include "FFIdentifier.hpp"

#include "SP3EphemerisStore.hpp"
#include "RinexEphemerisStore.hpp"
#include "YumaAlmanacStore.hpp"
#include "GPSGeoid.hpp"

#include "RinexObsStream.hpp"
#include "RinexObsData.hpp"

#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"
#include "RinexMetStream.hpp"
#include "RinexMetData.hpp"

#include "FICStream.hpp"
#include "FICData.hpp"

#include "SMODFStream.hpp"
#include "SMODFData.hpp"

#include "MSCData.hpp"
#include "MSCStream.hpp"

#include "MDPObsEpoch.hpp"
#include "MDPPVTSolution.hpp"
#include "MDPNavSubframe.hpp"
#include "MDPSelftestStatus.hpp"
#include "MDPStream.hpp"

#include "YumaData.hpp"
#include "YumaStream.hpp"

#include "SEMData.hpp"
#include "SEMStream.hpp"

#include "NovatelData.hpp"
#include "NovatelStream.hpp"

#include "AshtechData.hpp"
#include "AshtechStream.hpp"

namespace gpstk
{
   int FFIdentifier::debugLevel = 0;

   FFIdentifier::FFIdentifier(const std::string& fn)
   {
      using namespace std;
      fileType=tUnknown;

      {
         if (debugLevel>2)
            cout << "Trying " << fn << " as RINEX obs."<< endl;
         RinexObsStream s(fn.c_str(), ios::in);
      
         RinexObsHeader temp_roh;
         s >> temp_roh;
         RinexObsData rod;
         s >> rod;
         if (s)
         {
            fileType = tRinexObs;
            return;
         } 
      }

      {
         if (debugLevel>2)
            cout << "Trying " << fn << " as SMODF."<< endl;
         SMODFStream s(fn.c_str(), ios::in);
         
         SMODFData smodata;       
         s >> smodata;
         s >> smodata;
         if (s)
         {
            fileType = tSMODF;
            return;
         }
      }

      {
         if (debugLevel>2)
            cout << "Trying " << fn << " as MDP."<< endl;
         MDPStream s(fn.c_str(), ios::in);

         MDPHeader header;
         s >> header;
         s >> header;
         if (s)
         {
            fileType = tMDP;
            return;
         }
      }

      {
         if (debugLevel>2)
            cout << "Trying " << fn << " as RINEX nav."<< endl;
         RinexNavStream s(fn.c_str(), ios::in);

         RinexNavData rnd;
         RinexNavHeader rnh;
         s >> rnh;
         s >> rnd;
         if (s)
         {
            fileType = tRinexNav;
            return;
         }
      }
      
      {
         if (debugLevel>2)
            cout << "Trying " << fn << " as FIC nav."<< endl;
         FICStream s(fn.c_str(), ios::in);
         
         FICData data;
         s >> data;
         if (s)
         {
            fileType = tFIC;
            return;
         }
      }

      {
         if (debugLevel>2)
            cout << "Trying " << fn << " as SP3 ephemeris."<< endl;
         SP3Stream s(fn.c_str(), ios::in);
         
         SP3Header header;
         s >> header;
         SP3Data data;
         s >> data;
         if (s)
         {
            fileType = tSP3;
            return;
         }
      }

      {
         if (debugLevel>2)
            cout << "Trying " << fn << " as Yuma elmanac."<< endl;
         YumaStream s(fn.c_str(), ios::in);
         
         YumaHeader header;
         s >> header;
         YumaData data;
         s >> data;
         if (s)
         {
            fileType = tYuma;
            return;
         }
      }

      {
         if (debugLevel>2)
            cout << "Trying " << fn << " as SEM almanac."<< endl;
         SEMStream s(fn.c_str(), ios::in);
         
         SEMHeader header;
         s >> header;
         SEMData data;
         s >> data;
         if (s)
         {
            fileType = tYuma;
            return;
         }
      }

      {
         if (debugLevel>2)
            cout << "Trying " << fn << " as MSC."<< endl;
         MSCStream s(fn.c_str(), ios::in);

         MSCData mscd;
         s >> mscd;
         if (s)
         {
            fileType = tMSC;
            return;
         }
      }


      {
         if (debugLevel>2)
            cout << "Trying " << fn << " as Novatel OEM." << endl;
         NovatelStream s(fn.c_str(), ios::in);
         NovatelData nd;
         s >> nd;
         if (s)
         {
            fileType = tNovatelOem;
            return;
         }
      }

      {
         if (debugLevel>2)
            cout << "Trying " << fn << " as Ashtech serial." << endl;
         AshtechStream s(fn.c_str(), ios::in);
         AshtechData nd;
         s >> nd;
         if (s)
         {
            fileType = tAshtechSerial;
            return;
         }
      }

   }

}
