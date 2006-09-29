
/**
 * @file SolverLMS.hpp
 * Class to compute the Least Mean Squares Solution
 */

#ifndef SOLVER_LMS_HPP
#define SOLVER_LMS_HPP

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


#include "SolverBase.hpp"


namespace gpstk
{
    /** @addtogroup GPSsolutions */
    /// @ingroup math
      //@{

      /**
       * This class computes the Least Mean Squares Solution of a given equations set.
       *
       * @sa SolverBase.hpp for base class.
       *
       */
    class SolverLMS : public SolverBase
    {
    public:

        /// Default constructor
        SolverLMS() throw(InvalidSolver) { valid = false; };


        /** Compute the Least Mean Squares Solution of the given equations set.
         * @param prefitResiduals   Vector of prefit residuals
         * @param designMatrix      Design matrix for the equation system
         *
         * @return
         *  0 if OK
         *  -1 if problems arose
         */
        virtual int Compute(const Vector<double>& prefitResiduals, const Matrix<double>& designMatrix) throw(InvalidSolver);

        /// Destructor.
        virtual ~SolverLMS() {};


   }; // class SolverLMS

   //@}

} // namespace

#endif
