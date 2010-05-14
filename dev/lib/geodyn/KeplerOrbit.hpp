#pragma ident "$Id: $"

/**
* @file KeplerOrbit.hpp
* Class to do Kepler orbit computation.
*/

#ifndef GPSTK_KEPLER_ORBIT_HPP
#define GPSTK_KEPLER_ORBIT_HPP


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

#include "Vector.hpp"
#include "Matrix.hpp"

namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /**
       * This class do some useful Keplerian orbit computation.
       *
       * Reference:
       * Satellite orbits models methods applications
       * Montenbruck, E. Gill
       */
   class KeplerOrbit
   {
   public:
   
         /**Computes the satellite state vector from osculating Keplerian 
          * elements for elliptic orbits.
          * @param GM       Gravitational coefficient
          * @param Kep      Keplerian elements(a e i OMG omg M)
          * @param dt       Time since epoch
          * @return   State vector(position and velocity)
          *
          * @warning  The semimajor axis a=Kep(0), dt and GM must be given in 
          *           consistent units 
          */
      static Vector<double> State( double GM, const Vector<double>& Kep, double dt );
      
      
         /**Computes the partial derivatives of the satellite state vector with 
          * respect to the orbital elements for elliptic, Keplerian orbits
          *
          * @param GM   Gravitational coefficient
          * @param Kep  Keplerian elements (a,e,i,Omega,omega,M) 
          * @param dt   Time since epoch
          * @return     Partials derivatives of the state vector (x,y,z,vx,vy,vz) 
          *             at time dt with respect to the epoch orbital elements
          * warning The semimajor axis a=Kep(0), dt and GM must be given in consistent units, 
          * The function cannot be used with circular or non-inclined orbit.
          */
      static gpstk::Matrix<double> StatePartials ( double GM, const Vector<double>& Kep, double dt );
      
      
         /** Computes the osculating Keplerian elements from the satellite 
          * state vector for elliptic orbits.
          * @ GM       Gravitational coefficient
          * @ y        State vector(position and velocity)
          * @ return   Keplerian elements(a e i OGM omg M)
          * @ warning  The state vector and GM must be given in consistent units
          */
      static Vector<double> Elements ( double GM, const Vector<double>& y );
      
      
         /**Computes orbital elements from two given position vectors and 
          *associated times 
          * @param GM        Gravitational coefficient
          * @param Mjda     Time ta (Modified Julian Date)
          * @param Mjdb     Time tb (Modified Julian Date)
          * @param ra       Position vector at time t_a
          * @param rb       Position vector at time t_b
          * @return>   Keplerian elements (a,e,i,Omega,omega,M) at ta
          *
          * @warning   The function cannot be used with state vectors describing a 
          *            circular or non-inclined orbit.
          */
      static Vector<double> Elements( double GM, double Mjda, double Mjdb, 
         const Vector<double>& ra, const Vector<double>& rb );
      
   
         /**Propagates a given state vector and computes the state transition  
          * matrix for elliptical Keplerian orbits
          *
          * @param GM        Gravitational coefficient
          * @param Y0        Epoch state vector (position and velocity)
          * @param dt        Time since epoch
          * @param Y         State vector (position and velocity)
          * @param dYdY0     State transition matrix d(x,y,z,vx,vy,vz)/d(x,y,z,vx,vy,vz)_0
          * @warning         The state vector, dt and GM must be given in consistent units. 
          *   Due to the internal use of Keplerian elements, the function cannot be used with
          *   epoch state vectors describing a circular or non-inclined orbit.
          */
      static void TwoBody ( double GM, const Vector<double>& Y0, double dt, 
         Vector<double>& Y, Matrix<double>& dYdY0 );


         /** Computes the eccentric anomaly for elliptic orbits
          * @param M    Mean anomaly in [rad]
          * @param e    Eccentricity of the orbit [0,1]
          * @return     Eccentric anomaly in [rad]
          */
      static double EccentricAnomaly (double M, double e);


         /** Computes the true anomaly for elliptic orbits
          * @param M    Mean anomaly in [rad]
          * @param e    Eccentricity of the orbit [0,1]
          * @return     True anomaly in [rad]
          */
      static double TrueAnomaly (double M, double e);


         /** Computes the true anomaly for elliptic orbits
          * @param cta  True anomaly in [rad]
          * @param e    Eccentricity of the orbit [0,1]
          * @return     Mean anomaly in [rad]
          */
      static double MeanAnomaly (double cta, double e);


         /// Get the period of the orbit
      static double getPeriod(double GM, const Vector<double>& Kep);


         /// Get the distance to the apogee point
      static double getApogee(double GM, const Vector<double>& Kep);
         

         /// Get the distance to the perigee point
      static double getPerigee(double GM, const Vector<double>& Kep);


      static void test();

   protected:
         
         /// Default constructor
      KeplerOrbit(){};

         /// Default destructor
      ~KeplerOrbit(){};

         /**Computes the sector-triangle ratio from two position vectors and 
          * the intermediate time 
          * @param r_a     Position at time t_a
          * @param r_a     Position at time t_b
          * @param tau     Normalized time (sqrt(GM)*(t_a-t_b))
          * @return        Sector-triangle ratio
          */
      static double FindEta (const Vector<double>& r_a, 
                             const Vector<double>& r_b, 
                             double tau);


      
         /// Fractional part of a number (y=x-[x])
      static double Frac (double x) { return x-floor(x); };

      
         /// x mod y
      static double Modulo (double x, double y) { return y*Frac(x/y); }

      
         /// local function for use by FindEta()
      static double F (double eta, double m, double l);

         /// connect two vector
      static Vector<double> Stack(Vector<double> r,Vector<double> v);
      
   private:

         /// numeric limits
       static const double eps_mach;      // = numeric_limits<double>::epsilon();

      
   }; // End of class 'KeplerOrbit'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_KEPLER_ORBIT_HPP




