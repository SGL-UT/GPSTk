#pragma ident "$Id: RinexConverter.cpp 2009-08-25 17:31:40 tvarney $"

//============================================================================//
//                                                                            //
//  This file is part of GPSTk, the GPS Toolkit.                              //
//                                                                            //
//  The GPSTk is free software; you can redistribute it and/or modify it      //
//  under the terms of the GNU Lesser General Public License as published by  //
//  the Free Software Foundation; either version 2.1 of the License, or any   //
//  later version.                                                            //
//                                                                            //
//  The GPSTk is distributed in the hope that it will be useful, but WITHOUT  //
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     //
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public      //
//  License for more details.                                                 //
//                                                                            //
//  You should have received a copy of the GNU Lesser General Public  License //
//  along with GPSTk; if not, write to the Free Software Foundation, Inc.,    //
//  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA                   //
//                                                                            //
//  Copyright 2009, The University of Texas at Austin                         //
//                                                                            //
//============================================================================//

#include "RinexConverter.hpp"
#include "RinexSatID.hpp"

using namespace gpstk;
using namespace std;

//#define DEBUG

namespace gpstk
{

//============================================================================//

bool RinexConverter::fillOptionalFields = true;
bool RinexConverter::keepComments       = true;

string RinexConverter::markerType;

RinexConverter::CodeMap RinexConverter::obsMap;

RinexConverter::Initializer RinexSingleton;

RinexConverter::Initializer::Initializer()
{
#ifdef DEBUG
   cout << "Initialzing...";
#endif

   obsMap["C1"] = "C1C";
   obsMap["C2"] = "C2C";
   obsMap["C5"] = "C5C";
   obsMap["C6"] = "C6C";
   obsMap["C7"] = "C7C";
   obsMap["C8"] = "C8C";

   obsMap["P1"] = "C1P";
   obsMap["P2"] = "C2P";

   obsMap["L1"] = "L1C";
   obsMap["L2"] = "L2C";
   obsMap["L5"] = "L5C";
   obsMap["L6"] = "L6C";
   obsMap["L7"] = "L7C";
   obsMap["L8"] = "L8C";

   obsMap["D1"] = "D1C";
   obsMap["D2"] = "D2C";
   obsMap["D5"] = "D5C";
   obsMap["D6"] = "D6C";
   obsMap["D7"] = "D7C";
   obsMap["D8"] = "D8C";

   obsMap["S1"] = "S1C";
   obsMap["S2"] = "S2C";
   obsMap["S5"] = "S5C";
   obsMap["S6"] = "S6C";
   obsMap["S7"] = "S7C";
   obsMap["S8"] = "S8C";

#ifdef DEBUG
   cout << "Done!" << endl;
#endif
}

//============================================================================//


/**
 * RINEX 2.11 -> 3.0 methods do not currently account for the WAVELENGTH FACT
 * lines in the RINEX 2.11 header.
 */
bool RinexConverter::convertToRinex3(Rinex3ObsData& dest,
                                     const RinexObsData& src,
                                     const RinexObsHeader& srcHeader)
{
   /// Set the things that correlate 1 to 1 and are necessary for the Header.

   dest.epochFlag    = src.epochFlag;
   dest.numSVs       = src.numSvs;
   dest.clockOffset  = src.clockOffset;
   dest.time         = src.time;

   // Convert the Header first.

   convertToRinex3(dest.auxHeader, srcHeader);

   /// Clear the Obs list in the R3 object passed.

   dest.obs.clear();

   /// Iterator to walk over the observation data in the source data.

   RinexObsData::RinexSatMap::const_iterator iter = src.obs.begin();

   /// The list of observations in the source header.

   vector<RinexObsHeader::RinexObsType> oldTypeList = srcHeader.obsTypeList;

   /// The vector of observations to add to the destination header.

   vector<Rinex3ObsData::RinexDatum> vec;
   Rinex3ObsData::RinexDatum tempR3;
   RinexObsData::RinexDatum tempR2;

   /// Loop over the satellites in the data set.

   for ( ; iter != src.obs.end(); ++iter)
   {
      vec.clear();

      /// Loop over the Obs types in this file.

      for (int i = 0; i < oldTypeList.size(); ++i)
      {
         /// Get the satellite system character from the ID.

         std::string satSystem(1,RinexSatID(iter->first).systemChar());

         /// Transfer the RinexDatum to the new definition.

         tempR2 = (iter->second).find(oldTypeList[i])->second;
         tempR3.data = tempR2.data;
         tempR3.lli  = tempR2.lli;
         tempR3.ssi  = tempR2.ssi;

         ///Check the current code against the current satellite's system
         ///If it is good, add it to the vector.

         if      (satSystem == "G")
         {
            if (validGPScode(oldTypeList[i])) vec.push_back(tempR3);
         }
         else if (satSystem == "E")
         {
            if (validGALcode(oldTypeList[i])) vec.push_back(tempR3);
         }
         else if (satSystem == "R")
         {
            if (validGLOcode(oldTypeList[i])) vec.push_back(tempR3);
         }
         else if (satSystem == "S")
         {
            if (validGEOcode(oldTypeList[i])) vec.push_back(tempR3);
         }
         else
         {
            cout << "system not found" << endl;
         }
      }

      /// Add the filled vector to the destination's Obs map.

      dest.obs[iter->first] = vec;
   }

   return true;
}

bool RinexConverter::convertToRinex3(Rinex3ObsHeader& dest,
                                     const RinexObsHeader& src)
{
   /// Transfer all items with a 1 to 1 correlation.

   dest.version         = 3.0;
   dest.fileType        = src.fileType;
   dest.system          = src.system;
   dest.fileProgram     = src.fileProgram;
   dest.fileAgency      = src.fileAgency;
   dest.date            = src.date;
   dest.markerName      = src.markerName;
   dest.observer        = src.observer;
   dest.agency          = src.agency;
   dest.recNo           = src.recNo;
   dest.recType         = src.recType;
   dest.recVers         = src.recVers;
   dest.antNo           = src.antNo;
   dest.antType         = src.antType;
   dest.antennaPosition = src.antennaPosition;
   dest.antennaDeltaHEN = src.antennaOffset; //I hope...
   dest.firstObs        = src.firstObs;

   /// Marker Type does not exist in RINEX 2.11.
   /// If the user has defined the markerType string, use it.
   /// Otherwise, default to NON_GEODETIC for low-precision fixed receiver.

   if (markerType.length() > 0)
      dest.markerType   = markerType;
   else
      dest.markerType   = "NON_GEODETIC";

#ifdef DEBUG
   cout << endl;
   cout << "       Member: " << endl;
   cout << "      Version: " << dest.version         << endl;
   cout << "    File Type: " << dest.fileType        << endl;
   cout << "       System: " << dest.system          << endl;
   cout << " File Program: " << dest.fileProgram     << endl;
   cout << "  File Agency: " << dest.fileAgency      << endl;
   cout << "         Date: " << dest.date            << endl;
   cout << "  Marker Name: " << dest.markerName      << endl;
   cout << "  Marker Type: " << dest.markerType      << endl;
   cout << "     Observer: " << dest.observer        << endl;
   cout << "       Agency: " << dest.agency          << endl;
   cout << "       Rec No: " << dest.recNo           << endl;
   cout << "     Rec Type: " << dest.recType         << endl;
   cout << "     Rec Vers: " << dest.recVers         << endl;
   cout << "   Antenna No: " << dest.antNo           << endl;
   cout << " Antenna Type: " << dest.antType         << endl;
   cout << "  Antenna Pos: " << dest.antennaPosition << endl;
   cout << "Antenna Delta: " << dest.antennaDeltaHEN << endl;
   cout << "    First Obs: " << dest.firstObs        << endl;
#endif

   /// Obs type lists for each system.

   vector<ObsID> gpsTypeList;   ///< GPS type list
   vector<ObsID> galTypeList;   ///< Galileo type list
   vector<ObsID> gloTypeList;   ///< GLONASS type list
   vector<ObsID> geoTypeList;   ///< SBAS type list

   /// Types present in the old header.

   vector<RinexObsHeader::RinexObsType> oldTypeList = src.obsTypeList;

   /// The current code in the old Header.

   std::string currCode;

   /// Store the results of the map lookup to get the new ObsID object.

   std::string replacement;

   /// The new ObsID object.

   ObsID newID;

   /// Iterator to find the current code.

   CodeMap::const_iterator mapIter;

   /// Loop over the old codes.

#ifdef DEBUG
   cout << "Reading in codes..." << endl;
#endif

   for (int i = 0; i < oldTypeList.size(); ++i)
   {
      /// Strip the first two letters (there should only be 2) from the old type.

      currCode = oldTypeList[i].type;
//      currCode[1] = oldTypeList[i].type[1];
//      currCode[2] = 0;

#ifdef DEBUG
      cout << "   Current Code: " << currCode << endl;
#endif

      /// This shouldn't fail. Get the replacement string from the static map
      /// The resulting value from the lookup is actually char[3], but string
      /// is used to encapsulate it to allow ObsID to work right.

      for (mapIter = obsMap.begin(); mapIter != obsMap.end(); ++mapIter)
      {
#ifdef DEBUG
         cout << "      Checking <" << mapIter->first << "," << mapIter->second << ">" << endl;
#endif
         if (mapIter->first == currCode)
            break;
      }
      if (mapIter == obsMap.end())
      {
#ifdef DEBUG
         cout << "      Couldn't find code " << currCode << endl;
#endif
         continue;
      }

      replacement = mapIter->second;

#ifdef DEBUG
      cout << "   Found replacement string: " << replacement << endl;
#endif

      /// Create the new ObsID from the replacement string.

      newID = ObsID(replacement);

      /// If this code is a valid type for the systems, add it to their
      /// respective vector.

      if (validGPScode(oldTypeList[i]))
         gpsTypeList.push_back(newID);
      if (validGLOcode(oldTypeList[i]))
         gloTypeList.push_back(newID);
      if (validGALcode(oldTypeList[i]))
         galTypeList.push_back(newID);
      if (validGEOcode(oldTypeList[i]))
         geoTypeList.push_back(newID);
   }

#ifdef DEBUG
   cout << "Finished reading in codes";
#endif

   /// Add the type lists to the destination header.
   /// This will probably be very bloated, but the bloat is trivial in every
   /// case. This can either be trimmed later by a different problem, or if
   /// the header contains the optional Sat / # obs lines it can be stipped
   /// here...

   dest.mapObsTypes["G"] = gpsTypeList;
   dest.mapObsTypes["R"] = gloTypeList;
   dest.mapObsTypes["E"] = galTypeList;
   dest.mapObsTypes["S"] = geoTypeList;

   dest.valid = 1;

   ///Done if not doing optional fields, so return

   if (!fillOptionalFields) return true;

   /// Clear the destination's comment list, transfer comments from src only
   /// if keepComments is set.

   dest.commentList.clear();
   if (keepComments) dest.commentList = src.commentList;

   /// Transfer the non trivial, optional data members.

   dest.markerNumber    = src.markerNumber;
   dest.interval        = src.interval;
   dest.lastObs         = src.lastObs;
   dest.receiverOffset  = src.receiverOffset;
   dest.leapSeconds     = src.leapSeconds;
   dest.numSVs          = src.numSVs;

   /// An iterator over the optional numObsForSat data...

   map<SatID, vector<int> >::const_iterator iter = src.numObsForSat.begin();

   /// Booleans indicating if a particular system is present in the data.
   /// Set them all to false (the default).

   bool hasGPS, hasGLO, hasGAL, hasGEO;
   hasGPS = hasGLO = hasGAL = hasGEO = false;

   /// Loop over the satellites in this structure.

   for ( ; iter != src.numObsForSat.end(); ++iter)
   {
      /// Keep the satID as a RinexSatID for use later.

      RinexSatID id = iter->first;

      /// Copy over the data from src to dest, creating a new entry.

      dest.numObsForSat[id] = iter->second;

      /// Get the system character from the satellite id.

      std::string systemCode(1,id.systemChar());

      /// Test the system character, set the corresponding system's indicator
      /// to true.

      if      (systemCode == "G") hasGPS = true;
      else if (systemCode == "R") hasGLO = true;
      else if (systemCode == "E") hasGAL = true;
      else if (systemCode == "S") hasGEO = true;
   }

   /// Strip away the systems whose booleans are still false.
   /// This gives a perfectly accurate R3 Header.

   if (!hasGPS)
      dest.mapObsTypes.erase(dest.mapObsTypes.find("G"));
   if (!hasGLO)
      dest.mapObsTypes.erase(dest.mapObsTypes.find("R"));
   if (!hasGAL)
      dest.mapObsTypes.erase(dest.mapObsTypes.find("E"));
   if (!hasGEO)
      dest.mapObsTypes.erase(dest.mapObsTypes.find("S"));

//   dest.valid = src.valid;
   dest.valid = dest.allValid30; // ***** temporary kludge

   return true;
}

/// ***** The Rinex 3.0 -> 2.11 methods are not finished. *****

bool RinexConverter::convertFromRinex3(RinexObsData& dest,
                                       const Rinex3ObsData& src,
                                       const Rinex3ObsHeader& srcHeader)
{
   // Unsorted Obs! Obviously the header wasn't translated...

   Rinex3ObsHeader duplHeader = srcHeader;
   if (srcHeader.obsTypeList.size() == 0)
      sortRinex3ObsTypes(duplHeader);

   dest.epochFlag   = src.epochFlag;
   dest.numSvs      = src.numSVs;   // Capitilization is (too) important here.
   dest.clockOffset = src.clockOffset;
   dest.time        = src.time;
   convertFromRinex3(dest.auxHeader, srcHeader);

   ///TODO::Implement the translation between R3 and R2 maps here...
   /** Basic Steps
    * 1. Get an Observation Code from the obsTypeList vector.
    * 2. Translate code into a R2.11 compatible code.
    * 3. If the code isn't compatible with the current satellites system,
    *      continue from the top of the loop.
    * 2. Otherwise, get the index for this obs in the current satellite's system
    *      Get this by comparing the STRING values of the obs in the list
    *      to the obs code taken from the sorted list. MUST compare the string
    *      returned by ObsID::asRinex3ID(); otherwise redundant codes can be
    *      used.
    * 3. Found it? Move the data over to the correct spot from the R3 objec.
    *    Otherwise, search for a similar code in this system (same band/type)
    *    If one exists, put it in the appropriate spot. Otherwise, insert a
    *    blank entry ( double(0.0) )
    */
   return true;
}

bool RinexConverter::convertFromRinex3(RinexObsHeader& dest,
                                       const Rinex3ObsHeader& src)
{
   dest.version         = 2.11;
   dest.fileType        = src.fileType;
   dest.system          = src.system;
   dest.fileProgram     = src.fileProgram;
   dest.fileAgency      = src.fileAgency;
   dest.date            = src.date;
   dest.markerName      = src.markerName;
   dest.observer        = src.observer;
   dest.agency          = src.agency;
   dest.recNo           = src.recNo;
   dest.recType         = src.recType;
   dest.recVers         = src.recVers;
   dest.antNo           = src.antNo;
   dest.antType         = src.antType;
   dest.antennaPosition = src.antennaPosition;
   dest.antennaOffset   = src.antennaDeltaHEN;  //I hope...
   dest.firstObs        = src.firstObs;

   Rinex3ObsHeader duplHeader = src;
   sortRinex3ObsTypes(duplHeader);

   // Done if not doing optional fields...
   // This returns false right now...

   if (!fillOptionalFields) return dest.isValid();

   // The optional fields...

   dest.commentList     = src.commentList;
   dest.markerNumber    = src.markerNumber;
   dest.interval        = src.interval;
   dest.lastObs         = src.lastObs;
   dest.receiverOffset  = src.receiverOffset;
   dest.leapSeconds     = src.leapSeconds;
   dest.numSVs          = src.numSVs;

   map<RinexSatID, vector<int> >::const_iterator satIter = src.numObsForSat.begin();
   if (src.numObsForSat.size() > 0)
   {
      for ( ; satIter != src.numObsForSat.end(); ++satIter)
      {
         dest.numObsForSat[satIter->first] = satIter->second;
      }
   }

   return dest.isValid();
}

void RinexConverter::reset()
{
   fillOptionalFields = true;
   keepComments = true;
   markerType = "";
}

bool RinexConverter::validGPScode(const RinexObsHeader::RinexObsType& code)
{
   const int numGood = 14;
   std::string good[] = {"C1","C2","C5","P1","P2","L1","L2","L5","D1","D2",
                         "D5","S1","S2","S5"};
   for (int i = 0; i < numGood; ++i)
   {
      if (code.type == good[i])
      {
#ifdef DEBUG
         cout << "      Valid Code for system G" << endl;
#endif
         return true;
      }
   }
   return false;
}
bool RinexConverter::validGALcode(const RinexObsHeader::RinexObsType& code)
{
   const int numGood = 20;
   std::string good[] = {"C1","C5","C6","C7","C8","L1","L5","L6","L7","L8",
                         "D1","D5","D6","D7","D8","S1","S5","S6","S7","S8"};
   for (int i = 0; i < numGood; ++i)
   {
      if (code.type == good[i])
      {
#ifdef DEBUG
         cout << "      Valid Code for system E" << endl;
#endif
         return true;
      }
   }
   return false;
}
bool RinexConverter::validGLOcode(const RinexObsHeader::RinexObsType& code)
{
   const int numGood = 10;
   std::string good[] = {"C1","C2","P1","P2","L1","L2","D1","D2","S1","S2"};
   for (int i = 0; i < numGood; ++i)
   {
      if (code.type == good[i])
      {
#ifdef DEBUG
         cout << "      Valid Code for system R" << endl;
#endif
         return true;
      }
   }
   return false;
}

bool RinexConverter::validGEOcode(const RinexObsHeader::RinexObsType& code)
{
   const int numGood = 8;
   std::string good[] = {"C1","C5","L1","L5","D1","D5","S1","S5"};
   for (int i = 0; i < numGood; ++i)
   {
      if (code.type == good[i])
      {
#ifdef DEBUG
         cout << "      Valid Code for system S" << endl;
#endif
         return true;
      }
   }
   return false;
}

void RinexConverter::sortRinex3ObsTypes(Rinex3ObsHeader& header)
{
   /** Basic steps:
    * 1. Compile a set of existing codes, based on their STRING values.
    *    Unfortunately, just comparing the ObsID can lead to redundancy in
    *    the resulting entries.
    * 2. Sort based on a value determined by their string representation.
    *    Example would be:
    *       RinexCode: [t][b][c]
    *       value = b * 1000 + t * 100 + c;
    * 3. Trim off codes that have the same band/type but different codes
    *    Attempt to keep only the prefered ones in the obsMap object.
    *    If not possible, use the first code that appears.
    * 4. Store resulting sorted list of unique R3 identifiers in the Header.
    *    Because the obsTypeList member isn't really used in the new Obs
    *    Header object, store it there and hope no one else overwrites it.
    */
}

} // end namespace gpstk
