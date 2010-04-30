#pragma ident "$Id: $"

/**
* @file SatOrbitPropagator.hpp
* 
*/

#ifndef GPSTK_SAT_ORBIT_PROPAGATOR_HPP
#define GPSTK_SAT_ORBIT_PROPAGATOR_HPP


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

#include <iostream>
#include <string>

#include "Integrator.hpp"
#include "RungeKuttaFehlberg.hpp"
#include "SatOrbit.hpp"


namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /**
       * Satellite Orbit Progagator
       *
       * IERS::loadIERSFile("InputData\\IERS\\finals.data");
       * JPLEPH::resetEphFile("InputData\\DE405\\jpleph.405");
       *
       * cout<<fixed<<setprecision(6);
       *
       * UTCTime utc0(1999,3,1,0,0,0.0);
       * gpstk::Vector<double> rv0(6,0.0);
       * rv0(0) = 2682920.8943;
       * rv0(1) = 4652720.5672;
       * rv0(2) = 4244260.0400;
       * rv0(3) = 2215.5999;
       * rv0(4) = 4183.3573;
       * rv0(5) = -5989.0576;
       *
       * OrbitPropagator op;
       * op.setInitState(utc0,rv0);
       * op.setStepSize(30.0);
       * 
       * double tt=3600.0*12;
       * double step=1.0;
       * 
       * cout<<fixed<<setw(12)<<setprecision(5);
       * double t=0.0;
       * while(t<tt)
       * {
       *   bool isOk = op.integrateTo(t+step);
       * 
       *   cout<<op.getCurTime()<<" "
       *      <<op.rvState()<<endl;
       *
       *   t+=step;
       * }
       *
       * YAN Wei,May 20th, 2009
       *
       * Last modifed Oct 5th,2009
       */
   class SatOrbitPropagator
   {
   public:

         /// Default constructor
      SatOrbitPropagator();

         /// Default destructor
      virtual ~SatOrbitPropagator();

      /* set force model setting
      */
      //void setForceModel(ForceModelSetting& fms);
   

         /// set integrator, default is Rungge-Kutta 78
      SatOrbitPropagator& setIntegrator(Integrator* pIntg)
      { pIntegrator = pIntg; return (*this); }


         /// set the integrator to the default one
      SatOrbitPropagator& setDefaultIntegrator()
      { pIntegrator = &rkfIntegrator; return (*this); }

         /// set equation of motion of the orbit
      SatOrbitPropagator& setOrbit(SatOrbit* porbit)
      { pOrbit = porbit; return (*this); }
      

         /// set the orbit to the default one
      SatOrbitPropagator& setDefaultOrbit()
      { pOrbit = &defaultOrbit; return (*this); }


         /// set step size of the integrator
      SatOrbitPropagator& setStepSize(double step_size = 10.0)
      { pIntegrator->setStepSize(step_size); return (*this);}

         /**set init state
          * @param utc0   init epoch
          * @param rv0    init state
          * @return       
          */
      SatOrbitPropagator& setInitState(UTCTime utc0, Vector<double> rv0);
      

         /** Take a single integration step.
          * @param tf    next time
          * @return      state of integration
          */
      virtual bool integrateTo(double tf);


         /// return the position and velocity , the dimension is 6
      Vector<double> rvState(bool bJ2k = true);

         /// return the rv state transition matrix 6*6
      Matrix<double> transitionMatrix()
      { return phiMatrix; }

         /// return the sensitivity matrix 6*np
      Matrix<double> sensitivityMatrix()
      { return sMatrix; }

         /// return the current epoch
      UTCTime getCurTime()
      { return (pOrbit->getRefEpoch() + curT);}

         /// return the current state
      Vector<double> getCurState()
      { return curState;}

         /// get numble of force model parameters
      int getNP(){return (curState.size() - 42) / 6;}

         /// write curT curState to a file
      void writeToFile(ostream& s);

      /*
      * try to integrate ephemeris and print it to a file
      * just for compare it with stk8.1
      * 
      * yanwei,Sep 19 th,2009
      */
      //void makeSatEph(OrbitSetting& os,string fileName);
      //void makeRefSatEph(OrbitSetting& os,string fileName);

      /* For Testing and Debuging...
      */
      void test();
      void test2();

   protected:

         /** Take a single integration step.
          *
          * @param x     time or independent variable
          * @param y     containing needed inputs (usually the state)
          * @param tf    next time
          * @return      containing the new state
          */
      virtual Vector<double> integrateTo(double t, Vector<double> y, double tf);

      /* set initial state of the the integrator
      *
      *  v      3
      * dr_dr0    3*3
      * dr_dv0   3*3
      * dr_dp0   3*np
      * dv_dr0   3*3
      * dv_dv0   3*3
      * dv_dp0   3*np
      */
      void setState(gpstk::Vector<double> state);

      /* set reference epoch
      */
      void setRefEpoch(UTCTime utc){pOrbit->setRefEpoch(utc);}

      /* update phiMatrix sMatrix and rvState from curState
      */
      void updateMatrix();

      /// ode solver default is RungeKutta78
      Integrator*   pIntegrator;
      /// Equation Of Motion
      SatOrbit*   pOrbit;

   private:
         /// The default integrator is RKF78
      RungeKuttaFehlberg rkfIntegrator;

         /// The default orbit is kepler orbit
      SatOrbit   defaultOrbit;
            
      // current time
      double curT;         

         /* current state
         * r      3
         * v      3
         * dr_dr0   3*3
         * dr_dv0   3*3
         * dr_dp0   3*np
         * dv_dr0   3*3
         * dv_dv0   3*3
         * dv_dp0   3*np
         */
      Vector<double> curState;         // 42+6*np
      
      /// state transition matrix
      Matrix<double> phiMatrix;      // 6*6
      
         /// the sensitivity matrix 
      Matrix<double> sMatrix;         // 6*np
      
         /// the position and velocity
      Vector<double>   rvVector;      // 6


      std::set<ForceModel::ForceModelType> setFMT;

   }; // End of class 'SatOrbitPropagator'
   
   /**
   * Stream output for OrbitPropagator objects.  Typically used for debugging.
   * @param s stream to append formatted DayTime to.
   * @param t DayTime to append to stream  s.
   * @return reference to  s.
   */
   ostream& operator<<(ostream& s,SatOrbitPropagator& op);

      // @}

} // end namespace 'gpstk'


#endif   // GPSTK_SAT_ORBIT_PROPAGATOR_HPP






