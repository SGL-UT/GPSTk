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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2006, 2007, 2008
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
 * @file SolverBase.hpp
 * Abstract base class for solver algorithms.
 */

#ifndef SOLVERBASE_HPP
#define SOLVERBASE_HPP

#include "Exception.hpp"
#include "Matrix.hpp"
#include "Vector.hpp"


namespace gpstk
{

      /// Thrown when some problem appeared when solving a given
      /// equation set
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(InvalidSolver, gpstk::Exception);


      /// @ingroup GPSsolutions 
      /// @ingroup math
      //@{

      /**
       * Abstract base class for solver algorithms.
       */
   class SolverBase
   {
   public:


         /// Implicit constructor
      SolverBase() : valid(false) {};

         /// Return validity of results
      bool isValid(void)
      { return valid; }

         /// Solution
      Vector<double> solution;

         /// Postfit-residuals.
      Vector<double> postfitResiduals;

         /// Covariance matrix
      Matrix<double> covMatrix;

         /// Destructor
      virtual ~SolverBase() {};


   protected:


        bool valid;         // true only if results are valid


   }; // End of class 'SolverBase'

      //@}

}  // End of namespace gpstk
#endif   // SOLVERBASE_HPP
