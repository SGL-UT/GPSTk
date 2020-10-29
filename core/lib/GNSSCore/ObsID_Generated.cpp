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

#include "ObsID.hpp"

namespace gpstk
{
   // descriptions (strings) of each code, carrier and type
   std::map<TrackingCode, std::string> ObsID::tcDesc {
      { TrackingCode::Unknown,       "UnknownCode" },
      { TrackingCode::Any,           "AnyCode" },
      { TrackingCode::CA,            "GPSC/A" },
      { TrackingCode::P,             "GPSP" },
      { TrackingCode::Y,             "GPSY" },
      { TrackingCode::Ztracking,     "GPScodelessZ" },
      { TrackingCode::YCodeless,     "GPSsquare" },
      { TrackingCode::Semicodeless,  "GPScodeless" },
      { TrackingCode::MD,            "GPSMD" },
      { TrackingCode::MDP,           "GPSM" },
      { TrackingCode::MP,            "GPSMP" },
      { TrackingCode::MPA,           "GPSMPA" },
      { TrackingCode::MARL,          "GPSMARL" },
      { TrackingCode::MARLD,         "GPSMARLD" },
      { TrackingCode::MARLP,         "GPSMARLP" },
      { TrackingCode::Mprime,        "GPSM'" },
      { TrackingCode::MprimePA,      "GPSM'PA" },
      { TrackingCode::L2CM,          "GPSC2M" },
      { TrackingCode::L2CL,          "GPSC2L" },
      { TrackingCode::L2CML,         "GPSC2L+M" },
      { TrackingCode::L5I,           "GPSI5" },
      { TrackingCode::L5Q,           "GPSQ5" },
      { TrackingCode::L5IQ,          "GPSI+Q5" },
      { TrackingCode::L1CP,          "GPSC1P" },
      { TrackingCode::L1CD,          "GPSC1D" },
      { TrackingCode::L1CDP,         "GPSC1(D+P)" },
      { TrackingCode::NSCA,          "GPSNSCA" },
      { TrackingCode::NSCD,          "GPSNSCD" },
      { TrackingCode::NSCL,          "GPSNSCL" },
      { TrackingCode::NSCM,          "GPSNSCM" },
      { TrackingCode::NSCMCL,        "GPSNSCL+M" },
      { TrackingCode::NSCP,          "GPSNSCP" },
      { TrackingCode::NSCPCD,        "GPSNSC1(D+P)" },
      { TrackingCode::NSI5,          "GPSNSI5" },
      { TrackingCode::NSM,           "GPSNSM" },
      { TrackingCode::NSMPA,         "GPSNSMPA" },
      { TrackingCode::NSMprime,      "GPSNSM'" },
      { TrackingCode::NSMprimePA,    "GPSNSM'PA" },
      { TrackingCode::NSP,           "GPSNSP" },
      { TrackingCode::NSQ5,          "GPSNSQ5" },
      { TrackingCode::NSY,           "GPSNSY" },
      { TrackingCode::Standard,      "GLOC/A" },
      { TrackingCode::Precise,       "GLOP" },
      { TrackingCode::L3OCD,         "GLOIR5" },
      { TrackingCode::L3OCP,         "GLOQR5" },
      { TrackingCode::L3OCDP,        "GLOI+QR5" },
      { TrackingCode::L1OCD,         "GLOL1OCD" },
      { TrackingCode::L1OCP,         "GLOL1OCP" },
      { TrackingCode::L1OCDP,        "GLOL1OC" },
      { TrackingCode::L2CSIL2OCp,    "GLOL2CSI+OCp" },
      { TrackingCode::L2CSI,         "GLOL2CSI" },
      { TrackingCode::L2OCP,         "GLOL2OCP" },
      { TrackingCode::G3TestData,    "GLOGC3D" },
      { TrackingCode::G3TestPilot,   "GLOGC3P" },
      { TrackingCode::L1SC,          "GLOL1SC" },
      { TrackingCode::L2SC,          "GLOL2SC" },
      { TrackingCode::E1A,           "GALA" },
      { TrackingCode::E1B,           "GALB" },
      { TrackingCode::E1C,           "GALC" },
      { TrackingCode::E1BC,          "GALB+C" },
      { TrackingCode::E1ABC,         "GALA+B+C" },
      { TrackingCode::E5abI,         "GALI5" },
      { TrackingCode::E5abQ,         "GALQ5" },
      { TrackingCode::E5abIQ,        "GALI+Q5" },
      { TrackingCode::E5aI,          "GALI5a" },
      { TrackingCode::E5aQ,          "GALQ5a" },
      { TrackingCode::E5aIQ,         "GALI+Q5a" },
      { TrackingCode::E5bI,          "GALI5b" },
      { TrackingCode::E5bQ,          "GALQ5b" },
      { TrackingCode::E5bIQ,         "GALI+Q5b" },
      { TrackingCode::E6A,           "GALA6" },
      { TrackingCode::E6B,           "GALB6" },
      { TrackingCode::E6C,           "GALC6" },
      { TrackingCode::E6BC,          "GALB+C6" },
      { TrackingCode::E6ABC,         "GALA+B+C6" },
      { TrackingCode::L1S,           "QZSSL1-SAIF" },
      { TrackingCode::L5SI,          "QZSSL5SI" },
      { TrackingCode::L5SQ,          "QZSSL5SQ" },
      { TrackingCode::L5SIQ,         "QZSSL5SI+Q" },
      { TrackingCode::LEXS,          "QZSSL6I" },
      { TrackingCode::LEXL,          "QZSSL6Q" },
      { TrackingCode::LEXSL,         "QZSSL6I+Q" },
      { TrackingCode::L6D,           "QZSSL6D" },
      { TrackingCode::L6E,           "QZSSL6E" },
      { TrackingCode::L6DE,          "QZSSL6D+E" },
      { TrackingCode::B1I,           "BDSIB1" },
      { TrackingCode::B1Q,           "BDSQB1" },
      { TrackingCode::B1IQ,          "BDSI+QB1" },
      { TrackingCode::B2I,           "BDSIB2" },
      { TrackingCode::B2Q,           "BDSQB2" },
      { TrackingCode::B2IQ,          "BDSI+QB2" },
      { TrackingCode::B3I,           "BDSIB3" },
      { TrackingCode::B3Q,           "BDSQB3" },
      { TrackingCode::B3IQ,          "BDSI+QB3" },
      { TrackingCode::B1A,           "BDS1A" },
      { TrackingCode::B1CD,          "BDS1CD" },
      { TrackingCode::B1CDP,         "BDS1CDP" },
      { TrackingCode::B1CP,          "BDS1CP" },
      { TrackingCode::B2abI,         "BDSIB2ab" },
      { TrackingCode::B2abIQ,        "BDSI+QB2ab" },
      { TrackingCode::B2abQ,         "BDSQB2ab" },
      { TrackingCode::B2aI,          "BDSIB2a" },
      { TrackingCode::B2aIQ,         "BDSI+QB2a" },
      { TrackingCode::B2aQ,          "BDSQB2a" },
      { TrackingCode::B2bI,          "BDSIB2b" },
      { TrackingCode::B2bIQ,         "BDSI+QB2b" },
      { TrackingCode::B2bQ,          "BDSQB2b" },
      { TrackingCode::B3AQ,          "BDSQB3A" },
      { TrackingCode::BCodeless,     "BDSCodeless" },
      { TrackingCode::B3AI,          "BDSIB3A" },
      { TrackingCode::B3AIQ,         "BDSI+Q3A" },
      { TrackingCode::SPSL5,         "IRNSSL5A" },
      { TrackingCode::RSL5D,         "IRNSSL5B" },
      { TrackingCode::RSL5P,         "IRNSSL5C" },
      { TrackingCode::RSL5DP,        "IRNSSL5B+C" },
      { TrackingCode::SPSS,          "IRNSSL9A" },
      { TrackingCode::RSSD,          "IRNSSL9B" },
      { TrackingCode::RSSP,          "IRNSSL9C" },
      { TrackingCode::RSSDP,         "IRNSSL9B+C" },
      { TrackingCode::CW,            "CW" },
      { TrackingCode::Undefined,     "undefined" },
   }; // std::map<TrackingCode, std::string> ObsID::tcDesc

   std::map<CarrierBand, std::string> ObsID::cbDesc {
      { CarrierBand::Unknown,    "UnknownBand" },
      { CarrierBand::Any,        "AnyBand" },
      { CarrierBand::L1L2,       "comboL1L2" },
      { CarrierBand::L1,         "L1" },
      { CarrierBand::L2,         "L2" },
      { CarrierBand::L5,         "L5" },
      { CarrierBand::G1,         "G1" },
      { CarrierBand::G1a,        "G1a" },
      { CarrierBand::G2a,        "G2a" },
      { CarrierBand::G2,         "G2" },
      { CarrierBand::G3,         "G3" },
      { CarrierBand::E5b,        "E5b" },
      { CarrierBand::E5ab,       "E5a+b" },
      { CarrierBand::E6,         "E6" },
      { CarrierBand::B1,         "B1" },
      { CarrierBand::B2,         "B2" },
      { CarrierBand::B3,         "B3" },
      { CarrierBand::I9,         "I9" },
      { CarrierBand::Undefined,  "undefined" },
   }; // std::map<CarrierBand, std::string> ObsID::cbDesc

   std::map<ObservationType, std::string> ObsID::otDesc {
      { ObservationType::Unknown,    "UnknownType" },
      { ObservationType::Any,        "AnyType" },
      { ObservationType::Range,      "pseudorange" },
      { ObservationType::Phase,      "phase" },
      { ObservationType::Doppler,    "doppler" },
      { ObservationType::SNR,        "snr" },
      { ObservationType::Channel,    "channel" },
      { ObservationType::DemodStat,  "demodStatus" },
      { ObservationType::Iono,       "iono" },
      { ObservationType::SSI,        "ssi" },
      { ObservationType::LLI,        "lli" },
      { ObservationType::TrackLen,   "tlen" },
      { ObservationType::NavMsg,     "navmsg" },
      { ObservationType::RngStdDev,  "rngSigma" },
      { ObservationType::PhsStdDev,  "phsSigma" },
      { ObservationType::FreqIndx,   "freqIndx" },
      { ObservationType::Undefined,  "undefined" },
   }; // std::map<ObservationType, std::string> ObsID::otDesc
} // namespace gpstk

