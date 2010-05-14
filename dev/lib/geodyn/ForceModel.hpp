#pragma ident "$Id: $"

/**
* @file ForceModel.hpp
* Force Model is a simple interface which allows uniformity among the various force
* models 
*/

#ifndef GPSTK_FORCE_MODEL_HPP
#define GPSTK_FORCE_MODEL_HPP

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
#include "Spacecraft.hpp"
#include "EarthBody.hpp"

namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /**
       * Force Model is a simple interface which allows uniformity among the various force
       * models 
       */
   class ForceModel
   {
   public:

         /// To identify every force model and make sure one type of
         /// force exist only one instance in the force model list.
         /// class indexed with FMI_BASE and FMI_LIST can't be added
         /// to the force model list
      enum ForceModelIndex
      {
         FMI_BASE       = 1000,  ///< For This class 'ForceModel'

         FMI_GEOEARTH,           ///< Geopotential of Earth
         FMI_GEOSUN,             ///< Geopotential of Sun
         FMI_GEOMOON,            ///< Geopotential of Moon 
         FMI_DRAG,               ///< Atmospheric Drag
         FMI_SRP,                ///< Solar Radiation Pressure
         FMI_RELATIVE,           ///< Relative Effect   
         FMT_EMPIRICAL,          ///< Empirical Force 
         
         //... add more here

         FMI_LIST   = 2000,      ///< For class 'ForceModelList'
      };

      enum ForceModelType
      {
         Cd,               // Coefficient of drag
         Cr,               // Coefficient of Reflectivity
      };

         /// Default constructor
      ForceModel()
      {         
         a.resize(3,0.0);
         da_dr.resize(3,3,0.0);
         da_dv.resize(3,3,0.0);
         da_dp.resize(3,0,0.0);      // default np = 0;

         da_dcd.resize(3,1,0.0);
         da_dcr.resize(3,1,0.0);
      }

         /// Default destructor
      virtual ~ForceModel(){}


         /// this is the real one to do computation
      virtual void doCompute(UTCTime t, EarthBody& bRef, Spacecraft& sc)
      {
         a.resize(3,0.0);
         da_dr.resize(3,3,0.0);
         da_dv.resize(3,3,0.0);
         da_dp.resize(3,0,0.0);      // default np = 0;

         da_dcd.resize(3,1,0.0);
         da_dcr.resize(3,1,0.0);

      }
         
         /// return the force model name
      virtual std::string modelName() const
      { return "ForceModel"; };


         /// return the force model index
      virtual int forceIndex() const
      { return FMI_BASE; }


         /**
          * Return the acceleration
          * @return  acceleration
          */
      virtual Vector<double> getAccel() const
      { return a; };

         /**
          * Return the partial derivative of acceleration wrt position
          * @return Matrix containing the partial derivative of acceleration wrt velocity
          */
      virtual Matrix<double> partialR() const
      { return da_dr; }

         /**
          * Return the partial derivative of acceleration wrt velocity
          * @return Matrix containing the partial derivative of acceleration wrt velocity
          */
      virtual Matrix<double> partialV() const
      { return da_dv; }

         /**
          * Return the partial derivative of acceleration wrt velocity
          * @return Matrix containing the partial derivative of acceleration wrt velocity
          */
      virtual Matrix<double> partialP() const
      { return da_dp; }

         /**
          * Return the partial derivative of acceleration wrt velocity
          * @return Matrix containing the partial derivative of acceleration wrt cd
          */
      virtual Matrix<double> partialCd() const
      { return da_dcd; } 

         /**
          * Return the partial derivative of acceleration wrt velocity
          * @return Matrix containing the partial derivative of acceleration wrt cr
          */
      virtual Matrix<double> partialCr() const
      { return da_dcr; }

         /** return number of np
          */
      int getNP() const
      { return da_dp.cols(); };

         /// get A Matrix
      Matrix<double> getAMatrix() const
      {
            /* A Matrix
            |                        |
            | 0         I      0      |
            |                        |
         A =| da_dr      da_dv   da_dp  |
            |                        |
            | 0         0      0      |
            |                        |
            */

         const int np = da_dp.cols();

         gpstk::Matrix<double> A(6+np,6+np,0.0);

         A(0,3) = 1.0;
         A(1,4) = 1.0;
         A(2,5) = 1.0;

         // da_dr
         A(3,0) = da_dr(0,0);
         A(3,1) = da_dr(0,1);
         A(3,2) = da_dr(0,2);
         A(4,0) = da_dr(1,0);
         A(4,1) = da_dr(1,1);
         A(4,2) = da_dr(1,2);
         A(5,0) = da_dr(2,0);
         A(5,1) = da_dr(2,1);
         A(5,2) = da_dr(2,2);

         // da_dv
         A(3,3) = da_dv(0,0);
         A(3,4) = da_dv(0,1);
         A(3,5) = da_dv(0,2);
         A(4,3) = da_dv(1,0);
         A(4,4) = da_dv(1,1);
         A(4,5) = da_dv(1,2);
         A(5,3) = da_dv(2,0);
         A(5,4) = da_dv(2,1);
         A(5,5) = da_dv(2,2);

         // da_dp
         for(int i=0;i<np;i++)
         {
            A(3,6+i) = da_dp(0,i);
            A(4,6+i) = da_dp(1,i);
            A(5,6+i) = da_dp(2,i);
         }

         return A;

      }  // End of method 'getAMatrix()'

      void test()
      {
         /*
         cout<<"test Force Model"<<endl;

         a.resize(3,2.0);
         da_dr.resize(3,3,3.0);
         da_dv.resize(3,3,4.0);
         da_dp.resize(3,2,5.0);
         writeToFile("default.fm");

         // it work well
         */
      }

   protected:

         /// Acceleration
      Vector<double> a;         // 3
      
         /// Partial derivative of acceleration wrt position
      Matrix<double> da_dr;      // 3*3
      
         /// Partial derivative of acceleration wrt velocity
      Matrix<double> da_dv;      // 3*3
      
         /// Partial derivative of acceleration wrt dynamic parameters
      Matrix<double> da_dp;      // 3*np
         
         /// Partial derivative of acceleration wrt Cd
      Matrix<double> da_dcd;      // 3*1
         
         /// Partial derivative of acceleration wrt Cr
      Matrix<double> da_dcr;      // 3*1

      

   }; // End of 'class ForceModel'

      /**
       * Stream output for DayTime objects.  Typically used for debugging.
       * @param s stream to append formatted DayTime to.
       * @param t DayTime to append to stream \c s.
       * @return reference to \c s.
       */
   inline std::ostream& operator<<( std::ostream& s,
                                    const gpstk::ForceModel& fm )
   {
      Vector<double> a = fm.getAccel();
      Matrix<double> da_dr = fm.partialR();
      Matrix<double> da_dv = fm.partialV();
      Matrix<double> da_dp = fm.partialP();

      s<<"a ["<<a.size()<<"]\n{\n"
         <<a<<endl<<"}\n\n";

      s<<"da/dr ["<<da_dr.rows()<<","<<da_dr.cols()<<"]\n{\n"
         <<da_dr<<endl<<"}\n\n";

      s<<"da/dv ["<<da_dv.rows()<<","<<da_dv.cols()<<"]\n{\n"
         <<da_dv<<endl<<"}\n\n";

      s<<"da/dp ["<<da_dp.rows()<<","<<da_dp.cols()<<"]\n{\n"
         <<da_dp<<endl<<"}\n\n";

      Matrix<double> A = fm.getAMatrix();

      s<<"A = ["<<A.rows()<<","<<A.cols()<<"]\n{\n"
         <<A<<endl<<"}\n\n";
   }

      // @}

}  // End of namespace 'gpstk'


#endif  // GPSTK_FORCE_MODEL_HPP


