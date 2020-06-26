#include "ObservationType.hpp"

namespace gpstk
{
   namespace StringUtils
   {
      std::string asString(ObservationType e) throw()
      {
         switch (e)
         {
            case ObservationType::Unknown:   return "Unknown";
            case ObservationType::Any:       return "Any";
            case ObservationType::Range:     return "Range";
            case ObservationType::Phase:     return "Phase";
            case ObservationType::Doppler:   return "Doppler";
            case ObservationType::SNR:       return "SNR";
            case ObservationType::Channel:   return "Channel";
            case ObservationType::DemodStat: return "DemodStat";
            case ObservationType::Iono:      return "Iono";
            case ObservationType::SSI:       return "SSI";
            case ObservationType::LLI:       return "LLI";
            case ObservationType::TrackLen:  return "TrackLen";
            case ObservationType::NavMsg:    return "NavMsg";
            case ObservationType::RngStdDev: return "RngStdDev";
            case ObservationType::PhsStdDev: return "PhsStdDev";
            case ObservationType::FreqIndx:  return "FreqIndx";
            case ObservationType::Undefined: return "Undefined";
            default:                         return "???";
         }
      }


      gpstk::ObservationType asObservationType(const std::string& s) throw()
      {
         if (s == "Unknown")
            return ObservationType::Unknown;
         if (s == "Any")
            return ObservationType::Any;
         if (s == "Range")
            return ObservationType::Range;
         if (s == "Phase")
            return ObservationType::Phase;
         if (s == "Doppler")
            return ObservationType::Doppler;
         if (s == "SNR")
            return ObservationType::SNR;
         if (s == "Channel")
            return ObservationType::Channel;
         if (s == "DemodStat")
            return ObservationType::DemodStat;
         if (s == "Iono")
            return ObservationType::Iono;
         if (s == "SSI")
            return ObservationType::SSI;
         if (s == "LLI")
            return ObservationType::LLI;
         if (s == "TrackLen")
            return ObservationType::TrackLen;
         if (s == "NavMsg")
            return ObservationType::NavMsg;
         if (s == "RngStdDev")
            return ObservationType::RngStdDev;
         if (s == "PhsStdDev")
            return ObservationType::PhsStdDev;
         if (s == "FreqIndx")
            return ObservationType::FreqIndx;
         if (s == "Undefined")
            return ObservationType::Undefined;
         return ObservationType::Unknown;
      }
   } // namespace StringUtils
} // namespace gpstk
