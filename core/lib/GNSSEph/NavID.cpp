//============================================================================
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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

#include "NavID.hpp"


/**
 * @file NavID.cpp
 * gpstk::NavID - navigation message-independent representation of a satellite.
 */

namespace gpstk
{
      const std::string NavID::NavTypeStrings[] = {
         "GPS_LNAV",
         "GPS_L2_CNAV",
         "GPS_L5_CNAV",
         "GPS_MNAV",
         "Beidou_D1",
         "Beidou_D2",
         "GloCivilF",
         "GloCivilC",
         "GalOS",
         "Unknown"
      };
         /// explicit constructor, no defaults
      NavID::NavID( const SatID& sidr, const ObsID& oidr )
      {     //If SatID (sat system type) corresponds to GPS AND ObsID
            //(carrier band) corresponds to either L1 OR L2 AND ObsID
            //(tracking code) matches CA, P, Y, W, N, OR D then NavID 
            //corresponds to GPS LNAV.
         if ( sidr.system==SatID::systemGPS && 
            ( oidr.band==ObsID::cbL1 || oidr.band==ObsID::cbL2 ) &&
            ( oidr.code==ObsID::tcCA || oidr.code==ObsID::tcP ||
              oidr.code==ObsID::tcY  || oidr.code==ObsID::tcY ||
              oidr.code==ObsID::tcW  || oidr.code==ObsID::tcN ||
              oidr.code==ObsID::tcD  )) navType = ntGPSLNAV;
              
         else if ( sidr.system==SatID::systemGPS && oidr.band==ObsID::cbL2 &&
                 ( oidr.code==ObsID::tcC2M  || oidr.code==ObsID::tcC2L ||
                   oidr.code==ObsID::tcC2LM )) navType = ntGPSCNAVL2;
              
         else if ( sidr.system==SatID::systemGPS && oidr.band==ObsID::cbL5 &&             
                 ( oidr.code==ObsID::tcI5 || oidr.code==ObsID::tcQ5 ||                
                   oidr.code==ObsID::tcIQ5 )) navType = ntGPSCNAVL5;
              
         else if ( sidr.system==SatID::systemGPS && oidr.band==ObsID::cbL2 &&
                   oidr.code==ObsID::tcM ) navType = ntGPSMNAV;
              
         else if ( sidr.system==SatID::systemBeiDou && sidr.id>5 &&                                                  
                 ( oidr.band==ObsID::cbB1   || oidr.band==ObsID::cbB2   ||
                   oidr.band==ObsID::cbB3 )&&
                 ( oidr.code==ObsID::tcCI1  || oidr.code==ObsID::tcCQ1  ||                
                   oidr.code==ObsID::tcCIQ1 || oidr.code==ObsID::tcCI7  ||
                   oidr.code==ObsID::tcCQ7  || oidr.code==ObsID::tcCIQ7 ||
                   oidr.code==ObsID::tcCI6  || oidr.code==ObsID::tcCQ6  ||
                   oidr.code==ObsID::tcCIQ6 )) navType = ntBeiDou_D1; 
         
         else if ( sidr.system==SatID::systemBeiDou && sidr.id<=5 &&                                                  
                 ( oidr.band==ObsID::cbB1   || oidr.band==ObsID::cbB2   ||
                   oidr.band==ObsID::cbB3 )&&
                 ( oidr.code==ObsID::tcCI1  || oidr.code==ObsID::tcCQ1  ||                
                   oidr.code==ObsID::tcCIQ1 || oidr.code==ObsID::tcCI7  ||
                   oidr.code==ObsID::tcCQ7  || oidr.code==ObsID::tcCIQ7 ||
                   oidr.code==ObsID::tcCI6  || oidr.code==ObsID::tcCQ6  ||
                   oidr.code==ObsID::tcCIQ6 )) navType = ntBeiDou_D2;
         
         else if ( sidr.system==SatID::systemGlonass &&                   
                 ( oidr.band==ObsID::cbG1 || oidr.band==ObsID::cbG2 ) &&         
                 ( oidr.code==ObsID::tcGCA ))  navType = ntGloCivilF;
             
         else if ( sidr.system==SatID::systemGlonass && oidr.band==ObsID::cbG3 &&
                 ( oidr.code==ObsID::tcIR3 || oidr.code==ObsID::tcQR3 ||
                   oidr.code==ObsID::tcIQR3 )) navType = ntGloCivilC;
              
         else if ( sidr.system==SatID::systemGalileo &&                   
                 ( oidr.band==ObsID::cbL1  || oidr.band==ObsID::cbL5  ||
                   oidr.band==ObsID::cbE5b || oidr.band==ObsID::cbE5ab ) &&
                 ( oidr.code>=ObsID::tcB   || oidr.code==ObsID::tcBC  ||
                   oidr.code==ObsID::tcABC || oidr.code==ObsID::tcIE5 ||
                   oidr.code==ObsID::tcQE5 || oidr.code<=ObsID::tcIQE5 )) 
                   navType = ntGalOS; 
               
         else navType = ntUnknown;
         
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
            navType = ntGPSMNAV;
            
         else if ( s.compare( NavTypeStrings[4] ) == 0 )
            navType = ntBeiDou_D1;
            
         else if ( s.compare( NavTypeStrings[5] ) == 0 )
            navType = ntBeiDou_D2;
            
         else if ( s.compare( NavTypeStrings[6] ) == 0 )
            navType = ntGloCivilF;
            
         else if ( s.compare( NavTypeStrings[7] ) == 0 )
            navType = ntGloCivilC;
            
         else if ( s.compare( NavTypeStrings[8] ) == 0 )
            navType = ntGalOS;
            
         else navType = ntUnknown;
      }
} // namespace gpstk

