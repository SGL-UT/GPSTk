#pragma ident "$Id: Rinex3NavHeader.cpp 766 2007-09-20 18:32:38Z snelsen $"


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
  const string Rinex3NavHeader::versionString     = "RINEX VERSION / TYPE";
  const string Rinex3NavHeader::runByString       = "PGM / RUN BY / DATE";
  const string Rinex3NavHeader::commentString     = "COMMENT";
  const string Rinex3NavHeader::ionoCorrString    = "IONOSPHERIC CORR";
  const string Rinex3NavHeader::timeSysCorrString = "TIME SYSTEM CORR";
  const string Rinex3NavHeader::leapSecondsString = "LEAP SECONDS";
  const string Rinex3NavHeader::endOfHeader       = "END OF HEADER";


  void Rinex3NavHeader::reallyPutRecord(FFStream& ffs) const 
    throw(std::exception, FFStreamError, StringException)
  {
    Rinex3NavStream& strm = dynamic_cast<Rinex3NavStream&>(ffs);

    strm.header = (*this);

    unsigned long allValid;
    if (version == 3.0 || version == 3.00) allValid = allValid30;
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

    if (valid & versionValid)
      {
        line  = rightJustify(asString(version,4), 10);
        line += string(10, ' ');
        line += leftJustify(fileType, 20);
        line += string(19, ' ');
        line += satSys.substr(0,1) + string(19, ' ');
        line += versionString;
        strm << line << endl;
        strm.lineNumber++;
      }

    if (valid & runByValid) 
      {
        line  = leftJustify(fileProgram,20);
        line += leftJustify(fileAgency ,20);
        SystemTime sysTime;
        string curDate = (static_cast<CivilTime>(sysTime)).printf("%04Y%02m%02d %02H%02M%02S %P");
        line += leftJustify(curDate, 20);
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

    if (valid & ionoCorrValid)
      {
        if ( ionoCorrType == "GAL" )
          {
            line  = ionoCorrType;
            line += string(2, ' ');
            for (int i = 0; i < 3; i++)
              {
                line += rightJustify(doub2for(ionoParam1[i], 12, 2),12);  // should be 12.4
              }
            line += string(19, ' ');
            line += ionoCorrString;
            strm << line << endl;
            strm.lineNumber++;
          }
        else if ( ionoCorrType.substr(0,3) == "GPS" )
          {
            line  = ionoCorrType.substr(0,3) + "A";
            line += string(1, ' ');
            for (int i = 0; i < 4; i++)
              {
                line += rightJustify(doub2for(ionoParam1[i], 12, 2),12);  // should be 12.4
              }          
            line += string(7, ' ');
            line += ionoCorrString;
            strm << line << endl;
            strm.lineNumber++;

            line  = ionoCorrType.substr(0,3) + "B";
            line += string(1, ' ');
            for (int i = 0; i < 4; i++)
              {
                line += rightJustify(doub2for(ionoParam2[i], 12, 2),12);  // should be 12.4
              }          
            line += string(7, ' ');
            line += ionoCorrString;
            strm << line << endl;
            strm.lineNumber++;
          }
      }

    if (valid & timeSysCorrValid)
      {
        line  = timeSysCorrType;
        line += string(1, ' ');
        line += doub2for(A0, 17, 2); // These look suspicious.
        line += doub2for(A1, 16, 2); // Do they give 17.10 and 16.9? -DR
        line += rightJustify(asString(timeSysRefTime),7);
        line += rightJustify(asString(timeSysRefWeek),5);
        if ( timeSysCorrSBAS != "" )
          {
            line += string(1, ' ');
            line += leftJustify(asString(timeSysCorrSBAS),5);
            line += string(1, ' ');
            line += leftJustify(asString(timeSysUTCid),2);
            line += string(1, ' ');
          }
        else
          {
            line += string(10, ' ');
          }
        line += timeSysCorrString;
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

    if (valid & endValid)
      {
        line  = string(60,' ');
        line += endOfHeader;
        strm << line << endl;
        strm.lineNumber++;
      }

  } // end of reallyPutRecord


  void Rinex3NavHeader::reallyGetRecord(FFStream& ffs) 
    throw(std::exception, FFStreamError, StringException)
  {
    Rinex3NavStream& strm = dynamic_cast<Rinex3NavStream&>(ffs);

    // if already read, just return
    if (strm.headerRead == true)
      return;

    valid = 0;

    // clear out anything that was unsuccessfully read first
    commentList.clear();

    while (!(valid & endValid))
      {
        string line;
        strm.formattedGetLine(line);
        StringUtils::stripTrailing(line);

        if (line.length()==0) continue;
        else if (line.length()<60 || line.length()>80)
          {
            FFStreamError e("Invalid line length");
            GPSTK_THROW(e);
          }

        string thisLabel(line, 60, 20);

        if (thisLabel == versionString)
          {
            version = asDouble(line.substr(0,20));
            fileType =  strip(line.substr(20,20));
            satSys   =  strip(line.substr(40,20));
            if ( fileType[0] != 'N' && fileType[0] != 'n')
              {
                FFStreamError e("This isn't a RINEX Nav file.");
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
        else if (thisLabel == ionoCorrString)
          {
            ionoCorrType = strip(line.substr(0,4));
            if ( ionoCorrType == "GAL" )
              {
                for(int i = 0; i < 3; i++)
                  ionoParam1[i] = gpstk::StringUtils::for2doub(line.substr(5 + 12*i, 12));
              }
            else if ( ionoCorrType == "GPSA" )
              {
                for(int i = 0; i < 4; i++)
                  ionoParam1[i] = gpstk::StringUtils::for2doub(line.substr(5 + 12*i, 12));
              }
            else if ( ionoCorrType == "GPSB" )
              {
                for(int i = 0; i < 4; i++)
                  ionoParam2[i] = gpstk::StringUtils::for2doub(line.substr(5 + 12*i, 12));
              }
            else
              {
                FFStreamError e("The ionospheric correction data isn't valid.");
                GPSTK_THROW(e);
              }
            valid |= ionoCorrValid;
          }
        else if (thisLabel == timeSysCorrString)
          {
            timeSysCorrType = strip(line.substr(0,4));
            A0 = gpstk::StringUtils::for2doub(line.substr(5,17));
            A1 = gpstk::StringUtils::for2doub(line.substr(22,16));
            timeSysRefTime  = asInt(line.substr(38,7));
            timeSysRefWeek  = asInt(line.substr(45,5));
            timeSysCorrSBAS = strip(line.substr(51,6));
            timeSysUTCid    = asInt(line.substr(57,2));
            valid |= timeSysCorrValid;
          }
        else if (thisLabel == leapSecondsString)
          {
            leapSeconds = asInt(line.substr(0,6));
            valid |= leapSecondsValid;
          }
        else if (thisLabel == endOfHeader)
          {
            valid |= endValid;
          }
        else
          {
            throw(FFStreamError("Unknown header label at line " + 
                                asString<size_t>(strm.lineNumber)));
          }
      }

    unsigned long allValid;
    if (version == 3.0 && version == 3.00) allValid = allValid30;
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

    if (!(valid & versionValid)) s << " Version is NOT valid\n";
    if (!(valid & runByValid  )) s << " Run by is NOT valid\n";
    if (!(valid & endValid    )) s << " End is NOT valid\n";

    s << "---------------------------------- OPTIONAL ----------------------------------\n";

    if (valid & ionoCorrValid)
      {
        if ( ionoCorrType == "GAL" )
          {
            s << "Iono Corr for Galileo:";
            for(i=0; i<3; i++) s << " " << scientific << setprecision(4) << ionoParam1[i];
          }
        else if ( ionoCorrType.substr(0,3) == "GPS" )
          {
            s << "Iono Corr Alpha for GPS:";
            for(i=0; i<4; i++) s << " " << scientific << setprecision(4) << ionoParam1[i];
            s << endl;
            s << "Iono Corr Beta afor GPS:";
            for(i=0; i<4; i++) s << " " << scientific << setprecision(4) << ionoParam2[i];
          }
        s << endl;
      }
    else s << " Iono Corr is NOT valid\n";

    if(valid & timeSysCorrValid) s << "Time System Corr type " << timeSysCorrType << ", A0="
                                   << scientific << setprecision(12) << A0 << ", A1="
                                   << scientific << setprecision(12) << A1 << ", UTC ref = ("
                                   << timeSysRefWeek << "," << timeSysRefTime << ")\n";
    else s << " Time System Corr is NOT valid\n";

    if (valid & leapSecondsValid) s << "Leap seconds: " << leapSeconds << endl;
    else s << " Leap seconds is NOT valid\n";

    if (commentList.size() > 0)
      {
        s << "Comments (" << commentList.size() << ") :\n";
        for (int i=0; i<commentList.size(); i++)
          s << commentList[i] << endl;
      }

    s << "-------------------------------- END OF HEADER -------------------------------\n";
  } // end of dump

} // namespace
