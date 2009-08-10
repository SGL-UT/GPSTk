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

using namespace std;

DEFUN_DLD (readRinexObsFast, args, , "Reads RINEX obs file and returns it as a matrix.")
{
   Matrix obsMatrix;
   int numObsTypes=-1, numPrns;

   Octave_map headerStruct;

   if (args.length() != 1)
   {
      print_usage();
      return octave_value(headerStruct); // 
   }

   try
   {
      std::string obsfilename=args(0).string_value();

      gpstk::RinexObsStream roffs(obsfilename.data());
      roffs.exceptions(ios::failbit);
      gpstk::RinexObsHeader roh;
      gpstk::RinexObsData rod;
      
      roffs >> roh;

      if (roh.isValid())
      {
         if (roh.valid & gpstk::RinexObsHeader::versionValid)
            headerStruct.assign("rinex_version_type", roh.version);

         if (roh.valid & gpstk::RinexObsHeader::runByValid)
	 {
            headerStruct.assign("pgm", roh.fileProgram);
            headerStruct.assign("run_by", roh.fileAgency);
            headerStruct.assign("date",roh.date);
         }

         if (roh.valid & gpstk::RinexObsHeader::markerNameValid)
	 {
	     headerStruct.assign("marker_name",roh.markerName);
	 }

         if (roh.valid & gpstk::RinexObsHeader::markerNumberValid)
	 {
	     headerStruct.assign("marker_number",roh.markerNumber);
	 }

         if (roh.valid & gpstk::RinexObsHeader::commentValid)
	 {
	   Octave_map commentmap;
             
	     vector<string>::iterator i;
	     int n=0;
	     for (i = roh.commentList.begin();
                  i!=roh.commentList.end();
	    	 i++,n++) 
             commentmap.assign(gpstk::StringUtils::asString(n),*i);

             headerStruct.assign("comments",commentmap);
	 }
         
         
         if (roh.valid & gpstk::RinexObsHeader::observerValid)
	    headerStruct.assign("observer", roh.observer);

         if (roh.valid & gpstk::RinexObsHeader::receiverValid)
	 {
	    headerStruct.assign("receiver_number", roh.recNo);
	    headerStruct.assign("receiver_type",  roh.recType);
            headerStruct.assign("receiver_version", roh.recVers);
         }

         if (roh.valid & gpstk::RinexObsHeader::antennaTypeValid)
	 {
            headerStruct.assign("antenna_number",roh.antNo);
	    headerStruct.assign("antenna_type", roh.antType);
	 }

         if (roh.valid & gpstk::RinexObsHeader::antennaPositionValid)
	 {
            ColumnVector aPos(3);
            aPos(0) = roh.antennaPosition[0];
            aPos(1) = roh.antennaPosition[1];
            aPos(2) = roh.antennaPosition[2];
            headerStruct.assign("antenna_position", aPos);
	 }

         if (roh.valid & gpstk::RinexObsHeader::antennaOffsetValid)
	 {
	   ColumnVector aOff(3);
	   aOff(0) = roh.antennaOffset[0];
	   aOff(1) = roh.antennaOffset[1];
	   aOff(2) = roh.antennaOffset[2];
	   headerStruct.assign("antenna_offset", aOff);
	 }

         if (roh.valid & gpstk::RinexObsHeader::waveFactValid)
	 {
	   ColumnVector waveFact(2);
	   waveFact(0) = static_cast<short> (roh.wavelengthFactor[0]);
	   waveFact(1) = static_cast<short> (roh.wavelengthFactor[1]);
	   headerStruct.assign("wavelength_factor",  waveFact);
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
               int nprns = (*i).satList.size();
               waveFactors.resize(nprnsTot+nprns, 4);
               for (int j=0; j<nprns; j++)
	       {
		  waveFactors(nprnsTot+j,0) = (*i).satList[j].id;
	          waveFactors(nprnsTot+j,1) = (*i).satList[j].system;
                  waveFactors(nprnsTot+j,2) = (*i).wavelengthFactor[0];
                  waveFactors(nprnsTot+j,3) = (*i).wavelengthFactor[1];
	       }
               nprnsTot += nprns;
	    }

            headerStruct.assign("wave_factors_by_prn", waveFactors);
	 }

         if (roh.valid & gpstk::RinexObsHeader::obsTypeValid)
	 {
	    std::string obsList;
            for (int i=0; i<roh.obsTypeList.size(); i++)
	    {
	      obsList += roh.obsTypeList[i].type + string(" ");
	    }
	    headerStruct.assign("obs_types",obsList);

            numObsTypes = roh.obsTypeList.size();            
	 }

         if (roh.valid & gpstk::RinexObsHeader::intervalValid)
	 {
            headerStruct.assign("interval",roh.interval);
	 }

         if (roh.valid & gpstk::RinexObsHeader::firstTimeValid)
	 {
	    ColumnVector firstTime(3);
            firstTime(0)=static_cast<short> (roh.firstObs.DOYyear());
            firstTime(1)=static_cast<short> (roh.firstObs.DOYday());
            firstTime(2)=static_cast<double> (roh.firstObs.DOYsecond());
	    headerStruct.assign("time_of_first_obs",firstTime);
	 }

	 // Untested.
         if (roh.valid & gpstk::RinexObsHeader::lastTimeValid)
	 {
	    ColumnVector lastTime(3);
            lastTime(0)=static_cast<short> (roh.lastObs.DOYyear());
            lastTime(1)=static_cast<short> (roh.lastObs.DOYday());
            lastTime(2)=static_cast<double> (roh.lastObs.DOYsecond());
	    headerStruct.assign("time_of_last_obs", lastTime);
	 }

         if (roh.valid & gpstk::RinexObsHeader::receiverOffsetValid)
	 {
	    headerStruct.assign("receiver_offset",static_cast<int> (roh.receiverOffset));
	 }

         if (roh.valid & gpstk::RinexObsHeader::leapSecondsValid)
	 {
	    headerStruct.assign("leap_seconds",static_cast<int> (roh.leapSeconds));          }

         if (roh.valid & gpstk::RinexObsHeader::numSatsValid)
	 {
	    headerStruct.assign("numSVs",static_cast<short> (roh.numSVs));
	 }
	 
         if (roh.valid & gpstk::RinexObsHeader::prnObsValid)
	 {
	    Matrix prnObs(roh.numObsForSat.size(),numObsTypes+2);
            map<gpstk::SatID, vector<int> >::iterator i;
            int row=0;
            for (i=roh.numObsForSat.begin(),row=0;
                 i!=roh.numObsForSat.end();
                 i++,row++)
	    {
	      prnObs(row,0) = (*i).first.id;
              prnObs(row,1) = (*i).first.system;
              if (numObsTypes==-1)
                 numObsTypes = (*i).second.size();
	      for (int j=0;j<numObsTypes;j++)
		prnObs(row,j+2)=(*i).second[j];
      	    }
	    headerStruct.assign("num_of_obs_for_sat",prnObs);            
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

      obsMatrix.resize(nrecs,numObsTypes+5,0);
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

	    gpstk::RinexObsData::RinexSatMap::iterator it;
            for (it=rod.obs.begin(); it!=rod.obs.end(); it++) // PRN loop
	    {
               obsMatrix(currentRec,0)=year;
               obsMatrix(currentRec,1)=doy;
               obsMatrix(currentRec,2)=sod;
               obsMatrix(currentRec,3)=it->first.id;
               obsMatrix(currentRec,4)=it->first.system;
               // What to do with system information? sigh...a new matrix?

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

   return retval;
}
