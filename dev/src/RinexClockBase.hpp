#pragma ident "$Id$"

/**
 * @file RinexClockBase.hpp
 * Base class for RINEX3 clock data file 
 */

#ifndef GPSTK_RINEX3CLOCKBASE_HPP
#define GPSTK_RINEX3CLOCKBASE_HPP

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
//  Octavian Andrei - FGI ( http://www.fgi.fi ). 2008
//
//============================================================================

// GPSTk
#include "FFData.hpp"

namespace gpstk
{

   /** @defgroup RinexClock RINEX3 clock files */
   //@{

     /** This level isn't serving any purpose other than to make
      * the class diagram look nice...
      */
   class RinexClockBase : public FFData
   {
   public:

         /// Destructor per the coding standards
      virtual ~RinexClockBase() {}

   }; // End of class 'RinexClockBase'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_RINEX3CLOCKBASE_HPP
