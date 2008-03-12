#pragma ident "$Id: $"

/**
 * @file SolverWMS.hpp
 * Class to compute the Weighted Least Mean Squares Solution
 */

#ifndef SOLVERWMS_HPP
#define SOLVERWMS_HPP

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


#include "SolverLMS.hpp"


namespace gpstk
{

      /** @addtogroup GPSsolutions */
      /// @ingroup math

      //@{

      /** This class computes the Weighted Least Mean Squares Solution of 
       *  a given equations set.
       * 
       * This class may be used either in a Vector- and Matrix-oriented way, 
       * or with GNSS data structure objects from "DataStructures" class.
       *
       * A typical way to use this class with GNSS data structures follows:
       *
       * @code
       *   RinexObsStream rin("ebre0300.02o");  // Data stream
       *
       *   // More declarations here: Ionospheric and tropospheric models, 
       *   // ephemeris, etc.
       *
       *   // Declare the modeler object, setting all the parameters in one
       *   // pass
       *   ModelObs model(ionoStore, mopsTM, bceStore, TypeID::C1);
       *   model.Prepare();     // Set initial position (Bancroft method)
       *
       *   // Declare a SolverWMS object
       *   SolverWMS solverWMS;
       *
       *   // This object will compute the appropriate MOPS weights
       *   ComputeMOPSWeights mopsW(nominalPos, bceStore);
       *
       *   gnssRinex gRin;
       *
       *   while(rin >> gRin) {
       *      gRin >> model >> mopsW >> solverWMS;
       *   }
       * @endcode
       *
       * The "SolverWMS" object will extract all the data it needs from the
       * GNSS data structure that is "gRin" and will try to solve the system
       * of equations using the Weighted-Least-Mean-Squares method. It will 
       * also insert back postfit residual data into "gRin" if it successfully
       * solves the equation system.
       *
       * Please note it needs some weights assigned to each satellite.
       * This can be achieved with objects from classes such as
       * "ComputeIURAWeights", "ComputeMOPSWeights", etc., but in any case 
       * this is a mandatory step.
       *
       * By default, it will build the geometry matrix from the values of 
       * coefficients dx, dy, dz and cdt, and the independent vector will be
       * composed of the code prefit residuals (TypeID::prefitC) values.
       *
       * You may change the former by redefining the default equation
       * definition to be used. For instance:
       *
       * @code
       *   TypeIDSet unknownsSet;
       *   unknownsSet.insert(TypeID::dLat);
       *   unknownsSet.insert(TypeID::dLon);
       *   unknownsSet.insert(TypeID::dH);
       *   unknownsSet.insert(TypeID::cdt);
       *
       *   // Create a new equation definition
       *   // newEq(independent value, set of unknowns)
       *   gnssEquationDefinition newEq(TypeID::prefitC, unknownsSet);
       *
       *   // Reconfigure solver
       *   solverWMS.setDefaultEqDefinition(newEq);
       * @endcode
       *
       * @sa SolverBase.hpp and SolverLMS for base classes.
       *
       */
   class SolverWMS : public SolverLMS
   {
   public:

         /** Default constructor. When fed with GNSS data structures, the 
          *  default equation definition to be used is the common GNSS 
          * code equation.
          */
      SolverWMS();


         /** Explicit constructor. Sets the default equation definition 
          *  to be used when fed with GNSS data structures.
          *
          * @param eqDef     gnssEquationDefinition to be used
          */
      SolverWMS(const gnssEquationDefinition& eqDef);


         /** Compute the Weighted Least Mean Squares Solution of the given
          *  equations set.
          * @param prefitResiduals   Vector of prefit residuals
          * @param designMatrix      Design matrix for the equation system
          * @param weightMatrix      Matrix of weights
          *
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int Compute(const Vector<double>& prefitResiduals,
                          const Matrix<double>& designMatrix,
                          const Matrix<double>& weightMatrix)
         throw(InvalidSolver);


         /** Compute the Weighted Least Mean Squares Solution of the given
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
      virtual int Compute(const Vector<double>& prefitResiduals,
                          const Matrix<double>& designMatrix,
                          const Vector<double>& weightVector)
         throw(InvalidSolver);


         /** Compute the Weighted Least Mean Squares Solution of the given
          *  equations set.
          * @param prefitResiduals   Vector of prefit residuals
          * @param designMatrix      Design matrix for the equation system
          */
      virtual int Compute(const Vector<double>& prefitResiduals,
                          const Matrix<double>& designMatrix)
         throw(InvalidSolver) 
      { return SolverLMS::Compute(prefitResiduals, designMatrix); };


         /** Returns a reference to a satTypeValueMap object after solving 
          *  the previously defined equation system.
          *
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process(satTypeValueMap& gData)
         throw(InvalidSolver);


         /// Covariance matrix without weights. This must be used to 
         /// compute DOP
      Matrix<double> covMatrixNoWeight;


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /** Sets the index to a given arbitrary value. Use with caution.
          *
          * @param newindex      New integer index to be assigned to 
          *                      current object.
          */
      SolverWMS& setIndex(const int newindex)
      { index = newindex; return (*this); };


         /// Destructor.
      virtual ~SolverWMS() {};


   private:


         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; }; 


   }; // class SolverWMS

      //@}

} // namespace
#endif // SOLVERWMS_HPP
