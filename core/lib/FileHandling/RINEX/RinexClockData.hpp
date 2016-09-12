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
//  Copyright 2015, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
// This software developed by Applied Research Laboratories at the
// University of Texas at Austin, under contract to an agency or
// agencies within the U.S.  Department of Defense. The
// U.S. Government retains all rights to use, duplicate, distribute,
// disclose, or release this software.
//
// Pursuant to DoD Directive 523024
//
// DISTRIBUTION STATEMENT A: This software has been approved for public
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file RinexClockData.hpp
 * Encapsulate RINEX clock file data, including I/O
 */

#ifndef GPSTK_RINEXCLOCKDATA_HPP
#define GPSTK_RINEXCLOCKDATA_HPP

#include <string>
#include <vector>
#include "RinexClockBase.hpp"
#include "CivilTime.hpp"

namespace gpstk
{
      /// @ingroup FileHandling
      //@{
   
      /**
       * This class models a RINEX Clock Data Record.
       *
       * @sa gpstk::RinexClockStream and gpstk::RinexClockHeader.
       */   
   class RinexClockData : public RinexClockBase
   {
   public:
      

         /// Constructor
      RinexClockData(): epochTime(CommonTime::BEGINNING_OF_TIME),
                        dvCount(0), clockData(6,0) {}
      
         /// Destructor
      virtual ~RinexClockData() {}

         /// clock data type 
         /// (AR, AS, CR, DR, MS)
      RinexClkType type;
      
         /// receiver or satellite name
         /// IGS 4 char receiver designation or 
         /// 3 char satellite designation (Gnn for GPS) (Rnn for GLONASS) 
      std::string name;
      
         /// Epoch in GPS time
      CivilTime epochTime;
      
         /// number of data values
      short dvCount;
      
         /// clock data
         /// [0] Clock bias (seconds).
         /// [1] Clock bias sigma [optional] (seconds).
         /// [2] Clock rate [optional] (dimensionless).
         /// [3] Clock rate sigma [optional] (dimensionless).
         /// [4] Clock acceleration [optional] (per second).
         /// [5] Clock acceleration sigma [optional] (per second).
      std::vector<double> clockData;

         /// RinexClockData is a "data", so this function always returns 
         /// true.
      virtual bool isData() const { return true; }
      
         /// Debug output function. 
      virtual void dump(std::ostream& s) const;
      
   protected:
         /// Writes a correctly formatted record from this data to stream
      virtual void reallyPutRecord(FFStream& s) const
         throw(std::exception, FFStreamError,
               StringUtils::StringException);
      
         
         /// This functions obtains a RINEX Clock record from the given stream
      virtual void reallyGetRecord(FFStream& s)
         throw(std::exception, FFStreamError,
               StringUtils::StringException);
      
   }; // class RinexClockData
   
      //@}
   
} // namespace gpstk

#endif  //  GPSTK_RINEXCLOCKDATA_HPP

      

