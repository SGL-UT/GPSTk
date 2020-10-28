/** @warning This code is automatically generated.
 *
 *  DO NOT EDIT THIS CODE BY HAND.
 *
 *  Refer to the documenation in the toolkit_docs gitlab project.
 */

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

#include "TrackingCode.hpp"

namespace gpstk
{
   namespace StringUtils
   {
      std::string asString(TrackingCode e) throw()
      {
         switch (e)
         {
            case TrackingCode::Unknown:      return "Unknown";
            case TrackingCode::Any:          return "Any";
            case TrackingCode::CA:           return "CA";
            case TrackingCode::P:            return "P";
            case TrackingCode::Y:            return "Y";
            case TrackingCode::Ztracking:    return "Ztracking";
            case TrackingCode::YCodeless:    return "YCodeless";
            case TrackingCode::Semicodeless: return "Semicodeless";
            case TrackingCode::MD:           return "MD";
            case TrackingCode::MDP:          return "MDP";
            case TrackingCode::MP:           return "MP";
            case TrackingCode::MPA:          return "MPA";
            case TrackingCode::MARL:         return "MARL";
            case TrackingCode::MARLD:        return "MARLD";
            case TrackingCode::MARLP:        return "MARLP";
            case TrackingCode::Mprime:       return "Mprime";
            case TrackingCode::MprimePA:     return "MprimePA";
            case TrackingCode::L2CM:         return "L2CM";
            case TrackingCode::L2CL:         return "L2CL";
            case TrackingCode::L2CML:        return "L2CML";
            case TrackingCode::L5I:          return "L5I";
            case TrackingCode::L5Q:          return "L5Q";
            case TrackingCode::L5IQ:         return "L5IQ";
            case TrackingCode::L1CP:         return "L1CP";
            case TrackingCode::L1CD:         return "L1CD";
            case TrackingCode::L1CDP:        return "L1CDP";
            case TrackingCode::NSCA:         return "NSCA";
            case TrackingCode::NSCD:         return "NSCD";
            case TrackingCode::NSCL:         return "NSCL";
            case TrackingCode::NSCM:         return "NSCM";
            case TrackingCode::NSCMCL:       return "NSCMCL";
            case TrackingCode::NSCP:         return "NSCP";
            case TrackingCode::NSCPCD:       return "NSCPCD";
            case TrackingCode::NSI5:         return "NSI5";
            case TrackingCode::NSM:          return "NSM";
            case TrackingCode::NSMPA:        return "NSMPA";
            case TrackingCode::NSMprime:     return "NSMprime";
            case TrackingCode::NSMprimePA:   return "NSMprimePA";
            case TrackingCode::NSP:          return "NSP";
            case TrackingCode::NSQ5:         return "NSQ5";
            case TrackingCode::NSY:          return "NSY";
            case TrackingCode::Standard:     return "Standard";
            case TrackingCode::Precise:      return "Precise";
            case TrackingCode::L3OCD:        return "L3OCD";
            case TrackingCode::L3OCP:        return "L3OCP";
            case TrackingCode::L3OCDP:       return "L3OCDP";
            case TrackingCode::L1OCD:        return "L1OCD";
            case TrackingCode::L1OCP:        return "L1OCP";
            case TrackingCode::L1OCDP:       return "L1OCDP";
            case TrackingCode::L2CSIL2OCp:   return "L2CSIL2OCp";
            case TrackingCode::L2CSI:        return "L2CSI";
            case TrackingCode::L2OCP:        return "L2OCP";
            case TrackingCode::G3TestData:   return "G3TestData";
            case TrackingCode::G3TestPilot:  return "G3TestPilot";
            case TrackingCode::L1SC:         return "L1SC";
            case TrackingCode::L2SC:         return "L2SC";
            case TrackingCode::E1A:          return "E1A";
            case TrackingCode::E1B:          return "E1B";
            case TrackingCode::E1C:          return "E1C";
            case TrackingCode::E1BC:         return "E1BC";
            case TrackingCode::E1ABC:        return "E1ABC";
            case TrackingCode::E5abI:        return "E5abI";
            case TrackingCode::E5abQ:        return "E5abQ";
            case TrackingCode::E5abIQ:       return "E5abIQ";
            case TrackingCode::E5aI:         return "E5aI";
            case TrackingCode::E5aQ:         return "E5aQ";
            case TrackingCode::E5aIQ:        return "E5aIQ";
            case TrackingCode::E5bI:         return "E5bI";
            case TrackingCode::E5bQ:         return "E5bQ";
            case TrackingCode::E5bIQ:        return "E5bIQ";
            case TrackingCode::E6A:          return "E6A";
            case TrackingCode::E6B:          return "E6B";
            case TrackingCode::E6C:          return "E6C";
            case TrackingCode::E6BC:         return "E6BC";
            case TrackingCode::E6ABC:        return "E6ABC";
            case TrackingCode::L1S:          return "L1S";
            case TrackingCode::L5SI:         return "L5SI";
            case TrackingCode::L5SQ:         return "L5SQ";
            case TrackingCode::L5SIQ:        return "L5SIQ";
            case TrackingCode::LEXS:         return "LEXS";
            case TrackingCode::LEXL:         return "LEXL";
            case TrackingCode::LEXSL:        return "LEXSL";
            case TrackingCode::L6D:          return "L6D";
            case TrackingCode::L6E:          return "L6E";
            case TrackingCode::L6DE:         return "L6DE";
            case TrackingCode::B1I:          return "B1I";
            case TrackingCode::B1Q:          return "B1Q";
            case TrackingCode::B1IQ:         return "B1IQ";
            case TrackingCode::B2I:          return "B2I";
            case TrackingCode::B2Q:          return "B2Q";
            case TrackingCode::B2IQ:         return "B2IQ";
            case TrackingCode::B3I:          return "B3I";
            case TrackingCode::B3Q:          return "B3Q";
            case TrackingCode::B3IQ:         return "B3IQ";
            case TrackingCode::B1A:          return "B1A";
            case TrackingCode::B1CD:         return "B1CD";
            case TrackingCode::B1CDP:        return "B1CDP";
            case TrackingCode::B1CP:         return "B1CP";
            case TrackingCode::B2abI:        return "B2abI";
            case TrackingCode::B2abIQ:       return "B2abIQ";
            case TrackingCode::B2abQ:        return "B2abQ";
            case TrackingCode::B2aI:         return "B2aI";
            case TrackingCode::B2aIQ:        return "B2aIQ";
            case TrackingCode::B2aQ:         return "B2aQ";
            case TrackingCode::B2bI:         return "B2bI";
            case TrackingCode::B2bIQ:        return "B2bIQ";
            case TrackingCode::B2bQ:         return "B2bQ";
            case TrackingCode::B3AQ:         return "B3AQ";
            case TrackingCode::BCodeless:    return "BCodeless";
            case TrackingCode::B3AI:         return "B3AI";
            case TrackingCode::B3AIQ:        return "B3AIQ";
            case TrackingCode::SPSL5:        return "SPSL5";
            case TrackingCode::RSL5D:        return "RSL5D";
            case TrackingCode::RSL5P:        return "RSL5P";
            case TrackingCode::RSL5DP:       return "RSL5DP";
            case TrackingCode::SPSS:         return "SPSS";
            case TrackingCode::RSSD:         return "RSSD";
            case TrackingCode::RSSP:         return "RSSP";
            case TrackingCode::RSSDP:        return "RSSDP";
            case TrackingCode::CW:           return "CW";
            case TrackingCode::Undefined:    return "Undefined";
            default:                         return "???";
         } // switch (e)
      } // asString(TrackingCode)


      TrackingCode asTrackingCode(const std::string& s) throw()
      {
         if (s == "Unknown")
            return TrackingCode::Unknown;
         if (s == "Any")
            return TrackingCode::Any;
         if (s == "CA")
            return TrackingCode::CA;
         if (s == "P")
            return TrackingCode::P;
         if (s == "Y")
            return TrackingCode::Y;
         if (s == "Ztracking")
            return TrackingCode::Ztracking;
         if (s == "YCodeless")
            return TrackingCode::YCodeless;
         if (s == "Semicodeless")
            return TrackingCode::Semicodeless;
         if (s == "MD")
            return TrackingCode::MD;
         if (s == "MDP")
            return TrackingCode::MDP;
         if (s == "MP")
            return TrackingCode::MP;
         if (s == "MPA")
            return TrackingCode::MPA;
         if (s == "MARL")
            return TrackingCode::MARL;
         if (s == "MARLD")
            return TrackingCode::MARLD;
         if (s == "MARLP")
            return TrackingCode::MARLP;
         if (s == "Mprime")
            return TrackingCode::Mprime;
         if (s == "MprimePA")
            return TrackingCode::MprimePA;
         if (s == "L2CM")
            return TrackingCode::L2CM;
         if (s == "L2CL")
            return TrackingCode::L2CL;
         if (s == "L2CML")
            return TrackingCode::L2CML;
         if (s == "L5I")
            return TrackingCode::L5I;
         if (s == "L5Q")
            return TrackingCode::L5Q;
         if (s == "L5IQ")
            return TrackingCode::L5IQ;
         if (s == "L1CP")
            return TrackingCode::L1CP;
         if (s == "L1CD")
            return TrackingCode::L1CD;
         if (s == "L1CDP")
            return TrackingCode::L1CDP;
         if (s == "NSCA")
            return TrackingCode::NSCA;
         if (s == "NSCD")
            return TrackingCode::NSCD;
         if (s == "NSCL")
            return TrackingCode::NSCL;
         if (s == "NSCM")
            return TrackingCode::NSCM;
         if (s == "NSCMCL")
            return TrackingCode::NSCMCL;
         if (s == "NSCP")
            return TrackingCode::NSCP;
         if (s == "NSCPCD")
            return TrackingCode::NSCPCD;
         if (s == "NSI5")
            return TrackingCode::NSI5;
         if (s == "NSM")
            return TrackingCode::NSM;
         if (s == "NSMPA")
            return TrackingCode::NSMPA;
         if (s == "NSMprime")
            return TrackingCode::NSMprime;
         if (s == "NSMprimePA")
            return TrackingCode::NSMprimePA;
         if (s == "NSP")
            return TrackingCode::NSP;
         if (s == "NSQ5")
            return TrackingCode::NSQ5;
         if (s == "NSY")
            return TrackingCode::NSY;
         if (s == "Standard")
            return TrackingCode::Standard;
         if (s == "Precise")
            return TrackingCode::Precise;
         if (s == "L3OCD")
            return TrackingCode::L3OCD;
         if (s == "L3OCP")
            return TrackingCode::L3OCP;
         if (s == "L3OCDP")
            return TrackingCode::L3OCDP;
         if (s == "L1OCD")
            return TrackingCode::L1OCD;
         if (s == "L1OCP")
            return TrackingCode::L1OCP;
         if (s == "L1OCDP")
            return TrackingCode::L1OCDP;
         if (s == "L2CSIL2OCp")
            return TrackingCode::L2CSIL2OCp;
         if (s == "L2CSI")
            return TrackingCode::L2CSI;
         if (s == "L2OCP")
            return TrackingCode::L2OCP;
         if (s == "G3TestData")
            return TrackingCode::G3TestData;
         if (s == "G3TestPilot")
            return TrackingCode::G3TestPilot;
         if (s == "L1SC")
            return TrackingCode::L1SC;
         if (s == "L2SC")
            return TrackingCode::L2SC;
         if (s == "E1A")
            return TrackingCode::E1A;
         if (s == "E1B")
            return TrackingCode::E1B;
         if (s == "E1C")
            return TrackingCode::E1C;
         if (s == "E1BC")
            return TrackingCode::E1BC;
         if (s == "E1ABC")
            return TrackingCode::E1ABC;
         if (s == "E5abI")
            return TrackingCode::E5abI;
         if (s == "E5abQ")
            return TrackingCode::E5abQ;
         if (s == "E5abIQ")
            return TrackingCode::E5abIQ;
         if (s == "E5aI")
            return TrackingCode::E5aI;
         if (s == "E5aQ")
            return TrackingCode::E5aQ;
         if (s == "E5aIQ")
            return TrackingCode::E5aIQ;
         if (s == "E5bI")
            return TrackingCode::E5bI;
         if (s == "E5bQ")
            return TrackingCode::E5bQ;
         if (s == "E5bIQ")
            return TrackingCode::E5bIQ;
         if (s == "E6A")
            return TrackingCode::E6A;
         if (s == "E6B")
            return TrackingCode::E6B;
         if (s == "E6C")
            return TrackingCode::E6C;
         if (s == "E6BC")
            return TrackingCode::E6BC;
         if (s == "E6ABC")
            return TrackingCode::E6ABC;
         if (s == "L1S")
            return TrackingCode::L1S;
         if (s == "L5SI")
            return TrackingCode::L5SI;
         if (s == "L5SQ")
            return TrackingCode::L5SQ;
         if (s == "L5SIQ")
            return TrackingCode::L5SIQ;
         if (s == "LEXS")
            return TrackingCode::LEXS;
         if (s == "LEXL")
            return TrackingCode::LEXL;
         if (s == "LEXSL")
            return TrackingCode::LEXSL;
         if (s == "L6D")
            return TrackingCode::L6D;
         if (s == "L6E")
            return TrackingCode::L6E;
         if (s == "L6DE")
            return TrackingCode::L6DE;
         if (s == "B1I")
            return TrackingCode::B1I;
         if (s == "B1Q")
            return TrackingCode::B1Q;
         if (s == "B1IQ")
            return TrackingCode::B1IQ;
         if (s == "B2I")
            return TrackingCode::B2I;
         if (s == "B2Q")
            return TrackingCode::B2Q;
         if (s == "B2IQ")
            return TrackingCode::B2IQ;
         if (s == "B3I")
            return TrackingCode::B3I;
         if (s == "B3Q")
            return TrackingCode::B3Q;
         if (s == "B3IQ")
            return TrackingCode::B3IQ;
         if (s == "B1A")
            return TrackingCode::B1A;
         if (s == "B1CD")
            return TrackingCode::B1CD;
         if (s == "B1CDP")
            return TrackingCode::B1CDP;
         if (s == "B1CP")
            return TrackingCode::B1CP;
         if (s == "B2abI")
            return TrackingCode::B2abI;
         if (s == "B2abIQ")
            return TrackingCode::B2abIQ;
         if (s == "B2abQ")
            return TrackingCode::B2abQ;
         if (s == "B2aI")
            return TrackingCode::B2aI;
         if (s == "B2aIQ")
            return TrackingCode::B2aIQ;
         if (s == "B2aQ")
            return TrackingCode::B2aQ;
         if (s == "B2bI")
            return TrackingCode::B2bI;
         if (s == "B2bIQ")
            return TrackingCode::B2bIQ;
         if (s == "B2bQ")
            return TrackingCode::B2bQ;
         if (s == "B3AQ")
            return TrackingCode::B3AQ;
         if (s == "BCodeless")
            return TrackingCode::BCodeless;
         if (s == "B3AI")
            return TrackingCode::B3AI;
         if (s == "B3AIQ")
            return TrackingCode::B3AIQ;
         if (s == "SPSL5")
            return TrackingCode::SPSL5;
         if (s == "RSL5D")
            return TrackingCode::RSL5D;
         if (s == "RSL5P")
            return TrackingCode::RSL5P;
         if (s == "RSL5DP")
            return TrackingCode::RSL5DP;
         if (s == "SPSS")
            return TrackingCode::SPSS;
         if (s == "RSSD")
            return TrackingCode::RSSD;
         if (s == "RSSP")
            return TrackingCode::RSSP;
         if (s == "RSSDP")
            return TrackingCode::RSSDP;
         if (s == "CW")
            return TrackingCode::CW;
         if (s == "Undefined")
            return TrackingCode::Undefined;
         return TrackingCode::Unknown;
      } // asTrackingCode(string)
   } // namespace StringUtils
} // namespace gpstk
