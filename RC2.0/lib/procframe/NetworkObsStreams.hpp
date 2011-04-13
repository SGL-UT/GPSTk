#pragma ident "$Id: $"

/**
 * @file NetworkObsStreams.hpp
 * This class synchronizes rinex observation data streams.
 */

#ifndef GPSTK_NETWORK_OBS_STREAMS_HPP
#define GPSTK_NETWORK_OBS_STREAMS_HPP

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

#include <iostream>
#include <string>
#include <list>
#include <map>
#include "RinexObsStream.hpp"
#include "DataStructures.hpp"
#include "Synchronize.hpp"

namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{

      
      /** This class synchronizes rinex observation data streams.
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *    NetworkObsStreams network;
       *
       *    network.addRinexObsFile("NetworkDemo/acor1480.08o");
       *    network.addRinexObsFile("NetworkDemo/madr1480.08o");
       *    network.addRinexObsFile("NetworkDemo/scoa1480.08o");
       *    network.addRinexObsFile("NetworkDemo/sfer1480.08o");
       *    
       *    SourceID refSource = network.sourceIDOfRinexObsFile(
       *                                          "NetworkDemo/acor1480.08o");
       *    network.setReferenceSource(refSource);    
       *
       *    gnssDataMap gdsMap;
       *    while( network.readEpochData(gdsMap) )
       *    {
       *       // processing code here    
       *
       *    }  
       *
       * @endcode
       *
       * Each NetworkObsStreams object will handle a RinexObsStream object
       * for every rinex observation file of the network. It can synchronize
       * reference observation file with all other files, and get a gnssDataMap
       * object holding the network observation data epoch by epoch.
       *
       * By default, NetworkObsStreams object will skip the observation file failed
       * to be synchronized. When 'NetworkObsStreams::setSynchronizeException(true)'
       * is used, it'll throw a 'SynchronizeException' when faied to synchronize data.
       * Then, you must handle it appropriately.
       */
   class NetworkObsStreams
   {
   public:
         /// Default constructor
      NetworkObsStreams() : synchronizeException(false)
      {}

         /// Default destructor
      virtual ~NetworkObsStreams()
      { cleanUp(); }
         
         /// Add a rinex obs file to the network
         /// @obsFile Rinex observation file name
      bool addRinexObsFile(const std::string& obsFile);

         /** Sets the RinexObsStream object of reference data.
          *
          * @param refSource      Reference SourceID of the newwork.
          */
      void setReferenceSource(const SourceID& refSource)
      { referenceSource = refSource; }


      void setSynchronizeException(const bool& synException = true)
      { synchronizeException = synException; }

         /// Get epoch data of the network
         /// @gdsMap  Object hold epoch observation data of the network
         /// @return  Is there more epoch data for the network 
      bool readEpochData(gnssDataMap& gdsMap)
         throw(SynchronizeException);
         
         /// Get the SourceID of the rinex observation file
      SourceID sourceIDOfRinexObsFile(std::string obsFile);

   protected:

         /// Struct to hold all the data for a observation file
      struct ObsData
      {
         std::string obsFile;
         
         SourceID obsSource;

         RinexObsStream* pObsStream;
      };

         /// Object to hold all the data of the network
      std::list<ObsData> allStreamData;

         /// Map to easy access the streams by 'SourceID'
      std::map<SourceID, RinexObsStream*> mapSourceStream;
     
         /// Reference Sourcee
      SourceID referenceSource;
         
         /// Flag indicate will throw 'SynchronizeException'
      bool synchronizeException;

   private:
         // Do some clean operation 
      virtual void cleanUp();

   }; // End of class 'NetworkObsStreams'

      //@}


}  // End of namespace gpstk

#endif   // NetworkObsStreams

