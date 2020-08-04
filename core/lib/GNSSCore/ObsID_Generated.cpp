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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University
//  of Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
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
      { TrackingCode::Unknown,    "UnknownCode" },
      { TrackingCode::Any,        "AnyCode" },
      { TrackingCode::CA,         "GPSC/A" },
      { TrackingCode::P,          "GPSP" },
      { TrackingCode::Y,          "GPSY" },
      { TrackingCode::W,          "GPScodelessZ" },
      { TrackingCode::N,          "GPSsquare" },
      { TrackingCode::D,          "GPScodeless" },
      { TrackingCode::M,          "GPSM" },
      { TrackingCode::C2M,        "GPSC2M" },
      { TrackingCode::C2L,        "GPSC2L" },
      { TrackingCode::C2LM,       "GPSC2L+M" },
      { TrackingCode::I5,         "GPSI5" },
      { TrackingCode::Q5,         "GPSQ5" },
      { TrackingCode::IQ5,        "GPSI+Q5" },
      { TrackingCode::G1P,        "GPSC1P" },
      { TrackingCode::G1D,        "GPSC1D" },
      { TrackingCode::G1X,        "GPSC1(D+P)" },
      { TrackingCode::GCA,        "GLOC/A" },
      { TrackingCode::GP,         "GLOP" },
      { TrackingCode::IR3,        "GLOIR5" },
      { TrackingCode::QR3,        "GLOQR5" },
      { TrackingCode::IQR3,       "GLOI+QR5" },
      { TrackingCode::L1OCD,      "GLOL1OCD" },
      { TrackingCode::L1OCP,      "GLOL1OCP" },
      { TrackingCode::L1OC,       "GLOL1OC" },
      { TrackingCode::L2CSIOCp,   "GLOL2CSI+OCp" },
      { TrackingCode::L2CSI,      "GLOL2CSI" },
      { TrackingCode::L2OCP,      "GLOL2OCP" },
      { TrackingCode::A,          "GALA" },
      { TrackingCode::B,          "GALB" },
      { TrackingCode::C,          "GALC" },
      { TrackingCode::BC,         "GALB+C" },
      { TrackingCode::ABC,        "GALA+B+C" },
      { TrackingCode::IE5,        "GALI5" },
      { TrackingCode::QE5,        "GALQ5" },
      { TrackingCode::IQE5,       "GALI+Q5" },
      { TrackingCode::IE5a,       "GALI5a" },
      { TrackingCode::QE5a,       "GALQ5a" },
      { TrackingCode::IQE5a,      "GALI+Q5a" },
      { TrackingCode::IE5b,       "GALI5b" },
      { TrackingCode::QE5b,       "GALQ5b" },
      { TrackingCode::IQE5b,      "GALI+Q5b" },
      { TrackingCode::A6,         "GALA6" },
      { TrackingCode::B6,         "GALB6" },
      { TrackingCode::C6,         "GALC6" },
      { TrackingCode::BC6,        "GALB+C6" },
      { TrackingCode::ABC6,       "GALA+B+C6" },
      { TrackingCode::SCA,        "SBASC/A" },
      { TrackingCode::SI5,        "SBASI5" },
      { TrackingCode::SQ5,        "SBASQ5" },
      { TrackingCode::SIQ5,       "SBASI+Q5" },
      { TrackingCode::JCA,        "QZSSC/A" },
      { TrackingCode::JD1,        "QZSSL1C(D)" },
      { TrackingCode::JP1,        "QZSSL1C(P)" },
      { TrackingCode::JX1,        "QZSSL1C(D+P)" },
      { TrackingCode::JZ1,        "QZSSL1-SAIF" },
      { TrackingCode::JM2,        "QZSSL2C(M)" },
      { TrackingCode::JL2,        "QZSSL2C(L)" },
      { TrackingCode::JX2,        "QZSSL2C(M+L)" },
      { TrackingCode::JI5,        "QZSSL5I" },
      { TrackingCode::JQ5,        "QZSSL5Q" },
      { TrackingCode::JIQ5,       "QZSSL5I+Q" },
      { TrackingCode::JI5S,       "QZSSL5SI" },
      { TrackingCode::JQ5S,       "QZSSL5SQ" },
      { TrackingCode::JIQ5S,      "QZSSL5SI+Q" },
      { TrackingCode::JI6,        "QZSSL6I" },
      { TrackingCode::JQ6,        "QZSSL6Q" },
      { TrackingCode::JIQ6,       "QZSSL6I+Q" },
      { TrackingCode::JD6,        "QZSSL6D" },
      { TrackingCode::JE6,        "QZSSL6E" },
      { TrackingCode::JDE6,       "QZSSL6D+E" },
      { TrackingCode::CI1,        "BDSIB1" },
      { TrackingCode::CQ1,        "BDSQB1" },
      { TrackingCode::CIQ1,       "BDSI+QB1" },
      { TrackingCode::CI7,        "BDSIB2" },
      { TrackingCode::CQ7,        "BDSQB2" },
      { TrackingCode::CIQ7,       "BDSI+QB2" },
      { TrackingCode::CI6,        "BDSIB3" },
      { TrackingCode::CQ6,        "BDSQB3" },
      { TrackingCode::CIQ6,       "BDSI+QB3" },
      { TrackingCode::CA1,        "BDS1A" },
      { TrackingCode::CCD1,       "BDS1CD" },
      { TrackingCode::CCDP1,      "BDS1CDP" },
      { TrackingCode::CCP1,       "BDS1CP" },
      { TrackingCode::CI2ab,      "BDSIB2ab" },
      { TrackingCode::CIQ2ab,     "BDSI+QB2ab" },
      { TrackingCode::CQ2ab,      "BDSQB2ab" },
      { TrackingCode::CI2a,       "BDSIB2a" },
      { TrackingCode::CIQ2a,      "BDSI+QB2a" },
      { TrackingCode::CQ2a,       "BDSQB2a" },
      { TrackingCode::CI2b,       "BDSIB2b" },
      { TrackingCode::CIQ2b,      "BDSI+QB2b" },
      { TrackingCode::CQ2b,       "BDSQB2b" },
      { TrackingCode::CodelessC,  "BDSCodeless" },
      { TrackingCode::CIQ3A,      "BDSI+Q3A" },
      { TrackingCode::IA5,        "IRNSSL5A" },
      { TrackingCode::IB5,        "IRNSSL5B" },
      { TrackingCode::IC5,        "IRNSSL5C" },
      { TrackingCode::IX5,        "IRNSSL5B+C" },
      { TrackingCode::IA9,        "IRNSSL9A" },
      { TrackingCode::IB9,        "IRNSSL9B" },
      { TrackingCode::IC9,        "IRNSSL9C" },
      { TrackingCode::IX9,        "IRNSSL9B+C" },
      { TrackingCode::Undefined,  "undefined" },
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

