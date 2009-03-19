#pragma ident "$Id: Rinex3ObsHeader.cpp 1709 2009-02-18 20:27:47Z btolman $"

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
 * Encapsulate header of Rinex observation file, including I/O
 */

#include "StringUtils.hpp"
#include "Rinex3ObsStream.hpp"
#include "Rinex3ObsHeader.hpp"

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{
   const string Rinex3ObsHeader::stringVersion           = "RINEX VERSION / TYPE";
   const string Rinex3ObsHeader::stringRunBy             = "PGM / RUN BY / DATE";
   const string Rinex3ObsHeader::stringComment           = "COMMENT";
   const string Rinex3ObsHeader::stringMarkerName        = "MARKER NAME";
   const string Rinex3ObsHeader::stringMarkerNumber      = "MARKER NUMBER";
   const string Rinex3ObsHeader::stringMarkerType        = "MARKER TYPE";
   const string Rinex3ObsHeader::stringObserver          = "OBSERVER / AGENCY";
   const string Rinex3ObsHeader::stringReceiver          = "REC # / TYPE / VERS";
   const string Rinex3ObsHeader::stringAntennaType       = "ANT # / TYPE";
   const string Rinex3ObsHeader::stringAntennaPosition   = "APPROX POSITION XYZ";
   const string Rinex3ObsHeader::stringAntennaDeltaHEN   = "ANTENNA: DELTA H/E/N";
   const string Rinex3ObsHeader::stringAntennaDeltaXYZ   = "ANTENNA: DELTA X/Y/Z";
   const string Rinex3ObsHeader::stringAntennaPhaseCtr   = "ANTENNA: PHASECENTER";
   const string Rinex3ObsHeader::stringAntennaBsightXYZ  = "ANTENNA: B.SIGHT XYZ";
   const string Rinex3ObsHeader::stringAntennaZeroDirAzi = "ANTENNA: ZERODIR AZI";
   const string Rinex3ObsHeader::stringAntennaZeroDirXYZ = "ANTENNA: ZERODIR XYZ";
   const string Rinex3ObsHeader::stringCenterOfMass      = "CENTER OF MASS: XYZ";
   const string Rinex3ObsHeader::stringSystemNumObs      = "SYS / # / OBS TYPES";
   const string Rinex3ObsHeader::stringSigStrengthUnit   = "SIGNAL STRENGTH UNIT";
   const string Rinex3ObsHeader::stringInterval          = "INTERVAL";
   const string Rinex3ObsHeader::stringFirstTime         = "TIME OF FIRST OBS";
   const string Rinex3ObsHeader::stringLastTime          = "TIME OF LAST OBS";
   const string Rinex3ObsHeader::stringReceiverOffset    = "RCV CLOCK OFFS APPL";
   const string Rinex3ObsHeader::stringSystemDCBSapplied = "SYS / DCBS APPLIED";
   const string Rinex3ObsHeader::stringSystemPCVSapplied = "SYS / PCVS APPLIED";
   const string Rinex3ObsHeader::stringSystemScaleFac    = "SYS / SCALE FACTOR";
   const string Rinex3ObsHeader::stringLeapSeconds       = "LEAP SECONDS";
   const string Rinex3ObsHeader::stringNumSats           = "# OF SATELLITES";
   const string Rinex3ObsHeader::stringPrnObs            = "PRN / # OF OBS";
   const string Rinex3ObsHeader::stringEoH               = "END OF HEADER";


   std::vector<Rinex3ObsHeader::Rinex3ObsType> Rinex3ObsHeader::RegisteredRinex3ObsTypes
      = Rinex3ObsHeader::StandardRinex3ObsTypes;


   void Rinex3ObsHeader::reallyPutRecord(FFStream& ffs) const
      throw(std::exception, FFStreamError, StringException)
   {
      Rinex3ObsStream& strm = dynamic_cast<Rinex3ObsStream&>(ffs);

      strm.header = *this;

      unsigned long allValid;
      if      (version == 3.0)   allValid = allValid30;
      else if (version == 3.00)  allValid = allValid30;
      else
      {
         FFStreamError err("Unknown RINEX version: " + asString(version,2));
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


   // This function computes the number of valid header records which WriteHeaderRecords will write.
   int Rinex3ObsHeader::NumberHeaderRecordsToBeWritten(void) const throw()
   {
      int n = 0;

      if (valid & Rinex3ObsHeader::validVersion          ) n++;
      if (valid & Rinex3ObsHeader::validRunBy            ) n++;
      if (valid & Rinex3ObsHeader::validComment          ) n += commentList.size();
      if (valid & Rinex3ObsHeader::validMarkerName       ) n++;
      if (valid & Rinex3ObsHeader::validMarkerNumber     ) n++;
      if (valid & Rinex3ObsHeader::validMarkerType       ) n++;
      if (valid & Rinex3ObsHeader::validObserver         ) n++;
      if (valid & Rinex3ObsHeader::validReceiver         ) n++;
      if (valid & Rinex3ObsHeader::validAntennaType      ) n++;
      if (valid & Rinex3ObsHeader::validAntennaPosition  ) n++;
      if (valid & Rinex3ObsHeader::validAntennaDeltaHEN  ) n++;
      if (valid & Rinex3ObsHeader::validAntennaDeltaXYZ  ) n++;
      if (valid & Rinex3ObsHeader::validAntennaPhaseCtr  ) n++;
      if (valid & Rinex3ObsHeader::validAntennaBsightXYZ ) n++;
      if (valid & Rinex3ObsHeader::validAntennaZeroDirAzi) n++;
      if (valid & Rinex3ObsHeader::validAntennaZeroDirXYZ) n++;
      if (valid & Rinex3ObsHeader::validCenterOfmass     ) n++;
      if (valid & Rinex3ObsHeader::validObsType          ) n += 1 + (obsTypeList.size()-1)/9;
      if (valid & Rinex3ObsHeader::validSigStrengthUnit  ) n++;
      if (valid & Rinex3ObsHeader::validInterval         ) n++;
      if (valid & Rinex3ObsHeader::validFirstTime        ) n++;
      if (valid & Rinex3ObsHeader::validLastTime         ) n++;
      if (valid & Rinex3ObsHeader::validReceiverOffset   ) n++;
      if (valid & Rinex3ObsHeader::validSystemDCBSapplied) n++;
      if (valid & Rinex3ObsHeader::validSystemPCVSapplied) n++;
      if (valid & Rinex3ObsHeader::validSystemScaleFac   ) n++;
      if (valid & Rinex3ObsHeader::validLeapSeconds      ) n++;
      if (valid & Rinex3ObsHeader::validNumSats          ) n++;
      if (valid & Rinex3ObsHeader::validPrnObs           )
         n += numObsForSat.size() * (1+numObsForSat.begin()->second.size()/9);
      if (valid & Rinex3ObsHeader::validEoH              ) n++;

      return n;
   }


   // This function writes all valid header records.
   void Rinex3ObsHeader::WriteHeaderRecords(FFStream& ffs) const
      throw(FFStreamError, StringException)
   {
      Rinex3ObsStream& strm = dynamic_cast<Rinex3ObsStream&>(ffs);
      string line;
      if (valid & validVersion)
      {
         line  = rightJustify(asString(version,2), 9);
         line += string(11, ' ');
         if ((fileType[0] != 'O') && (fileType[0] != 'o'))
         {
            FFStreamError err("This isn't a Rinex Observation file: " +
                              fileType.substr(0,1));
            GPSTK_THROW(err);
         }

         if (system.system == RinexSatID::systemUnknown)
         {
            FFStreamError err("Invalid satellite system");
            GPSTK_THROW(err);
         }

         line += leftJustify(string("Observation"), 20);
         std::string str;
         str = system.systemChar();
         str = str + " (" + system.systemString() + ")";
         line += leftJustify(str, 20);
         line += stringVersion;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & validRunBy)
      {
         line  = leftJustify(fileProgram,20);
         line += leftJustify(fileAgency, 20);
         CivilTime dt;
//         dt.setLocalTime(); ***** NEED TO REPLACE WITH LOCAL SYSTEM TIME CALL
         string dat = dt.printf("%02m/%02d/%04Y %02H:%02M:%02S %P");
         line += leftJustify(dat, 20);
         line += stringRunBy;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & validMarkerName)
      {
         line  = leftJustify(markerName, 60);
         line += stringMarkerName;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & validObserver)
      {
         line  = leftJustify(observer, 20);
         line += leftJustify(agency, 40);
         line += stringObserver;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & validReceiver)
      {
         line  = leftJustify(recNo, 20);
         line += leftJustify(recType, 20);
         line += leftJustify(recVers, 20);
         line += stringReceiver;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & validAntennaType)
      {
         line  = leftJustify(antNo, 20);
         line += leftJustify(antType, 20);
         line += string(20, ' ');
         line += stringAntennaType;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & validAntennaPosition)
      {
         line  = rightJustify(asString(antennaPosition[0], 4), 14);
         line += rightJustify(asString(antennaPosition[1], 4), 14);
         line += rightJustify(asString(antennaPosition[2], 4), 14);
         line += string(18, ' ');
         line += stringAntennaPosition;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & validAntennaDeltaHEN)
      {
         line  = rightJustify(asString(antennaDeltaHEN[0], 4), 14);
         line += rightJustify(asString(antennaDeltaHEN[1], 4), 14);
         line += rightJustify(asString(antennaDeltaHEN[2], 4), 14);
         line += string(18, ' ');
         line += stringAntennaDeltaHEN;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & validAntennaDeltaXYZ)
      {
         line  = rightJustify(asString(antennaDeltaXYZ[0], 4), 14);
         line += rightJustify(asString(antennaDeltaXYZ[1], 4), 14);
         line += rightJustify(asString(antennaDeltaXYZ[2], 4), 14);
         line += string(18, ' ');
         line += stringAntennaDeltaXYZ;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & validObsType)
      {
         const int maxObsPerLine = 9;
         int obsWritten = 0;
         line = ""; // make sure the line contents are reset.

         vector<Rinex3ObsType>::const_iterator itr = obsTypeList.begin();

         while (itr != obsTypeList.end())
         {
               // the first line needs to have the # of obs
            if (obsWritten == 0)
               line  = rightJustify(asString(obsTypeList.size()), 6);
               // if you hit 9, write out the line and start a new one
            else if ((obsWritten % maxObsPerLine) == 0)
            {
               line += stringSystemNumObs;
               strm << line << endl;
               strm.lineNumber++;
               line  = string(6, ' ');
            }
            line += rightJustify(convertObsType(*itr), 6);
            obsWritten++;
            itr++;
         }
         line += string(60 - line.size(), ' ');
         line += stringSystemNumObs;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & validInterval)
      {
         line  = rightJustify(asString(interval, 3), 10);
         line += string(50, ' ');
         line += stringInterval;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & validFirstTime)
      {
         line  = writeTime(firstObs);
         line += string(60 - line.size(), ' ');
         line += stringFirstTime;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & validLastTime)
      {
         line  = writeTime(lastObs);
         line += string(60 - line.size(), ' ');
         line += stringLastTime;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & validMarkerNumber)
      {
         line  = leftJustify(markerNumber, 20);
         line += string(40, ' ');
         line += stringMarkerNumber;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & validReceiverOffset)
      {
         line  = rightJustify(asString(receiverOffset),6);
         line += string(54, ' ');
         line += stringReceiverOffset;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & validLeapSeconds)
      {
         line  = rightJustify(asString(leapSeconds),6);
         line += string(54, ' ');
         line += stringLeapSeconds;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & validComment)
      {
         vector<string>::const_iterator itr = commentList.begin();
         while (itr != commentList.end())
         {
            line  = leftJustify((*itr), 60);
            line += stringComment;
            strm << line << endl;
            strm.lineNumber++;
            itr++;
         }
      }
      if (valid & validNumSats)
      {
         line  = rightJustify(asString(numSVs), 6);
         line += string(54, ' ');
         line += stringNumSats;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & validPrnObs)
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
                     RinexSatID prn((*itr).first);
                     line  = string(3, ' ') + prn.toString();
                  }
                  catch (Exception& e) {
                     FFStreamError ffse(e);
                     GPSTK_RETHROW(ffse); 
                  }
               }
               else if ((numObsWritten % maxObsPerLine)  == 0)
               {
                  line += stringPrnObs;
                  strm << line << endl;
                  strm.lineNumber++;
                  line  = string(6, ' ');
               }
               line += rightJustify(asString(*vecItr), 6);
               ++vecItr;
               ++numObsWritten;
            }
            line += string(60 - line.size(), ' ');
            line += stringPrnObs;
            strm << line << endl;
            strm.lineNumber++;
            itr++;
         }
      }
      if (valid & validEoH)
      {
         line  = string(60, ' ');
         line += stringEoH;
         strm << line << endl;
         strm.lineNumber++;
      }   
   }   // end Rinex3ObsHeader::WriteHeaderRecords()


   // This function parses a single header record.
   void Rinex3ObsHeader::ParseHeaderRecord(string& line)
      throw(FFStreamError)
   {
      string label(line, 60, 20);
         
      if (label == stringVersion)
      {
         version = asDouble(line.substr(0,20));
         fileType = strip(line.substr(20,20));
         satSys   = strip(line.substr(40,20));
         if ( fileType[0] != 'O' && fileType[0] != 'o')
         {
            FFStreamError e("This isn't a RINEX 3 Obs file.");
            GPSTK_THROW(e);
         }
         if ( satSys[0] != 'G' && satSys[0] != 'g' &&
              satSys[0] != 'R' && satSys[0] != 'r' &&
              satSys[0] != 'E' && satSys[0] != 'e' &&
              satSys[0] != 'S' && satSys[0] != 's' &&
              satSys[0] != 'M' && satSys[0] != 'm'    )
         {
           FFStreamError e("The satellite system isn't valid.");
           GPSTK_THROW(e);
         }
         valid |= validVersion;
      }
      else if (label == stringRunBy)
      {
         fileProgram = strip(line.substr( 0,20));
         fileAgency  = strip(line.substr(20,20));
         date        = strip(line.substr(40,20));
         valid |= validRunBy;
      }
      else if (label == stringComment)
      {
         commentList.push_back(strip(line.substr(0,60)));
         valid |= validComment;
      }
      else if (label == stringMarkerName)
      {
         markerName = strip(line.substr(0,60));
         valid |= validMarkerName;
      }
      else if (label == stringMarkerNumber)
      {
         markerNumber = strip(line.substr(0,20));
         valid |= validMarkerNumber;
      }
      else if (label == stringMarkerType)
      {
         markerType = strip(line.substr(0,20));
         valid |= validMarkerType;
      }
      else if (label == stringObserver)
      {
         observer = strip(line.substr( 0,20));
         agency   = strip(line.substr(20,40));
         valid |= validObserver;
      }
      else if (label == stringReceiver)
      {
         recNo   = strip(line.substr( 0,20));
         recType = strip(line.substr(20,20));
         recVers = strip(line.substr(40,20));
         valid |= validReceiver;
      }
      else if (label ==stringAntennaType)
      {
         antNo   = strip(line.substr( 0,20));
         antType = strip(line.substr(20,20));
         valid |= validAntennaType;
      }
      else if (label == stringAntennaPosition)
      {
         antennaPosition[0] = asDouble(line.substr( 0,14));
         antennaPosition[1] = asDouble(line.substr(14,14));
         antennaPosition[2] = asDouble(line.substr(28,14));
         valid |= validAntennaPosition;
      }
      else if (label == stringAntennaDeltaHEN)
      {
         antennaDeltaHEN[0] = asDouble(line.substr( 0,14));
         antennaDeltaHEN[1] = asDouble(line.substr(14,14));
         antennaDeltaHEN[2] = asDouble(line.substr(28,14));
         valid |= validAntennaDeltaHEN;
      }
      else if (label == stringAntennaDeltaXYZ)
      {
         antennaDeltaXYZ[0] = asDouble(line.substr( 0,14));
         antennaDeltaXYZ[1] = asDouble(line.substr(14,14));
         antennaDeltaXYZ[2] = asDouble(line.substr(28,14));
         valid |= validAntennaDeltaXYZ;
      }
      else if (label == stringAntennaPhaseCtr)
      {
         antennaSatSys  = strip(line.substr(0,2));
         antennaObsCode = strip(line.substr(2,3));
         antennaPhaseCtr[0] = asDouble(line.substr( 5, 9));
         antennaPhaseCtr[1] = asDouble(line.substr(14,14));
         antennaPhaseCtr[2] = asDouble(line.substr(14,14));
         valid |= validAntennaPhaseCtr;
      }
      else if (label == stringAntennaBsightXYZ)
      {
         antennaBsight[0] = asDouble(line.substr( 0,14));
         antennaBsight[1] = asDouble(line.substr(14,14));
         antennaBsight[2] = asDouble(line.substr(28,14));
         valid |= validAntennaBsightXYZ;
      }
      else if (label == stringAntennaZeroDirAzi)
      {
         antennaZeroDirAzi = asDouble(line.substr(0,14));
         valid |= validAntennaBsightXYZ;
      }
      else if (label == stringAntennaZeroDirXYZ)
      {
         antennaZeroDirXYZ[0] = asDouble(line.substr( 0,14));
         antennaZeroDirXYZ[1] = asDouble(line.substr(14,14));
         antennaZeroDirXYZ[2] = asDouble(line.substr(28,14));
         valid |= validAntennaBsightXYZ;
      }
      else if (label == stringCenterOfMass)
      {
         centerOfMass[0] = asDouble(line.substr( 0,14));
         centerOfMass[1] = asDouble(line.substr(14,14));
         centerOfMass[2] = asDouble(line.substr(28,14));
         valid |= validCenterOfMass;
      }
      else if (label == stringSystemNumObs)
      {
         const int maxObsPerLine = 9;
            // process the first line
         if (! (valid & validObsType))
         {
            numObs = asInt(line.substr(0,6));
            
            for (int i = 0; (i < numObs) && (i < maxObsPerLine); i++)
            {
               int position = i * 6 + 6 + 4;
               Rinex3ObsType rt = convertObsType(line.substr(position,2));
               obsTypeList.push_back(rt);
            }
            valid |= validObsType;
         }
            // process continuation lines
         else
         {
            for (int i = obsTypeList.size();
                 (i < numObs) && ( (i % maxObsPerLine) < maxObsPerLine); i++)
            {
               int position = (i % maxObsPerLine) * 6 + 6 + 4;
               Rinex3ObsType rt = convertObsType(line.substr(position,2));
               obsTypeList.push_back(rt);
            }
         }
      }
      else if (label == stringInterval)
      {
         interval = asDouble(line.substr(0, 10));
         valid |= validInterval;
      }
      else if (label == stringFirstTime)
      {
         firstObs = parseTime(line);
         valid |= validFirstTime;
      }
      else if (label == stringLastTime)
      {
         lastObs = parseTime(line);
         valid |= validLastTime;
      }
      else if (label == stringReceiverOffset)
      {
         receiverOffset = asInt(line.substr(0,6));
         valid |= validReceiverOffset;
      }
      else if (label == stringLeapSeconds)
      {
         leapSeconds = asInt(line.substr(0,6));
         valid |= validLeapSeconds;
      }
      else if (label == stringNumSats)
      {
         numSVs = asInt(line.substr(0,6)) ;
         valid |= validNumSats;
      }
      else if (label == stringPrnObs)
      {
         const int maxObsPerLine = 9;
         // continuation lines... you have to know what PRN
         // this is continuing for, hence lastPRN
         if ( lastPRN.id != -1 && numObsForSat[lastPRN].size() != obsTypeList.size() )
         {
            for (int i = numObsForSat[lastPRN].size(); 
                 (i < obsTypeList.size()) && ( (i % maxObsPerLine) < maxObsPerLine); i++)
            {
               numObsForSat[lastPRN].push_back(asInt(line.substr((i%maxObsPerLine)*6+6,6)));
            }
         }
         else
         {
            try
            {
               lastPRN.fromString(line.substr(3,3));
            }
            catch (Exception& e)
            {
               FFStreamError ffse(e);
               GPSTK_RETHROW(ffse);
            }
            vector<int> numObsList;
            for (int i = 0; 
                   (i < obsTypeList.size()) && (i < maxObsPerLine); i++)
            {
               numObsList.push_back(asInt(line.substr(i*6+6,6)));
            }

            numObsForSat[lastPRN] = numObsList;
         }
         valid |= validPrnObs;
      }
      else if (label == stringEoH)
      {
         valid |= validEoH;
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

      // If already read, just return.
      if (strm.headerRead == true) return;

      // Since we're reading a new header, we need to reinitialize all our list structures.
      // All the other objects should be ok.  This also applies if we threw an exception
      // the first time we read the header and are now re-reading it.  Some of these could
      // be full and we need to empty them.
      clear();

      string line;

      while (!(valid & validEoH))
      {
         strm.formattedGetLine(line);
         StringUtils::stripTrailing(line);

         if (line.length() == 0)
         {
            FFStreamError e("No data read");
            GPSTK_THROW(e);
         }
         else if (line.length() < 60 || line.length() > 80)
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
      if      (version == 3.0)  allValid = allValid30;
      else if (version == 3.00) allValid = allValid30;
      else
      {
         FFStreamError e("Unknown or unsupported RINEX version " + 
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


   Rinex3ObsHeader::Rinex3ObsType Rinex3ObsHeader::convertObsType(const string& oneObs)
      throw(FFStreamError)
   {
      Rinex3ObsType ot(RegisteredRinex3ObsTypes[0]);   // Unknown type
      for (int i = 0; i < RegisteredRinex3ObsTypes.size(); i++) {
         if (RegisteredRinex3ObsTypes[i].type == oneObs)
         {
            ot = RegisteredRinex3ObsTypes[i];
            break;
         }
         //FFStreamError e("Bad obs type: " + oneObs);
         //GPSTK_THROW(e);
      }
      return ot;
   }


   string Rinex3ObsHeader::convertObsType(const Rinex3ObsHeader::Rinex3ObsType& oneObs)
      throw(FFStreamError)
   {
      return oneObs.type;
   }


   // return 1 if type already defined,
   //        0 if successful
   //       -1 if not successful - invalid input
   int RegisterExtendedRinex3ObsType(string t, string d, string u, unsigned int dep)
   {
      if (t.empty()) return -1;
      // throw if t="UN" ?
      // check that it is not duplicated
      for (int i = 0; i < Rinex3ObsHeader::RegisteredRinex3ObsTypes.size(); i++)
      {
         if (Rinex3ObsHeader::RegisteredRinex3ObsTypes[i].type == t) { return 1; }
      }
      Rinex3ObsHeader::Rinex3ObsType ot;
      if (t.size()>2) t.resize(2,' '); ot.type = stripTrailing(t);
      if (d.size()>20) d.resize(20,' '); ot.description = stripTrailing(d);
      if (u.size()>10) u.resize(10,' '); ot.units = stripTrailing(u);
//      ot.depend = dep;
      Rinex3ObsHeader::RegisteredRinex3ObsTypes.push_back(ot);
      return 0;
   }


   CivilTime Rinex3ObsHeader::parseTime(const string& line) const
   {
      int year, month, day, hour, min;
      double sec;
   
      year  = asInt(   line.substr(0,  6 ));
      month = asInt(   line.substr(6,  6 ));
      day   = asInt(   line.substr(12, 6 ));
      hour  = asInt(   line.substr(18, 6 ));
      min   = asInt(   line.substr(24, 6 ));
      sec   = asDouble(line.substr(30, 13));

      return CivilTime(year, month, day, hour, min, sec);
   }


   string Rinex3ObsHeader::writeTime(const CivilTime& civtime) const
   {
      string line;

      line  = rightJustify(asString<short>(civtime.year    ),  6);
      line += rightJustify(asString<short>(civtime.month   ),  6);
      line += rightJustify(asString<short>(civtime.day     ),  6);
      line += rightJustify(asString<short>(civtime.hour    ),  6);
      line += rightJustify(asString<short>(civtime.minute  ),  6);
      line += rightJustify(asString(       civtime.second,7), 13);

      return line;
   }


   void Rinex3ObsHeader::dump(ostream& s) const
   {
      int i,j;
      s << "---------------------------------- REQUIRED ----------------------------------\n";
      string str;
      str = system.systemChar();
      str = str + " (" + system.systemString() + ")";
      s << "Rinex Version " << fixed << setw(5) << setprecision(2) << version
        << ",  File type " << fileType << ",  System " << str << ".\n";
      s << "Prgm: " << fileProgram << ",  Run: " << date << ",  By: " << fileAgency << endl;
      s << "Marker name: " << markerName << ".\n";
      s << "Obs'r : " << observer << ",  Agency: " << agency << endl;
      s << "Rec#: " << recNo << ",  Type: " << recType << ",  Vers: " << recVers << endl;
      s << "Antenna # : " << antNo << ",  Type : " << antType << endl;
      s << "Position (XYZ,m) : " << setprecision(4) << antennaPosition << ".\n";
      s << "Antenna Delta (HEN,m) : " << setprecision(4) << antennaDeltaHEN << ".\n";
      s << "Antenna Delta (XYZ,m) : " << setprecision(4) << antennaDeltaXYZ << ".\n";
      s << "Observation types (" << obsTypeList.size() << ") :\n";
      for (i = 0; i < obsTypeList.size(); i++) 
         s << " Type #" << i << " = "
           << gpstk::Rinex3ObsHeader::convertObsType(obsTypeList[i])
           << " " << obsTypeList[i].description
           << " (" << obsTypeList[i].units << ")." << endl;
      s << "Time of first obs " << firstObs.printf("%04Y/%02m/%02d %02H:%02M:%010.7f %P");
      s << "(This header is ";
      if ((valid & allValid30) == allValid30) s << "VALID 3.00";
      else s << "NOT VALID";
      s << " Rinex.)\n";

      if (!(valid & validVersion        )) s << " Version is NOT valid\n";
      if (!(valid & validRunBy          )) s << " Run by is NOT valid\n";
      if (!(valid & validMarkerName     )) s << " Marker Name is NOT valid\n";
      if (!(valid & validObserver       )) s << " Observer is NOT valid\n";
      if (!(valid & validReceiver       )) s << " Receiver is NOT valid\n";
      if (!(valid & validAntennaType    )) s << " Antenna Type is NOT valid\n";
      if (!(valid & validAntennaPosition)) s << " Antenna Position is NOT valid\n";
      if (!(valid & validAntennaDeltaHEN)) s << " Antenna Delta HEN is NOT valid\n";
      if (!(valid & validAntennaDeltaXYZ)) s << " Antenna Delta XYZ is NOT valid\n";
      if (!(valid & validObsType        )) s << " Obs Type is NOT valid\n";
      if (!(valid & validFirstTime      )) s << " First time is NOT valid\n";
      if (!(valid & validEoH            )) s << " End is NOT valid\n";

      s << "---------------------------------- OPTIONAL ----------------------------------\n";
      if (valid & validMarkerNumber) s << "Marker number : " << markerNumber << endl;
      if (valid & validInterval) s << "Interval = "
                                   << fixed << setw(7) << setprecision(3) << interval << endl;
      if (valid & validLastTime) s << "Time of last obs "
                                   << lastObs.printf("%04Y/%02m/%02d %02H:%02M:%010.7f %P");
      if (valid & validLeapSeconds) s << "Leap seconds: " << leapSeconds << endl;
      if (valid & validReceiverOffset) s << "Clock offset record is present and offsets "
                                         << (receiverOffset?"ARE":"are NOT") << " applied." << endl;
      if (valid & validNumSats) s << "Number of Satellites with data : " << numSVs << endl;
      if (valid & validPrnObs)
      {
         s << "SAT  ";
         for (i = 0; i < obsTypeList.size(); i++)
            s << setw(7) << convertObsType(obsTypeList[i]);
         s << endl;
         map<SatID, vector<int> >::const_iterator sat_itr = numObsForSat.begin();
         while (sat_itr != numObsForSat.end())
         {
            vector<int> obsvec=sat_itr->second;
            s << " " << RinexSatID(sat_itr->first) << " ";
            for (i=0; i<obsvec.size(); i++) s << " " << setw(6) << obsvec[i];
            s << endl;
            sat_itr++;
         }
      }
      if (commentList.size() && !(valid & validComment)) s << " Comment is NOT valid\n";
      s << "Comments (" << commentList.size() << ") :\n";
      for (i = 0; i < commentList.size(); i++)
         s << commentList[i] << endl;

      s << "-------------------------------- END OF HEADER -------------------------------\n";
   }


   // Pretty print a list of standard Rinex observation types
   void DisplayStandardRinex3ObsTypes(ostream& s)
   {
      s << "The list of standard Rinex obs types:\n";
      s << "  OT Description          Units\n";
      s << "  -- -------------------- ---------\n";
      for (int i = 0; i < Rinex3ObsHeader::StandardRinex3ObsTypes.size(); i++)
      {
         string line;
         line = string("  ")+Rinex3ObsHeader::StandardRinex3ObsTypes[i].type;
         line += leftJustify(string(" ")+Rinex3ObsHeader::StandardRinex3ObsTypes[i].description,21);
         line += leftJustify(string(" ")+Rinex3ObsHeader::StandardRinex3ObsTypes[i].units,11);
         s << line << endl;
      }
   }


   // Pretty print a list of registered extended Rinex observation types
   void DisplayExtendedRinex3ObsTypes(ostream& s)
   {
      s << "The list of available extended Rinex obs types:\n";
      s << "  OT Description          Units     Required input (EP=ephemeris,PS=Rx Position)\n";
      s << "  -- -------------------- --------- ------------------\n";
      for (int i = Rinex3ObsHeader::StandardRinex3ObsTypes.size();
               i < Rinex3ObsHeader::RegisteredRinex3ObsTypes.size(); i++)
      {
         string line;
         line = string("  ")+Rinex3ObsHeader::RegisteredRinex3ObsTypes[i].type;
         line += leftJustify(string(" ")+Rinex3ObsHeader::RegisteredRinex3ObsTypes[i].description,21);
         line += leftJustify(string(" ")+Rinex3ObsHeader::RegisteredRinex3ObsTypes[i].units,11);
         for (int j = 1; j <= 6; j++)
         {
            if (j==3 || j==4) continue;
//            if (Rinex3ObsHeader::RegisteredRinex3ObsTypes[i].depend &
//                Rinex3ObsHeader::StandardRinex3ObsTypes[j].depend    )
//                  line += string(" ")+Rinex3ObsHeader::StandardRinex3ObsTypes[j].type;
//            else line += string("   ");
         }
//         if (Rinex3ObsHeader::RegisteredRinex3ObsTypes[i].depend & Rinex3ObsHeader::Rinex3ObsType::EPdepend)
//            line += string(" EP"); else line += string("   ");
//         if (Rinex3ObsHeader::RegisteredRinex3ObsTypes[i].depend & Rinex3ObsHeader::Rinex3ObsType::PSdepend)
//            line += string(" PS"); else line += string("   ");
         s << line << endl;
      }
   }


} // namespace gpstk
