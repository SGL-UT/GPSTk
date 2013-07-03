#pragma ident "$Id$"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
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

#include "ObsIDInitializer.hpp"

namespace gpstk {

   ObsIDInitializer::ObsIDInitializer()
   {
      ObsID::otDesc[ObsID::otUnknown]   = "UnknownType";   //Rinex (sp)
      ObsID::otDesc[ObsID::otAny]       = "AnyType";       //Rinex *
      ObsID::otDesc[ObsID::otRange]     = "pseudorange";   //Rinex C
      ObsID::otDesc[ObsID::otPhase]     = "phase";         //Rinex L
      ObsID::otDesc[ObsID::otDoppler]   = "doppler";       //Rinex D
      ObsID::otDesc[ObsID::otSNR]       = "snr";           //Rinex S
      ObsID::otDesc[ObsID::otChannel]   = "channel";       //Rinex  
      ObsID::otDesc[ObsID::otIono]      = "iono";          //Rinex  
      ObsID::otDesc[ObsID::otSSI]       = "ssi";           //Rinex  
      ObsID::otDesc[ObsID::otLLI]       = "lli";           //Rinex  
      ObsID::otDesc[ObsID::otTrackLen]  = "tlen";          //Rinex  
      ObsID::otDesc[ObsID::otNavMsg]    = "navmsg";        //Rinex
      ObsID::otDesc[ObsID::otUndefined] = "undefined";     //Rinex -

      ObsID::cbDesc[ObsID::cbUnknown]   = "UnknownBand";   //Rinex (sp)
      ObsID::cbDesc[ObsID::cbAny]       = "AnyBand";       //Rinex *
      ObsID::cbDesc[ObsID::cbZero]      = "";              //Rinex  
      ObsID::cbDesc[ObsID::cbL1]        = "L1";            //Rinex 1
      ObsID::cbDesc[ObsID::cbL2]        = "L2";            //Rinex 2
      ObsID::cbDesc[ObsID::cbL5]        = "L5";            //Rinex 5
      ObsID::cbDesc[ObsID::cbL1L2]      = "L1+L2";         //Rinex  
      ObsID::cbDesc[ObsID::cbG1]        = "G1";            //Rinex 1
      ObsID::cbDesc[ObsID::cbG2]        = "G2";            //Rinex 2
      ObsID::cbDesc[ObsID::cbE5b]       = "E5b";           //Rinex 7
      ObsID::cbDesc[ObsID::cbE5ab]      = "L5a+b";         //Rinex 8
      ObsID::cbDesc[ObsID::cbE6]        = "E6";            //Rinex 6
      ObsID::cbDesc[ObsID::cbE1]        = "E1";            //Rinex 1
      ObsID::cbDesc[ObsID::cbE2]        = "E2";            //Rinex 2
      ObsID::cbDesc[ObsID::cbC6]        = "C6";            //Rinex 6
      ObsID::cbDesc[ObsID::cbUndefined] = "undefined";     //Rinex -

      ObsID::tcDesc[ObsID::tcUnknown]   = "UnknownCode";   //Rinex (sp)
      ObsID::tcDesc[ObsID::tcAny]       = "AnyCode";       //Rinex *
      ObsID::tcDesc[ObsID::tcCA]        = "GPSC/A";        //Rinex C    // GPScivil
      ObsID::tcDesc[ObsID::tcP]         = "GPSP";          //Rinex P    // GPSprecise
      ObsID::tcDesc[ObsID::tcY]         = "GPSY";          //Rinex Y    // GPSprecise_encrypted
      ObsID::tcDesc[ObsID::tcW]         = "GPScodelessZ";  //Rinex W    // GPSprecise_encrypted_codeless_Z
      ObsID::tcDesc[ObsID::tcN]         = "GPSsquare";     //Rinex N    // GPSprecise_encrypted_codeless_squaring
      ObsID::tcDesc[ObsID::tcD]         = "GPScodeless";   //Rinex D    // GPSprecise_encrypted_codeless_other
      ObsID::tcDesc[ObsID::tcM]         = "GPSM";          //Rinex M    // GPSmilitary
      ObsID::tcDesc[ObsID::tcC2M]       = "GPSC2M";        //Rinex S    // GPScivil_M
      ObsID::tcDesc[ObsID::tcC2L]       = "GPSC2L";        //Rinex L    // GPScivil_L
      ObsID::tcDesc[ObsID::tcC2LM]      = "GPSC2L+M";      //Rinex X    // GPScivil_L+M
      ObsID::tcDesc[ObsID::tcI5]        = "GPSI5";         //Rinex I    // GPScivil_I
      ObsID::tcDesc[ObsID::tcQ5]        = "GPSQ5";         //Rinex Q    // GPScivil_Q
      ObsID::tcDesc[ObsID::tcIQ5]       = "GPSI+Q5";       //Rinex X    // GPScivil_I+Q
      ObsID::tcDesc[ObsID::tcGCA]       = "GLOC/A";        //Rinex C    // GLOcivil
      ObsID::tcDesc[ObsID::tcGP]        = "GLOP";          //Rinex P    // GLOprecise
      ObsID::tcDesc[ObsID::tcA]         = "GALA";          //Rinex A    // GAL
      ObsID::tcDesc[ObsID::tcB]         = "GALB";          //Rinex B    // GAL
      ObsID::tcDesc[ObsID::tcC]         = "GALC";          //Rinex C    // GAL
      ObsID::tcDesc[ObsID::tcBC]        = "GALB+C";        //Rinex X    // GAL
      ObsID::tcDesc[ObsID::tcABC]       = "GALA+B+C";      //Rinex Z    // GAL
      ObsID::tcDesc[ObsID::tcIE5]       = "GALIE5";        //Rinex I    // GAL
      ObsID::tcDesc[ObsID::tcQE5]       = "GALQE5";        //Rinex Q    // GAL
      ObsID::tcDesc[ObsID::tcIQE5]      = "GALI+QE5";      //Rinex X    // GAL
      ObsID::tcDesc[ObsID::tcSCA]       = "SBASC/A";       //Rinex C    // SBAS civil code
      ObsID::tcDesc[ObsID::tcSI5]       = "SBASI5";        //Rinex I    // SBAS L5 I code
      ObsID::tcDesc[ObsID::tcSQ5]       = "SBASQ5";        //Rinex Q    // SBAS L5 Q code
      ObsID::tcDesc[ObsID::tcSIQ5]      = "SBASI+Q5";      //Rinex X    // SBAS L5 I+Q code
      ObsID::tcDesc[ObsID::tcCI2]       = "COMIC2";        //Rinex I    // Compass E2 I code
      ObsID::tcDesc[ObsID::tcCQ2]       = "COMQC2";        //Rinex Q    // Compass E2 Q code
      ObsID::tcDesc[ObsID::tcCIQ2]      = "COMI+QC2";      //Rinex X    // Compass E2 I code
      ObsID::tcDesc[ObsID::tcCI5]       = "COMIC5";        //Rinex I    // Compass E5 I+Q code
      ObsID::tcDesc[ObsID::tcCQ5]       = "COMQC5";        //Rinex Q    // Compass E5 Q code
      ObsID::tcDesc[ObsID::tcCIQ5]      = "COMI+QC5";      //Rinex X    // Compass E5 I+Q code
      ObsID::tcDesc[ObsID::tcCI6]       = "COMIC6";        //Rinex I    // Compass E6 I code
      ObsID::tcDesc[ObsID::tcCQ6]       = "COMQC6";        //Rinex Q    // Compass E6 Q code
      ObsID::tcDesc[ObsID::tcCIQ6]      = "COMI+QC6";      //Rinex X    // Compass E6 I+Q code
      ObsID::tcDesc[ObsID::tcUndefined] = "undefined";

      if (ObsID::otDesc.size() != (int)ObsID::otLast)
         std::cerr << "Error in otDesc" << std::endl;
      if (ObsID::cbDesc.size() != (int)ObsID::cbLast)
         std::cerr << "Error in cbDesc" << std::endl;
      if (ObsID::tcDesc.size() != (int)ObsID::tcLast)
         std::cerr << "Error in tcDesc" << std::endl;

      // The following definitions really should only describe the items that are
      // in the Rinex 3 specification. If an application needs additional ObsID
      // types to be able to be translated to/from Rinex3, the additional types
      // must be added by the application.
      ObsID::char2ot[' '] = ObsID::otUnknown;
      ObsID::char2ot['*'] = ObsID::otAny;
      ObsID::char2ot['C'] = ObsID::otRange;
      ObsID::char2ot['L'] = ObsID::otPhase;
      ObsID::char2ot['D'] = ObsID::otDoppler;
      ObsID::char2ot['S'] = ObsID::otSNR;
      ObsID::char2ot['-'] = ObsID::otUndefined;

      ObsID::char2cb[' '] = ObsID::cbUnknown;
      ObsID::char2cb['*'] = ObsID::cbAny;
      ObsID::char2cb['1'] = ObsID::cbL1;
      ObsID::char2cb['2'] = ObsID::cbL2;
      ObsID::char2cb['5'] = ObsID::cbL5;
      ObsID::char2cb['6'] = ObsID::cbE6;
      ObsID::char2cb['7'] = ObsID::cbE5b;
      ObsID::char2cb['8'] = ObsID::cbE5ab;
      ObsID::char2cb['-'] = ObsID::cbUndefined;

      ObsID::char2tc[' '] = ObsID::tcUnknown;
      ObsID::char2tc['*'] = ObsID::tcAny;
      ObsID::char2tc['C'] = ObsID::tcCA;
      ObsID::char2tc['P'] = ObsID::tcP;
      ObsID::char2tc['W'] = ObsID::tcW;
      ObsID::char2tc['Y'] = ObsID::tcY;
      ObsID::char2tc['M'] = ObsID::tcM;
      ObsID::char2tc['N'] = ObsID::tcN;
      ObsID::char2tc['D'] = ObsID::tcD;
      ObsID::char2tc['S'] = ObsID::tcC2M;
      ObsID::char2tc['L'] = ObsID::tcC2L;
      ObsID::char2tc['X'] = ObsID::tcC2LM;
      ObsID::char2tc['I'] = ObsID::tcI5;
      ObsID::char2tc['Q'] = ObsID::tcQ5;
      ObsID::char2tc['A'] = ObsID::tcA;
      ObsID::char2tc['B'] = ObsID::tcB;
      ObsID::char2tc['Z'] = ObsID::tcABC;
      ObsID::char2tc['-'] = ObsID::tcUndefined;

      // Since some of the items in the enums don't have corresponding RINEX
      // definitions, make sure there is an entry for all values
      for (int i=ObsID::otUnknown; i<ObsID::otLast; i++) ObsID::ot2char[(ObsID::ObservationType)i] = ' ';
      for (int i=ObsID::cbUnknown; i<ObsID::cbLast; i++) ObsID::cb2char[(ObsID::CarrierBand)i] = ' ';
      for (int i=ObsID::tcUnknown; i<ObsID::tcLast; i++) ObsID::tc2char[(ObsID::TrackingCode)i] = ' ';

      // Here the above three maps are reversed to speed up the runtime
      for (std::map< char, ObsID::ObservationType>::const_iterator i=ObsID::char2ot.begin();
           i != ObsID::char2ot.end(); i++)
         ObsID::ot2char[i->second] = i->first;

      for (std::map< char, ObsID::CarrierBand>::const_iterator i=ObsID::char2cb.begin();
           i != ObsID::char2cb.end(); i++)
         ObsID::cb2char[i->second] = i->first;

      for (std::map< char, ObsID::TrackingCode>::const_iterator i=ObsID::char2tc.begin();
           i != ObsID::char2tc.end(); i++)
         ObsID::tc2char[i->second] = i->first;

      // And add the couple 'special' cases
      ObsID::cb2char[ObsID::cbG1] = '1';
      ObsID::cb2char[ObsID::cbG2] = '2';
      ObsID::cb2char[ObsID::cbE1] = '1';
      ObsID::cb2char[ObsID::cbE2] = '2';

      ObsID::tc2char[ObsID::tcC]    = 'C';
      ObsID::tc2char[ObsID::tcGCA]  = 'C';
      ObsID::tc2char[ObsID::tcGP]   = 'P';
      ObsID::tc2char[ObsID::tcIE5]  = 'I';
      ObsID::tc2char[ObsID::tcQE5]  = 'Q';
      ObsID::tc2char[ObsID::tcIQE5] = 'X';
      ObsID::tc2char[ObsID::tcIQ5]  = 'X';
      ObsID::tc2char[ObsID::tcBC]   = 'X';
      ObsID::tc2char[ObsID::tcSCA]  = 'C';
      ObsID::tc2char[ObsID::tcSI5]  = 'I';
      ObsID::tc2char[ObsID::tcSQ5]  = 'Q';
      ObsID::tc2char[ObsID::tcSIQ5] = 'X';
      ObsID::tc2char[ObsID::tcCI2]  = 'I';
      ObsID::tc2char[ObsID::tcCQ2]  = 'Q';
      ObsID::tc2char[ObsID::tcCIQ2] = 'X';
      ObsID::tc2char[ObsID::tcCI5]  = 'I';
      ObsID::tc2char[ObsID::tcCQ5]  = 'Q';
      ObsID::tc2char[ObsID::tcCIQ5] = 'X';
      ObsID::tc2char[ObsID::tcCI6]  = 'I';
      ObsID::tc2char[ObsID::tcCQ6]  = 'Q';
      ObsID::tc2char[ObsID::tcCIQ6] = 'X';

      ObsID::validRinexSystems = "GRESC";

      // NB these tc characters are ORDERED ~best to worst
      ObsID::validRinexTrackingCodes['G']['1'] = "PYWLMIQSXCN* ";      // except no C1N
      ObsID::validRinexTrackingCodes['G']['2'] = "PYWLMIQSXCDN* ";     // except no C2N
      ObsID::validRinexTrackingCodes['G']['5'] = "XIQ* ";
      ObsID::validRinexTrackingCodes['R']['1'] = "PC* ";
      ObsID::validRinexTrackingCodes['R']['2'] = "PC* ";
      ObsID::validRinexTrackingCodes['E']['1'] = "ABCIQXZ* ";
      ObsID::validRinexTrackingCodes['E']['5'] = "XIQ* ";
      ObsID::validRinexTrackingCodes['E']['7'] = "XIQ* ";
      ObsID::validRinexTrackingCodes['E']['8'] = "XIQ* ";
      ObsID::validRinexTrackingCodes['E']['6'] = "ABCIQXZ* ";
      ObsID::validRinexTrackingCodes['S']['1'] = "C* ";
      ObsID::validRinexTrackingCodes['S']['5'] = "XIQ* ";
      ObsID::validRinexTrackingCodes['C']['1'] = "* ";
      ObsID::validRinexTrackingCodes['C']['2'] = "XIQ* ";
      ObsID::validRinexTrackingCodes['C']['7'] = "XIQ* ";
      ObsID::validRinexTrackingCodes['C']['6'] = "XIQ* ";
   }
}