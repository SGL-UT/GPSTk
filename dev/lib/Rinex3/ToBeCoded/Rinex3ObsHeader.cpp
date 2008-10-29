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

/**
 * @file Rinex3ObsHeader.cpp
 * Encapsulate header of Rinex3 observation file, including I/O
 */

#include "StringUtils.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsStream.hpp"

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{
   const string Rinex3ObsHeader::versionString =         "RINEX3 VERSION / TYPE";
   const string Rinex3ObsHeader::runByString  =          "PGM / RUN BY / DATE";
   const string Rinex3ObsHeader::commentString =         "COMMENT";
   const string Rinex3ObsHeader::markerNameString =      "MARKER NAME";
   const string Rinex3ObsHeader::markerNumberString =    "MARKER NUMBER";
   const string Rinex3ObsHeader::observerString =        "OBSERVER / AGENCY";
   const string Rinex3ObsHeader::receiverString =        "REC # / TYPE / VERS";
   const string Rinex3ObsHeader::antennaTypeString =     "ANT # / TYPE";
   const string Rinex3ObsHeader::antennaPositionString = "APPROX POSITION XYZ";
   const string Rinex3ObsHeader::antennaOffsetString =   "ANTENNA: DELTA H/E/N";
   const string Rinex3ObsHeader::antennaReferncePoint =  "ANTENNA: DELTA X/Y/Z";
   const string Rinex3ObsHeader::antennaPhaseCenter =    "ANTENNA: PHASECENTER";
   const string Rinex3ObsHeader::antennaBSight =         "ANTENNA: B.SIGHT XYZ";
   const string Rinex3ObsHeader::antennaZeroDirAZI =     "ANTENNA: ZERODIR AZI";
   const string Rinex3ObsHeader::antennaZeroDirXYZ =     "ANTENNA: ZERODIR XYZ";
   const string Rinex3ObsHeader::centerOfMass =          "CENTER OF MASS: XYZ";
   const string Rinex3ObsHeader::numObsString =          "SYS / # / OBS TYPES";
   const string Rinex3ObsHeader::signalStrengthUnit =    "SISNGAL STRENGTH UNIT";
   const string Rinex3ObsHeader::intervalString =        "INTERVAL";
   const string Rinex3ObsHeader::firstTimeString =       "TIME OF FIRST OBS";
   const string Rinex3ObsHeader::lastTimeString =        "TIME OF LAST OBS";
   const string Rinex3ObsHeader::receiverOffsetString =  "RCV CLOCK OFFS APPL";
   const string Rinex3ObsHeader::sysDCBS =               "SYS / DCBS APPLIED";
   const string Rinex3ObsHeader::sysPCVS =               "SYS / PCVS APPLIED";
   const string Rinex3ObsHeader::sysScaleFactor =        "SYS / SCALE FACTOR";
   const string Rinex3ObsHeader::leapSecondsString =     "LEAP SECONDS";
   const string Rinex3ObsHeader::numSatsString =         "# OF SATELLITES";
   const string Rinex3ObsHeader::prnObsString =          "PRN / # OF OBS";
   const string Rinex3ObsHeader::endOfHeader =           "END OF HEADER";

   //GPS
   const ObsID UN(ObsID::ObservationType::otUnknown, ObsID::CarrierBand::cbUnknown, ObsID::TrackingCode::tcUnknown);
   const ObsID C1C(ObsID::ObservationType::otRange, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcCA);
   const ObsID L1C(ObsID::ObservationType::otPhase, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcCA);
   const ObsID D1C(ObsID::ObservationType::otDoppler, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcCA);
   const ObsID S1C(ObsID::ObservationType::otSNR, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcCA);
   const ObsID C1P(ObsID::ObservationType::otRange, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcP);
   const ObsID L1P(ObsID::ObservationType::otPhase, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcP);
   const ObsID D1P(ObsID::ObservationType::otDoppler, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcP);
   const ObsID S1P(ObsID::ObservationType::otSNR, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcP);
   const ObsID C1W(ObsID::ObservationType::otRange, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcW);
   const ObsID L1W(ObsID::ObservationType::otPhase, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcW);
   const ObsID D1W(ObsID::ObservationType::otDoppler, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcW);
   const ObsID S1W(ObsID::ObservationType::otSNR, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcW);
   //And so on...


   //Galileo
   const ObsID C1A(ObsID::ObservationType::otRange, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcA);
   const ObsID L1A(ObsID::ObservationType::otPhase, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcA);
   const ObsID D1A(ObsID::ObservationType::otDoppler, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcA);
   const ObsID S1A(ObsID::ObservationType::otSNR, ObsID::CarrierBand::cbL1, ObsID::TrackingCode::tcA);
   //And so on...
















































// Warning: the size of the above sot array needs to be put
   // in this initializer.
   const std::vector<ObsID> Rinex3ObsHeader::StandardObsIDs(sot,sot+29);

   std::vector<Rinex3ObsHeader::ObsID> Rinex3ObsHeader::RegisteredObsIDs
      = Rinex3ObsHeader::StandardObsIDs;

   void Rinex3ObsHeader::reallyPutRecord(FFStream& ffs) const 
      throw(std::exception, FFStreamError, StringException)
   {
      Rinex3ObsStream& strm = dynamic_cast<Rinex3ObsStream&>(ffs);
      
      strm.header = *this;
      
      unsigned long allValid;
      if (version == 2.0)        allValid = allValid20;
      else if (version == 2.1)   allValid = allValid21;
      else if (version == 2.11)  allValid = allValid211;
      else
      {
         FFStreamError err("Unknown RINEX3 version: " + asString(version,2));
         err.addText("Make sure to set the version correctly.");
         GPSTK_THROW(err);
      }
      
      if ((valid & allValid) != allValid)
      {
         FFStreamError err("Incomplete or invalid header.");
         err.addText("Make sure you set all header valid bits for all of the available data.");
         GPSTK_THROW(err);
      }
      
      try
      {
         WriteHeaderRecords(strm);
      }
      catch(FFStreamError& e)
      {
         GPSTK_RETHROW(e);
      }
      catch(StringException& e)
      {
         GPSTK_RETHROW(e);
      }

   }  // end Rinex3ObsHeader::reallyPutRecord
      

      // this function computes the number of valid header records which WriteHeaderRecords will write
   int Rinex3ObsHeader::NumberHeaderRecordsToBeWritten(void) const throw()
   {
      int n=0;
      if(valid & Rinex3ObsHeader::versionValid) n++;
      if(valid & Rinex3ObsHeader::runByValid) n++;
      if(valid & Rinex3ObsHeader::markerNameValid) n++;
      if(valid & Rinex3ObsHeader::observerValid) n++;
      if(valid & Rinex3ObsHeader::receiverValid) n++;
      if(valid & Rinex3ObsHeader::antennaTypeValid) n++;
      if(valid & Rinex3ObsHeader::antennaPositionValid) n++;
      if(valid & Rinex3ObsHeader::antennaOffsetValid) n++;
      if(valid & Rinex3ObsHeader::waveFactValid) {
         n++;
         if(extraWaveFactList.size()) n += 1 + (extraWaveFactList.size()-1)/7;
      }
      if(valid & Rinex3ObsHeader::obsTypeValid) n += 1 + (obsTypeList.size()-1)/9;
      if(valid & Rinex3ObsHeader::intervalValid) n++;
      if(valid & Rinex3ObsHeader::firstTimeValid) n++;
      if(valid & Rinex3ObsHeader::lastTimeValid) n++;
      if(valid & Rinex3ObsHeader::markerNumberValid) n++;
      if(valid & Rinex3ObsHeader::receiverOffsetValid) n++;
      if(valid & Rinex3ObsHeader::leapSecondsValid) n++;
      if(valid & Rinex3ObsHeader::commentValid) n += commentList.size();
      if(valid & Rinex3ObsHeader::numSatsValid) n++;
      if(valid & Rinex3ObsHeader::prnObsValid)
         n += numObsForSat.size() * (1+numObsForSat.begin()->second.size()/9);
      if(valid & Rinex3ObsHeader::endValid) n++;
      return n;
   }

      // this function writes all valid header records
   void Rinex3ObsHeader::WriteHeaderRecords(FFStream& ffs) const
      throw(FFStreamError, StringException)
   {
      Rinex3ObsStream& strm = dynamic_cast<Rinex3ObsStream&>(ffs);
      string line;
      if (valid & versionValid)
      {
         line  = rightJustify(asString(version,2), 9);
         line += string(11, ' ');
         if ((fileType[0] != 'O') && (fileType[0] != 'o'))
         {
            FFStreamError err("This isn't a Rinex3 Observation file: " + 
                              fileType.substr(0,1));
            GPSTK_THROW(err);
         }

         if (system.system == Rinex3SatID::systemUnknown)
         {
            FFStreamError err("Invalid satellite system");
            GPSTK_THROW(err);
         }

         line += leftJustify(string("Observation"), 20);
         std::string str;
         str = system.systemChar();
         str = str + " (" + system.systemString() + ")";
         line += leftJustify(str, 20);
         line += versionString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & runByValid)
      {
         line  = leftJustify(fileProgram,20);
         line += leftJustify(fileAgency,20);
         CommonTime dt;
         dt.setLocalTime();
         string dat = dt.printf("%02m/%02d/%04Y %02H:%02M:%02S");
         line += leftJustify(dat, 20);
         line += runByString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & markerNameValid)
      {
         line  = leftJustify(markerName, 60);
         line += markerNameString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & observerValid)
      {
         line  = leftJustify(observer, 20);
         line += leftJustify(agency, 40);
         line += observerString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & receiverValid)
      {
         line  = leftJustify(recNo, 20);
         line += leftJustify(recType, 20);
         line += leftJustify(recVers, 20);
         line += receiverString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & antennaTypeValid)
      {
         line  = leftJustify(antNo, 20);
         line += leftJustify(antType, 20);
         line += string(20, ' ');
         line += antennaTypeString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & antennaPositionValid)
      {
         line  = rightJustify(asString(antennaPosition[0], 4), 14);
         line += rightJustify(asString(antennaPosition[1], 4), 14);
         line += rightJustify(asString(antennaPosition[2], 4), 14);
         line += string(18, ' ');
         line += antennaPositionString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & antennaOffsetValid)
      {
         line  = rightJustify(asString(antennaOffset[0], 4), 14);
         line += rightJustify(asString(antennaOffset[1], 4), 14);
         line += rightJustify(asString(antennaOffset[2], 4), 14);
         line += string(18, ' ');
         line += antennaOffsetString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & waveFactValid)
      {
         line  = rightJustify(asString<short>(wavelengthFactor[0]),6);
         line += rightJustify(asString<short>(wavelengthFactor[1]),6);
         line += string(48, ' ');
         line += waveFactString;
         strm << line << endl;
         strm.lineNumber++;
         
            // handle continuation lines
         if (!extraWaveFactList.empty())
         {
            vector<ExtraWaveFact>::const_iterator itr = extraWaveFactList.begin();
            
            while (itr != extraWaveFactList.end())
            {
               const int maxSatsPerLine = 7;
               short satsWritten = 0, satsLeft = (*itr).satList.size(), satsThisLine;
               vector<SatID>::const_iterator vecItr = (*itr).satList.begin();

               while ((vecItr != (*itr).satList.end())) {
                  if(satsWritten == 0) {
                     line  = rightJustify(asString<short>((*itr).wavelengthFactor[0]),6);
                     line += rightJustify(asString<short>((*itr).wavelengthFactor[1]),6);
                     satsThisLine = (satsLeft > maxSatsPerLine ? maxSatsPerLine : satsLeft);
                     line += rightJustify(asString<short>(satsThisLine),6);
                  }
                  try {
                     line += string(3, ' ') + Rinex3SatID(*vecItr).toString();
                  }
                  catch (Exception& e) {
                     FFStreamError ffse(e);
                     GPSTK_THROW(ffse);
                  }
                  satsWritten++;
                  satsLeft--;
                  if(satsWritten==maxSatsPerLine || satsLeft==0) {      // output a complete line
                     line += string(60 - line.size(), ' ');
                     line += waveFactString;
                     strm << line << endl;
                     strm.lineNumber++;
                     satsWritten = 0;
                  }
                  vecItr++;
               }
               itr++;
            }
         }
      }
      if (valid & obsTypeValid)
      {
         const int maxObsPerLine = 9;
         int obsWritten = 0;
         line = ""; // make sure the line contents are reset.
         
         vector<ObsID>::const_iterator itr = obsTypeList.begin();
         
         while (itr != obsTypeList.end())
         {
               // the first line needs to have the # of obs
            if (obsWritten == 0)
               line  = rightJustify(asString(obsTypeList.size()), 6);
               // if you hit 9, write out the line and start a new one
            else if ((obsWritten % maxObsPerLine) == 0)
            {
               line += numObsString;
               strm << line << endl;
               strm.lineNumber++;
               line  = string(6, ' ');
            }
            line += rightJustify(convertObsType(*itr), 6);
            obsWritten++;
            itr++;
         }
         line += string(60 - line.size(), ' ');
         line += numObsString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & intervalValid)
      {
         line  = rightJustify(asString(interval, 3), 10);
         line += string(50, ' ');
         line += intervalString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & firstTimeValid)
      {
         line  = writeTime(firstObs);
         line += string(48-line.size(),' ');
         if(firstSystem.system == Rinex3SatID::systemGPS) line += "GPS";
         if(firstSystem.system == Rinex3SatID::systemGlonass) line += "GLO";
         if(firstSystem.system == Rinex3SatID::systemGalileo) line += "GAL";
         line += string(60 - line.size(), ' ');
         line += firstTimeString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & lastTimeValid)
      {
         line  = writeTime(lastObs);
         line += string(48-line.size(),' ');
         if(lastSystem.system == Rinex3SatID::systemGPS) line += "GPS";
         if(lastSystem.system == Rinex3SatID::systemGlonass) line += "GLO";
         if(lastSystem.system == Rinex3SatID::systemGalileo) line += "GAL";
         line += string(60 - line.size(), ' ');
         line += lastTimeString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & markerNumberValid)
      {
         line  = leftJustify(markerNumber, 20);
         line += string(40, ' ');
         line += markerNumberString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & receiverOffsetValid)
      {
         line  = rightJustify(asString(receiverOffset),6);
         line += string(54, ' ');
         line += receiverOffsetString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & leapSecondsValid)
      {
         line  = rightJustify(asString(leapSeconds),6);
         line += string(54, ' ');
         line += leapSecondsString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & commentValid)
      {
         vector<string>::const_iterator itr = commentList.begin();
         while (itr != commentList.end())
         {
            line  = leftJustify((*itr), 60);
            line += commentString;
            strm << line << endl;
            strm.lineNumber++;
            itr++;
         }
      }
      if (valid & numSatsValid)
      {
         line  = rightJustify(asString(numSVs), 6);
         line += string(54, ' ');
         line += numSatsString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & prnObsValid)
      {
         const int maxObsPerLine = 9;
         map<SatID, vector<int> >::const_iterator itr = numObsForSat.begin();
         while (itr != numObsForSat.end())
         {
            int numObsWritten = 0;
            
            vector<int>::const_iterator vecItr = (*itr).second.begin();
            while (vecItr != (*itr).second.end())
            {
               if (numObsWritten == 0)
               {
                  try {
                     Rinex3SatID prn((*itr).first);
                     line  = string(3, ' ') + prn.toString();
                  }
                  catch (Exception& e) {
                     FFStreamError ffse(e);
                     GPSTK_RETHROW(ffse); 
                  }
               }
               else if ((numObsWritten % maxObsPerLine)  == 0)
               {
                  line += prnObsString;
                  strm << line << endl;
                  strm.lineNumber++;
                  line  = string(6, ' ');
               }
               line += rightJustify(asString(*vecItr), 6);
               ++vecItr;
               ++numObsWritten;
            }
            line += string(60 - line.size(), ' ');
            line += prnObsString;
            strm << line << endl;
            strm.lineNumber++;
            itr++;
         }
      }
      if (valid & endValid)
      {
         line  = string(60, ' ');
         line += endOfHeader;
         strm << line << endl;
         strm.lineNumber++;               
      }   
   }   // end Rinex3ObsHeader::WriteHeaderRecords()


      // this function parses a single header record
   void Rinex3ObsHeader::ParseHeaderRecord(string& line)
      throw(FFStreamError)
   {
      string label(line, 60, 20);
         
      if (label == versionString)
      {
         version = asDouble(line.substr(0,20));
         fileType = strip(line.substr(20, 20));
         if ( (fileType[0] != 'O') &&
              (fileType[0] != 'o'))
         {
            FFStreamError e("This isn't a Rinex3 Obs file");
            GPSTK_THROW(e);
         }
         string system_str = strip(line.substr(40, 20));
         try {
            system.fromString(system_str);
         }
         catch (Exception& e)
         {
            FFStreamError ffse("Input satellite system is unsupported: " + system_str);
            GPSTK_THROW(ffse);
         }
         valid |= versionValid;
      }
      else if (label == runByString )
      {
         fileProgram =    strip(line.substr(0, 20));
         fileAgency =  strip(line.substr(20, 20));
         date =   strip(line.substr(40, 20));
         valid |= runByValid;
      }
      else if (label == commentString)
      {
         string s = strip(line.substr(0, 60));
         commentList.push_back(s);
         valid |= commentValid;
      }
      else if (label == markerNameString)
      {
         markerName = strip(line.substr(0,60));
         valid |= markerNameValid;
      }
      else if (label == markerNumberString)
      {
         markerNumber = strip(line.substr(0,20));
         valid |= markerNumberValid;
      }
      else if (label == observerString)
      {
         observer = strip(line.substr(0,20));
         agency = strip(line.substr(20,40));
         valid |= observerValid;
      }
      else if (label == receiverString)
      {
         recNo   = strip(line.substr(0, 20));
         recType = strip(line.substr(20,20));
         recVers = strip(line.substr(40,20));
         valid |= receiverValid;
      }
      else if (label ==antennaTypeString)
      {
         antNo =   strip(line.substr(0, 20));
         antType = strip(line.substr(20, 20));
         valid |= antennaTypeValid;
      }
      else if (label == antennaPositionString)
      {
         antennaPosition[0] = asDouble(line.substr(0,  14));
         antennaPosition[1] = asDouble(line.substr(14, 14));
         antennaPosition[2] = asDouble(line.substr(28, 14));
         valid |= antennaPositionValid;
      }
      else if (label == antennaOffsetString)
      {
         antennaOffset[0] = asDouble(line.substr(0,  14));
         antennaOffset[1] = asDouble(line.substr(14, 14));
         antennaOffset[2] = asDouble(line.substr(28, 14));
         valid |= antennaOffsetValid;
      }
      else if (label == waveFactString)
      {
            // first time reading this
         if (! (valid & waveFactValid))
         {
            wavelengthFactor[0] = asInt(line.substr(0,6));
            wavelengthFactor[1] = asInt(line.substr(6,6));
            valid |= waveFactValid;
         }
            // additional wave fact lines
         else
         {
            const int maxSatsPerLine = 7;
            int Nsats;
            ExtraWaveFact ewf;
            ewf.wavelengthFactor[0] = asInt(line.substr(0,6));
            ewf.wavelengthFactor[1] = asInt(line.substr(6,6));
            Nsats = asInt(line.substr(12,6));
               
            if (Nsats > maxSatsPerLine)   // > not >=
            {
               FFStreamError e("Invalid number of Sats for " + waveFactString);
               GPSTK_THROW(e);
            }
               
            for (int i = 0; i < Nsats; i++)
            {
               try {
                  Rinex3SatID prn(line.substr(21+i*6,3));
                  ewf.satList.push_back(prn); 
               }
               catch (Exception& e){
                  FFStreamError ffse(e);
                  GPSTK_RETHROW(ffse);
               }
            }
               
            extraWaveFactList.push_back(ewf);
         }
      }
      else if (label == numObsString)
      {
         const int maxObsPerLine = 9;
            // process the first line
         if (! (valid & obsTypeValid))
         {
            numObs = asInt(line.substr(0,6));
            
            for (int i = 0; (i < numObs) && (i < maxObsPerLine); i++)
            {
               int position = i * 6 + 6 + 4;
               ObsID rt = convertObsType(line.substr(position,2));
               obsTypeList.push_back(rt);
            }
            valid |= obsTypeValid;
         }
            // process continuation lines
         else
         {
            for (int i = obsTypeList.size();
                 (i < numObs) && ( (i % maxObsPerLine) < maxObsPerLine); i++)
            {
               int position = (i % maxObsPerLine) * 6 + 6 + 4;
               ObsID rt = convertObsType(line.substr(position,2));
               obsTypeList.push_back(rt);
            }
         }
      }
      else if (label == intervalString)
      {
         interval = asDouble(line.substr(0, 10));
         valid |= intervalValid;
      }
      else if (label == firstTimeString)
      {
         firstObs = parseTime(line);
         firstSystem.system = Rinex3SatID::systemGPS;
         if(line.substr(48,3)=="GLO") firstSystem.system=Rinex3SatID::systemGlonass;
         if(line.substr(48,3)=="GAL") firstSystem.system=Rinex3SatID::systemGalileo;
         valid |= firstTimeValid;
      }
      else if (label == lastTimeString)
      {
         lastObs = parseTime(line);
         lastSystem.system = Rinex3SatID::systemGPS;
         if(line.substr(48,3)=="GLO") lastSystem.system=Rinex3SatID::systemGlonass;
         if(line.substr(48,3)=="GAL") lastSystem.system=Rinex3SatID::systemGalileo;
         valid |= lastTimeValid;
      }
      else if (label == receiverOffsetString)
      {
         receiverOffset = asInt(line.substr(0,6));
         valid |= receiverOffsetValid;
      }
      else if (label == leapSecondsString)
      {
         leapSeconds = asInt(line.substr(0,6));
         valid |= leapSecondsValid;
      }
      else if (label == numSatsString)
      {
         numSVs = asInt(line.substr(0,6)) ;
         valid |= numSatsValid;
      }
      else if (label == prnObsString)
      {
         const int maxObsPerLine = 9;
            // continuation lines... you have to know what PRN
            // this is continuing for, hence lastPRN
         if ((lastPRN.id != -1) &&
             (numObsForSat[lastPRN].size() != obsTypeList.size()))
         {
            for(int i = numObsForSat[lastPRN].size(); 
                (i < obsTypeList.size()) && 
                   ( (i % maxObsPerLine) < maxObsPerLine); i++)
            {
               numObsForSat[lastPRN].push_back(asInt(line.substr((i%maxObsPerLine)*6+6,6)));
            }
         }
         else
         {
            try { 
               lastPRN.fromString(line.substr(3,3));
            }
            catch (Exception& e) {
               FFStreamError ffse(e);
               GPSTK_RETHROW(ffse);
            }
            vector<int> numObsList;
            for(int i = 0; 
                   (i < obsTypeList.size()) && (i < maxObsPerLine); i++)
            {
               numObsList.push_back(asInt(line.substr(i*6+6,6)));
            }

            numObsForSat[lastPRN] = numObsList;
         }
         valid |= prnObsValid;
      }
      else if (label == endOfHeader)
      {
         valid |= endValid;
      }
      else
      {
         FFStreamError e("Unidentified label: " + label);
         GPSTK_THROW(e);
      }
   }   // end of Rinex3ObsHeader::ParseHeaderRecord(string& line)


      // This function parses the entire header from the given stream
   void Rinex3ObsHeader::reallyGetRecord(FFStream& ffs)
      throw(std::exception, FFStreamError, 
            gpstk::StringUtils::StringException)
   {
      Rinex3ObsStream& strm = dynamic_cast<Rinex3ObsStream&>(ffs);
      
         // if already read, just return
      if (strm.headerRead == true)
         return;

         // since we're reading a new header, we need to reinitialize
         // all our list structures.  all the other objects should be ok.
         // this also applies if we threw an exception the first time we read
         // the header and are now re-reading it. some of these could be full
         // and we need to empty them.
      commentList.clear();
      wavelengthFactor[0] = wavelengthFactor[1] = 1;
      extraWaveFactList.clear();
      obsTypeList.clear();
      numObsForSat.clear();
      valid = 0;
      numObs = 0;
      lastPRN.id = -1;
      
      string line;
      
      while (!(valid & endValid))
      {
         strm.formattedGetLine(line);
         StringUtils::stripTrailing(line);

         if (line.length()==0)
         {
            FFStreamError e("No data read");
            GPSTK_THROW(e);
         }
         else if (line.length()<60 || line.length()>80)
         {
            FFStreamError e("Invalid line length");
            GPSTK_THROW(e);
         }

         try
         {
            ParseHeaderRecord(line);
         }
         catch(FFStreamError& e)
         {
            GPSTK_RETHROW(e);
         }
         
      }   // end while(not end of header)

      unsigned long allValid;
      if      (version == 2.0)      allValid = allValid20;
      else if (version == 2.1)      allValid = allValid21;
      else if (version == 2.11)     allValid = allValid211;
      else
      {
         FFStreamError e("Unknown or unsupported RINEX3 version " + 
                         asString(version));
         GPSTK_THROW(e);
      }
            
      if ( (allValid & valid) != allValid)
      {
         FFStreamError e("Incomplete or invalid header");
         GPSTK_THROW(e);               
      }
            
         // If we get here, we should have reached the end of header line
      strm.header = *this;
      strm.headerRead = true;
            
   }  // end of reallyGetRecord()



   Rinex3ObsHeader::ObsID 
   Rinex3ObsHeader::convertObsType(const string& oneObs)
      throw(FFStreamError)
   {
      ObsID ot(RegisteredObsIDs[0]);   // Unknown type
      for(int i=0; i<RegisteredObsIDs.size(); i++) {
         if(RegisteredObsIDs[i].type == oneObs) {
            ot = RegisteredObsIDs[i];
            break;
         }
         //FFStreamError e("Bad obs type: " + oneObs);
         //GPSTK_THROW(e);
      }
      return ot;
   }
   string 
   Rinex3ObsHeader::convertObsType(const Rinex3ObsHeader::ObsID& oneObs)
      throw(FFStreamError)
   {
      return oneObs.type;
   }


   CommonTime Rinex3ObsHeader::parseTime(const string& line) const
   {
      int year, month, day, hour, min;
      double sec;
   
      year  = asInt(   line.substr(0,  6 ));
      month = asInt(   line.substr(6,  6 ));
      day   = asInt(   line.substr(12, 6 ));
      hour  = asInt(   line.substr(18, 6 ));
      min   = asInt(   line.substr(24, 6 ));
      sec   = asDouble(line.substr(30, 13));
      return CommonTime(year, month, day, hour, min, sec);
   }

   string Rinex3ObsHeader::writeTime(const CommonTime& dt) const
   {
      string line;
      line  = rightJustify(asString<short>(dt.year()), 6);
      line += rightJustify(asString<short>(dt.month()), 6);
      line += rightJustify(asString<short>(dt.day()), 6);
      line += rightJustify(asString<short>(dt.hour()), 6);
      line += rightJustify(asString<short>(dt.minute()), 6);
      line += rightJustify(asString(dt.second(), 7), 13);
      return line;
   }

   void Rinex3ObsHeader::dump(ostream& s) const
   {
      int i,j;
      s << "---------------------------------- REQUIRED ----------------------------------\n";
      string str;
      str = system.systemChar();
      str = str + " (" + system.systemString() + ")";
      s << "Rinex3 Version " << fixed << setw(5) << setprecision(2) << version
         << ",  File type " << fileType << ",  System " << str << ".\n";
      s << "Prgm: " << fileProgram << ",  Run: " << date << ",  By: " << fileAgency << endl;
      s << "Marker name: " << markerName << ".\n";
      s << "Obs'r : " << observer << ",  Agency: " << agency << endl;
      s << "Rec#: " << recNo << ",  Type: " << recType << ",  Vers: " << recVers << endl;
      s << "Antenna # : " << antNo << ",  Type : " << antType << endl;
      s << "Position (XYZ,m) : " << setprecision(4) << antennaPosition << ".\n";
      s << "Antenna offset (ENU,m) : " << setprecision(4) << antennaOffset << ".\n";
      s << "Wavelength factors (default) L1:" << wavelengthFactor[0]
         << ", L2: " << wavelengthFactor[1] << ".\n";
      for(i=0; i<extraWaveFactList.size(); i++) {
         s << "Wavelength factors (extra)   L1:"
            << extraWaveFactList[i].wavelengthFactor[0]
            << ", L2: " << extraWaveFactList[i].wavelengthFactor[1]
            << ", for Sats";
         for(j=0; j<extraWaveFactList[i].satList.size(); j++)
            s << " " << extraWaveFactList[i].satList[j];
         s << endl;
      }
      s << "Observation types (" << obsTypeList.size() << ") :\n";
      for(i=0; i<obsTypeList.size(); i++) 
         s << " Type #" << i << " = "
            << gpstk::Rinex3ObsHeader::convertObsType(obsTypeList[i])
            << " " << obsTypeList[i].description
            << " (" << obsTypeList[i].units << ")." << endl;
      s << "Time of first obs " << firstObs.printf("%04Y/%02m/%02d %02H:%02M:%010.7f")
         << " " << (firstSystem.system==Rinex3SatID::systemGlonass ? "GLO" :
                   (firstSystem.system==Rinex3SatID::systemGalileo ? "GAL" : "GPS")) << endl;
      s << "(This header is ";
      if((valid & allValid211) == allValid211) s << "VALID 2.11";
      else if((valid & allValid21) == allValid21) s << "VALID 2.1";
      else if((valid & allValid20) == allValid20) s << "VALID 2.0";
      else s << "NOT VALID";
      s << " Rinex3.)\n";

      if(!(valid & versionValid)) s << " Version is NOT valid\n";
      if(!(valid & runByValid)) s << " Run by is NOT valid\n";
      if(!(valid & markerNameValid)) s << " Marker Name is NOT valid\n";
      if(!(valid & observerValid)) s << " Observer is NOT valid\n";
      if(!(valid & receiverValid)) s << " Receiver is NOT valid\n";
      if(!(valid & antennaTypeValid)) s << " Antenna Type is NOT valid\n";
      if(!(valid & antennaPositionValid)) s << " Ant Position is NOT valid\n";
      if(!(valid & antennaOffsetValid)) s << " Antenna Offset is NOT valid\n";
      if(!(valid & waveFactValid)) s << " Wavelength factor is NOT valid\n";
      if(!(valid & obsTypeValid)) s << " Obs Type is NOT valid\n";
      if(!(valid & firstTimeValid)) s << " First time is NOT valid\n";
      if(!(valid & endValid)) s << " End is NOT valid\n";

      s << "---------------------------------- OPTIONAL ----------------------------------\n";
      if(valid & markerNumberValid) s << "Marker number : " << markerNumber << endl;
      if(valid & intervalValid) s << "Interval = "
         << fixed << setw(7) << setprecision(3) << interval << endl;
      if(valid & lastTimeValid) s << "Time of last obs "
         << lastObs.printf("%04Y/%02m/%02d %02H:%02M:%010.7f")
         << " " << (lastSystem.system==Rinex3SatID::systemGlonass ? "GLO":
                   (lastSystem.system==Rinex3SatID::systemGalileo ? "GAL" : "GPS")) << endl;
      if(valid & leapSecondsValid) s << "Leap seconds: " << leapSeconds << endl;
      if(valid & receiverOffsetValid) s << "Clock offset record is present and offsets "
         << (receiverOffset?"ARE":"are NOT") << " applied." << endl;
      if(valid & numSatsValid) s << "Number of Satellites with data : " << numSVs << endl;
      if(valid & prnObsValid) {
         s << "SAT  ";
         for(i=0; i<obsTypeList.size(); i++)
            s << setw(7) << convertObsType(obsTypeList[i]);
         s << endl;
         map<SatID, vector<int> >::const_iterator sat_itr = numObsForSat.begin();
         while (sat_itr != numObsForSat.end()) {
            vector<int> obsvec=sat_itr->second;
            s << " " << Rinex3SatID(sat_itr->first) << " ";
            for(i=0; i<obsvec.size(); i++) s << " " << setw(6) << obsvec[i];
            s << endl;
            sat_itr++;
         }
      }
      if(commentList.size() && !(valid & commentValid)) s << " Comment is NOT valid\n";
      s << "Comments (" << commentList.size() << ") :\n";
      for(i=0; i<commentList.size(); i++)
         s << commentList[i] << endl;
      s << "-------------------------------- END OF HEADER -------------------------------\n";
   }

   // return 1 if type already defined,
   //        0 if successful
   //       -1 if not successful - invalid input
   int RegisterExtendedObsID(string t, string d, string u, unsigned int dep)
   {
      if(t.empty()) return -1;
      // throw if t="UN" ?
      // check that it is not duplicated
      for(int i=0; i<Rinex3ObsHeader::RegisteredObsIDs.size(); i++) {
         if(Rinex3ObsHeader::RegisteredObsIDs[i].type == t) { return 1; }
      }
      Rinex3ObsHeader::ObsID ot;
      if(t.size()>2) t.resize(2,' '); ot.type = stripTrailing(t);
      if(d.size()>20) d.resize(20,' '); ot.description = stripTrailing(d);
      if(u.size()>10) u.resize(10,' '); ot.units = stripTrailing(u);
      ot.depend = dep;
      Rinex3ObsHeader::RegisteredObsIDs.push_back(ot);
      return 0;
   }

      // Pretty print a list of standard Rinex3 observation types
   void DisplayStandardObsIDs(ostream& s)
   {
      s << "The list of standard Rinex3 obs types:\n";
      s << "  OT Description          Units\n";
      s << "  -- -------------------- ---------\n";
      for(int i=0; i<Rinex3ObsHeader::StandardObsIDs.size(); i++) {
         string line;
         line = string("  ")+Rinex3ObsHeader::StandardObsIDs[i].type;
         line += leftJustify(string(" ")+Rinex3ObsHeader::StandardObsIDs[i].description,21);
         line += leftJustify(string(" ")+Rinex3ObsHeader::StandardObsIDs[i].units,11);
         s << line << endl;
      }
   }

      // Pretty print a list of registered extended Rinex3 observation types
   void DisplayExtendedObsIDs(ostream& s)
   {
      s << "The list of available extended Rinex3 obs types:\n";
      s << "  OT Description          Units     Required input (EP=ephemeris,PS=Rx Position)\n";
      s << "  -- -------------------- --------- ------------------\n";
      for(int i=Rinex3ObsHeader::StandardObsIDs.size();
               i<Rinex3ObsHeader::RegisteredObsIDs.size(); i++) {
         string line;
         line = string("  ")+Rinex3ObsHeader::RegisteredObsIDs[i].type;
         line += leftJustify(string(" ")+Rinex3ObsHeader::RegisteredObsIDs[i].description,21);
         line += leftJustify(string(" ")+Rinex3ObsHeader::RegisteredObsIDs[i].units,11);
         for(int j=1; j<=6; j++) {
            if(j==3 || j==4) continue;
            if(Rinex3ObsHeader::RegisteredObsIDs[i].depend &
               Rinex3ObsHeader::StandardObsIDs[j].depend)
                  line += string(" ")+Rinex3ObsHeader::StandardObsIDs[j].type;
            else line += string("   ");
         }
         if(Rinex3ObsHeader::RegisteredObsIDs[i].depend & Rinex3ObsHeader::ObsID::EPdepend)
            line += string(" EP"); else line += string("   ");
         if(Rinex3ObsHeader::RegisteredObsIDs[i].depend & Rinex3ObsHeader::ObsID::PSdepend)
            line += string(" PS"); else line += string("   ");
         s << line << endl;
      }
   }

} // namespace gpstk
