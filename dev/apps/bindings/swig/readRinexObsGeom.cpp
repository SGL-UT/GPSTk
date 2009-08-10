#pragma ident "$Id$"
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






#include <string>

#include <octave/oct.h>
#include "oct-map.h"

#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "RinexNavBase.hpp"
#include "RinexNavData.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavStream.hpp"
#include "BCEphemerisStore.hpp"

using namespace std;

DEFUN_DLD (readRinexObsGeom, args, , "\n[header, obs, azel]=readRinexObsGeom(obsfile,navfile)\n\nReads RINEX obs file and returns it as a matrix.\nAlso computes azimuth and elevation for each obs.\n")
{

   Matrix obsMatrix, azelMatrix;
   gpstk::BCEphemerisStore bcestore;
   int numObsTypes=-1, numPrns;

   Octave_map headerStruct;

   if (args.length() != 2)
   {
      print_usage("readRinexObsGeom");
      octave_value_list retval;
      retval(0) = octave_value(Matrix());
      retval(1) = octave_value(Matrix());
      retval(2) = octave_value(Matrix());

      return retval;  
   }

   try
   {
      // Read nav file and build unique list of ephemeredes
      gpstk::RinexNavStream rnffs(args(1).string_value().data());
      gpstk::RinexNavHeader rnh;
      gpstk::RinexNavData rne;

      rnffs >> rnh;

      while (rnffs >> rne)
      {
         bcestore.addEphemeris(rne);
      }

      bcestore.SearchNear(); // This is unrealistic but necessary just
                             // to get an ephemeris almost 100% of the time
      //bcestore.dump(1);

      std::string obsfilename=args(0).string_value();

      gpstk::RinexObsStream roffs(obsfilename.data());
      roffs.exceptions(ios::failbit);
      gpstk::RinexObsHeader roh;
      gpstk::RinexObsData rod;
      
      roffs >> roh;

      if (roh.isValid())
      {
         if (roh.valid & gpstk::RinexObsHeader::versionValid)
            headerStruct["rinex_version_type"](0) = (roh.version);

         if (roh.valid & gpstk::RinexObsHeader::runByValid)
	 {
            headerStruct["pgm"](0) = roh.fileProgram;
            headerStruct["run_by"](0) = roh.fileAgency;
            headerStruct["date"](0) = roh.date;
         }

         if (roh.valid & gpstk::RinexObsHeader::markerNameValid)
	 {
	     headerStruct["marker_name"](0) = roh.markerName;
	 }

         if (roh.valid & gpstk::RinexObsHeader::markerNumberValid)
	 {
	     headerStruct["marker_number"](0) = roh.markerNumber;
	 }

         if (roh.valid & gpstk::RinexObsHeader::commentValid)
	 {
	    vector<string>::iterator i;
            i = roh.commentList.begin();
            int n = 0;
            for (;
                 i!=roh.commentList.end();
		 i++,n++)
               headerStruct["comment"](n) = *i;
	 }
         
         if (roh.valid & gpstk::RinexObsHeader::observerValid)
	    headerStruct["observer"](0) = roh.observer;

         if (roh.valid & gpstk::RinexObsHeader::receiverValid)
	 {
	    headerStruct["receiver_number" ](0) = roh.recNo;
	    headerStruct["receiver_type"   ](0) = roh.recType;
            headerStruct["receiver_version"](0) = roh.recVers;
         }

         if (roh.valid & gpstk::RinexObsHeader::antennaTypeValid)
	 {
            headerStruct["antenna_number"](0) = roh.antNo;
	    headerStruct["antenna_type"  ](0) = roh.antType;
	 }

         if (roh.valid & gpstk::RinexObsHeader::antennaPositionValid)
	 {
            ColumnVector aPos(3);
            aPos(0) = roh.antennaPosition[0];
            aPos(1) = roh.antennaPosition[1];
            aPos(2) = roh.antennaPosition[2];
            headerStruct["antenna_position"](0) = aPos;
	 }

         if (roh.valid & gpstk::RinexObsHeader::antennaOffsetValid)
	 {
	   ColumnVector aOff(3);
	   aOff(0) = roh.antennaOffset[0];
	   aOff(1) = roh.antennaOffset[1];
	   aOff(2) = roh.antennaOffset[2];
	   headerStruct["antenna_offset"](0) = aOff;
	 }

         if (roh.valid & gpstk::RinexObsHeader::waveFactValid)
	 {
	   ColumnVector waveFact(2);
	   waveFact(0) = static_cast<short> (roh.wavelengthFactor[0]);
	   waveFact(1) = static_cast<short> (roh.wavelengthFactor[1]);
	   headerStruct["wavelength_factor"](0) = waveFact;
	 }

         // The extraWaveFactList code is untested for lack of 
         // real datafiles (externally generated) to test it.
         // This generates a single matrix with all wave factor info
         // in it.
	 // Column, Contents
	 // 1       PRN
         // 2       System number
         // 3       L1 wave factor
         // 4       L2 wave factor
         if (!roh.extraWaveFactList.empty())
	 {
            Matrix waveFactors(0,0);
	    int nprnsTot = 0;
            vector<gpstk::RinexObsHeader::ExtraWaveFact>::iterator i;
            for (i=roh.extraWaveFactList.begin();
                 i!=roh.extraWaveFactList.end();
	         i++)
	    {
               int nprns = (*i).prnList.size();
               waveFactors.resize(nprnsTot+nprns, 4);
               for (int j=0; j<nprns; j++)
	       {
		  waveFactors(nprnsTot+j,0) = (*i).prnList[j].prn;
	          waveFactors(nprnsTot+j,1) = (*i).prnList[j].system;
                  waveFactors(nprnsTot+j,2) = (*i).wavelengthFactor[0];
                  waveFactors(nprnsTot+j,3) = (*i).wavelengthFactor[1];
	       }
               nprnsTot += nprns;
	    }

            headerStruct["wave_factors_by_prn"](0) = waveFactors;
	 }

         if (roh.valid & gpstk::RinexObsHeader::obsTypeValid)
	 {
	    std::string obsList;
            for (int i=0; i<roh.obsTypeList.size(); i++)
	    {
	      obsList += roh.obsTypeList[i].type + string(" ");
	    }
	    headerStruct["obs_types"](0)=obsList;

            numObsTypes = roh.obsTypeList.size();            
	 }

         if (roh.valid & gpstk::RinexObsHeader::intervalValid)
	 {
            headerStruct["interval"](0) = static_cast<double> (roh.interval);
	 }

         if (roh.valid & gpstk::RinexObsHeader::firstTimeValid)
	 {
	    ColumnVector firstTime(3);
            firstTime(0)=static_cast<short> (roh.firstObs.DOYyear());
            firstTime(1)=static_cast<short> (roh.firstObs.DOYday());
            firstTime(2)=static_cast<double> (roh.firstObs.DOYsecond());
	    headerStruct["time_of_first_obs"](0) = firstTime;
	 }

	 // Untested.
         if (roh.valid & gpstk::RinexObsHeader::lastTimeValid)
	 {
	    ColumnVector lastTime(3);
            lastTime(0)=static_cast<short> (roh.lastObs.DOYyear());
            lastTime(1)=static_cast<short> (roh.lastObs.DOYday());
            lastTime(2)=static_cast<double> (roh.lastObs.DOYsecond());
	    headerStruct["time_of_last_obs"](0) = lastTime;
	 }

         if (roh.valid & gpstk::RinexObsHeader::receiverOffsetValid)
	 {
	    headerStruct["receiver_offset"](0)=static_cast<int> (roh.receiverOffset);
	 }

         if (roh.valid & gpstk::RinexObsHeader::leapSecondsValid)
	 {
	    headerStruct["leap_seconds"](0)=static_cast<int> (roh.leapSeconds);          }

         if (roh.valid & gpstk::RinexObsHeader::numSatsValid)
	 {
	    headerStruct["numSVs"](0)=static_cast<short> (roh.numSVs);
	 }
	 
         if (roh.valid & gpstk::RinexObsHeader::prnObsValid)
	 {
	    Matrix prnObs(roh.numObsForPrn.size(),numObsTypes+2);
            map<gpstk::RinexPrn::RinexPrn, vector<int> >::iterator i;
            int row=0;
            for (i=roh.numObsForPrn.begin(),row=0;
                 i!=roh.numObsForPrn.end();
                 i++,row++)
	    {
	      prnObs(row,0) = (*i).first.prn;
              prnObs(row,1) = (*i).first.system;
              if (numObsTypes==-1)
                 numObsTypes = (*i).second.size();
	      for (int j=0;j<numObsTypes;j++)
		prnObs(row,j+2)=(*i).second[j];
      	    }
	    headerStruct["num_of_obs_for_prn"](0)=prnObs;            
	  }

      }

      long nrecs = 0;

      // First scan, just to get the total number of records (prn-epochs)
      while (roffs >> rod)
      {
         // Apply any editing criteria.
	 if  (rod.epochFlag == 0 || rod.epochFlag == 1)
            nrecs = nrecs + rod.numSvs;
      }

      obsMatrix.resize(nrecs,numObsTypes+4,0);
      azelMatrix.resize(nrecs,2); // First col is az, second is el

      // Second scan
      gpstk::RinexObsStream roffs2(obsfilename.data());
      roffs2.exceptions(ios::failbit);
      int currentRec = 0;
      roffs2 >> roh;

      while (roffs2>>rod) // Loop through each epoch
      {
         // Apply editing criteria (again)
         if  (rod.epochFlag == 0 || rod.epochFlag == 1) // Begin usable data
	 {
            short year = rod.time.DOYyear();
            short doy  = rod.time.DOYday();
	    double sod = rod.time.DOYsecond();

            short iodc;
	    gpstk::Xvt svpos;
            short prn;

	    gpstk::RinexObsData::RinexPrnMap::iterator it;
            for (it=rod.obs.begin(); it!=rod.obs.end(); it++) // PRN loop
	    {
               obsMatrix(currentRec,0)=year;
               obsMatrix(currentRec,1)=doy;
               obsMatrix(currentRec,2)=sod;
               prn = it->first.prn;
               obsMatrix(currentRec,3)= prn;
               // What to do with system information? sigh...a new matrix?


               // Compute topocentric coords for this satellite
               // i.e., azimuth and elevation
               if (roh.valid & gpstk::RinexObsHeader::antennaPositionValid)
	       {
		 //cout << " prn " << prn << " epoch " << rod.time << endl;
                 // cout << bcestore.getInitialTime() << endl;
                 // cout << bcestore.getFinalTime() << endl;

		  try {
                  svpos = bcestore.getPrnXvt(prn,rod.time,iodc);
                  azelMatrix(currentRec,0)=roh.antennaPosition.azAngle(svpos.x);
                  azelMatrix(currentRec,1)=roh.antennaPosition.elvAngle(svpos.x);
                  }
                  //catch (gpstk::EphemerisStore::NoEphemerisFound& x)
		    //{
                    // cout << "No ephemeris found!" << endl;
		     //} 
                  catch (...) {} 
	       }

               gpstk::RinexObsData::RinexObsTypeMap::iterator jt;
               		 
               int column = 4; // First column of observations

               for (jt  = it->second.begin();
                    jt != it->second.end();
                    jt++,column++) // Loop through obs types for this PRN
	       {
                  obsMatrix(currentRec,column)=jt->second.data;
                  // What can we do with lli and ssi? build a new matrix?
	       } // End loop through obs types for this PRN
               currentRec++;
	    } // End loop PRNs for this epoch

	 } // End usable data 

      } // End loop through each epoch
     //  exit(0);
   }
   catch(gpstk::FFStreamError& e)
   {
     // cout << e;
      //  exit(1);
   }
   catch(gpstk::Exception& e)
   {
     //cout << e;
      //      exit(1);
   }
   catch (...)
   {
     //cout << "unknown error.  Done." << endl;
      //      exit(1);
   }

   octave_value_list retval;
   retval(0) = octave_value( headerStruct);
   retval(1) = octave_value( obsMatrix);
   retval(2) = octave_value( azelMatrix);

   return retval;
}








