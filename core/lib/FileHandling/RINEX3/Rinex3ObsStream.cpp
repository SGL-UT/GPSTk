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
 * @file Rinex3ObsStream.cpp
 * File stream for RINEX 3 observation file data.
 */

#include "Rinex3ObsStream.hpp"

namespace gpstk
{
   Rinex3ObsStream ::
   Rinex3ObsStream()
   {
      init();
   }


   Rinex3ObsStream ::
   Rinex3ObsStream( const char* fn,
                    std::ios::openmode mode )
         : FFTextStream(fn, mode)
   {
      init();
   }


   Rinex3ObsStream ::
   Rinex3ObsStream( const std::string fn,
                    std::ios::openmode mode )
         : FFTextStream(fn.c_str(), mode)
   {
      init();
   }


   Rinex3ObsStream ::
   ~Rinex3ObsStream()
   {
   }


   void Rinex3ObsStream ::
   open( const char* fn,
         std::ios::openmode mode )
   {
      FFTextStream::open(fn, mode);
   }


   void Rinex3ObsStream ::
   init()
   {
      headerRead = false;
      header = Rinex3ObsHeader();
      timesystem = TimeSystem::GPS;
   }


   void Rinex3ObsStream ::
   open( const std::string& fn,
         std::ios::openmode mode )
   {
      open(fn.c_str(), mode);
   }


   bool Rinex3ObsStream ::
   isRinex3ObsStream(std::istream& i)
   {
      try
      {
         (void)dynamic_cast<Rinex3ObsStream&>(i);
      }
      catch(...)
      {
         return false;
      }

      return true;
   }

} // namespace gpstk
