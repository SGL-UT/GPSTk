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

#include "NavID.hpp"


/**
 * @file NavID.cpp
 * gpstk::NavID - navigation message-independent representation of a satellite.
 */

namespace gpstk
{
      /// explicit constructor, no defaults
   NavID::NavID( const SatID& sidr, const ObsID& oidr )
   {
         // Default case    
      navType = NavType::Unknown; 
          
         //If SatID (sat system type) corresponds to GPS AND ObsID
         //(carrier band) corresponds to either L1 OR L2 AND ObsID
         //(tracking code) matches CA, P, Y, W, N, OR D then NavID 
         //corresponds to GPS LNAV.
      switch (sidr.system)
      {
         case SatelliteSystem::GPS:
         {
            if (( oidr.band==CarrierBand::L1 || oidr.band==CarrierBand::L2 ) &&
                ( oidr.code==TrackingCode::CA || oidr.code==TrackingCode::P ||
                  oidr.code==TrackingCode::Y  || oidr.code==TrackingCode::Y ||
                  oidr.code==TrackingCode::Ztracking  || oidr.code==TrackingCode::YCodeless ||
                  oidr.code==TrackingCode::Semicodeless  ))
            {
               navType = NavType::GPSLNAV;
            }
            else if ( oidr.band==CarrierBand::L2 &&
                      (oidr.code==TrackingCode::L2CM || 
                       oidr.code==TrackingCode::L2CL ||
                       oidr.code==TrackingCode::L2CML )) 
            {
               navType = NavType::GPSCNAVL2;
            }
            else if ( oidr.band==CarrierBand::L5 &&             
                      (oidr.code==TrackingCode::L5I || 
                       oidr.code==TrackingCode::L5Q ||                
                       oidr.code==TrackingCode::L5IQ ))
            {
               navType = NavType::GPSCNAVL5;
            }
            else if ( oidr.band==CarrierBand::L1 &&
                      ( oidr.code==TrackingCode::L1CP ||
                        oidr.code==TrackingCode::L1CD ||
                        oidr.code==TrackingCode::L1CDP ) )
            {
               navType = NavType::GPSCNAV2;
            }
            else if ((oidr.band==CarrierBand::L1 ||
                      oidr.band==CarrierBand::L2) &&
                     oidr.code==TrackingCode::MDP )
            {
               navType = NavType::GPSMNAV;
            }
            break;
         }
         case SatelliteSystem::QZSS:
         {
            if ( oidr.band==CarrierBand::L1  &&
                 oidr.code==TrackingCode::CA )
            {
               navType = NavType::GPSLNAV;
            }
            else if ( oidr.band==CarrierBand::L2 &&
                      ( oidr.code==TrackingCode::L2CM  ||
                        oidr.code==TrackingCode::L2CL ||
                        oidr.code==TrackingCode::L2CML ))
            {
               navType = NavType::GPSCNAVL2;
            }
            else if ( oidr.band==CarrierBand::L5 &&             
                      ( oidr.code==TrackingCode::L5I ||
                        oidr.code==TrackingCode::L5Q ||                
                        oidr.code==TrackingCode::L5IQ ))
            {
               navType = NavType::GPSCNAVL5;
            }
            else if ( oidr.band==CarrierBand::L1 &&
                      ( oidr.code==TrackingCode::L1CP ||
                        oidr.code==TrackingCode::L1CD ||
                        oidr.code==TrackingCode::L1CDP ))
            {
               navType = NavType::GPSCNAV2;
            }
            break;
         }
         case SatelliteSystem::BeiDou:
         {             
            if ( sidr.id>5 &&                                                  
                 ( oidr.band==CarrierBand::B1   ||
                   oidr.band==CarrierBand::B2   ||
                   oidr.band==CarrierBand::B3 ) &&
                 ( oidr.code==TrackingCode::B1I  ||
                   oidr.code==TrackingCode::B1Q  ||                
                   oidr.code==TrackingCode::B1IQ ||
                   oidr.code==TrackingCode::B2I  ||
                   oidr.code==TrackingCode::B2Q  ||
                   oidr.code==TrackingCode::B2IQ ||
                   oidr.code==TrackingCode::B3I  ||
                   oidr.code==TrackingCode::B3Q  ||
                   oidr.code==TrackingCode::B3IQ ))
            {
               navType = NavType::BeiDou_D1; 
            }
            else if ( sidr.id<=5 &&                                             
                      ( oidr.band==CarrierBand::B1   ||
                        oidr.band==CarrierBand::B2   ||
                        oidr.band==CarrierBand::B3 )&&
                      ( oidr.code==TrackingCode::B1I  ||
                        oidr.code==TrackingCode::B1Q  ||                
                        oidr.code==TrackingCode::B1IQ ||
                        oidr.code==TrackingCode::B2I  ||
                        oidr.code==TrackingCode::B2Q  ||
                        oidr.code==TrackingCode::B2IQ ||
                        oidr.code==TrackingCode::B3I  ||
                        oidr.code==TrackingCode::B3Q  ||
                        oidr.code==TrackingCode::B3IQ ))
            {
               navType = NavType::BeiDou_D2;
            }
            break;
         }
         case SatelliteSystem::Glonass:
         {
            if (( oidr.band==CarrierBand::G1 ||
                  oidr.band==CarrierBand::G2 ) &&         
                ( oidr.code==TrackingCode::Standard ))
            {
               navType = NavType::GloCivilF;
            }
            else if ( oidr.band==CarrierBand::G3 &&
                      ( oidr.code==TrackingCode::L3OCD ||
                        oidr.code==TrackingCode::L3OCP ||
                        oidr.code==TrackingCode::L3OCDP ))
            {
               navType = NavType::GloCivilC;
            }
            break;
         }
         case SatelliteSystem::Galileo:
         {
            if ( oidr.band==CarrierBand::L1 && oidr.code==TrackingCode::E1B )
            {
               navType = NavType::GalINAV;
            }
            else if ( oidr.band==CarrierBand::E5b  && 
                      ( oidr.code==TrackingCode::E5bI ||
                        oidr.code==TrackingCode::E5bIQ ))
            {
               navType = NavType::GalINAV;
            }
            else if ( oidr.band==CarrierBand::L5 &&      // This is Galileo E5a
                      ( oidr.code==TrackingCode::E5aI ||
                        oidr.code==TrackingCode::E5aIQ ))
            {
               navType = NavType::GalFNAV;
            }
            break;
         }
         case SatelliteSystem::IRNSS:
         {
            if ( oidr.band==CarrierBand::L5 &&
                 ( oidr.code==TrackingCode::SPSL5 ||
                   oidr.code==TrackingCode::RSL5D ||
                   oidr.code==TrackingCode::RSL5P ||
                   oidr.code==TrackingCode::RSL5DP ))
            {
               navType = NavType::IRNSS_SPS;
            }
            break;
         }
         default:
            navType = NavType::Unknown;
            break;
      } // end of switch statement       
   }


   NavID::NavID( const std::string& s )
         : navType(convertStringToNavType(s))
   {
   }
} // namespace gpstk

