#pragma ident "$Id: //depot/msn/prototype/brent/coverage/VisSupport.cpp#2 $"
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
 * @file VisSupport.hpp - Support functions shared by compStaVis and compSatVis.
 */

#include "VisSupport.hpp"

using namespace std;
using namespace gpstk;

StaPosList  VisSupport::getStationCoordinates(
                 const CommandOptionWithAnyArg& mscFileName,
                 const DayTime& dt, 
                 const CommandOptionWithAnyArg& includeStation,
                 const CommandOptionWithAnyArg& excludeStation)
{
   StaPosList staPosList;
   MSCStore mscStore;
   
   try
   {
      mscStore.loadFile( mscFileName.getValue().front() );
      
         // Build list of station names
      list<string> IDList; 
      list<string>::const_iterator vci;
         
         // In the first case, the user has provided a specific list of 
         // stations that are to be included. 
      if (includeStation.getCount()>0)
      {
         vector<string> values;
         values = includeStation.getValue();
         vector<string>::const_iterator vi;
         for (vi=values.begin();vi!=values.end();++vi)
         {
            IDList.push_back( *vi );
         }
         IDList.sort();
         IDList.unique(); 
      }
      
         // In the second case, the user has provided a specific list of 
         // staitons that are to be excluded from the list provided in 
         // the station coordinates file. 
      else if (excludeStation.getCount()>0)
      {
            // First build a list of the stations found in the 
            // coordinate file. 
         IDList = mscStore.getIDList();
         
            // As a precaution against duplicate station numbers in
            // the coordinates file, the list is sorted and duplicates
            // removed.
         IDList.sort();
         IDList.unique(); 
         
            // Then remove any that are found on the exclusion list
         vector<string> values;
         values = excludeStation.getValue();
         vector<string>::const_iterator vi;
         for (vi=values.begin();vi!=values.end();++vi)
         {
            string temp = *vi;
            IDList.remove(temp);
         }
         
      }
         // In the final case, the user has not specified the list of stations
         // other than to provide a station coordinates file.  All stations 
         // in the file will be used. 
      else
      {
         IDList = mscStore.getIDList();
         IDList.sort();
         IDList.unique(); 
      }
      
         // Since the nominal SPS constellation has an epoch of 1993, coerce
         // the station coordinate search time to a later time if necessary.
      DayTime xdt = mscStore.getInitialTime();
      if (dt>xdt) xdt = dt; 

         // We're not going to worry about propagating drift, especically
         // since the "nomnial" constellation has a 1993 epoch and all
         // our positions are post that time.
      for (vci=IDList.begin();vci!=IDList.end();++vci)
      {
         const MSCData& mscd = mscStore.findMSC( (string) *vci, xdt );
         pair<string,ECEF> node( *vci, mscd.coordinates );
         staPosList.insert( node );
      }
   }
   catch (gpstk::Exception& e)
   {
      cerr << e << endl;
      exit(1);
   }
   return(staPosList);
}

void VisSupport::readFICNavData( const CommandOptionWithAnyArg& navFileNameOpt,
                                 GPSAlmanacStore& BCAlmList,
                                 GPSEphemerisStore& BCEphList)
{
      // Get name of the navigation file
   string fullName = navFileNameOpt.getValue().front();
   FileFilterFrame<FICStream, FICData> input(fullName);
   
      // filter the FIC data for the requested vlock(s)
   std::list<long> blockList;
   blockList.push_back(9);
   blockList.push_back(62);
   input.filter(FICDataFilterBlock(blockList));
   
      //some hand waving for the data conversion
   list<FICData>& ficList = input.getData();
   list<FICData>::iterator itr = ficList.begin();
   int count = 0;
   int count62 = 0;
   while (itr != ficList.end())
   {
      FICData& r = *itr;
      if ( r.blockNum == 9)
      {
         count++;
         EngEphemeris ee(r);
         if (checkIOD(ee))
         {
            BCEphList.addEphemeris( ee );
         }
      }
      if ( r.blockNum == 62)
      {
         count62++;
         AlmOrbit ao(r);
         BCAlmList.addAlmanac(ao);
      }
      itr++;
   }
}

void VisSupport::readRINEXNavData(
                 const CommandOptionWithAnyArg& navFileNameOpt,
                       GPSEphemerisStore&       BCEphList)
{
   int rcount = 0;

      // Get name of the navigation file
   string fullName = navFileNameOpt.getValue().front();

      // Process the RINEX data
   FileFilterFrame<RinexNavStream, RinexNavData> rinput(fullName);
   int numRec = 0;
   list<RinexNavData>& rnavList = rinput.getData();
   list<RinexNavData>::iterator ritr = rnavList.begin();
   while (ritr != rnavList.end())
   {
      rcount++;
      RinexNavData& r = *ritr;
      EngEphemeris ee(r);
      if (checkIOD(ee))
      { 
         BCEphList.addEphemeris( ee );
      }
      ritr++;
   }
}

void VisSupport::readYumaData(
                 const CommandOptionWithAnyArg& navFileNameOpt,
                       YumaAlmanacStore&        yumaAlmStore )
{
   int nFiles = navFileNameOpt.getCount();
   vector<std::string> values = navFileNameOpt.getValue(); 
   
   for (int i=0; i<nFiles; ++i)
   {
      // Get name of the navigation file
      string fullName = values[i];
         // Locate and read the Yuma almanac file
      try
      {
         yumaAlmStore.loadFile( fullName );
         //fprintf(logfp," Loaded Yuma data from %s\n",fullName.c_str());
      }
      catch(gpstk::Exception& e) 
      {
         cerr << "Caught Exception while reading Yuma almanac file " << 
         fullName << " : " << e << endl;
         exit(1);
      }
   }
}

void VisSupport::readSEMData( const CommandOptionWithAnyArg& navFileNameOpt,
                                    SEMAlmanacStore&         semAlmStore ) 
{
   int nFiles = navFileNameOpt.getCount();
   vector<std::string> values = navFileNameOpt.getValue(); 
   
   for (int i=0; i<nFiles; ++i)
   {
      // Get name of the navigation file
      string fullName = values[i];
         // Locate and read the almanac file
      try
      {
         semAlmStore.loadFile( fullName );
      }
      catch(gpstk::Exception& e) 
      {
         cerr << "Caught Exception while reading SEM almanac file " << 
         fullName << " : " << e << endl;
         exit(1);
      }
   }
}

void VisSupport::readPEData(
                 const CommandOptionWithAnyArg& navFileNameOpt,
                       SP3EphemerisStore&       SP3EphList )
{
   int nFiles = navFileNameOpt.getCount();
   vector<std::string> values = navFileNameOpt.getValue(); 
   
   for (int i=0; i<nFiles; ++i)
   {
      // Get name of the navigation file
      string fullName = values[i];
         // Locate and read the SP3 file
      try
      {
         SP3EphList.loadFile( fullName );
         //fprintf(logfp," Loaded SP3 data from %s\n",fullName.c_str());
      }
      catch(gpstk::Exception& e) 
      {
         cerr << "Caught Exception while reading SP3 Nav file " << 
         fullName << " : " << e << endl;
         exit(1);
      }
   }
}

   //------------------------------------------------------------------------
   // Test added as workaround for MSN bug.
   // Need to confirm consistency of IODC/IODE across all three subframes.
   // If inconsistent, we don't want to use the data.
bool VisSupport::checkIOD( const EngEphemeris ee )
{

   int IODC = ee.getIODC();
   int IODE = ee.getIODE();
   
      // Test for ephemeris consistency
   int testIODC = IODC & 0x00FF;
   if (testIODC!=IODE) return(false);
   return(true);
}

