#pragma ident "$Id$"

/**
 * @file XYZ2NED.hpp
 * This is a class to change the reference base from ECEF XYZ to topocentric
 * North-East-Down (NED).
 */

#ifndef XYZ2NED_HPP
#define XYZ2NED_HPP

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



#include "geometry.hpp"                   // DEG_TO_RAD
#include "Matrix.hpp"
#include "Position.hpp"
#include "TypeID.hpp"
#include "ProcessingClass.hpp"


namespace gpstk
{
      /** @addtogroup GPSsolutions */
      //@{

      /**
       * This class changes the reference base from an Earth-Centered,
       * Earth-Fixed (ECEF) system to a North-East-Down (NED) topocentric
       * system, centered at the provided reference location.
       *
       * The NED system may be used when comparing the relative accuracy
       * of a given GNSS data processing strategy. This is a "right-handed"
       * system, and be aware that "down" is positive and "up" is negative.
       * 
       * A typical way to use this class follows:
       *
       * @code
       *   RinexObsStream rin("ebre0300.02o");
       *   // Reference position of receiver station
       *   Position nominalPos(4833520.2269, 41537.00768, 4147461.489);
       *
       *   // Some more code and definitions here...
       *
       *   gnssRinex gRin;
       *
       *   // Set defaults of model. A typical C1-based modeling is used
       *   ModeledPR modelRef(nominalPos, ionoStore, mopsTM, bceStore, TypeID::C1, true);
       *
       *   // Let's define a new equation definition to adapt solver object to base change
       *   TypeIDSet typeSet;
       *   typeSet.insert(TypeID::dLat);
       *   typeSet.insert(TypeID::dLon);
       *   typeSet.insert(TypeID::dH);
       *   typeSet.insert(TypeID::cdt);
       *   gnssEquationDefinition newEq(TypeID::prefitC, typeSet);
       *
       *   // Declare (and tune) a SolverLMS object
       *   SolverLMS solver;
       *   solver.setDefaultEqDefinition(newEq);
       *
       *   // Declare the base-changing object setting the reference position
       *   XYZ2NED baseChange(nominalPos);
       *
       *   while(rin >> gRin) {
       *      gRin >> modelRef >> baseChange >> solver;
       *   }
       *
       * @endcode
       *
       * The "XYZ2NED" object will visit every satellite in the GNSS data
       * structure that is "gRin" and will apply a rotation matrix to
       * coefficients dx, dy and dz of the design matrix, yielding
       * corresponding dLat, dLon and dH for each satellite.
       * 
       * Take notice that the design matrix coefficients dx, dy and dz were
       * computed by the "ModeledPR" object, so that step is mandatory.
       *
       * Also, the "XYZ2NED" class is effective when properly coupled with 
       * the "solver" object (be it based on LMS or WMS). In order to get 
       * this, you must instruct the "solver" object to get the solution using
       * a geometry/design matrix based on dLat, dLon and dH, instead of the
       * defaults (dx, dy and dz).
       *
       * The later is achieved defining an appropriate "gnssEquationDefinition"
       * object and instructing "solver" to use it as the default equation
       * definition.
       *
       * @sa XYZ2NEU.hpp
       */
   class XYZ2NED : public ProcessingClass
   {
   public:

         /// Default constructor.
      XYZ2NED() : refLat(0.0), refLon(0.0)
      { init(); setIndex(); };


         /** Common constructor taking reference point latitude and longitude
          *
          * @param lat       Latitude of the reference point.
          * @param lon       Longitude of the reference point.
          */
      XYZ2NED(const double& lat,
              const double& lon)
      { setLatLon(lat, lon); setIndex(); }


         /** Common constructor taking reference point Position object
          *
          * @param refPos    Reference point Position object.
          */
      XYZ2NED(const Position& refPos);


         /** Method to set the latitude of the reference point, in degrees.
          * @param lat      Latitude of the reference point, in degrees.
          *
          * @warning If parameter lat is outside +90/-90 degrees range,
          *    then latitude will be set to 0 degrees.
          */
      virtual XYZ2NED& setLat(const double& lat);


         /// Method to get the latitude of the reference point, in degrees.
      virtual double getLat() const
      { return (refLat*RAD_TO_DEG); };


         /** Method to set the longitude of the reference point, in degrees.
          * @param lon       Longitude of the reference point, in degrees.
          */
      virtual XYZ2NED& setLon(const double& lon);


         /// Method to get the longitude of the reference point, in degrees.
      virtual double getLon() const
      { return (refLon*RAD_TO_DEG); };


         /** Method to set simultaneously the latitude and longitude of 
          *  the reference point, in degrees.
          * @param lat      Latitude of the reference point, in degrees.
          * @param lon       Longitude of the reference point, in degrees.
          *
          * @warning If parameter lat is outside +90/-90 degrees range,
          *    then latitude will be set to 0 degrees.
          */
      virtual XYZ2NED& setLatLon(const double& lat,
                                 const double& lon);


         /** Returns a reference to a satTypeValueMap object after 
          *  converting from a geocentric reference system to a topocentric
          *  reference system.
          *
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process(satTypeValueMap& gData);


         /** Returns a reference to a gnssSatTypeValue object after 
          *  converting from a geocentric reference system to a topocentric
          *  reference system.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
      { Process(gData.body); return gData; };


         /** Returns a reference to a gnnsRinex object after converting 
          *  from a geocentric reference system to a topocentric reference
          * system.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
      { Process(gData.body); return gData; };


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /** Sets the index to a given arbitrary value. Use with caution.
          *
          * @param newindex      New integer index to be assigned to 
          *                      current object.
          */
      XYZ2NED& setIndex(const int newindex)
      { index = newindex; return (*this); };


         /// Destructor.
      virtual ~XYZ2NED() {};


   private:


         /// Latitude of the reference point (topocentric reference),
         /// in radians.
      double refLat;


         /// Longitude of the reference point (topocentric reference),
         /// in radians.
      double refLon;


         /// Rotation matrix.
      Matrix<double> rotationMatrix;


         /// Set (TypeIDSet) containing the types of data to be converted 
         /// (dx, dy, dz).
      TypeIDSet inputSet;


         /// Set (TypeIDSet) containing the resulting types of data
         /// (dLat, dLon, dH).
      TypeIDSet outputSet;


         /// This method builds the rotation matrix according to refLat
         /// and refLon values.
      virtual void init();


         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; }; 


   }; // class XYZ2NED


      //@}

} // namespace
#endif //XYZ2NED_HPP
