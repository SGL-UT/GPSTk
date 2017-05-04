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
 * @file GPSOrbElemStore.cpp
 * Store GPS broadcast OrbElem information, and access by satellite and time
 */

#include <iostream>
#include <fstream>
#include <iomanip>

#include "StringUtils.hpp"
#include "GPSOrbElemStore.hpp"
#include "MathBase.hpp"
#include "OrbElem.hpp"
#include "CivilTime.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;
using gpstk::StringUtils::asString;


namespace gpstk
{

//--------------------------------------------------------------------------

   void GPSOrbElemStore::dump(std::ostream& s, short detail) const
      throw()
   {
      UBEMap::const_iterator it;
      static const string fmt("%04Y/%02m/%02d %02H:%02M:%02S %P");

      s << "Dump of GPSOrbElemStore:\n";
      if (detail==0)
      {
         s << " Span is " << (initialTime == CommonTime::END_OF_TIME
                                      ? "End_time" : printTime(initialTime,fmt))
           << " to " << (finalTime == CommonTime::BEGINNING_OF_TIME
                                      ? "Begin_time" : printTime(finalTime,fmt))
           << " with " << size() << " entries."
           << std::endl;
      } // end if-block
      else if (detail==1)
      {
         for (it = ube.begin(); it != ube.end(); it++)
         {
            const OrbElemMap& em = it->second;
            s << "  BCE map for satellite " << it->first
              << " has " << em.size() << " entries." << std::endl;
            OrbElemMap::const_iterator ei;

            for (ei = em.begin(); ei != em.end(); ei++) 
            {
                  // Since this is GPSOrbElemStore, then the type in the
                  // store must AT LEAST be OrbElem.  
               const OrbElemBase* oeb = ei->second;
               const OrbElem* oe = dynamic_cast<const OrbElem*>(oeb);
               s << "PRN " << setw(2) << it->first
                 << " TOE " << printTime(oe->ctToe,fmt)
                 << " TOC " << printTime(oe->ctToc,fmt)
                 << " KEY " << printTime(ei->first,fmt);
               s << " begVal: " << printTime(oe->beginValid,fmt)
                 << " endVal: " << printTime(oe->endValid,fmt); 
                
               s << std::endl;
            } //end inner for-loop */

         } // end outer for-loop
   
         s << "  End of GPSOrbElemStore data." << std::endl << std::endl;

      } //end else-block

         // In this case the output is
         // key, beginValid,  Toe,   endValid
      else if (detail==2)
      {
         string tf1 = "%02m/%02d/%02y %02H:%02M:%02S";
         string tf2 = "%02H:%02M:%02S";
         
         for (it = ube.begin(); it != ube.end(); it++)
         {
            const OrbElemMap& em = it->second;
            s << "  Map for satellite " << it->first
              << " has " << em.size() << " entries." << std::endl;
            OrbElemMap::const_iterator ei;

            s << "  PRN  MM/DD/YY      Key     Begin       Toe       Toc      End  Healthy" << endl;

            for (ei = em.begin(); ei != em.end(); ei++) 
            {
                  // Since this is GPSOrbElemStore, then the type in the
                  // store must AT LEAST be OrbElem.  
               const OrbElemBase* oeb = ei->second;
               const OrbElem* oe = dynamic_cast<const OrbElem*>(oeb);
               s << it->first << "  " << printTime(ei->first,tf1)
                              << "  " << printTime(oe->beginValid,tf2)
                              << "  " << printTime(oe->ctToe,tf2)
                              << "  " << printTime(oe->ctToc,tf2)
                              << "  " << printTime(oe->endValid,tf2);
	       if (oe->healthy) s << "  " << "   Yes";
	        else            s << "  " << "    No";
               s << std::endl;

            } //end inner for-loop */

         } // end outer for-loop
      }    // end of else-block
      else
      {
         for (it = ube.begin(); it != ube.end(); it++)
         {
            const OrbElemMap& em = it->second;
            s << "  Map for satellite " << it->first
              << " has " << em.size() << " entries." << std::endl;
            OrbElemMap::const_iterator ei;

            for (ei = em.begin(); ei != em.end(); ei++) 
            {
               const OrbElemBase* oe = ei->second;
               oe->dump(s);
            }
         }
      }
   } // end GPSOrbElemStore::dump
   
} // namespace
