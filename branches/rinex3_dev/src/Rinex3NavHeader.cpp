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
 * @file Rinex3NavHeader.cpp
 * Encapsulate header of Rinex 3 navigation file
 */

#include "StringUtils.hpp"
#include "SystemTime.hpp"
#include "CivilTime.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavStream.hpp"

#include <iostream>

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
  const string Rinex3NavHeader::stringVersion     = "RINEX VERSION / TYPE";
  const string Rinex3NavHeader::stringRunBy       = "PGM / RUN BY / DATE";
  const string Rinex3NavHeader::stringComment     = "COMMENT";
  const string Rinex3NavHeader::stringIonoCorr    = "IONOSPHERIC CORR";
  const string Rinex3NavHeader::stringTimeSysCorr = "TIME SYSTEM CORR";
  const string Rinex3NavHeader::stringLeapSeconds = "LEAP SECONDS";
  const string Rinex3NavHeader::stringEoH         = "END OF HEADER";

  /// Strings list for above Time System Correction enum.
  const std::string Rinex3NavHeader::timeSysCorrStrings[GLGP+1] =
  {
    std::string("GAUT"),
    std::string("GPUT"),
    std::string("SBUT"),
    std::string("GLUT"),
    std::string("GPGA"),
    std::string("GLGP")
  };


  void Rinex3NavHeader::setTimeSysCorrFromString(const std::string str)
    throw()
  {
    for (int i = 0; i <= GLGP; i++)
    {
      if (timeSysCorrStrings[i] == str)
      {
        timeSysCorrEnum = static_cast<TimeSysCorrEnum>(i);
        break;
      }
    }
  }

  //--------------------------------------------------------------------------
  // Keeps only one ephemeris with a given IODC/time.
  //--------------------------------------------------------------------------

  void Rinex3NavHeader::addTimeSysCorr(const TimeSysCorrInfo& tsci)
    throw()
  {
    tscMap[timeSysCorrEnum] = tsci;
  }


  void Rinex3NavHeader::reallyPutRecord(FFStream& ffs) const 
    throw(std::exception, FFStreamError, StringException)
  {
    Rinex3NavStream& strm = dynamic_cast<Rinex3NavStream&>(ffs);

    strm.header = (*this);

    unsigned long allValid;
    if (version == 3.0) allValid = allValid30;
    else
    {
      FFStreamError err("Unknown RINEX version: " + asString(version,3));
      err.addText("Make sure to set the version correctly.");
      GPSTK_THROW(err);
    }

    if ((valid & allValid) != allValid)
    {
      FFStreamError err("Incomplete or invalid header.");
      err.addText("Make sure you set all header valid bits for all of the available data.");
      GPSTK_THROW(err);
    }

    string line;

    if (valid & validVersion)
    {
      line  = rightJustify(asString(version,2), 10);
      line += string(10, ' ');
      line += leftJustify(fileType, 20);
      line += satSys.substr(0,1) + string(19, ' ');
      line += leftJustify(stringVersion,20);
      strm << line << endl;
      strm.lineNumber++;
    }

    if (valid & validRunBy) 
    {
      line  = leftJustify(fileProgram,20);
      line += leftJustify(fileAgency ,20);
      SystemTime sysTime;
      string curDate = (static_cast<CivilTime>(sysTime)).printf("%04Y%02m%02d %02H%02M%02S %P");
      line += leftJustify(curDate, 20);
      line += leftJustify(stringRunBy,20);
      strm << line << endl;
      strm.lineNumber++;
    }

    if (valid & validComment)
    {
      vector<string>::const_iterator itr = commentList.begin();
      while (itr != commentList.end())
      {
        line  = leftJustify((*itr), 60);
        line += leftJustify(stringComment,20);
        strm << line << endl;
        strm.lineNumber++;
        itr++;
      }
    }

    if (valid & validIonoCorrGal)
    {
      line  = "GAL  ";
      for (int i = 0; i < 3; i++)
      {
        line += rightJustify(doub2for(ionoParamGal[i], 12, 2),12);
      }
      line += string(19, ' ');
      line += leftJustify(stringIonoCorr,20);
      strm << line << endl;
      strm.lineNumber++;
    }

    if (valid & validIonoCorrGPS)
    {
      line  = "GPSA ";
      for (int i = 0; i < 4; i++)
      {
        line += rightJustify(doub2for(ionoParam1[i], 12, 2),12);
      }          
      line += string(7, ' ');
      line += leftJustify(stringIonoCorr,20);
      strm << line << endl;
      strm.lineNumber++;

      line  = "GPSB ";
      for (int i = 0; i < 4; i++)
      {
        line += rightJustify(doub2for(ionoParam2[i], 12, 2),12);
      }          
      line += string(7, ' ');
      line += leftJustify(stringIonoCorr,20);
      strm << line << endl;
      strm.lineNumber++;
    }

    if (valid & validTimeSysCorr)
    {
      TimeSysCorrMap::const_iterator iter;

      for (iter = tscMap.begin(); iter != tscMap.end(); iter++)
      {
        TimeSysCorrInfo info = iter->second;

        line  = info.timeSysCorrType;
        line += string(1, ' ');
        line += doub2for(info.A0, 17, 2);
        line += doub2for(info.A1, 16, 2);
        line += rightJustify(asString(info.timeSysRefTime),7);
        line += rightJustify(asString(info.timeSysRefWeek),5);
        if ( info.timeSysCorrSBAS != "" )
        {
          line += string(1, ' ');
          line += leftJustify(asString(info.timeSysCorrSBAS),5);
          line += string(1, ' ');
          line += leftJustify(asString(info.timeSysUTCid),2);
          line += string(1, ' ');
        }
        else
        {
          line += string(10, ' ');
        }
        line += leftJustify(stringTimeSysCorr,20);
        strm << line << endl;
        strm.lineNumber++;
      }
    }

    if (valid & validLeapSeconds)
    {
      line  = rightJustify(asString(leapSeconds),6);
      line += string(54, ' ');
      line += leftJustify(stringLeapSeconds,20);
      strm << line << endl;
      strm.lineNumber++;
    }

    if (valid & validEoH)
    {
      line  = string(60,' ');
      line += leftJustify(stringEoH,20);
      strm << line << endl;
      strm.lineNumber++;
    }

  } // end of reallyPutRecord


  void Rinex3NavHeader::reallyGetRecord(FFStream& ffs) 
    throw(std::exception, FFStreamError, StringException)
  {
    Rinex3NavStream& strm = dynamic_cast<Rinex3NavStream&>(ffs);

    // if already read, just return
    if (strm.headerRead == true) return;

    valid = 0;

    // clear out anything that was unsuccessfully read first
    commentList.clear();

    while (!(valid & validEoH))
    {
      string line;
      strm.formattedGetLine(line);
      StringUtils::stripTrailing(line);

      if (line.length() == 0) continue;
      else if (line.length() < 60 || line.length() > 80)
      {
        FFStreamError e("Invalid line length");
        GPSTK_THROW(e);
      }

      string thisLabel(line, 60, 20);

      if (thisLabel == stringVersion)
      {
        version  = asDouble(line.substr( 0,20));
        fileType =    strip(line.substr(20,20));
        satSys   =    strip(line.substr(40,20));
        if ( fileType[0] != 'N' && fileType[0] != 'n' )
        {
          FFStreamError e("This isn't a RINEX 3 Nav file.");
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
      else if (thisLabel == stringRunBy)
      {
        fileProgram = strip(line.substr( 0,20));
        fileAgency  = strip(line.substr(20,20));
        date        = strip(line.substr(40,20));
        valid |= validRunBy;
      }
      else if (thisLabel == stringComment)
      {
        commentList.push_back(strip(line.substr(0,60)));
        valid |= validComment;
      }
      else if (thisLabel == stringIonoCorr)
      {
        ionoCorrType = strip(line.substr(0,4));
        if ( ionoCorrType == "GAL" )
        {
          for (int i = 0; i < 3; i++)
            ionoParamGal[i] = gpstk::StringUtils::for2doub(line.substr(5 + 12*i, 12));
          valid |= validIonoCorrGal;
        }
        else if ( ionoCorrType == "GPSA" )
        {
          for (int i = 0; i < 4; i++)
            ionoParam1[i] = gpstk::StringUtils::for2doub(line.substr(5 + 12*i, 12));
        }
        else if ( ionoCorrType == "GPSB" )
        {
          for (int i = 0; i < 4; i++)
            ionoParam2[i] = gpstk::StringUtils::for2doub(line.substr(5 + 12*i, 12));
          valid |= validIonoCorrGPS; /// Assumes that GPSA always appears first. [DR]
        }
        else
        {
          FFStreamError e("The ionospheric correction data isn't valid.");
          GPSTK_THROW(e);
        }
      }
      else if (thisLabel == stringTimeSysCorr)
      {
        TimeSysCorrInfo info;

        timeSysCorrType = strip(line.substr(0,4));
        info.timeSysCorrType = timeSysCorrType;
        info.A0 = gpstk::StringUtils::for2doub(line.substr(5,17));
        info.A1 = gpstk::StringUtils::for2doub(line.substr(22,16));
        info.timeSysRefTime  = asInt(line.substr(38,7));
        info.timeSysRefWeek  = asInt(line.substr(45,5));
        info.timeSysCorrSBAS = strip(line.substr(51,6));
        info.timeSysUTCid    = asInt(line.substr(57,2));
        valid |= validTimeSysCorr;

        setTimeSysCorrFromString(timeSysCorrType);
        addTimeSysCorr(info);
      }
      else if (thisLabel == stringLeapSeconds)
      {
        leapSeconds = asInt(line.substr(0,6));
        valid |= validLeapSeconds;
      }
      else if (thisLabel == stringEoH)
      {
        valid |= validEoH;
      }
      else
      {
        throw(FFStreamError("Unknown header label at line " + 
                            asString<size_t>(strm.lineNumber)));
      }
    }

    unsigned long allValid;
    if (version == 3.0) allValid = allValid30;
    else
    {
      FFStreamError e("Unknown or unsupported RINEX version " + asString(version));
      GPSTK_THROW(e);
    }

    if ( (allValid & valid) != allValid)
    {
      FFStreamError e("Incomplete or invalid header");
      GPSTK_THROW(e);
    }

    // we got here, so something must be right...
    strm.header = *this;
    strm.headerRead = true;

  } // end of reallyGetRecord


  void Rinex3NavHeader::dump(ostream& s) const
  {
    int i;

    s << "---------------------------------- REQUIRED ----------------------------------\n";

    s << "Rinex Version " << fixed << setw(5) << setprecision(2) << version
      << ",  File type " << fileType << ".\n";
    s << "Prgm: " << fileProgram << ",  Run: " << date << ",  By: " << fileAgency << endl;

    s << "(This header is ";
    if ((valid & allValid30) == allValid30) s << "VALID 3.0";
    else s << "NOT VALID";
    s << " Rinex.)\n";

    if (!(valid & validVersion)) s << " Version is NOT valid\n";
    if (!(valid & validRunBy  )) s << " Run by is NOT valid\n";
    if (!(valid & validEoH    )) s << " End of Header is NOT valid\n";

    s << "---------------------------------- OPTIONAL ----------------------------------\n";

    if (valid & validIonoCorrGal)
    {
      s << "Iono Corr for Galileo:";
      for(i=0; i<3; i++) s << " " << scientific << setprecision(4) << ionoParamGal[i];
      s << endl;
    }

    if (valid & validIonoCorrGPS)
    {
      s << "Iono Corr Alpha for GPS:";
      for(i=0; i<4; i++) s << " " << scientific << setprecision(4) << ionoParam1[i];
      s << endl;
      s << "Iono Corr Beta afor GPS:";
      for(i=0; i<4; i++) s << " " << scientific << setprecision(4) << ionoParam2[i];
      s << endl;
    }

    if ( !(valid & validIonoCorrGal) && !(valid & validIonoCorrGPS) )
      s << "Iono Corr is NOT valid\n";

    if(valid & validTimeSysCorr)
    {
      TimeSysCorrMap::const_iterator iter;

      for (iter = tscMap.begin(); iter != tscMap.end(); iter++)
      {
        TimeSysCorrInfo info = iter->second;

        s << "Time System Corr type " << info.timeSysCorrType << ", A0="
          << scientific << setprecision(12) << info.A0 << ", A1="
          << scientific << setprecision(12) << info.A1 << ", UTC ref = ("
          << info.timeSysRefWeek << "," << info.timeSysRefTime << ")\n";
      }
    }
    else s << " Time System Corr is NOT valid\n";

    if (valid & validLeapSeconds) s << "Leap seconds: " << leapSeconds << endl;
    else s << " Leap seconds is NOT valid\n";

    if (commentList.size() > 0)
    {
      s << "Comments (" << commentList.size() << ") :\n";
      for (int i = 0; i < commentList.size(); i++)
        s << commentList[i] << endl;
    }

    s << "-------------------------------- END OF HEADER -------------------------------\n";
  } // end of dump

} // namespace
