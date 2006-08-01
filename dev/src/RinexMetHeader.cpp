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
 * @file RinexMetHeader.cpp
 * Encapsulate header of Rinex meterological file data, including I/O
 */

#include <algorithm>       // for find

#include "StringUtils.hpp"
#include "RinexMetHeader.hpp"
#include "RinexMetStream.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
   const int RinexMetHeader::maxObsPerLine = 9;

   const string RinexMetHeader::versionString = "RINEX VERSION / TYPE";
   const string RinexMetHeader::runByString = "PGM / RUN BY / DATE";
   const string RinexMetHeader::commentString = "COMMENT";
   const string RinexMetHeader::markerNameString = "MARKER NAME";
   const string RinexMetHeader::markerNumberString = "MARKER NUMBER";
   const string RinexMetHeader::obsTypeString = "# / TYPES OF OBSERV";
   const string RinexMetHeader::sensorTypeString = "SENSOR MOD/TYPE/ACC";
   const string RinexMetHeader::sensorPosString = "SENSOR POS XYZ/H";
   const string RinexMetHeader::endOfHeader = "END OF HEADER";

   std::string RinexMetHeader::bitString(unsigned long vb, char quote,
                                         std::string sep)
   {
      unsigned long b = 1;
      std::string rv;
      while (b)
      {
         if (vb & b)
         {
            if (rv.length())
               rv += sep;
            if (quote)
               rv += quote + bitsAsString((validBits)b) + quote;
            else
               rv += bitsAsString((validBits)b);
         }
         b <<= 1;
      }
      return rv;
   }

   void RinexMetHeader::reallyPutRecord(FFStream& ffs) const
      throw(std::exception, FFStreamError,
            gpstk::StringUtils::StringException)
   {
      RinexMetStream& strm = dynamic_cast<RinexMetStream&>(ffs);
      
         // since they want to output this header, let's store
         // it internally for use by the data
      strm.header = (*this);
      
         // i'm casting out const here to set the correct required valid bits.
         // deal with it =P
      unsigned long allValid;
      if (version == 2.0)        allValid = allValid20;
      else if (version == 2.1)   allValid = allValid21;
      else
      {
         FFStreamError err("Unknown RINEX version: " + asString(version,2));
         err.addText("Make sure to set the version correctly.");
         GPSTK_THROW(err);
      }
      
      if ((valid & allValid) != allValid)
      {
         string errstr("Incomplete or invalid header: missing: ");
         errstr += bitString(allValid & ~valid);
         FFStreamError err(errstr);
         err.addText("Make sure you set all header valid bits for all of the available data.");
         GPSTK_THROW(err);
      }
      
      string line;
         // line by line, let's do this.
      if (valid & versionValid)
      {
         line  = rightJustify(asString(version,2), 9);
         line += string(11, ' ');
         line += leftJustify(fileType, 40);
         line += versionString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & runByValid)
      {
         line  = leftJustify(fileProgram,20);
         line += leftJustify(fileAgency,20);
         line += leftJustify(date, 20);
         line += runByString;
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
      if (valid & markerNameValid)
      {
         line  = leftJustify(markerName, 60);
         line += markerNameString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & markerNumberValid)
      {
         line  = leftJustify(markerNumber, 60);
         line += markerNumberString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & obsTypeValid)
      {
         line  = rightJustify(asString(obsTypeList.size()),6);
         vector<RinexMetType>::const_iterator itr = obsTypeList.begin();
         size_t numWritten = 0;
         while (itr != obsTypeList.end())
         {
            numWritten++;
               // stupid continuation lines =P
            if ((numWritten % maxObsPerLine) == 0)
            {
               line += obsTypeString;
               strm << line << endl;
               strm.lineNumber++;
               line = string(6,' ');
            }
            line += rightJustify(convertObsType(*itr), 6);
            itr++;
         }
            // pad the line out to 60 chrs and add label
         line += string(60 - line.size(), ' ');
         line += obsTypeString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & sensorTypeValid)
      {
            // only write out the sensor types that are 
            // in the obsTypeList
         vector<sensorType>::const_iterator itr = sensorTypeList.begin();
         while (itr != sensorTypeList.end())
         {
            if (std::find(obsTypeList.begin(), obsTypeList.end(),
                          (*itr).obsType) != obsTypeList.end())
            {
               line  = leftJustify((*itr).model, 20);
               line += leftJustify((*itr).type, 20);
               line += string(6, ' ');
               line += rightJustify(asString((*itr).accuracy,1),7);
               line += string(4, ' ');
               line += convertObsType((*itr).obsType);
               line += string(1, ' ');
               line += sensorTypeString;
               strm << line << endl;
               strm.lineNumber++;
            }
            itr++;
         }
      }
      if (valid & sensorPosValid)
      {
            // only write out the sensor positions that are 
            // in the obsTypeList
         vector<sensorPosType>::const_iterator itr = sensorPosList.begin();
         while (itr != sensorPosList.end())
         {
            if (std::find(obsTypeList.begin(), obsTypeList.end(),
                     (*itr).obsType) != obsTypeList.end())
            {
               line  = rightJustify(asString((*itr).position[0],4),14);
               line += rightJustify(asString((*itr).position[1],4),14);
               line += rightJustify(asString((*itr).position[2],4),14);
               line += rightJustify(asString((*itr).height,4),14);
               line += string(1, ' ');
               line += convertObsType((*itr).obsType);
               line += string(1, ' ');
               line += sensorPosString;
               strm << line << endl;
               strm.lineNumber++;
            }
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
   }     
   

   void RinexMetHeader::reallyGetRecord(FFStream& ffs) 
      throw(std::exception, FFStreamError, 
            gpstk::StringUtils::StringException)
   {
      RinexMetStream& strm = dynamic_cast<RinexMetStream&>(ffs);
      
         // if already read, just return
      if (strm.headerRead == true)
         return;

      valid = 0;
      
         // clear out structures in case the last read was a partial header
         // and there's cruft left
      commentList.clear();
      obsTypeList.clear();
      sensorTypeList.clear();
      sensorPosList.clear();
      
      int numObs;
      
      while (! (valid & endValid))
      {
         string line;
         strm.formattedGetLine(line);
         
         if (line.length()<60 || line.length()>81)
         {
            FFStreamError e("Bad line length");
            GPSTK_THROW(e);
         }
         
         string thisLabel(line, 60, 20);
         
         if (thisLabel == versionString)
         {
            version = asDouble(line.substr(0,20));
            fileType = strip(line.substr(20,20));
            if ( (fileType[0] != 'M') &&
                 (fileType[0] != 'm'))
            {
               FFStreamError e("This isn't a Rinex Met file");
               GPSTK_THROW(e);
            }
            valid |= versionValid;
         }
         else if (thisLabel == runByString)
         {
            fileProgram = strip(line.substr(0,20));
            fileAgency = strip(line.substr(20,20));
            date = strip(line.substr(40,20));
            valid |= runByValid;
         }
         else if (thisLabel == commentString)
         {
            commentList.push_back(strip(line.substr(0,60)));
            valid |= commentValid;
         }
         else if (thisLabel == markerNameString)
         {
            markerName = strip(line.substr(0,60));
            valid |= markerNameValid;
         }
         else if (thisLabel == markerNumberString)
         {
            markerNumber = strip(line.substr(0,20));
            valid |= markerNumberValid;
         }
         else if (thisLabel == obsTypeString)
         {
               // read the first line
            if (! (valid & obsTypeValid))
            {
               numObs = gpstk::StringUtils::asInt(line.substr(0,6));
               for (int i = 0; (i < numObs) && (i < maxObsPerLine); i++)
               {
                  int currPos = i * 6 + 6;
                  if (line.substr(currPos, 4) != string(4, ' '))
                  {
                     FFStreamError e("Format error for line type " +
                                     obsTypeString);
                     GPSTK_THROW(e);
                  }
                  
                  obsTypeList.push_back(convertObsType(line.substr(currPos + 4, 2)));
               }
               valid |= obsTypeValid;
            }
               // read continuation lines
            else
            {
               int currentObsTypes = obsTypeList.size();
               for (int i = currentObsTypes; 
                    (i < numObs) && (i < (maxObsPerLine + currentObsTypes));
                    i++)
               {
                  int currPos = (i % maxObsPerLine) * 6 + 6;
                  if (line.substr(currPos, 4) != string(4,' '))
                  {
                     FFStreamError e("Format error for line type " +
                                     obsTypeString);
                     GPSTK_THROW(e);
                  }
                  
                  obsTypeList.push_back(convertObsType(line.substr(currPos + 4, 2)));
               }
            }
         }
         else if (thisLabel == sensorTypeString)
         {
            if (line.substr(40,6) != string(6, ' '))
            {
               FFStreamError e("Format error for line type " + 
                               sensorTypeString);
               GPSTK_THROW(e);
            }
            sensorType st;
            st.model = strip(line.substr(0,20));
            st.type = strip(line.substr(20,20));
            st.accuracy = asDouble(line.substr(46,9));
            st.obsType = convertObsType(line.substr(57,2));
            
            sensorTypeList.push_back(st);
            
               // only set this valid if there are exactly
               // the same number in both lists
            if (sensorTypeList.size() == obsTypeList.size())
            {
               valid |= sensorTypeValid;
            }
            else
            {
               valid &= ~(long)sensorTypeValid;
            }
         }
         else if (thisLabel == sensorPosString)
         {
               // read XYZ and H and obs type
            sensorPosType sp;
            sp.position[0] = asDouble(line.substr(0,14));
            sp.position[1] = asDouble(line.substr(14,14));
            sp.position[2] = asDouble(line.substr(28,14));
            sp.height = asDouble(line.substr(42,14));
            
            sp.obsType = convertObsType(line.substr(57,2));
            
            sensorPosList.push_back(sp);
            
               // only barometer is required, so
               // set it valid only if you see that record.
            if (sp.obsType == PR)
            {
               valid |= sensorPosValid;
            }
         }
         else if (thisLabel == endOfHeader)
         {
            valid |= endValid;
         }
         else
         {
            FFStreamError e("Unknown header label " + thisLabel);
            GPSTK_THROW(e);
         }
      }
      
      unsigned long allValid;
      if      (version == 2.0)      allValid = allValid20;
      else if (version == 2.1)      allValid = allValid21;
      else
      {
         FFStreamError e("Unknown or unsupported RINEX version " + 
                         asString(version));
         GPSTK_THROW(e);
      }
      
      if ( (allValid & valid) != allValid)
      {
         string errstr("Incomplete or invalid header: missing: ");
         errstr += bitString(allValid & ~valid);
         FFStreamError err(errstr);
         GPSTK_THROW(err);               
      }
      
         // we got here, so something must be right...
      strm.header = *this;
      strm.headerRead = true;
   } 

   void RinexMetHeader::dump(ostream& s) const
   {
      s << "Marker " << markerName << endl;

      if (!obsTypeList.empty())
      {
         cout << "Obs types:" << endl;
         vector<RinexMetType>::const_iterator itr = obsTypeList.begin();
         while (itr != obsTypeList.end())
         {
            cout << convertObsType(*itr) << " ";
            itr++;
         }
         cout << endl;
      }
   }


   RinexMetHeader::RinexMetType 
   RinexMetHeader::convertObsType(const string& oneObs)
      throw(FFStreamError)
   {
      if      (oneObs == "PR") return PR;
      else if (oneObs == "TD") return TD;
      else if (oneObs == "HR") return HR;
      else if (oneObs == "ZW") return ZW;
      else if (oneObs == "ZD") return ZD;
      else if (oneObs == "ZT") return ZT;
      else
      {
         FFStreamError e("Bad obs type:" + oneObs);
         GPSTK_THROW(e);
      } 
   }

   string RinexMetHeader::convertObsType(const RinexMetHeader::RinexMetType& oneObs)
      throw(FFStreamError)
   {
      if      (oneObs == PR) return "PR";
      else if (oneObs == TD) return "TD";
      else if (oneObs == HR) return "HR";
      else if (oneObs == ZW) return "ZW";
      else if (oneObs == ZD) return "ZD";
      else if (oneObs == ZT) return "ZT";
      else
      {
         FFStreamError e("Bad obs type:" + oneObs);
         GPSTK_THROW(e);
      } 
   }


} // namespace
