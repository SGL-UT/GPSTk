#pragma ident "$Id$"

/**
 * @file Variable.hpp
 * Class to define and handle GNSS variables.
 */

#ifndef GPSTK_VARIABLE_HPP
#define GPSTK_VARIABLE_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009
//
//============================================================================



#include "DataStructures.hpp"
#include "StochasticModel.hpp"



namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /// Class to define and handle GNSS variables.
   class Variable
   {
   public:

         /// Default constructor for Variable
      Variable();


         /** Common constructor for Variable.
          *  By default, it is indexed by SourceID.
          *
          * @param type             TypeID of variable.
          * @param pModel           Pointer to StochasticModel associated with
          *                         this variable. By default, it is a white
          *                         noise model.
          * @param sourceIndexed    Whether this variable is SourceID-indexed
          *                         or not. By default, it IS SourceID-indexed.
          * @param satIndexed       Whether this variable is SatID-indexed
          *                         or not. By default, it is NOT.
          * @param variance         Initial variance assigned to this variable.
          * @param coef             Default coefficient assigned.
          */
      Variable( const TypeID& type,
                StochasticModel* pModel   = &Variable::defaultModel,
                bool sourceIndexed        = true,
                bool satIndexed           = false,
                double variance           = 4.0e14,
                double coef               = 1.0 );


         /// Get variable type
      TypeID getType() const
      { return varType; };


         /** Set variable type
          *
          * @param type        New TypeID of variable.
          */
      Variable& setType(const TypeID& type)
      { varType = type; return (*this); };


         /// Get variable model pointer
      StochasticModel* getModel() const
      { return pVarModel; };


         /** Set variable model
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    this variable. By default, it is a white
          *                    noise model.
          */
      Variable& setModel(StochasticModel* pModel)
      { pVarModel = pModel; return (*this); };


         /// Get if this variable is SourceID-indexed
      bool getSourceIndexed() const
      { return isSourceIndexed; };


         /** Set if this variable is SourceID-indexed.
          *
          * @param sourceIndexed    Whether this variable is SourceID-indexed
          *                         or not. By default, it IS SourceID-indexed.
          */
      Variable& setSourceIndexed(bool sourceIndexed)
      { isSourceIndexed = sourceIndexed; return (*this); };


         /// Get if this variable is SatID-indexed.
      bool getSatIndexed() const
      { return isSatIndexed; };


         /** Set if this variable is SatID-indexed.
          *
          * @param satIndexed       Whether this variable is SatID-indexed
          *                         or not. By default, it is NOT.
          */
      Variable& setSatIndexed(bool satIndexed)
      { isSatIndexed = satIndexed; return (*this); };


         /// Get value of initial variance assigned to this variable.
      double getInitialVariance() const
      { return initialVariance; };


         /** Set value of initial variance assigned to this variable.
          *
          * @param variance      Initial variance assigned to this variable.
          */
      Variable& setInitialVariance(double variance)
      { initialVariance = variance; return (*this); };


         /// Get value of default coefficient assigned to this variable.
      double getDefaultCoefficient() const
      { return defaultCoefficient; };


         /** Set value of default coefficient assigned to this variable.
          *
          * @param coef    Default coefficient assigned to this variable.
          */
      Variable& setDefaultCoefficient(double coef)
      { defaultCoefficient = coef; return (*this); };


         /// Equality operator
      virtual bool operator==(const Variable& right) const;


         /// This ordering is somewhat arbitrary, but is required to be able
         /// to use a Variable as an index to a std::map, or as part of a
         /// std::set.
      virtual bool operator<(const Variable& right) const;


         /// Inequality operator
      bool operator!=(const Variable& right) const
      { return !(operator==(right)); }


         /// Assignment operator.
      virtual Variable& operator=(const Variable& right);


         /// SourceID object representing all sources : type(Unknown),
         /// sourceName("").
      static SourceID allSources;


         /// SourceID object representing "some" sources : type(Mixed),
         /// sourceName("").
      static SourceID someSources;


         /// SatID object representing no satellites:
         /// system(systemUnknown), id(-1).
      static SatID noSats;


         /// SatID object representing all satellites:
         /// system(systemMixed), id(-1).
      static SatID allSats;


         /// SatID object representing all satellites of GPS System:
         /// system(systemGPS), id(-1).
      static SatID allGPSSats;


         /// SatID object representing all satellites of Galileo System:
         /// system(systemGalileo), id(-1).
      static SatID allGalileoSats;


         /// SatID object representing all satellites of Glonass System:
         /// system(systemGlonass), id(-1).
      static SatID allGlonassSats;


         /// Default stochastic model to be assigned to variables.
      static WhiteNoiseModel defaultModel;


         /// Destructor
      virtual ~Variable() {};


   private:


         /// Type of the variable
      TypeID varType;


         /// Pointer stochastic model applying to variable
      StochasticModel* pVarModel;


         /** Whether this variable is or not source-specific. By default
          *  it is true, which means that this variable is different for
          *  each source (p.e., the TypeID::dx position variable of a given
          *  station).
          */
      bool isSourceIndexed;


         /** Whether this variable is or not satellite-specific. By default
          *  it is false, which means that this variable is the same
          *  independently from the satellites involved (p.e., the TypeID::dx
          *  position variable of a given station).
          *
          *  On the other hand, if this variable is satellite-specific (i.e.,
          *  this field is true), this means that this variable is different
          *  for each satellite on view; for instance, the TypeID::dtSat
          *  satellite clock offset variable.
          */
      bool isSatIndexed;


         /// Value of initial variance assigned to this variable.
      double initialVariance;


         /// Value of default coefficient assigned to this variable.
      double defaultCoefficient;


         /** Initializing function
          *
          * @param type        TypeID of variable.
          * @param pModel      Pointer to StochasticModel associated with
          *                    this variable. By default, it is a white
          *                    noise model.
          * @param variance    Initial variance assigned to this variable.
          * @param coef        Default coefficient assigned.
          */
      void Init( const TypeID& type,
                 StochasticModel* pModel = &Variable::defaultModel,
                 double variance = 4.0e14,
                 double coef     = 1.0 );


   }; // End of class 'Variable'

      //@}

}  // End of namespace gpstk
#endif   // GPSTK_VARIABLE_HPP
