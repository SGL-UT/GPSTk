#pragma ident "$Id: $"

/**
* @file SphericalHarmonicGravity.hpp
* 
*/

#ifndef GPSTK_SPHERICAL_HARMONIC_GRAVITY_HPP
#define GPSTK_SPHERICAL_HARMONIC_GRAVITY_HPP


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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================


#include "ForceModel.hpp"
#include "EarthSolidTide.hpp"
#include "EarthOceanTide.hpp"
#include "EarthPoleTide.hpp"

namespace gpstk
{
   using namespace StringUtils;

      /** @addtogroup GeoDynamics */
      //@{


      /** This class computes the body fixed acceleration due to the harmonic 
       *  gravity field of the central body
       */
   class SphericalHarmonicGravity : public ForceModel
   {
   public:

         /** Constructor.
          * @param n Desired degree.
          * @param m Desired order.
          */
      SphericalHarmonicGravity(int n, int m);


         /// Default destructor
      virtual ~SphericalHarmonicGravity() {};


         /// We declare a pure virtual function
      virtual void initialize() = 0;

   
         /** Computes the acceleration due to gravity in m/s^2.
          * @param r ECI position vector.
          * @param E ECI to ECEF transformation matrix.
          * @return ECI acceleration in m/s^2.
          */
      Vector<double> gravity(Vector<double> r, Matrix<double> E);


         /** Computes the partial derivative of gravity with respect to position.
          * @return ECI gravity gradient matrix.
          * @param r ECI position vector.
          * @param E ECI to ECEF transformation matrix.
          */
      Matrix<double> gravityGradient(Vector<double> r, Matrix<double> E);
      

         /** Call the relevant methods to compute the acceleration.
          * @param utc Time reference class
          * @param rb  Reference body class
          * @param sc  Spacecraft parameters and state
          * @return the acceleration [m/s^s]
          */
      virtual void doCompute(UTCTime utc, EarthBody& rb, Spacecraft& sc);

         

      /// Methods to enable earth tide correction

      void enableSolidTide(bool b = true)
      { correctSolidTide = b; }

      void enableOceanTide(bool b = true)
      { correctOceanTide = b; }
      
      void enablePoleTide(bool b = true)
      { correctPoleTide = b; }

   protected:

         /** Evaluates the two harmonic functions V and W.
          * @param r ECI position vector.
          * @param E ECI to ECEF transformation matrix.
          */
      void computeVW(Vector<double> r, Matrix<double> E);

         /// Add tides to coefficients 
      void correctCSTides(UTCTime t,int solidFlag = 0, int oceanFlag = 0, int poleFlag = 0);

         /// normalized coefficient
      double normFactor(int n, int m);

   protected:

      struct GravityModelData
      {
         std::string modelName;

         double GM;
         double refDistance;

         bool includesPermTide;

         double refMJD;

         double dotC20;
         double dotC21;
         double dotS21;

         int maxDegree;
         int maxOrder;

         Matrix<double> unnormalizedCS;

      } gmData;

         /// V W  (nmax+3)*(nmax+3)
         /// Harmonic function V and W
      Matrix<double> V, W;

         /// Degree and Order of gravity model desired.
      int desiredDegree, desiredOrder;
         
         /// Flags to indicate earth tides correction
      bool   correctSolidTide;
      bool   correctPoleTide;
      bool   correctOceanTide;

         /// Objects to do earth tides correction
      EarthSolidTide   solidTide;
      EarthPoleTide   poleTide;
      EarthOceanTide  oceanTide;


   }; // End of namespace 'gpstk'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_SPHERICAL_HARMONIC_GRAVITY_HPP




