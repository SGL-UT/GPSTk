#pragma ident "$Id: $"

/**
 * @file NetworkObsStreams.cpp
 * This class synchronizes rinex observation data streams.
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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================


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
         if(!oData.pObsStream)
         {
            // Failed to allocate memory
            return false;
         }
            // Open the file
         oData.pObsStream->exceptions(ios::failbit);
         oData.pObsStream->open(oData.obsFile, std::ios::in);

            // We reader the header of the obs file
         RinexObsHeader obsHeader;
         (*oData.pObsStream) >> obsHeader;

            // Try to get the SourceID of the receiver 
         gnssRinex gRin;
         obsHeader >> gRin;

         oData.obsSource = gRin.header.source;

            // Now, we should store the data for the receiver
         allStreamData.push_back(oData);
         mapSourceStream[gRin.header.source] = oData.pObsStream;

         referenceSource = gRin.header.source;

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
            
            Synchronize synchro( (*(*it).second), gRef);

            gnssRinex gRin;

            try
            {
               gRin >> synchro;
               gdsMap.addGnssRinex(gRin);
            }
            catch(...)
            {
               if(synchronizeException)
               {
                  stringstream ss;
                  ss << "Exception when try to synchronize at epoch: "
                     << gRef.header.epoch << endl;

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
         it->pObsStream->close();
         delete it->pObsStream;
         it->pObsStream = (RinexObsStream*)0;
      }

      allStreamData.clear();

   }  // End of method 'NetworkObsStreams::cleanUp()'

      // Get the SourceID of the rinex observation file
   SourceID NetworkObsStreams::sourceIDOfRinexObsFile(std::string obsFile)
   {
      try
      {
         RinexObsStream rin;
         rin.exceptions(ios::failbit);
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




