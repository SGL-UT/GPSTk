#pragma ident "$Id: $"

/**
 * @file SolverWMS.cpp
 * Class to compute the Weighted Least Mean Squares Solution
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
#include "SolverWMS.hpp"
#include "MatrixFunctors.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int SolverWMS::classIndex = 6100000;


      // Returns an index identifying this object.
   int SolverWMS::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverWMS::getClassName() const
   { return "SolverWMS"; }


      /* Default constructor. When fed with GNSS data structures, the 
       *  default equation definition to be used is the common GNSS 
       * code equation.
       */
   SolverWMS::SolverWMS()
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


      /* Explicit constructor. Sets the default equation definition 
       *  to be used when fed with GNSS data structures.
       *
       * @param eqDef     gnssEquationDefinition to be used
       */
   SolverWMS::SolverWMS(const gnssEquationDefinition& eqDef)
   {
      setDefaultEqDefinition(eqDef);
      setIndex();
   }


      /* Compute the Weighted Least Mean Squares Solution of the given
       *  equations set.
       * @param prefitResiduals   Vector of prefit residuals
       * @param designMatrix      Design matrix for the equation system
       * @param weightVector      Vector of weights assigned to each
       *                          satellite.
       *
       * @return
       *  0 if OK
       *  -1 if problems arose
       */
   int SolverWMS::Compute(const Vector<double>& prefitResiduals,
                          const Matrix<double>& designMatrix,
                          const Vector<double>& weightVector)
      throw(InvalidSolver)
   {
         // By default, results are invalid
      valid = false;

         // Check that everyting has a proper size
      int wSize = static_cast<int>(weightVector.size());
      int pSize = static_cast<int>(prefitResiduals.size());
      if (!(wSize==pSize))
      {
         InvalidSolver e("prefitResiduals size does not match dimension \
                          of weightVector");
         GPSTK_THROW(e);
      }

      Matrix<double> wMatrix(wSize,wSize,0.0);  // Declare a weight matrix

         // Fill the weight matrix diagonal with the content of 
         // the weight vector
      for (int i=0; i<wSize; i++)
      {
         wMatrix(i,i) = weightVector(i);
      }

         // Call the more general SolverWMS::Compute() method
      return SolverWMS::Compute(prefitResiduals, designMatrix, wMatrix);
   }


      // Compute the Weighted Least Mean Squares Solution of the given
      //  equations set.
      // @param prefitResiduals   Vector of prefit residuals
      // @param designMatrix      Design matrix for equation system
      // @param weightMatrix      Matrix of weights
      //
      // @return
      //  0 if OK
      //  -1 if problems arose
      //
   int SolverWMS::Compute(const Vector<double>& prefitResiduals,
                          const Matrix<double>& designMatrix,
                          const Matrix<double>& weightMatrix)
      throw(InvalidSolver)
   {
         // By default, results are invalid
      valid = false;

      if (!(weightMatrix.isSquare()))
      {
         InvalidSolver e("Weight matrix is not square");
         GPSTK_THROW(e);
      }

      int wRow = static_cast<int>(weightMatrix.rows());
      int pRow = static_cast<int>(prefitResiduals.size());
      if (!(wRow==pRow))
      {
         InvalidSolver e("prefitResiduals size does not match dimension of \
                          weightMatrix");
         GPSTK_THROW(e);
      }

      int gCol = static_cast<int>(designMatrix.cols());

      int gRow = static_cast<int>(designMatrix.rows());
      if (!(gRow==pRow))
      {
         InvalidSolver e("prefitResiduals size does not match dimension \
                          of designMatrix");
         GPSTK_THROW(e);
      }

      Matrix<double> AT = transpose(designMatrix);
      covMatrix.resize(gCol, gCol);
      covMatrixNoWeight.resize(gCol, gCol);
      solution.resize(gCol);

         // Temporary storage for covMatrix. It will be inverted later
      covMatrix = AT * weightMatrix * designMatrix;

         // Let's try to invert AT*W*A  matrix
      try { 
         covMatrix = inverseChol( covMatrix );
      }
      catch(...)
      {
         InvalidSolver e("Unable to invert matrix covMatrix");
         GPSTK_THROW(e);
      }

         // Temporary storage for covMatrixNoWeight. It will be inverted later
      covMatrixNoWeight = AT * designMatrix;

         // Let's try to invert AT*A  matrix
      try { 
         covMatrixNoWeight = inverseChol( covMatrixNoWeight );
      }
      catch(...)
      {
         InvalidSolver e("Unable to invert matrix covMatrixNoWeight");
         GPSTK_THROW(e);
      }

         // Now, compute the Vector holding the solution...
      solution = covMatrix * AT * weightMatrix * prefitResiduals;

         // ... and the postfit residuals Vector
      postfitResiduals = prefitResiduals - designMatrix * solution;

         // If everything is fine so far, then the results should be valid
      valid = true;

      return 0;

   }  // end SolverWMS::Compute()



      /* Returns a reference to a satTypeValueMap object after solving 
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& SolverWMS::Process(satTypeValueMap& gData)
      throw(InvalidSolver)
   {
         // First, let's fetch the vector of prefit residuals
      Vector<double> prefit(gData.getVectorOfTypeID(defaultEqDef.header));
         // Second, generate the corresponding geometry/design matrix
      Matrix<double> dMatrix(gData.getMatrixOfTypes((*this).defaultEqDef.body));
         // Third, generate the appropriate weights vector
      Vector<double> weightsVector(gData.getVectorOfTypeID(TypeID::weight));

      try
      {
            // Call the Compute() method with the defined equation model.
            // This equation model MUST HAS BEEN previously set, usually when
            // creating the SolverWMS object with the appropriate constructor.
         Compute(prefit, dMatrix, weightsVector);
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

   }   // End SolverWMS::Process(const DayTime& time, satTypeValueMap& gData)


} // end namespace gpstk
