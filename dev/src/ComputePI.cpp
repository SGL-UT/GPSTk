
/**
 * @file ComputePI.cpp
 * This class eases computing PI combination for GNSS data structures.
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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007
//
//============================================================================


#include "ComputePI.hpp"


namespace gpstk
{

    // Index initially assigned to this class
    int ComputePI::classIndex = 1800000;


    // Returns an index identifying this object.
    int ComputePI::getIndex() const { return (*this).index; }


    // Returns a string identifying this object.
    std::string ComputePI::getClassName() const { return "ComputePI"; }



} // end namespace gpstk
