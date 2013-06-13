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

#ifndef CNAVMESSAGEASM_HPP
#define CNAVMESSAGEASM_HPP

// Utility class to covert MDPNavSubframe data into CNAV messages.
#include "MDPNavSubframe.hpp"
#include "GPSOrbElemStore.hpp"
#include "ObsID.hpp"
#include "PackedNavBits.hpp"
#include "SatID.hpp"

class CNAVMessageAsm
{
public:
   CNAVMessageAsm();
   ~CNAVMessageAsm() {}

   // Add a MDPNavSubframe to the current set.
   // Add whatever larger structures are possible the ephemeris store. 
   void addMDPNavSubframe( const gpstk::MDPNavSubframe& mdpsf); 
//                           const gpstk::GPSOrbElemStore& store);

   // The index is 0 = msg type 11
   //              1 = msg type 12
   //              2 = msg type 3x
   //
   // These are used to group together the sets of message 11/12/3x.
   //typedef std::map<gpstk::ObsID,MapByMsgType> MapByObsID;
   //typedef std::map<gpstk::SatID,MapByObsID> CurrentMsgMap;
   gpstk::PackedNavBits* currentMsgMap[3];
   short currentWeek;
   bool weekSet; 

private: 
   short getMessageType(const gpstk::MDPNavSubframe& mdpsf);
   short getPRNID(const gpstk::MDPNavSubframe& mdpsf);
   short getWeek(const gpstk::MDPNavSubframe& mdpsf);
   unsigned long getTOWCount(const gpstk::MDPNavSubframe& mdpsf);
};

#endif

