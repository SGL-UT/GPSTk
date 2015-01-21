#pragma ident "$Id: //depot/msn/main/code/shared/gpstk/SinexHeader.cpp#4 $"

/**
 * @file SinexHeader.cpp
 * Encapsulate header of SINEX file data, including I/O
 */

//lgpl-license START
//lgpl-license END

//dod-release-statement START
//dod-release-statement END

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
