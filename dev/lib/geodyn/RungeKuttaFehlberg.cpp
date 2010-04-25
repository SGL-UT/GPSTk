#pragma ident "$Id: $"

/**
 * @file RungeKuttaFehlberg.cpp
 * This class do integrations with Runge Kutta Fehlberg algorithm.
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

#include "RungeKuttaFehlberg.hpp"

namespace gpstk
{
   using namespace std;

   const double RungeKuttaFehlberg::RKF_EPS = 1.0-30; 

   const double RungeKuttaFehlberg::RKF_MAXSTEP = 1000000;
      
      // RKF78 parameters
   const struct RungeKuttaFehlberg::RKF78Param RungeKuttaFehlberg::rkf78_param =
   {  // a
      { 
         0.0,
         2.0 / 27.0,
         1.0 / 9.0,
         1.0 / 6.0,
         5.0 / 12.0,
         1.0 / 2.0,
         5.0 / 6.0,
         1.0 / 6.0,
         2.0 / 3.0,
         1.0 / 3.0,
         1.0,
         0.0,
         1.0,
      } ,
      // b
      {
         {              0.0,        0.0,          0.0,            0.0,
                        0.0,        0.0,          0.0,            0.0,
                        0.0,        0.0,          0.0,            0.0 } ,

         {       2.0 / 27.0,        0.0,          0.0,            0.0,
                        0.0,        0.0,          0.0,            0.0,
                        0.0,        0.0,          0.0,            0.0 } ,

         {       1.0 / 36.0, 1.0 / 12.0,          0.0,            0.0,
                        0.0,        0.0,          0.0,            0.0,  
                        0.0,        0.0,          0.0,            0.0 } ,

         {       1.0 / 24.0,        0.0,    1.0 / 8.0,            0.0, 
                        0.0,        0.0,          0.0,            0.0,  
                        0.0,        0.0,          0.0,            0.0 } ,

         {       5.0 / 12.0,        0.0, -25.0 / 16.0,    25.0 / 16.0,
                        0.0,        0.0,          0.0,            0.0,
                        0.0,        0.0,          0.0,            0.0 } ,

         {       1.0 / 20.0,        0.0,          0.0,      1.0 / 4.0,
                  1.0 / 5.0,        0.0,          0.0,            0.0,   
                        0.0,        0.0,          0.0,            0.0 } ,

         {    -25.0 / 108.0,        0.0,          0.0,  125.0 / 108.0,
               -65.0 / 27.0,125.0 / 54.0,         0.0,            0.0,
                        0.0,        0.0,          0.0,            0.0 } ,

         {     31.0 / 300.0,        0.0,          0.0,            0.0, 
               61.0 / 225.0, -2.0 / 9.0, 13.0 / 900.0,            0.0, 
                        0.0,        0.0,          0.0,            0.0 } ,
         
         {              2.0,        0.0,          0.0,    -53.0 / 6.0,
               704.0 / 45.0,-107.0 / 9.0, 67.0 / 90.0,            3.0,  
                        0.0,        0.0,          0.0,            0.0 } ,
         
         {    -91.0 / 108.0,        0.0,          0.0,   23.0 / 108.0,  
             -976.0 / 135.0,311.0 / 54.0,-19.0 / 60.0,     17.0 / 6.0,  
                -1.0 / 12.0,        0.0,          0.0,            0.0 } ,
         
         {  2383.0 / 4100.0,        0.0,          0.0, -341.0 / 164.0,
            4496.0 / 1025.0,-301.0 / 82.0,2133.0 / 4100,  45.0 / 82.0, 
               45.0 / 164.0, 18.0 / 41.0,         0.0,            0.0 } ,
         
         {      3.0 / 205.0,        0.0,          0.0,            0.0,
                        0.0,-6.0 / 41.0, -3.0 / 205.0,    -3.0 / 41.0, 
                 3.0 / 41.0, 6.0 / 41.0,          0.0,            0.0 } ,
         
         { -1777.0 / 4100.0,        0.0,          0.0, -341.0 / 164.0, 
            4496.0 / 1025.0,-289.0 / 82.0, 2193.0 / 4100.0, 51.0 / 82.0,
               33.0 / 164.0, 12.0 / 41.0,         0.0,           1.0 } ,
      } ,
      // c1
      {
         41.0 / 840,
            0.0,
            0.0,
            0.0,
            0.0,
            34.0 / 105.0,
            9.0 / 35.0,
            9.0 / 35.0,
            9.0 / 280.0,
            9.0 / 280.0,
            41.0 / 840.0,
            0.0,
            0.0,
      } ,
      // c2
      {
         0.0,
         0.0,
         0.0,
         0.0,
         0.0,
         34.0 / 105.0,
         9.0 / 35.0,
         9.0 / 35.0,
         9.0 / 280.0,
         9.0 / 280.0,
         0.0,
         41.0 / 840.0,
         41.0 / 840.0,
     }
   };

   //---------------------------------------------------------------------------------

      // Default constructor
   RungeKuttaFehlberg::RungeKuttaFehlberg()
      :accuracyEps(1.0e-12),
      minStepSize(1.2e-10),
      isAdaptive(false)
   {
   }



      /* Take a single integration step.
       * @param t     tindependent variable (usually the time)
       * @param y     inputs (usually the state)
       * @param peom  Object containing the Equations of Motion
       * @param tf    next time
       * @return      containing the new state
       */
   Vector<double> RungeKuttaFehlberg::integrateTo(const double&           t, 
                                                  const Vector<double>&   y, 
                                                  EquationOfMotion*       peom,
                                                  const double&           tf )
   {
      if(isAdaptive)
      {
         // ATTENTION: NOT FINISHED !!!
         Exception e("The adaptive method not finished!!!");
         GPSTK_THROW(e);

         return integrateAdaptive(t,y,peom,tf);
      }
      else
      {
         return integrateFixedStep(t,y,peom,tf);
      }
      
   }  // End of 'RungeKuttaFehlberg::integrateTo()'


      // RKF78 single step
      // 0 = Success
      // 1 = Failed to allocate memory
   int RungeKuttaFehlberg::rkfs78(const double&          x,
                                  const Vector<double>&  y,
                                  const double&          h,
                                  EquationOfMotion*      peom,
                                  Vector<double>&        yout,
                                  Vector<double>&        yerr)
   {
      // number of variable
      const int n = y.size();

      Vector<double> dydx = peom->getDerivatives(x, y);  // ak1
      
      Vector<double> ytemp(n,0.0) ;

      // ak2
      ytemp = y + B(1,0) * h * dydx ;
      Vector<double> ak2 = peom->getDerivatives(x + A(1) * h, ytemp);


      // ak3
      ytemp = y + h * (B(2,0) * dydx + B(2,1) * ak2) ;
      Vector<double> ak3 = peom->getDerivatives(x + A(2) * h, ytemp);
      
      // ak4
      ytemp = y + h * (B(3,0) * dydx + B(3,1) * ak2 + B(3,2) * ak3) ;
      Vector<double> ak4 = peom->getDerivatives(x + A(3) * h, ytemp);

      // ak5
      ytemp = y + h * (B(4,0) * dydx + B(4,1) * ak2 + B(4,2) * ak3 + B(4,3) * ak4) ;
      Vector<double> ak5 = peom->getDerivatives(x + A(4) * h, ytemp);
      
      // ak6
      ytemp = y + h * (B(5,0) * dydx + B(5,1) * ak2 + B(5,2) * ak3 + B(5,3) * ak4 
         + B(5,4) * ak5) ;
      Vector<double> ak6 = peom->getDerivatives(x + A(5) * h, ytemp);
      
      // ak7
      ytemp = y + h * (B(6,0) * dydx + B(6,1) * ak2 + B(6,2) * ak3 + B(6,3) * ak4
         + B(6,4) * ak5 + B(6,5) * ak6) ;
      Vector<double> ak7 = peom->getDerivatives(x + A(6) * h, ytemp);
      
      // ak8
      ytemp = y + h * (B(7,0) * dydx + B(7,1) * ak2 + B(7,2) * ak3 + B(7,3) * ak4 
         + B(7,4) * ak5 + B(7,5) * ak6 + B(7,6) * ak7) ;
      Vector<double> ak8 = peom->getDerivatives(x + A(7) * h, ytemp);
      
      // ak9
      ytemp = y + h * (B(8,0) * dydx + B(8,1) * ak2 + B(8,2) * ak3 + B(8,3) * ak4 
         + B(8,4) * ak5 + B(8,5) * ak6 + B(8,6) * ak7 + B(8,7) * ak8) ;
      Vector<double> ak9 = peom->getDerivatives(x + A(8) * h, ytemp);
      
      // ak10
      ytemp = y + h * (B(9,0) * dydx + B(9,1) * ak2 + B(9,2) * ak3 + B(9,3) * ak4
         + B(9,4) * ak5 + B(9,5) * ak6 + B(9,6) * ak7 + B(9,7) * ak8 + B(9,8) * ak9) ;
      Vector<double> ak10 = peom->getDerivatives(x + A(9) * h, ytemp);

      // ak11
      ytemp = y + h * (B(10,0) * dydx + B(10,1) * ak2 + B(10,2) * ak3 + B(10,3) * ak4
         + B(10,4) * ak5 + B(10,5) * ak6 + B(10,6) * ak7 + B(10,7) * ak8 + B(10,8) * ak9 
         + B(10,9) * ak10) ;
      Vector<double> ak11 = peom->getDerivatives(x + A(10) * h, ytemp);
      
      // ak12
      ytemp = y + h * (B(11,0) * dydx + B(11,1) * ak2 + B(11,2) * ak3 + B(11,3) * ak4 
         + B(11,4) * ak5 + B(11,5) * ak6 + B(11,6) * ak7 + B(11,7) * ak8 + B(11,8) * ak9 
         + B(11,9) * ak10 + B(11,10) * ak11) ;
      Vector<double> ak12 = peom->getDerivatives(x + A(11) * h, ytemp);
      
      // ak13
      ytemp = y + h * (B(12,0) * dydx + B(12,1) * ak2 + B(12,2) * ak3 + B(12,3) * ak4 
         + B(12,4) * ak5 + B(12,5) * ak6 + B(12,6) * ak7 + B(12,7) * ak8 + B(12,8) * ak9
         + B(12,9) * ak10 + B(12,10) * ak11 + B(12,11) * ak12) ;
      Vector<double> ak13 = peom->getDerivatives(x + A(12) * h, ytemp);
      
      yout.resize(n,0.0);
      yerr.resize(n,0.0);
      for (int i = 0; i < n; i++ ) 
      {
         /*
         // y7th
         yout[i] = y[i] + h * (C(0) * dydx[i] + C(1) * ak2[i] + C(2) * ak3[i] + C(3) * ak4[i] 
            + C(4) * ak5[i] + C(5) * ak6[i] + C(6) * ak7[i] + C(7) * ak8[i] + C(8) * ak9[i] 
            + C(9) * ak10[i] + C(10) * ak11[i] + C(11) * ak12[i] + C(12) * ak13[i]) ;*/

         // y8th
         yout[i] = y[i] + h * (C2(0) * dydx[i] + C2(1) * ak2[i] + C2(2) * ak3[i] + C2(3) * ak4[i] 
            + C2(4) * ak5[i] + C2(5) * ak6[i] + C2(6) * ak7[i] + C2(7) * ak8[i] + C2(8) * ak9[i] 
            + C2(9) * ak10[i] + C2(10) * ak11[i] + C2(11) * ak12[i] + C2(12) * ak13[i]) ;
         
         /*
         yerr[i] = h * (DC(0) * dydx[i] + DC(1) * ak2[i] + DC(2) * ak3[i] + DC(3) * ak4[i] 
            + DC(4) * ak5[i] + DC(5) * ak6[i] + DC(6) * ak7[i] + DC(7) * ak8[i] + DC(8) * ak9[i] 
            + DC(9) * ak10[i] + DC(10) * ak11[i] + DC(11) * ak12[i] + DC(12) * ak13[i]) ;
            */
         
         //dydx ak2 ak3 4 5 6 7 8 9 10 11 12 13
         //0     1   2  3 4 5 6 7 8 9  10 11 12 
         yerr[i] = h*C(0)*(ak12[i] + ak13[i] - dydx[i] - ak11[i]);

      }

      return 0;

   }  // End of 'RungeKuttaFehlberg::rkfs78()'
   

   // takes one "quality-controlled" Runge-Kutta-Fehlberg step 
   // 0 = Success
   // 1 = Unable to allocate workspace memory
   // 2 = Stepsize underflow
   int RungeKuttaFehlberg::rkfqcs(double&           x,
                                  Vector<double>&   y,
                                  const double&     htry,
                                  const double&     accuracy,
                                  EquationOfMotion* peom,
                                  Vector<double>&   yscal,
                                  double&           hdid,
                                  double&           hnext)
   {
      const double SAFETY = 0.9;
      const double PGROW  = (-1.0 / 8.0); 
      const double PSHRINK = (-1.0 / 7.0); 
      const double ERRCON = 2.56578451395034701e-8 ;  // ERRCON equals (5/SAFETY) raised to the power (1/PGROW) 
      
      const int n = y.size();

      Vector<double> yerr(n,0.0);
      Vector<double> ytemp(n,0.0);

      double h = htry ;
      
      double errmax(0.0);

      while(true)
      {
         int jstat = rkfs78(x,y,h,peom,ytemp,yerr);
         if ( jstat != 0 ) 
         {
            return jstat ;
         } 

         errmax = 0.0 ;
         for (int i = 0; i < n; i++ ) 
         {  
            double temp = fabs ( yerr[i] / yscal[i] );
            errmax = (errmax >= temp) ? errmax : temp;
         }
         errmax /= accuracy;

         if ( errmax <= 1.0 ) 
         {
            break ;
         }

         double htemp = SAFETY * h * std::pow ( errmax, PSHRINK ) ;
         
         h = (h >=0 ) ? (htemp > 0.1*h ? htemp : 0.1*h) : (htemp < 0.1*h ? htemp : 0.1*h);
         
         double xnew = x + h ;

         // Test for Underflow

         if ( xnew == x ) 
         {
            return 2 ;
         }

      }

      if ( errmax > ERRCON ) 
      {
         hnext = SAFETY * h * std::pow ( errmax, PGROW ) ;
      }
      else 
      {
         hnext = 5.0 * h ;
      }
      
      hdid = h;
      
      // update x and y
      x += hdid ;
      y = ytemp;

      return 0;

   }  // End of method 'RungeKuttaFehlberg::rkfqcs()'
   

   Vector<double> RungeKuttaFehlberg::integrateFixedStep(const double&           t, 
                                                         const Vector<double>&   y, 
                                                         EquationOfMotion*       peom,
                                                         const double&           tf )
   {
      Vector<double> yout,yerr;

      Vector<double> oldState = y;

      double dt = stepSize;

      double tt = t;
      while(t <= tf)
      {
         if((tt + dt) >= tf)
         {
            dt = tf - tt;
            break;
         }
            
         rkfs78(tt,oldState,dt,peom,yout,yerr);
         
         oldState = yout;

         tt += dt;
      }

      dt=tf-tt;

      rkfs78(tt,oldState,dt,peom,yout,yerr);

      return yout;

   }  // End of method 'RungeKuttaFehlberg::integrateFixedStep'

   Vector<double> RungeKuttaFehlberg::integrateAdaptive(const double&           t, 
                                    const Vector<double>&   y, 
                                    EquationOfMotion*       peom,
                                    const double&           tf )
   {
      double eps = accuracyEps;
      double x1 = t;
      double x2 = tf;
      double h1 = stepSize;
      double hmin = minStepSize;

      // number of variables
      const int nvar = y.size();


      double x = x1 ;
      double h = std::fabs(h1) * ( ( x2 < x1 ) ? -1.0 : 1.0);

      // make a copy of the data
      Vector<double> yend = y;

      for (int nstp = 0; nstp < RKF_MAXSTEP; nstp++ ) 
      {

         Vector<double> dydx = peom->getDerivatives(x, yend);
      
         Vector<double> yscal(nvar,0.0);
         for(int i=0;i<nvar;i++)
         {
            yscal(i) = std::fabs(yend(i)) + std::fabs(dydx(i)*h) + RKF_EPS;
         }
         

         if ( ( x + h - x2 ) * ( x + h - x1 ) > 0.0)
         {
            h = x2 - x ;
         }

         double hdid(0.0),hnext(0.0);

         int jstat = rkfqcs(x, yend, h, eps, peom, yscal, hdid, hnext);
         if ( jstat != 0 ) 
         {
            // 1  Failed to allocate
            // 2  Stepsize underflow

            Exception e2("Stepsize underflow!");
            GPSTK_THROW(e2);
         } ;

         if ( ( x - x2 ) * ( x2 - x1 ) >= 0.0 )
         {           
            return yend ;
         }

         if ( fabs ( hnext ) <= hmin ) 
         {
            // Stepsize too small

            Exception e4("Stepsize too small!");
            GPSTK_THROW(e4);
         }

         h = hnext ;
      }

      // Maximum steps exceeded

      Exception e3("Maximum steps exceeded!");
      GPSTK_THROW(e3);

   }  // End of method 'RungeKuttaFehlberg::integrateAdaptive'



   //////////////////////////////////////////////////////////////////////////
   // class to test integrator
   
   class TestEOM : public EquationOfMotion
   {
   public:
      virtual Vector<double> getDerivatives(const double& t,
         const Vector<double>& y);
   };

   Vector<double>  TestEOM::getDerivatives(const double& t,
      const Vector<double>& y)
   {
      gpstk::Vector<double> dydx(y.size(),0.0);   
      dydx[0] = std::cos(t); //0.25 * y(0) * ( 1.0 - y(0) / 20.0 ) ;
      
     
      return dydx;
   }
   


   void RungeKuttaFehlberg::test()
   {
      TestEOM eom;
      
      double t0	= 0.0;
      double h	= 1.0;
      gpstk::Vector<double> y0(1,0.0); 
      int dim		= 1;
      gpstk::Vector<double> y(1,0.0);	// = {0.0};
      
      this->setStepSize(0.01);
      //this->setAdaptive(true);

      int i=0;
      for(i=0;i<1000000;i++)
      {
         y = integrateTo(t0, y0, &eom, t0 + h);
         double t = t0 + h;
         double err = std::sin(t)-y[0];

         //cout<<fixed<<setprecision(8)<<setw(18);
         cout<<fixed;
         cout<<setw(18)<<setprecision(8)<< t << " "
            <<setw(18)<<setprecision(8)<<y[0] << " "
            <<setw(18)<<setprecision(12)<<err<<endl;

         t0 += h;
         y0[0] = y[0];
         y[0] = 0.0;
      }
      
      int a =0;

   }  // End of method 'RungeKuttaFehlberg::test()'


}  // End of 'namespace gpstk'
