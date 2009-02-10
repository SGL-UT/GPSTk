#pragma ident "$Id: RinexObsData.cpp 685 2007-07-16 19:19:35Z tconn $"

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
 * @file RinexObsData.cpp
 * Encapsulate RINEX observation file data, including I/O
 */

#include "StringUtils.hpp"
#include "RinexObsData.hpp"
#include "RinexObsStream.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
   void RinexObsData::reallyPutRecord(FFStream& ffs) const 
      throw(std::exception, FFStreamError, StringException)
   {
      // is there anything to write?
      if( (epochFlag==0 || epochFlag==1 || epochFlag==6)
         && (numSvs==0 || obs.empty()) ) return;
      if((epochFlag>=2 && epochFlag<=5) &&
            auxHeader.NumberHeaderRecordsToBeWritten()==0) return;

      RinexObsStream& strm = dynamic_cast<RinexObsStream&>(ffs);
      string line;

      // first the epoch line to 'line'
      line  = writeTime(time);
      line += string(2, ' ');
      line += rightJustify(asString<short>(epochFlag), 1);
      line += rightJustify(asString<short>(numSvs), 3);
         
      // write satellite ids to 'line'
      const int maxPrnsPerLine = 12;
      int satsWritten = 0;
      RinexSatMap::const_iterator obsItr = obs.begin();
      if(epochFlag==0 || epochFlag==1 || epochFlag==6) {
         while ((obsItr != obs.end()) && (satsWritten < maxPrnsPerLine))
         {
            RinexSatID prn((*obsItr).first);
            line += prn.toString();
            satsWritten++;
            obsItr++;
         }

            // add clock offset
         if(clockOffset != 0.0) {
            line += string(68 - line.size(), ' ');
            line += rightJustify(asString(clockOffset, 9), 12);
         }
      
        // continuation lines
         while (satsWritten != obs.size())
         {
            if ((satsWritten % maxPrnsPerLine) == 0)
            {
               strm << line << endl;
               strm.lineNumber++;
               line  = string(32, ' ');
            }
            RinexSatID prn(obsItr->first);
            line += prn.toString();
            satsWritten++;
            obsItr++;
         }
      }

      // write the epoch line
      strm << line << endl;
      strm.lineNumber++;         
      
      // write the auxiliary header records, if any
      if(epochFlag >= 2 && epochFlag <= 5)
      {
         try {
            auxHeader.WriteHeaderRecords(strm);
         }
         catch(FFStreamError& e)
         {
            GPSTK_RETHROW(e);
         }
         catch(StringException& e)
         {
            GPSTK_RETHROW(e);
         }
      }
      
      // write the obs data
      else if (!obs.empty())
      {
            // write out the observations themselves
         obsItr = obs.begin();
         
         const int maxObsPerLine = 5;
         
         while(obsItr != obs.end())
         {
            vector<RinexObsHeader::RinexObsType>::iterator obsTypeItr = 
               strm.header.obsTypeList.begin();
            
            line.erase();
            int obsWritten = 0;
            
            while (obsTypeItr != strm.header.obsTypeList.end())
            {
               if (  ((obsWritten % maxObsPerLine) == 0) &&
                     (obsWritten != 0))
               {
                  strm << line << endl;
                  strm.lineNumber++;
                  line.erase();
               }

               RinexObsTypeMap::const_iterator rotmi(obsItr->second.find(*obsTypeItr));
               RinexDatum thisData;
               if (rotmi != obsItr->second.end())
                  thisData = rotmi->second;
               line += rightJustify(asString(thisData.data,3),14);
               if (thisData.lli == 0)
                  line += string(1, ' ');
               else
                  line += rightJustify(asString<short>(thisData.lli),1);
               if (thisData.ssi == 0)
                  line += string(1, ' ');
               else
                  line += rightJustify(asString<short>(thisData.ssi),1);
               obsWritten++;
               obsTypeItr++;
            }
            strm << line << endl;
            strm.lineNumber++;
            obsItr++;
         }  
      }      
   }   // end RinexObsData::reallyPutRecord


   void RinexObsData::reallyGetRecord(FFStream& ffs) 
      throw(exception, FFStreamError, gpstk::StringUtils::StringException)
   {
      RinexObsStream& strm = dynamic_cast<RinexObsStream&>(ffs);
      
         // If the header hasn't been read, read it...
      if(!strm.headerRead) strm >> strm.header;
      
         // Clear out this object
      RinexObsHeader& hdr = strm.header;
      
      RinexObsData rod;
      *this=rod;
      
      string line;
      
      strm.formattedGetLine(line, true);
      
      if (line.size()>80 || line[0] != ' ' || line[3] != ' ' || line[6] != ' ')
      {
         FFStreamError e("Bad epoch line");
         GPSTK_THROW(e);
      }
      
         // process the epoch line, including SV list and clock bias
      epochFlag = asInt(line.substr(28,1));
      if ((epochFlag < 0) || (epochFlag > 6))
      {
         FFStreamError e("Invalid epoch flag: " + asString(epochFlag));
         GPSTK_THROW(e);
      }
      time = parseTime(line, hdr);
      numSvs = asInt(line.substr(29,3));
      
      if( line.size() > 68 )
         clockOffset = asDouble(line.substr(68, 12));
      else
         clockOffset = 0.0;
      
         // Now read the observations ...
      if(epochFlag==0 || epochFlag==1 || epochFlag==6) {
         int isv, ndx, line_ndx;
         vector<SatID> satIndex(numSvs);
         int col=30;
         for (isv=1, ndx=0; ndx<numSvs; isv++, ndx++) {
            if(! (isv % 13)) {
               strm.formattedGetLine(line);
               isv = 1;
               if(line.size() > 80) {
                  FFStreamError err("Invalid line size:" + asString(line.size()));
                  GPSTK_THROW(err);
               }
            }
            try {
               satIndex[ndx] = RinexSatID(line.substr(col+isv*3-1, 3));
            }
            catch (Exception& e)
            { 
               FFStreamError ffse(e);
               GPSTK_THROW(ffse);
            }
         }
      
         for (isv=0; isv < numSvs; isv++)
         {
            short numObs = hdr.obsTypeList.size();
            for (ndx=0, line_ndx=0; ndx < numObs; ndx++, line_ndx++)
            {
               SatID sat = satIndex[isv];
               RinexObsHeader::RinexObsType obs_type = hdr.obsTypeList[ndx];
               if (! (line_ndx % 5))
               {
                  strm.formattedGetLine(line);
                  line_ndx = 0;
                  if (line.size() > 80)
                  {
                     FFStreamError err("Invalid line size:" + asString(line.size()));
                     GPSTK_THROW(err);
                  }
               }
               
               line.resize(80, ' ');
               
               obs[sat][obs_type].data = asDouble(line.substr(line_ndx*16,   14));
               obs[sat][obs_type].lli = asInt(    line.substr(line_ndx*16+14, 1));
               obs[sat][obs_type].ssi = asInt(    line.substr(line_ndx*16+15, 1));
            }
         }
      }
         // ... or the auxiliary header information
      else if(numSvs > 0) {
         auxHeader.clear();
         for(int i=0; i<numSvs; i++) {
            strm.formattedGetLine(line);
            StringUtils::stripTrailing(line);
            try {
               auxHeader.ParseHeaderRecord(line);
            }
            catch(FFStreamError& e)
            {
               GPSTK_RETHROW(e);
            }
            catch(StringException& e)
            {
               GPSTK_RETHROW(e);
            }
         }
      }

      return;
      
   } // end of reallyGetRecord()


   DayTime RinexObsData::parseTime(const string& line, 
                                   const RinexObsHeader& hdr) const
      throw(FFStreamError)
   {
      try
      {
            // check if the spaces are in the right place - an easy
            // way to check if there's corruption in the file
         if ( (line[0] != ' ') ||
              (line[3] != ' ') ||
              (line[6] != ' ') ||
              (line[9] != ' ') ||
              (line[12] != ' ') ||
              (line[15] != ' '))
         {
            FFStreamError e("Invalid time format");
            GPSTK_THROW(e);
         }

            // if there's no time, just return a bad time
         if (line.substr(0,26) == string(26, ' '))
         {
            return DayTime(DayTime::BEGINNING_OF_TIME);
         }

         int year, month, day, hour, min;
         double sec;
         int yy = hdr.firstObs.year()/100;
         yy *= 100;
   
         year  = asInt(   line.substr(1,  2 ));
         month = asInt(   line.substr(4,  2 ));
         day   = asInt(   line.substr(7,  2 ));
         hour  = asInt(   line.substr(10, 2 ));
         min   = asInt(   line.substr(13, 2 ));
         sec   = asDouble(line.substr(15, 11));

         // Real Rinex has epochs 'yy mm dd hr 59 60.0' surprisingly often....
         double ds=0;
         if(sec >= 60.) { ds=sec; sec=0.0; }
         DayTime rv(yy+year, month, day, hour, min, sec);
         if(ds != 0) rv += ds;

         return rv;
      }
         // string exceptions for substr are caught here
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }
      catch (gpstk::Exception& e)
      {
         std::string text;
         for(int i=0; i<e.getTextCount(); i++) text += e.getText(i);
         FFStreamError err("gpstk::Exception in parseTime(): " + text);
         GPSTK_THROW(err);
      }

   }

   string RinexObsData::writeTime(const DayTime& dt) const
      throw(StringException)
   {
      if (dt == DayTime::BEGINNING_OF_TIME)
      {
         return string(26, ' ');
      }

      string line;
      line  = string(1, ' ');
      line += rightJustify(asString<short>(dt.year()),2);
      line += string(1, ' ');
      line += rightJustify(asString<short>(dt.month()),2);
      line += string(1, ' ');
      line += rightJustify(asString<short>(dt.day()),2);
      line += string(1, ' ');
      line += rightJustify(asString<short>(dt.hour()),2);
      line += string(1, ' ');
      line += rightJustify(asString<short>(dt.minute()),2);
      line += rightJustify(asString(dt.second(), 7),11);

      return line;
   }


   void RinexObsData::dump(ostream& s) const
   {
      if (obs.empty())
         return;

      s << "Dump of RinexObsData - time: ";
      s << writeTime(time) << " epochFlag: "
        << " " << epochFlag << " numSvs: " << numSvs 
        << fixed << setprecision(6)
        << " clk offset: " << clockOffset << endl;
      if(epochFlag == 0 || epochFlag == 1) 
      {
         RinexSatMap::const_iterator it;
         for(it=obs.begin(); it!=obs.end(); it++) {
            s << "Sat " << setw(2) << RinexSatID(it->first);
            RinexObsTypeMap::const_iterator jt;
            for(jt=it->second.begin(); jt!=it->second.end(); jt++) 
            {
               s << " " << jt->first.type << ":" << fixed << setprecision(3)
                 << " " << setw(12) << jt->second.data
                 << "/" << jt->second.lli << "/" << jt->second.ssi;
            }
            s << endl;
         }
      }
      else {
         s << "aux. header info:\n";
         auxHeader.dump(s);
      }
   }

} // namespace
