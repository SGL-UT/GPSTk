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
 * @file UniqueAlmStore.cpp
 */
#include <iostream>   
#include "UniqueAlmStore.hpp"

#include "FICData162.hpp"
#include "FICData62.hpp"
#include "gps_constants.hpp"

namespace gpstk
{
   using namespace std;
   using namespace gpstk;

      // Table 20-V from IS-GPS-200.  Negative numbers inidicate
      // that the SVID given is nominal, but substitutions are allowe4. 
   static short SVIDOrder[] = { 57,  1, 
                                25,  2,
                                26,  3,
                                27,  4,
                                28,  5,
                                57,  6,
                                29,  7,
                                30,  8,
                                31,  9,
                                32, 10,
                                57, 11,
                                62, 12,
                                52, 13,
                                53, 14,
                                54, 15,
                                57, 16,
                                55, 17,
                                56, 18,
                               -58, 19,
                               -59, 20,
                                57, 21,
                               -60, 22,
                               -61, 23,
                                62, 24,
                                63, 51 };
   
   UniqueAlmStore::UniqueAlmStore( NavIndex ni, NavCode nc )
   {
      state = WAITING;
      prn = ni.second;
      range = (ni.first).first;
      navCode = nc;
      startingSOW = -10;
      candidateToa = -10;
      written = false;
      numPagesExamined = 0;
      ToaTime = gpstk::DayTime::BEGINNING_OF_TIME;
   }

   pmCI UniqueAlmStore::begin() const { return(pageMap.begin()); }
   pmCI UniqueAlmStore::end() const { return(pageMap.end()); } 

   void UniqueAlmStore::newSubframe( gpstk::MDPNavSubframe nav )
   {
      numPagesExamined++;
      
         // Check parity
      long sfa[10];
      nav.fillArray(sfa);
      uint32_t uint_sfa[10];
      for (int j=0; j<10; ++j) uint_sfa[j] = static_cast<uint32_t>( sfa[j] );
      if (!gpstk::EngNav::checkParity(uint_sfa)) 
         return;
         
         // Pull the SVID and time from the subframe
      short SVID = nav.getSVID();
      short week = nav.time.GPSfullweek();
      long sow = nav.getHOWTime();
      if ( sow >604800)
         return;

      DayTime howTime(week, sow);

      //if (nav.prn==1) cout << "state, SFID, SVID: " << state << ", " << nav.getSFID() << ", " << SVID;
         // Definitions that appear to need to be outside the switch
      short expectedSVID; 
      bool optional;
      bool storePage;
      int newState = state;
      switch (state)
      {
         // If WAITING, we're looking for SF 5, Page 25
         // which is SVID 51.
         case WAITING:
            if (SVID!=51) break;
            candidateToa = getToa(nav);
            newState = START_ON_NEXT_FRAME;
            if (nav.prn==1) cout << "State Change:START_ON_NEXT_FRAME" << endl;
               // Clear the subframe map so it's ready to fill
            pageMap.clear();
            ToaTime = DayTime::BEGINNING_OF_TIME;
            startingSOW = -10;
            written = false;
            break;
            
            // The previous SF5 was pg 25, SVID 51.
            // The next almanac SF we see should be
            // Pg 4, pg 1, SVID 57 with a time that's
            // equal to an even 12.5 min (750 sec) epoch
            // (lus an appropriate offset for the 3 ephemeris
            // pages and the usual +6 HOW offset( 
            // in terms of SOW.  If so, we've established
            // sync with the almanac cycle and we should
            // start collecting.
         case START_ON_NEXT_FRAME:
         {
            if (nav.prn==1) cout << "Checking for start.  SFID, SVID = " << nav.getSFID() << ", " << SVID << endl;
            if (nav.getSFID()!=4 ||
                SVID!=57 ) { newState=WAITING; break; }
            long test = nav.getHOWTime();
            long remainder = test % ALMANAC_PERIOD;
            if (remainder!=SF4_OFFSET) {newState=WAITING; break; }
            // Appear to have sync, initialize collection
            SVIDOrderNdx = 0; 
            startingSOW = test;
            newState = COLLECTING;
            if (nav.prn==1) cout << "State Change:COLLECTING" << endl;
            //break;            - NO BREAK, we WANT to drop through
            //                    to COLLECTING and process this SF
         }
         
         // If COLLECTING, we're 
         case COLLECTING:
            if (nav.prn==1) cout << "COLLECTING: SVID = : " << SVID << endl;
            if (isToaPage(SVID))
            {
               if (candidateToa!=getToa(nav))
               {
                  newState = WAITING;          // Toa mismatch, restart
                  if (nav.prn==1) cout << "State change: WAITING.  toa mismatch" << endl;
                  break; 
               }
            }
             
              // Test the SVID
            storePage = true;
            expectedSVID = SVIDOrder[SVIDOrderNdx];
            optional = false;
            if (expectedSVID < 0)
            {
               optional = true;
               expectedSVID = -1 * expectedSVID;
            }
               // Test if page is for unavailable SV
            if (nav.getSVID()==0 &&
               expectedSVID>1 && expectedSVID<=32)
            {
               storePage = false;
            }
               // A relaxed test if page is optional in the ordering
            else if (optional &&
                nav.getSVID()!=expectedSVID)
            {
               cerr << "WARNING: Expected SVID " << expectedSVID << 
                       ", received SVID " << nav.getSVID() << 
                       ". Continuing." << endl;
            }
            else if (expectedSVID!=nav.getSVID())
            {
               newState = WAITING;    // SVID  mismatch, restart 
               if (nav.prn==1) cout << "State change: WAITING.  SVID out of order" << endl;
               break; 
            }
            SVIDOrderNdx++;
            
              // All tests passed.  Store the page.
            if (storePage) pageMap.insert( make_pair(expectedSVID,nav) );
             
              // If this is true, we've collected the final
              // page of a possible set.  Perform some checks.  
            if (expectedSVID==51)
            {
               if (nav.prn==1) cout << "Testing for end of cycle." << endl;
                 // Check 12.5 min period
                 // First page in set was a SF4, we're now
                 // 24 frames later with a SF5.   
                 //    (24 frames * 30s/frame) + 6s = 726s; 
               long nowHOW = nav.getHOWTime();
               long diff = nowHOW - startingSOW;
               if (diff!= (ALMANAC_PERIOD+6-30) ) 
               {
                  newState = WAITING;   // Wrong time for cycle, restart
                  if (nav.prn==1) cout << "State change: WAITING. Cycle time of " << diff << " incorrect" << endl;
                  break;        
               }
               
                  // Check for page completeness
               if (!completeSetOfPages())
               {
                  newState = WAITING;     // Incomplete attempt, restart
                  if (nav.prn==1) cout << "State change: WAITING. incomplete set of pages" << endl;
                  break;
               }
                  
                  // Set ToA time
               short currentWeek = nav.time.GPSfullweek();
               uint32_t word = nav.subframe[3];
               word &= 0x00003FC0;
               word >>= 6;
               short toaWeek = fullWeekFrom8Bit( currentWeek, (short) word );
               ToaTime = DayTime( toaWeek, candidateToa );
               
               newState = COMPLETE;
               if (nav.prn==1) cout << "State Change:COMPLETE !!!" << endl;
            }
            break;
      
         // If COMPLETE, then we're
         // watching the stream to see a change in the 
         // Toa.  At that point, we flush the current 
         // data and start all over again in WAITING.
         case COMPLETE:
            if (isToaPage(SVID))
            {
               if (candidateToa!=getToa(nav))
               {
                  newState = WAITING;             // New Toa, start a new collection
                  if (nav.prn==1) cout << "State Change:WAITING. Found new Toa" << endl;
               }
            }
            break;
      }
         // update the state
      state = newState;
      return;
   }
   
   bool UniqueAlmStore::completeSetOfPages() const
   {
      int n;
      
         // SV IDs 51-57, 62, and 63 must be present
      pmCI p;
      for (n=51;n<57;++n)
      {
         p = pageMap.find(n);
         if ( p==pageMap.end() ) return(false);
      }
      p = pageMap.find(62);
      if ( p==pageMap.end() ) return(false);
      p = pageMap.find(63);
      if ( p==pageMap.end() ) return(false);
      
         // For SVID 1-32, should have a page for every
         // SV where health is not 0x3F.
         // First unpack the health for all 32 SVs 
         // (we already proved we have the health pages).
      short tempHealth[gpstk::MAX_PRN+2];   // a. want to index 1-32
      short SVndx = 0;                      // b. there's a "don't care" 33
      
         // SF5, pg 25, SVID 51.  Health for PRNID 1-24
      p = pageMap.find(51); 
      const MDPNavSubframe& nav51 = p->second;
      uint32_t word;
      uint32_t h;
      for (n=4;n<=9;++n)
      {
         word = nav51.subframe[n];
         word >>= 6;                         // Remove parity
         for (int bndx=4;bndx>=1;--bndx)
         {
            h = word & 0x0000003F;
            int pndx = SVndx+bndx;
            tempHealth[pndx] = (short) h;
            word >>= 6;
         }
         SVndx += 4;
      }
      
         // SF4, pg 25, SVID 63.  Health for PRNID 25-32
      p = pageMap.find(63); 
      const MDPNavSubframe& nav63 = p->second;
      
         // PRN 25 is all by itself
      word = nav63.subframe[8];
      word >>= 6;
      h = word & 0x0000003F;
      tempHealth[SVndx++] = (short) h;
      
         // PRN 26-32 (plus a bogus extra)
      for (n=9;n<=10;++n)
      {
         word = nav63.subframe[n];
         word >>= 6;                         // Remove parity
         for (int bndx=4;bndx>=1;--bndx)
         {
            h = word & 0x0000003F;
            int pndx = SVndx+bndx;
            tempHealth[pndx] = (short) h;
            word >>= 6;
        }
         SVndx += 4;
      }
      
      //debug
      //for (n=1;n<=gpstk::MAX_PRN;++n)
      //{
      //   cout << n << ":" << tempHealth[n] << ",  ";
      //   if (n%6==0) cout << endl;
      //}
      
         // Now perform the checks
      bool tripwire = false;
      for (n=1;n<=gpstk::MAX_PRN;++n)
      {
         if (tempHealth[n]!=DEAD_HEALTH && pageMap.find(n)==pageMap.end())
         {
            //cout << "Health = " << tempHealth[n] << " for PRNID " << n << " and no page found. " << endl;
            tripwire = true;
         }
      }
      if (tripwire) return(false);
      return(true);
   }
   
   short UniqueAlmStore::fullWeekFrom8Bit( const short full, const short eightBit )
   {
      short curr8bitWeek = full & 0x00FF;
      short diff = curr8bitWeek - eightBit;
      short retArg = eightBit;
      short upperBits = full & 0xFF00; 
      if (diff>HALF_8BITS) upperBits -= 0x0100;
      else if (diff<-HALF_8BITS) upperBits += 0x0100;
      retArg = upperBits | eightBit;
      return(retArg);
   }
   
   bool UniqueAlmStore::isToaPage( const short SVID ) 
   {
      if (SVID>=1 && SVID<=38) return(true);
      if (SVID==51) return(true);
      return(false);
   }

   long UniqueAlmStore::getToa( const MDPNavSubframe& nav )
   {
      uint32_t word;
      long retToa = -1;
      short SVID = nav.getSVID();
      if (SVID>=1 && SVID<=38)
      {
         word = nav.subframe[4];
         word &= 0x3FFFFFFF;
         word >>= 22;
         retToa = (long) word;
      }
      else if (SVID==51)
      {
         word = nav.subframe[3];
         word &= 0x003FC000;
         word >>= 14;
         retToa = (long) word;
      }
      return(retToa);
   }
   
   bool UniqueAlmStore::readyToWrite() const
   {
      if (state==COMPLETE && !written) return(true);
      return(false);
   }
   
   void UniqueAlmStore::write(gpstk::FICStream& out)
   {
         // Cycle through complete almanac
         // For each page, convert to 162, output 162, 
         //      convert to 62, output 62
         // NOTE: This would be a BAD IDEA for a real-time
         // implementation.  In such a system, the converts
         // would be better spaced out as the pages are collected;
      for (pmCI p1=pageMap.begin();p1!=pageMap.end();++p1)
      {
         const MDPNavSubframe& nav = p1->second;
         FICData162 new162( nav.prn,
                            nav.getSVID(),
                            nav.time.GPSfullweek(),                            
                            ToaTime.GPSfullweek(),
                            nav.subframe);
         FICData62 new62( new162);
         out << new162;
         out << new62;
      }
        
         // Set the flag so we don't keep writing the same data 
         // over and over.
      written = true;
   }
   
   
}   // namespace
