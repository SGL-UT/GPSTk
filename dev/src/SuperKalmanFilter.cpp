#pragma ident "$Id: $"

/**
 * @file SuperKalmanFilter.cpp
 * This class computes the solution using an adaptive kalman filter
 * with selection of the parameter weights.
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



#include "SuperKalmanFilter.hpp"
#include "MatrixFunctors.hpp"


namespace gpstk
{
      /// Default constructor.
   SuperKalmanFilter::SuperKalmanFilter()
      : xhat(1,0.0), P(1,1,0.0), xhatminus(1,0.0), Pminus(1,1,0.0),
        weightFactor(ident<double>(1))
   {
   }


      /* Common constructor.
       *
       * @param initialState     Vector setting the initial state of
       *                         the system.
       * @param initialErrorCovariance    Matrix setting the initial
       *                values of the a posteriori error covariance.
       */
   SuperKalmanFilter::SuperKalmanFilter( const Vector<double>& initialState,
                                         const Matrix<double>& initialErrorCovariance )
      : xhat(initialState), P(initialErrorCovariance),
        xhatminus(initialState.size(), 0.0),
        Pminus(initialErrorCovariance.rows(),
               initialErrorCovariance.cols(), 0.0),
        weightFactor(ident<double>(initialState.size()))
   {
   }


      /* Common constructor. This is meant to be used with one-dimensional
       * systems.
       *
       * @param initialValue      Initial value of system state.
       * @param initialErrorVariance  Initial value of the a posteriori
       *                              error variance.
       */
   SuperKalmanFilter::SuperKalmanFilter( const double& initialValue,
                                         const double& initialErrorVariance )
      : xhat(1,initialValue),
        P(1,1,initialErrorVariance), xhatminus(1,0.0),
        Pminus(1,1,0.0),
        weightFactor(ident<double>(1))
   {
   }


      /* Reset method.
       *
       * This method will reset the filter, setting new values for initial
       * system state vector and the a posteriori error covariance matrix.
       *
       * @param initialState      Vector setting the initial state of
       *                          the system.
       * @param initialErrorCovariance    Matrix setting the initial
       *                   values of the a posteriori error covariance.
       */
   void SuperKalmanFilter::Reset( const Vector<double>& initialState,
                                const Matrix<double>& initialErrorCovariance )
   {

      xhat = initialState;
      P = initialErrorCovariance;
      xhatminus.resize(initialState.size(), 0.0);
      Pminus.resize( initialErrorCovariance.rows(),
                     initialErrorCovariance.cols(), 0.0);

      weightFactor = ident<double>(xhat.size());

   }  // End of method 'SuperKalmanFilter::Reset()'


      /* Reset method.
       *
       * This method will reset the filter, setting new values for initial
       * system  state and the a posteriori error variance. Used for
       * one-dimensional systems.
       *
       * @param initialValue      Initial value of system state.
       * @param initialErrorVariance  Initial value of the a posteriori
       *                              error variance.
       */
   void SuperKalmanFilter::Reset( const double& initialValue,
                                   const double& initialErrorVariance )
   {

      xhat.resize(1, initialValue);
      P.resize(1,1, initialErrorVariance);
      xhatminus.resize(1, 0.0);
      Pminus.resize(1, 1, 0.0);

      weightFactor = ident<double>(xhat.size());

   }  // End of method 'SuperKalmanFilter::Reset()'


      // Predicts (or "time updates") the a priori estimate of the system
      // state, as well as the a priori estimate error covariance matrix.
      //
      // @param phiMatrix         State transition matrix.
      // @param previousState     Previous system state vector. It is the
      //                          last computed xhat.
      // @param previousErrorCovariance
      //                          Error covariance matrix of the previous state
      // @param controlMatrix     Control matrix.
      // @param controlInput      Control input vector.
      // @param processNoiseCovariance    Process noise covariance matrix.
      //
      // @return
      //  0 if OK
      //  -1 if problems arose
      //
   int SuperKalmanFilter::Predict( const Matrix<double>& phiMatrix,
                                   const Vector<double>& previousState,
                                   const Matrix<double>& previousErrorCovariance,
                                   const Matrix<double>& controlMatrix,
                                   const Vector<double>& controlInput,
                                   const Matrix<double>& processNoiseCovariance )
      throw(InvalidSolver)
   {
      // Let's check sizes before start
      int aposterioriStateRow(previousState.size());
      int controlInputRow(controlInput.size());

      int phiCol(phiMatrix.cols());
      int phiRow(phiMatrix.rows());

      int covCol(previousErrorCovariance.cols());
      int covRow(previousErrorCovariance.rows());

      int controlCol(controlMatrix.cols());
      int controlRow(controlMatrix.rows());

      int processNoiseRow(processNoiseCovariance.rows());

      if ( phiCol != phiRow )
      {
         InvalidSolver e("Predict(): State transition matrix is not square, \
                         and it must be.");
         GPSTK_THROW(e);
      }

      if ( phiCol != aposterioriStateRow )
      {
         InvalidSolver e("Predict(): Sizes of state transition matrix and \
                         a posteriori state estimation vector do not match.");
         GPSTK_THROW(e);
      }

      if ( controlCol != controlInputRow )
      {
         InvalidSolver e("Predict(): Sizes of control matrix and a control \
                         input vector do not match.");
         GPSTK_THROW(e);
      }

      if ( aposterioriStateRow != controlRow )
      {
         InvalidSolver e("Predict(): Sizes of control matrix and a \
                         posteriori state estimation vector do not match.");
         GPSTK_THROW(e);
      }

      if ( phiRow != processNoiseRow )
      {
         InvalidSolver e("Predict(): Sizes of state transition matrix and \
                         process noise covariance matrix do not match.");
         GPSTK_THROW(e);
      }

      if( (aposterioriStateRow != covRow) || (covRow != covCol) )
      {
         InvalidSolver e("Predict(): Sizes of state vector and \
                         state error covariance matrix do not match.");
         GPSTK_THROW(e);
      }

      // After checking sizes, lets' do the real prediction work
      try
      {
         // Compute the a priori state vector
         xhatminus = phiMatrix*previousState + controlMatrix * controlInput;

         Matrix<double> phiT(transpose(phiMatrix));

         // Compute the a priori estimate error covariance matrix
         Pminus = phiMatrix * previousErrorCovariance * phiT 
                + processNoiseCovariance;
      }
      catch(...)
      {
         InvalidSolver e("Predict(): Unable to predict next state.");
         GPSTK_THROW(e);
         return -1;
      }

      return 0;

   }  // End of method 'SuperKalmanFilter::Predict()'

     
      // Corrects (or "measurement updates") the a posteriori estimate of
      // the system state vector, as well as the a posteriori estimate error
      // covariance matrix, using as input the predicted a priori state vector
      // and error covariance matrix, plus measurements and associated
      // matrices.
      //
      // @param measurements      Measurements vector.
      // @param measurementsMatrix    Measurements matrix. Called geometry
      //                              matrix in GNSS.
      // @param measurementsNoiseCovariance   Measurements noise covariance
      //                                      matrix.
      //
      // @return
      //  0 if OK
      //  -1 if problems arose
      //
   int SuperKalmanFilter::Correct( const Vector<double>& measurements,
                                   const Matrix<double>& measurementsMatrix,
                                   const Matrix<double>& measurementsNoiseCovariance )
      throw(InvalidSolver)
   {
      // Let's check sizes before start
      int measRow(measurements.size());
      int aprioriStateRow(xhatminus.size());

      int mMRow(measurementsMatrix.rows());

      int mNCCol(measurementsNoiseCovariance.cols());
      int mNCRow(measurementsNoiseCovariance.rows());

      int pMCol(Pminus.cols());
      int pMRow(Pminus.rows());

      if ( ( mNCCol != mNCRow ) || 
         ( pMCol != pMRow )      )
      {
         InvalidSolver e("Correct(): Either Pminus or measurement covariance \
                         matrices are not square, and therefore not invertible.");
         GPSTK_THROW(e);
      }

      if ( mMRow != mNCRow )
      {
         InvalidSolver e("Correct(): Sizes of measurements matrix and \
                         measurements noise covariance matrix do not match.");
         GPSTK_THROW(e);
      }

      if ( mNCCol != measRow )
      {
         InvalidSolver e("Correct(): Sizes of measurements matrix and \
                         measurements vector do not match.");
         GPSTK_THROW(e);
      }

      if ( pMCol != aprioriStateRow )
      {
         InvalidSolver e("Correct(): Sizes of a priori error covariance \
                         matrix and a priori state estimation vector do not match.");
         GPSTK_THROW(e);
      }

      // After checking sizes, let's do the real correction work
      Matrix<double> invR;
      Matrix<double> invPMinus;
      Matrix<double> measMatrixT( transpose(measurementsMatrix) );

      try
      {

         invR = inverseChol(measurementsNoiseCovariance);

         // Some robust processing here
         Matrix<double> weightMatrix(invR);

         invR = weightMatrix;
      }
      catch(...)
      {
         InvalidSolver e("Correct(): Unable to compute invR matrix.");
         GPSTK_THROW(e);
         return -1;
      }

      try
      {

         invPMinus = inverseChol(Pminus);

         // Some adaptive processing here
         
         // warning:
         // weightFactor should  be set, by default it's identity
         // matrix, and that stands for a standard kalman filter. 

         Matrix<double> weightMatrix(invPMinus);

         invPMinus = weightFactor * weightMatrix * weightFactor;

      }
      catch(...)
      {
         InvalidSolver e("Correct(): Unable to compute invPMinus matrix.");
         GPSTK_THROW(e);
         return -1;
      }

      try
      {
         // Oliver Montenbruck P277
         Matrix<double> invTemp( measMatrixT * invR * measurementsMatrix 
                                +invPMinus );

         // Compute the a posteriori error covariance matrix
         P = inverseChol( invTemp );

      }
      catch(...)
      {
         InvalidSolver e("Correct(): Unable to compute P matrix.");
         GPSTK_THROW(e);
         return -1;
      }

      try
      {

         // Compute the a posteriori state estimation
         xhat = P * ( (measMatrixT * invR * measurements) 
                     +(invPMinus * xhatminus) );

      }
      catch(Exception e)
      {
         InvalidSolver eis("Correct(): Unable to compute xhat.");
         GPSTK_THROW(eis);
         return -1;
      }

      return 0;

   }  // End of method 'SuperKalmanFilter::Correct()'


      // Compute the a posteriori estimate of the system state, as well as
      // the a posteriori estimate error covariance matrix.
      //
      // @param phiMatrix         State transition matrix.
      // @param controlMatrix     Control matrix.
      // @param controlInput      Control input vector.
      // @param processNoiseCovariance    Process noise covariance matrix.
      // @param measurements      Measurements vector.
      // @param measurementsMatrix    Measurements matrix. Called geometry
      //                              matrix in GNSS.
      // @param measurementsNoiseCovariance   Measurements noise covariance
      //                                      matrix.
      //
      // @return
      //  0 if OK
      //  -1 if problems arose
      //
   int SuperKalmanFilter::Compute( const Matrix<double>& phiMatrix,
                                   const Matrix<double>& controlMatrix,
                                   const Vector<double>& controlInput,
                                   const Matrix<double>& processNoiseCovariance,
                                   const Vector<double>& measurements,
                                   const Matrix<double>& measurementsMatrix,
                                   const Matrix<double>& measurementsNoiseCovariance )
      throw(InvalidSolver)
   {

      try
      {
         Predict( phiMatrix,
            xhat,
            P,
            controlMatrix,
            controlInput,
            processNoiseCovariance );

         Correct( measurements,
            measurementsMatrix,
            measurementsNoiseCovariance );
      }
      catch(InvalidSolver e)
      {
         GPSTK_THROW(e);
         return -1;
      }

      return 0;

   }  // End of method 'SuperKalmanFilter::Compute()'


         /* Compute the a posteriori estimate of the system state, as well
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
      int SuperKalmanFilter::Compute( const Vector<double>& stateVector,
                           const Matrix<double>& phiMatrix,
                           const Matrix<double>& controlMatrix,
                           const Vector<double>& controlInput,
                           const Matrix<double>& processNoiseCovariance,
                           const Vector<double>& measurements,
                           const Matrix<double>& measurementsMatrix,
                           const Matrix<double>& measurementsNoiseCovariance )
         throw(InvalidSolver)
      {
         try
         {
            // We should do some check before starting
            if(stateVector.size() != xhat.size())
            {
               InvalidSolver e("Compute(): Sizes of predicted state vector and \
                               a priori state vector do not match.");
               GPSTK_THROW(e);
            }

            Predict( phiMatrix,
               xhat,
               P,
               controlMatrix,
               controlInput,
               processNoiseCovariance );

            xhatminus = stateVector;

            Correct( measurements,
               measurementsMatrix,
               measurementsNoiseCovariance );
         }
         catch(InvalidSolver e)
         {
            GPSTK_THROW(e);
            return -1;
         }

         return 0;

      }  // End of method 'SuperKalmanFilter::Compute()'


      // Compute the a posteriori estimate of the system state, as well as
      // the a posteriori estimate error covariance matrix. This version
      // assumes that no control inputs act on the system.
      //
      // @param phiMatrix         State transition matrix.
      // @param processNoiseCovariance    Process noise covariance matrix.
      // @param measurements      Measurements vector.
      // @param measurementsMatrix    Measurements matrix. Called geometry
      //                              matrix in GNSS.
      // @param measurementsNoiseCovariance   Measurements noise covariance
      //                                      matrix.
      //
      // @return
      //  0 if OK
      //  -1 if problems arose
      //
   int SuperKalmanFilter::Compute( const Matrix<double>& phiMatrix,
                                   const Matrix<double>& processNoiseCovariance,
                                   const Vector<double>& measurements,
                                   const Matrix<double>& measurementsMatrix,
                                   const Matrix<double>& measurementsNoiseCovariance )
      throw(InvalidSolver)
   {

      try
      {
         Matrix<double> dummyControMatrix(xhat.size(),1,0.0);
         Vector<double> dummyControlInput(1,0.0);

         Predict( phiMatrix,
            xhat,
            P,
            dummyControMatrix,
            dummyControlInput,
            processNoiseCovariance );

         Correct( measurements,
            measurementsMatrix,
            measurementsNoiseCovariance );
      }
      catch(InvalidSolver e)
      {
         GPSTK_THROW(e);
         return -1;
      }

      return 0;

   }  // End of method 'SuperKalmanFilter::Compute()'


         /* Compute the a posteriori estimate of the system state, as well
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
      int SuperKalmanFilter::Compute( const Vector<double>& stateVector,
                           const Matrix<double>& phiMatrix,
                           const Matrix<double>& processNoiseCovariance,
                           const Vector<double>& measurements,
                           const Matrix<double>& measurementsMatrix,
                           const Matrix<double>& measurementsNoiseCovariance )
         throw(InvalidSolver)
      {
         try
         {
            // We should do some check before starting
            if(stateVector.size() != xhat.size())
            {
               InvalidSolver e("Compute(): Sizes of predicted state vector and \
                               a priori state vector do not match.");
               GPSTK_THROW(e);
            }

            Matrix<double> dummyControMatrix(xhat.size(),1,0.0);
            Vector<double> dummyControlInput(1,0.0);

            Predict( phiMatrix,
               xhat,
               P,
               dummyControMatrix,
               dummyControlInput,
               processNoiseCovariance );

            xhatminus = stateVector;

            Correct( measurements,
               measurementsMatrix,
               measurementsNoiseCovariance );
         }
         catch(InvalidSolver e)
         {
            GPSTK_THROW(e);
            return -1;
         }

         return 0;

      }  // End of method 'SuperKalmanFilter::Compute()'


         // Compute the a posteriori estimate of the system state, as well as
         // the a posteriori estimate error variance. Version for
         // one-dimensional systems.
         //
         // @param phiValue          State transition gain.
         // @param controlGain       Control gain.
         // @param controlInput      Control input value.
         // @param processNoiseVariance    Process noise variance.
         // @param measurement       Measurement value.
         // @param measurementsGain  Measurements gain.
         // @param measurementsNoiseVariance   Measurements noise variance.
         //
         // @return
         //  0 if OK
         //  -1 if problems arose
         //
      int SuperKalmanFilter::Compute( const double& phiValue,
                                      const double& controlGain,
                                      const double& controlInput,
                                      const double& processNoiseVariance,
                                      const double& measurement,
                                      const double& measurementsGain,
                                      const double& measurementsNoiseVariance )
         throw(InvalidSolver)
      {

         try
         {
            const int size = xhat.size(); // 1

            Matrix<double> phiMatrix(size,size,phiValue);
            Matrix<double> dummyControlMatrix(size,1,controlGain);
            Vector<double> dummyControlInput(1,controlInput);
            Matrix<double> prvMatrix(size,size,processNoiseVariance);

            Vector<double> dummyMeasurements(1,measurement);
            Matrix<double> measurementsMatrix(1,size,measurementsGain);
            Matrix<double> mnvMatrix(1,1,measurementsNoiseVariance);

            Compute(phiMatrix,dummyControlMatrix,dummyControlInput,prvMatrix,
               dummyMeasurements,measurementsMatrix,mnvMatrix);
         }
         catch(InvalidSolver e)
         {
            GPSTK_THROW(e);
            return -1;
         }

         return 0;

      }  // End of method 'SuperKalmanFilter::Compute()'


         // Compute the a posteriori estimate of the system state, as well as
         // the a posteriori estimate error variance. Version for
         // one-dimensional systems.
         //
         // @param stateValue        Predicted state value
         // @param phiValue          State transition gain.
         // @param controlGain       Control gain.
         // @param controlInput      Control input value.
         // @param processNoiseVariance    Process noise variance.
         // @param measurement       Measurement value.
         // @param measurementsGain  Measurements gain.
         // @param measurementsNoiseVariance   Measurements noise variance.
         //
         // @return
         //  0 if OK
         //  -1 if problems arose
         //
      int SuperKalmanFilter::Compute( const double& stateValue, 
                                      const double& phiValue,
                                      const double& controlGain,
                                      const double& controlInput,
                                      const double& processNoiseVariance,
                                      const double& measurement,
                                      const double& measurementsGain,
                                      const double& measurementsNoiseVariance )
         throw(InvalidSolver)
      {

         try
         {
            const int size = xhat.size(); // 1

            Matrix<double> phiMatrix(size,size,phiValue);
            Matrix<double> dummyControlMatrix(size,1,controlGain);
            Vector<double> dummyControlInput(1,controlInput);
            Matrix<double> prvMatrix(size,size,processNoiseVariance);

            Vector<double> dummyMeasurements(1,measurement);
            Matrix<double> measurementsMatrix(1,size,measurementsGain);
            Matrix<double> mnvMatrix(1,1,measurementsNoiseVariance);

            Vector<double> stateVector(size,stateValue);

            Compute(stateVector,phiMatrix,dummyControlMatrix,dummyControlInput,prvMatrix,
               dummyMeasurements,measurementsMatrix,mnvMatrix);
         }
         catch(InvalidSolver e)
         {
            GPSTK_THROW(e);
            return -1;
         }

         return 0;

      }  // End of method 'SuperKalmanFilter::Compute()'


         // Compute the a posteriori estimate of the system state, as well as
         // the a posteriori estimate error variance. Version for
         // one-dimensional systems without control input on the system.
         //
         // @param phiValue          State transition gain.
         // @param processNoiseVariance    Process noise variance.
         // @param measurement       Measurement value.
         // @param measurementsGain  Measurements gain.
         // @param measurementsNoiseVariance   Measurements noise variance.
         //
         // @return
         //  0 if OK
         //  -1 if problems arose
         //
      int SuperKalmanFilter::Compute( const double& phiValue,
                                      const double& processNoiseVariance,
                                      const double& measurement,
                                      const double& measurementsGain,
                                      const double& measurementsNoiseVariance )
         throw(InvalidSolver)
      {

         try
         {
            const int size = xhat.size(); // 1

            Matrix<double> phiMatrix(size,size,phiValue);
            Matrix<double> dummyControlMatrix(size,1,0.0);
            Vector<double> dummyControlInput(1,0.0);
            Matrix<double> prvMatrix(size,size,processNoiseVariance);

            Vector<double> dummyMeasurements(1,measurement);
            Matrix<double> measurementsMatrix(1,size,measurementsGain);
            Matrix<double> mnvMatrix(1,1,measurementsNoiseVariance);

            Compute(phiMatrix,dummyControlMatrix,dummyControlInput,prvMatrix,
               dummyMeasurements,measurementsMatrix,mnvMatrix);
         }
         catch(InvalidSolver e)
         {
            GPSTK_THROW(e);
            return -1;
         }

         return 0;

      }  // End of method 'SuperKalmanFilter::Compute()'


         // Compute the a posteriori estimate of the system state, as well as
         // the a posteriori estimate error variance. Version for
         // one-dimensional systems without control input on the system.
         //
         // @param stateValue        Predicted state value
         // @param phiValue          State transition gain.
         // @param processNoiseVariance    Process noise variance.
         // @param measurement       Measurement value.
         // @param measurementsGain  Measurements gain.
         // @param measurementsNoiseVariance   Measurements noise variance.
         //
         // @return
         //  0 if OK
         //  -1 if problems arose
         //
      int SuperKalmanFilter::Compute( const double& stateValue,
                                      const double& phiValue,
                                      const double& processNoiseVariance,
                                      const double& measurement,
                                      const double& measurementsGain,
                                      const double& measurementsNoiseVariance )
         throw(InvalidSolver)
      {

         try
         {
            const int size = xhat.size(); // 1

            Matrix<double> phiMatrix(size,size,phiValue);
            Matrix<double> dummyControlMatrix(size,1,0.0);
            Vector<double> dummyControlInput(1,0.0);
            Matrix<double> prvMatrix(size,size,processNoiseVariance);

            Vector<double> dummyMeasurements(1,measurement);
            Matrix<double> measurementsMatrix(1,size,measurementsGain);
            Matrix<double> mnvMatrix(1,1,measurementsNoiseVariance);

            Vector<double> stateVector(size,stateValue);

            Compute(stateVector,phiMatrix,dummyControlMatrix,dummyControlInput,prvMatrix,
               dummyMeasurements,measurementsMatrix,mnvMatrix);
         }
         catch(InvalidSolver e)
         {
            GPSTK_THROW(e);
            return -1;
         }

         return 0;

      }  // End of method 'SuperKalmanFilter::Compute()'

}  // End of namespace gpstk


