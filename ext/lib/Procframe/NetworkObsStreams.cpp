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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
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
 * @file NetworkObsStreams.cpp
 * This class synchronizes rinex observation data streams.
 */

#include "NetworkObsStreams.hpp"
#include "RinexObsHeader.hpp"


namespace gpstk
{
      // Add a rinex obs file to the network
      // @obsFile Rinex observation file name
   bool NetworkObsStreams::addRinexObsFile(const std::string& obsFile)
   {
         // object to hold the data
      ObsData oData;
      oData.obsFile = obsFile;

      try
      {
            // allocate memory
         oData.pObsStream = new RinexObsStream();
         oData.pSynchro = new Synchronize();
         if(!oData.pObsStream || !oData.pSynchro)
         {
            if(oData.pObsStream) delete oData.pObsStream;
            if(oData.pSynchro) delete oData.pSynchro;

            return false;
         }
            // Open the file
         oData.pObsStream->exceptions(std::ios::failbit);
         oData.pObsStream->open(oData.obsFile, std::ios::in);

            // We reader the header of the obs file
         RinexObsHeader obsHeader;
         (*oData.pObsStream) >> obsHeader;


         oData.obsSource.type = SatIDsystem2SourceIDtype(obsHeader.system);
         oData.obsSource.sourceName = obsHeader.markerName;

         oData.pSynchro->setReferenceSource(*oData.pObsStream);

            // Now, we should store the data for the receiver
         allStreamData.push_back(oData);

         mapSourceStream[oData.obsSource] = oData.pObsStream;
         mapSourceSynchro[oData.obsSource] = oData.pSynchro;

         referenceSource = oData.obsSource;

         return true;
      }
      catch(...)
      {
         // Problem opening the file
         // Maybe it doesn't exist or you don't have proper read permissions

         // We have to deallocate the memory here
         delete oData.pObsStream;
         oData.pObsStream = (RinexObsStream*)0;

         return false;
      }

   }  // End of method 'NetworkObsStreams::addRinexObsFile()'

      // Get epoch data of the network
      // @gdsMap  Object hold epoch observation data of the network
      // @return  Is there more epoch data for the network 
   bool NetworkObsStreams::readEpochData(gnssDataMap& gdsMap)
      throw(SynchronizeException)
   {
      // First, We clear the data map
      gdsMap.clear();


      RinexObsStream* pRefObsStream = mapSourceStream[referenceSource];

      gnssRinex gRef;
  
      if( (*pRefObsStream) >> gRef )
      {
         gdsMap.addGnssRinex(gRef);

         std::map<SourceID, RinexObsStream*>::iterator it;
         for( it = mapSourceStream.begin();
              it != mapSourceStream.end();
            ++it)
         {
            if( it->first == referenceSource) continue;

            Synchronize* synchro = mapSourceSynchro[it->first];
            synchro->setRoverData(gRef);

            gnssRinex gRin;

            try
            {
               gRin >> (*synchro);
               gdsMap.addGnssRinex(gRin);
            }
            catch(...)
            {
               if(synchronizeException)
               {
                  std::stringstream ss;
                  ss << "Exception when try to synchronize at epoch: "
                     << gRef.header.epoch << std::endl;

                  SynchronizeException e(ss.str());

                  GPSTK_THROW(e);
               }
            }

         }  // End of 'for(std::map<SourceID, RinexObsStream*>::iterator it;
         
         return true;

      }  // End of 'if( (*pRefObsStream) >> gRef )'


      return false;

   }  // End of method 'NetworkObsStreams::readEpochData()'

      // do some clean operation 
   void NetworkObsStreams::cleanUp()
   {
      mapSourceStream.clear();

      std::list<ObsData>::iterator it;
      for( it = allStreamData.begin();
           it != allStreamData.end();
         ++it)
      {
         if(it->pObsStream)
         {
            it->pObsStream->close();
            delete it->pObsStream;
            it->pObsStream = (RinexObsStream*)0;
         }
         
         if(it->pSynchro)
         {
            delete it->pSynchro;
            it->pSynchro = (Synchronize*)0;
         }
      }

      allStreamData.clear();

   }  // End of method 'NetworkObsStreams::cleanUp()'

      // Get the SourceID of the rinex observation file
   SourceID NetworkObsStreams::sourceIDOfRinexObsFile(std::string obsFile)
   {
      try
      {
         RinexObsStream rin;
         rin.exceptions(std::ios::failbit);
         rin.open(obsFile, std::ios::in);
 
         gnssRinex gRin;
         rin >> gRin;

         rin.close();
         
         return gRin.header.source;
      }
      catch(...)
      {
         // Problem opening the file
         // Maybe it doesn't exist or you don't have proper read permissions

         Exception e("Problem opening the file "
            + obsFile 
            + "Maybe it doesn't exist or you don't have proper read permissions");

         GPSTK_THROW(e);
      }

   }  // End of method 'NetworkObsStreams::sourceIDOfRinexObsFile'

}  // End of namespace gpstk
