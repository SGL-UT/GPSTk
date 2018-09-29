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
 * @file Rinex3ClockHeader.cpp
 * Encapsulate header of RINEX Clock file header data, including I/O
 */

#include "StringUtils.hpp"
#include "CommonTime.hpp"
#include "SystemTime.hpp"
#include "Rinex3ClockStream.hpp"
#include "Rinex3ClockHeader.hpp"
#include "TimeString.hpp"

#define debug 0

using namespace std;

namespace gpstk
{
   using namespace StringUtils;

   const string Rinex3ClockHeader::versionString =        "RINEX VERSION / TYPE";
   const string Rinex3ClockHeader::runByString =          "PGM / RUN BY / DATE";
   const string Rinex3ClockHeader::commentString =        "COMMENT";
   const string Rinex3ClockHeader::sysString =            "SYS / # / OBS TYPES";
   const string Rinex3ClockHeader::timeSystemString =     "TIME SYSTEM ID";
   const string Rinex3ClockHeader::leapSecondsString =    "LEAP SECONDS";
   const string Rinex3ClockHeader::sysDCBString =         "SYS / DCBS APPLIED";
   const string Rinex3ClockHeader::sysPCVString =         "SYS / PCVS APPLIED";
   const string Rinex3ClockHeader::numDataString =        "# / TYPES OF DATA";
   const string Rinex3ClockHeader::stationNameString =    "STATION NAME / NUM";
   const string Rinex3ClockHeader::stationClockRefString ="STATION CLK REF";
   const string Rinex3ClockHeader::analysisCenterString = "ANALYSIS CENTER";
   const string Rinex3ClockHeader::numClockRefString =    "# OF CLK REF";
   const string Rinex3ClockHeader::analysisClkRefrString ="ANALYSIS CLK REF";
   const string Rinex3ClockHeader::numReceiversString =   "# OF SOLN STA / TRF";
   const string Rinex3ClockHeader::solnStateString =      "SOLN STA NAME / NUM";
   const string Rinex3ClockHeader::numSolnSatsString =    "# OF SOLN SATS";
   const string Rinex3ClockHeader::prnListString =        "PRN LIST";
   const string Rinex3ClockHeader::endOfHeaderString =    "END OF HEADER";

      // --------------------------------------------------------------------------------
   void Rinex3ClockHeader::reallyGetRecord(FFStream& ffs)
      throw(exception, FFStreamError, StringException)
   {
      Rinex3ClockStream& strm = dynamic_cast<Rinex3ClockStream&>(ffs);
      
         // if header is already read, just return
      if(strm.headerRead) return;

         // clear the storage
      clear();

      string line;
      while(!(valid & endOfHeaderValid)) {
            // get a line
         strm.formattedGetLine(line);
         stripTrailing(line);

         if(debug) cout << "Rinex3Clock Header Line " << line << endl;

         if(line.length() == 0) continue;
         else if(line.length() < 60 || line.length() > 80) {
            FFStreamError e("Invalid line length");
            GPSTK_THROW(e);
         }

            // parse the line
         try {
            string label(line, 60, 20);
            if(label == versionString) {
               version = asDouble(line.substr(0,9));
               if(line[20] != 'C') {
                  FFStreamError e("Invalid file type: " + line.substr(20,1));
                  GPSTK_THROW(e);
               }
               fileSys = strip(line.substr(35,20));
               valid |= versionValid;
            }
            else if(label == runByString) {
               program = strip(line.substr(0,20));
               runby = strip(line.substr(20,20));
                  //date = strip(line.substr(40,20));
               valid |= runByValid;
            }
            else if(label == commentString) {
               commentList.push_back(strip(line.substr(0,60)));
               valid |= commentValid;
            }
            else if(label == sysString) {
               string satSys = strip(line.substr(0,1));
               if (satSys != "")
               {
                  numObs = asInt(line.substr(3,3));
                  satSysPrev = satSys;
               }
               else
                  satSys = satSysPrev;

               try
               {
                  const int maxObsPerLine = 13;
                  for (int i=0; i < maxObsPerLine && sysObsTypes[satSys].size() < numObs; i++)
                     sysObsTypes[satSys].push_back(RinexObsID(satSys+line.substr(4 * i + 7, 3)));
               }
               catch(InvalidParameter& ip)
               {
                  FFStreamError fse("InvalidParameter: "+ip.what());
                  GPSTK_THROW(fse);
               }
               valid |= sysValid;
            }
            else if(label == timeSystemString) {
               string ts(upperCase(line.substr(3,3)));
               timeSystem.fromString(ts);
               valid |= timeSystemValid;
            }
            else if(label == leapSecondsString) {
               leapSeconds = asInt(line.substr(0,6));
               valid |= leapSecondsValid;
            }
            else if(label == sysDCBString) {
               if(line[0] == 'G' || line[0] == 'R')
                  dcbsMap[line.substr(0,1)] = stringPair(strip(line.substr(1,17)),strip(line.substr(20,40)));
               else {
                  FFStreamError e("Invalid dcbs system : " + line.substr(0,1));
                  GPSTK_THROW(e);
               }
               valid |= sysDCBValid;
            }
            else if(label == sysPCVString) {
               if(line[0] == 'G' || line[0] == 'R')
                  pcvsMap[line.substr(0,1)] = stringPair(strip(line.substr(1,17)),strip(line.substr(20,40)));
               else {
                  FFStreamError e("Invalid pcvs system : " + line.substr(0,1));
                  GPSTK_THROW(e);
               }
               valid |= sysPCVValid;
            }
            else if(label == numDataString) {
               int n(asInt(line.substr(0,6)));
               for(int i=0; i<n; ++i)
                  dataTypes.push_back(line.substr(10+i*6,2));
               valid |= numDataValid;
            }
            else if(label == stationNameString) {
                  //string label(strip(line.substr(0,4)));
                  //stationID[label] = strip(line.substr(5,20));
               valid |= stationNameValid;
            }
            else if(label == stationClockRefString) {
               valid |= stationClockRefValid;
            }
            else if(label == analysisCenterString) {
               analCenterDesignator = strip(line.substr(0,3));
               analysisCenter = strip(line.substr(5,55));
               valid |= analysisCenterValid;
            }
            else if(label == numClockRefString) {
               valid |= numClockRefValid;
            }
            else if(label == analysisClkRefrString) {
               valid |= analysisClkRefrValid;
            }
            else if(label == numReceiversString) {
               numSolnStations = asInt(line.substr(0,6));
               terrRefFrame = strip(line.substr(10,50));
               valid |= numReceiversValid;
            }
            else if(label == solnStateString) {
               string label(strip(line.substr(0,4)));
               stationID[label] = strip(line.substr(5,20));
               stationX[label] = strip(line.substr(25,11));
               stationY[label] = strip(line.substr(37,11));
               stationZ[label] = strip(line.substr(49,11));
               valid |= solnStateValid;
            }
            else if(label == numSolnSatsString) {
               numSolnSatellites = asInt(line.substr(0,6));
               valid |= numSolnSatsValid;
            }
            else if(label == prnListString) {
               int i,prn;
               string label;
               for(i=0; i<15; ++i) {
                  label = line.substr(4*i,3);
                  if(label == string("   ")) break;
                  prn = asInt(line.substr(4*i+1,2));
                  if(line[4*i] == 'G')
                     satList.push_back(RinexSatID(prn,RinexSatID::systemGPS));
                  else if(line[4*i] == 'R')
                     satList.push_back(RinexSatID(prn,RinexSatID::systemGlonass));
                  else {
                     FFStreamError e("Invalid sat (PRN LIST): /" + label + "/");
                     GPSTK_THROW(e);
                  }
               }
                  // @TODO how to check numSolnSatsValid == satList.size() ?
               valid |= prnListValid;
            }
            else if(label == endOfHeaderString) {
               valid |= endOfHeaderValid;
            }
            else {
               FFStreamError e("Invalid line label: " + label);
               GPSTK_THROW(e);
            }

            if(debug) cout << "Valid is " << hex << valid << fixed << endl;

         }  // end parsing the line
         catch(FFStreamError& e) { GPSTK_RETHROW(e); }

      }  // end while end-of-header not found

      if(debug) cout << "Header read; Valid is " << hex << valid << fixed << endl;

         // is this header valid?
      if( (valid & allRequiredValid) != allRequiredValid) {
         cout << "Header is invalid on input (valid is x" << hex << valid
              << dec << ").\n";
         dumpValid(cout);
         FFStreamError e("Invalid header");
         GPSTK_THROW(e);
      }

      strm.headerRead  = true;

   }  // end Rinex3ClockHeader::reallyGetRecord()


   void Rinex3ClockHeader::reallyPutRecord(FFStream& ffs) const
      throw(exception, FFStreamError, StringException)
   {
      try {
         Rinex3ClockStream& strm = dynamic_cast<Rinex3ClockStream&>(ffs);

            // is this header valid?
         if( (valid & allRequiredValid) != allRequiredValid) {
            FFStreamError e("Invalid header");
            GPSTK_THROW(e);
         }

         size_t i;
         string line;
         try {
            line = rightJustify(asString(version,2), 9);
            line += string(11,' ');
            line += string("CLOCK") + string(15,' ');
            if (version >=3)
               line += leftJustify(fileSys,20);
            else
               line += string(20,' ');
            line += versionString;         // "RINEX VERSION / TYPE"
            strm << line << endl;
            strm.lineNumber++;

            line = leftJustify(program,20);
            line += leftJustify(runby,20);
            CommonTime dt = SystemTime();
            string dat;
            if (version >= 3)
               dat = printTime(dt,"%04Y/%02m/%02d %02H:%02M:%02S %4P");
            else
               dat = printTime(dt,"%02m/%02d/%04Y %02H:%02M:%02S");
            line += leftJustify(dat, 20);
            line += runByString;           // "PGM / RUN BY / DATE"
            strm << line << endl;
            strm.lineNumber++;

            if(valid & sysValid && version >= 3) {
               RinexObsMap::const_iterator it = sysObsTypes.begin();
               for (;it != sysObsTypes.end(); it++)
               {
                  static const int maxObsPerLine = 13;

                  map<string,vector<RinexObsID> >::const_iterator mapIter;
                  for(mapIter = sysObsTypes.begin(); mapIter != sysObsTypes.end();
                      mapIter++)
                  {
                     int obsWritten = 0;
                     line = ""; // make sure the line contents are reset

                     vector<RinexObsID> ObsTypeList = mapIter->second;

                     for(size_t i = 0; i < ObsTypeList.size(); i++)
                     {
                        // the first line needs to have the GNSS type and # of obs
                        if(obsWritten == 0)
                        {
                           line  =  leftJustify(mapIter->first, 1);
                           line += string(2, ' ');
                           line += rightJustify(asString(ObsTypeList.size()), 3);
                        }
                           // if you hit 13, write out the line and start a new one
                        else if((obsWritten % maxObsPerLine) == 0)
                        {
                           line += string(2, ' ');
                           line += sysString;
                           strm << line << endl;
                           strm.lineNumber++;
                           line  = string(6, ' ');
                        }
                        line += string(1, ' ');
                        line += rightJustify(ObsTypeList[i].asString(), 3);
                        obsWritten++;
                     }
                     line += string(60 - line.size(), ' ');
                     line += sysString;
                     strm << line << endl;
                     strm.lineNumber++;
                  }
               }
            }

            if(valid & timeSystemValid && version >= 3) {
               line = string(3,' ');
               line += leftJustify(timeSystem.asString(),57);
               line += timeSystemString;      // "TIME SYSTEM ID"
               strm << line << endl;
               strm.lineNumber++;
            }

            for(i=0; i<commentList.size(); ++i) {
               line = leftJustify(commentList[i],60);
               line += commentString;         // "COMMENT"
               strm << line << endl;
               strm.lineNumber++;
            }

            if(valid & leapSecondsValid) {
               line = rightJustify(asString(leapSeconds), 6);
               line += string(54,' ');
               line += leapSecondsString;     // "LEAP SECONDS"
               strm << line << endl;
               strm.lineNumber++;
            }

            if(valid & sysDCBValid && version >= 3) {
               std::map<std::string,stringPair>::const_iterator cbsCor;
               for (cbsCor = dcbsMap.begin(); cbsCor != dcbsMap.end(); cbsCor++)
               {
                  line = leftJustify(cbsCor->first,2);
                  line += leftJustify(cbsCor->second.first,18);
                  line += leftJustify(cbsCor->second.second,40);
                  line += sysDCBString;          // "SYS / DCBS APPLIED"
                  strm << line << endl;
                  strm.lineNumber++;
               }
            }

            if(valid & sysPCVValid && version >= 3) {
               std::map<std::string,stringPair>::const_iterator pcCor;
               for (pcCor = pcvsMap.begin(); pcCor != pcvsMap.end(); pcCor++)
               {
                  line = leftJustify(pcCor->first,2);
                  line += leftJustify(pcCor->second.first,18);
                  line += leftJustify(pcCor->second.second,40);
                  line += sysPCVString;          // "SYS / PCVS APPLIED"
                  strm << line << endl;
                  strm.lineNumber++;
               }
            }

            line = rightJustify(asString(dataTypes.size()), 6);
            for(i=0; i<dataTypes.size(); ++i)
               line += string(4,' ') + dataTypes[i];
            line += string(60-line.size(),' ');
            line += numDataString;         // "# / TYPES OF DATA"
            strm << line << endl;
            strm.lineNumber++;

               //@TODO line += stationNameString;     // "STATION NAME / NUM"
               //strm << line << endl;
               //strm.lineNumber++;

               //@TODO line += stationClockRefString; // "STATION CLK REF"
               //strm << line << endl;
               //strm.lineNumber++;

            line = analCenterDesignator;
            line += string(2,' ');
            line += leftJustify(analysisCenter,55);
            line += analysisCenterString;  // "ANALYSIS CENTER"
            strm << line << endl;
            strm.lineNumber++;

               //line += numClockRefString;     // "# OF CLK REF"
               //strm << line << endl;
               //strm.lineNumber++;

               //line += analysisClkRefrString; // "ANALYSIS CLK REF"
               //strm << line << endl;
               //strm.lineNumber++;

            line = rightJustify(asString(numSolnStations), 6);
            line += string(4,' ');
            line += leftJustify(terrRefFrame,50);
            line += numReceiversString;    // "# OF SOLN STA / TRF"
            strm << line << endl;
            strm.lineNumber++;

            map<string,string>::const_iterator it, jt;
            for(it=stationID.begin(); it != stationID.end(); ++it) {
               string label(it->first),field;
               line = label;
               line += string(1,' ');
               line += leftJustify(it->second,20);
               jt = stationX.find(label);
               field = jt->second;
               line += rightJustify(field, 11);
               line += string(1,' ');
               jt = stationY.find(label);
               field = jt->second;
               line += rightJustify(field, 11);
               line += string(1,' ');
               jt = stationZ.find(label);
               field = jt->second;
               line += rightJustify(field, 11);
               line += solnStateString;       // "SOLN STA NAME / NUM"
               strm << line << endl;
               strm.lineNumber++;
            }

            line = rightJustify(asString(numSolnSatellites), 6);
            line += string(54,' ');
            line += numSolnSatsString;     // "# OF SOLN SATS"
            strm << line << endl;
            strm.lineNumber++;

            line = string();
            for(i=0; i<satList.size(); ++i) {
               string satstr(" ");
               satstr[0] = satList[i].systemChar();
               satstr += rightJustify(asString(satList[i].id), 2);
               if(satstr[1] == ' ') satstr[1] = '0';
               line += satstr + string(1,' ');
               if(((i+1) % 15) == 0 || i==satList.size()-1) {
                  line += string(60-line.size(),' ');
                  line += prnListString;         // "PRN LIST"
                  strm << line << endl;
                  strm.lineNumber++;
                  line = string();
               }
            }

            line = string(60,' ');
            line += endOfHeaderString;     // "END OF HEADER"
            strm << line << endl;
            strm.lineNumber++;
         }
         catch(FFStreamError& e) { GPSTK_RETHROW(e); }
         catch(StringException& e) { GPSTK_RETHROW(e); }

      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
      catch(exception& e) { Exception g(e.what()); GPSTK_THROW(g); }
   }


   void Rinex3ClockHeader::dump(ostream& os, short detail) const throw()
   {
      size_t i;
      os << "Dump Rinex3Clock Header:\n";
      os << " Version = " << fixed << setprecision(2) << version
         << " Prgm /" << program << "/ Run By /" << runby << "/" << endl;
      os << " There are " << dataTypes.size() << " data types, as follows:";
      for(i=0; i<dataTypes.size(); ++i)
         os << " " << dataTypes[i];
      os << endl;
      os << " Leap seconds is " << leapSeconds << endl;
      os << " Analysis center: /" << analCenterDesignator
         << "/ /" << analysisCenter << "/" << endl;
      os << " Terrestrial Reference Frame " << terrRefFrame << endl;
      std::map<std::string,stringPair>::const_iterator dcbs;
      for (dcbs = dcbsMap.begin(); dcbs != dcbsMap.end(); dcbs++)
      {
         os << " DCBs: " << dcbs->first << " /" << dcbs->second.first << "/ /"
            << dcbs->second.second << "/" << endl;
      }
      std::map<std::string,stringPair>::const_iterator pcvs;
      for (pcvs = pcvsMap.begin(); pcvs != pcvsMap.end(); pcvs++)
      {
         os << " PCVs: " << pcvs->first << " /" << pcvs->second.first << "/ /"
            << pcvs->second.second << "/" << endl;
      }
      os << " Comments:\n";
      for(i=0; i<commentList.size(); ++i)
         os << "    " << commentList[i] << endl;
      os << " There are " << stationID.size() << " stations." << endl;
      os << " There are " << satList.size() << " satellites." << endl;
      if(detail > 0) {
         os << " Stations:  identifier     X(mm)       Y(mm)       Z(mm)\n";
         map<string,string>::const_iterator it, jt;
         for(it=stationID.begin(); it!=stationID.end(); ++it) {
            string label(it->first),field;
            os << "     " << label << "   " << it->second;
            jt = stationX.find(label);
            field = jt->second;
            os << rightJustify(field,12);
            jt = stationY.find(label);
            field = jt->second;
            os << rightJustify(field,12);
            jt = stationZ.find(label);
            field = jt->second;
            os << rightJustify(field,12) << endl;
         }
         os << " Sat list:\n";
         for(i=0; i<satList.size(); ++i) {
            os << " " << satList[i];
            if(((i+1)%15) == 0 || i == satList.size()-1) os << endl;
         }

         if(detail >= 2) dumpValid(os);
      }

      os << "End of Rinex3Clock header dump." << endl;

   }  // end Rinex3ClockHeader::dump()

   void Rinex3ClockHeader::dumpValid(ostream& os) const throw()
   {
      if( (valid & allValid) == allValid) return;
      string tag("  Invalid or missing header line: ");
      os << "Dump invalid or missing header records:\n";
      if(!(valid & versionValid)) os << tag << versionString << endl;
      if(!(valid & runByValid)) os << tag << runByString << endl;
      if(!(valid & commentValid)) os << tag << commentString << endl;
      if(!(valid & sysValid)) os << tag << sysString << endl;
      if(!(valid & timeSystemValid)) os << tag << timeSystemString << endl;
      if(!(valid & leapSecondsValid)) os << tag << leapSecondsString << endl;
      if(!(valid & sysDCBValid)) os << tag << sysDCBString << endl;
      if(!(valid & sysPCVValid)) os << tag << sysPCVString << endl;
      if(!(valid & numDataValid)) os << tag << numDataString << endl;
      if(!(valid & stationNameValid)) os << tag << stationNameString << endl;
      if(!(valid & stationClockRefValid)) os << tag << stationClockRefString << endl;
      if(!(valid & analysisCenterValid)) os << tag << analysisCenterString << endl;
      if(!(valid & numClockRefValid)) os << tag << numClockRefString << endl;
      if(!(valid & analysisClkRefrValid)) os << tag << analysisClkRefrString << endl;
      if(!(valid & numReceiversValid)) os << tag << numReceiversString << endl;
      if(!(valid & solnStateValid)) os << tag << solnStateString << endl;
      if(!(valid & numSolnSatsValid)) os << tag << numSolnSatsString << endl;
      if(!(valid & prnListValid)) os << tag << prnListString << endl;
      if(!(valid & endOfHeaderValid)) os << tag << endOfHeaderString << endl;
      os << "End of invalid or missing dump" << endl;
   }

}  // namespace
