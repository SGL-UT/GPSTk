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

/**
*
*  This program reads an FIC file, filters it down to the 
*  Block 109 data, and generates pseudo-CNAV (or CNAV-2) data
*  in "as broadcast binary" format.  The original purpose 
*  was to test the OrbElemICE/OrbElemCNAV/OrbElemCNAV2 
*  classes.  See the corresponding test reader program.  
*  (That program hasn't been written when this comment was
*  generated.)
*
*  Command line
*    -i : input file
*    -o : output file
*    -t : ObsType to output - <TBD Add examples>
*
*
*  Output format
*  
*  Any line beginning with '!' in considered a comment.
*
* For CNAV (TBD - need two messages)
*  Gpp ooo wwww ssssss   0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX
*  0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX
*  0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX
*  0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX
*  0xXXXXXXXX 
*
* For CNAV-2 
*  Gpp ooo wwww ssssss   0xYYYYYYYY
*  0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX
*  0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX
*  0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX
*  0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX
*  0xXXXXXXXX 0xXXXXXXXX 0xXXXXXX00 
*
*  G = GPS
*  pp = PRN ID
*  ooo = ObsID string
*  xMit time = in week # (wwww) and SOW (ssssss)
*  YYYY = Subframe 1
*  XXXXXXXX = Subframe 2, left-justified, 32 bits per word
*             (600 bits - 18.75 32-bit words)
* 
*  Note that one can (must) infer CNAV or CNAV-2 from the ObsID.
* 
*/
// System
#include <iostream>
#include <fstream>
#include <cstdlib>

// gpstk
#include "FileFilterFrame.hpp"
#include "BasicFramework.hpp"
#include "StringUtils.hpp"
#include "gps_constants.hpp"
#include "PackedNavBits.hpp"
#include "SatID.hpp"
#include "ObsID.hpp"
#include "OrbElemFIC109.hpp"
#include "TimeString.hpp"
#include "TimeConstants.hpp"
#include "GNSSconstants.hpp"

// fic
#include "FICStream.hpp"
#include "FICHeader.hpp"
#include "FICData.hpp"
#include "FICFilterOperators.hpp"

// Project

using namespace std;
using namespace gpstk;

class GenSyntheticCNAVData : public gpstk::BasicFramework
{
public:
   GenSyntheticCNAVData(const std::string& applName,
              const std::string& applDesc) throw();
   ~GenSyntheticCNAVData() {}
   virtual bool initialize(int argc, char *argv[]) throw();
   
protected:
   virtual void process();
   gpstk::CommandOptionWithAnyArg inputOption;
   gpstk::CommandOptionWithAnyArg outputOption;
   gpstk::CommandOptionWithAnyArg obsTypeOption;

   std::list<long> blockList;

   void convertCNAV(ofstream& out, const OrbElemFIC109& oe);
   void convertCNAV2(ofstream& out, const OrbElemFIC109& oe);

   bool outputCNAV; 

   //ofstream out;
};

int main( int argc, char*argv[] )
{
   try
   {
      GenSyntheticCNAVData fc("GenSyntheticCNAVData", "");
      if (!fc.initialize(argc, argv)) return(false);
      fc.run();
   }
   catch(gpstk::Exception& exc)
   {
      cout << exc << endl;
      return 1;
   }
   catch(...)
   {
      cout << "Caught an unnamed exception. Exiting." << endl;
      return 1;
   }
   return 0;
}

GenSyntheticCNAVData::GenSyntheticCNAVData(const std::string& applName, 
                       const std::string& applDesc) throw()
          :BasicFramework(applName, applDesc),
           inputOption('i', "input-file", "The name of the FIC file to be read.", true),
           outputOption('o', "output-file", "The name of the output file to write.", true),
           obsTypeOption('t',"obs type","obs type to simulate: CNAV or CNAV-2.",true),
           outputCNAV(false)
{
   inputOption.setMaxCount(1); 
   outputOption.setMaxCount(1);
   obsTypeOption.setMaxCount(1);
}

bool GenSyntheticCNAVData::initialize(int argc, char *argv[])
   throw()
{
   if (!BasicFramework::initialize(argc, argv)) return false;
   
   if (debugLevel)
   {
      cout << "Output File: " << outputOption.getValue().front() << endl;
   }
   
      // Set up the FIC data filter
   blockList.push_back(109);

   string typeCheck = obsTypeOption.getValue().front();
   if       (typeCheck.compare("CNAV-2")==0) outputCNAV = false;
    else if (typeCheck.compare("CNAV")==0) outputCNAV = true;
    else 
    {
       cerr << "Type must be 'CNAV' or 'CNAV-2'" << endl;
       return false;
    }

                
   return true;   
}

void GenSyntheticCNAVData::process()
{

      // Open the output stream
   char ofn[100];
   strcpy( ofn, outputOption.getValue().front().c_str());
   ofstream out( ofn, ios::out );
   if (!out) 
   {
      cerr << "Failed to open output file. Exiting." << endl;
      exit(1);
   }
 
   string fn = inputOption.getValue().front();
   cout << "Attempting to read from file '" << fn << "'" << endl;
   FileFilterFrame<FICStream, FICData> input(fn);
   int recordCount = 0; 


      // Put descriptive header into the output file
   if (outputCNAV) {}
   else
   {
      out << "!  Synthetic CNAV-2 data generated from Legcay Nav Data" << endl;
      out << "!  Input File: " << fn << endl; 
      out << "!  " << endl; 
      out << "!  Gpp ooo wwww ssssss   0xYYYY" << endl;
      out << "!  0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX" << endl;
      out << "!  0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX" << endl;
      out << "!  0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX" << endl;
      out << "!  0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX 0xXXXXXXXX" << endl;
      out << "!  0xXXXXXXXX 0xXXXXXXXX 0xXXXXXX00 " << endl;
      out << "!  " << endl; 
      out << "!  G = GPS" << endl;
      out << "!  pp = PRN ID" << endl;
      out << "!  ooo = ObsID string" << endl;
      out << "!  xMit time = in week # (wwww) and SOW (ssssss)" << endl;
      out << "!  YYYY = Subframe 1" << endl;
      out << "!  XXXXXXXX = Subframe 2, left-justified, 32 bits per word" << endl;
      out << "!             (600 bits - 18.75 32-bit words)" << endl;
      out << "!" << endl; 
   
   }

      // Filter the FIC data for the requested vlock(s)
   input.filter(FICDataFilterBlock(blockList));

   list<FICData>& ficList = input.getData();
   cout << "Read " << input.size() << " records from input file" << endl;
   list<FICData>::iterator itr = ficList.begin();
   
   while (itr != ficList.end())
   {
      recordCount++;
         
      FICData& r = *itr;
      OrbElemFIC109 oe(r);

      if (outputCNAV) convertCNAV( out, oe );
       else convertCNAV2( out, oe );
      
      itr++;
   }  // End of FIC Block loop

}

void GenSyntheticCNAVData::convertCNAV(ofstream& out, const OrbElemFIC109& oe)
{
}


void GenSyntheticCNAVData::convertCNAV2(ofstream& out, const OrbElemFIC109& oe)
{
   out << "G" << setw(2) << setfill('0') <<  dec << oe.satID.id;
   out << setfill(' ') << " L1C ";

      //Debug
   cout << "G" << setw(2) << setfill('0') << oe.satID.id;
   cout << setfill(' ') << " L1C " << 
             printTime(oe.transmitTime,"%04F %06.0g") << endl; 
      
      // Synthesize an ObsID such that this data APPEARS
      // to have come from L1C.
      // Need to add an L1C tracking code to ObsID
   ObsID obsID( ObsID::otNavMsg, ObsID::cbL1, ObsID::tcAny);

      // Instantiate a PackedNavBits object to contain the Subframe 2 data
   PackedNavBits pnb( oe.satID, obsID, oe.transmitTime ); 

      // Timing considerations.  See IS-GPS-800 Section 3.5.2
      // Subframe 1 is a 9-bit value (TOI count) that represents the number of
      // 18-second intervals since the last two-hour epoch that will
      // be complete at the END of this frame.   
      // That is to say, the TOI in the FIRST frame at an even two-hour
      // epoch will be "1".  The TOI in the LAST frame before the two-hour
      // epoch will be "0".
      //
      // oe.transmitTime represents the beginning time of the tranmission of
      // this message.  The legacy navigation message is based on 30-second
      // subframes and CNAV-2 has a 18-second frame rate.  Therefore, some 
      // adjustments need to be made. 
   double SOWBeginLegacyMsg = (static_cast<GPSWeekSecond>(oe.transmitTime)).sow;

      // Round BACK to nearest 18 second interval.  
   double adjSOWBegin = (double) (( (long)SOWBeginLegacyMsg / 18 ) * 18);

   int numTwoHourEpochs = (long) adjSOWBegin / 7200; 
   double secSinceLastTwoHourEpoch =  adjSOWBegin - (numTwoHourEpochs * 7200); 
   int currentNumCNAV2Frames = secSinceLastTwoHourEpoch / 18;
   int nextNumCNAV2Frames = currentNumCNAV2Frames + 1; 
      // ---End Timing considerations

      // output the transmit SOW (as adjusted)
   out << setw(6) << setfill('0') << (unsigned long)adjSOWBegin << "  "; 


      // Output subframe 1 (TOI count)
   out << "0x" << setw(4) << setfill('0') << hex << nextNumCNAV2Frames << endl; 

      // Translate data from FIC 109 legacy format to 
      // CNAV 2 structure.
   unsigned long TOWWeek   = (static_cast<GPSWeekSecond>(oe.transmitTime)).week;
   int n_TOWWeek           = 13;
   int s_TOWWeek           = 1;

   unsigned long ITOW      = numTwoHourEpochs;
   int n_ITOW              = 8;
   int s_ITOW              = 1;

      // Form estimate of Top based on Toe and AODO
   double dTop             = (static_cast<GPSWeekSecond>(oe.ctToe)).sow;
   dTop                   -= oe.AODO;        // Convert from counts to seconds, 
                                             // then subtract from Toe.
   if (dTop<0.0) dTop += FULLWEEK;
   unsigned long Top       = (unsigned long) (dTop/300.0);
   int n_Top               = 11;
   int s_Top               = 300;

   unsigned long L1CHealth = 0;
   if (!oe.healthy) L1CHealth = 1;
   int n_L1CHealth         = 1;
   int s_L1CHealth         = 1;

   long URAed              = oe.accFlag;
   int n_URAed             = 5;
   int s_URAed             = 1;

   double fullToeSOW       = (static_cast<GPSWeekSecond>(oe.ctToe)).sow;
   double ToeScaled        = fullToeSOW/300.0;
   unsigned long Toe       = ToeScaled; 
   int n_Toe               = 11;
   int s_Toe               = 300;

   double deltaA           = oe.A - A_REF_GPS;
   int n_deltaA            = 26;
   int s_deltaA            = -9;

   double Adot             = 0;
   int n_Adot              = 25;
   int s_Adot              = -21;

   double dn               = oe.dn;
   int n_dn                = 17;
   int s_dn                = -44;

   double dndot            = 0;
   int n_dndot             = 23;
   int s_dndot             = -57;

   double M0               = oe.M0;
   int n_M0                = 33;
   int s_M0                = -32;

   double ecc              = oe.ecc;
   int n_ecc               = 33;
   int s_ecc               = -34;

   double w                = oe.w;
   int n_w                 = 33;
   int s_w                 = -32;

   double OMEGA0           = oe.OMEGA0;
   int n_OMEGA0            = 33;
   int s_OMEGA0            = -32;

   double i0               = oe.i0;
   int n_i0                = 33;
   int s_i0                = -32;

   double OMEGAdot         = oe.OMEGAdot;
   int n_OMEGAdot          = 24;
   int s_OMEGAdot          = -43;

   double deltaOMEGAdot    = OMEGAdot - OMEGADOT_REF_GPS;
   int n_deltaOMEGAdot     = 17;
   int s_deltaOMEGAdot     = -44;

   double idot             = oe.idot;
   int n_idot              = 15;
   int s_idot              = -44;

   double Cis              = oe.Cis;
   int n_Cis               = 16;
   int s_Cis               = -30;

   double Cic              = oe.Cic;
   int n_Cic               = 16;
   int s_Cic               = -30;

   double Crs              = oe.Crs;
   int n_Crs               = 24;
   int s_Crs               = -8;

   double Crc              = oe.Crc;
   int n_Crc               = 24;
   int s_Crc               = -8;

   double Cus              = oe.Cus;
   int n_Cus               = 21;
   int s_Cus               = -30;

   double Cuc              = oe.Cuc;
   int n_Cuc               = 21;
   int s_Cuc               = -30;

   long URAned0              = oe.accFlag;
   int n_URAned0             = 5;
   int s_URAned0             = 1;

   unsigned long URAned1    = 1;           // No value provided in legacy
   int n_URAned1            = 3;
   int s_URAned1            = 1;

   unsigned long URAned2    = 2;           // No value provided in legacy
   int n_URAned2            = 3;
   int s_URAned2            = 1;
   
   double af0              = oe.af0;
   int n_af0               = 26;
   int s_af0               = -35;

   double af1              = oe.af1;
   int n_af1               = 20;
   int s_af1               = -48;

   double af2              = oe.af2;
   int n_af2               = 10;
   int s_af2               = -60;

   double Tgd              = oe.Tgd;
   int n_Tgd               = 13;
   int s_Tgd               = -35;

   double ISCL1cp          = 1E-8;         // No value provided in legacy
   int n_ISCL1cp           = 13;
   int s_ISCL1cp           = -35;

   double ISCL1cd          = -1E-8;        // No value provided in legacy
   int n_ISCL1cd           = 13;
   int s_ISCL1cd           = -35;

   unsigned long sflag     = 0;            // Assume integrity status flag OFF
   int n_sflag             = 1;
   int s_sflag             = 1;

   unsigned long reservedBits = 0;
   int n_reservedBits         = 10;
   int s_reservedBits         = 1;

   unsigned long CRC       = 0;
   int n_CRC               = 24;
   int s_CRC               = 1; 


   //cout << "TOWWeek, ITOW, Top, AODO = " << TOWWeek << ", " << ITOW << ", " 
   //     << Top << ", " << oe.AODO << endl;

      // Pack the individual items into the structure
   pnb.addUnsignedLong(TOWWeek, n_TOWWeek, s_TOWWeek);
   pnb.addUnsignedLong(ITOW, n_ITOW, s_ITOW);
   pnb.addUnsignedLong(Top, n_Top, s_Top);
   pnb.addUnsignedLong(L1CHealth, n_L1CHealth, s_L1CHealth);
   pnb.addLong(URAed, n_URAed, s_URAed);
   pnb.addUnsignedLong(Toe, n_Toe, s_Toe);
   pnb.addSignedDouble(deltaA, n_deltaA, s_deltaA);
   pnb.addSignedDouble(Adot, n_Adot, s_Adot);
   pnb.addDoubleSemiCircles(dn, n_dn, s_dn);
   pnb.addDoubleSemiCircles(dndot, n_dndot, s_dndot);
   pnb.addDoubleSemiCircles(M0, n_M0, s_M0);
   pnb.addUnsignedDouble(ecc, n_ecc, s_ecc);
   pnb.addDoubleSemiCircles(w, n_w, s_w);
   pnb.addDoubleSemiCircles(OMEGA0, n_OMEGA0, s_OMEGA0);
   pnb.addDoubleSemiCircles(i0, n_i0, s_i0);
   pnb.addDoubleSemiCircles(deltaOMEGAdot, n_deltaOMEGAdot, s_deltaOMEGAdot);
   pnb.addDoubleSemiCircles(idot, n_idot, s_idot); 
   pnb.addSignedDouble(Cis, n_Cis, s_Cis);
   pnb.addSignedDouble(Cic, n_Cic, s_Cic);
   pnb.addSignedDouble(Crs, n_Crs, s_Crs);
   pnb.addSignedDouble(Crc, n_Crc, s_Crc);
   pnb.addSignedDouble(Cus, n_Cus, s_Cus);
   pnb.addSignedDouble(Cuc, n_Cuc, s_Cuc);
   pnb.addLong(URAned0, n_URAned0, s_URAned0);
   pnb.addUnsignedLong(URAned1, n_URAned1, s_URAned1);
   pnb.addUnsignedLong(URAned2, n_URAned2, s_URAned2);
   pnb.addSignedDouble(af0, n_af0, s_af0);
   pnb.addSignedDouble(af1, n_af1, s_af1);
   pnb.addSignedDouble(af2, n_af2, s_af2);
   pnb.addSignedDouble(Tgd, n_Tgd, s_Tgd);
   pnb.addSignedDouble(ISCL1cp, n_ISCL1cp, s_ISCL1cp);
   pnb.addSignedDouble(ISCL1cd, n_ISCL1cd, s_ISCL1cd);
   pnb.addUnsignedLong(sflag, n_sflag, s_sflag);
   pnb.addUnsignedLong(reservedBits, n_reservedBits, s_reservedBits);
   pnb.addUnsignedLong(CRC, n_CRC, s_CRC);

      // Resize the vector holding the packed nav message data
   pnb.trimsize();

      // Output the results 
   pnb.outputPackedBits(out);
   out << endl;

   return;
}
