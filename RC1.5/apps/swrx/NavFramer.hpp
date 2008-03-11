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
   virtual bool process(const EMLTracker& tr);

   void dump(std::ostream& s, int detail=0) const;

   int debugLevel;

   // A subclass to help keep track of the subframes found
   struct Subframe
   {
      Subframe() : words(10), complete(false) {}
      double t;
      size_t ni;
      gpstk::CodeIndex ci;
      bool inverted;
      bool prevD30;
      bool complete;
      std::vector<uint32_t> words;
      void dump(std::ostream& s, int detail=0) const;
      bool checkParity(bool knownUpright=false) const;
      void load(const std::bitset<5 * 300>& bs);
      const char* checkWords() const;
   };

private:
   // This buffer holds 5 300 bit subframes of nav data
   std::bitset<5 * 300> navBuffer;
   std::vector<gpstk::CodeIndex> codeIndex;
   std::bitset<8> eightBaker, lastEight;
   size_t navIndex;
   gpstk::CodeIndex prevNavCount;

   // length of each bit
   double bitLength;

   std::list<Subframe> candidates;
   std::list<Subframe> subframes;

   // This is used to indicate that we have found a TLM and HOW
   // that have passed parity
   bool inSync;

   // Most recent how
   bool howCurrent;
   gpstk::CodeIndex tlmIndex;
   uint32_t how;
};

std::ostream& operator<<(std::ostream& s, const NavFramer::Subframe& sf);

#endif
