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

#include "RinexObsID.hpp"

namespace gpstk
{
      // Mappings between code, carrier, type and characters.
      // The following definitions really should only describe the
      // items that are in the Rinex 3 specification. If an
      // application needs additional ObsID types to be able to be
      // translated to/from Rinex3, the additional types must be added
      // by the application.
   std::map<char, ObservationType> RinexObsID::char2ot {
      { ' ',  ObservationType::Unknown },
      { '*',  ObservationType::Any },
      { 'C',  ObservationType::Range },
      { 'L',  ObservationType::Phase },
      { 'D',  ObservationType::Doppler },
      { 'S',  ObservationType::SNR },
      { 'X',  ObservationType::Channel },
      { 'I',  ObservationType::Iono },
      { '-',  ObservationType::Undefined },
   }; // std::map<char, ObservationType> RinexObsID::char2ot

   std::map<char, CarrierBand> RinexObsID::char2cb {
      { ' ',  CarrierBand::Unknown },
      { '*',  CarrierBand::Any },
      { '1',  CarrierBand::L1 },
      { '2',  CarrierBand::L2 },
      { '5',  CarrierBand::L5 },
      { '4',  CarrierBand::G1a },
      { '6',  CarrierBand::G2a },
      { '3',  CarrierBand::G3 },
      { '7',  CarrierBand::E5b },
      { '8',  CarrierBand::E5ab },
      { '9',  CarrierBand::I9 },
      { '-',  CarrierBand::Undefined },
   }; // std::map<char, CarrierBand> RinexObsID::char2cb

   std::map<char, TrackingCode> RinexObsID::char2tc {
      { ' ',  TrackingCode::Unknown },
      { '*',  TrackingCode::Any },
      { 'C',  TrackingCode::CA },
      { 'P',  TrackingCode::P },
      { 'Y',  TrackingCode::Y },
      { 'W',  TrackingCode::Ztracking },
      { 'N',  TrackingCode::YCodeless },
      { 'D',  TrackingCode::Semicodeless },
      { 'M',  TrackingCode::MDP },
      { 'S',  TrackingCode::L2CM },
      { 'L',  TrackingCode::L2CL },
      { 'X',  TrackingCode::L2CML },
      { 'I',  TrackingCode::L5I },
      { 'Q',  TrackingCode::L5Q },
      { 'A',  TrackingCode::L1OCD },
      { 'B',  TrackingCode::L1OCP },
      { 'Z',  TrackingCode::E1ABC },
      { 'E',  TrackingCode::L6E },
      { '-',  TrackingCode::Undefined },
   }; // std::map<char, TrackingCode> RinexObsID::char2tc

   std::map<ObservationType, char> RinexObsID::ot2char {
      { ObservationType::Unknown,    ' ' },
      { ObservationType::Any,        '*' },
      { ObservationType::Range,      'C' },
      { ObservationType::Phase,      'L' },
      { ObservationType::Doppler,    'D' },
      { ObservationType::SNR,        'S' },
      { ObservationType::Channel,    'X' },
      { ObservationType::Iono,       'I' },
      { ObservationType::Undefined,  '-' },
   }; // std::map<ObservationType, char> RinexObsID::ot2char

   std::map<CarrierBand, char> RinexObsID::cb2char {
      { CarrierBand::Unknown,    ' ' },
      { CarrierBand::Any,        '*' },
      { CarrierBand::L1,         '1' },
      { CarrierBand::L2,         '2' },
      { CarrierBand::L5,         '5' },
      { CarrierBand::G1,         '1' },
      { CarrierBand::G1a,        '4' },
      { CarrierBand::G2a,        '6' },
      { CarrierBand::G2,         '2' },
      { CarrierBand::G3,         '3' },
      { CarrierBand::E5b,        '7' },
      { CarrierBand::E5ab,       '8' },
      { CarrierBand::E6,         '6' },
      { CarrierBand::B1,         '2' },
      { CarrierBand::B2,         '7' },
      { CarrierBand::B3,         '6' },
      { CarrierBand::I9,         '9' },
      { CarrierBand::Undefined,  '-' },
   }; // std::map<CarrierBand, char> RinexObsID::cb2char

   std::map<TrackingCode, char> RinexObsID::tc2char {
      { TrackingCode::Unknown,       ' ' },
      { TrackingCode::Any,           '*' },
      { TrackingCode::CA,            'C' },
      { TrackingCode::P,             'P' },
      { TrackingCode::Y,             'Y' },
      { TrackingCode::Ztracking,     'W' },
      { TrackingCode::YCodeless,     'N' },
      { TrackingCode::Semicodeless,  'D' },
      { TrackingCode::MDP,           'M' },
      { TrackingCode::L2CM,          'S' },
      { TrackingCode::L2CL,          'L' },
      { TrackingCode::L2CML,         'X' },
      { TrackingCode::L5I,           'I' },
      { TrackingCode::L5Q,           'Q' },
      { TrackingCode::L5IQ,          'X' },
      { TrackingCode::L1CP,          'L' },
      { TrackingCode::L1CD,          'S' },
      { TrackingCode::L1CDP,         'X' },
      { TrackingCode::Standard,      'C' },
      { TrackingCode::Precise,       'P' },
      { TrackingCode::L3OCD,         'I' },
      { TrackingCode::L3OCP,         'Q' },
      { TrackingCode::L3OCDP,        'X' },
      { TrackingCode::L1OCD,         'A' },
      { TrackingCode::L1OCP,         'B' },
      { TrackingCode::L1OCDP,        'X' },
      { TrackingCode::L2CSIL2OCp,    'X' },
      { TrackingCode::L2CSI,         'A' },
      { TrackingCode::L2OCP,         'B' },
      { TrackingCode::E1A,           'A' },
      { TrackingCode::E1B,           'B' },
      { TrackingCode::E1C,           'C' },
      { TrackingCode::E1BC,          'X' },
      { TrackingCode::E1ABC,         'Z' },
      { TrackingCode::E5abI,         'I' },
      { TrackingCode::E5abQ,         'Q' },
      { TrackingCode::E5abIQ,        'X' },
      { TrackingCode::E5aI,          'I' },
      { TrackingCode::E5aQ,          'Q' },
      { TrackingCode::E5aIQ,         'X' },
      { TrackingCode::E5bI,          'I' },
      { TrackingCode::E5bQ,          'Q' },
      { TrackingCode::E5bIQ,         'X' },
      { TrackingCode::E6A,           'A' },
      { TrackingCode::E6B,           'B' },
      { TrackingCode::E6C,           'C' },
      { TrackingCode::E6BC,          'X' },
      { TrackingCode::E6ABC,         'Z' },
      { TrackingCode::L1S,           'Z' },
      { TrackingCode::L5SI,          'D' },
      { TrackingCode::L5SQ,          'P' },
      { TrackingCode::L5SIQ,         'Z' },
      { TrackingCode::LEXS,          'S' },
      { TrackingCode::LEXL,          'L' },
      { TrackingCode::LEXSL,         'X' },
      { TrackingCode::L6D,           'S' },
      { TrackingCode::L6E,           'E' },
      { TrackingCode::L6DE,          'Z' },
      { TrackingCode::B1I,           'I' },
      { TrackingCode::B1Q,           'Q' },
      { TrackingCode::B1IQ,          'X' },
      { TrackingCode::B2I,           'I' },
      { TrackingCode::B2Q,           'Q' },
      { TrackingCode::B2IQ,          'X' },
      { TrackingCode::B3I,           'I' },
      { TrackingCode::B3Q,           'Q' },
      { TrackingCode::B3IQ,          'X' },
      { TrackingCode::B1A,           'A' },
      { TrackingCode::B1CD,          'D' },
      { TrackingCode::B1CDP,         'X' },
      { TrackingCode::B1CP,          'P' },
      { TrackingCode::B2abI,         'D' },
      { TrackingCode::B2abIQ,        'X' },
      { TrackingCode::B2abQ,         'P' },
      { TrackingCode::B2aI,          'D' },
      { TrackingCode::B2aIQ,         'X' },
      { TrackingCode::B2aQ,          'P' },
      { TrackingCode::B2bI,          'D' },
      { TrackingCode::B2bIQ,         'Z' },
      { TrackingCode::B2bQ,          'P' },
      { TrackingCode::BCodeless,     'N' },
      { TrackingCode::B3AIQ,         'A' },
      { TrackingCode::SPSL5,         'A' },
      { TrackingCode::RSL5D,         'B' },
      { TrackingCode::RSL5P,         'C' },
      { TrackingCode::RSL5DP,        'X' },
      { TrackingCode::SPSS,          'A' },
      { TrackingCode::RSSD,          'B' },
      { TrackingCode::RSSP,          'C' },
      { TrackingCode::RSSDP,         'X' },
      { TrackingCode::Undefined,     '-' },
   }; // std::map<TrackingCode, char> RinexObsID::tc2char

   // map of valid RINEX tracking codes, systems and frequency
   std::map<char, std::map<char, std::string> >
   RinexObsID::validRinexTrackingCodes {
      { 'G', {
               // 1N: Pseudorange (C) not valid
            { '1', "PYWLMCSXN* " },
               // 2N: Pseudorange (C) not valid
            { '2', "PYWLMCSXDN* " },
            { '5', "IQX* " } } },

      { 'R', {
            { '1', "PC* " },
            { '2', "PC* " },
            { '3', "IQX* " },
            { '4', "ABX* " },
            { '6', "ABX* " } } },

      { 'E', {
            { '1', "ABCXZ* " },
            { '5', "IQX* " },
            { '6', "ABCXZ* " },
            { '7', "IQX* " },
            { '8', "IQX* " } } },

      { 'S', {
            { '1', "C* " },
            { '5', "IQX* " } } },

      { 'J', {
            { '1', "CSLXZ* " },
            { '2', "SLX* " },
            { '5', "IQDPXZ* " },
            { '6', "ESLXZ* " } } },

      { 'C', {
               // 1I: RINEX 3.02 kludge
               // 1Q: RINEX 3.02 kludge
            { '1', "PAIQXDN* " },
            { '2', "IQX* " },
            { '5', "PXD* " },
            { '6', "AIQX* " },
            { '7', "ZPIQXD* " },
            { '8', "PXD* " } } },

      { 'I', {
            { '1', "* " }, // I1 is *only* for channel number
            { '5', "ABCX* " },
            { '9', "ABCX* " } } }
   }; // RinexObsID::validRinexTrackingCodes

   RinexObsID::RinexObsID(const std::string& strID, double version)
         : rinexVersion(version)
   {
      std::string modStrID(strID);
      type = ObservationType::Unknown;
      band = CarrierBand::Unknown;
      code = TrackingCode::Unknown;
      int i = modStrID.length() - 3;
      if (i < 0 || i > 1)
      {
         InvalidParameter e("identifier must be 3 or 4 characters long");
         GPSTK_THROW(e);
      }
      else if (i == 0)
      {
         modStrID = std::string("G") + modStrID; // GPS is default
      }

         // make an initial guess
      const auto& tc = char2tc.find(modStrID[3]);
      const auto& cb = char2cb.find(modStrID[2]);
      const auto& ot = char2ot.find(modStrID[1]);
      if (tc != char2tc.end())
         code = tc->second;
      if (cb != char2cb.end())
         band = cb->second;
      if (ot != char2ot.end())
         type = ot->second;

      std::string rincode(modStrID.substr(2));
         // do the observation type which is mapped 1:1
      switch (modStrID[1])
      {
         case ' ': // Unknown
            type = ObservationType::Unknown;
            break;
         case '*': // Any
            type = ObservationType::Any;
            break;
         case 'C': // Range
            type = ObservationType::Range;
            break;
         case 'L': // Phase
            type = ObservationType::Phase;
            break;
         case 'D': // Doppler
            type = ObservationType::Doppler;
            break;
         case 'S': // SNR
            type = ObservationType::SNR;
            break;
         case 'X': // Channel
            type = ObservationType::Channel;
            if ((modStrID[2] != '1') || (modStrID[3] != ' '))
            {
               InvalidParameter e("Invalid channel number pseudo-obs ID " +
                                  strID);
               GPSTK_THROW(e);
            }
            code = TrackingCode::Undefined;
               /** @bug Not convinced this is an appropriate value, but
                * it's what has been used in the past. */
            band = CarrierBand::L1;
            break;
         case 'I': // Iono
            type = ObservationType::Iono;
            if ((modStrID[2] < '1') || (modStrID[2] > '9') ||
                (modStrID[3] != ' '))
            {
               InvalidParameter e("Invalid ionospheric delay pseudo-obs ID " +
                                  strID);
               GPSTK_THROW(e);
            }
            code = TrackingCode::Undefined;
            band = char2cb[modStrID[2]];
            break;
         case '-': // Undefined
            type = ObservationType::Undefined;
            break;
         default:
            type = ObservationType::Unknown;
            break;
      }
         // group first by rinex system ID
      switch (modStrID[0])
      {
         case 'C': // BeiDou
            if (rincode == "2I")
            {
               band = CarrierBand::B1;
               code = TrackingCode::B1I;
            }
            else if (rincode == "2Q")
            {
               band = CarrierBand::B1;
               code = TrackingCode::B1Q;
            }
            else if (rincode == "2X")
            {
               band = CarrierBand::B1;
               code = TrackingCode::B1IQ;
            }
            else if (rincode == "1D")
            {
               band = CarrierBand::L1;
               code = TrackingCode::B1CD;
            }
            else if (rincode == "1P")
            {
               band = CarrierBand::L1;
               code = TrackingCode::B1CP;
            }
            else if (rincode == "1X")
            {
               if (fabs(rinexVersion - 3.02) < 0.005)
               {
                  band = CarrierBand::B1;
                  code = TrackingCode::B1IQ;
               }
               else
               {
                  band = CarrierBand::L1;
                  code = TrackingCode::B1CDP;
               }
            }
            else if (rincode == "1A")
            {
               band = CarrierBand::L1;
               code = TrackingCode::B1A;
            }
            else if (rincode == "1N")
            {
               band = CarrierBand::L1;
               code = TrackingCode::BCodeless;
            }
            else if (rincode == "1I")
            {
               band = CarrierBand::B1;
               code = TrackingCode::B1I;
            }
            else if (rincode == "1Q")
            {
               band = CarrierBand::B1;
               code = TrackingCode::B1Q;
            }
            else if (rincode == "5D")
            {
               band = CarrierBand::L5;
               code = TrackingCode::B2aI;
            }
            else if (rincode == "5P")
            {
               band = CarrierBand::L5;
               code = TrackingCode::B2aQ;
            }
            else if (rincode == "5X")
            {
               band = CarrierBand::L5;
               code = TrackingCode::B2aIQ;
            }
            else if (rincode == "7I")
            {
               band = CarrierBand::B2;
               code = TrackingCode::B2I;
            }
            else if (rincode == "7Q")
            {
               band = CarrierBand::B2;
               code = TrackingCode::B2Q;
            }
            else if (rincode == "7X")
            {
               band = CarrierBand::B2;
               code = TrackingCode::B2IQ;
            }
            else if (rincode == "7D")
            {
               band = CarrierBand::B2;
               code = TrackingCode::B2bI;
            }
            else if (rincode == "7P")
            {
               band = CarrierBand::B2;
               code = TrackingCode::B2bQ;
            }
            else if (rincode == "7Z")
            {
               band = CarrierBand::B2;
               code = TrackingCode::B2bIQ;
            }
            else if (rincode == "8D")
            {
               band = CarrierBand::E5ab;
               code = TrackingCode::B2abI;
            }
            else if (rincode == "8P")
            {
               band = CarrierBand::E5ab;
               code = TrackingCode::B2abQ;
            }
            else if (rincode == "8X")
            {
               band = CarrierBand::E5ab;
               code = TrackingCode::B2abIQ;
            }
            else if (rincode == "6I")
            {
               band = CarrierBand::B3;
               code = TrackingCode::B3I;
            }
            else if (rincode == "6Q")
            {
               band = CarrierBand::B3;
               code = TrackingCode::B3Q;
            }
            else if (rincode == "6X")
            {
               band = CarrierBand::B3;
               code = TrackingCode::B3IQ;
            }
            else if (rincode == "6A")
            {
               band = CarrierBand::B3;
               code = TrackingCode::B3AIQ;
            }
            break;
         case 'E': // Galileo
            if (rincode == "1A")
            {
               band = CarrierBand::L1;
               code = TrackingCode::E1A;
            }
            else if (rincode == "1B")
            {
               band = CarrierBand::L1;
               code = TrackingCode::E1B;
            }
            else if (rincode == "1C")
            {
               band = CarrierBand::L1;
               code = TrackingCode::E1C;
            }
            else if (rincode == "1X")
            {
               band = CarrierBand::L1;
               code = TrackingCode::E1BC;
            }
            else if (rincode == "1Z")
            {
               band = CarrierBand::L1;
               code = TrackingCode::E1ABC;
            }
            else if (rincode == "5I")
            {
               band = CarrierBand::L5;
               code = TrackingCode::E5aI;
            }
            else if (rincode == "5Q")
            {
               band = CarrierBand::L5;
               code = TrackingCode::E5aQ;
            }
            else if (rincode == "5X")
            {
               band = CarrierBand::L5;
               code = TrackingCode::E5aIQ;
            }
            else if (rincode == "7I")
            {
               band = CarrierBand::E5b;
               code = TrackingCode::E5bI;
            }
            else if (rincode == "7Q")
            {
               band = CarrierBand::E5b;
               code = TrackingCode::E5bQ;
            }
            else if (rincode == "7X")
            {
               band = CarrierBand::E5b;
               code = TrackingCode::E5bIQ;
            }
            else if (rincode == "8I")
            {
               band = CarrierBand::E5ab;
               code = TrackingCode::E5abI;
            }
            else if (rincode == "8Q")
            {
               band = CarrierBand::E5ab;
               code = TrackingCode::E5abQ;
            }
            else if (rincode == "8X")
            {
               band = CarrierBand::E5ab;
               code = TrackingCode::E5abIQ;
            }
            else if (rincode == "6A")
            {
               band = CarrierBand::E6;
               code = TrackingCode::E6A;
            }
            else if (rincode == "6B")
            {
               band = CarrierBand::E6;
               code = TrackingCode::E6B;
            }
            else if (rincode == "6C")
            {
               band = CarrierBand::E6;
               code = TrackingCode::E6C;
            }
            else if (rincode == "6X")
            {
               band = CarrierBand::E6;
               code = TrackingCode::E6BC;
            }
            else if (rincode == "6Z")
            {
               band = CarrierBand::E6;
               code = TrackingCode::E6ABC;
            }
            break;
         case 'R': // GLONASS
            if (rincode == "1C")
            {
               band = CarrierBand::G1;
               code = TrackingCode::Standard;
            }
            else if (rincode == "1P")
            {
               band = CarrierBand::G1;
               code = TrackingCode::Precise;
            }
            else if (rincode == "4A")
            {
               band = CarrierBand::G1a;
               code = TrackingCode::L1OCD;
            }
            else if (rincode == "4B")
            {
               band = CarrierBand::G1a;
               code = TrackingCode::L1OCP;
            }
            else if (rincode == "4X")
            {
               band = CarrierBand::G1a;
               code = TrackingCode::L1OCDP;
            }
            else if (rincode == "2C")
            {
               band = CarrierBand::G2;
               code = TrackingCode::Standard;
            }
            else if (rincode == "2P")
            {
               band = CarrierBand::G2;
               code = TrackingCode::Precise;
            }
            else if (rincode == "6A")
            {
               band = CarrierBand::G2a;
               code = TrackingCode::L2CSI;
            }
            else if (rincode == "6B")
            {
               band = CarrierBand::G2a;
               code = TrackingCode::L2OCP;
            }
            else if (rincode == "6X")
            {
               band = CarrierBand::G2a;
               code = TrackingCode::L2CSIL2OCp;
            }
            else if (rincode == "3I")
            {
               band = CarrierBand::G3;
               code = TrackingCode::L3OCD;
            }
            else if (rincode == "3Q")
            {
               band = CarrierBand::G3;
               code = TrackingCode::L3OCP;
            }
            else if (rincode == "3X")
            {
               band = CarrierBand::G3;
               code = TrackingCode::L3OCDP;
            }
            break;
         case 'G': // GPS
            if (rincode == "1C")
            {
               band = CarrierBand::L1;
               code = TrackingCode::CA;
            }
            else if (rincode == "1S")
            {
               band = CarrierBand::L1;
               code = TrackingCode::L1CD;
            }
            else if (rincode == "1L")
            {
               band = CarrierBand::L1;
               code = TrackingCode::L1CP;
            }
            else if (rincode == "1X")
            {
               band = CarrierBand::L1;
               code = TrackingCode::L1CDP;
            }
            else if (rincode == "1P")
            {
               band = CarrierBand::L1;
               code = TrackingCode::P;
            }
            else if (rincode == "1W")
            {
               band = CarrierBand::L1;
               code = TrackingCode::Ztracking;
            }
            else if (rincode == "1Y")
            {
               band = CarrierBand::L1;
               code = TrackingCode::Y;
            }
            else if (rincode == "1M")
            {
               band = CarrierBand::L1;
               code = TrackingCode::MDP;
            }
            else if (rincode == "1N")
            {
               band = CarrierBand::L1;
               code = TrackingCode::YCodeless;
            }
            else if (rincode == "2C")
            {
               band = CarrierBand::L2;
               code = TrackingCode::CA;
            }
            else if (rincode == "2D")
            {
               band = CarrierBand::L2;
               code = TrackingCode::Semicodeless;
            }
            else if (rincode == "2S")
            {
               band = CarrierBand::L2;
               code = TrackingCode::L2CM;
            }
            else if (rincode == "2L")
            {
               band = CarrierBand::L2;
               code = TrackingCode::L2CL;
            }
            else if (rincode == "2X")
            {
               band = CarrierBand::L2;
               code = TrackingCode::L2CML;
            }
            else if (rincode == "2P")
            {
               band = CarrierBand::L2;
               code = TrackingCode::P;
            }
            else if (rincode == "2W")
            {
               band = CarrierBand::L2;
               code = TrackingCode::Ztracking;
            }
            else if (rincode == "2Y")
            {
               band = CarrierBand::L2;
               code = TrackingCode::Y;
            }
            else if (rincode == "2M")
            {
               band = CarrierBand::L2;
               code = TrackingCode::MDP;
            }
            else if (rincode == "2N")
            {
               band = CarrierBand::L2;
               code = TrackingCode::YCodeless;
            }
            else if (rincode == "5I")
            {
               band = CarrierBand::L5;
               code = TrackingCode::L5I;
            }
            else if (rincode == "5Q")
            {
               band = CarrierBand::L5;
               code = TrackingCode::L5Q;
            }
            else if (rincode == "5X")
            {
               band = CarrierBand::L5;
               code = TrackingCode::L5IQ;
            }
            break;
         case 'I': // NavIC
            if (rincode == "5A")
            {
               band = CarrierBand::L5;
               code = TrackingCode::SPSL5;
            }
            else if (rincode == "5B")
            {
               band = CarrierBand::L5;
               code = TrackingCode::RSL5D;
            }
            else if (rincode == "5C")
            {
               band = CarrierBand::L5;
               code = TrackingCode::RSL5P;
            }
            else if (rincode == "5X")
            {
               band = CarrierBand::L5;
               code = TrackingCode::RSL5DP;
            }
            else if (rincode == "9A")
            {
               band = CarrierBand::I9;
               code = TrackingCode::SPSS;
            }
            else if (rincode == "9B")
            {
               band = CarrierBand::I9;
               code = TrackingCode::RSSD;
            }
            else if (rincode == "9C")
            {
               band = CarrierBand::I9;
               code = TrackingCode::RSSP;
            }
            else if (rincode == "9X")
            {
               band = CarrierBand::I9;
               code = TrackingCode::RSSDP;
            }
            break;
         case 'J': // QZSS
            if (rincode == "1C")
            {
               band = CarrierBand::L1;
               code = TrackingCode::CA;
            }
            else if (rincode == "1L")
            {
               band = CarrierBand::L1;
               code = TrackingCode::L1CP;
            }
            else if (rincode == "1S")
            {
               band = CarrierBand::L1;
               code = TrackingCode::L1CD;
            }
            else if (rincode == "1X")
            {
               band = CarrierBand::L1;
               code = TrackingCode::L1CDP;
            }
            else if (rincode == "1Z")
            {
               band = CarrierBand::L1;
               code = TrackingCode::L1S;
            }
            else if (rincode == "2S")
            {
               band = CarrierBand::L2;
               code = TrackingCode::L2CM;
            }
            else if (rincode == "2L")
            {
               band = CarrierBand::L2;
               code = TrackingCode::L2CL;
            }
            else if (rincode == "2X")
            {
               band = CarrierBand::L2;
               code = TrackingCode::L2CML;
            }
            else if (rincode == "5I")
            {
               band = CarrierBand::L5;
               code = TrackingCode::L5I;
            }
            else if (rincode == "5Q")
            {
               band = CarrierBand::L5;
               code = TrackingCode::L5Q;
            }
            else if (rincode == "5X")
            {
               band = CarrierBand::L5;
               code = TrackingCode::L5IQ;
            }
            else if (rincode == "5D")
            {
               band = CarrierBand::L5;
               code = TrackingCode::L5SI;
            }
            else if (rincode == "5P")
            {
               band = CarrierBand::L5;
               code = TrackingCode::L5SQ;
            }
            else if (rincode == "5Z")
            {
               band = CarrierBand::L5;
               code = TrackingCode::L5SIQ;
            }
            else if (rincode == "6S")
            {
               band = CarrierBand::E6;
               code = TrackingCode::LEXS;
            }
            else if (rincode == "6S")
            {
               band = CarrierBand::E6;
               code = TrackingCode::L6D;
            }
            else if (rincode == "6L")
            {
               band = CarrierBand::E6;
               code = TrackingCode::LEXL;
            }
            else if (rincode == "6X")
            {
               band = CarrierBand::E6;
               code = TrackingCode::LEXSL;
            }
            else if (rincode == "6E")
            {
               band = CarrierBand::E6;
               code = TrackingCode::L6E;
            }
            else if (rincode == "6Z")
            {
               band = CarrierBand::E6;
               code = TrackingCode::L6DE;
            }
            break;
         case 'S': // SBAS
            if (rincode == "1C")
            {
               band = CarrierBand::L1;
               code = TrackingCode::CA;
            }
            else if (rincode == "5I")
            {
               band = CarrierBand::L5;
               code = TrackingCode::L5I;
            }
            else if (rincode == "5Q")
            {
               band = CarrierBand::L5;
               code = TrackingCode::L5Q;
            }
            else if (rincode == "5X")
            {
               band = CarrierBand::L5;
               code = TrackingCode::L5IQ;
            }
            break;
      } // switch (modStrID[0])
   } // RinexObsID(const std::string& strID, double version)
} // namespace gpstk

