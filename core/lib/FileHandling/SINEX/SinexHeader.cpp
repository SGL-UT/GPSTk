#pragma ident "$Id: //depot/msn/main/code/shared/gpstk/SinexHeader.cpp#4 $"
//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file SinexHeader.cpp
 * Encapsulate header of SINEX file data, including I/O
 */

#include "StringUtils.hpp"
#include "SinexStream.hpp"
#include "SinexHeader.hpp"

namespace gpstk
{
namespace Sinex
{

   using namespace gpstk::StringUtils;
   using namespace std;

   const size_t  Header::MIN_LINE_LEN;
   const size_t  Header::MAX_LINE_LEN;


   Header::operator std::string() const
   {
      try
      {
         ostringstream  ss;
         ss << FILE_BEGIN;
         ss << ' ' << setw(4)  << fixed << setprecision(2) << version;
         ss << ' ' << setw(3)  << creationAgency;
         ss << ' ' << setw(12) << (std::string)creationTime;
         ss << ' ' << setw(3)  << dataAgency;
         ss << ' ' << setw(12) << (std::string)dataTimeStart;
         ss << ' ' << setw(12) << (std::string)dataTimeEnd;
         ss << ' ' << obsCode;
         ss << setfill('0');
         ss << ' ' << setw(5)  << setprecision(5) << paramCount;
         ss << setfill(' ');
         ss << ' ' << constraintCode;
         ss << ' ' << setw(6)  << solutionTypes;
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // Header::operator std::string()


   void Header::operator=(const std::string& line)
   {
      if (line.compare(0, FILE_BEGIN.size(), FILE_BEGIN) != 0)
      {
         Exception  err("Invalid Sinex Header");
         GPSTK_THROW(err);
      }
      static int FIELD_DIVS[] = {5, 10, 14, 27, 31, 44, 57, 59, 65, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         version = asFloat(line.substr(6,4) );
         creationAgency = line.substr(11,3);
         creationTime = line.substr(15,12);
         dataAgency = line.substr(28,3);
         dataTimeStart = line.substr(32,12);
         dataTimeEnd = line.substr(45,12);
         obsCode = line[58];
         isValidObsCode(obsCode);
         paramCount = asInt(line.substr(60,5) );
         constraintCode = line[66];
         isValidConstraintCode(constraintCode);
         if (line.size() > 67)
         {
            solutionTypes = line.substr(68,6);
            for (size_t i = 0; i < solutionTypes.size(); ++i)
            {
               isValidSolutionType(solutionTypes[i]);
            }
         }
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // Header::operator=()


   void Header::dump(ostream& s) const
   {
      s << "SINEX HEADER :" << endl;
      s << " version=" << version << endl;
      s << " creationAgency=" << creationAgency << endl;
      s << " dataAgency=" << dataAgency << endl;
      s << " creationTime=" << (std::string)creationTime << endl;
      s << " dataTimeStart=" << (std::string)dataTimeStart << endl;
      s << " dataEndTime=" << (std::string)dataTimeEnd << endl;
      s << " obsCode=" << obsCode << endl;
      s << " constraintCode=" << constraintCode << endl;
      s << " paramCount=" << paramCount << endl;
      s << " solutionTypes=" << solutionTypes << endl;

   }  // Header::dump()

}  // namespace Sinex

}  // namespace gpstk
