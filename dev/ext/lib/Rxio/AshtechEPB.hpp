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

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

#ifndef ASHTECHEPB_HPP
#define ASHTECHEPB_HPP

/**
 * @file AshtechEPB.hpp
 * gpstk::AshtechEPB - Class to hold an Ashtech EPB message
 */

#include "AshtechData.hpp"

namespace gpstk
{
   class AshtechEPB : public AshtechData
   {
   public:
      
      AshtechEPB() {};

      std::string header; // 11 characters exactly

      unsigned prn;

      // Note that an extra element is allocated so these can be
      // numbered as they are in the IS-GPS-200. Therefore
      // element 0 of each index is not used.
      long     word[4][11];

      static const char* myId;

      virtual std::string getName() const {return "epb";}
      
      bool checkId(std::string hdrId) const {return hdrId==myId;}

      void dump(std::ostream& out) const throw();

      virtual void decode(const std::string& data)
         throw(std::exception, FFStreamError);

   protected:
      virtual void reallyGetRecord(FFStream& ffs)
         throw(std::exception, FFStreamError, EndOfFile);
   };

} // namespace gpstk

#endif
