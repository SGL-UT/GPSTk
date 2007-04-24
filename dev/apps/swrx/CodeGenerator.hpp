#pragma ident "$Id$"

#ifndef CODEGENERATOR_HPP
#define CODEGENERATOR_HPP

#include "ObsID.hpp"
#include "SatID.hpp"

namespace gpstk
{
   typedef unsigned long CodeIndex;

   class CodeGenerator 
   {
   public:
   
      CodeGenerator(const ObsID::TrackingCode code, const SatID& sv)
      : code(code), sv(sv)
      {}

      // Returns the current chip 
      virtual bool operator*() const = 0;
      virtual CodeIndex operator++() = 0;
 
      virtual CodeIndex setIndex(CodeIndex new_index) = 0;
      virtual CodeIndex getIndex() const = 0;

      // This returns the total chips generated since the last set/reset;
      virtual CodeIndex getChipCount() const = 0;

      // This is used to get a clue as to where in the code the integrate
      // and dump interval should be alligned
      virtual CodeIndex getSyncIndex() const = 0;

      virtual void reset() {setIndex(0);}

      const ObsID::TrackingCode code;
      const SatID sv;
   };

}
#endif
