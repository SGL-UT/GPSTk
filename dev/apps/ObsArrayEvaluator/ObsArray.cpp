#pragma ident "$Id: ObsArray.cpp 1161 2008-03-27 17:16:22Z ckiesch $"

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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

/**
 * @file ObsArray.cpp
 * Provides ability to operate mathematically on large, logical groups of observations
 * Class definitions.
 */

#include <vector>
#include <set>

#include "ValarrayUtils.hpp"
#include "PRSolution.hpp"
#include "ObsReader.hpp"

#include "ObsArray.hpp"

using namespace std;

namespace gpstk 
{
   
   ObsArray::ObsArray()
      : numObsTypes(0), numSatEpochs(0), highestPass(0),
        debugLevel(0), interval(0)
   {}


   ObsIndex ObsArray::add(const ObsID type)
      throw()
   {
      isBasic[numObsTypes]=true;
      basicTypeMap[numObsTypes]=type;
      return numObsTypes++;
   }

   
   ObsIndex ObsArray::add(const std::string& expression)
      throw()
   {
      isBasic[numObsTypes]=false;
      expressionMap[numObsTypes] = Expression(expression);
      expressionMap[numObsTypes].setGPSConstants();
      return numObsTypes++;
   }


   void ObsArray::load(const std::vector<std::string>& obsFiles,
                       EphReader::EphemerisStore& eph,
                       Position& rxPos)
      throw(FileMissingException, ObsArrayException)
   {
      for (int i=0; i< obsFiles.size(); i++)
         loadObsFile(obsFiles[i], eph, rxPos);
   }
   

   void ObsArray::loadObsFile(const std::string& fn, 
                              EphReader::EphemerisStore& eph,
                              Position& rxPos)
      throw(FileMissingException, ObsArrayException)
   {
      if (interval == 0)
      {
         // This will throw a FileMissingException for us if we can't open the file
         ObsReader obsReader(fn, debugLevel);
         interval = obsReader.estimateObsInterval();
         if (interval<0)
         {
            ObsArrayException oae("Cannot determine data interval for " + fn);
            GPSTK_THROW(oae);
         }
      }

      // Make a pass through the file and just figure out how much data
      // we have in this file.
      size_t newSize = numSatEpochs;
      {
         ObsReader obsReader(fn);
         for (ObsEpoch oe; obsReader >> oe;)
            newSize += oe.size();
      }

      if (debugLevel)
         cout << newSize-numSatEpochs << " observations in file" << endl;

      observation.resize(newSize*numObsTypes);
      epoch.resize(newSize);
      satellite.resize(newSize);
      lli.resize(newSize);
      azimuth.resize(newSize);
      elevation.resize(newSize);
      validAzEl.resize(newSize);
      pass.resize(newSize);

      // Now fill in observations and pass numbers for all data in the file
      try
      {
         ObsReader obsReader(fn);
         for (ObsEpoch oe; obsReader >> oe;)
         {
            for (ObsEpoch::const_iterator i=oe.begin(); i != oe.end(); i++)
            {

               const SvObsEpoch& soe = i->second;
               const SatID& svid = i->first;

               epoch[numSatEpochs] = oe.time;
               satellite[numSatEpochs] = svid;
            
               // Step through obs to see if loss of lock is true for any obs type
               // If it is, mark the entire sv with lli for the epoch.
               // Ignore the collected under AS bit.
               bool thislli=false;
               SvObsEpoch::const_iterator j;
               for (j = soe.begin(); j != soe.end() && !thislli; j++)
                  thislli = j->first.type == ObsID::otLLI &&
                     (int(j->second) & ~0x4);
               lli[numSatEpochs] = thislli;

               map<SatID, DayTime>::const_iterator it2 = lastObsTime.find(svid);
               if (it2==lastObsTime.end() || thislli ||  
                   (oe.time-lastObsTime[svid]) > 1.1*interval )
               {
                  pass[numSatEpochs] = highestPass;
                  lastObsTime[svid] = oe.time;
                  currPass[svid] = highestPass++;
               }
               else
               { 
                  pass[numSatEpochs] = currPass[svid];
                  lastObsTime[svid] = oe.time;
               }

               const size_t oi = numSatEpochs*numObsTypes;
               for (size_t idx=0; idx<numObsTypes; idx++)
               {
                  if (isBasic[idx])
                  {
                     SvObsEpoch::const_iterator j = soe.find(basicTypeMap[idx]);
                     if (j != soe.end())
                        observation[oi+idx] = j->second;
                  }
                  else
                  {
                     expressionMap[idx].setSvObsEpoch(soe);
                     observation[oi+idx] = expressionMap[idx].evaluate();
                  }
               } // end of walk through observations to record for this epoch

               // Now compute a 'good' az/el for the SV
               Xvt svPos = eph.getXvt(svid, oe.time);
               elevation[numSatEpochs]= rxPos.elvAngle(svPos.x);
               azimuth[numSatEpochs]  = rxPos.azAngle( svPos.x);
               validAzEl[numSatEpochs]=true;

               numSatEpochs++;
            } // end of walk through prns at this epoch
         } // end of walk through entire obs file
      }
      catch (Exception& e)
      {
         ObsArrayException oae(e);
         GPSTK_THROW(oae);
      }
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
   
   double ObsArray::getPassLength(long passNo) throw()
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
