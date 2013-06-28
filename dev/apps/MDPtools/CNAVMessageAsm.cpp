#pragma ident "$Id:$"

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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Copyright 2013, The University of Texas at Austin
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
#include "CNAVMessageAsm.hpp"
#include "OrbElemCNAV.hpp"

using namespace gpstk; 

CNAVMessageAsm::CNAVMessageAsm()
{
   currentWeek = -1;
   weekSet = false;
   currentMsgMap[0] = 0;
   currentMsgMap[1] = 0;
   currentMsgMap[2] = 0;
}

void CNAVMessageAsm::addMDPNavSubframe( const gpstk::MDPNavSubframe& mdpsf) 
//                                   const gpstk::GPSOrbElemStore& store)
{
   // Create a new PackedNavBits object from the MDPNavSubframe object.
   short msgType = getMessageType(mdpsf);

   //std::cout << "In addMDPNavSubframe: msgType:" << msgType << ", wkSet:" << weekSet 
   //     << ", week:" << currentWeek << std::endl;

   // Can't do much of anything until we see a msg type 10 and determine
   // the current week number. 
   if (!weekSet && msgType!=10) return;
   if (!weekSet && msgType==10)
   {
      currentWeek = getWeek(mdpsf);
      weekSet = true;
   }

   short PRNID = getPRNID(mdpsf);
   SatID satID(PRNID, SatID::systemGPS);

   ObsID::CarrierBand cb = ObsID::cbUnknown;
   switch (mdpsf.carrier)
   {
      case ccL1 : cb = ObsID::cbL1; break;
      case ccL2 : cb = ObsID::cbL2; break;
      case ccL5 : cb = ObsID::cbL5; break;
      default: break;  
   }

   ObsID::TrackingCode tc = ObsID::tcUnknown;
   switch (mdpsf.range)
   {
      case rcCM   : tc = ObsID::tcC2M;  break;
      case rcCL   : tc = ObsID::tcC2L;  break;
      case rcCMCL : tc = ObsID::tcC2LM; break;
      case rcI5   : tc = ObsID::tcI5;   break;
      case rcQ5   : tc = ObsID::tcQ5;   break;      
      default: break;
   }

   ObsID::ObservationType ot = ObsID::otNavMsg;
   ObsID obsID(ot, cb, tc); 

   unsigned long xmitSOW = getTOWCount(mdpsf) - 12;
   CommonTime xmitTime = GPSWeekSecond(currentWeek, xmitSOW, TimeSystem::GPS); 
   std::cout << "SOW, time:" << xmitSOW << ", " << xmitTime << std::endl;
   PackedNavBits* pnbp = new PackedNavBits(satID, obsID, xmitTime); 

   // Now add the data
   for (int i=1; i<=10; i++)
   {
      unsigned long ul = (unsigned long) mdpsf.subframe[i];
      pnbp->addUnsignedLong(ul, 30, 1);
   }
   pnbp->trimsize();

      // Debug
   std::cout << "MsgType:" << msgType << std::endl;
   pnbp->dump(); 
   std::cout << std::endl;

   // Add the new PackedNavBits object to the map of current messages.
   int ndx = -1;
   bool foundA3xMsg = false;
   if (msgType==10) ndx = 0;
   if (msgType==11) ndx = 1;
   if (msgType>=30 && msgType<=37) 
   {
      ndx = 2;
      foundA3xMsg = true;
   }
   if (ndx>=0)
   {
      std::cout << " Update msgType " << msgType << ", ndx " << ndx << std::endl;
      currentMsgMap[ndx] = pnbp->clone();
   }

   // If this is a msg type 3X, check to see if the map has a contiguous
   // set of Msg 10/Msg 11/Msg 30x for this SatId/ObsID.  If so, attempt
   // to create a CNAVOrbElem object.  If that succeeds, add the object
   // to the orbital element store. 
   if (foundA3xMsg &&
       currentMsgMap[0] != 0 &&
       currentMsgMap[1] != 0 &&
       currentMsgMap[2] != 0 )
   {
      CommonTime time10 = currentMsgMap[0]->getTransmitTime();
      time10.setTimeSystem(TimeSystem::GPS);
      /*
      std::cout << "Time (10): " << time10 << std::endl;
      std::cout << "Size, SatID, ObsID: " << currentMsgMap[0]->getNumBits()
                << ", " << currentMsgMap[0]->getsatSys()
                << ", " << currentMsgMap[0]->getobsID() << std::endl;
      */
      CommonTime time11 = currentMsgMap[1]->getTransmitTime();
      time11.setTimeSystem(TimeSystem::GPS);
      CommonTime time3x = currentMsgMap[2]->getTransmitTime();
      time3x.setTimeSystem(TimeSystem::GPS);
      /*
      std::cout << "Time (30): " << time3x << std::endl;
      std::cout << "Size, SatID, ObsID: " << currentMsgMap[2]->getNumBits()
                << ", " << currentMsgMap[2]->getsatSys()
                << ", " << currentMsgMap[2]->getobsID() << std::endl;
      */
      double diff11minus10 = time11-time10;
      std::cout << "diff11minus10: " << diff11minus10 << std::endl;
      if (diff11minus10==12.0) 
      {
         double diff3xMinus10 = time3x - time10;
         std::cout << "diff3xMinus10: " << diff3xMinus10 << std::endl;
         if (diff3xMinus10<=36)
         {
            std::cout << "Attempt to create a CNAV object" << std::endl;            
            SatID satId = currentMsgMap[0]->getsatSys();
            ObsID obsId = currentMsgMap[0]->getobsID(); 
            try
            {
            OrbElemCNAV oecn (obsId, satId, 
                              *currentMsgMap[0],
                              *currentMsgMap[1],
                              *currentMsgMap[2]); 
            std::cout << oecn << std::endl;
            }
            catch(gpstk::Exception e)
            {
               std::cout << "Caught exception converting to OrbElemCNAV."  << std::endl;
               std::cout << e << std::endl;
            }
         }
      }
   }
}

   // Crack the message type from the MDPNavSubframe
short CNAVMessageAsm::getMessageType(const gpstk::MDPNavSubframe& mdpsf)
{
   uint32_t front30 = mdpsf.subframe[1];
   front30 >>= 10;
   front30 &= 0x0000003F;
   short retVal = (short) front30;
   return retVal;
}

   // Crack the PRN ID for the SV from the MDPNavSubframe
short CNAVMessageAsm::getPRNID(const gpstk::MDPNavSubframe& mdpsf)
{
   uint32_t front30 = mdpsf.subframe[1];
   front30 >>= 16;
   front30 &= 0x0000003F;
   short retVal = (short) front30;
   return retVal;
}

   // Crack the week number from the MDPNavSubframe.
   // NOTE: This assumes the message type is message type 10.
   //       otherwise the results will be invalid.
short CNAVMessageAsm::getWeek(const gpstk::MDPNavSubframe& mdpsf)
{
   uint32_t front30 = mdpsf.subframe[2];
   front30 >>= 9;
   front30 &= 0x00001FFF;
   short retVal = (short) front30;
   return retVal;
}

   // Crack the TOW count from the message and return that as a SOW.
   //  NOTE:  This is the true
   // TOW count.  That is to say, the begin time of the NEXT message. 
unsigned long CNAVMessageAsm::getTOWCount(const gpstk::MDPNavSubframe& mdpsf)
{
   uint32_t front30 = mdpsf.subframe[1];
   uint32_t next30 = mdpsf.subframe[2]; 
   front30 &= 0x000003FF;
   uint32_t highBits = front30 << 7;

   next30 >>= 23; 
   uint32_t lowBits = next30 & 0x0000007F;

   unsigned long retVal = highBits | lowBits; 
   retVal *= 6;
   return retVal;  
}

