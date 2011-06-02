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

#include <cmath>
#include <algorithm>       // find

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

RinexConverter::ValidCodes RinexConverter::validGPScodes;
RinexConverter::ValidCodes RinexConverter::validGLOcodes;
RinexConverter::ValidCodes RinexConverter::validGALcodes;
RinexConverter::ValidCodes RinexConverter::validGEOcodes;

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

   validGPScodes.insert("C1");
   validGPScodes.insert("C2");
   validGPScodes.insert("C5");
   validGPScodes.insert("P1");
   validGPScodes.insert("P2");
   validGPScodes.insert("L1");
   validGPScodes.insert("L2");
   validGPScodes.insert("L5");
   validGPScodes.insert("D1");
   validGPScodes.insert("D2");
   validGPScodes.insert("S1");
   validGPScodes.insert("S2");
   validGPScodes.insert("S5");

   validGLOcodes.insert("C1");
   validGLOcodes.insert("C2");
   validGLOcodes.insert("P1");
   validGLOcodes.insert("P2");
   validGLOcodes.insert("L1");
   validGLOcodes.insert("L2");
   validGLOcodes.insert("D1");
   validGLOcodes.insert("D2");
   validGLOcodes.insert("S1");
   validGLOcodes.insert("S2");

   validGALcodes.insert("C1");
   validGALcodes.insert("C5");
   validGALcodes.insert("C6");
   validGALcodes.insert("C7");
   validGALcodes.insert("C8");
   validGALcodes.insert("L1");
   validGALcodes.insert("L5");
   validGALcodes.insert("L6");
   validGALcodes.insert("L7");
   validGALcodes.insert("L8");
   validGALcodes.insert("D1");
   validGALcodes.insert("D5");
   validGALcodes.insert("D6");
   validGALcodes.insert("D7");
   validGALcodes.insert("D8");
   validGALcodes.insert("S1");
   validGALcodes.insert("S5");
   validGALcodes.insert("S6");
   validGALcodes.insert("S7");
   validGALcodes.insert("S8");

   validGEOcodes.insert("C1");
   validGEOcodes.insert("C5");
   validGEOcodes.insert("L1");
   validGEOcodes.insert("L5");
   validGEOcodes.insert("D1");
   validGEOcodes.insert("D5");
   validGEOcodes.insert("S1");
   validGEOcodes.insert("S5");

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

   // Convert the Aux Header, if one exists.

   if (src.epochFlag > 0 && src.epochFlag < 6)
      convertToRinex3(dest.auxHeader, src.auxHeader);

   /// Clear the Obs list in the R3 object passed.

   dest.obs.clear();

   /// Iterator to walk over the observation data in the source data.

   RinexObsData::RinexSatMap::const_iterator iter = src.obs.begin();
   if ( iter == src.obs.end() ) return false; // fail & return if no map

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

   if (int(src.version) == 2)
   {
      dest.version         = 3.0;
   }
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
   dest.antennaDeltaHEN = src.antennaOffset;
   dest.firstObs        = src.firstObs;

   /// Marker Type does not exist in RINEX 2.11.
   /// If the user has defined the markerType string, use it.
   /// Otherwise, default to NON_GEODETIC for low-precision fixed receiver.

   /// Antenna Position will always be marked as "NOT valid".  In RINEX 3
   /// it's optional for a moving receiver, so we wrote the RINEX3ObsHeader
   /// logic to ignore a check for its existence.

   /// Ignore all warnings about valid or not valid in header dump.
   /// This code merely translates RINEX 2 to RINEX 3.  It does not
   /// determine if a RINEX file conforms to the standard, nor should it.

   if (markerType.length() > 0)
      dest.markerType   = markerType;
   else
      dest.markerType   = "NON_GEODETIC";

#ifdef DEBUG
   cout << endl;
   cout << "RinexConverter:convertToRinex3(header): destination header info" << endl;
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
   cout << " validVersion: " << dest.validVersion    << endl;
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
      /// Get the old type.

      currCode = oldTypeList[i].type;

#ifdef DEBUG
      cout << "   Current Code: " << currCode << endl;
#endif

      /// Get the replacement string from the static map.

      mapIter = obsMap.begin();
      if (mapIter == obsMap.end())
      {
#ifdef DEBUG
         cout << "      Couldn't find code " << currCode << endl;
#endif
         continue; // terminates the outer FOR loop
      }
      for ( ; mapIter != obsMap.end(); ++mapIter)
      {
#ifdef DEBUG
         cout << "      Checking <" << mapIter->first << "," << mapIter->second << ">" << endl;
#endif
         if (mapIter->first == currCode) break; // terminates this inner FOR loop
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

   /// Done if not doing optional fields, so return.

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

   /// Declare iterator over the optional numObsForSat data.

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

   // TODO::No consistency in 'validBits' definition between R2 and R3. 
   // Thus, this leads to not writing the R3 header
   dest.valid = src.valid;                // perhaps needs item check
   dest.valid |= dest.validSystemObsType; // necessary since not in R2

   return true;
}

/// ***** The Rinex 3 -> 2 methods are not finished. *****
/// 2009-12-10 OA: This is a very basic implementation. Check after TODO
///                Use it with CAUTION!!!

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

   /// OA implementation starts
   Rinex3ObsData::DataMap::const_iterator it;
   for (it = src.obs.begin(); it != src.obs.end(); it++)
   {

      // get satellite
      RinexSatID satid = it->first;

      // extract the corresponding satellite system
      string satSysTemp = satid.toString().substr(0,1);

      //and its R3 data
      vector<Rinex3ObsData::RinexDatum> r3data= it->second;

      // Now let's see what obs type are for this satellite system
      std::vector<ObsID> obsTypeList = srcHeader.mapObsTypes.find(satSysTemp)->second;

      // and loop through them
       std::vector<ObsID>::size_type iObs;
      for( iObs=0; iObs < obsTypeList.size();iObs++ )
      {

         // R3 code as a three character string (see ObsId.hpp, line 204)
         string currCode = obsTypeList[iObs].asRinex3ID();

         // and its R2 corresponding
         string replacement;

         // Iterator to find the current code.
         CodeMap::const_iterator mapIter = obsMap.begin();
         for ( ; mapIter != obsMap.end(); ++mapIter)
         {
            if (mapIter->second == currCode) break; // terminates this inner FOR loop
         }

         // nothing was found?, then grab the first two characters
         if ( mapIter == obsMap.end())
         {
            replacement = currCode.substr(0,2);
         }
         else
         {
            replacement = mapIter->first; 
         }

         // construct a R2 obs type
         RinexObsHeader::RinexObsType rot = RinexObsHeader::convertObsType(replacement);

         // declare a R2 data object
         RinexObsData::RinexDatum r2data;

         // copy R3 to R2
         r2data.data  = r3data[iObs].data;
         r2data.lli   = r3data[iObs].lli;
         r2data.ssi   = r3data[iObs].ssi;

         // now we can insert it
         dest.obs[satid][rot]=r2data;

      }  // End of 'for( iObs=0; iObs < obsTypeList.size();iObs++ )'

   }  // End of 'for (it = src.obs.begin(); ...'
   ///OA implementation ends here

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

   // very basic convertion from R3 ObsTypeList to R2 ObsTypeList
   map<std::string,vector<ObsID> >::const_iterator itSys = src.mapObsTypes.begin();
   for( ; itSys != src.mapObsTypes.end(); itSys++ )
   {

      vector<ObsID> vecObs = itSys->second;
      vector<ObsID>::const_iterator itObs = vecObs.begin();
      for( ; itObs != vecObs.end(); itObs++ )
      {

         // R3 obs as a three character string
         string currCode = itObs->asRinex3ID();

         // and its R2 corresponding
         string replacement;

         // Iterator to find the current code.
         CodeMap::const_iterator mapIter = obsMap.begin();
         for ( ; mapIter != obsMap.end(); ++mapIter)
         {
            if (mapIter->second == currCode) break; // terminates this inner FOR loop
         }

         // nothing was found?, then grab the first two characters
         if ( mapIter == obsMap.end())
         {
            replacement = currCode.substr(0,2);
         }
         else
         {
            replacement = mapIter->first; 
         }

         // construct a R2 obs type
         RinexObsHeader::RinexObsType rot = RinexObsHeader::convertObsType(replacement);

         // look if this R2 obs type has been already found
         vector<RinexObsHeader::RinexObsType>::iterator itType;
         itType = std::find(dest.obsTypeList.begin(), dest.obsTypeList.end(), rot);

         //if NOT, then insert it
         if( itType == dest.obsTypeList.end() ) dest.obsTypeList.push_back(rot);

      }  // End of 'for( ; itObs != vecObs.end(); itObs++ )'

   }  // End of 'for( ; itSys != src.mapObsTypes.end(); itSys++ )'

   dest.valid = 1;

   // Let's set the REQUIRED valid bits
   if (src.valid & src.validVersion        ) dest.valid |= dest.versionValid;
   if (src.valid & src.validRunBy          ) dest.valid |= dest.runByValid;
   if (src.valid & src.validMarkerName     ) dest.valid |= dest.markerNameValid;
   if (src.valid & src.validObserver       ) dest.valid |= dest.observerValid;
   if (src.valid & src.validReceiver       ) dest.valid |= dest.receiverValid;
   if (src.valid & src.validAntennaType    ) dest.valid |= dest.antennaTypeValid;
   if (src.valid & src.validAntennaPosition) dest.valid |= dest.antennaPositionValid;
   if (src.valid & src.validAntennaDeltaHEN) dest.valid |= dest.antennaOffsetValid;
   dest.valid |= dest.waveFactValid; // necessary since not in R3
   if (src.valid & src.validSystemObsType  ) dest.valid |= dest.obsTypeValid;
   if (src.valid & src.validFirstTime      ) dest.valid |= dest.firstTimeValid;
   if (src.valid & src.validEoH            ) dest.valid |= dest.endValid;

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

   // now let's set the OPTIONAL valid bits (if any)
   if (src.valid & src.validComment          ) dest.valid |= dest.commentValid;
   if (src.valid & src.validMarkerNumber     ) dest.valid |= dest.markerNumberValid;
   if (src.valid & src.validInterval         ) dest.valid |= dest.intervalValid;
   if (src.valid & src.validLastTime         ) dest.valid |= dest.lastTimeValid;
   if (src.valid & src.validReceiverOffset   ) dest.valid |= dest.receiverOffsetValid;
   if (src.valid & src.validLeapSeconds      ) dest.valid |= dest.leapSecondsValid;
   if (src.valid & src.validNumSats          ) dest.valid |= dest.numSatsValid;
   if (src.valid & src.validPrnObs           ) dest.valid |= dest.prnObsValid;

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
   if (validGPScodes.find(code.type) != validGPScodes.end())
   {
#ifdef DEBUG
      cout << "      Valid Code for system G" << endl;
#endif
      return true;
   }
   return false;
}

bool RinexConverter::validGLOcode(const RinexObsHeader::RinexObsType& code)
{
   if (validGLOcodes.find(code.type) != validGLOcodes.end())
   {
#ifdef DEBUG
      cout << "      Valid Code for system R" << endl;
#endif
      return true;
   }
   return false;
}

bool RinexConverter::validGALcode(const RinexObsHeader::RinexObsType& code)
{
   if (validGALcodes.find(code.type) != validGALcodes.end())
   {
#ifdef DEBUG
      cout << "      Valid Code for system E" << endl;
#endif
      return true;
   }
   return false;
}

bool RinexConverter::validGEOcode(const RinexObsHeader::RinexObsType& code)
{
   if (validGEOcodes.find(code.type) != validGEOcodes.end())
   {
#ifdef DEBUG
      cout << "      Valid Code for system S" << endl;
#endif
      return true;
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
