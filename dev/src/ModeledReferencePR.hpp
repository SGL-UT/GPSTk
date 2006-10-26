/**
 * @file ModeledReferencePR.hpp
 * Class to compute modeled pseudoranges using a reference station
 */

#ifndef GPSTK_MODELEDREFERENCEPR_HPP
#define GPSTK_MODELEDREFERENCEPR_HPP

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
//  Dagoberto Salazar - gAGE. 2006
//
//============================================================================



#include "ModeledPseudorangeBase.hpp"
#include "DayTime.hpp"
#include "EngEphemeris.hpp"
#include "EphemerisStore.hpp"
#include "EphemerisRange.hpp"
#include "TropModel.hpp"
#include "IonoModel.hpp"
#include "IonoModelStore.hpp"
#include "Geodetic.hpp"
#include "Position.hpp"
#include "icd_200_constants.hpp"


namespace gpstk
{
      /** @addtogroup GPSsolutions */
      //@{

      /**
       * This class compute modeled pseudoranges from satellites to a reference station.
       *
       * @sa ModeledPseudorangeBase.hpp for base class.
       *
       */
   class ModeledReferencePR : public ModeledPseudorangeBase
   {
    public:

        /// Implicit constructor
        ModeledReferencePR() throw(Exception) { 
            InitializeValues();
        };


        /** Explicit constructor taking as input reference station coordinates.
         *
         * Those coordinates may be Cartesian (X, Y, Z in meters) or Geodetic
         * (Latitude, Longitude, Altitude), but defaults to Cartesian. 
         *
         * Also, a pointer to GeoidModel may be specified, but default is NULL 
         * (in which case WGS84 values will be used).
         *
         * @param aRx   first coordinate [ X(m), or latitude (degrees N) ]
         * @param bRx   second coordinate [ Y(m), or longitude (degrees E) ]
         * @param cRx   third coordinate [ Z, height above ellipsoid or radius, in m ]
         * @param s     coordinate system (default is Cartesian, may be set to Geodetic).
         * @param geoid pointer to GeoidModel (default is null, implies WGS84)
         */
        ModeledReferencePR(double aRx, double bRx, double cRx, 
            Position::CoordinateSystem s = Position::Cartesian,
            GeoidModel *geoid = NULL) throw(Exception) 
        { 
            InitializeValues();
            setInitialRxPosition(aRx, bRx, cRx, s, geoid);
        };


        /// Explicit constructor, taking as input a Position object containing reference station coordinates.
        ModeledReferencePR(Position RxCoordinates) throw(Exception) { 
            InitializeValues();
            setInitialRxPosition(RxCoordinates);
        };


        /** Compute the modeled pseudoranges, given satellite ID's, pseudoranges and other data
         * @param Tr            Measured time of reception of the data.
         * @param Satellite     Vector of satellites; on successful return, satellites that
         *                      were excluded by the algorithm are marked by a negative
         *                      Satellite[i].prn
         * @param Pseudorange   Vector of raw pseudoranges (parallel to satellite), in meters.
         * @param Eph           EphemerisStore to be used.
         * @param pTropModel    Pointer to tropospheric model to be used. By default points to NULL.
         * @param pIonoModel    Pointer to ionospheric model to be used. By default points to NULL.
         * @param extraBiases   Vector of extra biases to be added to the model.
         *
         * @return
         *  Number of satellites with valid data
         *
         * @sa TropModel.hpp, IonoModelStore.hpp.
         */
        int Compute(const DayTime& Tr, Vector<SatID>& Satellite, Vector<double>& Pseudorange,
            const EphemerisStore& Eph, const Vector<double>& extraBiases, TropModel 
            *pTropModel=NULL, IonoModelStore *pIonoModel=NULL) throw(Exception);


        /// Compute the modeled pseudoranges, given satellite ID's, pseudoranges and other data
        int Compute(const DayTime& Tr, Vector<SatID>& Satellite, Vector<double>& Pseudorange,
            const EphemerisStore& Eph) throw(Exception);


        /// Compute the modeled pseudoranges, given satellite ID's, pseudoranges and other data
        int Compute(const DayTime& Tr, Vector<SatID>& Satellite, Vector<double>& Pseudorange,
            const EphemerisStore& Eph, TropModel *pTropModel) throw(Exception);


        /// Compute the modeled pseudoranges, given satellite ID's, pseudoranges and other data
        int Compute(const DayTime& Tr, Vector<SatID>& Satellite, Vector<double>& Pseudorange,
            const EphemerisStore& Eph, const Vector<double>& extraBiases, IonoModelStore
            *pIonoModel) throw(Exception);


        /// Compute the modeled pseudoranges, given satellite ID's, pseudoranges and other data
        int Compute(const DayTime& Tr, Vector<SatID>& Satellite, Vector<double>& Pseudorange,
            const EphemerisStore& Eph, IonoModelStore *pIonoModel) throw(Exception);


        /// Compute the modeled pseudoranges, given satellite ID's, pseudoranges and other data
        int Compute(const DayTime& Tr, Vector<SatID>& Satellite, Vector<double>& Pseudorange,
            const EphemerisStore& Eph, TropModel *pTropModel, IonoModelStore *pIonoModel)
            throw(Exception);


        /** Compute just one modeled pseudorange, given satellite ID's, pseudorange and other data
         * @param Tr            Measured time of reception of the data.
         * @param Satellite     ID's of satellite
         * @param Pseudorange   Pseudorange (parallel to satellite), in meters.
         * @param Eph           EphemerisStore to be used.
         * @param pTropModel    Pointer to tropospheric model to be used. By default points to NULL.
         * @param pIonoModel    Pointer to ionospheric model to be used. By default points to NULL.
         * @param extraBiases   Extra bias to be added to the model.
         *
         * @return
         *  1 if satellite has valid data
         *
         * @sa TropModel.hpp, IonoModelStore.hpp.
         */
        int Compute(const DayTime& Tr, SatID& Satellite, double& Pseudorange,
            const EphemerisStore& Eph, const double& extraBiases, TropModel *pTropModel=NULL,
            IonoModelStore *pIonoModel=NULL) throw(Exception);


        /// Boolean variable indicating if SV instrumental delays (TGD) will be included  in results. It is true by default.
        bool useTGD;


        /// Destructor.
        virtual ~ModeledReferencePR() throw() {};


    protected:

        /// Initialization method
        void InitializeValues() throw(Exception) { 
            setInitialRxPosition();
            geometricRho(0);
            svClockBiases(0);
            svXvt(0);
            svTGD(0);
            svRelativity(0);
            ionoCorrections(0);
            tropoCorrections(0);
            modeledPseudoranges(0);
            prefitResiduals(0);
            extraBiases(0);
            availableSV(0);
            rejectedSV(0);
            useTGD = true;
            minElev = 10.0;  // By default, cut-off elevation is set to 10 degrees
        };


        /** Method to set the initial (a priori) position of receiver.
         * @return
         *  0 if OK
         *  -1 if problems arose
         */
        int setInitialRxPosition(double aRx, double bRx, double cRx, 
            Position::CoordinateSystem s=Position::Cartesian,
            GeoidModel *geoid=NULL) throw(GeometryException) 
        {
            try {
                Position rxpos(aRx, bRx, cRx, s, geoid);
                setInitialRxPosition(rxpos);
                return 0;
            }
            catch(GeometryException& e) {
                return -1;
            }
        };


        /// Method to set the initial (a priori) position of receiver.
        int setInitialRxPosition(Position RxCoordinates) throw(GeometryException) 
        {
            try {
                rxPos = RxCoordinates;
                return 0;
            }
            catch(GeometryException& e) {
                return -1;
            }
        };


        /// Method to set the initial (a priori) position of receiver.
        int setInitialRxPosition() throw(GeometryException) 
        {
            try {
                rxPos.setECEF(0.0, 0.0, 0.0);
                return 0;
            }
            catch(GeometryException& e) {
                return -1;
            }
        };


        /// Method to get the tropospheric corrections.
        double getTropoCorrections(TropModel *pTropModel, double elevation) throw() 
        {
            double tropoCorr = 0.0;
            try {
                tropoCorr = pTropModel->correction(elevation);
                // Check validity
                if(!(pTropModel->isValid())) tropoCorr = 0.0;
            }
            catch(TropModel::InvalidTropModel& e) {
                tropoCorr = 0.0;
            }
            return tropoCorr;
        };


        /// Method to get the ionospheric corrections.
        double getIonoCorrections(IonoModelStore *pIonoModel, DayTime Tr, Geodetic rxGeo, double elevation, double azimuth) throw() 
        {
            double ionoCorr = 0.0;
            try {
                ionoCorr = pIonoModel->getCorrection(Tr, rxGeo, elevation, azimuth);
            }
            catch(IonoModelStore::NoIonoModelFound& e) {
                ionoCorr = 0.0;
            }
            return ionoCorr;
        };


        /// Method to get TGD corrections.
        double getTGDCorrections(DayTime Tr, const EphemerisStore& Eph, SatID sat) throw() 
        {

            double TGDCorr = 0.0;

            try {
                TGDCorr = (Eph.getTGD(sat, Tr));
            }
            catch(EphemerisStore::NoTGDFound& e) {
                TGDCorr = 0.0;
            }
            return TGDCorr;
        };


        /// Method to get satellite elevation cut-off angle. By default, it is set to 10 degrees.
        double getMinElev()
        {
           return minElev;
        };


        /// Method to set satellite elevation cut-off angle. By default, it is set to 10 degrees.
        void setMinElev(double newElevation)
        {
           minElev = newElevation;
        };


   }; // class ModeledReferencePR

   //@}

} // namespace

#endif
