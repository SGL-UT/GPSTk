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

//------------------------------------------------------------------------------------      
// See notes in the .hpp.  This function is designed to be called 
// AFTER all elements are loaded.  It can then make adjustments to
// time relationships based on inter-comparisons between sets of 
// elements that cannot be performed until the ordering has been
// determined. 
//-----------------------------------------------------------------------------
   void GPSOrbElemStore::rationalize( )
   {
      UBEMap::iterator it;
      for (it = ube.begin(); it != ube.end(); it++)
      {
         OrbElemMap& em = it->second;
         OrbElemMap::iterator ei;
  	     OrbElemMap::iterator eiPrev;
         bool begin = true;
         double previousOffset = 0.0;
         long previousToe = 0.0;
         bool previousIsOffset = false; 
         bool currentIsOffset = false;
         bool previousBeginAdjusted = false;
         bool adjustedBegin = false;
         CommonTime prevOrigBeginValid; 

         string tForm = "%03j.%02H:%02M:%02S";
         //SatID sid = it->first;
         //cout << " Scannning PRN ID: " << sid.id << endl;

            // Scan the map for this SV looking for 
            // uploads.  Uploads are identifed by 
            // Toe values that are offset from 
            // an even hour.  
         OrbElemBase* oePrev = 0;
         for (ei = em.begin(); ei != em.end(); ei++) 
         {
            currentIsOffset = false;      // start with this assumption
              // Since this is GPSOrbElemStore, then the type in the
              // store must AT LEAST be OrbElem.  
            OrbElemBase* oeb = ei->second;
            OrbElem* oe = dynamic_cast<OrbElem*>(oeb);
            long Toe = (long) (static_cast<GPSWeekSecond> (oe->ctToe)).sow;
            double currentOffset = Toe % 3600;
            
            CommonTime currOrigBeginValid = oe->beginValid;

            //cout << "Top of For Loop.  oe->beginValid = " << printTime(oe->beginValid,tForm);
            //cout << ", currentOffset =" << currentOffset << endl;
            
            if ( (currentOffset)!=0) 
            {
               currentIsOffset = true; 

               //cout << "*** Found an offset" << endl;
               //cout << " currentIsOffset: " << currentIsOffset; 
               //cout << " previousIsOffset: " << previousIsOffset; 
               //cout << " current, previous Offset = " << currentOffset << ", " << previousOffset << endl; 
            
                  // If this set is offset AND the previous set is offset AND
                  // the two offsets are the same AND the difference
                  // in time between the two Toe == two hours, 
                  // then this is the SECOND
                  // set of elements in an upload.  In that case the OrbElem
                  // load routines have conservatively set the beginning time
                  // of validity to the transmit time because there was no
                  // way to prove this was the second data set.  Since we can
                  // now prove its second by observing the ordering, we can
                  // adjust the beginning of validity as needed.
                  // Since the algorithm is dependent on the message
                  // format, this must be done in OrbElem.
                  // 
                  // IMPORTANT NOTE:  We also need to adjust the 
                  // key in the map, which is based on the beginning
                  // of validity.  However, we can't do it in this
                  // loop without destroying the integrity of the
                  // iterator.  This is handled later in a second
                  // loop.  See notes on the second loop, below. 
               long diffToe = Toe - previousToe;
               if (previousIsOffset &&
                   currentIsOffset  &&
                   currentOffset==previousOffset &&
                   diffToe==7200)
               {
                 //cout << "*** Adjusting beginValid.  Initial value: "
                 //     << printTime(oe->beginValid,tForm); 
                 oe->adjustBeginningValidity();
                 adjustedBegin = true;
                 //cout << ", Adjusted value: " 
                 //     << printTime(oe->beginValid,tForm) << endl;
               }
               
                  // If the previous set is not offset, then 
                  // we've found an upload.
                  // For that matter, if previous IS offset, but 
                  // the current offset is different than the
                  // previous, then it is an upload.
               if (!previousIsOffset ||
                   (previousIsOffset && (currentOffset!=previousOffset) ) )
               {
                     // Record the offset for later reference
                  previousOffset = currentOffset;

                     // Adjust the ending time of validity of any elements
                     // broadcast BEFORE the new upload such that they
                     // end at the beginning validity of the upload.  
                     // That beginning validity value should already be
                     // set to the transmit time (or earliest transmit
                     // time found) by OrbElem and GPSOrbElemStore.addOrbElem( )
                     // This adjustment is consistent with the IS-GPS-XXX 
                     // rules that a new upload invalidates previous elements.
                     // Note that this may be necessary for more than one
                     // preceding set of elements.
                  if (!begin)
                  {
                     //cout << "*** Adjusting endValid Times" << endl;
                     OrbElemMap::iterator ri;
                     ri = em.find(oePrev->beginValid);     // We KNOW it exists in the map
                        // Actuall, there is a really, odd rare case where it 
                        // DOES NOT exist in the map.  That case is the case 
                        // in which we modified the begin valid time of oePrev
                        // in the previous iteration of the loop.  
                     if (ri==em.end() && previousBeginAdjusted)
                     {
                        //cout << "*** Didn't find oePrev->beginValid(), but prev beginValid changed.";
                        //cout << "  Testing the prevOrigBeginValid value." << endl;
                        ri = em.find(prevOrigBeginValid);
                        if (ri==em.end())
                        {
                           //cout << "Still didn't find a valid key.  Abort rationalization for this SV." << endl;
                           continue;    // Abort rationalization for this SV with no further changes.
                        }
                        //cout << "  Successfully found the previous object." << endl;
                     }
                     
                     
                     bool done = false;
                     while (!done)
                     {
                        OrbElemBase* oeRev = ri->second;
                        //cout << "Testing Toe of " << printTime(oeRev->ctToe,"%02H:%02M:%02S");
                        //cout << " with endValid of " << printTime(oeRev->endValid,"%02H:%02M:%02S") << endl;

                           // If the current set of elements has an ending
                           // validity prior to the upload, then do NOT
                           // adjust the ending and set done to true.
                        if (oeRev->endValid <= oe->beginValid) done = true;

                           // Otherwise, adjust the ending validity to 
                           // match the begin of the upload. 
                         else oeRev->endValid = oe->beginValid;

                           // If we've reached the beginning of the map, stop. 
                           // Otherwise, decrement and test again.
                        if (ri!=em.begin()) ri--;
                         else done = true;
                     }
                  }
               }
            }
            
               // Update condition flags for next loop
            previousIsOffset = currentIsOffset;
            previousToe      = Toe;

               // If beginValid was adjusted for THIS oe, set
               // the flag previousBeginAdjusted so we have that
               // information to inform the next iteration.  However,
               // do not let the flag persist beyond one iteration
               // unless adjustedBegin is set again. 
            previousBeginAdjusted = false;
            if (adjustedBegin) previousBeginAdjusted = true;
            adjustedBegin = false;
            prevOrigBeginValid = currOrigBeginValid; 
            
            oePrev = oe;           // May need this for next loop.
            begin = false; 
            //cout << "Bottom of For loop.  currentIsOffset: " << currentIsOffset <<
            //        ", previousIsOffset: " << previousIsOffset << endl;
         } //end inner for-loop 

            // The preceding process has left some elements in a condition
            // when the beginValid value != the key in the map.  This 
            // must be addressed, but the map key is a const (by definition).
            // We have to search the map for these disagreements.  When found,
            // the offending item need to be copied out of the map, the 
            // existing entry deleted, the item re-entered with the new key.
            // Since it is unsafe to modify a map while traversing the map, 
            // each time this happens, we have to reset the loop process.
            //
            // NOTE: Simplisitically, we could restart the loop at the 
            // beginning each time.  HOWEVER, we sometimes load a long
            // period in a map (e.g. a year).  At one upload/day, that
            // would mean ~365 times, each time one day deeper into the map.
            // As an alternate, we use the variable loopStart to note how
            // far we scanned prior to finding a problem and manipulating
            // the map.  Then we can restart at that point. 
         bool done = false; 
         CommonTime loopStart = CommonTime::BEGINNING_OF_TIME;
         while (!done)
         {
            ei = em.lower_bound(loopStart);
            while (ei!=em.end())
            {
              OrbElemBase* oe = ei->second;
              if (ei->first!=oe->beginValid)
              {
                 //cout << "Removing an element.....";
                 OrbElemBase* oeAdj= oe->clone();       // Adjustment was done in 
                                                   // first loop above.
                 delete ei->second;                // oe becomes invalid.
                 em.erase(ei);                     // Remove the map entry.
                 em[oeAdj->beginValid] = oeAdj->clone(); // Add back to map
                 //cout << "...restored the element." << endl;
                 break;            // exit this while loop without finishing
              }
              loopStart = ei->first; // Scanned this far successfully, so
                                     // save this as a potential restart point.
              ei++;
              if (ei==em.end()) done = true;   // Successfully completed 
                                               // the loop w/o finding any 
                                               // mismatches.  We're done. 
            } 
        }

           // Well, not quite done.  We need to update the initial/final 
           // times of the map.
        const OrbElemMap::iterator Cei = em.begin( );
        initialTime = Cei->second->beginValid;
        const OrbElemMap::reverse_iterator rCei = em.rbegin();
        finalTime   = rCei->second->endValid;

      } // end outer for-loop
      //cout << "Exiting GPSOrbElem.rationalize()" << endl; 
   }
   
} // namespace
