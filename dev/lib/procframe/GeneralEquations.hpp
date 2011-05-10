#pragma ident "$Id$"

/**
 * @file GeneralEquations.hpp
 * 
 */

#ifndef GNSSTK_GENERALEQUATIONS_HPP
#define GNSSTK_GENERALEQUATIONS_HPP

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
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================

#include <vector>
#include <map>
#include "EquationSystem.hpp"
#include "DataStructures.hpp"

namespace gpstk
{
      /** This class ...
       *
       */
   class GeneralEquations
   {
   public:
         /// struct holding satellite related data
      struct SatData
      {
         vector<SatID>  satellite;
         vector<double> elevation;
         vector<bool>   csflag;
         vector<bool>   lockflag;

         void addData(SatID sat,double eleva=0.0,double cflag=0.0, bool lflag=false)
         {
            satellite.push_back(sat);
            elevation.push_back(eleva);
            csflag.push_back(cflag);
            lockflag.push_back(lflag);
         }

         inline int indexOfReferenceSat(double minElev = 90.0);
         inline int indexOfReferenceSat(SatID oldSat,double minElev = 35.0);
      };
         /// Handy type definition
      typedef map<SourceID,SatData> SourceSatDataMap;

   public:

         /// Default constructor
      GeneralEquations() { init(); }

         /// Default deconstructor
      ~GeneralEquations() {}

      EquationSystem getEquationSystem()
      { return getEquations(); }

      ConstraintSystem getConstraintSystem()
      { return getConstraints(); }

      ConstraintSystem getConstraintSystem(const gnssRinex& gRin)
      {
         gnssDataMap gdsMap; gdsMap.addGnssRinex(gRin);
         updateSourceSatDataMap(gdsMap);

         return getConstraints(gRin);
      }

      ConstraintSystem getConstraintSystem(const gnssDataMap& gdsMap)
      { updateSourceSatDataMap(gdsMap); return getConstraints(gdsMap);}


      // virtual functions

      virtual EquationSystem getEquations() = 0;


      virtual ConstraintSystem getConstraints()
      { return ConstraintSystem(); }


      virtual ConstraintSystem getConstraints(const gnssRinex& gRin)
      { return ConstraintSystem(); }


      virtual ConstraintSystem getConstraints(const gnssDataMap& gdsMap)
      { return ConstraintSystem(); }


      virtual void remarkCycleSlip()
      { }


      virtual void remarkCycleSlip(const gnssRinex& gRin)
      {  }


      virtual void remarkCycleSlip(const gnssDataMap& gdsMap)
      {  }


      GeneralEquations& setCoordinatesStatic()
      { 
         pCoordXStoModel = &sm_dx;
         pCoordYStoModel = &sm_dy;
         pCoordZStoModel = &sm_dz; 

         return (*this); 
      };

      GeneralEquations& setCoordinatesKinematic(double sigmaX,double sigmaY,double sigmaZ)
      { 
         sm_dx_wn.setSigma(sigmaX);
         sm_dy_wn.setSigma(sigmaY);
         sm_dz_wn.setSigma(sigmaZ);

         pCoordXStoModel = &sm_dx_wn;
         pCoordYStoModel = &sm_dy_wn;
         pCoordZStoModel = &sm_dz_wn; 

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
      { return pTropoStoModel; };


      GeneralEquations& setTroposphereModel(StochasticModel* pModel)
      { pTropoStoModel = pModel; return (*this); };


      virtual StochasticModel* getIonosphereModel(void) const
      { return pIonoStoModel; };


      virtual GeneralEquations& setIonosphereModel(StochasticModel* pModel)
      { pIonoStoModel = pModel; return (*this); };


      virtual StochasticModel* getReceiverClockModel(void) const
      { return pClockStoModel; };


      virtual GeneralEquations& setReceiverClockModel(StochasticModel* pModel)
      { pClockStoModel = pModel; return (*this); };


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

   protected:

      inline virtual void init();

      inline void updateSourceSatDataMap(const gnssDataMap& gdsMap);

      bool estimatePosition;
      bool estimateTropsphere;
      bool estimateIonosphere;

      /// Pointer to stochastic model for dx dy dz (or dLat dLon dH) coordinate
      StochasticModel* pCoordXStoModel;
      StochasticModel* pCoordYStoModel;
      StochasticModel* pCoordZStoModel;

      /// Pointer to stochastic model for receiver clock
      StochasticModel* pClockStoModel;

      /// Pointer to stochastic model for troposphere
      StochasticModel* pTropoStoModel;

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
 
      SourceSatDataMap sourceSatDataMap;
      
    
   private:

      // default stochastic model
      StochasticModel defaultStochasticModel;
      WhiteNoiseModel defaultWhiteNoiseModel;
      PhaseAmbiguityModel defaultPhaseAmbiguityModel;

      StochasticModel sm_dx;
      StochasticModel sm_dy;
      StochasticModel sm_dz;

      WhiteNoiseModel sm_dx_wn;
      WhiteNoiseModel sm_dy_wn;
      WhiteNoiseModel sm_dz_wn;
      
      WhiteNoiseModel sm_cdt;
      RandomWalkModel sm_trop;

      WhiteNoiseModel sm_iono_wn;

      PhaseAmbiguityModel sm_ambL1;
      PhaseAmbiguityModel sm_ambL2;
      PhaseAmbiguityModel sm_ambLC;
      PhaseAmbiguityModel sm_ambWL;
      PhaseAmbiguityModel sm_ambWL2;
   };

   // Method to do the initialization work 
   inline void GeneralEquations::init()
   {
      estimatePosition = true;
      estimateTropsphere = true;
      estimateIonosphere = true;

      const double sigmaCoordXYZ = 0.1;

      sm_dx_wn.setSigma(sigmaCoordXYZ);
      sm_dy_wn.setSigma(sigmaCoordXYZ);
      sm_dz_wn.setSigma(sigmaCoordXYZ);

      sm_trop.setQprime(3.0e-8);
      sm_iono_wn.setSigma(150.0);

      pCoordXStoModel = &sm_dx;
      pCoordYStoModel = &sm_dy;
      pCoordZStoModel = &sm_dz;

      pClockStoModel = &sm_cdt;

      pTropoStoModel = &sm_trop;
      pIonoStoModel = &sm_iono_wn;

      pBiasStoModelL1 = &sm_ambL1;
      pBiasStoModelL2 = &sm_ambL2;

      pBiasStoModelLC = &sm_ambLC;

      pBiasStoModelWL = &sm_ambWL;
      pBiasStoModelWL2 = &sm_ambWL2;

   }  // End of method 'void GeneralEquations::init()'

   inline void GeneralEquations::updateSourceSatDataMap(const gnssDataMap& gdsMap)
   {
      SourceSatDataMap dataMap;

      // Iterate through all items in the gnssDataMap
      for(gnssDataMap::const_iterator it = gdsMap.begin();
         it != gdsMap.end();
         ++it )
      {
         // Look for current SourceID
         sourceDataMap::const_iterator sdmIter; //(it->second.find(source));
         for(sdmIter=it->second.begin();
             sdmIter!=it->second.end();
             ++sdmIter)
         {
            SourceID source(sdmIter->first);
            SatData data;

            // Iterate through corresponding 'satTypeValueMap'
            satTypeValueMap::const_iterator stvmIter;
            for( stvmIter = sdmIter->second.begin();
                 stvmIter != sdmIter->second.end();
                 ++stvmIter )
            {
               SatID sat(stvmIter->first);

               typeValueMap::const_iterator itt1 = stvmIter->second.find(TypeID::elevation);
               typeValueMap::const_iterator itt2 = stvmIter->second.find(TypeID::CSL1);
               if( (itt1==stvmIter->second.end()) || 
                   (itt2==stvmIter->second.end())   )
               {
                  Exception e("The elevation should be exist but not, check it!");
                  GPSTK_THROW(e);
               }
               
               data.addData(sat,itt1->second,(itt2->second!=0.0)?true:false,false);
 
            }  // End of 'for( satTypeValueMap::const_iterator ...'

            dataMap[source] = data;
         }

      }  // End of 'for( gnssDataMap::const_iterator it = ...'

      sourceSatDataMap = dataMap;

   }  // End of method 'void GeneralEquations::updateSourceSatDataMap'


   inline int GeneralEquations::SatData::indexOfReferenceSat(double minElev)
   {
      map<SatID,int> satCS,satNoCS;
      for(int i=0;i<satellite.size();i++)
      {
         if(csflag[i]==true) satCS[satellite[i]]=i;
         else                satNoCS[satellite[i]]=i;
      }

      int index(-1);

         // First we check satellite no cs
      int indexMaxElev(-1);
      double maxElev(-90);
      for( map<SatID,int>::iterator it = satNoCS.begin();
         it != satNoCS.end();
         ++it)
      {
         if(lockflag[it->second]==true) continue;

         if( (elevation[it->second]>=minElev)) return it->second;

         if(elevation[it->second]>maxElev)
         {
            maxElev = elevation[it->second];
            indexMaxElev = it->second;
         }
      }

      if(indexMaxElev>=0) return indexMaxElev;

         // Second we check satellite cs
      indexMaxElev = -1;
      maxElev = -90;
      for( map<SatID,int>::iterator it = satCS.begin();
         it != satCS.end();
         ++it)
      {
         if(lockflag[it->second]==true) continue;

         if( (elevation[it->second]>=minElev)) return it->second;

         if(elevation[it->second]>maxElev)
         {
            maxElev = elevation[it->second];
            indexMaxElev = it->second;
         }
      }

      if(indexMaxElev>=0) return indexMaxElev;

         // If code come here, that meant we failed to pick up any satellite 
         // as reference, and we throw an exception
      Exception e("Failed to pick up any satellite as reference, check it please.");
      GPSTK_THROW(e);

      return 0;

   }  // End of method 'GeneralEquations::SatData::indexOfReferenceSat()'


   inline int GeneralEquations::SatData::indexOfReferenceSat(SatID oldSat,
                                                             double minElev)
   {
      int index(-1);
      for(int i=0;i<satellite.size();i++)
      {
         if(satellite[i]==oldSat)
         {
            index = i;
            break;
         }
      }

      if(index>-1)
      {
         if( elevation[index]>=minElev && 
             csflag[index]==false      &&  lockflag[index]==false)
         {
            return index;
         } 
      }
      
      return indexOfReferenceSat(90.0);

   }  // End of method 'GeneralEquations::SatData::indexOfReferenceSat()''


}  // End of namespace gpstk


#endif // GNSSTK_GENERALEQUATIONS_HPP

