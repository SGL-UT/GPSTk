#pragma ident "$Id$"


/**
 * @file SP3Data.hpp
 * Encapsulate SP3 file data, including I/O
 */

#ifndef GPSTK_SP3DATA_HPP
#define GPSTK_SP3DATA_HPP

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






#include "Triple.hpp"
#include "SP3Base.hpp"

namespace gpstk
{
   /** @addtogroup SP3ephem */
   //@{

      /**
       * This class models Satellite Position in Three Dimensions.
       *
       * @sa gpstk::SP3Header and gpstk::SP3Stream for more information.
       * @sa sp3_test.cpp for an example.
       */
   class SP3Data : public SP3Base
   {
   public:
         /// Constructor.
      SP3Data() : flag('\000'), time(gpstk::DayTime::BEGINNING_OF_TIME) 
         {}
     
         /// Destructor
      virtual ~SP3Data() {}
     
         // The next four lines is our common interface
         /// SP3Data is "data" so this function always returns true.
      virtual bool isData() const {return true;}

         /// Debug output function.
      virtual void dump(std::ostream& s) const;

         ///@name data members
         //@{
      char flag;  ///< Data type indicator.  P for position, V for velocity.
      short id;   ///< Satellite ID (usually prn)
      Triple x;   ///< The three-vector for position or velocity. @see flag
      double  clk; ///< The time bias (clock bias) for P, or clock drift for V. @see x
      DayTime time; ///< Time of epoch for this record
         //@}
      
   protected:
         /// Writes the formatted record to the FFStream \a s.
         /// @warning This function is currently unimplemented
      virtual void reallyPutRecord(FFStream& s) const 
         throw(std::exception, FFStreamError,
               gpstk::StringUtils::StringException);

         /**
          * This function reads a record from the given FFStream.
          * If an error is encountered in retrieving the record, the 
          * stream is reset to its original position and its fail-bit is set.
          * @throws StringException when a StringUtils function fails
          * @throws FFStreamError when exceptions(failbit) is set and
          *  a read or formatting error occurs.  This also resets the
          *  stream to its pre-read position.
          */
      virtual void reallyGetRecord(FFStream& s) 
         throw(std::exception, FFStreamError,
               gpstk::StringUtils::StringException);
   };

   //@}

}  // namespace

#endif
