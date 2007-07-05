
/**
 * @file XYZ2NEU.cpp
 * This is a class to change the reference base from XYZ to North-East-Up (NEU)
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


#include "XYZ2NEU.hpp"


namespace gpstk
{

    // Returns a reference to a satTypeValueMap object after converting from a geocentric reference
    // system to a topocentric reference system.
    //
    // @param gData     Data object holding the data.
    //
    satTypeValueMap& XYZ2NEU::Convert(satTypeValueMap& gData)
    {
        Matrix<double> neuMatrix;

        // Get the corresponding geometry/design matrix data
        Matrix<double> dMatrix(gData.getMatrixOfTypes((*this).inputSet));

        // Compute the base change. For convenience, we use the property:
        // Y = A*B => Y^T = (A*B)^T => Y^T = B^T * A^T
        neuMatrix = dMatrix*rotationMatrix;

        gData.insertMatrix(outputSet, neuMatrix);

        return gData;

    }  // end XYZ2NEU::Convert()


    // This method builds the rotation matrix according to refLat and refLon values.
    void XYZ2NEU::Prepare()
    {
        // First, let's resize rotation matrix and assign the proper values
        rotationMatrix.resize(3,3);

        // The clasical rotation matrix is transposed here for convenience
        rotationMatrix(0,0) = -std::sin(refLat)*std::cos(refLon);
        rotationMatrix(1,0) = -std::sin(refLat)*std::sin(refLon);
        rotationMatrix(2,0) = std::cos(refLat);
        rotationMatrix(0,1) = -std::sin(refLon);
        rotationMatrix(1,1) = std::cos(refLon);
        rotationMatrix(2,1) = 0.0;
        rotationMatrix(0,2) = std::cos(refLat)*std::cos(refLon);
        rotationMatrix(1,2) = std::cos(refLat)*std::sin(refLon);
        rotationMatrix(2,2) = std::sin(refLat);

        // Then, fill the sets with the proper types
        inputSet.clear();
        inputSet.insert(TypeID::dx);
        inputSet.insert(TypeID::dy);
        inputSet.insert(TypeID::dz);

        outputSet.clear();
        outputSet.insert(TypeID::dLat);
        outputSet.insert(TypeID::dLon);
        outputSet.insert(TypeID::dH);

    }  // end XYZ2NEU::Prepare()


} // end namespace gpstk
