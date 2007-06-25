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

#ifndef MDPSELFTESTSTATUS_HPP
#define MDPSELFTESTSTATUS_HPP

#include "Xvt.hpp"

#include "MDPHeader.hpp"

namespace gpstk
{
   /// This class represents a runtime selftest status message
   class MDPSelftestStatus : public MDPHeader
   {
   public:
      MDPSelftestStatus() throw();

      /**  Encode this object to a string.
       * @return this object as a string.
       */
      virtual std::string encode() const
         throw();
         
      /** Decode this object from a string.
       * @param str the string to read from.
       * @note This is a non-destructive decode.
       */
      virtual void decode(std::string str)
         throw();
         
      virtual std::string getName() const {return "sts";};

      /** Dump some debugging information to the given ostream.
       * @param out the ostream to write to
       */
      void dump(std::ostream& out) const throw();

      float antennaTemp;
      float receiverTemp;
      long status;
      float cpuLoad;
      gpstk::DayTime selfTestTime; ///< SelfTest SOW & week
      gpstk::DayTime firstPVTTime; ///< First PVT SOW & week
      short extFreqStatus;
      short saasmStatusWord;

      static const unsigned myLength = 32;
      static const unsigned myId = 400;

   }; // class MDPSelftestStatus
      
} // namespace gpstk

#endif //MDPSELFTESTSTATUS_HPP
