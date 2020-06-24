#include "CarrierBand.hpp"

namespace gpstk
{
   namespace StringUtils
   {
      std::string asString(CarrierBand e) throw()
      {
         switch (e)
         {
            case CarrierBand::Zero:      return "Zero";
            case CarrierBand::L1L2:      return "L1L2";
            case CarrierBand::L1:        return "L1";
            case CarrierBand::L2:        return "L2";
            case CarrierBand::L5:        return "L5";
            case CarrierBand::G1:        return "G1";
            case CarrierBand::G1a:       return "G1a";
            case CarrierBand::G2a:       return "G2a";
            case CarrierBand::G2:        return "G2";
            case CarrierBand::G3:        return "G3";
            case CarrierBand::E6:        return "E6";
            case CarrierBand::E5b:       return "E5b";
            case CarrierBand::E5ab:      return "E5ab";
            case CarrierBand::B1:        return "B1";
            case CarrierBand::B3:        return "B3";
            case CarrierBand::B2:        return "B2";
            case CarrierBand::I9:        return "I9";
            case CarrierBand::Unknown:   return "Unknown";
            case CarrierBand::Any:       return "Any";
            case CarrierBand::Undefined: return "Undefined";
            case CarrierBand::Last:      return "Last";
            default:                     return "???";
         }
      }


      CarrierBand asCarrierBand(const std::string& s) throw()
      {
         if (s == "Zero")
            return CarrierBand::Zero;
         if (s == "L1L2")
            return CarrierBand::L1L2;
         if (s == "L1")
            return CarrierBand::L1;
         if (s == "L2")
            return CarrierBand::L2;
         if (s == "L5")
            return CarrierBand::L5;
         if (s == "G1")
            return CarrierBand::G1;
         if (s == "G1a")
            return CarrierBand::G1a;
         if (s == "G2a")
            return CarrierBand::G2a;
         if (s == "G2")
            return CarrierBand::G2;
         if (s == "G3")
            return CarrierBand::G3;
         if (s == "E6")
            return CarrierBand::E6;
         if (s == "E5b")
            return CarrierBand::E5b;
         if (s == "E5ab")
            return CarrierBand::E5ab;
         if (s == "B1")
            return CarrierBand::B1;
         if (s == "B3")
            return CarrierBand::B3;
         if (s == "B2")
            return CarrierBand::B2;
         if (s == "I9")
            return CarrierBand::I9;
         if (s == "Unknown")
            return CarrierBand::Unknown;
         if (s == "Any")
            return CarrierBand::Any;
         if (s == "Undefined")
            return CarrierBand::Undefined;
         if (s == "Last")
            return CarrierBand::Last;
         return CarrierBand::Unknown;
      }
   } // namespace StringUtils
} // namespace gpstk
