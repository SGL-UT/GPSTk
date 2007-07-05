
/**
 * @file XYZ2NEU.hpp
 * This is a class to change the reference base from ECEF XYZ to topocentric North-East-Up (NEU).
 */

#ifndef XYZ2NEU_HPP
#define XYZ2NEU_HPP

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
#include "DataStructures.hpp"


namespace gpstk
{
    /** @addtogroup GPSsolutions */
      //@{

      /**
       * This class changes the reference base from an Earth-Centered,
       * Earth-Fixed (ECEF) system to a North-East-Up (NEU) topocentric system,
       * centered at the provided reference location.
       *
       * The NEU system is commonly used when comparing the relative accuracy
       * of a given GNSS data processing strategy. Be mindful, however, that NEU
       * is a "left-handed" reference system, whereas geocentric ECEF and
       * topocentric North-East-Down (NED) are "right-handed" systems.
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
       *   XYZ2NEU baseChange(nominalPos);
       *
       *   while(rin >> gRin) {
       *      gRin >> modelRef >> baseChange >> solver;
       *   }
       *
       * @endcode
       *
       * The "XYZ2NEU" object will visit every satellite in the GNSS data structure that
       * is "gRin" and will apply a rotation matrix to coefficients dx, dy and dz of the
       * design matrix, yielding corresponding dLat, dLon and dH for each satellite.
       * 
       * Take notice that the design matrix coefficients dx, dy and dz were computed by 
       * the "ModeledPR" object, so that step is mandatory.
       *
       * Also, the "XYZ2NEU" class is effective when properly coupled with the "solver"
       * object (be it based on LMS or WMS). In order to get this, you must instruct the
       * "solver" object to get the solution using a geometry/design matrix based on
       * dLat, dLon and dH, instead of the defaults (dx, dy and dz).
       *
       * The later is achieved defining an appropriate "gnssEquationDefinition" object
       * and instructing "solver" to use it as the default equation definition.
       *
       */
    class XYZ2NEU
    {
    public:

        /// Default constructor.
        XYZ2NEU() : refLat(0.0), refLon(0.0)
        {
            Prepare();
        };


        /** Common constructor taking reference point latitude and longitude
         *
         * @param lat       Latitude of the reference point.
         * @param lon       Longitude of the reference point.
         */
        XYZ2NEU(const double& lat, const double& lon)
        {
            setLat(lat);
            setLon(lon);
        }


        /** Common constructor taking reference point Position object
         *
         * @param refPos    Reference point Position object.
         */
        XYZ2NEU(const Position& refPos)
        {
            setLat(refPos.getGeodeticLatitude());
            setLon(refPos.getLongitude());
        }


        /** Method to set the latitude of the reference point, in degrees.
         * @param lat      Latitude of the reference point, in degrees.
         */
        virtual void setLat(const double& lat)
        {
            // Don't allow latitudes out of the -90/+90 interval
            if ( (lat > 90.0) || (lat < -90.0) ) refLat = 0.0; else refLat = (lat*DEG_TO_RAD);
            Prepare();
        };


        /// Method to get the latitude of the reference point, in degrees.
        virtual double getLat() const
        {
            return (refLat*RAD_TO_DEG);
        };


        /** Method to set the longitude of the reference point, in degrees.
         * @param lon       Longitude of the reference point, in degrees.
         */
        virtual void setLon(const double& lon)
        {
            refLon = (lon*DEG_TO_RAD);
            Prepare();
        };


        /// Method to get the longitude of the reference point, in degrees.
        virtual double getLon() const
        {
           return (refLon*RAD_TO_DEG);
        };


        /** Method to set simultaneously the latitude and longitude of the reference point, in degrees.
         * @param lat      Latitude of the reference point, in degrees.
         * @param lon       Longitude of the reference point, in degrees.
         */
        virtual void setLatLon(const double& lat, const double& lon)
        {
            // Don't allow latitudes out of the -90/+90 interval
            if ( (lat > 90.0) || (lat < -90.0) ) refLat = 0.0; else refLat = (lat*DEG_TO_RAD);
            refLon = (lon*DEG_TO_RAD);
            Prepare();
        };


        /** Returns a reference to a satTypeValueMap object after converting from a geocentric reference system to a topocentric reference system.
         *
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Convert(satTypeValueMap& gData);


        /** Returns a reference to a gnssSatTypeValue object after converting from a geocentric reference system to a topocentric reference system.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssSatTypeValue& Convert(gnssSatTypeValue& gData) 
        {
            (*this).Convert(gData.body);
            return gData;
        };


        /** Returns a reference to a gnnsRinex object after converting from a geocentric reference system to a topocentric reference system.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssRinex& Convert(gnssRinex& gData)
        {
            (*this).Convert(gData.body);
            return gData;
        };


        /// Destructor.
        virtual ~XYZ2NEU() {};


    private:


        /// Latitude of the reference point (topocentric reference), in radians.
        double refLat;


        /// Longitude of the reference point (topocentric reference), in radians.
        double refLon;


        /// Rotation matrix.
        Matrix<double> rotationMatrix;


        /// Set (TypeIDSet) containing the types of data to be converted (dx, dy, dz).
        TypeIDSet inputSet;


        /// Set (TypeIDSet) containing the resulting types of data (dLat, dLon, dH).
        TypeIDSet outputSet;


        /// This method builds the rotation matrix according to refLat and refLon values.
        virtual void Prepare();


   }; // class XYZ2NEU


    /// Input operator from gnssSatTypeValue to XYZ2NEU.
    inline gnssSatTypeValue& operator>>(gnssSatTypeValue& gData, XYZ2NEU& converter) 
    {
            converter.Convert(gData);
            return gData;
    }


    /// Input operator from gnssRinex to XYZ2NEU.
    inline gnssRinex& operator>>(gnssRinex& gData, XYZ2NEU& converter) 
    {
            converter.Convert(gData);
            return gData;
    }

   //@}

} // namespace

#endif
