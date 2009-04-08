#pragma ident "$Id$"

/**
 * @file Variable.cpp
 * Class to define and handle GNSS variables.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009
//
//============================================================================


#include "Variable.hpp"


namespace gpstk
{


      // SourceID object representing all sources : type(Unknown),
      // sourceName("").
   SourceID Variable::allSources;


      // SourceID object representing "some" sources : type(Mixed),
      // sourceName("").
   SourceID Variable::someSources(SourceID::Mixed, "");


      // SatID object representing no satellites:
      // system(systemUnknown), id(-1).
   SatID Variable::noSats( -1, SatID::systemUnknown );


      // SatID object representing all satellites:
      // system(systemMixed), id(-1).
   SatID Variable::allSats( -1, SatID::systemMixed );


      // SatID object representing all satellites of GPS System:
      // system(systemGPS), id(-1).
   SatID Variable::allGPSSats;


      // SatID object representing all satellites of Galileo System:
      // system(systemGalileo), id(-1).
   SatID Variable::allGalileoSats( -1, SatID::systemGalileo );


      // SatID object representing all satellites of Glonass System:
      // system(systemGlonass), id(-1).
   SatID Variable::allGlonassSats( -1, SatID::systemGlonass );


      // Default stochastic model to be assigned to variables.
   WhiteNoiseModel Variable::defaultModel;



      // Default constructor for Variable
   Variable::Variable()
   {

      TypeID type;   // Unknown/undefined variable type

         // Call Init method
      Init( type );

   }  // End of 'Variable::Variable()'



      /* Common constructor for Variable.
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
   Variable::Variable( const TypeID& type,
                       StochasticModel* pModel,
                       bool sourceIndexed,
                       bool satIndexed,
                       double variance,
                       double coef )
   {

         // Call Init method
      Init( type,
            pModel,
            variance,
            coef );

         // This couple lines override settings by Init.
      isSourceIndexed = sourceIndexed;
      isSatIndexed = satIndexed;

   }  // End of 'Variable::Variable()'



      /* Initializing function
       *
       * @param type        TypeID of variable.
       * @param pModel      Pointer to StochasticModel associated with
       *                    this variable. By default, it is a white
       *                    noise model.
       * @param variance    Initial variance assigned to this variable.
       * @param coef        Default coefficient assigned.
       */
   void Variable::Init( const TypeID& type,
                        StochasticModel* pModel,
                        double variance,
                        double coef )
   {

      varType = type;

      if(pModel == NULL)
      {
         pVarModel = &defaultModel;
      }
      else
      {
         pVarModel = pModel;
      }

         // By default, it is source-indexed
      isSourceIndexed = true;

         // By default, it is not source indexed
      isSatIndexed = false;

         // Set initial variance
      initialVariance = variance;

      defaultCoefficient = coef;

      return;

   }  // End of method 'Variable::Init()'



      // Equality operator
   bool Variable::operator==(const Variable& right) const
   {

      return ( ( varType == right.getType() )                           &&
               ( pVarModel == right.getModel() )                        &&
               ( isSourceIndexed == right.getSourceIndexed() )          &&
               ( isSatIndexed == right.getSatIndexed() )                &&
               ( initialVariance == right.getInitialVariance() )        &&
               ( defaultCoefficient == right.getDefaultCoefficient() ) );

   }  // End of 'Variable::operator=='



      // This ordering is somewhat arbitrary, but is required to be able
      // to use a Variable as an index to a std::map, or as part of a
      // std::set.
   bool Variable::operator<(const Variable& right) const
   {

      if( varType == right.getType() )
      {
         if( pVarModel == right.getModel() )
         {

            if( isSourceIndexed == right.getSourceIndexed() )
            {

               if( isSatIndexed == right.getSatIndexed() )
               {

                  if( initialVariance == right.getInitialVariance() )
                  {
                     return ( defaultCoefficient <
                              right.getDefaultCoefficient() );
                  }
                  else
                  {
                     return (initialVariance < right.getInitialVariance());
                  }

               }
               else
               {
                  return (isSatIndexed < right.getSatIndexed());
               }

            }
            else
            {
               return (isSourceIndexed < right.getSourceIndexed());
            }

         }
         else
         {
            return (pVarModel < right.getModel());
         }
      }
      else
      {
         return (varType < right.getType());
      }

   }  // End of 'Variable::operator<'



      // Assignment operator.
   Variable& Variable::operator=(const Variable& right)
   {

         // First check if these Variables are the same
      if ( this == &right ) return (*this);

      setType(right.getType());

      setModel(right.getModel());

      setSourceIndexed(right.getSourceIndexed());

      setSatIndexed(right.getSatIndexed());

      setInitialVariance(right.getInitialVariance());

      setDefaultCoefficient(right.getDefaultCoefficient());

      return *this;

   }  // End of 'Variable::operator='



}  // End of namespace gpstk
