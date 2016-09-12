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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010, 2011
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
 * @file ProblemSatFilter.cpp
 * This class filters out satellites that are in maneuvers or bad data.
 */

#include "ProblemSatFilter.hpp"
#include <fstream>
#include "StringUtils.hpp"


namespace gpstk
{

   using namespace std;

      // Returns a string identifying this object.
   std::string ProblemSatFilter::getClassName() const
   { return "ProblemSatFilter"; }


      /* Returns a satTypeValueMap object, adding the new data generated
       *  when calling this object.
       *
       * @param epoch     Time of observations.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ProblemSatFilter::Process( const CommonTime& epoch,
                                                satTypeValueMap& gData )
      throw(ProcessingException)
   {

      try
      {

         SatIDSet satRejectedSet;
   
            // Loop through all the satellites
         satTypeValueMap::iterator it;
         for (it = gData.begin(); it != gData.end(); ++it) 
         {
            if( isBadSat(epoch,it->first)) satRejectedSet.insert(it->first);
         }

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of 'ProblemSatFilter::Process()'


      /* Returns a gnnsRinex object, adding the new data generated when
       *  calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& ProblemSatFilter::Process(gnssRinex& gData)
      throw(ProcessingException)
   {

      try
      {

         Process(gData.header.epoch, gData.body);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of 'ProblemSatFilter::Process()'


   int ProblemSatFilter::loadSatelliteProblemFile(const string& crxFile)
   {
      ifstream istrm(crxFile.c_str());
      if(istrm.bad()) return -1;

      string buffer;
      
      // let's skip the first 6 lines
      for(int i=0;i<6;i++) getline(istrm,buffer);

      long counter(0);

      while( getline(istrm,buffer) )
      {
         string data(buffer);
         StringUtils::stripTrailing(data);

         StringUtils::strip(buffer);
         if (buffer.length()<1) break;
         
       
         int satellite(0), problem(0), action(0);
         stringstream ss(data);
         ss >> satellite >> problem >> action;
         
         int s[6] = {0};
         for(int i=0;i<6;i++) ss >> s[i];

         CivilTime tempEpoch(s[0],s[1],s[2],s[3],s[4],double(s[5]));
         CommonTime startEpoch( tempEpoch.convertToCommonTime() );
         CommonTime endEpoch(startEpoch);

         if( data.length()>70 )
         {
            for(int i=0;i<6;i++) ss >> s[i];
            
            CivilTime tempEpoch2(s[0],s[1],s[2],s[3],s[4],double(s[5]));
            endEpoch = tempEpoch2.convertToCommonTime();
         }
         
         int spiltFlag = 0;

         string temp = StringUtils::stripLeading(data);
         if(temp[0]=='+') spiltFlag = 1;
         if(temp[0]=='-') spiltFlag =-1;
         
         // We only handle GPS satellites
         if( std::abs(satellite) <= 32 )
         {
            SatID sat(std::abs(satellite),SatID::systemGPS);
            
            SatDataMap::iterator it = satDataMap.find(sat);
            if(it==satDataMap.end())
            {
               satDataMap[sat] = SatDataList();
            }
            
            SatDataList& satDataList = satDataMap[sat];
            
            SatData satData;
            satData.spiltFlag = spiltFlag;
            satData.problemFlag = problem;
            satData.actionFlag = action;
            satData.startEpoch = startEpoch;
            satData.endEpoch = endEpoch;

            satDataList.push_back(satData);
         }
        
         counter++;
      }


      istrm.close();

      return 0;
   }

   bool ProblemSatFilter::isBadSat(const CommonTime& time,const SatID& sat)
   {
      SatDataMap::iterator it = satDataMap.find(sat);
      if( it == satDataMap.end() ) return false;

      SatDataList& dataList = satDataMap[sat];

      for( SatDataList::iterator itt = dataList.begin();
         itt != dataList.end();
         ++itt)
      {
         if( time >= itt->startEpoch && time <= itt->endEpoch )
         {
            if( (itt->actionFlag == 2) || 
                (itt->problemFlag== 1) ||
                (itt->problemFlag== 2) ||
                (itt->problemFlag== 3)   ) { return true; }

         } 

      }  // for( SatDataList::iterator...

      return false;

   }  // End of method 'ProblemSatFilter::isBadSat()'


} // End of namespace gpstk
