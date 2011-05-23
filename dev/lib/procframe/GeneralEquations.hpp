#pragma ident "$Id$"

/**
 * @file GeneralEquations.hpp
 * 
 */

#ifndef GPSTK_GENERALEQUATIONS_HPP
#define GPSTK_GENERALEQUATIONS_HPP

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
#include <iostream>
#include <iomanip>
#include "EquationSystem.hpp"
#include "DataStructures.hpp"


using namespace std;

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
         vector<SatID>  satellite;
         vector<double> elevation;
         vector<bool>   csflag;
         vector<bool>   lockflag;

         void addData(SatID sat, double eleva=0.0, double cflag=0.0,
                      bool lflag=false)
         {
            satellite.push_back(sat);
            elevation.push_back(eleva);
            csflag.push_back(cflag);
            lockflag.push_back(lflag);
         }

         inline int indexOfSat(const SatID& sat) const;
         inline int indexOfReferenceSat(double minElev = 90.0);
         inline int indexOfReferenceSat(SatID oldSat,double minElev = 35.0);

      };
         /// Handy type definition
      typedef std::map<SourceID,SatData> SourceSatDataMap;

   public:

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


      inline void dumpSourceSatData(ostream& s,SourceSatDataMap dataMap); 


      SatSourceMap getRefSatSourceMap()
      { return refsatSourceMap; }


      SourceSatMap getSourceRefSatMap()
      { return sourceRefsatMap; }


   protected:

      virtual EquationSystem getEquations() = 0;


      virtual ConstraintSystem getConstraints()
      { return ConstraintSystem(); }


      virtual ConstraintSystem getConstraints(gnssRinex& gRin)
      { return ConstraintSystem(); }


      virtual ConstraintSystem getConstraints(gnssDataMap& gdsMap)
      { return ConstraintSystem(); }


         /// Method to do the initialization work
      inline virtual void init();
   

         /// Remark cycle slip flag of the input GDS object
      inline void remarkCycleSlip(gnssRinex& gRin);


         /// Remark cycle slip flag of the input GDS object
      inline void remarkCycleSlip(gnssDataMap& gdsMap);

      
         /// Reset the CS flag due to the reference satellite
      inline void resetCSFlag(const SatSourceMap& satSource,
                              const SourceSatMap& sourceSat,
                              SourceSatDataMap& dataMap);

         /// Synchronize the CS flag of input GDS object with the 
         /// SourceSatDataMap object
      inline void synchronizeCSFlag(const SourceSatDataMap& dataMap,
                                    gnssRinex& gRin);
         

         /// Synchronize the CS flag of input GDS object with the 
         /// SourceSatDataMap object
      inline void synchronizeCSFlag(const SourceSatDataMap& dataMap,
                                    gnssDataMap& gdsMap);


         /// update the satellite data due to the input GDS object
      inline void updateSourceSatDataMap(const gnssDataMap& gdsMap);


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
   };

      // Method to do the initialization work 
   inline void GeneralEquations::init()
   {
      defaultTropModel.setQprime(3.0e-8);
      defaultIonoModel.setSigma(100.0);

      estimatePosition = true;
      estimateTropsphere = true;
      estimateIonosphere = true;

      const double sigmaCoordXYZ = 0.1;
      defaultXCoordinatesModel.setSigma(sigmaCoordXYZ);
      defaultYCoordinatesModel.setSigma(sigmaCoordXYZ);
      defaultZCoordinatesModel.setSigma(sigmaCoordXYZ);

      pCoordXStoModel = &defaultStochasticModel;
      pCoordYStoModel = &defaultStochasticModel;
      pCoordZStoModel = &defaultStochasticModel;

      pClockStoModel    = &defaultWhiteNoiseModel;
      pSatClockStoModel = &defaultWhiteNoiseModel;

      pTropStoModel  = &defaultTropModel;
      pIonoStoModel  = &defaultIonoModel;

      pBiasStoModelL1 = &sm_ambL1;
      pBiasStoModelL2 = &sm_ambL2;

      pBiasStoModelLC = &sm_ambLC;

      pBiasStoModelWL = &sm_ambWL;
      pBiasStoModelWL2 = &sm_ambWL2;

   }  // End of method 'void GeneralEquations::init()'


      // Remark cycle slip flag of the input GDS object
   inline void GeneralEquations::remarkCycleSlip(gnssRinex& gRin)
   {
      SourceSatDataMap dataMap = getSourceSatDataMap();
      resetCSFlag(refsatSourceMap,sourceRefsatMap,dataMap);
      synchronizeCSFlag(dataMap,gRin);

   }  // End of method 'GeneralEquations::remarkCycleSlip()'


   /// Remark cycle slip flag of the input GDS object
   inline void GeneralEquations::remarkCycleSlip(gnssDataMap& gdsMap)
   {
      SourceSatDataMap dataMap = getSourceSatDataMap();
      resetCSFlag(refsatSourceMap,sourceRefsatMap,dataMap);
      synchronizeCSFlag(dataMap,gdsMap);

   }  // End of method 'GeneralEquations::remarkCycleSlip(gnssDataMap& gdsMap)'


      // Reset the CS flag due to the reference satellite
   inline void GeneralEquations::resetCSFlag(const SatSourceMap& satSource,
                                             const SourceSatMap& sourceSat,
                                             SourceSatDataMap& dataMap)
   {
      for(SatSourceMap::const_iterator it = satSource.begin();
         it!=satSource.end();
         ++it)
      {
         SatID sat(it->first);
         SourceID source(it->second);

         int index = dataMap[source].indexOfSat(sat);
         if(index <0)
         {
            Exception e("The satellite not exist in the input GDS");
            GPSTK_THROW(e);
         }

         bool refCS = dataMap[source].csflag[index];

         if(refCS==false) continue;

         for(SourceSatDataMap::iterator its = dataMap.begin();
            its!=dataMap.end();
            ++its)
         {
            int i = its->second.indexOfSat(sat);
            if(i>=0) its->second.csflag[i] = refCS;   // refCS=true
         }
      }

      for(SourceSatMap::const_iterator it = sourceSat.begin();
          it!=sourceSat.end();
          ++it)
      {
         SatID sat(it->second);
         SourceID source(it->first);

         int index = dataMap[source].indexOfSat(sat);
         if(index <0)
         {
            Exception e("The satellite not exist in the input GDS");
            GPSTK_THROW(e);
         }

         bool refCS = dataMap[source].csflag[index];

         if(refCS==false) continue;

         for(int i=0;i<dataMap[source].satellite.size();i++)
         {
            dataMap[source].csflag[i] = refCS;     // refCS=true
         }
      }

   }  // End of method 'GeneralEquations::resetCSFlag()'  


      // Synchronize the CS flag of input GDS object with the 
      // SourceSatDataMap object
   inline void GeneralEquations::synchronizeCSFlag(
                                               const SourceSatDataMap& dataMap,
                                               gnssRinex& gRin )
   {
      SourceID source = gRin.header.source;
      
      SourceSatDataMap::const_iterator it = dataMap.find(source);
      if(it==dataMap.end()) return;
      
      for(int i = 0; i< it->second.satellite.size();i++)
      {
         SatID sat(it->second.satellite[i]);
         double csValue = it->second.csflag[i]?1.0:0.0; 
         
         satTypeValueMap::iterator its = gRin.body.find(sat);
         if(its!=gRin.body.end())
         {
            gRin.body[sat][TypeID::CSL1] = csValue;
            gRin.body[sat][TypeID::CSL2] = csValue;
         }
         
      }  // for(int i = 0...
       
   }  // End of method 'GeneralEquations::synchronizeCSFlag()'


      /// Synchronize the CS flag of input GDS object with the 
      /// SourceSatDataMap object
   inline void GeneralEquations::synchronizeCSFlag(
                                                const SourceSatDataMap& dataMap,
                                                gnssDataMap& gdsMap)
   {
         // Iterate through the gnssDatamap
      for(gnssDataMap::iterator it = gdsMap.begin();
          it != gdsMap.end();
          ++it )
      {
         // Look for current SourceID
         for(sourceDataMap::iterator sdmIter = it->second.begin();
             sdmIter != it->second.end();
             ++sdmIter)
         {
            SourceID source(sdmIter->first);

            // Iterate through corresponding 'satTypeValueMap'
            for(satTypeValueMap::iterator stvmIter = sdmIter->second.begin();
                stvmIter != sdmIter->second.end();
                ++stvmIter )
            {
               SatID sat(stvmIter->first);
               
               SourceSatDataMap::const_iterator its = dataMap.find(source);
               if(its!=dataMap.end())
               {
                  int index = its->second.indexOfSat(sat);
                  if(index>=0)
                  {
                     double csValue = its->second.csflag[index]?1.0:0.0;

                     stvmIter->second[TypeID::CSL1] = csValue;
                     stvmIter->second[TypeID::CSL2] = csValue;
                  }
               }
               
            }  // End of 'for( satTypeValueMap::const_iterator ...'

         }  // End of 'for(sourceDataMap::iterator '

      }  // End of 'for( gnssDataMap::const_iterator it = ...'

   }  // End of method 'GeneralEquations::synchronizeCSFlag()'


      // update the satellite data due to the input GDS object
   inline void GeneralEquations::updateSourceSatDataMap(
                                                     const gnssDataMap& gdsMap)
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

               typeValueMap::const_iterator itt1 = 
                                       stvmIter->second.find(TypeID::elevation);

               typeValueMap::const_iterator itt2 = 
                                            stvmIter->second.find(TypeID::CSL1);

               if( (itt1==stvmIter->second.end()) || 
                   (itt2==stvmIter->second.end())   )
               {
                  Exception e("The elevation should be exist but not.");
                  GPSTK_THROW(e);
               }
               
               data.addData(sat, itt1->second,
                            (itt2->second!=0.0)?true:false, false);
 
            }  // End of 'for( satTypeValueMap::const_iterator ...'

            dataMap[source] = data;
         }

      }  // End of 'for( gnssDataMap::const_iterator it = ...'

      sourceSatDataMap = dataMap;

   }  // End of method 'void GeneralEquations::updateSourceSatDataMap'


      // Find the index of the satellite in the struct
   inline int GeneralEquations::SatData::indexOfSat(const SatID& sat) const
   {
      int index(-1);

      for(int i=0;i<satellite.size();i++)
      {
         if(satellite[i]==sat)
         {
            index = i;
            break;
         }
      }
      return index;

   }  // End of method 'GeneralEquations::SatData::indexOfSat()'


      // Find a satellite as reference sat and return it's index 
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
      Exception e("Failed to pick up any satellite as reference.");
      GPSTK_THROW(e);

      return 0;

   }  // End of method 'GeneralEquations::SatData::indexOfReferenceSat()'


      // Find a satellite as reference sat and return it's index 
   inline int GeneralEquations::SatData::indexOfReferenceSat(SatID  oldSat,
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


   inline void GeneralEquations::dumpSourceSatData(ostream& s,
                                                   SourceSatDataMap dataMap)
   { 
      for(SourceSatDataMap::iterator it=dataMap.begin();
          it!=dataMap.end();
          ++it)
      {
         s << StringUtils::asString(it->first) << endl;

         for(int i=0;i<it->second.satellite.size();i++)
         {
            s << setw(5)<< i << " " 
               << StringUtils::asString(it->second.satellite[i])<<"  "
              << it->second.csflag[i] <<" " 
              << it->second.lockflag[i]<<" "
              << it->second.elevation[i]<<endl;
         }
      }
      
   }  // End of method 'GeneralEquations::dumpSourceSatData()'


}  // End of namespace gpstk


#endif // GPSTK_GENERALEQUATIONS_HPP

