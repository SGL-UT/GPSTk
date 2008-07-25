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

#include "EngNav.hpp"
#include "NavFramer.hpp"


using namespace gpstk;
using namespace std;


void NavFramer::Subframe::dump(std::ostream& s, int detail) const
{
   if (detail==0)
   {
      s << "t:" << fixed << setprecision(1) << t * 1e3
        << ", ni:" << ni
        << ", ci:" << ci
        << ", inv:" << inverted
        << ", prevD30:" << prevD30;
      
      if (!complete)
         return;
      if (checkParity())
         s << ", SFID:" << EngNav::getSFID(words[1])
           << ", Z:" << EngNav::getHOWTime(words[1]);
      else
         s << ", Parity:" << checkWords();
   }
   else
   {
      for (int w=0; w<10; w++)
         s << "# w[" << w << "]:" << bitset<30>(words[w]) << endl;
   }
}

std::ostream& operator<<(std::ostream& s, const NavFramer::Subframe& sf)
{
   sf.dump(s,0);
   return s;
}



bool NavFramer::Subframe::checkParity(bool knownUpright) const
{
   return EngNav::checkParity(words, false);
}


void NavFramer::Subframe::load(const std::bitset<5 * 300>& bs)
{
   bitset<30> word;
   for (int w=0; w<10; w++)
   {
      for(int b=0; b<30; b++)
         word[29-b] = bs.test((ni + w*30 + b)%1500);
      
      if (inverted)
         word = ~word;

      words[w] = word.to_ulong();
   }
   complete = true;
}


const char* NavFramer::Subframe::checkWords() const
{
   if (!complete)
      return string("??????????").c_str();

   string good;
   for (int w=0; w<10; w++)
   {
      uint32_t prev=0;
      if (w)
         prev = words[w-1];
      uint32_t par = EngNav::computeParity(words[w], prev, false);
      if (par == (words[w] & 0x3f))
         good.append("1");
      else
         good.append("0");
   }
   return good.c_str();
}


NavFramer::NavFramer()
   : prevNavCount(0), navIndex(0), howCurrent(false),inSync(false),
     codeIndex(5*300), eightBaker(0x8b), bitLength(20e-3)
{}


bool NavFramer::process(const EMLTracker& tr)
{
   // number of code chips that go into each bit
   const unsigned long chipsPerBit = 
      static_cast<unsigned long>(bitLength / tr.localReplica.codeChipLen);
   const CodeIndex now = tr.localReplica.codeGenPtr->getChipCount();
   const unsigned navCount = now/chipsPerBit;

   if (navCount == prevNavCount)
      return howCurrent;

   prevNavCount = navCount;
   navBuffer[navIndex] = tr.getNav();
   codeIndex[navIndex] = now;
   navIndex++;
   navIndex %= navBuffer.size();
   lastEight <<= 1;
   lastEight[0] = tr.getNav();

   if (debugLevel>2)
      cout << "# t:" << fixed << setprecision(2)
           << tr.localReplica.localTime *1e3
           << " ms, n:" << tr.getNav() << endl;

   if (lastEight == eightBaker || ~lastEight == eightBaker)
   {
      Subframe sf;
      sf.ni = (navIndex-8) % 1500;
      sf.ci = codeIndex[sf.ni];
      sf.prevD30 = navBuffer[(navIndex-9)%1500];
      sf.t = tr.localReplica.localTime;
      sf.inverted = lastEight != eightBaker;
      if (debugLevel>1)
         cout << "# " << sf << endl;
      candidates.push_back(sf);
   }
   
   list<Subframe>::iterator sf;
   for (sf = candidates.begin(); sf != candidates.end(); )
   {
      if (navIndex - sf->ni >= 300)
      {
         sf->load(navBuffer);
         if (sf->checkParity())
         {
            subframes.push_back(*sf);
            howCurrent = true;
            how = sf->words[1];
            if (debugLevel)
               cout << "# " << *sf << endl;
            if (debugLevel>1)
               sf->dump(cout,1);
         }
         else
         {
            howCurrent = false;
            if (debugLevel>1)
            {
               cout << "# " << *sf << endl;
               sf->dump(cout, 1);
            }
         }
         candidates.erase(sf++);
      }
      else
         sf++;
   }
   return howCurrent;
}


void NavFramer::dump(std::ostream& s, int detail) const
{
   if (detail>1)
      s << "# NavDump lastEight:" << lastEight << endl;
};
