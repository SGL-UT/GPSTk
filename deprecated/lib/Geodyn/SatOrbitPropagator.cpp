//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
* @file SatOrbitPropagator.cpp
* 
*/

#include "SatOrbitPropagator.hpp"

#include "ASConstant.hpp"
#include "KeplerOrbit.hpp"
#include "ReferenceFrames.hpp"
#include "IERS.hpp"

using namespace std;

   /*
    * Stream output for OrbitPropagator objects.  Typically used for debugging.
    * @param s stream to append formatted CommonTime to.
    * @param t CommonTime to append to stream  s.
    * @return reference to  s.
    */
// Commented out because of compiler errors I can't be bothered to figure out
/*
std::ostream& operator<<(std::ostream& s, const gpstk::SatOrbitPropagator& op)
{
   op.writeToFile(s);
   
   return s;
}
*/

namespace gpstk
{

      // Constructor
   SatOrbitPropagator::SatOrbitPropagator()
         : pIntegrator(NULL),
           curT(0.0)
   {
      setDefaultIntegrator();
      setDefaultOrbit();

      setStepSize(1.0);

      setFMT.clear();
         //setFMT.insert(ForceModel::Cd);
         //setFMT.insert(ForceModel::Cr);
      pOrbit->setForceModelType(setFMT);

     
      
   }  // End of constructor 'SatOrbitPropagator::SatOrbitPropagator()'
   

      // Default destructor
   SatOrbitPropagator::~SatOrbitPropagator()
   {
      pIntegrator = NULL;
      pOrbit = NULL;
   }
   
      /* Take a single integration step.
       *
       * @param x     time or independent variable
       * @param y     containing needed inputs (usually the state)
       * @param tf    next time
       * @return      containing the new state
       */
   Vector<double> SatOrbitPropagator::integrateTo(double t,Vector<double> y,double tf)
   {
      try
      {
         curT = tf;
         curState = pIntegrator->integrateTo(t,y,pOrbit,tf);
         
         updateMatrix();

         return curState;
      }
      catch(...)
      {
         Exception e("Error in OrbitPropagator::integrateTo()");
         GPSTK_THROW(e);
      }

   }  // End of method 'SatOrbitPropagator::integrateTo()'


   bool SatOrbitPropagator::integrateTo(double tf)
   {
      try
      {
         double t = curT;
         Vector<double> y = curState;

         curT = tf;
         curState = pIntegrator->integrateTo(t, y, pOrbit, tf);
         
         updateMatrix();

         return true;
      }
      catch(Exception& e)
      {
         GPSTK_RETHROW(e);
      }

      catch(...)
      {
         Exception e("Unknown error in SatOrbitPropagator::integrateTo()");
         GPSTK_THROW(e);
      }

      return false;

   }  // End of method 'SatOrbitPropagator::integrateTo()'

      /*
       * set init state
       * utc0   init epoch
       * rv0    init state
       */
   SatOrbitPropagator& SatOrbitPropagator::setInitState(UTCTime utc0, Vector<double> rv0)
   {
      const int np = setFMT.size();
      
      curT = double(0.0);
      curState.resize(42+6*np,0.0);
      
         // position and velocity
      curState(0) = rv0(0);
      curState(1) = rv0(1);
      curState(2) = rv0(2);
      curState(3) = rv0(3);
      curState(4) = rv0(4);
      curState(5) = rv0(5);

      double I[9] = {1.0, 0.0 ,0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
      
      for(int i = 0; i < 9; i++)
      {
         curState(6+i) = I[i];
         curState(33+3*np+i) = I[i];
      }
      
      updateMatrix();
      
         // set reference epoch
      setRefEpoch(utc0);


      return (*this);

   }  // End of method 'SatOrbitPropagator::setInitState()'

   
      /// update phiMatrix sMatrix and rvState from curState
   void SatOrbitPropagator::updateMatrix()
   {
      const int np = getNP();

      Vector<double> dr_dr0(9,0.0);
      Vector<double> dr_dv0(9,0.0);
      Vector<double> dr_dp0(3*np,0.0);
      Vector<double> dv_dr0(9,0.0);
      Vector<double> dv_dv0(9,0.0);
      Vector<double> dv_dp0(3*np,0.0);

      for(int i = 0; i < 9; i++)
      {
         dr_dr0(i) = curState(6+i);
         dr_dv0(i) = curState(15+i);

         dv_dr0(i) = curState(24+3*np+i);
         dv_dv0(i) = curState(33+3*np+i);
      }
      for(int i = 0;i < 3*np; i++)
      {
         dr_dp0 = curState(24+i);
         dv_dp0 = curState(42+3*np+i);
      }

         // update phiMatrix
      phiMatrix.resize(6,6,0.0);

         // dr/dr0
      phiMatrix(0,0) = dr_dr0(0);
      phiMatrix(0,1) = dr_dr0(1);
      phiMatrix(0,2) = dr_dr0(2);
      phiMatrix(1,0) = dr_dr0(3);
      phiMatrix(1,1) = dr_dr0(4);
      phiMatrix(1,2) = dr_dr0(5);
      phiMatrix(2,0) = dr_dr0(6);
      phiMatrix(2,1) = dr_dr0(7);
      phiMatrix(2,2) = dr_dr0(8);
         // dr/dv0
      phiMatrix(0,3) = dr_dv0(0);
      phiMatrix(0,4) = dr_dv0(1);
      phiMatrix(0,5) = dr_dv0(2);
      phiMatrix(1,3) = dr_dv0(3);
      phiMatrix(1,4) = dr_dv0(4);
      phiMatrix(1,5) = dr_dv0(5);
      phiMatrix(2,3) = dr_dv0(6);
      phiMatrix(2,4) = dr_dv0(7);
      phiMatrix(2,5) = dr_dv0(8);
         // dv/dr0
      phiMatrix(3,0) = dv_dr0(0);
      phiMatrix(3,1) = dv_dr0(1);
      phiMatrix(3,2) = dv_dr0(2);
      phiMatrix(4,0) = dv_dr0(3);
      phiMatrix(4,1) = dv_dr0(4);
      phiMatrix(4,2) = dv_dr0(5);
      phiMatrix(5,0) = dv_dr0(6);
      phiMatrix(5,1) = dv_dr0(7);
      phiMatrix(5,2) = dv_dr0(8);
         // dv/dv0
      phiMatrix(3,3) = dv_dv0(0);
      phiMatrix(3,4) = dv_dv0(1);
      phiMatrix(3,5) = dv_dv0(2);
      phiMatrix(4,3) = dv_dv0(3);
      phiMatrix(4,4) = dv_dv0(4);
      phiMatrix(4,5) = dv_dv0(5);
      phiMatrix(5,3) = dv_dv0(6);
      phiMatrix(5,4) = dv_dv0(7);
      phiMatrix(5,5) = dv_dv0(8);

         // update sMatrix 6*np
      sMatrix.resize(6,np,0.0);
      for(int i = 0; i<np; i++)
      {
         sMatrix(0,i) = dr_dp0(0*np+i);
         sMatrix(1,i) = dr_dp0(1*np+i);
         sMatrix(2,i) = dr_dp0(2*np+i);

         sMatrix(3,i) = dv_dp0(0*np+i);
         sMatrix(4,i) = dv_dp0(1*np+i);
         sMatrix(5,i) = dv_dp0(2*np+i);
      }
      
         // update rvVector
      rvVector.resize(6,0.0);
      for(int i = 0; i < 6; i++) 
      { 
         rvVector(i) = curState(i);
      }

   }  // End of method 'SatOrbitPropagator::updateMatrix()'
   

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
   void SatOrbitPropagator::setState(Vector<double> state)
   {
      int np = (state.size()-42)/6;
      if(np<0)
      {
         Exception e("The size of the imput state is not valid");
         GPSTK_THROW(e);
      }
      curT = 0;
      curState.resize(state.size(),0.0);
      for(size_t i=0;i<state.size();i++)
      {
         curState(i) = state(i);
      }

      updateMatrix();

   }  // End of method 'SatOrbitPropagator::setState()'


   Vector<double> SatOrbitPropagator::rvState(bool bJ2k)
   {
      if(bJ2k == true)      // state ICRF
      {
         return rvVector;
      }
      else                 // state in ITRF
      {
         UTCTime utc = getCurTime();
         return ReferenceFrames::J2kPosVelToECEF(utc,rvVector);
      }
      
   }  // End of method 'SatOrbitPropagator::rvState()'


      /// write curT curState to a file
   void SatOrbitPropagator::writeToFile(ostream& s) const
   {
      UTCTime utcRef = pOrbit->getRefEpoch();
      UTCTime utc = utcRef;
      utc += curT;
      
      const int np = getNP();

      s << fixed;
      s << "#" << utc << " "
        << setprecision(12) << utc.mjdUTC() << endl;
      
      for(int i=0;i<6;i++)
      {
         s << setw(20) << setprecision(12) << rvVector(i) << " ";
      }
      s << endl;

         // [phi s]
      for(int i=0;i<6;i++)
      {
         for(int j=0;j<6;j++)
         {
            s << setw(20) << setprecision(12) << phiMatrix(i,j) << " ";
         }
         for(int j=0;j<np;j++)
         {
            s << setw(20) << setprecision(12) << sMatrix(i,j) << " ";
         }

         s << endl;
      }
   }


      /*
        void OrbitPropagator::setForceModel(ForceModelSetting& fms)
        {
        if(pOrbit)
        {
        pOrbit->setForceModel(fms);
        }
        }*/

      /* For Testing and Debuging...
       */
   void SatOrbitPropagator::test()
   {
      cout << "testing OrbitPropagator[KeplerOrbit]" << endl;
      cout << fixed << setprecision(6);

         // load global data
      IERS::loadSTKFile("InputData\\EOP-v1.1.txt");
      ReferenceFrames::setJPLEphFile("InputData\\DE405\\jplde405");
     
      ofstream fout("outorbit.txt");

      UTCTime utc0(2002,3,1,0,0,0.0);

      double state[42]={2682920.8943,4652720.5672,4244260.0400,2215.5999,4183.3573,-5989.0576,
                        1,0,0,
                        0,1,0,
                        0,0,1,
                        0,0,0,
                        0,0,0,
                        0,0,0,
                        0,0,0,
                        0,0,0,
                        0,0,0,
                        1,0,0,
                        0,1,0,
                        0,0,1};
      
      Vector<double> y0(42,0.0);
      y0 = state;

      Vector<double> yy0(6,0.0);
      yy0(0) = y0(0);
      yy0(1) = y0(1);
      yy0(2) = y0(2);
      yy0(3) = y0(3);
      yy0(4) = y0(4);
      yy0(5) = y0(5);
      
   
      Vector<double> kep(6,0.0);
      kep = KeplerOrbit::Elements(ASConstant::GM_Earth, yy0);


      SatOrbitPropagator op;

      SatOrbit* porbit = op.getSatOrbitPointer();
      porbit->enableGeopotential(SatOrbit::GM_JGM3,1,1);


      op.setRefEpoch(utc0.mjdUTC());
      op.setStepSize(10.0);

      double tt = 3600.0*24;
      double step = 60.0;

      cout << fixed << setw(12) << setprecision(5);
      
      double t=0.0;
      while(t < tt)
      {
         Vector<double> yy = op.integrateTo(t,y0,t+step);

// Commented out because of compiler errors I can't be bothered to figure out
//         fout << op;

         Vector<double> yy_prev(6,0.0);
         Vector<double> yy_out(6,0.0);
         for(int i=0; i<6; i++) 
         {
            yy_prev(i) = y0(i);
            yy_out(i) = yy(i);
         }
         
         Vector<double> yy_ref(6,0.0);
         Matrix<double> phi_ref(6,6,0.0);
         KeplerOrbit::TwoBody(ASConstant::GM_Earth,yy0,t+step,yy_ref,phi_ref);
         Vector<double> checky0 = KeplerOrbit::State(ASConstant::GM_Earth, kep, t+step);

         Matrix<double> phi = op.transitionMatrix();

         Vector<double> diff = yy_out - yy_ref;
         
         UTCTime utc = op.getCurTime();
         cout << utc << " " << diff <<endl;
         cout << phi-phi_ref << endl;
         
         t += step;
         y0 = yy;
      }
      
      fout.close();
      
   }

}  // End of namespace 'gpstk'
