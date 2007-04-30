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
