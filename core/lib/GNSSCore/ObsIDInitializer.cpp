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

      ObsID::cbDesc[CarrierBand::Unknown]   = "UnknownBand";   //Rinex (sp)
      ObsID::cbDesc[CarrierBand::Any]       = "AnyBand";       //Rinex *
      ObsID::cbDesc[CarrierBand::L1]        = "L1";            //Rinex 1
      ObsID::cbDesc[CarrierBand::L2]        = "L2";            //Rinex 2
      ObsID::cbDesc[CarrierBand::L5]        = "L5";            //Rinex 5
      ObsID::cbDesc[CarrierBand::G1a]       = "G1a";
      ObsID::cbDesc[CarrierBand::G1]        = "G1";            //Rinex 1
      ObsID::cbDesc[CarrierBand::G2a]       = "G2a";
      ObsID::cbDesc[CarrierBand::G2]        = "G2";            //Rinex 2
      ObsID::cbDesc[CarrierBand::G3]        = "G3";            //Rinex 3
      ObsID::cbDesc[CarrierBand::E5b]       = "E5b";           //Rinex 7
      ObsID::cbDesc[CarrierBand::E5ab]      = "E5a+b";         //Rinex 8
      ObsID::cbDesc[CarrierBand::E6]        = "E6";            //Rinex 6
      ObsID::cbDesc[CarrierBand::B1]        = "B1";            //Rinex 1  2 in RINEX 3.0[013]
      ObsID::cbDesc[CarrierBand::B2]        = "B2";            //Rinex 7
      ObsID::cbDesc[CarrierBand::B3]        = "B3";            //Rinex 6
      ObsID::cbDesc[CarrierBand::I9]        = "I9";            //Rinex 9
      ObsID::cbDesc[CarrierBand::L1L2]      = "comboL1L2";     //Rinex 3
      ObsID::cbDesc[CarrierBand::Undefined] = "undefined";     //Rinex -

      ObsID::tcDesc[TrackingCode::Unknown]   = "UnknownCode";   //Rinex (sp)
      ObsID::tcDesc[TrackingCode::Any]       = "AnyCode";       //Rinex *
      ObsID::tcDesc[TrackingCode::CA]        = "GPSC/A";        //Rinex C    // GPScivil
      ObsID::tcDesc[TrackingCode::P]         = "GPSP";          //Rinex P    // GPSprecise
      ObsID::tcDesc[TrackingCode::Y]         = "GPSY";          //Rinex Y    // GPSprecise_encrypted
      ObsID::tcDesc[TrackingCode::W]         = "GPScodelessZ";  //Rinex W    // GPSprecise_encrypted_codeless_Z
      ObsID::tcDesc[TrackingCode::N]         = "GPSsquare";     //Rinex N    // GPSprecise_encrypted_codeless_squaring
      ObsID::tcDesc[TrackingCode::D]         = "GPScodeless";   //Rinex D    // GPSprecise_encrypted_codeless_other
      ObsID::tcDesc[TrackingCode::M]         = "GPSM";          //Rinex M    // GPSmilitary
      ObsID::tcDesc[TrackingCode::C2M]       = "GPSC2M";        //Rinex S    // GPScivil_M
      ObsID::tcDesc[TrackingCode::C2L]       = "GPSC2L";        //Rinex L    // GPScivil_L
      ObsID::tcDesc[TrackingCode::C2LM]      = "GPSC2L+M";      //Rinex X    // GPScivil_L+M
      ObsID::tcDesc[TrackingCode::I5]        = "GPSI5";         //Rinex I    // GPScivil_I
      ObsID::tcDesc[TrackingCode::Q5]        = "GPSQ5";         //Rinex Q    // GPScivil_Q
      ObsID::tcDesc[TrackingCode::IQ5]       = "GPSI+Q5";       //Rinex X    // GPScivil_I+Q
      ObsID::tcDesc[TrackingCode::G1P]       = "GPSC1P";        //Rinex L    // GPScivil_L1P
      ObsID::tcDesc[TrackingCode::G1D]       = "GPSC1D";        //Rinex S    // GPScivil_L1D
      ObsID::tcDesc[TrackingCode::G1X]       = "GPSC1(D+P)";    //Rinex X    // GPScivil_L1D+P

      ObsID::tcDesc[TrackingCode::GCA]       = "GLOC/A";        //Rinex C    // GLOcivil
      ObsID::tcDesc[TrackingCode::GP]        = "GLOP";          //Rinex P    // GLOprecise
      ObsID::tcDesc[TrackingCode::IR3]       = "GLOIR5";        //Rinex I    // GLO L3 I code
      ObsID::tcDesc[TrackingCode::QR3]       = "GLOQR5";        //Rinex Q    // GLO L3 Q code
      ObsID::tcDesc[TrackingCode::IQR3]      = "GLOI+QR5";      //Rinex X    // GLO L3 I+Q code
      ObsID::tcDesc[TrackingCode::L1OC]      = "GLOL1OC";
      ObsID::tcDesc[TrackingCode::L1OCD]     = "GLOL1OCD";
      ObsID::tcDesc[TrackingCode::L1OCP]     = "GLOL1OCP";
      ObsID::tcDesc[TrackingCode::L2CSI]     = "GLOL2CSI";
      ObsID::tcDesc[TrackingCode::L2CSIOCp]  = "GLOL2CSI+OCp";
      ObsID::tcDesc[TrackingCode::L2OCP]     = "GLOL2OCP";

      ObsID::tcDesc[TrackingCode::A]         = "GALA";          //Rinex A    // GAL
      ObsID::tcDesc[TrackingCode::B]         = "GALB";          //Rinex B    // GAL
      ObsID::tcDesc[TrackingCode::C]         = "GALC";          //Rinex C    // GAL
      ObsID::tcDesc[TrackingCode::BC]        = "GALB+C";        //Rinex X    // GAL
      ObsID::tcDesc[TrackingCode::ABC]       = "GALA+B+C";      //Rinex Z    // GAL
      ObsID::tcDesc[TrackingCode::IE5]       = "GALI5";         //Rinex I    // GAL
      ObsID::tcDesc[TrackingCode::QE5]       = "GALQ5";         //Rinex Q    // GAL
      ObsID::tcDesc[TrackingCode::IQE5]      = "GALI+Q5";       //Rinex X    // GAL
      ObsID::tcDesc[TrackingCode::IE5a]      = "GALI5a";        //Rinex I    // GAL
      ObsID::tcDesc[TrackingCode::QE5a]      = "GALQ5a";        //Rinex Q    // GAL
      ObsID::tcDesc[TrackingCode::IQE5a]     = "GALI+Q5a";      //Rinex X    // GAL
      ObsID::tcDesc[TrackingCode::IE5b]      = "GALI5b";        //Rinex I    // GAL
      ObsID::tcDesc[TrackingCode::QE5b]      = "GALQ5b";        //Rinex Q    // GAL
      ObsID::tcDesc[TrackingCode::IQE5b]     = "GALI+Q5b";      //Rinex X    // GAL
      ObsID::tcDesc[TrackingCode::A6]        = "GALA6";
      ObsID::tcDesc[TrackingCode::ABC6]      = "GALA+B+C6";
      ObsID::tcDesc[TrackingCode::B6]        = "GALB6";
      ObsID::tcDesc[TrackingCode::BC6]       = "GALB+C6";
      ObsID::tcDesc[TrackingCode::C6]        = "GALC6";

      ObsID::tcDesc[TrackingCode::SCA]       = "SBASC/A";       //Rinex C    // SBAS civil code
      ObsID::tcDesc[TrackingCode::SI5]       = "SBASI5";        //Rinex I    // SBAS L5 I code
      ObsID::tcDesc[TrackingCode::SQ5]       = "SBASQ5";        //Rinex Q    // SBAS L5 Q code
      ObsID::tcDesc[TrackingCode::SIQ5]      = "SBASI+Q5";      //Rinex X    // SBAS L5 I+Q code

      ObsID::tcDesc[TrackingCode::JCA]       = "QZSSC/A";       //Rinex C    // QZSS L1 civil code
      ObsID::tcDesc[TrackingCode::JD1]       = "QZSSL1C(D)";    //Rinex S    // QZSS L1C(D)
      ObsID::tcDesc[TrackingCode::JP1]       = "QZSSL1C(P)";    //Rinex L    // QZSS L1C(P)
      ObsID::tcDesc[TrackingCode::JX1]       = "QZSSL1C(D+P)";  //Rinex X    // QZSS L1C(D+P)
      ObsID::tcDesc[TrackingCode::JZ1]       = "QZSSL1-SAIF";   //Rinex Z    // QZSS L1-SAIF
      ObsID::tcDesc[TrackingCode::JM2]       = "QZSSL2C(M)";    //Rinex S    // QZSS L2 M code
      ObsID::tcDesc[TrackingCode::JL2]       = "QZSSL2C(L)";    //Rinex L    // QZSS L2 L code
      ObsID::tcDesc[TrackingCode::JX2]       = "QZSSL2C(M+L)";  //Rinex X    // QZSS L2 M+L code
      ObsID::tcDesc[TrackingCode::JI5]       = "QZSSL5I";       //Rinex I    // QZSS L5 I code
      ObsID::tcDesc[TrackingCode::JQ5]       = "QZSSL5Q";       //Rinex Q    // QZSS L5 Q code
      ObsID::tcDesc[TrackingCode::JIQ5]      = "QZSSL5I+Q";     //Rinex X    // QZSS L5 I+Q code
      ObsID::tcDesc[TrackingCode::JI6]       = "QZSSL6I";       //Rinex S    // QZSS LEX(6) I code
      ObsID::tcDesc[TrackingCode::JQ6]       = "QZSSL6Q";       //Rinex L    // QZSS LEX(6) Q code
      ObsID::tcDesc[TrackingCode::JIQ6]      = "QZSSL6I+Q";     //Rinex X    // QZSS LEX(6) I+Q code
      ObsID::tcDesc[TrackingCode::JI5S]      = "QZSSL5SI";
      ObsID::tcDesc[TrackingCode::JIQ5S]     = "QZSSL5SI+Q";
      ObsID::tcDesc[TrackingCode::JQ5S]      = "QZSSL5SQ";
      ObsID::tcDesc[TrackingCode::JD6]       = "QZSSL6D";
      ObsID::tcDesc[TrackingCode::JDE6]      = "QZSSL6D+E";
      ObsID::tcDesc[TrackingCode::JE6]       = "QZSSL6E";

      ObsID::tcDesc[TrackingCode::CI1]       = "BDSIB1";        //Rinex I    // BeiDou L1 I code
      ObsID::tcDesc[TrackingCode::CQ1]       = "BDSQB1";        //Rinex Q    // BeiDou L1 Q code
      ObsID::tcDesc[TrackingCode::CIQ1]      = "BDSI+QB1";      //Rinex X    // BeiDou L1 I+Q code
      ObsID::tcDesc[TrackingCode::CI7]       = "BDSIB2";        //Rinex I    // BeiDou B2 I code
      ObsID::tcDesc[TrackingCode::CQ7]       = "BDSQB2";        //Rinex Q    // BeiDou B2 Q code
      ObsID::tcDesc[TrackingCode::CIQ7]      = "BDSI+QB2";      //Rinex X    // BeiDou B2 I+Q code
      ObsID::tcDesc[TrackingCode::CI6]       = "BDSIB3";        //Rinex I    // BeiDou B3 I code
      ObsID::tcDesc[TrackingCode::CQ6]       = "BDSQB3";        //Rinex Q    // BeiDou B3 Q code
      ObsID::tcDesc[TrackingCode::CIQ6]      = "BDSI+QB3";      //Rinex X    // BeiDou B3 I+Q code
      ObsID::tcDesc[TrackingCode::CA1]       = "BDS1A";
      ObsID::tcDesc[TrackingCode::CCD1]      = "BDS1CD";
      ObsID::tcDesc[TrackingCode::CCDP1]     = "BDS1CDP";
      ObsID::tcDesc[TrackingCode::CCP1]      = "BDS1CP";
      ObsID::tcDesc[TrackingCode::CodelessC] = "BDSCodeless";
      ObsID::tcDesc[TrackingCode::CI2a]      = "BDSIB2a";
      ObsID::tcDesc[TrackingCode::CI2ab]     = "BDSIB2ab";
      ObsID::tcDesc[TrackingCode::CI2b]      = "BDSIB2b";
      ObsID::tcDesc[TrackingCode::CIQ3A]     = "BDSI+Q3A";
      ObsID::tcDesc[TrackingCode::CIQ2a]     = "BDSI+QB2a";
      ObsID::tcDesc[TrackingCode::CIQ2ab]    = "BDSI+QB2ab";
      ObsID::tcDesc[TrackingCode::CIQ2b]     = "BDSI+QB2b";
      ObsID::tcDesc[TrackingCode::CQ2a]      = "BDSQB2a";
      ObsID::tcDesc[TrackingCode::CQ2ab]     = "BDSQB2ab";
      ObsID::tcDesc[TrackingCode::CQ2b]      = "BDSQB2b";

      ObsID::tcDesc[TrackingCode::IA5]       = "IRNSSL5A";      //Rinex A    // IRNSS L5 SPS
      ObsID::tcDesc[TrackingCode::IB5]       = "IRNSSL5B";      //Rinex B    // IRNSS L5 RS(D)
      ObsID::tcDesc[TrackingCode::IC5]       = "IRNSSL5C";      //Rinex C    // IRNSS L5 RS(P)
      ObsID::tcDesc[TrackingCode::IX5]       = "IRNSSL5B+C";    //Rinex X    // IRNSS L5 B+C
      ObsID::tcDesc[TrackingCode::IA9]       = "IRNSSL9A";      //Rinex A    // IRNSS S-band SPS
      ObsID::tcDesc[TrackingCode::IB9]       = "IRNSSL9B";      //Rinex B    // IRNSS S-band RS(D)
      ObsID::tcDesc[TrackingCode::IC9]       = "IRNSSL9C";      //Rinex C    // IRNSS S-band RS(P)
      ObsID::tcDesc[TrackingCode::IX9]       = "IRNSSL9B+C";    //Rinex X    // IRNSS S-band B+C

      ObsID::tcDesc[TrackingCode::Undefined] = "undefined";

      if (ObsID::otDesc.size() != (int)ObsID::otLast)
         std::cerr << "Error in otDesc" << std::endl;
      if (ObsID::cbDesc.size() != (int)CarrierBand::Last)
         std::cerr << "Error in cbDesc" << std::endl;
      if (ObsID::tcDesc.size() != (int)TrackingCode::Last)
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

      ObsID::char2cb[' '] = CarrierBand::Unknown;
      ObsID::char2cb['*'] = CarrierBand::Any;
      ObsID::char2cb['1'] = CarrierBand::L1;
      ObsID::char2cb['2'] = CarrierBand::L2;
      ObsID::char2cb['3'] = CarrierBand::G3;
      ObsID::char2cb['4'] = CarrierBand::G1a;
      ObsID::char2cb['5'] = CarrierBand::L5;
      ObsID::char2cb['6'] = CarrierBand::E6;
      ObsID::char2cb['7'] = CarrierBand::E5b;
      ObsID::char2cb['8'] = CarrierBand::E5ab;
      ObsID::char2cb['9'] = CarrierBand::I9;
      ObsID::char2cb['-'] = CarrierBand::Undefined;

      ObsID::char2tc[' '] = TrackingCode::Unknown;
      ObsID::char2tc['*'] = TrackingCode::Any;
      ObsID::char2tc['C'] = TrackingCode::CA;
      ObsID::char2tc['P'] = TrackingCode::P;  
      ObsID::char2tc['W'] = TrackingCode::W;
      ObsID::char2tc['Y'] = TrackingCode::Y;
      ObsID::char2tc['M'] = TrackingCode::M;
      ObsID::char2tc['N'] = TrackingCode::N;
      ObsID::char2tc['D'] = TrackingCode::D;
      ObsID::char2tc['S'] = TrackingCode::C2M;
      ObsID::char2tc['L'] = TrackingCode::C2L;
      ObsID::char2tc['X'] = TrackingCode::C2LM;
      ObsID::char2tc['I'] = TrackingCode::I5;
      ObsID::char2tc['Q'] = TrackingCode::Q5;
      ObsID::char2tc['A'] = TrackingCode::A;
      ObsID::char2tc['B'] = TrackingCode::B;
      ObsID::char2tc['Z'] = TrackingCode::ABC;
      ObsID::char2tc['E'] = TrackingCode::JE6;
      ObsID::char2tc['-'] = TrackingCode::Undefined;

      // Since some of the items in the enums don't have corresponding RINEX
      // definitions, make sure there is an entry for all values
      for(int i=ObsID::otUnknown; i<ObsID::otLast; i++)
         ObsID::ot2char[(ObsID::ObservationType)i] = ' ';
      for (CarrierBand i : CarrierBandIterator())
         ObsID::cb2char[i] = ' ';
      for(TrackingCode i : TrackingCodeIterator())
         ObsID::tc2char[i] = ' ';

      // Here the above three maps are reversed to speed up the runtime
      for(std::map<char, ObsID::ObservationType>::const_iterator i=ObsID::char2ot.begin();
           i != ObsID::char2ot.end(); i++)
         ObsID::ot2char[i->second] = i->first;

      for(std::map<char, CarrierBand>::const_iterator i=ObsID::char2cb.begin();
           i != ObsID::char2cb.end(); i++)
         ObsID::cb2char[i->second] = i->first;

      for(std::map<char, gpstk::TrackingCode>::const_iterator i=ObsID::char2tc.begin();
           i != ObsID::char2tc.end(); i++)
         ObsID::tc2char[i->second] = i->first;

      // And add the couple 'special' cases
      ObsID::cb2char[CarrierBand::L1]        = '1';
      ObsID::cb2char[CarrierBand::L2]        = '2';
      ObsID::cb2char[CarrierBand::L5]        = '5';
      ObsID::cb2char[CarrierBand::G1]        = '1';
      ObsID::cb2char[CarrierBand::G1a]       = '4';
      ObsID::cb2char[CarrierBand::G2a]       = '6';
      ObsID::cb2char[CarrierBand::G2]        = '2';
      ObsID::cb2char[CarrierBand::G3]        = '3';
      ObsID::cb2char[CarrierBand::E6]        = '6';
      ObsID::cb2char[CarrierBand::E5b]       = '7';
      ObsID::cb2char[CarrierBand::E5ab]      = '8';
      ObsID::cb2char[CarrierBand::B1]        = '2';
      ObsID::cb2char[CarrierBand::B3]        = '6';
      ObsID::cb2char[CarrierBand::B2]        = '7';
      ObsID::cb2char[CarrierBand::I9]        = '9';
      ObsID::cb2char[CarrierBand::Unknown]   = ' ';
      ObsID::cb2char[CarrierBand::Any]       = '*';
      ObsID::cb2char[CarrierBand::Undefined] = '-';

      ObsID::tc2char[TrackingCode::CA]         = 'C';
      ObsID::tc2char[TrackingCode::N]          = 'N';
      ObsID::tc2char[TrackingCode::I5]         = 'I';
      ObsID::tc2char[TrackingCode::G1D]        = 'S';
      ObsID::tc2char[TrackingCode::G1X]        = 'X';
      ObsID::tc2char[TrackingCode::G1P]        = 'L';
      ObsID::tc2char[TrackingCode::C2LM]       = 'X';
      ObsID::tc2char[TrackingCode::C2L]        = 'L';
      ObsID::tc2char[TrackingCode::C2M]        = 'S';
      ObsID::tc2char[TrackingCode::IQ5]        = 'X';
      ObsID::tc2char[TrackingCode::M]          = 'M';
      ObsID::tc2char[TrackingCode::P]          = 'P';
      ObsID::tc2char[TrackingCode::Q5]         = 'Q';
      ObsID::tc2char[TrackingCode::D]          = 'D';
      ObsID::tc2char[TrackingCode::Y]          = 'Y';
      ObsID::tc2char[TrackingCode::W]          = 'W';
      ObsID::tc2char[TrackingCode::L1OCD]      = 'A';
      ObsID::tc2char[TrackingCode::L1OCP]      = 'B';
      ObsID::tc2char[TrackingCode::L1OC]       = 'X';
      ObsID::tc2char[TrackingCode::L2CSIOCp]   = 'X';
      ObsID::tc2char[TrackingCode::L2CSI]      = 'A';
      ObsID::tc2char[TrackingCode::L2OCP]      = 'B';
      ObsID::tc2char[TrackingCode::IR3]        = 'I';
      ObsID::tc2char[TrackingCode::IQR3]       = 'X';
      ObsID::tc2char[TrackingCode::QR3]        = 'Q';
      ObsID::tc2char[TrackingCode::GP]         = 'P';
      ObsID::tc2char[TrackingCode::GCA]        = 'C';
      ObsID::tc2char[TrackingCode::A]          = 'A';
      ObsID::tc2char[TrackingCode::ABC]        = 'Z';
      ObsID::tc2char[TrackingCode::B]          = 'B';
      ObsID::tc2char[TrackingCode::BC]         = 'X';
      ObsID::tc2char[TrackingCode::C]          = 'C';
      ObsID::tc2char[TrackingCode::IE5]        = 'I';
      ObsID::tc2char[TrackingCode::IQE5]       = 'X';
      ObsID::tc2char[TrackingCode::QE5]        = 'Q';
      ObsID::tc2char[TrackingCode::IE5a]       = 'I';
      ObsID::tc2char[TrackingCode::IQE5a]      = 'X';
      ObsID::tc2char[TrackingCode::QE5a]       = 'Q';
      ObsID::tc2char[TrackingCode::IE5b]       = 'I';
      ObsID::tc2char[TrackingCode::IQE5b]      = 'X';
      ObsID::tc2char[TrackingCode::QE5b]       = 'Q';
      ObsID::tc2char[TrackingCode::A6]         = 'A';
      ObsID::tc2char[TrackingCode::ABC6]       = 'Z';
      ObsID::tc2char[TrackingCode::B6]         = 'B';
      ObsID::tc2char[TrackingCode::BC6]        = 'X';
      ObsID::tc2char[TrackingCode::C6]         = 'C';
      ObsID::tc2char[TrackingCode::SCA]        = 'C';
      ObsID::tc2char[TrackingCode::SI5]        = 'I';
      ObsID::tc2char[TrackingCode::SIQ5]       = 'X';
      ObsID::tc2char[TrackingCode::SQ5]        = 'Q';
      ObsID::tc2char[TrackingCode::JCA]        = 'C';
      ObsID::tc2char[TrackingCode::JD1]        = 'S';
      ObsID::tc2char[TrackingCode::JX1]        = 'X';
      ObsID::tc2char[TrackingCode::JP1]        = 'L';
      ObsID::tc2char[TrackingCode::JZ1]        = 'Z';
      ObsID::tc2char[TrackingCode::JL2]        = 'L';
      ObsID::tc2char[TrackingCode::JM2]        = 'S';
      ObsID::tc2char[TrackingCode::JX2]        = 'X';
      ObsID::tc2char[TrackingCode::JI5]        = 'I';
      ObsID::tc2char[TrackingCode::JIQ5]       = 'X';
      ObsID::tc2char[TrackingCode::JQ5]        = 'Q';
      ObsID::tc2char[TrackingCode::JI5S]       = 'D';
      ObsID::tc2char[TrackingCode::JIQ5S]      = 'Z';
      ObsID::tc2char[TrackingCode::JQ5S]       = 'P';
      ObsID::tc2char[TrackingCode::JI6]        = 'S';
      ObsID::tc2char[TrackingCode::JIQ6]       = 'X';
      ObsID::tc2char[TrackingCode::JQ6]        = 'L';
      ObsID::tc2char[TrackingCode::JDE6]       = 'Z';
      ObsID::tc2char[TrackingCode::JD6]        = 'S';
      ObsID::tc2char[TrackingCode::JE6]        = 'E';
      ObsID::tc2char[TrackingCode::CIQ1]       = 'X';
      ObsID::tc2char[TrackingCode::CA1]        = 'A';
      ObsID::tc2char[TrackingCode::CCD1]       = 'D';
      ObsID::tc2char[TrackingCode::CCDP1]      = 'X';
      ObsID::tc2char[TrackingCode::CCP1]       = 'P';
      ObsID::tc2char[TrackingCode::CI1]        = 'I';
      ObsID::tc2char[TrackingCode::CQ1]        = 'Q';
      ObsID::tc2char[TrackingCode::CI2ab]      = 'D';
      ObsID::tc2char[TrackingCode::CIQ2ab]     = 'X';
      ObsID::tc2char[TrackingCode::CQ2ab]      = 'P';
      ObsID::tc2char[TrackingCode::CIQ7]       = 'X';
      ObsID::tc2char[TrackingCode::CI2a]       = 'D';
      ObsID::tc2char[TrackingCode::CIQ2a]      = 'X';
      ObsID::tc2char[TrackingCode::CQ2a]       = 'P';
      ObsID::tc2char[TrackingCode::CI2b]       = 'D';
      ObsID::tc2char[TrackingCode::CIQ2b]      = 'Z';
      ObsID::tc2char[TrackingCode::CQ2b]       = 'P';
      ObsID::tc2char[TrackingCode::CI7]        = 'I';
      ObsID::tc2char[TrackingCode::CQ7]        = 'Q';
      ObsID::tc2char[TrackingCode::CIQ6]       = 'X';
      ObsID::tc2char[TrackingCode::CI6]        = 'I';
      ObsID::tc2char[TrackingCode::CQ6]        = 'Q';
      ObsID::tc2char[TrackingCode::CodelessC]  = 'N';
      ObsID::tc2char[TrackingCode::CIQ3A]      = 'A';
      ObsID::tc2char[TrackingCode::IB5]        = 'B';
      ObsID::tc2char[TrackingCode::IX5]        = 'X';
      ObsID::tc2char[TrackingCode::IC5]        = 'C';
      ObsID::tc2char[TrackingCode::IB9]        = 'B';
      ObsID::tc2char[TrackingCode::IX9]        = 'X';
      ObsID::tc2char[TrackingCode::IC9]        = 'C';
      ObsID::tc2char[TrackingCode::IA5]        = 'A';
      ObsID::tc2char[TrackingCode::IA9]        = 'A';
      ObsID::tc2char[TrackingCode::Unknown]    = ' ';
      ObsID::tc2char[TrackingCode::Any]        = '*';
      ObsID::tc2char[TrackingCode::Undefined]  = '-';

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

      ObsID::validRinexFrequencies = "123456789";

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
