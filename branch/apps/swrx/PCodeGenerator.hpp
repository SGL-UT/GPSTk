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

#ifndef PCODEGENERATOR_HPP
#define PCODEGENERATOR_HPP

#include "SVPCodeGen.hpp"
#include "CodeBuffer.hpp"
#include "DayTime.hpp"

#include "CodeGenerator.hpp"

namespace gpstk
{
   class PCodeGenerator : public CodeGenerator
   {
   public:
      PCodeGenerator(const int prn)
         : CodeGenerator(ObsID::tcP, SatID(prn, SatID::systemGPS)),
           cb(prn), svp(prn, gpstk::GPSZcount(0)), index(0)
      {
         svp.getCurrentSixSeconds(cb);
      }

      bool operator*() const { return cb.getBit(index) & 0x1; }

      CodeIndex operator++() { index++; handleWrap(); return getIndex(); }

      CodeIndex setIndex(CodeIndex new_index)
      {
         unsigned long z = new_index / (15345000*4);
         z *= 4;
         if (svp.getCurrentZCount().fullZcountFloor() != z)
         {
            std::cerr << "Regen cb" << std::endl;
            svp.setCurrentZCount(z);
            svp.getCurrentSixSeconds(cb);
         }
         index = new_index % (15345000*4);
         return getIndex();
      }

      CodeIndex getIndex() const
      {
         unsigned long z = svp.getCurrentZCount().fullZcountFloor();
         return index + z * 15345000;
      }

      bool isLastChipofX1Sequence() const
      { return (index%15345000)==15344999; }


      CodeIndex getSyncIndex() const {return 10230;}

      CodeIndex getChipCount() const {return getIndex();}

      void reset() { setIndex(0);}

   private:

      inline int handleWrap()
      {
         while(index>=15345000*4)
         {
            index-=15345000*4;
            svp.increment4ZCounts();
            svp.getCurrentSixSeconds(cb);
         }
      }

      inline static void initXSeq() __attribute__ ((constructor))
      {
         try
         {
            gpstk::X1Sequence::allocateMemory();
            gpstk::X2Sequence::allocateMemory();
         }
         catch (...)
         {}
      }

      CodeBuffer cb;
      SVPCodeGen svp;
      CodeIndex index;
   };
}
#endif
