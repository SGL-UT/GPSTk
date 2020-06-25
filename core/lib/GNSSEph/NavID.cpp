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
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
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
      const std::string NavID::NavTypeStrings[] = {
         "GPS_LNAV",
         "GPS_CNAV_L2",
         "GPS_CNAV_L5",
         "GPS_CNAV2",
         "GPS_MNAV",
         "Beidou_D1",
         "Beidou_D2",
         "GloCivilF",
         "GloCivilC",
         "GalFNAV",
         "GalINAV",
         "IRNSS_SPS",
         "Unknown"
      };
         /// explicit constructor, no defaults
      NavID::NavID( const SatID& sidr, const ObsID& oidr )
      {
         // Default case    
         navType = ntUnknown; 
          
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
                    oidr.code==TrackingCode::W  || oidr.code==TrackingCode::N ||
                    oidr.code==TrackingCode::D  )) 
                    navType = ntGPSLNAV;
              
              else if ( oidr.band==CarrierBand::L2 &&
                       (oidr.code==TrackingCode::C2M || 
                        oidr.code==TrackingCode::C2L ||
                        oidr.code==TrackingCode::C2LM )) 
                        navType = ntGPSCNAVL2;
              
              else if ( oidr.band==CarrierBand::L5 &&             
                       (oidr.code==TrackingCode::I5 || 
                        oidr.code==TrackingCode::Q5 ||                
                        oidr.code==TrackingCode::IQ5 )) 
                        navType = ntGPSCNAVL5;
 
              else if ( oidr.band==CarrierBand::L1 &&
                      ( oidr.code==TrackingCode::G1P ||
                        oidr.code==TrackingCode::G1D ||
                        oidr.code==TrackingCode::G1X ) )
                        navType = ntGPSCNAV2;

              else if ((oidr.band==CarrierBand::L1 ||
                        oidr.band==CarrierBand::L2) &&
                        oidr.code==TrackingCode::M ) 
                        navType = ntGPSMNAV;
              break;
            }
            case SatelliteSystem::QZSS:
            {
               if ( oidr.band==CarrierBand::L1  &&
                    oidr.code==TrackingCode::CA )
                    navType = ntGPSLNAV;
              
               else if ( oidr.band==CarrierBand::L2 &&
                       ( oidr.code==TrackingCode::C2M  || oidr.code==TrackingCode::C2L ||
                         oidr.code==TrackingCode::C2LM )) 
                         navType = ntGPSCNAVL2;
              
               else if ( oidr.band==CarrierBand::L5 &&             
                       ( oidr.code==TrackingCode::I5 || oidr.code==TrackingCode::Q5 ||                
                         oidr.code==TrackingCode::IQ5 )) 
                         navType = ntGPSCNAVL5;

               else if ( oidr.band==CarrierBand::L1 &&
                       ( oidr.code==TrackingCode::G1P ||
                         oidr.code==TrackingCode::G1D ||
                         oidr.code==TrackingCode::G1X ))
                         navType = ntGPSCNAV2;
               break;
            }
            case SatelliteSystem::BeiDou:
            {             
               if ( sidr.id>5 &&                                                  
                  ( oidr.band==CarrierBand::B1   || oidr.band==CarrierBand::B2   ||
                    oidr.band==CarrierBand::B3 ) &&
                  ( oidr.code==TrackingCode::CI1  || oidr.code==TrackingCode::CQ1  ||                
                    oidr.code==TrackingCode::CIQ1 || oidr.code==TrackingCode::CI7  ||
                    oidr.code==TrackingCode::CQ7  || oidr.code==TrackingCode::CIQ7 ||
                    oidr.code==TrackingCode::CI6  || oidr.code==TrackingCode::CQ6  ||
                    oidr.code==TrackingCode::CIQ6 )) 
                    navType = ntBeiDou_D1; 
         
               else if ( sidr.id<=5 &&                                                  
                       ( oidr.band==CarrierBand::B1   || oidr.band==CarrierBand::B2   ||
                         oidr.band==CarrierBand::B3 )&&
                       ( oidr.code==TrackingCode::CI1  || oidr.code==TrackingCode::CQ1  ||                
                         oidr.code==TrackingCode::CIQ1 || oidr.code==TrackingCode::CI7  ||
                         oidr.code==TrackingCode::CQ7  || oidr.code==TrackingCode::CIQ7 ||
                         oidr.code==TrackingCode::CI6  || oidr.code==TrackingCode::CQ6  ||
                         oidr.code==TrackingCode::CIQ6 )) 
                         navType = ntBeiDou_D2;
               break;
            }
            case SatelliteSystem::Glonass:
            {
               if (( oidr.band==CarrierBand::G1 || oidr.band==CarrierBand::G2 ) &&         
                   ( oidr.code==TrackingCode::GCA ))  
                   navType = ntGloCivilF;
             
               else if ( oidr.band==CarrierBand::G3 &&
                       ( oidr.code==TrackingCode::IR3 || oidr.code==TrackingCode::QR3 ||
                         oidr.code==TrackingCode::IQR3 )) 
                         navType = ntGloCivilC;
               break;
            }
            case SatelliteSystem::Galileo:
            {
               if ( oidr.band==CarrierBand::L1 && oidr.code==TrackingCode::B )
                  navType = ntGalINAV;
               else if ( oidr.band==CarrierBand::E5b  && 
                       ( oidr.code==TrackingCode::IE5b || oidr.code==TrackingCode::IQE5b ))
                       navType = ntGalINAV;
               else if ( oidr.band==CarrierBand::L5 &&            // This is Galileo E5a
                       ( oidr.code==TrackingCode::IE5a || oidr.code==TrackingCode::IQE5a ))
                       navType = ntGalFNAV; 
               break;
            }
            case SatelliteSystem::IRNSS:
            {
               if ( oidr.band==CarrierBand::L5 &&
                  ( oidr.code==TrackingCode::IA5 || oidr.code==TrackingCode::IB5 ||
                    oidr.code==TrackingCode::IC5 || oidr.code==TrackingCode::IX5 ))  
                    navType = ntIRNSS_SPS; 
               break;
            }
            default:
               navType = ntUnknown;  
         } // end of switch statement       
      }
      
      NavID::NavID( const std::string& s )
      {
         if ( s.compare( NavTypeStrings[0] ) == 0 )
            navType = ntGPSLNAV;
         
         else if ( s.compare( NavTypeStrings[1] ) == 0 )
            navType = ntGPSCNAVL2;
            
         else if ( s.compare( NavTypeStrings[2] ) == 0 )
            navType = ntGPSCNAVL5;

         else if ( s.compare( NavTypeStrings[3] ) == 0 )
            navType = ntGPSCNAV2;
                        
         else if ( s.compare( NavTypeStrings[4] ) == 0 )
            navType = ntGPSMNAV;
            
         else if ( s.compare( NavTypeStrings[5] ) == 0 )
            navType = ntBeiDou_D1;
            
         else if ( s.compare( NavTypeStrings[6] ) == 0 )
            navType = ntBeiDou_D2;
            
         else if ( s.compare( NavTypeStrings[7] ) == 0 )
            navType = ntGloCivilF;
            
         else if ( s.compare( NavTypeStrings[8] ) == 0 )
            navType = ntGloCivilC;
            
         else if ( s.compare( NavTypeStrings[9] ) == 0 )
            navType = ntGalFNAV;
            
         else if ( s.compare( NavTypeStrings[10] ) == 0 )
            navType = ntGalINAV;

         else if ( s.compare( NavTypeStrings[11] ) == 0 )
            navType = ntIRNSS_SPS;
            
         else navType = ntUnknown;
      }
} // namespace gpstk

