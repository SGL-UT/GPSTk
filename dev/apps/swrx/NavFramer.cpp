#pragma ident "$Id$"

#include "EngNav.hpp"
#include "NavFramer.hpp"


using namespace gpstk;
using namespace std;


void NavFramer::Subframe::dump(std::ostream& s, int detail) const
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

std::ostream& operator<<(std::ostream& s, const NavFramer::Subframe& sf)
{
   sf.dump(s,0);
}



bool NavFramer::Subframe::checkParity(bool knownUpright) const
{
   return EngNav::checkParity(words, false);
}


void NavFramer::Subframe::load(const std::bitset<5 * 300> bs)
{
   bitset<30> word;
   for (int w=0; w<10; w++)
   {
      for(int b=0; b<30; b++)
         word[29-b] = bs[ni + w*30 + b];

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
     codeIndex(5*300), eightBaker(0x8b)
{}


bool NavFramer::process(const EMLTracker& tr)
{
   const CodeIndex now = tr.localReplica.codeGenPtr->getChipCount();
   const unsigned caCount = now/1023;
   const unsigned navCount = caCount/20;

   if (navCount == prevNavCount)
      return howCurrent;

   prevNavCount = navCount;
   navBuffer[navIndex] = tr.getNav();
   codeIndex[navIndex] = now;
   navIndex++;
   navIndex %= navBuffer.size();
   lastEight <<= 1;
   lastEight[0] = tr.getNav();

   if (lastEight == eightBaker || ~lastEight == eightBaker)
   {
      Subframe sf;
      sf.ni = navIndex-8;
      sf.ci = codeIndex[navIndex-8];
      sf.prevD30 = navBuffer[navIndex-9];
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
         }
         else
         {
            howCurrent = false;
            if (debugLevel>1)
               cout << "# " << *sf << endl;
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
