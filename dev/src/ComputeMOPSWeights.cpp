
/**
 * @file ComputeMOPSWeights.cpp
 * This class computes satellites weights based on the Appendix J of MOPS C, and is meant to be used with GNSS data structures.
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


#include "ComputeMOPSWeights.hpp"


namespace gpstk
{

    // Index initially assigned to this class
    int ComputeMOPSWeights::classIndex = 4100000;


    // Returns an index identifying this object.
    int ComputeMOPSWeights::getIndex() const { return (*this).index; }


    // Returns a string identifying this object.
    std::string ComputeMOPSWeights::getClassName() const { return "ComputeMOPSWeights"; }



} // end namespace gpstk
