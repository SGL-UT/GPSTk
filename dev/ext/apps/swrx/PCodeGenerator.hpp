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

#ifndef PCODEGENERATOR_HPP
#define PCODEGENERATOR_HPP

#include "SVPCodeGen.hpp"
#include "CodeBuffer.hpp"
#include "CommonTime.hpp"
#include "GPSWeekZcount.hpp"
#include "Epoch.hpp"
#include "CodeGenerator.hpp"

/* If we're not using GNU C, elide __attribute__ */
#if !defined( __GNUC__ ) || ( __GNUC__ < 4 )
#  define  __attribute__(x)  /*NOTHING*/
#endif

namespace gpstk
{
   class PCodeGenerator : public CodeGenerator
   {
   public:
      PCodeGenerator(const int prn)
         : CodeGenerator(ObsID::tcP, SatID(prn, SatID::systemGPS)),
           cb(prn), svp(int(prn), GPSWeekZcount(0,0).convertToCommonTime()), index(0)
      {
         svp.getCurrentSixSeconds(cb);
      }

      bool operator*() const { return cb.getBit(index) & 0x1; }

      CodeIndex operator++() { index++; handleWrap(); return getIndex(); }

      CodeIndex setIndex(CodeIndex new_index)
      {
         unsigned long z = new_index / (15345000*4);
         z *= 4;
         if (static_cast<Epoch>(svp.getCurrentZCount()).GPSzcount32Floor() != z)
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
         unsigned long z = static_cast<Epoch>(svp.getCurrentZCount()).GPSzcount32Floor();
         return index + z * 15345000;
      }

      bool isLastChipofX1Sequence() const
      { return (index%15345000)==15344999; }


      CodeIndex getSyncIndex() const {return 10230;}

      CodeIndex getChipCount() const {return getIndex();}

      void reset() { setIndex(0);}

   private:

      inline void handleWrap()
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
