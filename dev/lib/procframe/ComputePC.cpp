
/**
 * @file ComputePC.cpp
 * This class eases computing PC combination for GNSS data structures.
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


#include "ComputePC.hpp"


namespace gpstk
{

    // Index initially assigned to this class
    int ComputePC::classIndex = 1600000;


    // Returns an index identifying this object.
    int ComputePC::getIndex() const { return (*this).index; }


    // Returns a string identifying this object.
    std::string ComputePC::getClassName() const { return "ComputePC"; }



} // end namespace gpstk
