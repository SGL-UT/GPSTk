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
 * @file GeneralEquations.cpp
 * 
 */

#include "GeneralEquations.hpp"


using namespace std;

namespace gpstk
{


      // Method to do the initialization work
   void GeneralEquations::init()
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
   void GeneralEquations::remarkCycleSlip(gnssRinex& gRin)
   {

      SourceSatDataMap dataMap = getSourceSatDataMap();
      resetCSFlag(refsatSourceMap,sourceRefsatMap,dataMap);
      synchronizeCSFlag(dataMap,gRin);

   }  // End of method 'GeneralEquations::remarkCycleSlip()'


      // Remark cycle slip flag of the input GDS object
   void GeneralEquations::remarkCycleSlip(gnssDataMap& gdsMap)
   {

      SourceSatDataMap dataMap = getSourceSatDataMap();
      resetCSFlag(refsatSourceMap,sourceRefsatMap,dataMap);
      synchronizeCSFlag(dataMap,gdsMap);

   }  // End of method 'GeneralEquations::remarkCycleSlip(gnssDataMap& gdsMap)'


      // Reset the CS flag due to the reference satellite
   void GeneralEquations::resetCSFlag( const SatSourceMap& satSource,
                                       const SourceSatMap& sourceSat,
                                       SourceSatDataMap& dataMap )
   {

      for( SatSourceMap::const_iterator it = satSource.begin();
           it!=satSource.end();
           ++it )
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
         
      }  // End of 'for(SatSourceMap::const_iterator it = satSource.begin();...'

      for( SourceSatMap::const_iterator it = sourceSat.begin();
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

         for(size_t i=0;i<dataMap[source].satellite.size();i++)
         {
            dataMap[source].csflag[i] = refCS;     // refCS=true
         }

      }  // End of 'for( SourceSatMap::const_iterator it = sourceSat.begin();'

   }  // End of method 'GeneralEquations::resetCSFlag()'  


      // Synchronize the CS flag of input GDS object with the 
      // SourceSatDataMap object
   void GeneralEquations::synchronizeCSFlag( const SourceSatDataMap& dataMap,
                                             gnssRinex& gRin )
   {

      SourceID source = gRin.header.source;
      
      SourceSatDataMap::const_iterator it = dataMap.find(source);
      if(it==dataMap.end()) return;
      
      for(size_t i = 0; i< it->second.satellite.size();i++)
      {

         SatID sat(it->second.satellite[i]);
         double csValue = it->second.csflag[i]?1.0:0.0; 
         
         satTypeValueMap::iterator its = gRin.body.find(sat);
         if(its!=gRin.body.end())
         {
            gRin.body[sat][TypeID::CSL1] = csValue;
            gRin.body[sat][TypeID::CSL2] = csValue;
         }
         
      }  // End of 'for(int i = 0...'
       
   }  // End of method 'GeneralEquations::synchronizeCSFlag()'


      // Synchronize the CS flag of input GDS object with the
      // SourceSatDataMap object
   void GeneralEquations::synchronizeCSFlag( const SourceSatDataMap& dataMap,
                                             gnssDataMap& gdsMap )
   {

         // Iterate through the gnssDatamap
      for( gnssDataMap::iterator it = gdsMap.begin();
           it != gdsMap.end();
           ++it )
      {

            // Look for current SourceID
         for( sourceDataMap::iterator sdmIter = it->second.begin();
              sdmIter != it->second.end();
              ++sdmIter)
         {

            SourceID source(sdmIter->first);

               // Iterate through corresponding 'satTypeValueMap'
            for( satTypeValueMap::iterator stvmIter = sdmIter->second.begin();
                 stvmIter != sdmIter->second.end();
                 ++stvmIter )
            {

               SatID sat(stvmIter->first);
               
               SourceSatDataMap::const_iterator its = dataMap.find(source);
               if( its!=dataMap.end() )
               {

                  int index = its->second.indexOfSat(sat);
                  if( index>=0 )
                  {

                     double csValue = its->second.csflag[index]?1.0:0.0;

                     stvmIter->second[TypeID::CSL1] = csValue;
                     stvmIter->second[TypeID::CSL2] = csValue;

                  }  // End of 'if( index>=0 )'

               }  // End of 'if( its!=dataMap.end() )'
               
            }  // End of 'for( satTypeValueMap::const_iterator ...'

         }  // End of 'for(sourceDataMap::iterator '

      }  // End of 'for( gnssDataMap::const_iterator it = ...'

   }  // End of method 'GeneralEquations::synchronizeCSFlag()'


      // update the satellite data due to the input GDS object
   void GeneralEquations::updateSourceSatDataMap( const gnssDataMap& gdsMap )
   {

      SourceSatDataMap dataMap;

         // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = gdsMap.begin();
           it != gdsMap.end();
           ++it )
      {

            // Look for current SourceID
         sourceDataMap::const_iterator sdmIter; //(it->second.find(source));
         for( sdmIter=it->second.begin();
              sdmIter!=it->second.end();
              ++sdmIter )
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
                  Exception e("Elevation was not found.");
                  GPSTK_THROW(e);
               }
               
               data.addData(sat, itt1->second,
                            (itt2->second!=0.0)?true:false, false);
 
            }  // End of 'for( satTypeValueMap::const_iterator ...'

            dataMap[source] = data;

         }  // End of 'for( sdmIter=it->second.begin();...'

      }  // End of 'for( gnssDataMap::const_iterator it = ...'

      sourceSatDataMap = dataMap;

   }  // End of method 'void GeneralEquations::updateSourceSatDataMap'


      // Find the index of the satellite in the struct
   int GeneralEquations::SatData::indexOfSat(const SatID& sat) const
   {

      int index(-1);

      for(size_t i=0;i<satellite.size();i++)
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
   int GeneralEquations::SatData::indexOfReferenceSat(double minElev)
   {

      map<SatID,int> satCS,satNoCS;
      for(size_t i=0;i<satellite.size();i++)
      {
         if(csflag[i]==true) satCS[satellite[i]]=i;
         else                satNoCS[satellite[i]]=i;
      }

         // First we check satellite no cs
      int indexMaxElev(-1);
      double maxElev(-90);
      for( map<SatID,int>::iterator it = satNoCS.begin();
           it != satNoCS.end();
           ++it )
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
           ++it )
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
   int GeneralEquations::SatData::indexOfReferenceSat( SatID oldSat,
                                                       double minElev )
   {

      int index(-1);
      for(size_t i=0;i<satellite.size();i++)
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


   void GeneralEquations::dumpSourceSatData( ostream& s,
                                             SourceSatDataMap dataMap )
   { 

      for( SourceSatDataMap::iterator it=dataMap.begin();
           it!=dataMap.end();
           ++it )
      {

         s << StringUtils::asString(it->first) << endl;

         for(size_t i=0;i<it->second.satellite.size();i++)
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
