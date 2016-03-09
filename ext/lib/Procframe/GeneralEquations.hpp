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
//  Wei Yan - Chinese Academy of Sciences . 2011
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
 * @file GeneralEquations.hpp
 * 
 */

#ifndef GPSTK_GENERALEQUATIONS_HPP
#define GPSTK_GENERALEQUATIONS_HPP

#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include "ConstraintSystem.hpp"
#include "EquationSystem.hpp"
#include "DataStructures.hpp"

namespace gpstk
{



      /// Handy type definition
   typedef std::map<SatID,SourceID> SatSourceMap;

   typedef std::map<SourceID,SatID> SourceSatMap;


      /** This class ...
       *
       */
   class GeneralEquations
   {
   public:

         /// struct holding satellite related data
      struct SatData
      {
         std::vector<SatID>  satellite;
         std::vector<double> elevation;
         std::vector<bool>   csflag;
         std::vector<bool>   lockflag;

         void addData(SatID sat, double eleva=0.0, double cflag=0.0,
                      bool lflag=false)
         {
            satellite.push_back(sat);
            elevation.push_back(eleva);
            csflag.push_back(cflag);
            lockflag.push_back(lflag);
         }

         int indexOfSat(const SatID& sat) const;
         int indexOfReferenceSat(double minElev = 90.0);
         int indexOfReferenceSat( SatID oldSat,
                                  double minElev = 35.0 );

      };

         /// Handy type definition
      typedef std::map<SourceID,SatData> SourceSatDataMap;


         /// Default constructor
      GeneralEquations() 
      { init(); }


         /// Default destructor
      ~GeneralEquations() {}


         /// Get the EuqationSystem object
      EquationSystem getEquationSystem()
      { return getEquations(); }
         

         /// Get the ConstraintSystem object
      ConstraintSystem getConstraintSystem()
      { return getConstraints(); }


         /// Get the ConstraintSystem object
      ConstraintSystem getConstraintSystem(gnssRinex& gRin)
      {
         gnssDataMap gdsMap; gdsMap.addGnssRinex(gRin);      
         updateSourceSatDataMap(gdsMap);
         ConstraintSystem toReturn = getConstraints(gRin);
         remarkCycleSlip(gRin);

         return toReturn;
      }


         /// Get the ConstraintSystem object
      ConstraintSystem getConstraintSystem(gnssDataMap& gdsMap)
      { 
         updateSourceSatDataMap(gdsMap); 
         ConstraintSystem toReturn = getConstraints(gdsMap);
         remarkCycleSlip(gdsMap);

         return toReturn;
      }

         // public methods

      GeneralEquations& setCoordinatesStatic()
      { 
         pCoordXStoModel = &defaultStochasticModel;
         pCoordYStoModel = &defaultStochasticModel;
         pCoordZStoModel = &defaultStochasticModel; 

         return (*this); 
      };

      GeneralEquations& setCoordinatesKinematic(double sigmaX,
                                                double sigmaY,
                                                double sigmaZ)
      { 
         defaultXCoordinatesModel.setSigma(sigmaX);
         defaultYCoordinatesModel.setSigma(sigmaY);
         defaultZCoordinatesModel.setSigma(sigmaZ);

         pCoordXStoModel = &defaultXCoordinatesModel;
         pCoordYStoModel = &defaultYCoordinatesModel;
         pCoordZStoModel = &defaultZCoordinatesModel; 

         return (*this); 
      };


      StochasticModel* getXCoordinatesModel() const
      { return pCoordXStoModel; };


      GeneralEquations& setXCoordinatesModel(StochasticModel* pModel)
      { pCoordXStoModel = pModel; return (*this); };


      StochasticModel* getYCoordinatesModel() const
      { return pCoordYStoModel; };


      GeneralEquations& setYCoordinatesModel(StochasticModel* pModel)
      { pCoordYStoModel = pModel; return (*this); };


      StochasticModel* getZCoordinatesModel() const
      { return pCoordZStoModel; };


      GeneralEquations& setZCoordinatesModel(StochasticModel* pModel)
      { pCoordZStoModel = pModel; return (*this); };


      GeneralEquations& setCoordinatesModel(StochasticModel* pModel);


      StochasticModel* getTroposphereModel(void) const
      { return pTropStoModel; };


      GeneralEquations& setTroposphereModel(StochasticModel* pModel)
      { pTropStoModel = pModel; return (*this); };


      virtual StochasticModel* getIonosphereModel(void) const
      { return pIonoStoModel; };


      virtual GeneralEquations& setIonosphereModel(StochasticModel* pModel)
      { pIonoStoModel = pModel; return (*this); };


      virtual StochasticModel* getReceiverClockModel(void) const
      { return pClockStoModel; };


      virtual GeneralEquations& setReceiverClockModel(StochasticModel* pModel)
      { pClockStoModel = pModel; return (*this); };


      virtual StochasticModel* getSatClockModel(void) const
      { return pSatClockStoModel; };


      virtual GeneralEquations& setSatClockModel(StochasticModel* pModel)
      { pSatClockStoModel = pModel; return (*this); };


         //
      virtual bool getEstimatePosition(void) const
      { return estimatePosition; };


      virtual GeneralEquations& setEstimatePosition(bool flag = true)
      { estimatePosition = flag; return (*this); };


      virtual bool getEstimateTropsphere(void) const
      { return estimateTropsphere; };


      virtual GeneralEquations& setEstimateTropsphere(bool flag = true)
      { estimateTropsphere = flag; return (*this); };


      virtual bool getEstimateIonosphere(void) const
      { return estimateIonosphere; };


      virtual GeneralEquations& setEstimateIonosphere(bool flag = true)
      { estimateIonosphere = flag; return (*this); };


      SourceSatDataMap getSourceSatDataMap()
      { return sourceSatDataMap; }


      void dumpSourceSatData( std::ostream& s,
                              SourceSatDataMap dataMap );


      SatSourceMap getRefSatSourceMap()
      { return refsatSourceMap; }


      SourceSatMap getSourceRefSatMap()
      { return sourceRefsatMap; }


      inline EquationSystem getPPPEquations(const SourceID& source);

   protected:


      virtual EquationSystem getEquations() = 0;


      virtual ConstraintSystem getConstraints()
      { return ConstraintSystem(); }


      virtual ConstraintSystem getConstraints(gnssRinex& gRin)
      { return ConstraintSystem(); }


      virtual ConstraintSystem getConstraints(gnssDataMap& gdsMap)
      { return ConstraintSystem(); }


         /// Method to do the initialization work
      virtual void init();
   

         /// Remark cycle slip flag of the input GDS object
      void remarkCycleSlip(gnssRinex& gRin);


         /// Remark cycle slip flag of the input GDS object
      void remarkCycleSlip(gnssDataMap& gdsMap);

      
         /// Reset the CS flag due to the reference satellite
      void resetCSFlag( const SatSourceMap& satSource,
                        const SourceSatMap& sourceSat,
                        SourceSatDataMap& dataMap );

         /// Synchronize the CS flag of input GDS object with the 
         /// SourceSatDataMap object
      void synchronizeCSFlag( const SourceSatDataMap& dataMap,
                              gnssRinex& gRin );
         

         /// Synchronize the CS flag of input GDS object with the 
         /// SourceSatDataMap object
      void synchronizeCSFlag( const SourceSatDataMap& dataMap,
                              gnssDataMap& gdsMap );


         /// update the satellite data due to the input GDS object
      void updateSourceSatDataMap(const gnssDataMap& gdsMap);


      bool estimatePosition;
      bool estimateTropsphere;
      bool estimateIonosphere;

         /// Pointer to stochastic model for dx dy dz (or dLat dLon dH)
      StochasticModel* pCoordXStoModel;
      StochasticModel* pCoordYStoModel;
      StochasticModel* pCoordZStoModel;

         /// Pointer to stochastic model for receiver clock
      StochasticModel* pClockStoModel;

         /// Pointer to stochastic model for satellite clock
      StochasticModel* pSatClockStoModel;

         /// Pointer to stochastic model for troposphere
      StochasticModel* pTropStoModel;

         /// Pointer to stochastic model for troposphere
      StochasticModel* pIonoStoModel;

         /// Pointer to stochastic model for phase biases
      StochasticModel* pBiasStoModelL1;

         /// Pointer to stochastic model for phase biases
      StochasticModel* pBiasStoModelL2;

         /// Pointer to stochastic model for phase biases LC
      StochasticModel* pBiasStoModelLC;

         /// Pointer to stochastic model for phase biases WL
      StochasticModel* pBiasStoModelWL;

         /// Pointer to stochastic model for phase biases WL2
      StochasticModel* pBiasStoModelWL2;

         /// Pointer to stochastic model for phase biases WL3
      StochasticModel* pBiasStoModelWL3;

         /// Object holding the useful satellite data for pick up reference sat.
      SourceSatDataMap sourceSatDataMap;
      
         /// Object holding the map of reference satellite to source. 
      SatSourceMap refsatSourceMap;

         /// Object holding the map of source to reference satellite. 
      SourceSatMap sourceRefsatMap;


   private:


         /// default stochastic model
      StochasticModel defaultStochasticModel;
      WhiteNoiseModel defaultWhiteNoiseModel;
      RandomWalkModel defaultTropModel;
      WhiteNoiseModel defaultIonoModel;
      PhaseAmbiguityModel defaultPhaseAmbiguityModel;

      WhiteNoiseModel defaultXCoordinatesModel;
      WhiteNoiseModel defaultYCoordinatesModel;
      WhiteNoiseModel defaultZCoordinatesModel;
     
      PhaseAmbiguityModel sm_ambL1;
      PhaseAmbiguityModel sm_ambL2;
      PhaseAmbiguityModel sm_ambLC;
      PhaseAmbiguityModel sm_ambWL;
      PhaseAmbiguityModel sm_ambWL2;
      PhaseAmbiguityModel sm_ambWL3;

   }; // End of class 'GeneralEquations'


   EquationSystem GeneralEquations::getPPPEquations(const SourceID& source)
   {
      Variable dx(TypeID::dLat, pCoordXStoModel, true, false, 100.0);
      Variable dy(TypeID::dLon, pCoordYStoModel, true, false, 100.0);
      Variable dz(TypeID::dH, pCoordZStoModel, true, false, 100.0);

      Variable cdt(TypeID::cdt,pClockStoModel,true,false,4e14,+1.0,true);
      Variable trop(TypeID::wetMap,pTropStoModel,true,false,0.25);
      Variable amb(TypeID::BLC,pBiasStoModelLC,true,true,4e14,1.0,true);

         //((PhaseAmbiguityModel*)pBiasStoModelLC)->setWatchSatArc(false);

      Variable prefitPC(TypeID::prefitC);
      Variable prefitLC(TypeID::prefitL);

      Equation equPCRover( prefitPC );
      equPCRover.addVariable(dx);
      equPCRover.addVariable(dy);
      equPCRover.addVariable(dz);
      equPCRover.addVariable(cdt);
      equPCRover.addVariable(trop);

      equPCRover.header.equationSource = source;

         // Rover phase equation description
      Equation equLCRover( prefitLC );
      equLCRover.addVariable(dx);
      equLCRover.addVariable(dy);
      equLCRover.addVariable(dz);
      equLCRover.addVariable(cdt);
      equLCRover.addVariable(trop);
      equLCRover.addVariable(amb);

      equLCRover.setWeight(10000.0);     // 100.0 * 100.0
      equLCRover.header.equationSource = source;

         // Setup equation system
      EquationSystem system;
      system.addEquation(equPCRover);
      system.addEquation(equLCRover);

      return system;

   }  // GeneralEquations::getPPPEquations()

}  // End of namespace gpstk

#endif   // GPSTK_GENERALEQUATIONS_HPP
