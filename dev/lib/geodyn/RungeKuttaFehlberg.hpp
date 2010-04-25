#pragma ident "$Id: $"

/**
* @file RungeKuttaFehlberg.hpp
* This class do integrations with Runge Kutta Fehlberg algorithm.
*/


#ifndef GPSTK_RUNGE_KUTTA_FEHLBERG_HPP
#define GPSTK_RUNGE_KUTTA_FEHLBERG_HPP


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


#include "Integrator.hpp"


namespace gpstk
{

      /** @addtogroup GeoDynamics */
      //@{

      /** This class do integrations with Runge Kutta Fehlberg algorithm.
       *
       *  References:
       *
       *     NASA Technical Report TR R-352
       *     Some Experimental Results Concerning The Error Propagation in
       *     Runge-Kutte type integration formulas
       *     by Erwin Fehlberg
       *     October 1970
       *
       *@warning  The adaptive have not been finished!!!
       */
   class RungeKuttaFehlberg : public Integrator
   {
   public:
         // Parametera for RKF78
      struct RKF78Param
      {
         double a[13] ;
         double b[13][12] ;
         double c1[13] ;
         double c2[13] ;
      };

   public:

         /// Default constructor
      RungeKuttaFehlberg();

         /// Default destructor
      virtual ~RungeKuttaFehlberg()
      { };

         /// Some test
      void test();

         /** Take a single integration step.
          * @param t     tindependent variable (usually the time)
          * @param y     inputs (usually the state)
          * @param peom  Object containing the Equations of Motion
          * @param tf    next time
          * @return      containing the new state
          */
      virtual Vector<double> integrateTo(const double&           t, 
                                         const Vector<double>&   y, 
                                         EquationOfMotion*       peom,
                                         const double&           tf );
      
         /// Set accuracy
      RungeKuttaFehlberg& setAccuracy(const double& accuracy)
      { accuracyEps = accuracy; return (*this);}
         
         /// Set minimum stepsize
      RungeKuttaFehlberg& setMinStepSize(const double& step)
      { minStepSize = step; return (*this ); }
      
         /// Set isAdaptive
      RungeKuttaFehlberg& setAdaptive(const bool& adaptive = true)
      { isAdaptive = adaptive; return (*this); }


   protected:
         

      Vector<double> integrateFixedStep(const double&           t, 
                                        const Vector<double>&   y, 
                                        EquationOfMotion*       peom,
                                        const double&           tf );

      Vector<double> integrateAdaptive(const double&           t, 
                                       const Vector<double>&   y, 
                                       EquationOfMotion*       peom,
                                       const double&           tf );
      

         // takes one "quality-controlled" Runge-Kutta-Fehlberg step 
         // 0 = Success
         // 1 = Unable to allocate workspace memory
         // 2 = Stepsize underflow
      int rkfqcs(double&           x,
                 Vector<double>&   y,
                 const double&     htry,
                 const double&     accuracy,
                 EquationOfMotion* peom,
                 Vector<double>&   yscal,
                 double& hdid,
                 double& hnext);
         
         // RKF78 single step
         // 0 = Success
         // 1 = Failed to allocate memory
      int rkfs78(const double&          x,
                 const Vector<double>&  y,
                 const double&          h,
                 EquationOfMotion*      peom,
                 Vector<double>&        yout,
                 Vector<double>&        yerr);

   protected:
      
         /// Accuracy
      double accuracyEps;

         /// Minimum step-size
      double minStepSize;

         /// Flag if adaptive is used
      bool isAdaptive;

         ///
      static const double RKF_EPS;

         /// Max step
      static const double RKF_MAXSTEP;

   private:
     
      /// Object holding all parameters for rkf78
      const static struct RKF78Param rkf78_param;
      
         // Easy accessing RKF78 parameters
      double A(int i){ return rkf78_param.a[i]; }

      double B(int i, int j){ return rkf78_param.b[i][j]; }
      
      double C(int i){ return rkf78_param.c1[i]; }

      double C2(int i){ return rkf78_param.c2[i]; }
      
      double DC(int i){ return (rkf78_param.c1[i]-rkf78_param.c2[i]); }


   }; // End of class 'RungeKuttaFehlberg'

      // @}

}  // End of namespace 'gpstk'


#endif // GPSTK_RUNGE_KUTTA_FEHLBERG_HPP

