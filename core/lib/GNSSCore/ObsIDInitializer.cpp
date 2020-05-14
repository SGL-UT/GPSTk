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

/// @file ObsIDInitializer.cpp This class exists to initialize maps for ObsID.
/// It was initally an inner class of ObsID but has been exported
/// so it can be wrapped by SWIG, as SWIG does not support
/// C++ inner classes as of summer 2013.

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
      ObsID::otDesc[ObsID::otDemodStat] = "demodStatus";
      ObsID::otDesc[ObsID::otIono]      = "iono";          //Rinex  
      ObsID::otDesc[ObsID::otSSI]       = "ssi";           //Rinex  
      ObsID::otDesc[ObsID::otLLI]       = "lli";           //Rinex  
      ObsID::otDesc[ObsID::otTrackLen]  = "tlen";          //Rinex  
      ObsID::otDesc[ObsID::otNavMsg]    = "navmsg";        //Rinex
      ObsID::otDesc[ObsID::otRngStdDev] = "rngSigma";
      ObsID::otDesc[ObsID::otPhsStdDev] = "phsSigma";
      ObsID::otDesc[ObsID::otFreqIndx]  = "freqIndx";
      ObsID::otDesc[ObsID::otUndefined] = "undefined";     //Rinex -

      ObsID::cbDesc[ObsID::cbUnknown]   = "UnknownBand";   //Rinex (sp)
      ObsID::cbDesc[ObsID::cbAny]       = "AnyBand";       //Rinex *
      ObsID::cbDesc[ObsID::cbZero]      = "";              //Rinex  
      ObsID::cbDesc[ObsID::cbL1]        = "L1";            //Rinex 1
      ObsID::cbDesc[ObsID::cbL2]        = "L2";            //Rinex 2
      ObsID::cbDesc[ObsID::cbL5]        = "L5";            //Rinex 5
      ObsID::cbDesc[ObsID::cbG1a]       = "G1a";
      ObsID::cbDesc[ObsID::cbG1]        = "G1";            //Rinex 1
      ObsID::cbDesc[ObsID::cbG2a]       = "G2a";
      ObsID::cbDesc[ObsID::cbG2]        = "G2";            //Rinex 2
      ObsID::cbDesc[ObsID::cbG3]        = "G3";            //Rinex 3
      ObsID::cbDesc[ObsID::cbE5b]       = "E5b";           //Rinex 7
      ObsID::cbDesc[ObsID::cbE5ab]      = "E5a+b";         //Rinex 8
      ObsID::cbDesc[ObsID::cbE6]        = "E6";            //Rinex 6
      ObsID::cbDesc[ObsID::cbB1]        = "B1";            //Rinex 1  2 in RINEX 3.0[013]
      ObsID::cbDesc[ObsID::cbB2]        = "B2";            //Rinex 7
      ObsID::cbDesc[ObsID::cbB3]        = "B3";            //Rinex 6
      ObsID::cbDesc[ObsID::cbI9]        = "I9";            //Rinex 9
      ObsID::cbDesc[ObsID::cbL1L2]      = "comboL1L2";     //Rinex 3
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
      ObsID::tcDesc[ObsID::tcG1P]       = "GPSC1P";        //Rinex L    // GPScivil_L1P
      ObsID::tcDesc[ObsID::tcG1D]       = "GPSC1D";        //Rinex S    // GPScivil_L1D
      ObsID::tcDesc[ObsID::tcG1X]       = "GPSC1(D+P)";    //Rinex X    // GPScivil_L1D+P

      ObsID::tcDesc[ObsID::tcGCA]       = "GLOC/A";        //Rinex C    // GLOcivil
      ObsID::tcDesc[ObsID::tcGP]        = "GLOP";          //Rinex P    // GLOprecise
      ObsID::tcDesc[ObsID::tcIR3]       = "GLOIR5";        //Rinex I    // GLO L3 I code
      ObsID::tcDesc[ObsID::tcQR3]       = "GLOQR5";        //Rinex Q    // GLO L3 Q code
      ObsID::tcDesc[ObsID::tcIQR3]      = "GLOI+QR5";      //Rinex X    // GLO L3 I+Q code
      ObsID::tcDesc[ObsID::tcL1OC]      = "GLOL1OC";
      ObsID::tcDesc[ObsID::tcL1OCD]     = "GLOL1OCD";
      ObsID::tcDesc[ObsID::tcL1OCP]     = "GLOL1OCP";
      ObsID::tcDesc[ObsID::tcL2CSI]     = "GLOL2CSI";
      ObsID::tcDesc[ObsID::tcL2CSIOCp]  = "GLOL2CSI+OCp";
      ObsID::tcDesc[ObsID::tcL20CP]     = "GLOL2OCP";

      ObsID::tcDesc[ObsID::tcA]         = "GALA";          //Rinex A    // GAL
      ObsID::tcDesc[ObsID::tcB]         = "GALB";          //Rinex B    // GAL
      ObsID::tcDesc[ObsID::tcC]         = "GALC";          //Rinex C    // GAL
      ObsID::tcDesc[ObsID::tcBC]        = "GALB+C";        //Rinex X    // GAL
      ObsID::tcDesc[ObsID::tcABC]       = "GALA+B+C";      //Rinex Z    // GAL
      ObsID::tcDesc[ObsID::tcIE5]       = "GALI5";         //Rinex I    // GAL
      ObsID::tcDesc[ObsID::tcQE5]       = "GALQ5";         //Rinex Q    // GAL
      ObsID::tcDesc[ObsID::tcIQE5]      = "GALI+Q5";       //Rinex X    // GAL
      ObsID::tcDesc[ObsID::tcIE5a]      = "GALI5a";        //Rinex I    // GAL
      ObsID::tcDesc[ObsID::tcQE5a]      = "GALQ5a";        //Rinex Q    // GAL
      ObsID::tcDesc[ObsID::tcIQE5a]     = "GALI+Q5a";      //Rinex X    // GAL
      ObsID::tcDesc[ObsID::tcIE5b]      = "GALI5b";        //Rinex I    // GAL
      ObsID::tcDesc[ObsID::tcQE5b]      = "GALQ5b";        //Rinex Q    // GAL
      ObsID::tcDesc[ObsID::tcIQE5b]     = "GALI+Q5b";      //Rinex X    // GAL
      ObsID::tcDesc[ObsID::tcA6]        = "GALA6";
      ObsID::tcDesc[ObsID::tcABC6]      = "GALABC6";
      ObsID::tcDesc[ObsID::tcB6]        = "GALB6";
      ObsID::tcDesc[ObsID::tcBC6]       = "GALBC6";
      ObsID::tcDesc[ObsID::tcC6]        = "GALC6";

      ObsID::tcDesc[ObsID::tcSCA]       = "SBASC/A";       //Rinex C    // SBAS civil code
      ObsID::tcDesc[ObsID::tcSI5]       = "SBASI5";        //Rinex I    // SBAS L5 I code
      ObsID::tcDesc[ObsID::tcSQ5]       = "SBASQ5";        //Rinex Q    // SBAS L5 Q code
      ObsID::tcDesc[ObsID::tcSIQ5]      = "SBASI+Q5";      //Rinex X    // SBAS L5 I+Q code

      ObsID::tcDesc[ObsID::tcJCA]       = "QZSSC/A";       //Rinex C    // QZSS L1 civil code
      ObsID::tcDesc[ObsID::tcJD1]       = "QZSSL1C(D)";    //Rinex S    // QZSS L1C(D)
      ObsID::tcDesc[ObsID::tcJP1]       = "QZSSL1C(P)";    //Rinex L    // QZSS L1C(P)
      ObsID::tcDesc[ObsID::tcJX1]       = "QZSSL1C(D+P)";  //Rinex X    // QZSS L1C(D+P)
      ObsID::tcDesc[ObsID::tcJZ1]       = "QZSSL1-SAIF";   //Rinex Z    // QZSS L1-SAIF
      ObsID::tcDesc[ObsID::tcJM2]       = "QZSSL2C(M)";    //Rinex S    // QZSS L2 M code
      ObsID::tcDesc[ObsID::tcJL2]       = "QZSSL2C(L)";    //Rinex L    // QZSS L2 L code
      ObsID::tcDesc[ObsID::tcJX2]       = "QZSSL2C(M+L)";  //Rinex X    // QZSS L2 M+L code
      ObsID::tcDesc[ObsID::tcJI5]       = "QZSSL5I";       //Rinex I    // QZSS L5 I code
      ObsID::tcDesc[ObsID::tcJQ5]       = "QZSSL5Q";       //Rinex Q    // QZSS L5 Q code
      ObsID::tcDesc[ObsID::tcJIQ5]      = "QZSSL5I+Q";     //Rinex X    // QZSS L5 I+Q code
      ObsID::tcDesc[ObsID::tcJI6]       = "QZSSL6I";       //Rinex S    // QZSS LEX(6) I code
      ObsID::tcDesc[ObsID::tcJQ6]       = "QZSSL6Q";       //Rinex L    // QZSS LEX(6) Q code
      ObsID::tcDesc[ObsID::tcJIQ6]      = "QZSSL6I+Q";     //Rinex X    // QZSS LEX(6) I+Q code
      ObsID::tcDesc[ObsID::tcJI5S]      = "QZSSL5SI";
      ObsID::tcDesc[ObsID::tcJIQ5S]     = "QZSSL5SI+Q";
      ObsID::tcDesc[ObsID::tcJQ5S]      = "QZSSL5SQ";
      ObsID::tcDesc[ObsID::tcJD6]       = "QZSSL6D";
      ObsID::tcDesc[ObsID::tcJDE6]      = "QZSSL6D+E";
      ObsID::tcDesc[ObsID::tcJE6]       = "QZSSL6E";

      ObsID::tcDesc[ObsID::tcCI1]       = "BDSIB1";        //Rinex I    // BeiDou L1 I code
      ObsID::tcDesc[ObsID::tcCQ1]       = "BDSQB1";        //Rinex Q    // BeiDou L1 Q code
      ObsID::tcDesc[ObsID::tcCIQ1]      = "BDSI+QB1";      //Rinex X    // BeiDou L1 I+Q code
      ObsID::tcDesc[ObsID::tcCI7]       = "BDSIB2";        //Rinex I    // BeiDou B2 I code
      ObsID::tcDesc[ObsID::tcCQ7]       = "BDSQB2";        //Rinex Q    // BeiDou B2 Q code
      ObsID::tcDesc[ObsID::tcCIQ7]      = "BDSI+QB2";      //Rinex X    // BeiDou B2 I+Q code
      ObsID::tcDesc[ObsID::tcCI6]       = "BDSIB3";        //Rinex I    // BeiDou B3 I code
      ObsID::tcDesc[ObsID::tcCQ6]       = "BDSQB3";        //Rinex Q    // BeiDou B3 Q code
      ObsID::tcDesc[ObsID::tcCIQ6]      = "BDSI+QB3";      //Rinex X    // BeiDou B3 I+Q code
      ObsID::tcDesc[ObsID::tcCA1]       = "BDS1A";
      ObsID::tcDesc[ObsID::tcCCD1]      = "BDS1CD";
      ObsID::tcDesc[ObsID::tcCCDP1]     = "BDS1CDP";
      ObsID::tcDesc[ObsID::tcCCP1]      = "BDS1CP";
      ObsID::tcDesc[ObsID::tcCodelessC] = "BDSCodeless";
      ObsID::tcDesc[ObsID::tcCI2a]      = "BDSIB2a";
      ObsID::tcDesc[ObsID::tcCI2ab]     = "BDSIB2ab";
      ObsID::tcDesc[ObsID::tcCI2b]      = "BDSIB2b";
      ObsID::tcDesc[ObsID::tcCIQ3A]     = "BDSI+Q3A";
      ObsID::tcDesc[ObsID::tcCIQ2a]     = "BDSI+QB2a";
      ObsID::tcDesc[ObsID::tcCIQ2ab]    = "BDSI+QB2ab";
      ObsID::tcDesc[ObsID::tcCIQ2b]     = "BDSI+QB2b";
      ObsID::tcDesc[ObsID::tcCQ2a]      = "BDSQB2a";
      ObsID::tcDesc[ObsID::tcCQ2ab]     = "BDSQB2ab";
      ObsID::tcDesc[ObsID::tcCQ2b]      = "BDSQB2b";

      ObsID::tcDesc[ObsID::tcIA5]       = "IRNSSL5A";      //Rinex A    // IRNSS L5 SPS
      ObsID::tcDesc[ObsID::tcIB5]       = "IRNSSL5B";      //Rinex B    // IRNSS L5 RS(D)
      ObsID::tcDesc[ObsID::tcIC5]       = "IRNSSL5C";      //Rinex C    // IRNSS L5 RS(P)
      ObsID::tcDesc[ObsID::tcIX5]       = "IRNSSL5B+C";    //Rinex X    // IRNSS L5 B+C
      ObsID::tcDesc[ObsID::tcIA9]       = "IRNSSL9A";      //Rinex A    // IRNSS S-band SPS
      ObsID::tcDesc[ObsID::tcIB9]       = "IRNSSL9B";      //Rinex B    // IRNSS S-band RS(D)
      ObsID::tcDesc[ObsID::tcIC9]       = "IRNSSL9C";      //Rinex C    // IRNSS S-band RS(P)
      ObsID::tcDesc[ObsID::tcIX9]       = "IRNSSL9B+C";    //Rinex X    // IRNSS S-band B+C

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
      ObsID::char2ot['I'] = ObsID::otIono;
      ObsID::char2ot['S'] = ObsID::otSNR;
      ObsID::char2ot['X'] = ObsID::otChannel;
      ObsID::char2ot['-'] = ObsID::otUndefined;

      ObsID::char2cb[' '] = ObsID::cbUnknown;
      ObsID::char2cb['*'] = ObsID::cbAny;
      ObsID::char2cb['1'] = ObsID::cbL1;
      ObsID::char2cb['2'] = ObsID::cbL2;
      ObsID::char2cb['3'] = ObsID::cbG3;
      ObsID::char2cb['4'] = ObsID::cbG1a;
      ObsID::char2cb['5'] = ObsID::cbL5;
      ObsID::char2cb['6'] = ObsID::cbE6;
      ObsID::char2cb['7'] = ObsID::cbE5b;
      ObsID::char2cb['8'] = ObsID::cbE5ab;
      ObsID::char2cb['9'] = ObsID::cbI9;
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
      ObsID::char2tc['E'] = ObsID::tcJE6;
      ObsID::char2tc['-'] = ObsID::tcUndefined;

      // Since some of the items in the enums don't have corresponding RINEX
      // definitions, make sure there is an entry for all values
      for(int i=ObsID::otUnknown; i<ObsID::otLast; i++)
         ObsID::ot2char[(ObsID::ObservationType)i] = ' ';
      for(int i=ObsID::cbUnknown; i<ObsID::cbLast; i++)
         ObsID::cb2char[(ObsID::CarrierBand)i] = ' ';
      for(int i=ObsID::tcUnknown; i<ObsID::tcLast; i++)
         ObsID::tc2char[(ObsID::TrackingCode)i] = ' ';

      // Here the above three maps are reversed to speed up the runtime
      for(std::map<char, ObsID::ObservationType>::const_iterator i=ObsID::char2ot.begin();
           i != ObsID::char2ot.end(); i++)
         ObsID::ot2char[i->second] = i->first;

      for(std::map<char, ObsID::CarrierBand>::const_iterator i=ObsID::char2cb.begin();
           i != ObsID::char2cb.end(); i++)
         ObsID::cb2char[i->second] = i->first;

      for(std::map<char, ObsID::TrackingCode>::const_iterator i=ObsID::char2tc.begin();
           i != ObsID::char2tc.end(); i++)
         ObsID::tc2char[i->second] = i->first;

      // And add the couple 'special' cases
      ObsID::cb2char[ObsID::cbG1] = '1';
      ObsID::cb2char[ObsID::cbG2] = '2';
      ObsID::cb2char[ObsID::cbB1] = '2';
      ObsID::cb2char[ObsID::cbB2] = '7';
      ObsID::cb2char[ObsID::cbB3] = '6';

      ObsID::tc2char[ObsID::tcG1D]  = 'S';
      ObsID::tc2char[ObsID::tcG1P]  = 'L';
      ObsID::tc2char[ObsID::tcG1X]  = 'X';
      ObsID::tc2char[ObsID::tcC]    = 'C';
      ObsID::tc2char[ObsID::tcGCA]  = 'C';
      ObsID::tc2char[ObsID::tcGP]   = 'P';
      ObsID::tc2char[ObsID::tcIR3]  = 'I';
      ObsID::tc2char[ObsID::tcQR3]  = 'Q';
      ObsID::tc2char[ObsID::tcIQR3] = 'X';
      ObsID::tc2char[ObsID::tcIE5]  = 'I';
      ObsID::tc2char[ObsID::tcQE5]  = 'Q';
      ObsID::tc2char[ObsID::tcIQE5] = 'X';
      ObsID::tc2char[ObsID::tcIE5a]  = 'I';
      ObsID::tc2char[ObsID::tcQE5a]  = 'Q';
      ObsID::tc2char[ObsID::tcIQE5a] = 'X';
      ObsID::tc2char[ObsID::tcIE5b]  = 'I';
      ObsID::tc2char[ObsID::tcQE5b]  = 'Q';
      ObsID::tc2char[ObsID::tcIQE5b] = 'X';
      ObsID::tc2char[ObsID::tcIQ5]  = 'X';
      ObsID::tc2char[ObsID::tcBC]   = 'X';
      ObsID::tc2char[ObsID::tcSCA]  = 'C';
      ObsID::tc2char[ObsID::tcSI5]  = 'I';
      ObsID::tc2char[ObsID::tcSQ5]  = 'Q';
      ObsID::tc2char[ObsID::tcSIQ5] = 'X';
      ObsID::tc2char[ObsID::tcCI1]  = 'I';
      ObsID::tc2char[ObsID::tcCQ1]  = 'Q';
      ObsID::tc2char[ObsID::tcCIQ1] = 'X';
      ObsID::tc2char[ObsID::tcCI7]  = 'I';
      ObsID::tc2char[ObsID::tcCQ7]  = 'Q';
      ObsID::tc2char[ObsID::tcCIQ7] = 'X';
      ObsID::tc2char[ObsID::tcCI6]  = 'I';
      ObsID::tc2char[ObsID::tcCQ6]  = 'Q';
      ObsID::tc2char[ObsID::tcCIQ6] = 'X';
      ObsID::tc2char[ObsID::tcJCA]  = 'C';
      ObsID::tc2char[ObsID::tcJD1]  = 'S';
      ObsID::tc2char[ObsID::tcJP1]  = 'L';
      ObsID::tc2char[ObsID::tcJX1]  = 'X';
      ObsID::tc2char[ObsID::tcJZ1]  = 'Z';
      ObsID::tc2char[ObsID::tcJM2]  = 'S';
      ObsID::tc2char[ObsID::tcJL2]  = 'L';
      ObsID::tc2char[ObsID::tcJX2]  = 'X';
      ObsID::tc2char[ObsID::tcJI5]  = 'I';
      ObsID::tc2char[ObsID::tcJQ5]  = 'Q';
      ObsID::tc2char[ObsID::tcJIQ5] = 'X';
      ObsID::tc2char[ObsID::tcJI6]  = 'S';
      ObsID::tc2char[ObsID::tcJQ6]  = 'L';
      ObsID::tc2char[ObsID::tcJIQ6] = 'X';
      ObsID::tc2char[ObsID::tcIA5]  = 'A';
      ObsID::tc2char[ObsID::tcIB5]  = 'B';
      ObsID::tc2char[ObsID::tcIC5]  = 'C';
      ObsID::tc2char[ObsID::tcIX5]  = 'X';
      ObsID::tc2char[ObsID::tcIA9]  = 'A';
      ObsID::tc2char[ObsID::tcIB9]  = 'B';
      ObsID::tc2char[ObsID::tcIC9]  = 'C';
      ObsID::tc2char[ObsID::tcIX9]  = 'X';

      ObsID::validRinexSystems = "GRESCJI";

      ObsID::map1to3sys["G"] = "GPS";
      ObsID::map1to3sys["R"] = "GLO";
      ObsID::map1to3sys["E"] = "GAL";
      ObsID::map1to3sys["S"] = "GEO";
      ObsID::map1to3sys["C"] = "BDS";
      ObsID::map1to3sys["J"] = "QZS";
      ObsID::map1to3sys["I"] = "IRN";

      ObsID::map3to1sys["GPS"] = "G";
      ObsID::map3to1sys["GLO"] = "R";
      ObsID::map3to1sys["GAL"] = "E";
      ObsID::map3to1sys["GEO"] = "S";
      ObsID::map3to1sys["BDS"] = "C";
      ObsID::map3to1sys["QZS"] = "J";
      ObsID::map3to1sys["IRN"] = "I";

      ObsID::validRinexFrequencies = "12356789";

      // this defines the valid obs types
      // NB these tc characters are ORDERED ~best to worst
         // except no C1N
      ObsID::validRinexTrackingCodes['G']['1'] = "PYWLMCSXN* ";
         // except no C2N
      ObsID::validRinexTrackingCodes['G']['2'] = "PYWLMCSXDN* ";
      ObsID::validRinexTrackingCodes['G']['5'] = "IQX* ";

      ObsID::validRinexTrackingCodes['R']['1'] = "PC* ";
      ObsID::validRinexTrackingCodes['R']['2'] = "PC* ";
      ObsID::validRinexTrackingCodes['R']['3'] = "IQX* ";
      ObsID::validRinexTrackingCodes['R']['4'] = "ABX* ";
      ObsID::validRinexTrackingCodes['R']['6'] = "ABX* ";

      ObsID::validRinexTrackingCodes['E']['1'] = "ABCXZ* ";
      ObsID::validRinexTrackingCodes['E']['5'] = "IQX* ";
      ObsID::validRinexTrackingCodes['E']['6'] = "ABCXZ* ";
      ObsID::validRinexTrackingCodes['E']['7'] = "IQX* ";
      ObsID::validRinexTrackingCodes['E']['8'] = "IQX* ";

      ObsID::validRinexTrackingCodes['S']['1'] = "C* ";
      ObsID::validRinexTrackingCodes['S']['5'] = "IQX* ";

         /** @note 3.02 uses carrier "1" for "B1-2", every other RINEX
          * version uses carrier "2", so we have to leave IQ in C1 */
      ObsID::validRinexTrackingCodes['C']['1'] = "PAIQXDN* ";
      ObsID::validRinexTrackingCodes['C']['2'] = "IQX* ";
      ObsID::validRinexTrackingCodes['C']['5'] = "PXD* ";
      ObsID::validRinexTrackingCodes['C']['6'] = "AIQX* ";
      ObsID::validRinexTrackingCodes['C']['7'] = "ZPIQXD* ";
      ObsID::validRinexTrackingCodes['C']['8'] = "PXD* ";

      ObsID::validRinexTrackingCodes['J']['1'] = "CSLXZ* ";
      ObsID::validRinexTrackingCodes['J']['2'] = "SLX* ";
      ObsID::validRinexTrackingCodes['J']['5'] = "IQDPXZ* ";
      ObsID::validRinexTrackingCodes['J']['6'] = "ESLXZ* ";

         // I1 is *only* for channel number
      ObsID::validRinexTrackingCodes['I']['1'] = "* ";
      ObsID::validRinexTrackingCodes['I']['5'] = "ABCX* ";
      ObsID::validRinexTrackingCodes['I']['9'] = "ABCX* ";
   }


}
