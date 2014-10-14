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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009
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
 * @file Variable.cpp
 * Class to define and handle 'descriptions' of GNSS variables.
 */

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
       * @param forceCoef        Always use default coefficient.
       */
   Variable::Variable( const TypeID& type,
                       StochasticModel* pModel,
                       bool sourceIndexed,
                       bool satIndexed,
                       double variance,
                       double coef,
                       bool forceCoef )
   {

         // Call Init method
      Init( type,
            pModel,
            variance,
            coef,
            forceCoef );

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
       * @param forceCoef   Always use default coefficient.
       */
   void Variable::Init( const TypeID& type,
                        StochasticModel* pModel,
                        double variance,
                        double coef,
                        bool forceCoef, 
                        bool typeIndex)
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

      forceDefault = forceCoef;

      isTypeIndexed = typeIndex;    // default is true, this is important YAN Wei added

     
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
               ( defaultCoefficient == right.getDefaultCoefficient() )  &&
               ( forceDefault == right.isDefaultForced() )              &&
               ( varSource == right.getSource() )                       &&
               ( varSat == right.getSatellite() )                       &&
               ( isTypeIndexed == right.getTypeIndexed() )
               );            

   }  // End of 'Variable::operator=='



      // This ordering is somewhat arbitrary, but is required to be able
      // to use a Variable as an index to a std::map, or as part of a
      // std::set.
   bool Variable::operator<(const Variable& right) const
   {

         // Compare each field in turn
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

                     if ( defaultCoefficient == right.getDefaultCoefficient() )
                     {

                        if ( forceDefault == right.isDefaultForced() )
                        {

                           if ( varSource == right.getSource() )
                           {

                               if( varSat == right.getSatellite())
                               {
                                  return ( isTypeIndexed < right.getTypeIndexed() );
                               }
                               else
                               {
                                   return ( varSat < right.getSatellite() );
                               }

                           }
                           else
                           {
                              return ( varSource < right.getSource() );
                           }

                        }
                        else
                        {
                           return ( forceDefault < right.isDefaultForced() );
                        }

                     }
                     else
                     {
                        return ( defaultCoefficient <
                                 right.getDefaultCoefficient() );
                     }

                  }
                  else
                  {
                     return ( initialVariance < right.getInitialVariance() );
                  }

               }
               else
               {
                  return ( isSatIndexed < right.getSatIndexed() );
               }

            }
            else
            {
               return ( isSourceIndexed < right.getSourceIndexed() );
            }

         }
         else
         {
            return ( pVarModel < right.getModel() );
         }
      }
      else
      {
         return ( varType < right.getType() );
      }

   }  // End of 'Variable::operator<'



      // Assignment operator.
   Variable& Variable::operator=(const Variable& right)
   {

         // First check if these Variables are the same
      if ( this == &right ) return (*this);

         // If Variables are different, then set values of all fields
      setType( right.getType() );

      setModel( right.getModel() );

      setSourceIndexed( right.getSourceIndexed() );

      setSatIndexed( right.getSatIndexed() );

      setInitialVariance( right.getInitialVariance() );

      setDefaultCoefficient( right.getDefaultCoefficient() );

      setDefaultForced( right.isDefaultForced() );

      setSource( right.getSource() );

      setSatellite( right.getSatellite() );

      setTypeIndexed(right.getTypeIndexed());              


      return *this;

   }  // End of 'Variable::operator='



}  // End of namespace gpstk
