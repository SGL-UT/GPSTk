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
#include <iostream>
#include <valarray>

#include <cmath>

#include "ValarrayUtils.hpp"
#include "PRSolution.hpp"
#include "IonoModel.hpp"

#include "ExtractPC.hpp"

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
      {
         if (!FileUtils::fileAccessCheck(obsList[i]))
         {
            ObsArrayException oae("Cannot read obs file " + obsList[i]);
            GPSTK_THROW(oae);
         }
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

      long totalEpochsObs = 0;
      Triple antPos;
      double dR;
   
      for (int i=0; i< obsList.size(); i++)
      {
         //RinexObsHeader roh;
         long numEpochsObs = 0;
         double dataRate;
         Triple antennaPos;

         scanObsFile(obsList[i], numEpochsObs, dataRate, antennaPos);
         if (i==0)
         {
            antPos=antennaPos;
            dR=dataRate;

            if (antennaPos.mag()<1) // A reported antenna position near the
                                    // center of the Earth. REcompute.
	    {
	       PRSolution prSolver;
               prSolver.RMSLimit = 400;
               GGTropModel ggTropModel; 
	       ggTropModel.setWeather(20., 1000., 50.); // A default model for sea level.

               RinexObsStream tempObsStream(obsList[i]);
               RinexObsData   tempObsData;
                
               tempObsStream >> tempObsData;

               ExtractPC ifObs;
               ifObs.getData(tempObsData);

	       std::vector<SatID> vsats(ifObs.availableSV.size());
               for (size_t ii=0; ii<ifObs.availableSV.size(); ++ii)
	       {
                  vsats[ii]=ifObs.availableSV[ii];
	       }

	       std::vector<double> vranges(ifObs.obsData.size());
               for (size_t ii=0; ii<ifObs.obsData.size(); ++ii)
	       {
                  vranges[ii]=ifObs.obsData[ii];
	       }


               prSolver.RAIMCompute(tempObsData.time,
				    vsats, vranges, 
				    ephStore, &ggTropModel);

               antPos[0] = prSolver.Solution[0];
               antPos[1] = prSolver.Solution[1];
               antPos[2] = prSolver.Solution[2];
	       /*
	       std::cout << "Position resolved at " 
			 << antPos[0] << ", " << antPos[1] << ", "
		         << antPos[2] << std::endl;		
	       */ 

	    }
         }

         if (i!=0)
         {
	   if ( (antPos - antennaPos).mag()>100.)
            {
               ObsArrayException oae("Antenna position approximation varies too much between input files.");
               GPSTK_THROW(oae);
            }

            if ( dataRate != dR )
            {
               ObsArrayException oae("Data rate is not consistent among files");
               GPSTK_THROW(oae);
            }
         }
         totalEpochsObs=totalEpochsObs+numEpochsObs;
      }

      observation.resize(totalEpochsObs*numObsTypes);
      epoch.resize(totalEpochsObs);
      satellite.resize(totalEpochsObs);
      lli.resize(totalEpochsObs);
      azimuth.resize(totalEpochsObs);
      elevation.resize(totalEpochsObs);
      validAzEl.resize(totalEpochsObs);
      pass.resize(totalEpochsObs);
      pass = -1; // Inserted

      validAzEl = true;
      long satEpochIdx = 0; // size_t satEpochIdx=0;
         
      std::map<SatID, DayTime> lastObsTime;
      std::map<SatID, DayTime>::const_iterator it2;
      std::map<SatID, long> currPass;
     
      long highestPass = 0;
      long thisPassNo;

// totalEpochsObs calculated correctly! Now, we need to fill in the valarrays.

      for (size_t i=0 ; i<obsList.size() ; i++)
      {
         RinexObsStream robs(obsList[i]);
         RinexObsHeader roh;

         //robs >> roh;

         RinexObsData rod;
         RinexObsData::RinexSatMap::const_iterator it;

         while (robs >> rod)
         {
            // Second step through the obs for each SV 

            for (it = rod.obs.begin(); it!=rod.obs.end(); it++)
            {

               it2 = lastObsTime.find((*it).first);

               // Step through obs to see if loss of lock is true
               bool thislli=false;
               RinexObsData::RinexObsTypeMap::const_iterator i_rotm;

               for (i_rotm = it->second.begin(); i_rotm!= it->second.end(); i_rotm++)
               {
                  thislli = thislli || (i_rotm->second.lli > 0);
               }
               lli[satEpochIdx]=thislli;
             
            
               if (  (it2==lastObsTime.end()) || (thislli) || ( (rod.time-lastObsTime[(*it).first]) > 1.1*RinexObsHeader::intervalValid) )
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
                     observation[satEpochIdx*numObsTypes+idx] = rod.obs[it->first][basicTypeMap[idx]].data;
                  }
                  else 
                  {
                     expressionMap[idx].setRinexObs(rod.obs[it->first]);
                     observation[satEpochIdx*numObsTypes+idx] = expressionMap[idx].evaluate();
                  }

                  satellite[satEpochIdx]  = it->first;
               } // end of walk through observations to record

            // Get topocentric coords for given sat

               try
               {
                  Xvt svPos = ephStore.getXvt(it->first,rod.time); // Divide by 0 error occurs somewhere in here
                  elevation[satEpochIdx]= antPos.elvAngle(svPos.x); // antennaPosition --> antennaPos.blah1
                  azimuth[satEpochIdx]  = antPos.azAngle(svPos.x); // antennaPosition --> antennaPos.blah2
               }
               catch(InvalidRequest)
               {
                  validAzEl[satEpochIdx]=false;
               }
               //std::cout << "i: (" << satEpochIdx << ")" << rod.time << std::endl;    

            epoch[satEpochIdx]=rod.time;

//std::cout << lli[satEpochIdx] << " " << pass[satEpochIdx] << " " << observation[satEpochIdx] << " " << satellite[satEpochIdx] << " " << elevation[satEpochIdx] << " " << azimuth[satEpochIdx] << " " << epoch[satEpochIdx] << "\n";

            satEpochIdx++;
            }
         }
      }

   numSatEpochs = totalEpochsObs;
   }

   void ObsArray::scanObsFile(const std::string& obsfilename, long& numEpochsObs, double& dataRate, Triple& antennaPos)
   {
      RinexObsStream robs(obsfilename.c_str());
      RinexObsHeader roh;

      robs >> roh;

      RinexObsData rod;

      if ( (roh.valid & RinexObsHeader::antennaPositionValid) == RinexObsHeader::antennaPositionValid)
      {
         if ( ! ((roh.antennaPosition[0]==roh.antennaPosition[1]) && (roh.antennaPosition[0]==roh.antennaPosition[2]) && (roh.antennaPosition[0]==0)) )
	 {
            antennaPos = roh.antennaPosition;
         }
      }

/* this if loop never seems to be entered
      if ( (roh.valid & RinexObsHeader::intervalValid) == RinexObsHeader::intervalValid)
      {
         dataRate = roh.interval;
      }
*/

      ///// Using this because the loops used before were not being entered into, can investigate that later and maybe reinsert them.
      dataRate=RinexObsHeader::intervalValid;
      /////

      while ( robs >> rod )
      { 
         numEpochsObs += rod.obs.size();
      }
   }

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
      //std::cout << "ObsArray::edit.numSatEpochs: " << numSatEpochs << "\n";
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
 

