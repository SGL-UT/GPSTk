#include "TrackingCode.hpp"

namespace gpstk
{
   namespace StringUtils
   {
      std::string asString(TrackingCode e) throw()
      {
         switch (e)
         {
            case TrackingCode::CA:        return "CA";
            case TrackingCode::N:         return "N";
            case TrackingCode::I5:        return "I5";
            case TrackingCode::G1D:       return "G1D";
            case TrackingCode::G1X:       return "G1X";
            case TrackingCode::G1P:       return "G1P";
            case TrackingCode::C2LM:      return "C2LM";
            case TrackingCode::C2L:       return "C2L";
            case TrackingCode::C2M:       return "C2M";
            case TrackingCode::IQ5:       return "IQ5";
            case TrackingCode::M:         return "M";
            case TrackingCode::P:         return "P";
            case TrackingCode::Q5:        return "Q5";
            case TrackingCode::D:         return "D";
            case TrackingCode::Y:         return "Y";
            case TrackingCode::W:         return "W";
            case TrackingCode::L1OCD:     return "L1OCD";
            case TrackingCode::L1OCP:     return "L1OCP";
            case TrackingCode::L1OC:      return "L1OC";
            case TrackingCode::L2CSIOCp:  return "L2CSIOCp";
            case TrackingCode::L2CSI:     return "L2CSI";
            case TrackingCode::L2OCP:     return "L2OCP";
            case TrackingCode::IR3:       return "IR3";
            case TrackingCode::IQR3:      return "IQR3";
            case TrackingCode::QR3:       return "QR3";
            case TrackingCode::GP:        return "GP";
            case TrackingCode::GCA:       return "GCA";
            case TrackingCode::A:         return "A";
            case TrackingCode::ABC:       return "ABC";
            case TrackingCode::B:         return "B";
            case TrackingCode::BC:        return "BC";
            case TrackingCode::C:         return "C";
            case TrackingCode::IE5:       return "IE5";
            case TrackingCode::IQE5:      return "IQE5";
            case TrackingCode::QE5:       return "QE5";
            case TrackingCode::IE5a:      return "IE5a";
            case TrackingCode::IQE5a:     return "IQE5a";
            case TrackingCode::QE5a:      return "QE5a";
            case TrackingCode::IE5b:      return "IE5b";
            case TrackingCode::IQE5b:     return "IQE5b";
            case TrackingCode::QE5b:      return "QE5b";
            case TrackingCode::A6:        return "A6";
            case TrackingCode::ABC6:      return "ABC6";
            case TrackingCode::B6:        return "B6";
            case TrackingCode::BC6:       return "BC6";
            case TrackingCode::C6:        return "C6";
            case TrackingCode::SCA:       return "SCA";
            case TrackingCode::SI5:       return "SI5";
            case TrackingCode::SIQ5:      return "SIQ5";
            case TrackingCode::SQ5:       return "SQ5";
            case TrackingCode::JCA:       return "JCA";
            case TrackingCode::JD1:       return "JD1";
            case TrackingCode::JX1:       return "JX1";
            case TrackingCode::JP1:       return "JP1";
            case TrackingCode::JZ1:       return "JZ1";
            case TrackingCode::JL2:       return "JL2";
            case TrackingCode::JM2:       return "JM2";
            case TrackingCode::JX2:       return "JX2";
            case TrackingCode::JI5:       return "JI5";
            case TrackingCode::JIQ5:      return "JIQ5";
            case TrackingCode::JQ5:       return "JQ5";
            case TrackingCode::JI5S:      return "JI5S";
            case TrackingCode::JIQ5S:     return "JIQ5S";
            case TrackingCode::JQ5S:      return "JQ5S";
            case TrackingCode::JI6:       return "JI6";
            case TrackingCode::JIQ6:      return "JIQ6";
            case TrackingCode::JQ6:       return "JQ6";
            case TrackingCode::JDE6:      return "JDE6";
            case TrackingCode::JD6:       return "JD6";
            case TrackingCode::JE6:       return "JE6";
            case TrackingCode::CIQ1:      return "CIQ1";
            case TrackingCode::CA1:       return "CA1";
            case TrackingCode::CCD1:      return "CCD1";
            case TrackingCode::CCDP1:     return "CCDP1";
            case TrackingCode::CCP1:      return "CCP1";
            case TrackingCode::CI1:       return "CI1";
            case TrackingCode::CQ1:       return "CQ1";
            case TrackingCode::CI2ab:     return "CI2ab";
            case TrackingCode::CIQ2ab:    return "CIQ2ab";
            case TrackingCode::CQ2ab:     return "CQ2ab";
            case TrackingCode::CIQ7:      return "CIQ7";
            case TrackingCode::CI2a:      return "CI2a";
            case TrackingCode::CIQ2a:     return "CIQ2a";
            case TrackingCode::CQ2a:      return "CQ2a";
            case TrackingCode::CI2b:      return "CI2b";
            case TrackingCode::CIQ2b:     return "CIQ2b";
            case TrackingCode::CQ2b:      return "CQ2b";
            case TrackingCode::CI7:       return "CI7";
            case TrackingCode::CQ7:       return "CQ7";
            case TrackingCode::CIQ6:      return "CIQ6";
            case TrackingCode::CI6:       return "CI6";
            case TrackingCode::CQ6:       return "CQ6";
            case TrackingCode::CodelessC: return "CodelessC";
            case TrackingCode::CIQ3A:     return "CIQ3A";
            case TrackingCode::IB5:       return "IB5";
            case TrackingCode::IX5:       return "IX5";
            case TrackingCode::IC5:       return "IC5";
            case TrackingCode::IB9:       return "IB9";
            case TrackingCode::IX9:       return "IX9";
            case TrackingCode::IC9:       return "IC9";
            case TrackingCode::IA5:       return "IA5";
            case TrackingCode::IA9:       return "IA9";
            case TrackingCode::Unknown:   return "Unknown";
            case TrackingCode::Any:       return "Any";
            case TrackingCode::Undefined: return "Undefined";
            case TrackingCode::Last:      return "Last";
            default:                      return "???";
         }
      }


      TrackingCode asTrackingCode(const std::string& s) throw()
      {
         if (s == "CA")
            return TrackingCode::CA;
         if (s == "N")
            return TrackingCode::N;
         if (s == "I5")
            return TrackingCode::I5;
         if (s == "G1D")
            return TrackingCode::G1D;
         if (s == "G1X")
            return TrackingCode::G1X;
         if (s == "G1P")
            return TrackingCode::G1P;
         if (s == "C2LM")
            return TrackingCode::C2LM;
         if (s == "C2L")
            return TrackingCode::C2L;
         if (s == "C2M")
            return TrackingCode::C2M;
         if (s == "IQ5")
            return TrackingCode::IQ5;
         if (s == "M")
            return TrackingCode::M;
         if (s == "P")
            return TrackingCode::P;
         if (s == "Q5")
            return TrackingCode::Q5;
         if (s == "D")
            return TrackingCode::D;
         if (s == "Y")
            return TrackingCode::Y;
         if (s == "W")
            return TrackingCode::W;
         if (s == "L1OCD")
            return TrackingCode::L1OCD;
         if (s == "L1OCP")
            return TrackingCode::L1OCP;
         if (s == "L1OC")
            return TrackingCode::L1OC;
         if (s == "L2CSIOCp")
            return TrackingCode::L2CSIOCp;
         if (s == "L2CSI")
            return TrackingCode::L2CSI;
         if (s == "L2OCP")
            return TrackingCode::L2OCP;
         if (s == "IR3")
            return TrackingCode::IR3;
         if (s == "IQR3")
            return TrackingCode::IQR3;
         if (s == "QR3")
            return TrackingCode::QR3;
         if (s == "GP")
            return TrackingCode::GP;
         if (s == "GCA")
            return TrackingCode::GCA;
         if (s == "A")
            return TrackingCode::A;
         if (s == "ABC")
            return TrackingCode::ABC;
         if (s == "B")
            return TrackingCode::B;
         if (s == "BC")
            return TrackingCode::BC;
         if (s == "C")
            return TrackingCode::C;
         if (s == "IE5")
            return TrackingCode::IE5;
         if (s == "IQE5")
            return TrackingCode::IQE5;
         if (s == "QE5")
            return TrackingCode::QE5;
         if (s == "IE5a")
            return TrackingCode::IE5a;
         if (s == "IQE5a")
            return TrackingCode::IQE5a;
         if (s == "QE5a")
            return TrackingCode::QE5a;
         if (s == "IE5b")
            return TrackingCode::IE5b;
         if (s == "IQE5b")
            return TrackingCode::IQE5b;
         if (s == "QE5b")
            return TrackingCode::QE5b;
         if (s == "A6")
            return TrackingCode::A6;
         if (s == "ABC6")
            return TrackingCode::ABC6;
         if (s == "B6")
            return TrackingCode::B6;
         if (s == "BC6")
            return TrackingCode::BC6;
         if (s == "C6")
            return TrackingCode::C6;
         if (s == "SCA")
            return TrackingCode::SCA;
         if (s == "SI5")
            return TrackingCode::SI5;
         if (s == "SIQ5")
            return TrackingCode::SIQ5;
         if (s == "SQ5")
            return TrackingCode::SQ5;
         if (s == "JCA")
            return TrackingCode::JCA;
         if (s == "JD1")
            return TrackingCode::JD1;
         if (s == "JX1")
            return TrackingCode::JX1;
         if (s == "JP1")
            return TrackingCode::JP1;
         if (s == "JZ1")
            return TrackingCode::JZ1;
         if (s == "JL2")
            return TrackingCode::JL2;
         if (s == "JM2")
            return TrackingCode::JM2;
         if (s == "JX2")
            return TrackingCode::JX2;
         if (s == "JI5")
            return TrackingCode::JI5;
         if (s == "JIQ5")
            return TrackingCode::JIQ5;
         if (s == "JQ5")
            return TrackingCode::JQ5;
         if (s == "JI5S")
            return TrackingCode::JI5S;
         if (s == "JIQ5S")
            return TrackingCode::JIQ5S;
         if (s == "JQ5S")
            return TrackingCode::JQ5S;
         if (s == "JI6")
            return TrackingCode::JI6;
         if (s == "JIQ6")
            return TrackingCode::JIQ6;
         if (s == "JQ6")
            return TrackingCode::JQ6;
         if (s == "JDE6")
            return TrackingCode::JDE6;
         if (s == "JD6")
            return TrackingCode::JD6;
         if (s == "JE6")
            return TrackingCode::JE6;
         if (s == "CIQ1")
            return TrackingCode::CIQ1;
         if (s == "CA1")
            return TrackingCode::CA1;
         if (s == "CCD1")
            return TrackingCode::CCD1;
         if (s == "CCDP1")
            return TrackingCode::CCDP1;
         if (s == "CCP1")
            return TrackingCode::CCP1;
         if (s == "CI1")
            return TrackingCode::CI1;
         if (s == "CQ1")
            return TrackingCode::CQ1;
         if (s == "CI2ab")
            return TrackingCode::CI2ab;
         if (s == "CIQ2ab")
            return TrackingCode::CIQ2ab;
         if (s == "CQ2ab")
            return TrackingCode::CQ2ab;
         if (s == "CIQ7")
            return TrackingCode::CIQ7;
         if (s == "CI2a")
            return TrackingCode::CI2a;
         if (s == "CIQ2a")
            return TrackingCode::CIQ2a;
         if (s == "CQ2a")
            return TrackingCode::CQ2a;
         if (s == "CI2b")
            return TrackingCode::CI2b;
         if (s == "CIQ2b")
            return TrackingCode::CIQ2b;
         if (s == "CQ2b")
            return TrackingCode::CQ2b;
         if (s == "CI7")
            return TrackingCode::CI7;
         if (s == "CQ7")
            return TrackingCode::CQ7;
         if (s == "CIQ6")
            return TrackingCode::CIQ6;
         if (s == "CI6")
            return TrackingCode::CI6;
         if (s == "CQ6")
            return TrackingCode::CQ6;
         if (s == "CodelessC")
            return TrackingCode::CodelessC;
         if (s == "CIQ3A")
            return TrackingCode::CIQ3A;
         if (s == "IB5")
            return TrackingCode::IB5;
         if (s == "IX5")
            return TrackingCode::IX5;
         if (s == "IC5")
            return TrackingCode::IC5;
         if (s == "IB9")
            return TrackingCode::IB9;
         if (s == "IX9")
            return TrackingCode::IX9;
         if (s == "IC9")
            return TrackingCode::IC9;
         if (s == "IA5")
            return TrackingCode::IA5;
         if (s == "IA9")
            return TrackingCode::IA9;
         if (s == "Unknown")
            return TrackingCode::Unknown;
         if (s == "Any")
            return TrackingCode::Any;
         if (s == "Undefined")
            return TrackingCode::Undefined;
         if (s == "Last")
            return TrackingCode::Last;
         return TrackingCode::Unknown;
      }
   } // namespace StringUtils
} // namespace gpstk
