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

/**
 * @file YumaHeader.cpp
 * Encapsulate Yuma header almanac file data, including I/O
 */

#include "StringUtils.hpp"
#include "icd_200_constants.hpp"

#include "YumaHeader.hpp"
#include "YumaStream.hpp"


using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
   /// There is actually no Yuma Header information, it was just much
   /// easier to use this format for Yuma storage.
   
   void YumaHeader::reallyPutRecord(FFStream& ffs) const 
      throw(std::exception, FFStreamError, 
               gpstk::StringUtils::StringException)  
   {
            
   }   // end YumaAHeader::reallyPutRecord


   void YumaHeader::reallyGetRecord(FFStream& ffs) 
      throw(std::exception, FFStreamError, 
               gpstk::StringUtils::StringException)  
   {
     
      
   } // end of reallyGetRecord()

   void YumaHeader::dump(ostream& s) const 
   {      
      
   }
   
} // namespace
