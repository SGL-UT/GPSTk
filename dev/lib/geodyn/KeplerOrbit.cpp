#pragma ident "$Id: $"

/**
* @file KeplerOrbit.cpp
* Class to do Kepler orbit computation.
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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================

#include "KeplerOrbit.hpp"
#include <limits>
#include <cmath>
#include "ASConstant.hpp"
#include "ReferenceFrames.hpp"

namespace gpstk
{
   using namespace std;

   const double KeplerOrbit::eps_mach = numeric_limits<double>::epsilon();

      /* Computes the eccentric anomaly for elliptic orbits
       * @param M    Mean anomaly in [rad]
       * @param e    Eccentricity of the orbit [0,1]
       * @return     Eccentric anomaly in [rad]
       */
   double KeplerOrbit::EccentricAnomaly(double M, double e)
   {
      const int maxit = 15;
      const double eps = 100.0*eps_mach;

      // Variables
      int    i=0;
      double E, f;

      // Starting value

      M = Modulo(M, 2.0*ASConstant::PI);   
      if (e<0.8) E=M; else E= ASConstant::PI;

      // Iteration
      do 
      {
         f = E - e*std::sin(E) - M;
         E = E - f / ( 1.0 - e*std::cos(E) );
         ++i;
         if (i == maxit) 
         {
            cerr << " convergence problems in EccAnom" << endl;
            break;
         }
      }
      while (fabs(f) > eps);

      return E;

   }  // End of 'KeplerOrbit::EccAnom()'


      /* Computes the true anomaly for elliptic orbits
       * @param M    Mean anomaly in [rad]
       * @param e    Eccentricity of the orbit [0,1]
       * @return     True anomaly in [rad]
       */
   double KeplerOrbit::TrueAnomaly (double M, double e)
   {
      double E = EccentricAnomaly(M, e);

      double scta = std::sqrt(1.0-e*e)*std::sin(E)/(1.0-e*std::cos(E));
      double ccta = (std::cos(E)-e)/(1.0-e*std::cos(E));

      return atan2(scta, ccta);
   }
   

      /** Computes the true anomaly for elliptic orbits
       * @param cta  True anomaly in [rad]
       * @param e    Eccentricity of the orbit [0,1]
       * @return     Mean anomaly in [rad]
       */
   double KeplerOrbit::MeanAnomaly (double cta, double e)
   {
      double sinE = (std::sqrt(1.0-e*e)*std::sin(cta))/(1.0+e*std::cos(cta));
      double cosE = (e+std::cos(cta))/(1.0+e*std::cos(cta));
      double E = atan2(sinE,cosE);

      return (E- e * std::sin(E));
   }

      /*Computes the sector-triangle ratio from two position vectors and 
       * the intermediate time 
       * @param r_a     Position at time t_a
       * @param r_a     Position at time t_b
       * @param tau     Normalized time (sqrt(GM)*(t_a-t_b))
       * @return        Sector-triangle ratio
        */
   double KeplerOrbit::FindEta (const Vector<double>& r_a, 
                                const Vector<double>& r_b, 
                                double tau)
   {
      // Constants

      const int maxit = 30;
      const double delta = 100.0*eps_mach;  

      // Variables

      double kappa, m, l, s_a, s_b, eta_min, eta1, eta2, F1, F2, d_eta;


      // Auxiliary quantities

      s_a = norm(r_a);  
      s_b = norm(r_b);  

      kappa = std::sqrt ( 2.0*(s_a*s_b+dot(r_a,r_b)) );

      m = tau*tau / std::pow(kappa,3);   
      l = (s_a+s_b) / (2.0*kappa) - 0.5;

      eta_min = std::sqrt(m/(l+1.0));

      // Start with Hansen's approximation

      eta2 = ( 12.0 + 10.0*std::sqrt(1.0+(44.0/9.0)*m /(l+5.0/6.0)) ) / 22.0;
      eta1 = eta2 + 0.1;   

      // Secant method

      F1 = F(eta1, m, l);   
      F2 = F(eta2, m, l);  

      int i = 0;

      while (std::fabs(F2-F1) > delta)
      {
         d_eta = -F2*(eta2-eta1)/(F2-F1);  
         eta1 = eta2; F1 = F2; 
         while (eta2+d_eta<=eta_min)  d_eta *= 0.5;
         eta2 += d_eta;  
         F2 = F(eta2,m,l); ++i;

         if ( i == maxit ) 
         {
            cerr << "WARNING: Convergence problems in FindEta" << endl;
            break;
         }
      }

      return eta2;
   }

      /*Computes the satellite state vector from osculating Keplerian 
       * elements for elliptic orbits.
       * @GM       Gravitational coefficient
       * @Kep      Keplerian elements(a e i OMG omg M)
       * @dt       Time since epoch
       * @return   State vector(position and velocity)
       *
       * @warning  The semimajor axis a=Kep(0), dt and GM must be given in 
       *           consistent units 
       */
   Vector<double> KeplerOrbit::State( double GM, const Vector<double>& Kep, double dt )
   {
      
      // Keplerian elements at epoch
      const double a     = Kep(0);  
      const double e     = Kep(1);  
      const double i     = Kep(2);  
      const double Omega = Kep(3);
      const double omega = Kep(4); 
      const double M0    = Kep(5);
      
      // Mean anomaly
      double M = M0 + std::sqrt(GM/(a*a*a)) * dt;

      // Eccentric anomaly

      double E  = EccentricAnomaly(M,e);   

      double cosE = std::cos(E); 
      double sinE = std::sin(E);

      // Perifocal coordinates

      double fac = std::sqrt( (1.0-e)*(1.0+e) );  

      double R = a*(1.0-e*cosE);       // Distance
      double V = std::sqrt(GM*a)/R;    // Velocity

      Vector<double>  r(3,0.0),v(3,0.0);
      r(0) = a * (cosE - e); 
      r(1) = a * fac * sinE;  
      r(2) = 0.0;

      v(0) = -V * sinE;    
      v(1) = +V * fac * cosE;
      v(2) = 0.0;

      // Transformation to reference system (Gaussian vectors)
      Matrix<double> PQW(3,3,0.0);
      PQW = ReferenceFrames::Rz(-Omega) 
          * ReferenceFrames::Rx(-i) 
          * ReferenceFrames::Rz(-omega);

      r = PQW * r;
      v = PQW * v;

      return Stack(r,v);

   }  // End of method 'KeplerOrbit::State()'


   Vector<double> KeplerOrbit::Stack(Vector<double> r,Vector<double> v)
   {
      const int n = r.size() + v.size();
      Vector<double> rv(n,0.0);

      for(int i=0;i<n;i++)
      {
         if(i<r.size())
         {
            rv(i) = r(i);
         }
         else
         {
            rv(i) = v(i-r.size());
         }
      }

      return rv;
   }

   
      /*Computes the partial derivatives of the satellite state vector with 
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
   Matrix<double> KeplerOrbit::StatePartials ( double GM, const Vector<double>& Kep, double dt )
   {

      // Variables
      
      
      Vector<double>  dPdi(3),dPdO(3),dPdo(3),dQdi(3),dQdO(3),dQdo(3); 
      Vector<double>  dYda(6),dYde(6),dYdi(6),dYdO(6),dYdo(6),dYdM(6);
      Matrix<double>  dYdA(6,6);

      // Keplerian elements at epoch

      const double a     = Kep(0);  
      const double e     = Kep(1);  
      const double i     = Kep(2);  
      const double Omega = Kep(3);
      const double omega = Kep(4); 
      const double M0    = Kep(5);

      // Mean and eccentric anomaly

      double n = std::sqrt (GM/(a*a*a));
      double M = M0 +n*dt;
      double E = EccentricAnomaly(M,e);   

      // Perifocal coordinates

      double cosE = std::cos(E); 
      double sinE = std::sin(E);
      double fac  = std::sqrt((1.0-e)*(1.0+e));  

      double r = a*(1.0-e*cosE);  // Distance
      double v = std::sqrt(GM*a)/r;    // Velocity

      double x  = +a*(cosE-e); 
      double y  = +a*fac*sinE;
      double vx = -v*sinE;     
      double vy = +v*fac*cosE; 

      // Transformation to reference system (Gaussian vectors) and partials
      Matrix<double>  PQW(3,3,0.0);
      PQW = ReferenceFrames::Rz(-Omega) 
          * ReferenceFrames::Rx(-i) 
          * ReferenceFrames::Rz(-omega);

      //P = PQW.Col(0);  Q = PQW.Col(1);  W = PQW.Col(2);
      Vector<double>  P(3),Q(3),W(3),e_z(3),N(3);
      P(0) = PQW(0,0);
      P(1) = PQW(1,0);
      P(2) = PQW(2,0);

      Q(0) = PQW(0,1);
      Q(1) = PQW(1,1);
      Q(2) = PQW(2,1);
      
      W(0) = PQW(0,2);
      W(1) = PQW(1,2);
      W(2) = PQW(2,2);
      
      //e_z = Vector(0,0,1);  
      e_z(0) = 0.0;
      e_z(1) = 0.0;
      e_z(2) = 1.0;
      
      
      N = cross(e_z,W); N = N/norm(N);
      
      dPdi = cross(N,P);  dPdO = cross(e_z,P); dPdo =  Q;
      dQdi = cross(N,Q);  dQdO = cross(e_z,Q); dQdo = -1.0*P;
      
      // Partials w.r.t. semimajor axis, eccentricity and mean anomaly at time dt      
      dYda = Stack ( (x/a)*P + (y/a)*Q,
         (-vx/(2*a))*P + (-vy/(2*a))*Q );


      dYde = Stack ( (-a-std::pow(y/fac,2)/r)*P + (x*y/(r*fac*fac))*Q , 
         (vx*(2*a*x+e*std::pow(y/fac,2))/(r*r))*P
         + ((n/fac)*std::pow(a/r,2)*(x*x/r-std::pow(y/fac,2)/a))*Q );


      dYdM = Stack ( (vx*P+vy*Q)/n, (-n*std::pow(a/r,3))*(x*P+y*Q) );


      // Partials w.r.t. inlcination, node and argument of pericenter
      dYdi = Stack ( x*dPdi+y*dQdi, vx*dPdi+vy*dQdi ); 
      dYdO = Stack ( x*dPdO+y*dQdO, vx*dPdO+vy*dQdO ); 
      dYdo = Stack ( x*dPdo+y*dQdo, vx*dPdo+vy*dQdo ); 

      // Derivative of mean anomaly at time dt w.r.t. the semimajor axis at epoch

      double dMda = -1.5*(n/a)*dt;  

      // Combined partial derivative matrix of state with respect to epoch elements

      for (int k=0; k<6; k++) 
      {
         dYdA(k,0) = dYda(k) + dYdM(k)*dMda;  
         dYdA(k,1) = dYde(k); 
         dYdA(k,2) = dYdi(k); 
         dYdA(k,3) = dYdO(k);
         dYdA(k,4) = dYdo(k);
         dYdA(k,5) = dYdM(k);
      }

      return dYdA;

   }  // End of method 'KeplerOrbit::StatePartials()'


      /* Computes the osculating Keplerian elements from the satellite 
       * state vector for elliptic orbits.
       * @param GM       Gravitational coefficient
       * @param y        State vector(position and velocity)
       * @param return   Keplerian elements(a e i OGM omg M)
       * @param warning  The state vector and GM must be given in consistent units
       */
   Vector<double> KeplerOrbit::Elements( double GM, const Vector<double>& y )
   {
      // position and velocity
      Vector<double>  r(3),v(3);
      r(0) = y(0);
      r(1) = y(1);
      r(2) = y(2);
      v(0) = y(3);
      v(1) = y(4);
      v(2) = y(5);

      Vector<double> h = cross(r,v);                           // Areal velocity
      double H = norm(h);

      double Omega = atan2( h(0), -h(1) );                     // Long. ascend. node 
      Omega = Modulo(Omega,2.0*ASConstant::PI);

      double i     = std::atan2( std::sqrt(h(0)*h(0)+h(1)*h(1)), h(2) ); // Inclination        
      
      double u  = std::atan2( r(2)*H, -r(0)*h(1)+r(1)*h(0) );    // Arg. of latitude   
      double R  = norm(r);                                       // Distance           

      double a = 1.0 / (2.0/R-dot(v,v)/GM);                     // Semi-major axis    

      double eCosE = 1.0-R/a;                                   // e*cos(E)           
      double eSinE = dot(r,v)/std::sqrt(GM*a);                  // e*sin(E)           

      double e2 = eCosE*eCosE +eSinE*eSinE;
      double e  = std::sqrt(e2);                                     // Eccentricity 
      double E  = std::atan2(eSinE,eCosE);                           // Eccentric anomaly  

      double M  = Modulo(E-eSinE,2.0*ASConstant::PI);                // Mean anomaly

      double nu = std::atan2(std::sqrt(1.0-e2)*eSinE, eCosE-e2);     // True anomaly

      double omega = Modulo(u-nu,2.0*ASConstant::PI);                // Arg. of perihelion 

      // Keplerian elements vector

      Vector<double> Kep(6,0.0);
      Kep(0) = a;
      Kep(1) = e;
      Kep(2) = i;
      Kep(3) = Omega;
      Kep(4) = omega;
      Kep(5) = M;

      return Kep; 

   }  // End of method 'KeplerOrbit::Elements()'

   
      /*Computes orbital elements from two given position vectors and 
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
   Vector<double> KeplerOrbit::Elements( double GM, double Mjda, double Mjdb, 
      const Vector<double>& ra, const Vector<double>& rb )
   {

      // Variables
      double  a, e, i, Omega, omega, M;

      // Calculate vector r_0 (fraction of r_b perpendicular to r_a) 
      // and the magnitudes of r_a,r_b and r_0

      double s_a = norm(ra);  
      Vector<double> e_a = ra/s_a;

      double s_b = norm(rb); 
      double fac = dot(rb,e_a); 
      Vector<double> r_0 = rb-fac*e_a;

      double s_0 = norm(r_0);  
      Vector<double> e_0 = r_0/s_0;

      // Inclination and ascending node 

      Vector<double> W = cross(e_a,e_0);
      Omega = std::atan2( W(0), -W(1) );                     // Long. ascend. node 
      Omega = Modulo(Omega,2.0*ASConstant::PI);
      i     = std::atan2( std::sqrt(W(0)*W(0)+W(1)*W(1)), W(2) ); // Inclination        
      
      double u(0.0);
      if (i==0.0) 
         u = std::atan2( ra(1), ra(0) );
      else 
         u = std::atan2( +e_a(2) , -e_a(0)*W(1)+e_a(1)*W(0) );

      // Semilatus rectum

      double tau = std::sqrt(GM) * 86400.0*fabs(Mjdb-Mjda);   
      double eta = FindEta( ra, rb, tau );
      double p   = std::pow( s_a*s_0*eta/tau, 2 );   

      // Eccentricity, true anomaly and argument of perihelion

      double cos_dnu = fac / s_b;    
      double sin_dnu = s_0 / s_b;

      double ecos_nu = p / s_a - 1.0;  
      double esin_nu = ( ecos_nu * cos_dnu - (p/s_b-1.0) ) / sin_dnu;

      e  = std::sqrt( ecos_nu*ecos_nu + esin_nu*esin_nu );
      double nu = std::atan2(esin_nu,ecos_nu);

      omega = Modulo(u-nu,2.0*ASConstant::PI);

      // Perihelion distance, semimajor axis and mean motion

      a = p/(1.0-e*e);
      double n = std::sqrt( GM / std::fabs(a*a*a) );

      // Mean anomaly and time of perihelion passage

      if (e<1.0) {
         double E = std::atan2( std::sqrt((1.0-e)*(1.0+e)) * esin_nu,  ecos_nu + e*e );
         M = Modulo( E - e*std::sin(E), 2.0*ASConstant::PI );
      }
      else 
      {
         double sinhH = std::sqrt((e-1.0)*(e+1.0)) * esin_nu / ( e + e * ecos_nu );
         M = e * sinhH - std::log ( sinhH + std::sqrt(1.0+sinhH*sinhH) );
      }

      // Keplerian elements vector

      gpstk::Vector<double> Kep(6,0.0);
      Kep(0) = a;
      Kep(1) = e;
      Kep(2) = i;
      Kep(3) = Omega;
      Kep(4) = omega;
      Kep(5) = M;

      return Kep;

   }  // End of method 'KeplerOrbit::Elements()'


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
   void KeplerOrbit::TwoBody( double GM, const Vector<double>& Y0, double dt, 
      Vector<double>& Y, Matrix<double>& dYdY0 )
   {
      double  a,e,i,n, sqe2,naa;
      double  P_aM, P_eM, P_eo, P_io, P_iO;
      gpstk::Vector<double>  A0(6);
      gpstk::Matrix<double>  dY0dA0(6,6), dYdA0(6,6), dA0dY0(6,6);

      // Orbital elements at epoch
      A0 = Elements(GM,Y0);

      a = A0(0);  e = A0(1);  i = A0(2);

      n = std::sqrt(GM/(a*a*a));

      // Propagated state 

      Y = State(GM,A0,dt);

      // State vector partials w.r.t epoch elements

      dY0dA0 = StatePartials(GM,A0,0.0);
      dYdA0  = StatePartials(GM,A0,dt);
      
      // Poisson brackets

      sqe2 = std::sqrt((1.0-e)*(1.0+e));
      naa  = n*a*a;

      P_aM = -2.0/(n*a);                   // P(a,M)     = -P(M,a)
      P_eM = -(1.0-e)*(1.0+e)/(naa*e);     // P(e,M)     = -P(M,e)
      P_eo = +sqe2/(naa*e);                // P(e,omega) = -P(omega,e)
      P_io = -1.0/(naa*sqe2*std::tan(i));       // P(i,omega) = -P(omega,i)
      P_iO = +1.0/(naa*sqe2*std::sin(i));       // P(i,Omega) = -P(Omega,i)

      // Partials of epoch elements w.r.t. epoch state

      for(int k=0;k<3;k++) 
      {

         dA0dY0(0,k)   = + P_aM*dY0dA0(k+3,5);
         dA0dY0(0,k+3) = - P_aM*dY0dA0(k  ,5);

         dA0dY0(1,k)   = + P_eo*dY0dA0(k+3,4) + P_eM*dY0dA0(k+3,5);
         dA0dY0(1,k+3) = - P_eo*dY0dA0(k  ,4) - P_eM*dY0dA0(k  ,5);

         dA0dY0(2,k)   = + P_iO*dY0dA0(k+3,3) + P_io*dY0dA0(k+3,4);
         dA0dY0(2,k+3) = - P_iO*dY0dA0(k  ,3) - P_io*dY0dA0(k  ,4);

         dA0dY0(3,k)   = - P_iO*dY0dA0(k+3,2);
         dA0dY0(3,k+3) = + P_iO*dY0dA0(k  ,2);

         dA0dY0(4,k)   = - P_eo*dY0dA0(k+3,1) - P_io*dY0dA0(k+3,2);
         dA0dY0(4,k+3) = + P_eo*dY0dA0(k  ,1) + P_io*dY0dA0(k  ,2);

         dA0dY0(5,k)   = - P_aM*dY0dA0(k+3,0) - P_eM*dY0dA0(k+3,1);
         dA0dY0(5,k+3) = + P_aM*dY0dA0(k  ,0) + P_eM*dY0dA0(k  ,1);

      };

      // State transition matrix
      dYdY0 = dYdA0 * dA0dY0;

   }  // End of method 'KeplerOrbit::TwoBody()'

      
      // F : local function for use by FindEta()
      // F = 1 - eta +(m/eta**2)*W(m/eta**2-l)
      //
   double KeplerOrbit::F (double eta, double m, double l)
   {

      const double eps = 100.0 * eps_mach;

      double  w,W,a,n,g;

      w = m/(eta*eta)-l; 

      if (std::fabs(w)<0.1) 
      { 
         W = a = 4.0/3.0; n = 0.0;
         do 
         {
            n += 1.0;  a *= w*(n+2.0)/(n+1.5);  W += a; 
         }
         while (std::fabs(a) >= eps);
      }
      else 
      {
         if (w > 0.0) 
         {
            g = 2.0 * std::asin(std::sqrt(w));  
            W = (2.0*g - std::sin(2.0*g)) / std::pow(std::sin(g), 3);
         }
         else 
         {
            g = 2.0*std::log(std::sqrt(-w)+std::sqrt(1.0-w));  // =2.0*arsinh(sqrt(-w))
            W = (std::sinh(2.0*g) - 2.0*g) / std::pow(std::sinh(g), 3);
         }
      }

      return ( 1.0 - eta + (w+l)*W );

   }   // End of function F

   double KeplerOrbit::getPeriod(double GM, const Vector<double>& Kep)
   {
      const double n = std::sqrt(GM / std::pow(Kep(0),3));
      return 2.0 * ASConstant::PI / n;
   }
   
   double KeplerOrbit::getApogee(double GM, const Vector<double>& Kep)
   {
      // p =  a*(1-e*e)
      double p = Kep(0) * (1.0 - Kep(1) * Kep(1));
      return p / (1 - Kep(1));
   }

   double KeplerOrbit::getPerigee(double GM, const Vector<double>& Kep)
   {
      // p =  a*(1-e*e)
      double p = Kep(0) * (1.0 - Kep(1) * Kep(1));

      return p / (1 + Kep(1));
   }



   void KeplerOrbit::test()
   {
      const double GM_Earth    = 398600.4415e+9;    // [m^3/s^2]; JGM3
      
      const double Deg = 180.0 / ASConstant::PI;
      
      double rv[6]={-6345.000e3, -3723.000e3,  -580.000e3, +2.169000e3, -9.266000e3, -1.079000e3 };

      Vector<double> Y0_ref(6);
      Y0_ref = rv;

      Vector<double> Kep = Elements ( GM_Earth, Y0_ref );
      double ceta = KeplerOrbit::TrueAnomaly(Kep(5),Kep(1));
      double ecc = KeplerOrbit::EccentricAnomaly(Kep(5),Kep(1));

      cout<<fixed<<setprecision(6);
      cout << "Orbital elements:" << endl << endl
         << setprecision(3)
         << "  Semimajor axis   " << setw(10) << Kep(0)/1000.0 << " km" << endl
         << setprecision(7)
         << "  Eccentricity     " << setw(10) << Kep(1)<< endl
         << setprecision(3)
         << "  Inclination      " << setw(10) << Kep(2)*Deg << " deg"<< endl
         << "  RA ascend. node  " << setw(10) << Kep(3)*Deg << " deg"<< endl
         << "  Arg. of perigee  " << setw(10) << Kep(4)*Deg << " deg"<< endl
         << "  Mean anomaly     " << setw(10) << Kep(5)*Deg << " deg"<< endl
         << "  True anomaly     " << setw(10) << ceta*Deg << " deg"<< endl
         << "  Eccentric anomaly" << setw(10) << ecc*Deg << " deg"<< endl
         << endl;
      

      Vector<double> Y_ref = State(GM_Earth,Kep,0);
      Vector<double> Y(6);
      Matrix<double> phi(6,6);

      KeplerOrbit::TwoBody ( GM_Earth,Y0_ref,0, Y,phi); // State vector
      
      cout<<fixed<<setw(12)<<setprecision(8);
      for(int i=0;i<6;i++)
      {
         cout<<Y(i)<<endl;
      }

      for(int i=0;i<6;i++)
      {
         for(int j=0;j<6;j++)
         {
            cout<<phi(i,j)<< " ";
         }
         cout<<endl;
      }

      Vector<double> Y2(6),diff(6);
      Y2 = phi*Y0_ref;

      diff = Y2-Y;
      for(int i=0;i<6;i++)
      {
         cout<<diff(i)<<endl;
      }
      
  
   }	// End of method 'KeplerOrbit::test()'


}	// End of namespace 'gpstk'





