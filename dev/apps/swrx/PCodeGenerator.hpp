#pragma ident "$Id$"

#ifndef PCODEGENERATOR_H
#define PCODEGENERATOR_H

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
