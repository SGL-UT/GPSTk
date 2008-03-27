#pragma ident "$Id$"

/**
 * @file XYZ2NEU.cpp
 * This is a class to change the reference base from ECEF XYZ to topocentric
 * North-East-Up (NEU).
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


      // Index initially assigned to this class
   int XYZ2NEU::classIndex = 5000000;


      // Returns an index identifying this object.
   int XYZ2NEU::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string XYZ2NEU::getClassName() const
   { return "XYZ2NEU"; }


      /* Common constructor taking reference point Position object
       *
       * @param refPos    Reference point Position object.
       */
   XYZ2NEU::XYZ2NEU(const Position& refPos)
   {
      setLatLon(refPos.getGeodeticLatitude(), refPos.getLongitude());
      setIndex();
   }


      /* Method to set the latitude of the reference point, in degrees.
       * @param lat      Latitude of the reference point, in degrees.
       *
       * @warning If parameter lat is outside +90/-90 degrees range,
       *    then latitude will be set to 0 degrees.
       */
   XYZ2NEU& XYZ2NEU::setLat(const double& lat)
   {
         // Don't allow latitudes out of the -90/+90 interval
      if ( (lat > 90.0) || 
           (lat < -90.0)   )
      {
         refLat = 0.0;
      }
      else
      {
         refLat = (lat*DEG_TO_RAD);
      }

      init();

      return (*this);
   }


      /* Method to set the longitude of the reference point, in degrees.
       * @param lon       Longitude of the reference point, in degrees.
       */
   XYZ2NEU& XYZ2NEU::setLon(const double& lon)
   {
      refLon = (lon*DEG_TO_RAD);
      init();

      return (*this);
   }


      /* Method to set simultaneously the latitude and longitude of the
       *  reference point, in degrees.
       * @param lat      Latitude of the reference point, in degrees.
       * @param lon       Longitude of the reference point, in degrees.
       *
       * @warning If parameter lat is outside +90/-90 degrees range,
       *    then latitude will be set to 0 degrees.
       */
   XYZ2NEU& XYZ2NEU::setLatLon(const double& lat,
                               const double& lon)
   {
         // Don't allow latitudes out of the -90/+90 interval
      if ( (lat > 90.0) ||
           (lat < -90.0)  )
      {
         refLat = 0.0;
      }
      else
      {
         refLat = (lat*DEG_TO_RAD);
      }

      refLon = (lon*DEG_TO_RAD);
      init();

      return (*this);
   }


      // Returns a reference to a satTypeValueMap object after converting 
      // from a geocentric reference system to a topocentric reference system.
      //
      // @param gData     Data object holding the data.
      //
   satTypeValueMap& XYZ2NEU::Process(satTypeValueMap& gData)
   {
      Matrix<double> neuMatrix;

         // Get the corresponding geometry/design matrix data
      Matrix<double> dMatrix(gData.getMatrixOfTypes(inputSet));

         // Compute the base change. For convenience, we use the property:
         // Y = A*B => Y^T = (A*B)^T => Y^T = B^T * A^T
      neuMatrix = dMatrix*rotationMatrix;

      gData.insertMatrix(outputSet, neuMatrix);

      return gData;

   }  // end XYZ2NEU::Process()


      // This method builds the rotation matrix according to refLat 
      // and refLon values.
   void XYZ2NEU::init()
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

   }  // end XYZ2NEU::init()


} // end namespace gpstk
