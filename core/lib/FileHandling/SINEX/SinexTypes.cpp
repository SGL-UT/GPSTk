#pragma ident "$Id: //depot/msn/main/code/shared/gpstk/SinexTypes.cpp#7 $"

//lgpl-license START
//lgpl-license END

//dod-release-statement START
//dod-release-statement END

/**
 * @file SinexData.cpp
 * Encapsulate SINEX data types, including I/O
 */

#include "StringUtils.hpp"
#include "SinexTypes.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
namespace Sinex
{

   const std::string  FileReference::BLOCK_TITLE("FILE/REFERENCE");
   const size_t       FileReference::MIN_LINE_LEN;
   const size_t       FileReference::MAX_LINE_LEN;


   FileReference::operator std::string() const
   {
      try
      {
         ostringstream  ss;
         ss << DATA_START << formatStr(infoType, 18);
         ss << ' ' << formatStr(infoValue, 60);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // FileReference::operator std::string()


   void
   FileReference::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 19, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         infoType  = line.substr(1, 18);
         infoValue = line.substr(20, 60);
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // FileReference::operator=()


   void
   FileReference::dump(std::ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " infoType=" << infoType  << endl;
      s << " infoValue=" << infoValue << endl;

   }  // FileReference::dump()


   const std::string  FileComment::BLOCK_TITLE("FILE/COMMENT");
   const size_t       FileComment::MIN_LINE_LEN;
   const size_t       FileComment::MAX_LINE_LEN;


   FileComment::operator std::string() const
   {
      try
      {
         ostringstream  ss;
         ss << DATA_START << formatStr(comment, 79, true);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // FileComment::operator std::string()


   void
   FileComment::operator=(const std::string& line)
   {
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN);
         comment = line.substr(1, 79);
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // FileComment::operator=()


   void
   FileComment::dump(std::ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " comment=" << comment << endl;

   }  // FileComment::dump()


   const std::string  InputHistory::BLOCK_TITLE("INPUT/HISTORY");
   const size_t       InputHistory::MIN_LINE_LEN;
   const size_t       InputHistory::MAX_LINE_LEN;

   InputHistory::operator std::string() const
   {
      try
      {
         const Sinex::Header  *ptr = this;
         string  header = (std::string)(*ptr);
         header[0] = DATA_START;
         header[1] = fileCode;
         return header;
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // InputHistory::operator std::string()


   void
   InputHistory::operator=(const std::string& line)
   {
      if (line.size() > 1)
      {
         string  header(line);
         fileCode = header[1];
         header[0] = HEAD_TAIL_START;
         header[1] = '=';
         try
         {
            Sinex::Header::operator=(header);
         }
         catch (Exception& exc)
         {
            GPSTK_RETHROW(exc);
         }
      }
      else
      {
         ostringstream  ss;
         ss << "Missing data; inadequate line length ("
            << line.size() << " < " << MIN_LINE_LEN << ")";
         Exception  exc(ss.str() );
         GPSTK_THROW(exc);
      }
   }  // InputHistory::operator=()


   void
   InputHistory::dump(std::ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " fileCode=" << fileCode << endl;
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

   }  // InputHistory::dump()


   const std::string  InputFile::BLOCK_TITLE("INPUT/FILES");
   const size_t       InputFile::MIN_LINE_LEN;
   const size_t       InputFile::MAX_LINE_LEN;


   InputFile::operator std::string() const
   {
      try
      {
         ostringstream  ss;
         ss << DATA_START << formatStr(agencyCode, 3);
         ss << ' ' << (std::string)creationTime;
         ss << ' ' << formatStr(fileName, 29);
         ss << ' ' << formatStr(fileDesc, 32);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // InputFile::operator std::string()


   void
   InputFile::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 4, 17, 47, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         agencyCode   = line.substr(1, 3);
         creationTime = line.substr(5, 12);
         fileName     = line.substr(18, 29);
         fileDesc     = line.substr(48, 32);
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // InputFile::operator=()


   void
   InputFile::dump(std::ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " agencyCode=" << agencyCode << endl;
      s << " creationTime=" << (std::string)creationTime << endl;
      s << " fileName=" << fileName << endl;
      s << " fileDesc=" << fileDesc << endl;

   }  // InputFile::dump()


   const std::string  InputAck::BLOCK_TITLE("INPUT/ACKNOWLEDGMENTS");
   const size_t       InputAck::MIN_LINE_LEN;
   const size_t       InputAck::MAX_LINE_LEN;


   InputAck::operator std::string() const
   {
      try
      {
         ostringstream  ss;
         ss << DATA_START << formatStr(agencyCode, 3);
         ss << ' ' << formatStr(agencyDesc, 75);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // InputAck::operator std::string()


   void
   InputAck::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 4, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         agencyCode = line.substr(1, 3);
         agencyDesc = line.substr(5, 75);
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // InputAck::operator=()


   void
   InputAck::dump(std::ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " agencyCode=" << agencyCode << endl;
      s << " agencyDesc=" << agencyDesc << endl;

   }  // InputAck::dump()


   const std::string  NutationData::BLOCK_TITLE("NUTATION/DATA");
   const size_t       NutationData::MIN_LINE_LEN;
   const size_t       NutationData::MAX_LINE_LEN;


   NutationData::operator std::string() const
   {
      try
      {
         ostringstream  ss;
         ss << DATA_START << formatStr(nutationCode, 8);
         ss << ' ' << formatStr(nutationDesc, 70);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // NutationData::operator std::string()


   void
   NutationData::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 9, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         nutationCode = line.substr(1, 8);
         nutationDesc = line.substr(10, 70);
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // NutationData::operator=()


   void
   NutationData::dump(std::ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " nutationCode=" << nutationCode << endl;
      s << " nutationDesc=" << nutationDesc << endl;

   }  // NutationData::dump()


   const std::string  PrecessionData::BLOCK_TITLE("PRECESSION/DATA");
   const size_t       PrecessionData::MIN_LINE_LEN;
   const size_t       PrecessionData::MAX_LINE_LEN;


   PrecessionData::operator std::string() const
   {
      try
      {
         ostringstream  ss;
         ss << DATA_START << formatStr(precessionCode, 8);
         ss << ' ' << formatStr(precessionDesc, 70);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // Precession::operator std::string()


   void
   PrecessionData::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 9, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         precessionCode = line.substr(1, 8);
         precessionDesc = line.substr(10, 70);
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // PrecessionData::operator=()


   void
   PrecessionData::dump(std::ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " precessionCode=" << precessionCode << endl;
      s << " precessionDesc=" << precessionDesc << endl;

   }  // PrecessionData::dump()


   const std::string  SourceId::BLOCK_TITLE("SOURCE/ID");
   const size_t       SourceId::MIN_LINE_LEN;
   const size_t       SourceId::MAX_LINE_LEN;


   SourceId::operator std::string() const
   {
      try
      {
         ostringstream  ss;
         ss << DATA_START << formatStr(sourceCode, 4);
         ss << ' ' << formatStr(iers, 8);
         ss << ' ' << formatStr(icrf, 16);
         ss << ' ' << formatStr(comment, 48);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SourceId::operator std::string()


   void
   SourceId::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 5, 14, 31, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         sourceCode = line.substr(1, 4);
         iers       = line.substr(6, 8);
         icrf       = line.substr(15, 16);
         comment    = line.substr(32, 48);
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SourceId::operator=()


   void
   SourceId::dump(std::ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " sourceCode=" << sourceCode << endl;
      s << " iers=" << iers << endl;
      s << " icrf=" << icrf << endl;
      s << " comment=" << comment << endl;

   }  // SourceId::dump()


   const string  SiteId::BLOCK_TITLE("SITE/ID");
   const size_t  SiteId::MIN_LINE_LEN;
   const size_t  SiteId::MAX_LINE_LEN;


   SiteId::operator std::string() const
   {
      try
      {
         std::ostringstream  ss;
         ss << DATA_START << formatStr(siteCode, 4);
         ss << ' ' << formatStr(pointCode, 2);
         ss << ' ' << formatStr(monumentId, 9);
         ss << ' ' << obsCode;
         ss << ' ' << formatStr(siteDesc, 22);
         ss << ' ' << formatUint(longitudeDeg, 3);
         ss << ' ' << formatUint(longitudeMin, 2);
         ss << ' ' << formatFixed(longitudeSec, 4, 1);
         ss << ' ' << formatInt(latitudeDeg, 3);
         ss << ' ' << formatUint(latitudeMin, 2);
         ss << ' ' << formatFixed(latitudeSec, 4, 1);
         ss << ' ' << formatFixed(height, 7, 1);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SiteId::operator std::string()


   void SiteId::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 5, 8, 18, 20, 43, 47, 50, 55, 59, 62, 67, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         siteCode     = line.substr(1, 4);
         pointCode    = line.substr(6, 2);
         monumentId   = line.substr(9, 9);
         obsCode      = line[19];
         isValidObsCode(obsCode);
         siteDesc     = line.substr(21, 22);
         longitudeDeg = asUnsigned(line.substr(44, 3) );
         longitudeMin = asUnsigned(line.substr(48, 2) );
         longitudeSec = asFloat(line.substr(51, 4) );
         latitudeDeg  = asInt(line.substr(56, 3) );
         latitudeMin  = asUnsigned(line.substr(60, 2) );
         latitudeSec  = asFloat(line.substr(63, 4) );
         height       = asDouble(line.substr(68, 7) );
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SiteId::operator=()


   void SiteId::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " siteCode=" << siteCode << endl;
      s << " pointCode=" << pointCode << endl;
      s << " monumentId=" << monumentId << endl;
      s << " obsCode=" << obsCode  << endl;
      s << " siteDesc=" << siteDesc << endl;
      s << " longitudeDeg=" << longitudeDeg << endl;
      s << " longitudeMin=" << (uint16_t)longitudeMin << endl;
      s << " longitudeSec=" << longitudeSec << endl;
      s << " latitudeDeg=" << latitudeDeg << endl;
      s << " latitudeMin=" << (uint16_t)latitudeMin << endl;
      s << " latitudeSec=" << latitudeSec << endl;
      s << " height=" << height << endl;

   }  // SiteId::dump()


   const string  SiteData::BLOCK_TITLE("SITE/DATA");
   const size_t  SiteData::MIN_LINE_LEN;
   const size_t  SiteData::MAX_LINE_LEN;


   SiteData::operator std::string() const
   {
      try
      {
         std::ostringstream  ss;
         ss << DATA_START << formatStr(siteCodeSol, 4);
         ss << ' ' << formatStr(pointCodeSol, 2);
         ss << ' ' << formatStr(solutionIdSol, 4);
         ss << ' ' << formatStr(siteCodeInp, 4);
         ss << ' ' << formatStr(pointCodeInp, 2);
         ss << ' ' << formatStr(solutionIdInp, 4);
         ss << ' ' << obsCode;
         ss << ' ' << setw(12) << right << (std::string)timeStart;
         ss << ' ' << setw(12) << right << (std::string)timeEnd;
         ss << ' ' << formatStr(agencyCode, 3);
         ss << ' ' << setw(12) << right << (std::string)creationTime;
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SiteData::operator std::string()


   void SiteData::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 5, 8, 13, 18, 21, 26, 28, 41, 54, 58, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         siteCodeSol   = line.substr(1, 4);
         pointCodeSol  = line.substr(6, 2);
         solutionIdSol = line.substr(9, 4);
         siteCodeInp   = line.substr(14, 4);
         pointCodeInp  = line.substr(19, 2);
         solutionIdInp = line.substr(22, 4);
         obsCode       = line[27];
         isValidObsCode(obsCode);
         timeStart     = line.substr(29,12);
         timeEnd       = line.substr(42,12);
         agencyCode    = line.substr(55, 3);
         creationTime  = line.substr(59,12);
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SiteData::operator=()


   void SiteData::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " siteCodeSol=" << siteCodeSol << endl;
      s << " pointCodeSol=" << pointCodeSol << endl;
      s << " solutionIdSol=" << solutionIdSol << endl;
      s << " siteCodeInp=" << siteCodeInp << endl;
      s << " pointCodeInp=" << pointCodeInp << endl;
      s << " solutionIdInp=" << solutionIdInp << endl;
      s << " obsCode=" << obsCode  << endl;
      s << " timeStart=" << (std::string)timeStart << endl;
      s << " timeEnd=" << (std::string)timeEnd << endl;
      s << " agencyCode=" << agencyCode << endl;
      s << " creationTime=" << (std::string)creationTime << endl;

   }  // SiteData::dump()


   const string  SiteReceiver::BLOCK_TITLE("SITE/RECEIVER");
   const size_t  SiteReceiver::MIN_LINE_LEN;
   const size_t  SiteReceiver::MAX_LINE_LEN;


   SiteReceiver::operator std::string() const
   {
      try
      {
         std::ostringstream  ss;
         ss << DATA_START << formatStr(siteCode, 4);
         ss << ' ' << formatStr(pointCode, 2);
         ss << ' ' << formatStr(solutionId, 4);
         ss << ' ' << obsCode;
         ss << ' ' << setw(12) << right << (std::string)timeSince;
         ss << ' ' << setw(12) << right << (std::string)timeUntil;
         ss << ' ' << formatStr(rxType, 20);
         ss << ' ' << formatStr(rxSerialNo, 5);
         ss << ' ' << formatStr(rxFirmware, 11);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SiteReceiver::operator std::string()


   void SiteReceiver::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 5, 8, 13, 15, 28, 41, 62, 68, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         siteCode   = line.substr(1, 4);
         pointCode  = line.substr(6, 2);
         solutionId = line.substr(9, 4);
         obsCode    = line[14];
         isValidObsCode(obsCode);
         timeSince  = line.substr(16,12);
         timeUntil  = line.substr(29,12);
         rxType     = line.substr(42, 20);
         rxSerialNo = line.substr(63, 5);
         rxFirmware = line.substr(69, 11);
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SiteReceiver::operator=()


   void SiteReceiver::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " siteCode=" << siteCode << endl;
      s << " pointCode=" << pointCode << endl;
      s << " solutionId=" << solutionId << endl;
      s << " obsCode=" << obsCode  << endl;
      s << " timeSince=" << (std::string)timeSince << endl;
      s << " timeUntil=" << (std::string)timeUntil << endl;
      s << " rxType=" << rxType << endl;
      s << " rxSerialNo=" << rxSerialNo << endl;
      s << " rxFirmware=" << rxFirmware << endl;

   }  // SiteReceiver::dump()


   const string  SiteAntenna::BLOCK_TITLE("SITE/ANTENNA");
   const size_t  SiteAntenna::MIN_LINE_LEN;
   const size_t  SiteAntenna::MAX_LINE_LEN;


   SiteAntenna::operator std::string() const
   {
      try
      {
         std::ostringstream  ss;
         ss << DATA_START << formatStr(siteCode, 4);
         ss << ' ' << formatStr(pointCode, 2);
         ss << ' ' << formatStr(solutionId, 4);
         ss << ' ' << obsCode;
         ss << ' ' << setw(12) << right << (std::string)timeSince;
         ss << ' ' << setw(12) << right << (std::string)timeUntil;
         ss << ' ' << formatStr(antennaType, 20);
         ss << ' ' << formatStr(antennaSerialNo, 5);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SiteAntenna::operator std::string()


   void SiteAntenna::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 5, 8, 13, 15, 28, 41, 62, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         siteCode     = line.substr(1, 4);
         pointCode    = line.substr(6, 2);
         solutionId   = line.substr(9, 4);
         obsCode      = line[14];
         isValidObsCode(obsCode);
         timeSince    = line.substr(16,12);
         timeUntil    = line.substr(29,12);
         antennaType  = line.substr(42, 20);
         antennaSerialNo = line.substr(63, 5);
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SiteAntenna::operator=()


   void SiteAntenna::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " siteCode=" << siteCode << endl;
      s << " pointCode=" << pointCode << endl;
      s << " solutionId=" << solutionId << endl;
      s << " obsCode=" << obsCode  << endl;
      s << " timeSince=" << (std::string)timeSince << endl;
      s << " timeUntil=" << (std::string)timeUntil << endl;
      s << " antennaType=" << antennaType << endl;
      s << " antennaSerialNo=" << antennaSerialNo << endl;

   }  // SiteAntenna::dump()


   const size_t  SitePhaseCenter::MIN_LINE_LEN;
   const size_t  SitePhaseCenter::MAX_LINE_LEN;


   SitePhaseCenter::operator std::string() const
   {
      try
      {
         std::ostringstream  ss;
         ss << DATA_START << formatStr(antennaType, 20);
         ss << ' ' << formatStr(antennaSerialNo, 5);
         ss << ' ' << formatFixed(offsetA[0], 6, 4);
         ss << ' ' << formatFixed(offsetA[1], 6, 4);
         ss << ' ' << formatFixed(offsetA[2], 6, 4);
         ss << ' ' << formatFixed(offsetB[0], 6, 4);
         ss << ' ' << formatFixed(offsetB[1], 6, 4);
         ss << ' ' << formatFixed(offsetB[2], 6, 4);
         ss << ' ' << formatStr(antennaCalibration, 10);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SitePhaseCenter::operator std::string()


   void SitePhaseCenter::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 21, 27, 34, 41, 48, 55, 62, 69, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         antennaType = line.substr(1, 20);
         antennaSerialNo = line.substr(22, 5);
         offsetA[0] = asDouble(line.substr(28, 6) );
         offsetA[1] = asDouble(line.substr(35, 6) );
         offsetA[2] = asDouble(line.substr(42, 6) );
         offsetB[0] = asDouble(line.substr(49, 6) );
         offsetB[1] = asDouble(line.substr(56, 6) );
         offsetB[2] = asDouble(line.substr(63, 6) );
         antennaCalibration = line.substr(70, 10);
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SitePhaseCenter::operator=()


   void SitePhaseCenter::dump(ostream& s) const
   {
      s << " antennaType=" << antennaType << endl;
      s << " antennaSerialNo=" << antennaSerialNo << endl;
      s << " offsetA_Up="    << offsetA[0] << endl;
      s << " offsetA_North=" << offsetA[1] << endl;
      s << " offsetA_East="  << offsetA[2] << endl;
      s << " offsetB_Up="    << offsetB[0] << endl;
      s << " offsetB_North=" << offsetB[1] << endl;
      s << " offsetB_East="  << offsetB[2] << endl;
      s << " antennaCalibration=" << antennaCalibration << endl;

   }  // SitePhaseCenter::dump()


   const string  SiteGpsPhaseCenter::BLOCK_TITLE("SITE/GPS_PHASE_CENTER");


   void SiteGpsPhaseCenter::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      SitePhaseCenter::dump(s);

   }  // SiteGpsPhaseCenter::dump()


   const string  SiteGalPhaseCenter::BLOCK_TITLE("SITE/GAL_PHASE_CENTER");


   void SiteGalPhaseCenter::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      SitePhaseCenter::dump(s);

   }  // SiteGalPhaseCenter::dump()


   const string  SiteEccentricity::BLOCK_TITLE("SITE/ECCENTRICITY");
   const size_t  SiteEccentricity::MIN_LINE_LEN;
   const size_t  SiteEccentricity::MAX_LINE_LEN;


   SiteEccentricity::operator std::string() const
   {
      try
      {
         std::ostringstream ss;
         ss << DATA_START << formatStr(siteCode, 4);
         ss << ' ' << formatStr(pointCode, 2);
         ss << ' ' << formatStr(solutionId, 4);
         ss << ' ' << obsCode;
         ss << ' ' << setw(12) << right << (std::string)timeSince;
         ss << ' ' << setw(12) << right << (std::string)timeUntil;
         ss << ' ' << formatStr(refSystem, 3);
         ss << ' ' << formatFixed(eccentricity[0], 8, 4);
         ss << ' ' << formatFixed(eccentricity[1], 8, 4);
         ss << ' ' << formatFixed(eccentricity[2], 8, 4);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SiteEccentricity::operator std::string()


   void SiteEccentricity::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 5, 8, 13, 15, 28, 41, 45, 54, 63, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         siteCode   = line.substr(1, 4);
         pointCode  = line.substr(6, 2);
         solutionId = line.substr(9, 4);
         obsCode    = line[14];
         isValidObsCode(obsCode);
         timeSince = line.substr(16,12);
         timeUntil = line.substr(29,12);
         refSystem = line.substr(42, 3);
         eccentricity[0] = asDouble(line.substr(46, 8) );
         eccentricity[1] = asDouble(line.substr(55, 8) );
         eccentricity[2] = asDouble(line.substr(64, 8) );
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SiteEccentricity::operator=()


   void SiteEccentricity::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " siteCode=" << siteCode << endl;
      s << " pointCode=" << pointCode << endl;
      s << " solutionId=" << solutionId << endl;
      s << " obsCode=" << obsCode  << endl;
      s << " timeSince=" << (std::string)timeSince << endl;
      s << " timeUntil=" << (std::string)timeUntil << endl;
      s << " refSystem=" << refSystem << endl;
      s << " uX=" << eccentricity[0] << endl;
      s << " nY=" << eccentricity[1] << endl;
      s << " eZ=" << eccentricity[2] << endl;

   }  // SiteEccentricity::dump()


   const string  SatelliteId::BLOCK_TITLE("SATELLITE/ID");
   const size_t  SatelliteId::MIN_LINE_LEN;
   const size_t  SatelliteId::MAX_LINE_LEN;


   SatelliteId::operator std::string() const
   {
      try
      {
         std::ostringstream  ss;
         ss << DATA_START << formatStr(svCode, 4);
         ss << ' ' << formatStr(prn, 2);
         ss << ' ' << formatStr(cosparId, 9);
         ss << ' ' << obsCode;
         ss << ' ' << setw(12) << right << (std::string)timeSince;
         ss << ' ' << setw(12) << right << (std::string)timeUntil;
         ss << ' ' << formatStr(antennaType, 20);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SatelliteId::operator std::string()


   void SatelliteId::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 5, 8, 18, 20, 33, 46, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         svCode   = line.substr(1, 4);
         prn      = line.substr(6, 2);
         cosparId = line.substr(9, 9);
         obsCode  = line[19];
         isValidObsCode(obsCode);
         timeSince = line.substr(21,12);
         timeUntil = line.substr(34,12);
         antennaType = line.substr(47, 20);
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SatelliteId::operator=()


   void SatelliteId::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " svCode=" << svCode << endl;
      s << " prn=" << prn << endl;
      s << " cosparId=" << cosparId << endl;
      s << " obsCode=" << obsCode  << endl;
      s << " timeSince=" << (std::string)timeSince << endl;
      s << " timeUntil=" << (std::string)timeUntil << endl;
      s << " antennaType=" << antennaType << endl;

   }  // SatelliteId::dump()


   const string  SatellitePhaseCenter::BLOCK_TITLE("SATELLITE/PHASE_CENTER");
   const size_t  SatellitePhaseCenter::MIN_LINE_LEN;
   const size_t  SatellitePhaseCenter::MAX_LINE_LEN;


   SatellitePhaseCenter::operator std::string() const
   {
      try
      {
         std::ostringstream  ss;
         ss << DATA_START << formatStr(svCode, 4);
         ss << ' ' << freqCodeA;
         ss << ' ' << formatFixed(offsetA[2], 6, 4);
         ss << ' ' << formatFixed(offsetA[0], 6, 4);
         ss << ' ' << formatFixed(offsetA[1], 6, 4);
         ss << ' ' << freqCodeB;
         ss << ' ' << formatFixed(offsetB[2], 6, 4);
         ss << ' ' << formatFixed(offsetB[0], 6, 4);
         ss << ' ' << formatFixed(offsetB[1], 6, 4);
         ss << ' ' << formatStr(antennaCalibration, 10);
         ss << ' ' << pcvType;
         ss << ' ' << pcvModel;
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SatellitePhaseCenter::operator std::string()


   void SatellitePhaseCenter::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 5, 7, 14, 21, 28, 30, 37, 44, 51, 62, 64, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         svCode     = line.substr(1, 4);
         freqCodeA  = line[6];
         offsetA[2] = asDouble(line.substr(8, 6) );
         offsetA[0] = asDouble(line.substr(15, 6) );
         offsetA[1] = asDouble(line.substr(22, 6) );
         freqCodeB  = line[29];
         offsetB[2] = asDouble(line.substr(31, 6) );
         offsetB[0] = asDouble(line.substr(38, 6) );
         offsetB[1] = asDouble(line.substr(45, 6) );
         antennaCalibration = line.substr(52, 10);
         pcvType    = line[63];
         pcvModel   = line[65];
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SatellitePhaseCenter::operator=()


   void SatellitePhaseCenter::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " svCode=" << svCode << endl;
      s << " freqCodeA=" << freqCodeA << endl;
      s << " offsetA.x=" << formatFixed(offsetA[0], 6, 4) << endl;
      s << " offsetA.y=" << formatFixed(offsetA[1], 6, 4) << endl;
      s << " offsetA.z=" << formatFixed(offsetA[2], 6, 4) << endl;
      s << " freqCodeB=" << freqCodeB << endl;
      s << " offsetB.x=" << formatFixed(offsetB[0], 6, 4) << endl;
      s << " offsetB.y=" << formatFixed(offsetB[1], 6, 4) << endl;
      s << " offsetB.z=" << formatFixed(offsetB[2], 6, 4) << endl;
      s << " antennaCalibration=" << antennaCalibration << endl;
      s << " pcvType=" << pcvType  << endl;
      s << " pcvModel=" << pcvModel << endl;

   }  // SatellitePhaseCenter::dump()


   const string  BiasEpoch::BLOCK_TITLE("BIAS/EPOCHS");
   const size_t  BiasEpoch::MIN_LINE_LEN;
   const size_t  BiasEpoch::MAX_LINE_LEN;


   BiasEpoch::operator std::string() const
   {
      try
      {
         std::ostringstream  ss;
         ss << DATA_START << formatStr(siteCode, 4);
         ss << ' ' << formatStr(pointCode, 2);
         ss << ' ' << formatStr(solutionId, 4);
         ss << ' ' << biasType;
         ss << ' ' << setw(12) << right << (std::string)firstTime;
         ss << ' ' << setw(12) << right << (std::string)lastTime;
         ss << ' ' << setw(12) << right << (std::string)meanTime;
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // BiasEpoch::operator std::string()


   void BiasEpoch::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 5, 8, 13, 15, 28, 41, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         siteCode   = line.substr(1, 4);
         pointCode  = line.substr(6, 2);
         solutionId = line.substr(9, 4);
         biasType   = line[14];
         firstTime  = line.substr(16,12);
         lastTime   = line.substr(29,12);
         meanTime   = line.substr(42,12);
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // BiasEpoch::operator=()


   void BiasEpoch::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " siteCode=" << siteCode << endl;
      s << " pointCode=" << pointCode << endl;
      s << " solutionId=" << solutionId << endl;
      s << " biasType=" << biasType << endl;
      s << " firstTime=" << (std::string)firstTime << endl;
      s << " lastTime=" << (std::string)lastTime << endl;
      s << " meanTime=" << (std::string)meanTime << endl;

   }  // BiasEpoch::dump()


   const string  SolutionStatistics::BLOCK_TITLE("SOLUTION/STATISTICS");
   const size_t  SolutionStatistics::MIN_LINE_LEN;
   const size_t  SolutionStatistics::MAX_LINE_LEN;


   SolutionStatistics::operator std::string() const
   {
      try
      {
         ostringstream  ss;
         ss << DATA_START << formatStr(infoType, 30);
         ss << ' ' << formatFixed(infoValue, 22, 15);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SolutionStatistics::operator std::string()


   void
   SolutionStatistics::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 31, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         infoType  = line.substr(1, 30);
         infoValue = asLongDouble(line.substr(32, 22) );
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SolutionStatistics::operator=()


   void
   SolutionStatistics::dump(std::ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " infoType=" << infoType << endl;
      s << " infoValue=" << infoValue << endl;

   }  // SolutionStatistics::dump()


   const string  SolutionEpoch::BLOCK_TITLE("SOLUTION/EPOCHS");
   const size_t  SolutionEpoch::MIN_LINE_LEN;
   const size_t  SolutionEpoch::MAX_LINE_LEN;


   SolutionEpoch::operator std::string() const
   {
      try
      {
         std::ostringstream  ss;
         ss << DATA_START << formatStr(siteCode, 4);
         ss << ' ' << formatStr(pointCode, 2);
         ss << ' ' << formatStr(solutionId, 4);
         ss << ' ' << obsCode;
         ss << ' ' << setw(12) << right << (std::string)startTime;
         ss << ' ' << setw(12) << right << (std::string)endTime;
         ss << ' ' << setw(12) << right << (std::string)meanTime;
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SolutionEpoch::operator std::string()


   void SolutionEpoch::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 5, 8, 13, 15, 28, 41, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         siteCode   = line.substr(1, 4);
         pointCode  = line.substr(6, 2);
         solutionId = line.substr(9, 4);
         obsCode    = line[14];
         isValidObsCode(obsCode);
         startTime  = line.substr(16,12);
         endTime    = line.substr(29,12);
         meanTime   = line.substr(42,12);
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SolutionEpoch::operator=()


   void SolutionEpoch::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " siteCode=" << siteCode << endl;
      s << " pointCode=" << pointCode << endl;
      s << " solutionId=" << solutionId << endl;
      s << " obsCode=" << obsCode  << endl;
      s << " startTime=" << (std::string)startTime << endl;
      s << " endTime=" << (std::string)endTime << endl;
      s << " meanTime=" << (std::string)meanTime << endl;

   }  // SolutionEpoch::dump()


   const string  SolutionEstimate::BLOCK_TITLE("SOLUTION/ESTIMATE");
   const size_t  SolutionEstimate::MIN_LINE_LEN;
   const size_t  SolutionEstimate::MAX_LINE_LEN;


   SolutionEstimate::operator std::string() const
   {
      try
      {
         std::ostringstream  ss;
         ss << DATA_START << formatUint(paramIndex, 5);
         ss << ' ' << formatStr(paramType, 6);
         ss << ' ' << formatStr(siteCode, 4);
         ss << ' ' << formatStr(pointCode, 2);
         ss << ' ' << formatStr(solutionId, 4);
         ss << ' ' << setw(12) << right << (std::string)epoch;
         ss << ' ' << formatStr(paramUnits, 4);
         ss << ' ' << constraintCode;
         ss << ' ' << formatFor(paramEstimate, 21, 2);
            // Erase extra leading space for '-' since StdDev is non-negative
         ss << ' ' << formatFor(paramStdDev, 12, 2).erase(0, 1);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SolutionEstimate::operator std::string()


   void SolutionEstimate::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 6, 13, 18, 21, 26, 39, 44, 46, 68, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         paramIndex   = asUnsigned(line.substr(1, 5) );
         paramType    = line.substr(7, 6);
         siteCode     = line.substr(14, 4);
         pointCode    = line.substr(19, 2);
         solutionId   = line.substr(22, 4);
         epoch = line.substr(27,12);
         paramUnits     = line.substr(40, 4);
         constraintCode = line[45];
         paramEstimate  = asDouble(line.substr(47, 21) );
         paramStdDev    = asDouble(line.substr(69, 11) );
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SolutionEstimate::operator=()


   void SolutionEstimate::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " paramIndex=" << paramIndex << endl;
      s << " paramType=" << paramType << endl;
      s << " siteCode=" << siteCode << endl;
      s << " pointCode=" << pointCode << endl;
      s << " solutionId=" << solutionId << endl;
      s << " epoch=" << (std::string)epoch << endl;
      s << " constraintCode=" << constraintCode << endl;
      s << " paramEstimate=" << paramEstimate << endl;
      s << " paramStdDev=" << paramStdDev << endl;

   }  // SolutionEstimate::dump()


   const string  SolutionApriori::BLOCK_TITLE("SOLUTION/APRIORI");
   const size_t  SolutionApriori::MIN_LINE_LEN;
   const size_t  SolutionApriori::MAX_LINE_LEN;


   SolutionApriori::operator std::string() const
   {
      try
      {
         std::ostringstream  ss;
         ss << DATA_START << formatUint(paramIndex, 5);
         ss << ' ' << formatStr(paramType, 6);
         ss << ' ' << formatStr(siteCode, 4);
         ss << ' ' << formatStr(pointCode, 2);
         ss << ' ' << formatStr(solutionId, 4);
         ss << ' ' << setw(12) << right << (std::string)epoch;
         ss << ' ' << formatStr(paramUnits, 4);
         ss << ' ' << constraintCode;
         ss << ' ' << formatFor(paramApriori, 21, 2);
            // Erase extra leading space for '-' since StdDev is non-negative
         ss << ' ' << formatFor(paramStdDev, 12, 2).erase(0, 1);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SolutionApriori::operator std::string()


   void SolutionApriori::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 6, 13, 18, 21, 26, 39, 44, 46, 68, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         paramIndex = asUnsigned(line.substr(1, 5) );
         paramType  = line.substr(7, 6);
         siteCode   = line.substr(14, 4);
         pointCode  = line.substr(19, 2);
         solutionId = line.substr(22, 4);
         epoch      = line.substr(27,12);
         paramUnits     = line.substr(40, 4);
         constraintCode = line[45];
         paramApriori   = asDouble(line.substr(47, 21) );
         paramStdDev    = asDouble(line.substr(69, 11) );
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SolutionApriori::operator=()


   void SolutionApriori::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " paramIndex=" << paramIndex << endl;
      s << " paramType=" << paramType << endl;
      s << " siteCode=" << siteCode << endl;
      s << " pointCode=" << pointCode << endl;
      s << " solutionId=" << solutionId << endl;
      s << " epoch=" << (std::string)epoch << endl;
      s << " constraintCode=" << constraintCode << endl;
      s << " paramApriori=" << paramApriori << endl;
      s << " paramStdDev=" << paramStdDev << endl;

   }  // SolutionApriori::dump()

   // BLOCK_TITLE defined only for subclasses of SolutionMatrixEstimate
   const size_t  SolutionMatrixEstimate::MIN_LINE_LEN;
   const size_t  SolutionMatrixEstimate::MAX_LINE_LEN;


   SolutionMatrixEstimate::operator std::string() const
   {
      try
      {
         std::ostringstream  ss;
         ss << DATA_START << formatUint(row, 5);
         ss << ' ' << formatUint(col, 5);
         ss << ' ' << formatFor(val1, 21, 2);
         ss << ' ' << formatFor(val2, 21, 2);
         ss << ' ' << formatFor(val3, 21, 2);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SolutionMatrixEstimate::operator std::string()


   void SolutionMatrixEstimate::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 6, 12, 34, 56, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         row  = asUnsigned(line.substr(1, 5) );
         col  = asUnsigned(line.substr(7, 5) );
         val1 = asDouble(line.substr(13, 21) );
         val2 = asDouble(line.substr(35, 21) );
         val3 = asDouble(line.substr(57, 21) );
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SolutionMatrixEstimate::operator=()


   void SolutionMatrixEstimate::dump(ostream& s) const
   {
      s << " row=" << row << endl;
      s << " col=" << col << endl;
      s << " val1=" << val1 << endl;
      s << " val2=" << val2 << endl;
      s << " val3=" << val3 << endl;

   }  // SolutionMatrixEstimate::dump()


   const string  SolutionMatrixEstimateLCorr::BLOCK_TITLE("SOLUTION/MATRIX_ESTIMATE L CORR");

   void SolutionMatrixEstimateLCorr::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      SolutionMatrixEstimate::dump(s);

   }  // SolutionMatrixEstimateLCorr::dump()


   const string  SolutionMatrixEstimateUCorr::BLOCK_TITLE("SOLUTION/MATRIX_ESTIMATE U CORR");

   void SolutionMatrixEstimateUCorr::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      SolutionMatrixEstimate::dump(s);

   }  // SolutionMatrixEstimateUCorr::dump()


   const string  SolutionMatrixEstimateLCova::BLOCK_TITLE("SOLUTION/MATRIX_ESTIMATE L COVA");

   void SolutionMatrixEstimateLCova::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      SolutionMatrixEstimate::dump(s);

   }  // SolutionMatrixEstimateLCova::dump()


   const string  SolutionMatrixEstimateUCova::BLOCK_TITLE("SOLUTION/MATRIX_ESTIMATE U COVA");

   void SolutionMatrixEstimateUCova::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      SolutionMatrixEstimate::dump(s);

   }  // SolutionMatrixEstimateUCova::dump()


   const string  SolutionMatrixEstimateLInfo::BLOCK_TITLE("SOLUTION/MATRIX_ESTIMATE L INFO");

   void SolutionMatrixEstimateLInfo::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      SolutionMatrixEstimate::dump(s);

   }  // SolutionMatrixEstimateLInfo::dump()


   const string  SolutionMatrixEstimateUInfo::BLOCK_TITLE("SOLUTION/MATRIX_ESTIMATE U INFO");

   void SolutionMatrixEstimateUInfo::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      SolutionMatrixEstimate::dump(s);

   }  // SolutionMatrixEstimateUInfo::dump()


   // BLOCK_TITLE defined only for subclasses of SolutionMatrixApriori
   const size_t       SolutionMatrixApriori::MIN_LINE_LEN;
   const size_t       SolutionMatrixApriori::MAX_LINE_LEN;

   SolutionMatrixApriori::operator std::string() const
   {
      try
      {
         std::ostringstream  ss;
         ss << DATA_START << formatUint(row, 5);
         ss << ' ' << formatUint(col, 5);
         ss << ' ' << formatFor(val1, 21, 2);
         ss << ' ' << formatFor(val2, 21, 2);
         ss << ' ' << formatFor(val3, 21, 2);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SolutionMatrixApriori::operator std::string()


   void SolutionMatrixApriori::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 6, 12, 34, 56, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         row  = asUnsigned(line.substr(1, 5) );
         col  = asUnsigned(line.substr(7, 5) );
         val1 = asDouble(line.substr(13, 21) );
         val2 = asDouble(line.substr(35, 21) );
         val3 = asDouble(line.substr(57, 21) );
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SolutionMatrixApriori::operator=()


   void SolutionMatrixApriori::dump(ostream& s) const
   {
      s << " row=" << row << endl;
      s << " col=" << col << endl;
      s << " val1=" << val1 << endl;
      s << " val2=" << val2 << endl;
      s << " val3=" << val3 << endl;

   }  // SolutionMatrixApriori::dump()


   const std::string  SolutionMatrixAprioriLCorr::BLOCK_TITLE("SOLUTION/MATRIX_APRIORI L CORR");

   void SolutionMatrixAprioriLCorr::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      SolutionMatrixApriori::dump(s);

   }  // SolutionMatrixAprioriLCorr::dump()


   const std::string  SolutionMatrixAprioriUCorr::BLOCK_TITLE("SOLUTION/MATRIX_APRIORI U CORR");

   void SolutionMatrixAprioriUCorr::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      SolutionMatrixApriori::dump(s);

   }  // SolutionMatrixAprioriUCorr::dump()


   const std::string  SolutionMatrixAprioriLCova::BLOCK_TITLE("SOLUTION/MATRIX_APRIORI L COVA");

   void SolutionMatrixAprioriLCova::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      SolutionMatrixApriori::dump(s);

   }  // SolutionMatrixAprioriLCova::dump()


   const std::string  SolutionMatrixAprioriUCova::BLOCK_TITLE("SOLUTION/MATRIX_APRIORI U COVA");

   void SolutionMatrixAprioriUCova::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      SolutionMatrixApriori::dump(s);

   }  // SolutionMatrixAprioriUCova::dump()


   const std::string  SolutionMatrixAprioriLInfo::BLOCK_TITLE("SOLUTION/MATRIX_APRIORI L INFO");

   void SolutionMatrixAprioriLInfo::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      SolutionMatrixApriori::dump(s);

   }  // SolutionMatrixAprioriLInfo::dump()


   const std::string  SolutionMatrixAprioriUInfo::BLOCK_TITLE("SOLUTION/MATRIX_APRIORI U INFO");

   void SolutionMatrixAprioriUInfo::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      SolutionMatrixApriori::dump(s);

   }  // SolutionMatrixAprioriUInfo::dump()


   const std::string  SolutionNormalEquationVector::BLOCK_TITLE("SOLUTION/NORMAL_EQUATION_VECTOR");
   const size_t       SolutionNormalEquationVector::MIN_LINE_LEN;
   const size_t       SolutionNormalEquationVector::MAX_LINE_LEN;


   SolutionNormalEquationVector::operator std::string() const
   {
      try
      {
         std::ostringstream  ss;
         ss << DATA_START << formatUint(paramIndex, 5);
         ss << ' ' << formatStr(paramType, 6);
         ss << ' ' << formatStr(siteCode, 4);
         ss << ' ' << formatStr(pointCode, 2);
         ss << ' ' << formatStr(solutionId, 4);
         ss << ' ' << setw(12) << right << (std::string)epoch;
         ss << ' ' << formatStr(paramUnits, 4);
         ss << ' ' << constraintCode;
         ss << ' ' << formatFor(value, 21, 2);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SolutionNormalEquationVector::operator std::string()


   void SolutionNormalEquationVector::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 6, 13, 18, 21, 26, 39, 44, 46, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         paramIndex = asUnsigned(line.substr(1, 5) );
         paramType  = line.substr(7, 6);
         siteCode   = line.substr(14, 4);
         pointCode  = line.substr(19, 2);
         solutionId = line.substr(22, 4);
         epoch      = line.substr(27,12);
         paramUnits     = line.substr(40, 4);
         constraintCode = line[45];
         value          = asDouble(line.substr(47, 21) );
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SolutionNormalEquationVector::operator=()


   void SolutionNormalEquationVector::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      s << " paramIndex=" << paramIndex << endl;
      s << " paramType=" << paramType << endl;
      s << " siteCode=" << siteCode << endl;
      s << " pointCode=" << pointCode << endl;
      s << " solutionId=" << solutionId << endl;
      s << " epoch=" << (std::string)epoch << endl;
      s << " constraintCode=" << constraintCode << endl;
      s << " value=" << value << endl;

   }  // SolutionNormalEquationVector::dump()


   // BLOCK_TITLE defined only for subclasses of SolutionNormalEquationMatrix
   const size_t       SolutionNormalEquationMatrix::MIN_LINE_LEN;
   const size_t       SolutionNormalEquationMatrix::MAX_LINE_LEN;


   SolutionNormalEquationMatrix::operator std::string() const
   {
      try
      {
         std::ostringstream  ss;
         ss << DATA_START << formatUint(row, 5);
         ss << ' ' << formatUint(col, 5);
         ss << ' ' << formatFor(val1, 21, 3);
         ss << ' ' << formatFor(val2, 21, 3);
         ss << ' ' << formatFor(val3, 21, 3);
         return ss.str();
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SolutionNormalEquationMatrix::operator std::string()


   void SolutionNormalEquationMatrix::operator=(const std::string& line)
   {
      static int FIELD_DIVS[] = {0, 6, 12, 34, 56, -1};
      try
      {
         isValidLineStructure(line, MIN_LINE_LEN, MAX_LINE_LEN, FIELD_DIVS);
         row  = asUnsigned(line.substr(1, 5) );
         col  = asUnsigned(line.substr(7, 5) );
         val1 = asDouble(line.substr(13, 21) );
         val2 = asDouble(line.substr(35, 21) );
         val3 = asDouble(line.substr(57, 21) );
      }
      catch (Exception& exc)
      {
         GPSTK_RETHROW(exc);
      }
   }  // SolutionNormalEquationMatrix::operator=()


   void SolutionNormalEquationMatrix::dump(ostream& s) const
   {
      s << " row=" << row << endl;
      s << " col=" << col << endl;
      s << " val1=" << val1 << endl;
      s << " val2=" << val2 << endl;
      s << " val3=" << val3 << endl;

   }  // SolutionNormalEquationMatrix::dump()


   const std::string  SolutionNormalEquationMatrixL::BLOCK_TITLE("SOLUTION/NORMAL_EQUATION_MATRIX L");

   void SolutionNormalEquationMatrixL::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      SolutionNormalEquationMatrix::dump(s);

   }  // SolutionNormalEquationMatrixL::dump()


   const std::string  SolutionNormalEquationMatrixU::BLOCK_TITLE("SOLUTION/NORMAL_EQUATION_MATRIX U");

   void SolutionNormalEquationMatrixU::dump(ostream& s) const
   {
      s << BLOCK_TITLE << " :" << endl;
      SolutionNormalEquationMatrix::dump(s);

   }  // SolutionNormalEquationMatrixU::dump()

}  // namespace Sinex

}  // namespace gpstk
