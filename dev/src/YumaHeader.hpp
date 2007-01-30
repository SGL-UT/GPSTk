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
 * @file YumaHeader.hpp
 * Encapsulate Yuma Almanac header, including I/O
 */

#ifndef YUMAHEADER_HPP
#define YUMAHEADER_HPP

#include <vector>
#include <list>
#include <map>

#include "FFStream.hpp"
#include "AlmOrbit.hpp"
#include "YumaBase.hpp"
#include "StringUtils.hpp"

namespace gpstk
{
   /** @addtogroup Yuma */
   //@{

      /** 
       * This class does not really do anything.  It is here to conform to the
       * other file types, even though the Yuma file type 
       * does not have any header information.
       *
       * @sa tests/Yuma for examples
       * @sa YumaStream.
       * @sa YumaData for more information on writing Yuma files.
       */
   class YumaHeader : public YumaBase
   {
   public:
         /// Constructor.
      YumaHeader() {}

         /// Destructor
      virtual ~YumaHeader() {}
      

         /**
          * Debug output function. 
          * Dump the contents of each of the Yuma header to a
          * given ostream \c s.
          */ 
      virtual void dump(std::ostream& s) const;
      
         //! This class is a "header" so this function always returns "true". 
      virtual bool isHeader() const {return true;}
   
      

   protected:      
	 /**
          * Writes a correctly formatted record from this header to stream \a s.
          */
      virtual void reallyPutRecord(FFStream& s) const 
         throw(std::exception, FFStreamError, 
               gpstk::StringUtils::StringException);  
  
         /**
          * This functions obtains a Yuma header record from the given 
          * FFStream.
          * If there is an error in reading from the stream, it is reset
          * to its original position and its fail-bit is set.
          * @throws StringException when a StringUtils function fails
          * @throws FFStreamError when exceptions(failbit) is set and
          *  a read or formatting error occurs.  This also resets the
          *  stream to its pre-read position.
          */
      virtual void reallyGetRecord(FFStream& s) 
         throw(std::exception, FFStreamError, 
               gpstk::StringUtils::StringException);  
      
   }; // class YumaHeader

   //@}

} // namespace

#endif
