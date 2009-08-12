gpstk;

file = "bahr1620.04o"
roffs=gpstk.RinexObsStream(file);

roh=gpstk.RinexObsHeader();
rod=gpstk.RInexObsData();

roh.getRecord(roffs);
check=roh.isValid();

if (check=1)

	if (bitand(roh.valid,gpstk.RinexObsHeader_versionValid)>0) 
 	headerStruct.rinex_version_type=roh.version;
	end

	if (bitand(roh.valid,gpstk.RinexObsHeader_runByValid)>0)
	headerStruct.pgm=roh.fileProgram;
	headerStruct.run_by=roh.fileAgency;
	headerStruct.date=roh.date;
	end 

	if (bitand(roh.valid,gpstk.RinexObsHeader_markerNameValid)>0)
	headerStruct.marker_name=roh.markerName;
	end 

	if (bitand(roh.valid,gpstk.RinexObsHeader_markerNumberValid)>0)
	headerStruct.marker_number=roh.markerNumber;
	end

	if (bitand(roh.valid,gpstk.RinexObsHeader_commentValid)>0)
	/*Octave_map commentmap;
             
	     vector<string>::iterator i;
	     int n=0;
	     for (i = roh.commentList.begin();
                  i!=roh.commentList.end();
	    	 i++,n++) 
             commentmap.assign(gpstk::StringUtils::asString(n),*i);

             headerStruct.assign("comments",commentmap);
	*/
	headerStruct.comments=commentmap;
	end

	if (bitand(roh.header,gpstk.RinexObsHeader_observerValid)>0)
	headerStruct.observer=roh.observer;
	end

	if (bitand(roh.valid, gpstk.RinexObsHeader_receiverValid)>0)
	headerStruct.receiver_number=roh.recNo;
	headerStruct.receiver_type=roh.recType;
	headerStruct.receiver_version=roh.recVers;
	end

	if (bitand(roh.valid, gpstk.RinexObsHeader_antennaTypeValid)>0)
	headerStruct.antenna_number=roh.antNo;
	headerStruct.antenna_type=antType;
	end

	if (bitand(roh.valid, gpstk.RinexObsHeader_antennaPositionValid)>0)
	/*  aPos(0) = roh.antennaPosition[0];
            aPos(1) = roh.antennaPosition[1];
            aPos(2) = roh.antennaPosition[2];
	*/
	aPos = ones(3,1);
	aPos(1) =
	aPos(2) =
	aPos(3) =
	headerStruct.antenna_position=aPos;
	end

	if (bitand(roh.valid, gpstk.RinexObsHeader_antennOffsetValid)>0)
	/* aOff(0) = roh.antennaOffset[0];
	   aOff(1) = roh.antennaOffset[1];
	   aOff(2) = roh.antennaOffset[2];
	*/
	aOff = ones(3,1)
	aOff(1) = 
	aOff(2) = 
	aOff(3) = 
	headerStruct.antenna_offset=aOff;
	end

	if (bitand(roh.valid, gpstk.RinexObsHeader_waveFactValid)>0)
	/* waveFact(0) = static_cast<short> (roh.wavelengthFactor[0]);
	   waveFact(1) = static_cast<short> (roh.wavelengthFactor[1]);
	*/
	waveFact = ones(2,1)
	waveFact(1) = 
	waveFact(2) = 
	headerStruct.wavelength_factor = waveFact;
	end
	/*
	if (!roh.extraWaveFactList.empty())
	 {
            Matrix waveFactors(0,0);          						 **octave code: waveFactors = [];
	    int nprnsTot = 0;		       						 **	        nprnsTot = 0;
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
	*/
	if (bitand(roh.valid, gpstk.RinexObsHeader_obsTypeValid)>0)
	/*std::string obsList;
            for (int i=0; i<roh.obsTypeList.size(); i++)
	    {
	      obsList += roh.obsTypeList[i].type + string(" ");
	    }
	    headerStruct.assign("obs_types",obsList);

            numObsTypes = roh.obsTypeList.size();  
	*/
	end

	if (bitand(roh.valid, gpstk.RinexObsHeader_intervalValid)>0)
	headerStruct.interval = roh.interval;
	end

	if (bitand(roh.valid, gpstk.RinexObsHeader_firstTimeValid)>0)
	firstTime = ones(3,1);
	firstTime(1) = roh.firstObs.DOYyear();
	firstTime(2) = roh.firstObs.DOYday();
	firstTime(3) = roh.firstObs.DOYsecond();
	headerStruct.time_of_first_obs = firstTime;
	end

	if (bitand(roh.valid, gpstk.RinexObsHeader_lastTimeValid)>0)
	lastTime = ones(3,1);
	lastTime(1) = roh.lastObs.DOYyear();
	lastTime(2) = roh.lastObs.DOYday();
	lastTime(3) = roh.lastObs.Doysecond();
	headerStruct.time_of_last_obs = lastTime;
	end

	if (bitand(roh.valid, gpstk.RinexObsHeader_receiverOffsetValid)>0)
	headerStruct.receiver_offset = roh.receiverOffset;
	end

	if (bitand(roh.valid, gpstk.RinexObsHeader_leapSecondsValid)>0)
	headerStruct.leap_seconds = leapSeconds;	
	end

	if ((bitand(roh.valid, gpstk.RinexObsHeader_numSatsValid)>0)
	headerStruct.numSVs = roh.numSVs;
	end

	if ((bitand(roh.valid, gpstk.RinexObsHeader_prnObsValid)>0)
	/*Matrix prnObs(roh.numObsForSat.size(),numObsTypes+2);
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
	  }*/
	end

end

old = 0;
new = 1;
nrecs = 0;

while (new > old)
old = roffs.recordNumber();
rod.getRecord(roffs);
nrecs = nrecs + rod.numSvs;
new = roffs.recordNumber();
end


//obsMatrix.resize(nrecs,numObsTypes+5,0);
//second scan
roffs=gpstk.RinexObsStream(file);
roh.getRecord(roffs2);


/*

    while (roffs2>>rod) // Loop through each epoch						**while (new2 > old2)
      {			
         // Apply editing criteria (again)		
         if  (rod.epochFlag == 0 || rod.epochFlag == 1) // Begin usable data			**	if (rod.epochFlag == 0 || rod.epochFlag == 1)
	 {
            short year = rod.time.DOYyear();							**		year = rod.time.DOYyear();
            short doy  = rod.time.DOYday();							**		doy  = rod.time.DOYday();
	    double sod = rod.time.DOYsecond();							**		sod  = rod.time.DOYday();

	    gpstk::RinexObsData::RinexSatMap::iterator it; 					**	
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
                 
	       } 
               currentRec++;
	    } 

	 } 

      } 
  
  

   octave_value_list retval;
   retval(0) = octave_value( headerStruct);
   retval(1) = octave_value( obsMatrix);

   return retval;

*/



