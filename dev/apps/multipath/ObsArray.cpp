#pragma ident "$Id$"

/**
 * @file ObsArray.cpp
 * Provides ability to operate mathematically on large, logical groups of observations
 * Class definitions.
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
//  Copyright 2008, The University of Texas at Austin
//
//============================================================================

#include <vector>
#include <set>

#include "ValarrayUtils.hpp"
#include "PRSolution.hpp"

#include "RinexObsStream.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsData.hpp"
#include "FileUtils.hpp"
#include "ObsArray.hpp"

namespace gpstk 
{
   
   ObsArray::ObsArray(void) 
         : numObsTypes(0)
   {
   }

   ObsIndex ObsArray::add(RinexObsHeader::RinexObsType type)
   {
      isBasic[numObsTypes]=true;
      basicTypeMap[numObsTypes]=type;
      return numObsTypes++;
   }
   
   ObsIndex ObsArray::add(const std::string& expression)
   {
      isBasic[numObsTypes]=false;
      expressionMap[numObsTypes] = Expression(expression);
      expressionMap[numObsTypes].setGPSConstants();
      return numObsTypes++;
   }

   void ObsArray::load(const std::string& obsfilename, 
                       const std::string& navfilename)
   {
      std::vector<std::string> obsList(1), navList(1);
      obsList[0]=obsfilename;
      navList[0]=navfilename;
      load(obsList, navList);
   }

   void ObsArray::load(const std::vector<std::string>& obsList, 
                       const std::vector<std::string>& navList)
   {
         // First check for existance of input files
      for (int i=0; i< obsList.size(); i++)
         if (!FileUtils::fileAccessCheck(obsList[i]))
         {
            ObsArrayException oae("Cannot read obs file " + obsList[i]);
            GPSTK_THROW(oae);
         }

      for (int i=0; i< navList.size(); i++)
      {
         
         if (!FileUtils::fileAccessCheck(navList[i]))
         {
            ObsArrayException oae("Cannot read nav file " + navList[i]);
            GPSTK_THROW(oae);
         }
         else

         // Load the ephemeris information from the named NAV file.
         ephStore.loadFile(navList[i]);
      }
   
      for (int i=0; i< obsList.size(); i++)
      {
         loadObsFile(obsList[i]);
      }
      
   }
   

   void ObsArray::loadObsFile(const std::string& obsfilename)
   {
         // Load the obs file header
      RinexObsStream robs(obsfilename.c_str());
      RinexObsStream robsAgain(obsfilename.c_str());
      RinexObsHeader roh;
      
      robs >> roh;
      
         // Verify we have a suggested approximate location. If not, note that.
      bool staticPositionDefined=false;
      Triple antennaPos;
      
      if ( (roh.valid & RinexObsHeader::antennaPositionValid) == 
           RinexObsHeader::antennaPositionValid)
      {
         if ( ! ((antennaPos[0]==antennaPos[1]) &&
                (antennaPos[0]==antennaPos[2]) &&
                (antennaPos[0]==0)) )
	 {
            antennaPos = roh.antennaPosition;
            staticPositionDefined=true;
         }
      }
      
         // Remember the data collection rate. If not available, note that.
      bool intervalDefined=false;
      
      if ( (roh.valid & RinexObsHeader::intervalValid) == 
           RinexObsHeader::intervalValid)
      {
         interval = roh.interval;
         intervalDefined=true;
         intervalInferred=false;
      }

      RinexObsData rod;
      RinexObsData::RinexSatMap::const_iterator it;
      
         // Read through file the first time.
         // In this pass, get the "size" of the data
         // Calculate if needed an approximate user positoin,
         // and data collection interval.

      numSatEpochs = 0;

      bool firstEpochCompleted = false;
      DayTime lastEpochValue;
      std::set<double> intervalDifferences;
      
      while ( robs >> rod )
      { 
            // Account for total amount of obs data in this file
         numSatEpochs += rod.obs.size();

            // Record the interval differences
         if (!intervalDefined)
         {
            if (!firstEpochCompleted)
            {
               lastEpochValue = rod.time;
               firstEpochCompleted = true;
            }
            else
            {
               intervalDifferences.insert(ceil(rod.time - lastEpochValue));
               lastEpochValue = rod.time;
            }
         }
         

            // If necessary, determine the initial user position
         if (!staticPositionDefined)
         {
            PRSolution prEst;
            ZeroTropModel nullTropModel;
            
            std::vector<SatID> sats;
            std::vector<double> ranges;
            RinexObsData::RinexSatMap::const_iterator it;

            for (it = rod.obs.begin(); it!= rod.obs.end(); it++)
            {
               RinexObsData::RinexObsTypeMap otmap;
               RinexObsData::RinexObsTypeMap::const_iterator itPR;
               otmap = it->second;
               itPR = otmap.find(RinexObsHeader::P1);
               if (itPR == otmap.end())
                 itPR = otmap.find(RinexObsHeader::C1);

               if (itPR!=otmap.end())
               {
                  sats.push_back(it->first);
                  ranges.push_back(itPR->second.data);
               }
            }
            
            prEst.RMSLimit = 10000;
            prEst.RAIMCompute(rod.time, sats, ranges, ephStore, 
                              &nullTropModel);
            
            if (prEst.isValid())
            {
               antennaPos[0] = prEst.Solution[0];
               antennaPos[1] = prEst.Solution[1];
               antennaPos[2] = prEst.Solution[2];
               staticPositionDefined = true;
            }
         } // End first blush estimate of static or initial position

      } // Finish first run through file

      if (!intervalDefined)
      {
         
         using namespace ValarrayUtils;   
            //std::cout << "intervals were: " << intervalDifferences << std::endl;
         std::set<double>::iterator itEpochDiff = intervalDifferences.begin();
         interval = *itEpochDiff;
         intervalDefined = true;
         intervalInferred = true;
      }
      
      if (!intervalDefined)
      {
         ObsArrayException oae("Cannot determine data interval for " + obsfilename);
         GPSTK_THROW(oae);
      }

      int i=0;

         // Size the storage valarrays. 
      observation.resize(numSatEpochs*numObsTypes);
      epoch.resize(numSatEpochs);
      satellite.resize(numSatEpochs);
      lli.resize(numSatEpochs);
      azimuth.resize(numSatEpochs);
      elevation.resize(numSatEpochs);
      validAzEl.resize(numSatEpochs);
      pass.resize(numSatEpochs);
      pass = -1;
      
      validAzEl = true;
      size_t satEpochIdx=0;
         
      std::map<SatID, DayTime> lastObsTime;
      std::map<SatID, DayTime>::const_iterator it2;
      std::map<SatID, long> currPass;
     
      long highestPass = 0;
      long thisPassNo;
      
         // Second time through, fill in observations and pass numbers      
         // First step through each epoch of observation
      while (robsAgain >> rod)
      {
            // Second step through the obs for each SV 
         
         for (it = rod.obs.begin(); it!=rod.obs.end(); it++)
         {
            it2 = lastObsTime.find((*it).first);

                // Step through obs to see if loss of lock is true
            bool thislli=false;
            RinexObsData::RinexObsTypeMap::const_iterator i_rotm;
            for (i_rotm = it->second.begin(); 
                 i_rotm!= it->second.end(); i_rotm++)
            {
               thislli = thislli || (i_rotm->second.lli > 0);
            }
            lli[satEpochIdx]=thislli;
             
            
            if (  (it2==lastObsTime.end()) || (thislli) ||  
                   ( (rod.time-lastObsTime[(*it).first]) > 1.1*interval) )
            {
               thisPassNo = highestPass;
               lastObsTime[(*it).first]=rod.time;
               currPass[(*it).first]=highestPass++;
            }
            else
            { 
               thisPassNo = currPass[(*it).first];
               lastObsTime[(*it).first]=rod.time;
            }


            pass[satEpochIdx]=thisPassNo;

            for (int idx=0; idx<numObsTypes; idx++)
            {
               if (isBasic[idx])
               {
                  observation[i] = rod.obs[it->first][basicTypeMap[idx]].data;
               }
               else 
               {
                  expressionMap[idx].setRinexObs(rod.obs[it->first]);
                  observation[i] = expressionMap[idx].evaluate();
               }

               satellite[satEpochIdx]  = it->first;               
               i++;
            } // end of walk through observations to record
 
            // Get topocentric coords for given sat
            try
            {
               Xvt svPos = ephStore.getXvt(it->first, rod.time);
               elevation[satEpochIdx]= antennaPos.elvAngle(svPos.x);
               azimuth[satEpochIdx]  = antennaPos.azAngle( svPos.x);
            }
            catch(InvalidRequest)
            {
               validAzEl[satEpochIdx]=false;
            }
               //std::cout << "i: (" << satEpochIdx << ")" << rod.time << std::endl;
            

            epoch[satEpochIdx]=rod.time;

            satEpochIdx++;
         } // end of walk through prns at this epoch
      } // end of walk through obs file



   } // end of ObsArray::loadObsFile(...)

   void ObsArray::edit(const std::valarray<bool> strikeList)
     throw(ObsArrayException)
   {
      using namespace std;
 
      if (epoch.size() != strikeList.size())
      {
         ObsArrayException e("Edit request has wrong size.");
         GPSTK_THROW(e);
      }

      valarray<bool> keepList = !strikeList;

      valarray<DayTime> newEpoch = epoch[keepList];
      size_t newObsEpochCount = newEpoch.size();
      
      epoch.resize(newObsEpochCount);
      epoch = newEpoch;
     
      valarray<SatID> newSatellite = satellite[keepList];
      satellite.resize(newObsEpochCount);
      satellite = newSatellite;
      
      valarray<bool> newLLI = lli[keepList];
      lli.resize(newObsEpochCount);
      lli = newLLI;
      
      valarray<double> newAz = azimuth[keepList];
      azimuth.resize(newObsEpochCount);
      azimuth = newAz;
      
      valarray<double> newEl = elevation[keepList];
      elevation.resize(newObsEpochCount);
      elevation = newEl;
      
      valarray<bool> newValidAzEl = validAzEl[keepList];
      validAzEl.resize(newObsEpochCount);
      validAzEl   = newValidAzEl;
      
      valarray<long> newPass = pass[keepList];
      pass.resize(newObsEpochCount);
      pass = newPass;
      
      valarray<bool> keepObs;
      keepObs.resize(numObsTypes*numSatEpochs);
      for (size_t i=0; i<(numObsTypes*numSatEpochs); i+=numSatEpochs)
      {
         slice thisObsTypeSlice(i,i+numSatEpochs,1);
         keepObs[thisObsTypeSlice]=keepList;
      }
      valarray<double> newObs =observation[keepObs];
      observation.resize(newObs.size());
      observation = newObs;

         // Update public attributes
      numSatEpochs = newObsEpochCount;
      
   }
   
   double ObsArray::getPassLength(long passNo)
   {
         // TODO: use find_first_of to smartly search over pass number.
         // it doesn't seem to be working at least in gcc. :-(
         // Again we must allocate space just to do a search!!

      using namespace std;
      
      valarray<bool> ptest = (pass==passNo);
      valarray<DayTime> pepochs = epoch[ptest];
      double length =  pepochs[pepochs.size()-1] - pepochs[0];
      return length;
   }
   
} // end namespace gpstk
 

