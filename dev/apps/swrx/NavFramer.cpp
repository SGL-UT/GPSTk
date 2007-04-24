#pragma ident "$Id$"

#include "EngNav.hpp"
#include "NavFramer.hpp"


using namespace gpstk;
using namespace std;


NavFramer::NavFramer()
   : prevNavCount(0), navIndex(0), tlmCurrent(false),
     codeIndex(5*300)
{
   eightBaker = 0x8b;
   notEightBaker = ~eightBaker;
}


bool NavFramer::process(const EMLTracker& tr)
{
   const CodeIndex now = tr.localReplica.codeGenPtr->getChipCount();
   const unsigned caCount = now/1023;
   const unsigned navCount = caCount/20;

   if (navCount == prevNavCount)
      return false;

   prevNavCount = navCount;
   navBuffer[navIndex] = tr.getNav();
   codeIndex[navIndex] = now;
   navIndex++;
   navIndex %= navBuffer.size();
   lastEight <<= 1;
   lastEight[0] = tr.getNav();
   bool found = lastEight == eightBaker || lastEight == notEightBaker;
   
   if (found)
   {
      if (debugLevel)
         cout << left
              << "# navIndex: " << setw(6) << navIndex-8
              << "  caCount: " << setw(4) << codeIndex[navIndex-8]/1023/20
              << "  lastEight: " << lastEight
              << endl;
      tlmCandidates.push_back(navIndex-8);
   }
   
   list<size_t>::iterator tlm;
   for (tlm = tlmCandidates.begin(); tlm != tlmCandidates.end(); )
   {
      if (navIndex - *tlm >= 300)
      {
         uint32_t sf[10];
         bitset<30> word;
         for (int w=0; w<10; w++)
         {
            size_t ni = *tlm + w*30;
            for(int b=0; b<30; b++)
               word[29-b] = navBuffer[ni + b];
            sf[w] = word.to_ulong();
            if (debugLevel>2)
            {
               uint32_t prev=0;
               if (w)
                  prev = sf[w-1];
               cout << "# navBuffer[" << ni << "]:" << word;
               uint32_t par = EngNav::computeParity(sf[w], prev, false);
               if (par == (sf[w] & 0x3f))
                  cout << " Good" << endl;
               else
                  cout << " Bad" << endl;
            }
         }
         bool good = EngNav::checkParity(sf, false);
         if (good)
         {
            if (debugLevel)
               cout << "# Found subframe at navIndex=" << *tlm 
                    << ", caCount=" << codeIndex[*tlm] << endl;
            subframes[codeIndex[*tlm]] = vector<uint32_t>(10);
            for (int i=0; i<10; i++)
               subframes[codeIndex[*tlm]][i] = sf[i];
         }
         tlmCandidates.erase(tlm++);
      }
      else
         tlm++;
   }

   return false;
}


void NavFramer::dump(std::ostream& s, int detail) const
{
   if (detail>1)
      s << "# NavDump lastEight:" << lastEight << endl;
};
