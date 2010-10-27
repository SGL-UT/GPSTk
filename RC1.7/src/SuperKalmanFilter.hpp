#pragma ident "$Id:$"

/**
 * @file SuperKalmanFilter.hpp
 * This class computes the solution using an adaptive kalman filter
 * with selection of the parameter weights.
 */

#ifndef GPSTK_SUPER_KALMANFILTER_HPP
#define GPSTK_SUPER_KALMANFILTER_HPP

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



#include "Exception.hpp"
#include "Matrix.hpp"
#include "Vector.hpp"
#include "SolverBase.hpp"


namespace gpstk
{

      /** @addtogroup GPSsolutions */
      /// @ingroup math

      //@{

      /** This class computes the solution using an adaptive kalman filter
       * with selection of the parameter weights.
       *
       * A typical way to use this class follows:
       *
       * @code
       *    // Declarations and initializations here...
       *
       *    SuperKalmanFilter kalman(xhat0, pmatrix);
       *
       *    while(cin >> x >> y)
       *    {
       *
       *       try
       *       {
       *
       *          meas(0) = x;
       *          meas(1) = y;
       *
       *          kalman.Compute(phimatrix, qmatrix, meas, hmatrix, rmatrix);
       *
       *          cout << kalman.xhat(0) << " " << kalman.xhat(1) << endl;
       *       }
       *       catch (Exception e)
       *       {
       *          cout << e;
       *       }
       *    }
       * @endcode
       *
       * This class is modified from 'SimpleKalmanFilter', some new methods
       * were added to do precise orbit determination. Comparing with the 
       * 'SimpleKalmanFilter' class, this class will have advanced features:
       * 1) robust
       * 2) adaptive with selection of the parameter weights.
       */
   class SuperKalmanFilter
   {
   public:

         /// Default constructor.
      SuperKalmanFilter();


         /** Common constructor.
          *
          * @param initialState     Vector setting the initial state of
          *                         the system.
          * @param initialErrorCovariance    Matrix setting the initial
          *                values of the a posteriori error covariance.
          */
      SuperKalmanFilter( const Vector<double>& initialState,
                         const Matrix<double>& initialErrorCovariance );



         /** Common constructor. This is meant to be used with one-dimensional
          * systems.
          *
          * @param initialValue      Initial value of system state.
          * @param initialErrorVariance  Initial value of the a posteriori
          *                              error variance.
          */
      SuperKalmanFilter( const double& initialValue,
                         const double& initialErrorVariance );


         /// Destructor.
      virtual ~SuperKalmanFilter() {};


         /** Reset method.
          *
          * This method will reset the filter, setting new values for initial
          * system state vector and the a posteriori error covariance matrix.
          *
          * @param initialState      Vector setting the initial state of
          *                          the system.
          * @param initialErrorCovariance    Matrix setting the initial
          *                   values of the a posteriori error covariance.
          */
      virtual void Reset( const Vector<double>& initialState,
                          const Matrix<double>& initialErrorCovariance );


         /** Reset method.
          *
          * This method will reset the filter, setting new values for initial
          * system  state and the a posteriori error variance. Used for
          * one-dimensional systems.
          *
          * @param initialValue      Initial value of system state.
          * @param initialErrorVariance  Initial value of the a posteriori
          *                              error variance.
          */
      virtual void Reset( const double& initialValue,
                          const double& initialErrorVariance );


         /** Compute the a posteriori estimate of the system state, as well
          *  as the a posteriori estimate error covariance matrix.
          *
          * @param phiMatrix         State transition matrix.
          * @param controlMatrix     Control matrix.
          * @param controlInput      Control input vector.
          * @param processNoiseCovariance    Process noise covariance matrix.
          * @param measurements      Measurements vector.
          * @param measurementsMatrix    Measurements matrix. Called geometry
          *                              matrix in GNSS.
          * @param measurementsNoiseCovariance   Measurements noise covariance
          *                                      matrix.
          *
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int Compute( const Matrix<double>& phiMatrix,
                           const Matrix<double>& controlMatrix,
                           const Vector<double>& controlInput,
                           const Matrix<double>& processNoiseCovariance,
                           const Vector<double>& measurements,
                           const Matrix<double>& measurementsMatrix,
                           const Matrix<double>& measurementsNoiseCovariance )
         throw(InvalidSolver);



         /** Compute the a posteriori estimate of the system state, as well
          *  as the a posteriori estimate error covariance matrix.
          *
          * @param stateVector       Predicted state vector
          * @param phiMatrix         State transition matrix.
          * @param controlMatrix     Control matrix.
          * @param controlInput      Control input vector.
          * @param processNoiseCovariance    Process noise covariance matrix.
          * @param measurements      Measurements vector.
          * @param measurementsMatrix    Measurements matrix. Called geometry
          *                              matrix in GNSS.
          * @param measurementsNoiseCovariance   Measurements noise covariance
          *                                      matrix.
          *
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int Compute( const Vector<double>& stateVector,
                           const Matrix<double>& phiMatrix,
                           const Matrix<double>& controlMatrix,
                           const Vector<double>& controlInput,
                           const Matrix<double>& processNoiseCovariance,
                           const Vector<double>& measurements,
                           const Matrix<double>& measurementsMatrix,
                           const Matrix<double>& measurementsNoiseCovariance )
         throw(InvalidSolver);


         /** Compute the a posteriori estimate of the system state, as well
          *  as the a posteriori estimate error covariance matrix. This
          *  version assumes that no control inputs act on the system.
          *
          * @param phiMatrix         State transition matrix.
          * @param processNoiseCovariance    Process noise covariance matrix.
          * @param measurements      Measurements vector.
          * @param measurementsMatrix    Measurements matrix. Called geometry
          *                              matrix in GNSS.
          * @param measurementsNoiseCovariance   Measurements noise covariance
          *                                      matrix.
          *
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int Compute( const Matrix<double>& phiMatrix,
                           const Matrix<double>& processNoiseCovariance,
                           const Vector<double>& measurements,
                           const Matrix<double>& measurementsMatrix,
                           const Matrix<double>& measurementsNoiseCovariance )
         throw(InvalidSolver);


         /** Compute the a posteriori estimate of the system state, as well
          *  as the a posteriori estimate error covariance matrix. This
          *  version assumes that no control inputs act on the system.
          *
          * @param stateVector       Predicted state vector
          * @param phiMatrix         State transition matrix.
          * @param processNoiseCovariance    Process noise covariance matrix.
          * @param measurements      Measurements vector.
          * @param measurementsMatrix    Measurements matrix. Called geometry
          *                              matrix in GNSS.
          * @param measurementsNoiseCovariance   Measurements noise covariance
          *                                      matrix.
          *
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int Compute( const Vector<double>& stateVector,
                           const Matrix<double>& phiMatrix,
                           const Matrix<double>& processNoiseCovariance,
                           const Vector<double>& measurements,
                           const Matrix<double>& measurementsMatrix,
                           const Matrix<double>& measurementsNoiseCovariance )
         throw(InvalidSolver);


         /** Compute the a posteriori estimate of the system state, as well
          *  as the a posteriori estimate error variance. Version for
          *  one-dimensional systems.
          *
          * @param phiValue          State transition gain.
          * @param controlGain       Control gain.
          * @param controlInput      Control input value.
          * @param processNoiseVariance    Process noise variance.
          * @param measurement       Measurement value.
          * @param measurementsGain  Measurements gain.
          * @param measurementsNoiseVariance   Measurements noise variance.
          *
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int Compute( const double& phiValue,
                           const double& controlGain,
                           const double& controlInput,
                           const double& processNoiseVariance,
                           const double& measurement,
                           const double& measurementsGain,
                           const double& measurementsNoiseVariance )
         throw(InvalidSolver);


         /** Compute the a posteriori estimate of the system state, as well
          *  as the a posteriori estimate error variance. Version for
          *  one-dimensional systems.
          *
          * @param stateValue        Predicted state value
          * @param phiValue          State transition gain.
          * @param controlGain       Control gain.
          * @param controlInput      Control input value.
          * @param processNoiseVariance    Process noise variance.
          * @param measurement       Measurement value.
          * @param measurementsGain  Measurements gain.
          * @param measurementsNoiseVariance   Measurements noise variance.
          *
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int Compute( const double& stateValue,
                           const double& phiValue,
                           const double& controlGain,
                           const double& controlInput,
                           const double& processNoiseVariance,
                           const double& measurement,
                           const double& measurementsGain,
                           const double& measurementsNoiseVariance )
         throw(InvalidSolver);


         /** Compute the a posteriori estimate of the system state, as well
          *  as the a posteriori estimate error variance. Version for
          *  one-dimensional systems without control input on the system.
          *
          * @param phiValue          State transition gain.
          * @param processNoiseVariance    Process noise variance.
          * @param measurement       Measurement value.
          * @param measurementsGain  Measurements gain.
          * @param measurementsNoiseVariance   Measurements noise variance.
          *
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int Compute( const double& phiValue,
                           const double& processNoiseVariance,
                           const double& measurement,
                           const double& measurementsGain,
                           const double& measurementsNoiseVariance )
         throw(InvalidSolver);


         /** Compute the a posteriori estimate of the system state, as well
          *  as the a posteriori estimate error variance. Version for
          *  one-dimensional systems without control input on the system.
          *
          * @param phiValue          State transition gain.
          * @param processNoiseVariance    Process noise variance.
          * @param measurement       Measurement value.
          * @param measurementsGain  Measurements gain.
          * @param measurementsNoiseVariance   Measurements noise variance.
          *
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int Compute( const double& stateValue,
                           const double& phiValue,
                           const double& processNoiseVariance,
                           const double& measurement,
                           const double& measurementsGain,
                           const double& measurementsNoiseVariance )
         throw(InvalidSolver);

   public:

         /// A posteriori state estimation. This is usually your target.
      Vector<double> xhat;


         /// A posteriori error covariance.
      Matrix<double> P;


         /// A priori state estimation.
      Vector<double> xhatminus;


         /// A priori error covariance.
      Matrix<double> Pminus;


         /// A weight factor, it's a identity matrix by default.
      Matrix<double> weightFactor;


   protected:

         /** Predicts (or "time updates") the a priori estimate of the
          *  system state, as well as the a priori estimate error covariance
          *  matrix.
          *
          * @param phiMatrix         State transition matrix.
          * @param previousState     Previous system state vector. It is the
          *                          last computed xhat.
          * @param previousErrorCovariance
          *                          Error covariance matrix of the previous state
          *                            
          * @param controlMatrix     Control matrix.
          * @param controlInput      Control input vector.
          * @param processNoiseCovariance    Process noise covariance matrix.
          *
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int Predict( const Matrix<double>& phiMatrix,
                           const Vector<double>& previousState,
                           const Matrix<double>& previousErrorCovariance,
                           const Matrix<double>& controlMatrix,
                           const Vector<double>& controlInput,
                           const Matrix<double>& processNoiseCovariance )
         throw(InvalidSolver);

      
         /** Corrects (or "measurement updates") the a posteriori estimate
          *  of the system state vector, as well as the a posteriori estimate
          *  error covariance matrix, using as input the predicted a priori
          *  state vector and error covariance matrix, plus measurements and
          *  associated matrices.
          *
          * @param measurements      Measurements vector.
          * @param measurementsMatrix    Measurements matrix. Called geometry
          *                              matrix in GNSS.
          * @param measurementsNoiseCovariance   Measurements noise covariance
          *                                      matrix.
          *
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int Correct( const Vector<double>& measurements,
                           const Matrix<double>& measurementsMatrix,
                           const Matrix<double>& measurementsNoiseCovariance )
         throw(InvalidSolver);


   }; // End of class 'SuperKalmanFilter'

      //@}

}  // End of namespace gpstk

#endif // GPSTK_SUPER_KALMANFILTER_HPP

