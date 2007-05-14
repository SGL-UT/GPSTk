
/**
 * @file SolverWMS.hpp
 * Class to compute the Weighted Least Mean Squares Solution
 */

#ifndef SOLVER_WMS_HPP
#define SOLVER_WMS_HPP

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
//  Dagoberto Salazar - gAGE. 2006
//
//============================================================================


#include "SolverLMS.hpp"


namespace gpstk
{
    /** @addtogroup GPSsolutions */
    /// @ingroup math
      //@{

      /**
       * This class computes the Weighted Least Mean Squares Solution of a given equations set.
       *
       * @sa SolverBase.hpp for base class.
       *
       */
    class SolverWMS : public SolverLMS
    {
    public:

        /// Implicit constructor
        SolverWMS() { valid = false; };


        /** Compute the Weighted Least Mean Squares Solution of the given equations set.
         * @param prefitResiduals   Vector of prefit residuals
         * @param designMatrix      Design matrix for the equation system
         * @param weightMatrix      Matrix of weights
         *
         * @return
         *  0 if OK
         *  -1 if problems arose
         */
        virtual int Compute(const Vector<double>& prefitResiduals, const Matrix<double>& designMatrix, const Matrix<double>& weightMatrix) throw(InvalidSolver);


        /** Compute the Weighted Least Mean Squares Solution of the given equations set.
         * @param prefitResiduals   Vector of prefit residuals
         * @param designMatrix      Design matrix for the equation system
         * @param weightVector      Vector of weights assigned to each satellite.
         *
         * @return
         *  0 if OK
         *  -1 if problems arose
         */
        virtual int Compute(const Vector<double>& prefitResiduals, const Matrix<double>& designMatrix, const Vector<double>& weightVector) throw(InvalidSolver)
        {
            // First, check that everyting has a proper size
            int wSize = (int) weightVector.size();
            int pSize = (int) prefitResiduals.size();
            if (!(wSize==pSize)) {
                InvalidSolver e("prefitResiduals size does not match dimension of weightVector");
                GPSTK_THROW(e);
            }

            Matrix<double> wMatrix(wSize,wSize,0.0);  // Declare a weight matrix

            // Fill the weight matrix diagonal with the content of the weight vector
            for (int i=0; i<wSize; i++) wMatrix(i,i) = weightVector(i);

            // Call the more general SolverWMS::Compute() method
            return SolverWMS::Compute(prefitResiduals, designMatrix, wMatrix);
        };


        /// Compute the Weighted Least Mean Squares Solution of the given equations set.
        virtual int Compute(const Vector<double>& prefitResiduals, const Matrix<double>& designMatrix) throw(InvalidSolver) 
        {
            return SolverLMS::Compute(prefitResiduals, designMatrix);
        };


        /// Covariance matrix without weights. This must be used to compute DOP
        Matrix<double> covMatrixNoWeight;


        /// Destructor.
        virtual ~SolverWMS() {};


   }; // class SolverLMS

   //@}

} // namespace

#endif
