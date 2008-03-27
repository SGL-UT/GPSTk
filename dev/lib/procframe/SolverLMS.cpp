#pragma ident "$Id$"

/**
 * @file SolverLMS.cpp
 * Class to compute the Least Mean Squares Solution
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2006, 2007
//
//============================================================================


#include "SolverBase.hpp"
#include "SolverLMS.hpp"
#include "MatrixFunctors.hpp"


namespace gpstk
{


      // Index initially assigned to this class
   int SolverLMS::classIndex = 6000000;


      // Returns an index identifying this object.
   int SolverLMS::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverLMS::getClassName() const
   { return "SolverLMS"; }


      /* Default constructor. When fed with GNSS data structures, the 
       *  default the equation definition to be used is the common GNSS 
       * code equation.
       */
   SolverLMS::SolverLMS()
   {
         // First, let's define a set with the typical unknowns
      TypeIDSet tempSet;
      tempSet.insert(TypeID::dx);
      tempSet.insert(TypeID::dy);
      tempSet.insert(TypeID::dz);
      tempSet.insert(TypeID::cdt);

         // Now, we build the default definition for a common GNSS 
         // code-based equation
      defaultEqDef.header = TypeID::prefitC;
      defaultEqDef.body = tempSet;
      setIndex();
   }


      // Compute the Least Mean Squares Solution of the given equations set.
      // @param prefitResiduals   Vector of prefit residuals
      // @param designMatrix      Design matrix for equation system
      //
      // @return
      //  0 if OK
      //  -1 if problems arose
      //
   int SolverLMS::Compute(const Vector<double>& prefitResiduals,
                          const Matrix<double>& designMatrix)
      throw(InvalidSolver)
   {
         // By default, results are invalid
      valid = false;

      int gCol = static_cast<int>(designMatrix.cols());

      int gRow = static_cast<int>(designMatrix.rows());
      int pRow = static_cast<int>(prefitResiduals.size());
      if (!(gRow==pRow))
      {
         InvalidSolver e("prefitResiduals size does not match dimension \
                          of designMatrix");
         GPSTK_THROW(e);
      }

      Matrix<double> AT = transpose(designMatrix);
      covMatrix.resize(gCol, gCol);
      solution.resize(gCol);

         // Temporary storage for covMatrix. It will be inverted later
      covMatrix = AT * designMatrix;

      // Let's try to invert AT*A   matrix
      try
      { 
         covMatrix = inverseChol( covMatrix );
      }
      catch(...)
      {
         InvalidSolver e("Unable to invert matrix covMatrix");
         GPSTK_THROW(e);
      }

         // Now, compute the Vector holding the solution...
      solution = covMatrix * AT * prefitResiduals;

         // ... and the postfit residuals Vector
      postfitResiduals = prefitResiduals - designMatrix * solution;

         // If everything is fine so far, then the results should be valid
      valid = true;

      return 0;

   }  // end SolverLMS::Compute()


      /* Returns a reference to a satTypeValueMap object after solving 
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& SolverLMS::Process(satTypeValueMap& gData)
      throw(InvalidSolver)
   {
         // First, let's fetch the vector of prefit residuals
      Vector<double> prefit(gData.getVectorOfTypeID(defaultEqDef.header));
         // Then, generate the corresponding geometry/design matrix
      Matrix<double> dMatrix(gData.getMatrixOfTypes(defaultEqDef.body));

      try
      {
            // Call the Compute() method with the defined equation model.
            // This equation model MUST HAS BEEN previously set, usually 
            // when creating the SolverLMS object with the appropriate
            // constructor.
         Compute(prefit, dMatrix);
      }
      catch(InvalidSolver& e)
      {
         GPSTK_RETHROW(e);
      }

         // Now we have to add the new values to the data structure
      if ( defaultEqDef.header == TypeID::prefitC )
      {
         gData.insertTypeIDVector(TypeID::postfitC, postfitResiduals);
      }

      if ( defaultEqDef.header == TypeID::prefitL )
      {
         gData.insertTypeIDVector(TypeID::postfitL, postfitResiduals);
      }

      return gData;

   }   // End SolverLMS::Process(const DayTime& time, satTypeValueMap& gData)


} // end namespace gpstk
