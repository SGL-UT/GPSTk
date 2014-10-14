//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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


/**
 * @file AshtechALB.hpp
 * gpstk::AshtechALB - class to hold an Ashtech ALB message
 */

#ifndef ASHTECHALB_HPP
#define ASHTECHALB_HPP

#include "AshtechData.hpp"

namespace gpstk
{
   class AshtechALB : public AshtechData
   {
   public:
      
      AshtechALB() {};

      std::string header; // 11 characters exactly

      unsigned svid;
      long     word[10];

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
