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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
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
      if (!complete)
         return;
      if (checkParity())
         s << endl << fixed
           << "# PRN:" << prn 
           << setprecision(2) << ", SFID:" << EngNav::getSFID(words[1])
           << ", Z:" << EngNav::getHOWTime(words[1])
           << ", Start Data Point:" << dataPoint << endl;
      else
         s << ", Parity:" << checkWords();
      if(0) // not using this output for now.
      s << "# RxTime: " << (float)dataPoint / 16368 << " ms"
        << ", CodePO: " << codePO << " us" << endl
        << "# t:" << t * 1e3
        << ", ni:" << ni
        << ", ci:" << ci
        << ", inv:" << inverted
        << ", prevD30:" << prevD30;
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
   return EngNav::checkParity(words, /*true*/false);
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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreorder"
NavFramer::NavFramer()
   : prevNavCount(0), navIndex(0), howCurrent(false),inSync(false),
     codeIndex(5*300), eightBaker(0x8b), bitLength(20e-3)
{}
#pragma clang diagnostic pop

long int NavFramer::process(const EMLTracker& tr, long int dp, float cPO)
{
   // number of code chips that go into each bit
   const unsigned long chipsPerBit = 
      static_cast<unsigned long>(bitLength / tr.localReplica.codeChipLen);
   
   const CodeIndex now = tr.localReplica.codeGenPtr->getChipCount();
   const unsigned navCount = now/chipsPerBit;
   
// Code below can be uncommented if the NavFramer needs to count on it's own 
// to know when there is a new nav bit.  Right now it is only called when the
// tracker says there is one.
/*
   if (navCount == prevNavCount)
   {
         //cout << "skipping" << endl;
      return howCurrent;
   }
*/

      //cout << "doing" << endl;
   howCurrent = false;
   prevNavCount = navCount;
   navBuffer[navIndex] = tr.getNav();
   codeIndex[navIndex] = now;
   startDP.push_back(dp);
   codePO.push_back(cPO);
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
     //cout << "FOUND CANDIDATE" << endl ;
      Subframe sf;
      sf.ni = (navIndex-8) % 1500;
      sf.ci = codeIndex[sf.ni];
      sf.dataPoint = startDP[sf.ni];
      sf.prn = tr.prn;
      sf.codePO = codePO[sf.ni];
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
            how = sf->words[1];
               //if (debugLevel)
               //cout << *sf << endl;
               //if (debugLevel)
               //sf->dump(cout,1);
               howCurrent = true;

// Following block pulls nav data from subframes, just playing around for now.
/*
               long framesArrayFormat[10];
               int gpsWeek = 1433;
               double output[60];
               for(int k = 0; k < 10; k++)
               {
                  framesArrayFormat[k] = sf->words[k];
               }
               EngNav::subframeConvert(framesArrayFormat, gpsWeek, output);
   
               for(int i = 0; i < 60; i++)
                  cout << output[i] << endl;
*/
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
