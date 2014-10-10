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
*  This program reads a synthetic CNAV data file generated
*  by ReadSyntheticCNAVData.   As each data block is read, 
*  generate an OrbElemCNAV or OrbElemCNAV2 (as appropriate)
*  and then "dump" the resulting block to an output file
*  for verification.
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
#include "OrbElemCNAV.hpp"
#include "OrbElemCNAV2.hpp"
#include "TimeString.hpp"
#include "TimeConstants.hpp"
#include "GNSSconstants.hpp"

// Project

using namespace std;
using namespace gpstk;

class ReadSyntheticCNAVData : public gpstk::BasicFramework
{
public:
   ReadSyntheticCNAVData(const std::string& applName,
              const std::string& applDesc) throw();
   ~ReadSyntheticCNAVData() {}
   virtual bool initialize(int argc, char *argv[]) throw();
   
protected:
   virtual void process();
   gpstk::CommandOptionWithAnyArg inputOption;
   gpstk::CommandOptionWithAnyArg outputOption;
   gpstk::CommandOptionNoArg      terseOption;
};

int main( int argc, char*argv[] )
{
   try
   {
      ReadSyntheticCNAVData fc("ReadSyntheticCNAVData", "");
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

ReadSyntheticCNAVData::ReadSyntheticCNAVData(const std::string& applName, 
                       const std::string& applDesc) throw()
          :BasicFramework(applName, applDesc),
           inputOption('i', "input-file", "The name of the Synthetic CNAV data file to be read.", true),
           outputOption('o', "output-file", "The name of the output file to write.", true),
           terseOption('t', "terse output", "Test one-line-per-set output.", false)
{
   inputOption.setMaxCount(1); 
   outputOption.setMaxCount(1);
   terseOption.setMaxCount(1);
}

bool ReadSyntheticCNAVData::initialize(int argc, char *argv[])
   throw()
{
   if (!BasicFramework::initialize(argc, argv)) return false;
   
   if (debugLevel)
   {
      cout << "Output File: " << outputOption.getValue().front() << endl;
   }
               
   return true;   
}

void ReadSyntheticCNAVData::process()
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

   if (terseOption.getCount()>0)
   {
      out << "         ! Begin Valid  !      Toe     ! End Valid    ! URA(m) !  IODC !   Health  !" << endl;
      out << " SVN PRN ! DOY hh:mm:ss ! DOY hh:mm:ss ! DOY hh:mm:ss !   dec  !   hex !  hex  dec !" << endl;
   }
 
   string fn = inputOption.getValue().front();
   char ifn[100];
   strcpy( ifn, inputOption.getValue().front().c_str());
   cout << "Attempting to read from file '" << fn << "'" << endl;
   ifstream in( ifn, ios::in);

   int lineCount = 0;
   char inputLine[100];
   bool CNAV2Record = false;


   string separators=" \t,";

   int recordCount = 0;
   while ( in.getline(inputLine, 100 ) )
   {
      recordCount++;

      if (inputLine[0]=='!') continue;   // Found a comment line
      string input(inputLine);
      
         // Debug
      //cout << input << endl;  

         // Should be the first line of a record.
         // Determine if record is CNAV or CNAV-2
      string sigCode = input.substr(4,3);

         //Debug
      //cout << " sigCode = '" << sigCode << "'" << endl;
      if (sigCode.compare("L1C")==0) CNAV2Record = true;
       else CNAV2Record = false;

         // Debug
      //if (CNAV2Record) cout << "CNAV2Record = true" << endl; 
      // else cout << "CNAV2Record = false" << endl; 

         // Capture SV ID, Obs ID
      string SVID = input.substr(1,2);
      int iSVID = StringUtils::asInt( SVID ); 
      SatID satID(iSVID, SatID::systemGPS);

         // Capture Transmit Time
      string::size_type pos = input.find_first_of(separators);
      pos = input.find_first_not_of(separators, pos);    // find beginning of second item
      pos = input.find_first_of(separators, pos ); // end of second item
      pos = input.find_first_not_of(separators, pos);  // beginning of third item
      string::size_type end = input.find_first_of(separators, pos);
      string sWeek = input.substr(pos, (end-pos));
      int week = StringUtils::asInt( sWeek );

      pos = input.find_first_not_of(separators, end);  // beginning of fourth item
      end = input.find_first_of(separators, pos);
      string sXMit = input.substr(pos, (end-pos) ); 
      long xMit = StringUtils::asInt( sXMit ); 

      CommonTime xMitTime = GPSWeekSecond( week, xMit );
        
      if (CNAV2Record)
      {
            // Set ObsID.  CNAV-2, so must be GPS L1C, but no code defined for 
            // that at this time.
         ObsID obsID(ObsID::otNavMsg, ObsID::cbL1, ObsID::tcAny);
         
            // Capture subframe1
         string::size_type n = input.find_last_of( separators );
         string::size_type len = input.size() - n;
         string sf1 = input.substr(n, len);
         int SF1value = StringUtils::x2uint( sf1 );
            
            // Build input string for PackedNavBits.  Start with the
            // number of characters in the record (fixed) then append
            // the next four lines of data
         string sf2String = "600 "; 
         for (int i=0; i<4; ++i)
         {
            recordCount++;
            if (! (in.getline(inputLine,100)) )
            {
               cerr << "Unexpected end of input file at line " << recordCount << endl; 
               exit(1);
            }
            sf2String += inputLine; 
         }

            // Debug
         //cout << "Input String: '" << sf2String << "'" << endl;
         
         PackedNavBits pnb( satID, obsID, xMitTime );
         try
         {
            pnb.rawBitInput( sf2String ); 
         }
         catch(InvalidParameter exc)
         {
            cerr << "Conversion to PackedNavBits failed.  Message:" << endl;
            cerr << exc.getText( ) << endl;
            exit(1);
         }
         
             // Convert the PackedNavBits into an OrbElemCNAV2 object
         try
         {
            OrbElemCNAV2 oe( obsID, satID.id, SF1value, pnb );
               // Output a terse (one-line) summary of the object 
            if (terseOption.getCount()>0) oe.dumpTerse(out);
             else out << oe << endl;
         }
         catch(InvalidParameter exc)
         {
            cerr << "Conversion of PackedNavBit to OrbElemCNAV2 failed.  Message:" << endl;
            cerr << exc.getText( )  << endl;
            exit(1);
         }
         catch(InvalidRequest exc2)
         {
            cerr << "Output of OrbElemCNAV2 object.  Message:" << endl;
            cerr << exc2.getText( )  << endl;
            exit(1);
         }
             
      }   // End of if (CNAV2Record)


         // Code for reading a CNAV record
      else
      {
            // Set ObsID.  CNAV, so may be L2C or L5
         ObsID obsID(ObsID::otNavMsg, ObsID::cbL5, ObsID::tcI5);
         if (sigCode.find("L2")!=string::npos)
         {
            obsID.band = ObsID::cbL2;
            obsID.code = ObsID::tcC2LM;
         }

            // Message 10
            // Build input string for PackedNavBits.  Start with the
            // number of characters in the record (fixed) then append
            // the next two lines of data
         string msg10String = "300 "; 
         for (int i=0; i<2; ++i)
         {
            recordCount++;
            if (! (in.getline(inputLine,100)) )
            {
               cerr << "Unexpected end of input file at line " << recordCount << endl; 
               exit(1);
            }
            msg10String += inputLine; 
         }

            // Debug
         //cout << "msg10 input Strings: '" << msg10String << "'" << endl;
         
         PackedNavBits pnb10( satID, obsID, xMitTime );
         try
         {
            pnb10.rawBitInput( msg10String ); 
         }
         catch(InvalidParameter exc)
         {
            cerr << "Conversion to PackedNavBits failed.  Message 10:" << endl;
            cerr << exc.getText( ) << endl;
            exit(1);
         }
         
            // Message 11
         string msg11String = "300 "; 
         for (int i=0; i<2; ++i)
         {
            recordCount++;
            if (! (in.getline(inputLine,100)) )
            {
               cerr << "Unexpected end of input file at line " << recordCount << endl; 
               exit(1);
            }
            msg11String += inputLine; 
         }

            // Debug
         //cout << "msg11 input Strings: '" << msg11String << "'" << endl;
         
         PackedNavBits pnb11( satID, obsID, xMitTime );
         try
         {
            pnb11.rawBitInput( msg11String ); 
         }
         catch(InvalidParameter exc)
         {
            cerr << "Conversion to PackedNavBits failed.  Message 11:" << endl;
            cerr << exc.getText( ) << endl;
            exit(1);
         }
         
            // Message Clock
         string msgClkString = "300 "; 
         for (int i=0; i<2; ++i)
         {
            recordCount++;
            if (! (in.getline(inputLine,100)) )
            {
               cerr << "Unexpected end of input file at line " << recordCount << endl; 
               exit(1);
            }
            msgClkString += inputLine; 
         }

            // Debug
         //cout << "msgClk input Strings: '" << msgClkString << "'" << endl;
         
         PackedNavBits pnbClk( satID, obsID, xMitTime );
         try
         {
            pnbClk.rawBitInput( msgClkString ); 
         }
         catch(InvalidParameter exc)
         {
            cerr << "Conversion to PackedNavBits failed.  Message Clk:" << endl;
            cerr << exc.getText( ) << endl;
            exit(1);
         }
         
             // Convert the PackedNavBits into an OrbElemCNAV2 object
         try
         {
            OrbElemCNAV oe( obsID, satID, pnb10, pnb11, pnbClk );
               // Output a terse (one-line) summary of the object 
            if (terseOption.getCount()>0) oe.dumpTerse(out);
             else out << oe << endl;
         }
         catch(InvalidParameter exc)
         {
            cerr << "Conversion of PackedNavBit to OrbElemCNAV failed.  Message:" << endl;
            cerr << exc.getText( )  << endl;
            exit(1);
         }
         catch(InvalidRequest exc2)
         {
            cerr << "Output of OrbElemCNAV object failed.  Message:" << endl;
            cerr << exc2.getText( )  << endl;
            exit(1);
         }
             
      }   // End of else
      
   }  // End of read loop

}
