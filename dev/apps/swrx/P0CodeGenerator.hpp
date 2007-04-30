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
