//==============================================================================
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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file SEMData.hpp
 * Encapsulate SEM Almanac file data, including I/O
 */

#ifndef SEMDATA_HPP
#define SEMDATA_HPP

#include <vector>
#include <list>
#include <map>

#include "FFStream.hpp"
#include "AlmOrbit.hpp"
#include "OrbAlmGen.hpp"
#include "SEMBase.hpp"
#include "StringUtils.hpp"
#include "SEMHeader.hpp"

namespace gpstk
{
   /// @ingroup SEM 
   //@{

      /** 
       * This class stores, reads, and writes SEM records. 
       * @warning The SEM header information and data information don't
       * correctly talk to each other at the time of completion of this file.
       * The current fix is in SEMAlamanacStore.hpp.
       *
       * @sa tests/SEM for examples
       * @sa SEMStream.
       * @sa SEMHeader for information on writing SEM files.
       */
   class SEMData : public SEMBase
   {
   public:
   
         /// Constructor.
      SEMData() {}

         /// Destructor
      virtual ~SEMData() {}
      
         
      short PRN;
      short SVNnum;          // SVN
      short URAnum;          // "Avg" URA index over unknown period
      double ecc;            // no units
      double i_offset;       // redians
      double i_total;        // radians
      double OMEGAdot;       // redians
      double Ahalf;          // m**0.5
      double OMEGA0;         // radians
      double w;              // radians
      double M0;             // radians
      double AF0;            // sec
      double AF1;            // sec/sec 
      short SV_health;
      short satConfig;
      
      long xmit_time;
      
      long Toa;
      short week;
      
         /**
          * Debug output function. 
          * Dump the contents of each of the SEM class to a
          * given ostream \c s.
          */ 
      virtual void dump(std::ostream& s) const;
      
         //! This class is "data" so this function always returns "true". 
      virtual bool isData() const {return true;}

         /**
          * cast *this into an AlmOrbit
          * @return the constructed AlmOrbit object
          */
      operator AlmOrbit() const;
      
         /**
          * cast *this into an OrbAlmGen
          * @return the constructed OrbAlmGen object
          */
      operator OrbAlmGen() const;    
            
   protected:      
	 
	 /**
          * Writes a correctly formatted record from this data to stream \a s.
          * @throw std::exception
          * @throw FFStreamError
          * @throw StringUtils::StringException
          */
      virtual void reallyPutRecord(FFStream& s) const;
  
         /**
          * This functions obtains a SEM almanac record from the given 
          * FFStream.
          * If there is an error in reading from the stream, it is reset
          * to its original position and its fail-bit is set.
          * @throw std::exception
          * @throw StringException when a StringUtils function fails
          * @throw FFStreamError when exceptions(failbit) is set and
          *  a read or formatting error occurs.  This also resets the
          *  stream to its pre-read position.
          */
      virtual void reallyGetRecord(FFStream& s);
      
   }; // class SEMData

   //@}

} // namespace

#endif
