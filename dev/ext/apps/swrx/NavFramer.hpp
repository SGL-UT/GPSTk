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

#ifndef NAVFRAMER_HPP
#define NAVFRAMER_HPP

#include <iostream>
#include <bitset>

#include "EMLTracker.hpp"

//-----------------------------------------------------------------------------
// This is intended to use a generic tracker and frame up the nav data.
//-----------------------------------------------------------------------------
class NavFramer
{

public:
   NavFramer();

   // This takes a tracker, just after it has been dumped and
   // accumulates the nav bit from it. It returns true when there is a
   // current HOW
   virtual long int process(const EMLTracker& tr, long int dp, float cPO);

   void dump(std::ostream& s, int detail=0) const;

   int debugLevel;
   

   // A subclass to help keep track of the subframes found
   struct Subframe
   {
      Subframe() : complete(false), words(10) {}

      double t;
      size_t ni;
      gpstk::CodeIndex ci;
      int dataPoint;
      int prn;
      float codePO;
      bool inverted;
      bool prevD30;
      bool complete;
      std::vector<uint32_t> words;
      void dump(std::ostream& s, int detail=0) const;
      bool checkParity(bool knownUpright=false) const;
      void load(const std::bitset<5 * 300>& bs);
      const char* checkWords() const;
   };
   std::list<Subframe> subframes;

private:
   // This buffer holds 5 300 bit subframes of nav data
   std::bitset<5 * 300> navBuffer;
   std::vector<gpstk::CodeIndex> codeIndex;
   std::vector<int> startDP; //data point of beginning of each nav bit
   std::vector<float> codePO;
   std::bitset<8> eightBaker, lastEight;
   size_t navIndex;
   gpstk::CodeIndex prevNavCount;
   
   // length of each bit
   double bitLength;

   std::list<Subframe> candidates;
   
   // Most recent how
   bool howCurrent;
   //gpstk::CodeIndex tlmIndex;
   uint32_t how;
};

std::ostream& operator<<(std::ostream& s, const NavFramer::Subframe& sf);

#endif
