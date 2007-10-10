
/**
 * @file SolverLMS.hpp
 * Class to compute the Least Mean Squares Solution
 */

#ifndef GPSTK_SOLVER_LMS_HPP
#define GPSTK_SOLVER_LMS_HPP

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
#include "TypeID.hpp"
#include "ProcessingClass.hpp"


namespace gpstk
{
    /** @addtogroup GPSsolutions */
    /// @ingroup math
      //@{

      /** This class computes the Least Mean Squares Solution of a given equations set.
       * 
       * This class may be used either in a Vector- and Matrix-oriented way, or
       * with GNSS data structure objects from "DataStructures" class.
       *
       * A typical way to use this class with GNSS data structures follows:
       *
       * @code
       *   RinexObsStream rin("ebre0300.02o");  // Data stream
       *
       *   // More declarations here: Ionospheric and tropospheric models, 
       *   // ephemeris, etc.
       *
       *   // Declare the modeler object, setting all the parameters in one pass
       *   ModelObs model(ionoStore, mopsTM, bceStore, TypeID::C1);
       *   model.Prepare();     // Set initial position (Bancroft method)
       *
       *   // Declare a SolverLMS object
       *   SolverLMS solver;
       *
       *   gnssRinex gRin;
       *
       *   while(rin >> gRin) {
       *      gRin >> model >> solver;
       *   }
       * @endcode
       *
       * The "SolverLMS" object will extract all the data it needs from the
       * GNSS data structure that is "gRin" and will try to solve the system
       * of equations using the Least-Mean-Squares method. It will also insert
       * back postfit residual data into "gRin" if it successfully solves the
       * equation system.
       *
       * By default, it will build the geometry matrix from the values of 
       * coefficients dx, dy, dz and cdt, and the independent vector will be
       * composed of the code prefit residuals (TypeID::prefitC) values.
       *
       * You may change the former by redefining the default equation definition 
       * to be used. For instance:
       *
       * @code
       *   TypeIDSet unknownsSet;
       *   unknownsSet.insert(TypeID::dLat);
       *   unknownsSet.insert(TypeID::dLon);
       *   unknownsSet.insert(TypeID::dH);
       *   unknownsSet.insert(TypeID::cdt);
       *
       *   // Create a new equation definition (independent value, unknowns set)
       *   gnssEquationDefinition newEq(TypeID::prefitC, unknownsSet);
       *
       *   // Reconfigure solver
       *   solver.setDefaultEqDefinition(newEq);
       * @endcode
       *
       * @sa SolverBase.hpp for base class.
       *
       */
    class SolverLMS : public SolverBase, public ProcessingClass
    {
    public:

        /// Default constructor. When fed with GNSS data structures, the 
        /// default the equation definition to be used is the common GNSS 
        /// code equation.
        SolverLMS()
        {
            // First, let's define a set with the typical unknowns
            TypeIDSet tempSet;
            tempSet.insert(TypeID::dx);
            tempSet.insert(TypeID::dy);
            tempSet.insert(TypeID::dz);
            tempSet.insert(TypeID::cdt);

            // Now, we build the default definition for a common GNSS code equation
            defaultEqDef.header = TypeID::prefitC;
            defaultEqDef.body = tempSet;
            setIndex();
        };


        /** Explicit constructor. Sets the default equation definition to be used when fed with GNSS data structures.
         *
         * @param eqDef     gnssEquationDefinition to be used
         */
        SolverLMS(const gnssEquationDefinition& eqDef) : defaultEqDef(eqDef)
        {
            setIndex();
        };


        /** Compute the Least Mean Squares Solution of the given equations set.
         * @param prefitResiduals   Vector of prefit residuals
         * @param designMatrix      Design matrix for the equation system
         *
         * @return
         *  0 if OK
         *  -1 if problems arose
         */
        virtual int Compute(const Vector<double>& prefitResiduals, const Matrix<double>& designMatrix) throw(InvalidSolver);


        /** Returns a reference to a satTypeValueMap object after solving the previously defined equation system.
         *
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Process(satTypeValueMap& gData) throw(InvalidSolver);


        /** Returns a reference to a gnnsSatTypeValue object after solving the previously defined equation system.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData) throw(InvalidSolver)
        {
            (*this).Process(gData.body);
            return gData;
        };


        /** Returns a reference to a gnnsRinex object after solving the previously defined equation system.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssRinex& Process(gnssRinex& gData) throw(InvalidSolver)
        {
            (*this).Process(gData.body);
            return gData;
        };


        /** Method to set the default equation definition to be used when fed with GNSS data structures.
         * @param eqDef     gnssEquationDefinition to be used by default
         */
        virtual void setDefaultEqDefinition(const gnssEquationDefinition& eqDef)
        {
           defaultEqDef = eqDef;
        };


        /// Method to get the default equation definition being used with GNSS data structures.
        virtual gnssEquationDefinition getDefaultEqDefinition() const
        {
           return defaultEqDef;
        };


        /// Returns an index identifying this object.
        virtual int getIndex(void) const;


        /// Returns a string identifying this object.
        virtual std::string getClassName(void) const;


        /** Sets the index to a given arbitrary value. Use with caution.
         *
         * @param newindex      New integer index to be assigned to current object.
         */
        void setIndex(const int newindex) { (*this).index = newindex; };


        /// Destructor.
        virtual ~SolverLMS() {};


    protected:

        /// Default equation definition to be used when fed with GNSS data structures.
        gnssEquationDefinition defaultEqDef;


    private:


        /// Initial index assigned to this class.
        static int classIndex;

        /// Index belonging to this object.
        int index;

        /// Sets the index and increment classIndex.
        void setIndex(void) { (*this).index = classIndex++; }; 


   }; // class SolverLMS


   //@}

} // namespace

#endif
