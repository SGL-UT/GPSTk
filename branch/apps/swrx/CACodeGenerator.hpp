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

#ifndef CACODEGENERATOR_HPP
#define CACODEGENERATOR_HPP

#include <iostream>

#include "CodeGenerator.hpp"
#include "ConstLinearRecurrentSequence.hpp"

namespace gpstk
{
   class CACodeGenerator : public CodeGenerator
   {
   public:
      CACodeGenerator(int prn)
         : CodeGenerator(ObsID::tcCA, SatID(prn, SatID::systemGPS)),
           g2Offset(getG2Delay(prn))
      {setIndex(0);}

      bool operator*() const { return (*G1)^(*G2); }

      CodeIndex operator++() { ++G1; ++G2; ++chipCount; return getIndex(); }
 
      CodeIndex setIndex(CodeIndex new_index)
      {
         G1.setIndex(new_index);
         G2.setIndex(new_index + codeLen - g2Offset);
         chipCount = 0;
         return getIndex();
      }

      CodeIndex getIndex() const
      { return G1.getIndex(); }

      bool isLastInSequence() const
      { return G1.isLastInSequence(); }

      CodeIndex getSyncIndex() const {return codeLen;}

      CodeIndex getChipCount() const {return chipCount;}

      void reset() {setIndex(0);}

   private:
      const static int G1_InitialVector=0x3FF;
      const static int G2_InitialVector=0x3FF;

      const static int G1_Polynomial=(1<<10)|(1<<3);
      const static int G2_Polynomial=(1<<10)|(1<<9)|(1<<8)|(1<<6)|(1<<3)|(1<<2);
   
      const static int codeLen = 1023;

      CodeIndex chipCount;

      ConstLinearRecurrentSequence<codeLen, G1_InitialVector, G1_Polynomial> G1;

      ConstLinearRecurrentSequence<codeLen, G2_InitialVector, G2_Polynomial> G2;

      const CodeIndex g2Offset;

      static CodeIndex getG2Delay(int prn)
      {
         const CodeIndex G2DelayTable[]={
            0,5,6,7,8,17,18,139,140,141,251,252,254,255,256,
            257,258,469,470,471,472,473,474,509,512,513,514,
            515,516,859,860,861,862,863,950,947,948,950
         };
         return G2DelayTable[prn];
      }
   };
}

#endif
