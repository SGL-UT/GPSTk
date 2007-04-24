#pragma ident "$Id$"


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
   // accumulates the nav bit from it. It returns true when a complete
   // nav subframe is ready
   virtual bool process(const EMLTracker& tr);

   void dump(std::ostream& s, int detail=0) const;

   int debugLevel;

private:
   // This buffer holds 5 300 bit subframes of nav data
   std::bitset<5 * 300> navBuffer;
   std::vector<gpstk::CodeIndex> codeIndex;
   std::bitset<8> eightBaker, notEightBaker, lastEight;
   size_t navIndex;
   gpstk::CodeIndex prevNavCount;

   std::list<size_t> tlmCandidates;
   std::map< gpstk::CodeIndex, std::vector<uint32_t> > subframes;

   // Most recent tlm and how
   bool tlmCurrent;
   gpstk::CodeIndex tlmIndex;
   uint32_t tlm,how;

   void cookSubframe(std::vector<uint32_t> sf) throw();
};

#endif
