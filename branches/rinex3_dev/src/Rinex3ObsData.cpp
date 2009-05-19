#pragma ident "$Id: Rinex3ObsData.cpp 1709 2009-02-18 20:27:47Z btolman $"

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
 * @file Rinex3ObsData.cpp
 * Encapsulate RINEX 3 observation file data, including I/O.
 */

#include "StringUtils.hpp"
#include "CivilTime.hpp"
#include "ObsID.hpp"
#include "Rinex3ObsStream.hpp"
#include "Rinex3ObsData.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
  void Rinex3ObsData::reallyPutRecord(FFStream& ffs) const 
    throw(std::exception, FFStreamError, StringException)
  {
    // is there anything to write?
    if ( (epochFlag == 0 || epochFlag == 1 || epochFlag == 6)
         && (numSVs==0 || obs.empty())                        ) return;
    if ( (epochFlag >= 2 && epochFlag <= 5) &&
         auxHeader.NumberHeaderRecordsToBeWritten() == 0 ) return;

    Rinex3ObsStream& strm = dynamic_cast<Rinex3ObsStream&>(ffs);
    string line;

    // first the epoch line

    line  = ">";
    line += writeTime(time);
    line += string(2, ' ');
    line += rightJustify(asString<short>(epochFlag), 1);
    line += rightJustify(asString<short>(numSVs   ), 3);
    line += string(6, ' ');
    if (clockOffset != 0.0) // optional data; need to test for its existence
      line += rightJustify(asString(clockOffset, 12), 15);

    strm << line << endl;
    strm.lineNumber++;
    line.erase();

    if (epochFlag == 0 || epochFlag == 1 || epochFlag == 6)
    {
      // next the data lines

      DataMap::const_iterator itr = obs.begin();

      while (itr != obs.end())
      {
        line = itr->first.toString();

        for (int i = 0; i < itr->second.size(); i++)
        {
          RinexDatum thisData = itr->second[i];

          line += rightJustify(asString(thisData.data,3),14);

          if (thisData.lli == 0)
            line += string(1, ' ');
          else
            line += rightJustify(asString<short>(thisData.lli),1);

          if (thisData.ssi == 0)
            line += string(1, ' ');
          else
            line += rightJustify(asString<short>(thisData.ssi),1);
        }

        // write the data line out
        strm << line << endl;
        strm.lineNumber++;
        line.erase();

        itr++;
      } // end loop through itr

    }
    // write the auxiliary header records, if any
    else if (epochFlag >= 2 && epochFlag <= 5)
    {
      try
      {
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

  }   // end Rinex3ObsData::reallyPutRecord


  void Rinex3ObsData::reallyGetRecord(FFStream& ffs) 
    throw(exception, FFStreamError, gpstk::StringUtils::StringException)
  {
    Rinex3ObsStream& strm = dynamic_cast<Rinex3ObsStream&>(ffs);

    // If the header hasn't been read, read it.
    if (!strm.headerRead) strm >> strm.header;

    // Point to the correct header.
    Rinex3ObsHeader& hdr = strm.header;

    Rinex3ObsData rod;
    *this = rod;

    string line;

    strm.formattedGetLine(line, true);

    // Check for epoch marker ('>') and following space.
    if (line[0] != '>' || line[1] != ' ')
    {
      FFStreamError e("Bad epoch line");
      GPSTK_THROW(e);
    }

    // Process the epoch line, including #SVs and clock bias.

    epochFlag = asInt(line.substr(31,1));
    if (epochFlag < 0 || epochFlag > 6)
    {
      FFStreamError e("Invalid epoch flag: " + asString(epochFlag));
      GPSTK_THROW(e);
    }

    time = parseTime(line, hdr);
    numSVs = asInt(line.substr(32,3));

    if ( line.size() > 41 )
      clockOffset = asDouble(line.substr(41,15));
    else
      clockOffset = 0.0;

    // Now read the observations: SV ID and data.

    if (epochFlag == 0 || epochFlag == 1 || epochFlag == 6)
    {
      vector<RinexSatID> satIndex(numSVs);
      std::map<RinexSatID, vector<RinexDatum> > tempDataMap;

      for (int isv = 0; isv < numSVs; isv++)
      {
        strm.formattedGetLine(line);

        // get the SV ID
        try
        {
          satIndex[isv] = RinexSatID(line.substr(0,3));
        }
        catch (Exception& e)
        { 
          FFStreamError ffse(e);
          GPSTK_THROW(ffse);
        }

        // get the data (# entries in ObsType map of maps from header)
        vector<RinexDatum> data;
        std::string gnss = asString(satIndex[isv].systemChar());
        int size = hdr.mapObsTypes.find(gnss)->second.size();
        for (int i = 0; i < size; i++)
        {
          int pos = 3 + 16*i;
          
          RinexDatum tempData;
          tempData.data = asDouble(line.substr(pos   , 14));
          if ( line.size() > pos+14 )
            tempData.lli  = asInt( line.substr(pos+14,  1));
          if ( line.size() > pos+15 )
            tempData.ssi  = asInt( line.substr(pos+15,  1));
          data.push_back(tempData);
        }
        obs[satIndex[isv]] = data;
      }

    }
    // ... or the auxiliary header information
    else if (numSVs > 0)
    {
      auxHeader.clear();
      for (int i = 0; i < numSVs; i++)
      {
        strm.formattedGetLine(line);
        StringUtils::stripTrailing(line);
        try
        {
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


  CommonTime Rinex3ObsData::parseTime(const string& line, 
                                      const Rinex3ObsHeader& hdr) const
    throw(FFStreamError)
  {
    try
    {
      // check if the spaces are in the right place - an easy
      // way to check if there's corruption in the file
      if ( (line[ 1] != ' ') ||
           (line[ 6] != ' ') ||
           (line[ 9] != ' ') ||
           (line[12] != ' ') ||
           (line[15] != ' ') ||
           (line[18] != ' ') ||
           (line[29] != ' ') ||
           (line[30] != ' ')    )
      {
        FFStreamError e("Invalid time format");
        GPSTK_THROW(e);
      }

      // if there's no time, just return a bad time
      if (line.substr(2,27) == string(27, ' '))
        return CommonTime(CommonTime::BEGINNING_OF_TIME);

      int year, month, day, hour, min;
      double sec;

      year  = asInt(   line.substr( 2,  4));
      month = asInt(   line.substr( 7,  2));
      day   = asInt(   line.substr(10,  2));
      hour  = asInt(   line.substr(13,  2));
      min   = asInt(   line.substr(16,  2));
      sec   = asDouble(line.substr(19, 11));

      // Real Rinex has epochs 'yy mm dd hr 59 60.0' surprisingly often.
      double ds = 0;
      if (sec >= 60.) { ds = sec; sec = 0.0; }

      CommonTime rv = CivilTime(year, month, day, hour, min, sec).convertToCommonTime();
      if (ds != 0) rv += ds;

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
      for (int i=0; i<e.getTextCount(); i++) text += e.getText(i);
      FFStreamError err("gpstk::Exception in parseTime(): " + text);
      GPSTK_THROW(err);
    }
  }

  string Rinex3ObsData::writeTime(const CommonTime& ct) const
    throw(StringException)
  {
    if (ct == CommonTime::BEGINNING_OF_TIME)
      return string(26, ' ');

    CivilTime civtime = ct;

    string line;

    line  = string(1, ' ');
    line += rightJustify(asString<short>(civtime.year    ), 4);
    line += string(1, ' ');
    line += rightJustify(asString<short>(civtime.month   ), 2, '0');
    line += string(1, ' ');
    line += rightJustify(asString<short>(civtime.day     ), 2, '0');
    line += string(1, ' ');
    line += rightJustify(asString<short>(civtime.hour    ), 2, '0');
    line += string(1, ' ');
    line += rightJustify(asString<short>(civtime.minute  ), 2, '0');
    line += rightJustify(asString       (civtime.second,7),11);

    return line;
  }


  void Rinex3ObsData::dump(ostream& s) const
  {
    if (obs.empty())
      return;

    s << "Dump of Rinex3ObsData" << endl << " - time: " << writeTime(time)
      << " epochFlag: " << " " << epochFlag
      << " numSVs: " << numSVs 
      << fixed << setprecision(6) << " clk offset: " << clockOffset << endl;

    if (epochFlag == 0 || epochFlag == 1) 
    {
      CivilTime civtime(time);
      s << "Sat " << setw(2) << civtime.printf("%02m/%02d/%04Y %02H:%02M:%02S %P") << endl;;
      DataMap::const_iterator jt;
      for (jt = obs.begin(); jt != obs.end(); jt++) 
      {
        s << " " << (jt->first).toString() << ":" << fixed << setprecision(3);
        for (int i = 0; i < jt->second.size(); i++)
        {
          s << " " << setw(12) << jt->second[i].data
            << "/" << jt->second[i].lli << "/" << jt->second[i].ssi;
        }
        s << endl;
      }
    }
    else
    {
      s << "aux. header info:\n";
      auxHeader.dump(s);
    }
  }

} // namespace
