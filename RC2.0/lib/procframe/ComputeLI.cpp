#pragma ident "$Id$"

/**
 * @file ComputeLI.cpp
 * This class eases computing LI combination for GNSS data structures.
 */

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2011
//
//============================================================================


#include "ComputeLI.hpp"


namespace gpstk
{

      // Returns a string identifying this object.
   std::string ComputeLI::getClassName() const
   { return "ComputeLI"; }


      // Default constructor
   ComputeLI::ComputeLI()
   {
      type1 = TypeID::L1;
      type2 = TypeID::L2;
      resultType = TypeID::LI;
   }


} // End of namespace gpstk
