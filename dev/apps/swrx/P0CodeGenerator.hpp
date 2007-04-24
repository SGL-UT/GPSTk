#pragma ident "$Id$"

#ifndef P0CODEGENERATOR_H
#define P0CODEGENERATOR_H

#include "SVPCodeGen.hpp"
#include "CodeBuffer.hpp"
#include "DayTime.hpp"

#include "CodeGenerator.hpp"

namespace gpstk
{
   class P0CodeGenerator : public CodeGenerator
   {
   public:
      P0CodeGenerator(const int prn)
         : CodeGenerator(ObsID::tcP, SatID(prn, SatID::systemGPS))
      {setIndex(0);}

      bool operator*() const { return false; }

      CodeIndex operator++() { return getIndex(); }
 
      CodeIndex setIndex(CodeIndex new_index)
      {
         return getIndex();
      }

      CodeIndex getIndex() const
      {
         return 0;
      }

      CodeIndex getSyncIndex() const
      {
         return 0;
      }

      CodeIndex getChipCount() const {return getIndex();}

      void reset() { setIndex(0);}

   private:

   };
}
#endif
