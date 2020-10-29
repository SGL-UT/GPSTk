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

#ifndef GPSTK_TRACKINGCODE_HPP
#define GPSTK_TRACKINGCODE_HPP

#include <string>
#include "EnumIterator.hpp"

namespace gpstk
{
      /** The code used to collect the observation. Each of these
       * should uniquely identify a code that was correlated
       * against to track the signal. While the notation generally
       * follows section 5.1 of RINEX 3, due to ambiguities in that
       * specification some extensions are made. Note that as
       * concrete specifications for the codes are released, this
       * list may need to be adjusted. Specifically, this lists
       * assumes that the same I & Q codes will be used on all
       * three of the Galileo carriers. If that is not true, more
       * identifiers need to be allocated */
   enum class TrackingCode
   {
      Unknown,      ///< Uninitialized value
      Any,          ///< Used to match any tracking code
      CA,           ///< Legacy GPS civil code
      P,            ///< Legacy GPS precise code
      Y,            ///< Encrypted legacy GPS precise code
      Ztracking,    ///< Encrypted legacy GPS precise code, codeless Z tracking
      YCodeless,    ///< Encrypted legacy GPS precise code, squaring codeless tracking
      Semicodeless, ///< Encrypted legacy GPS precise code, other codeless tracking
      MD,
      MDP,          ///< Modernized GPS military unique code
      MP,
      MPA,
      MARL,
      MARLD,
      MARLP,
      Mprime,
      MprimePA,
      L2CM,         ///< Modernized GPS L2 civil M code
      L2CL,         ///< Modernized GPS L2 civil L code
      L2CML,        ///< Modernized GPS L2 civil M+L combined tracking
      L5I,          ///< Modernized GPS L5 civil in-phase
      L5Q,          ///< Modernized GPS L5 civil quadrature
      L5IQ,         ///< Modernized GPS L5 civil I+Q combined tracking
      L1CP,         ///< Modernized GPS L1C civil code tracking (pilot)
      L1CD,         ///< Modernized GPS L1C civil code tracking (data)
      L1CDP,        ///< Modernized GPS L1C civil code tracking (pilot + data)
      NSCA,
      NSCD,
      NSCL,
      NSCM,
      NSCMCL,
      NSCP,
      NSCPCD,
      NSI5,
      NSM,
      NSMPA,
      NSMprime,
      NSMprimePA,
      NSP,
      NSQ5,
      NSY,
      Standard,     ///< Legacy Glonass civil signal
      Precise,      ///< Legacy Glonass precise signal
      L3OCD,        ///< Glonass L3 I code
      L3OCP,        ///< Glonass L3 Q code
      L3OCDP,       ///< Glonass L3 I+Q combined tracking
      L1OCD,        ///< GLONASS L1 OCd code
      L1OCP,        ///< GLONASS L1 OCp code
      L1OCDP,       ///< GLONASS L1 OCd+OCp combined tracking
      L2CSIL2OCp,   ///< GLONASS L2 CSI+OCp combined tracking
      L2CSI,        ///< GLONASS L2 CSI code
      L2OCP,        ///< GLONASS L2 OCp code
      G3TestData,
      G3TestPilot,
      L1SC,         ///< GLONASS L1SC in-phase signal
      L2SC,         ///< GLONASS L2SC in-phase signal
      E1A,          ///< Galileo L1 PRS code
      E1B,          ///< Galileo E1-B signal, supporting OS/HAS/SoL
      E1C,          ///< Galileo E1 Dataless code
      E1BC,         ///< Galileo E1 B+C combined tracking
      E1ABC,        ///< Galileo E1 A+B+C combined tracking
      E5abI,        ///< Galileo E5 I code
      E5abQ,        ///< Galileo E5 Q code
      E5abIQ,       ///< Galileo E5 I+Q combined tracking
      E5aI,         ///< Galileo E5a I code
      E5aQ,         ///< Galileo E5a Q code
      E5aIQ,        ///< Galileo E5a I+Q combined tracking
      E5bI,         ///< Galileo E5b I code
      E5bQ,         ///< Galileo E5b Q code
      E5bIQ,        ///< Galileo E5b I+Q combined tracking
      E6A,          ///< Galileo E6 PRS code
      E6B,          ///< Galileo E6-b signal
      E6C,          ///< Galileo E6 Dataless code
      E6BC,         ///< Galileo E6 B+C combined tracking
      E6ABC,        ///< Galileo E6 A+B+C combined tracking
      L1S,          ///< QZSS L1-SAIF
      L5SI,         ///< QZSS L5S in-phase
      L5SQ,         ///< QZSS L5S I+Q combined tracking
      L5SIQ,        ///< QZSS L5S quadrature
      LEXS,         ///< QZSS LEX(6) short
      LEXL,         ///< QZSS LEX(6) long
      LEXSL,        ///< QZSS LEX(6) combined tracking
      L6D,          ///< QZSS L6 Block II D code
      L6E,          ///< QZSS L6 Block II E code
      L6DE,         ///< QZSS L6 Block II D+E combined tracking
      B1I,          ///< BeiDou B1 I code
      B1Q,          ///< BeiDou B1 Q code
      B1IQ,         ///< BeiDou B1 I+Q code
      B2I,          ///< BeiDou B2 I code
      B2Q,          ///< BeiDou B2 Q code
      B2IQ,         ///< BeiDou B2 I+Q code
      B3I,          ///< BeiDou B3 I code
      B3Q,          ///< BeiDou B3 Q code
      B3IQ,         ///< BeiDou B3 I+Q code
      B1A,          ///< BeiDou B1A code
      B1CD,         ///< BeiDou B1C D code
      B1CDP,        ///< BeiDou B1C D+P code
      B1CP,         ///< BeiDou B1C P code
      B2abI,        ///< BeiDou B2a+b I code
      B2abIQ,       ///< BeiDou B2a+B I+Q code
      B2abQ,        ///< BeiDou B2a+B Q code
      B2aI,         ///< BeiDou B2a I code
      B2aIQ,        ///< BeiDou B2a I+Q code
      B2aQ,         ///< BeiDou B2a Q code
      B2bI,         ///< BeiDou B2b I code
      B2bIQ,        ///< BeiDou B2b I+Q code
      B2bQ,         ///< BeiDou B2b Q code
      B3AQ,         ///< BeiDou B3A Q code
      BCodeless,    ///< BeiDou codeless tracking
      B3AI,         ///< BeiDou B3A I code
      B3AIQ,        ///< BeiDou B3A I+Q code
      SPSL5,        ///< IRNSS L5 SPS
      RSL5D,        ///< IRNSS L5 RS(D)
      RSL5P,        ///< IRNSS L5 RS(P)
      RSL5DP,       ///< IRNSS L5 B+C
      SPSS,         ///< IRNSS S-band SPS
      RSSD,         ///< IRNSS S-band RS(D)
      RSSP,         ///< INRSS S-band RS(P)
      RSSDP,        ///< IRNSS S-band B+C
      CW,           ///< Continuous Wave, i.e. no chipping sequence
      Undefined,    ///< Code is known to be undefined (as opposed to unknown)
      Last,         ///< Used to verify that all items are described at compile time
   }; // enum class TrackingCode

      /** Define an iterator so C++11 can do things like
       * for (TrackingCode i : TrackingCodeIterator()) */
   typedef EnumIterator<TrackingCode, TrackingCode::Unknown, TrackingCode::Last> TrackingCodeIterator;

   namespace StringUtils
   {
         /// Convert a TrackingCode to a whitespace-free string name.
      std::string asString(TrackingCode e) throw();
         /// Convert a string name to an TrackingCode
      TrackingCode asTrackingCode(const std::string& s) throw();
   }
} // namespace gpstk

#endif // GPSTK_TRACKINGCODE_HPP
