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

/// @file Rinex3ObsFileLoader.cpp  Read a RINEX3 file, selecting, counting and saving
/// obs data, then allow caller to access that data easily.

//------------------------------------------------------------------------------------
// system includes
#include <iostream>

// GPSTk
#include "Exception.hpp"
#include "StringUtils.hpp"
#include "TimeString.hpp"        // printTime
#include "GPSWeekSecond.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsData.hpp"
#include "Rinex3ObsStream.hpp"
#include "MostCommonValue.hpp"

// geomatics
#include "Rinex3ObsFileLoader.hpp"
#include "GSatID.hpp"            // for SatPass
#include "logstream.hpp"         // TEMP

using namespace std;

namespace gpstk
{

//------------------------------------------------------------------------------------
const double Rinex3ObsFileLoader::dttol(0.001);

//------------------------------------------------------------------------------------
// Read the files already defined
// param[out] errmsg an error/warning message, blank for success
// param[out] msg an informative message
// return 0 ok, >0 number of files read
int Rinex3ObsFileLoader::loadFiles(string& errmsg, string& msg)
{
try {
   int nint;
   unsigned int i,j;
   double dt;
   string str;
   // Rinex3ObsHeader from Rinex3ObsHeader class GPSTk class
   // roh = rinex obs header
   Rinex3ObsHeader roh;
   // Rinex3ObsData from Rinex3ObsData class in GPSTk
   // rod = rinex obs data, and outrod = output rinex obs data?
   Rinex3ObsData rod, outrod;
   vector<string>::const_iterator vit;
   map< RinexSatID, vector<int> >::iterator soit;     // SatObsCountMap
   ostringstream oss, ossx;

   prevtime = CommonTime::BEGINNING_OF_TIME;
   // setTimeSystem sets the method for internal variable m_timeSystem
   prevtime.setTimeSystem(TimeSystem::Any);

   // read the files
   // initialize number read counter to zero
   int nread(0);
   // loop over file names
   for(unsigned int nf=0; nf<filenames.size(); nf++) {
      // set the file name to a particular string
      string filename(filenames[nf]);
      // strip any blank values from beginning of file name
      StringUtils::stripLeading(filename);
      // strip any blank values from end of file name
      StringUtils::stripTrailing(filename);
      // If the file name list is empty, then an error in the file name
      if(filename.empty()) {
         oss << "Error - file name " << nf+1 << " is blank";
         continue;
      }

      // declare and initialize onOrder to false
      bool onOrder(false);
      vector<int> nOrder;
      vector<CommonTime> timeOrder;

      // read one file
      for(;;) {
         // open file ---------------------------------------------
         // converts file name from a string to a vector of characters using c_str
         Rinex3ObsStream strm(filename.c_str());
         // if the obs stream is not successfully opened
         if(!strm.is_open()) {
            oss << "Error - could not open file " << filename << endl;
            break;
         }
         strm.exceptions(fstream::failbit);

         // read header -------------------------------------------
         try {
            strm >> roh;

            // update list of wanted obs types
            // create iterator for looping through roh.mapObsTypes
            // make it a const_iterator, so that it can be used for access only,
            // and cannot be used for modification
            // the roh.mapObsTypes is of type RinexObsMap, which has format
            // map<string,RinexObsVec> == map<string,vector<RinexObsID>>
            // A map where string values are the keys and vector<RinexObsID>
            // are the values mapped to those strings
            map<string,vector<RinexObsID> >::const_iterator kt;
            for(kt = roh.mapObsTypes.begin(); kt != roh.mapObsTypes.end(); kt++) {
               for(i=0; i<kt->second.size(); i++) {
                  // need 4-char string version of ObsID
                  string sys = kt->first;                // system
                  string rot = kt->second[i].asString(); // 3-char id
                  string srot = sys + rot;               // 4-char id

                  // is this ObsID Wanted? and should it be added?
                  // NB RinexObsID::operator==() handles '*' but does not compare sys
                  // NB input (loadObsID()) checks validity of ObsIDs

                  for(j=0; j<inputWantedObsTypes.size(); j++) {
                     // Guessing wsrot = wanted string rinex obs type
                     string wsrot(inputWantedObsTypes[j]);
                     string wsys(wsrot.substr(0,1));
                     string wrot(wsrot.substr(1,3));

                     // if sys and rot match, and srot is not found, add it
                     if(((wsys == "*" &&
                          RinexObsID(wrot, roh.version) ==
                          RinexObsID(rot, roh.version)) ||
                         (wsys == sys &&
                          RinexObsID(wsrot, roh.version) ==
                          RinexObsID(srot, roh.version))) &&
                        vectorindex(wantedObsTypes,srot) == -1)
                     {
                        wantedObsTypes.push_back(srot);  // add it
                        // the number of observations for each observation type
                        countWantedObsTypes.push_back(0);
                        
                        ossx << " Add obs type " << srot
                           << " =~ " << inputWantedObsTypes[j]
                           << " from " << filename << endl;
                     }
                  }
               }
            }  // end loop over obs types in header

            // must keep SatObsCountMap vectors parallel to wantedObsTypes
            if(SatObsCountMap.size() > 0) {                       // table exists
               vector<int> v(SatObsCountMap.begin()->second);
               j = wantedObsTypes.size();
               if(v.size() < j) {                                 // vectors are short
                  soit = SatObsCountMap.begin();
                  for( ; soit != SatObsCountMap.end(); ++soit)
                     soit->second.resize(j , 0);                  // extend with zeros
               }
            }

            headers.push_back(roh);
         }
         catch(Exception& e) {
            oss << "Error - failed to read header for file " << filename
               << " with exception " << e.getText(0) << endl;
            strm.close();
            break;
         }

         // loop over epochs --------------------------------------
         // while(1) always true, so only breaks out of loop with a break statement
         while(1) {
            try {
               strm >> rod;
               rod.time.setTimeSystem(TimeSystem::Any);
            }
            catch(Exception& e) {
               oss << "Error - failed to read data in file " << filename
                  << " with exception " << e.getText(0) << endl;
               break;
            }

            // EOF or error
            if(strm.eof() || !strm.good()) break;

            // skip aux header, etc
            if(rod.epochFlag != 0 && rod.epochFlag != 1) continue;

            // decimate to dtdec-even sec-of-week
            if(dtdec > 0.0) {
               double sow(static_cast<GPSWeekSecond>(rod.time).sow);
               //LOG(INFO) << fixed << setprecision(3) << "Dt " << dt
               //<< " dtdec " << dtdec << " sow " << sow
               //<< " test " << ::fabs(sow-dtdec*long(0.5+sow/dtdec));
               if(::fabs(sow - dtdec*long(0.5+sow/dtdec)) > 0.5)
                  continue;
            }

            // consider timestep
            if(prevtime != CommonTime::BEGINNING_OF_TIME) {
               // compute time since the previous epoch
               dt = rod.time - prevtime;

               if(dt >= dttol) {      // positive dt only
                  // add to the timestep estimator
                  mcv.add(dt);
               }
               else if(dt < dttol) {  // negative, and positive but tiny (< dttol)
                  //if(isLenient) {
                     if(!onOrder) {
                        nOrder.push_back(0);
                        timeOrder.push_back(prevtime);
                        onOrder = true;
                     }
                     nOrder[nOrder.size()-1]++;
                     continue;
                  //}
                  //GPSTK_THROW(Exception(string("Records out of time order: dt ")
                  //   + StringUtils::asString<double>(dt) + string(" at time ")
                  //   + printTime(rod.time,timefmt)));
               }
               onOrder = false;
            }

            // set previous time to current time
            prevtime = rod.time;
            // ignore data outside of time limits given by user
            if(rod.time < startTime) continue;
            if(rod.time > stopTime) break;
            if(rod.time < begDataTime) begDataTime = rod.time;
            if(rod.time > endDataTime) endDataTime = rod.time;

            // The integer number of epochs is advanced
            nepochs++;
            if(nepochsToRead > -1 && nepochs >= nepochsToRead) break;

            // prepare output rod
            outrod.time = rod.time;
            outrod.clockOffset = rod.clockOffset;
            outrod.epochFlag = rod.epochFlag;
            //?? outrod.auxHeader.clear();
            outrod.numSVs = 0;
            outrod.obs.clear();

            // loop over satellites, counting data per ObsID
            //vector<SatID> toSkip;
            Rinex3ObsData::DataMap::const_iterator it;
            for(it=rod.obs.begin(); it != rod.obs.end(); ++it) {
               // Create new RinexSatID variable called sat, initialize with the
               // it->first pointer from rod.obs current iteration
               RinexSatID sat(it->first);

               // is the sat excluded?  NB it does not exclude sat=(sys,-1)
               if(exSats.size() > 0 &&
                  find(exSats.begin(), exSats.end(), sat) != exSats.end())
                     continue;

               // Exract GNSS system from sat ID, creating new string variable sys
               string sys(sat.toString().substr(0,1));
               // Extract the observation types from the rinex obs header object
               // roh corresponding to the GNSS system from the sat ID, creating
               // new RinexObsID vector types
               const vector<RinexObsID> types(roh.mapObsTypes[sys]);

               // loop over obs
               for(i=0; i<it->second.size(); i++) {
                  // if the obs data is equal to zero, then do not consider that
                  // obs value (equivalent to missing data)
                  if(it->second[i].data == 0.0) continue;   // don't count missing

                  // combine the system and obs type into total rinex obs ID
                  string srot = sys + types[i].asString();  // 4-char RinexObsID

                  // is it wanted? nint is the index into
                  // wantedObsTypes, SatObsCountMap and outrod.obs
                  // vectorindex returns the index of the value srot in wantedObsTypes
                  // if it doesn't exist in that vector, return -1
                  nint = vectorindex(wantedObsTypes,srot);
                  if(nint == -1) continue;

                  // count the sat/obs
                  // map<RinexSatID, std::vector<int>>
                  soit = SatObsCountMap.find(sat);
                  if(soit == SatObsCountMap.end()) {           // add the sat
                     vector<int> v(wantedObsTypes.size(),0);   // keep parallel
                     SatObsCountMap[sat] = v;                  // creates map entry
                  }
                  SatObsCountMap[sat][nint]++;
                  countWantedObsTypes[nint]++;

                  // add it to outrod
                  if(saveData) {
                     // if the satellite is not in outrod.obs
                     if(outrod.obs.find(sat) == outrod.obs.end()) {
                        vector<RinexDatum> v(wantedObsTypes.size());
                        outrod.obs[sat] = v;
                        outrod.numSVs++;
                     }
                     outrod.obs[sat][nint] = it->second[i];
                  }
               }
            }

            // if saving data, save
            if(saveData && outrod.obs.size() > 0) datastore.push_back(outrod);

         }  // end loop over epochs

         // time steps
         rawdt = mcv.bestDT();
         nominalDT = (dtdec > 0.0 ? (dtdec > rawdt ? dtdec : rawdt) : rawdt);

         strm.close();

         break;      // mandatory
      }  // end for(;;)

      nread++;

      // warn of time order problems
      if(timeOrder.size() > 0) {
         for(i=0; i<timeOrder.size(); i++)
            oss << "Warning - in file " << filename << " " << nOrder[i]
               << " data records following epoch "
               << printTime(timeOrder[i],timefmt) << " are out of time order" << endl;
      }

      if(nepochsToRead > -1 && nepochs >= nepochsToRead) break;

   }  // end loop over files

   if(!errmsg.empty()) errmsg += string("\n");
   errmsg += oss.str();
   if(!errmsg.empty()) {
      StringUtils::stripTrailing(errmsg,'\n');
      StringUtils::stripTrailing(errmsg,'\r');
   }
   msg = ossx.str();
   if(!msg.empty()) {
      StringUtils::stripTrailing(msg,'\n');
      StringUtils::stripTrailing(msg,'\r');
   }

   return nread;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
string Rinex3ObsFileLoader::asString(void)
{
   int i;
   static const string longfmt("%04Y/%02m/%02d %02H:%02M:%02S %4F %10.3g");
   ostringstream oss;
   oss << "Summary of input RINEX obs data files (" << filenames.size() << "):\n"; 
   for(i=0; i<filenames.size(); i++)
      oss << (i==0 ? " RINEX obs file: " : "                 ")
         << filenames[i] << endl;
   oss << " Interval " << fixed << setprecision(2) << getDT() << "sec, obs types";
   for(i=0; i<wantedObsTypes.size(); i++) oss << " " << wantedObsTypes[i];
   oss << ", store size " << datastore.size();
   oss << "\n";
   oss << " Time limits: begin  " << printTime(begDataTime,longfmt) << "\n"
       << "                end  " << printTime(endDataTime,longfmt) << "\n";

   dumpSatObsTable(oss);

   oss << "End of summary";

   return oss.str();
}

//------------------------------------------------------------------------------------
// Write the stored data to a list of SatPass objects, given a vector of obstypes
// and (for each system) a parallel vector of indexes into the Loader's ObsIDs
// (getWantedObsTypes()), and a vector of SatPass to be written to.
// SPList need not be empty; however if not empty, obstypes must be identical to
// those of existing SatPasses.
// param[in] sysSPOT map of <sys,vector<ObsID>> for SatPass (2-char obsID)
// param[in] indexLoadOT map<char,vector<int>> with key=system char,
//    value=vector parallel to obstypes with elements equal to
//    {index in loader's ObsIDs for each obstype, or -1 if not in loader}
// param[in,out] SPList vector of SatPass to which data store is written
// return >0 number of passes created, -1 inconsistent input, -2 obstypes
// inconsistent with existing SatPass, -3 Loader not configured to save data, 
// -4 no data -5 obstypes not provided for all systems
int Rinex3ObsFileLoader::WriteSatPassList(const map<char, vector<string> >& sysSPOT,
                                          const map<char, vector<int> >& indexLoadOT,
                                          vector<SatPass>& SPList)
{
try {
   if(!dataSaved()) return -3;
   if(datastore.size() == 0) return -4;

   char sys;
   int npass(0);
   unsigned int i;
   unsigned short flag;
   GSatID sat;
   map<GSatID,unsigned int> indexForSat;
   // satellite iterator
   map<GSatID,unsigned int>::const_iterator satit;
   // observation iterator
   map<char,vector<string> >::const_iterator obsit;

   // add to existing SPList
   if(SPList.size() > 0) {
      // sort existing list on time - this probably already done
      std::sort(SPList.begin(),SPList.end());

      // fill index array using SPList - later ones overwrite earlier ones
      for(i=0; i<SPList.size(); i++)
         indexForSat[SPList[i].getSat()] = i;
   }

   // for use in putting data into SatPass
   // initialize observation iterator
   obsit = sysSPOT.begin();
   // initialize/determine number of observations, from input sysSPOT map
   const int nobs(obsit->second.size());
   vector<double> data(nobs,0.0);
   vector<unsigned short> ssi(nobs,0), lli(nobs,0);

   // loop over the data store = vector<Rinex3ObsData>
   for(unsigned int nds=0; nds<datastore.size(); nds++) {

      //LOG(INFO) << "WriteSPL " << printTime(datastore[nds].time,timefmt)
      //<< " Nsats " << datastore[nds].obs.size();
      //dumpStoreEpoch(LOGstrm,datastore[nds]);

      // loop over satellites
      Rinex3ObsData::DataMap::const_iterator it;
      map<char, vector<int> >::const_iterator jt;
      for(it = datastore[nds].obs.begin(); it != datastore[nds].obs.end(); ++it) {
         sys = it->first.systemChar();
         jt = indexLoadOT.find(sys);
         if(jt == indexLoadOT.end())      // skip unwanted system
            continue;
         sat = GSatID(it->first);         // converts from RinexSatID

         // get obstypes for this sys
         obsit = sysSPOT.find(sys);
         if(obsit == sysSPOT.end())       // sysSPOT not found for system sys
            return -5;

         // pull data out of store and put in arrays
         flag = SatPass::OK;
         for(i=0; i<jt->second.size(); i++) {
            int ind = jt->second[i];
            //LOG(INFO) << " ind " << ind << " SPOT " << obsit->second[i]
            //<< " R3OT " << (ind >= 0 ? wantedObsTypes[ind]:"NA")
            //<< " sat " << sat
            //<< " data " << fixed << setprecision(4)
            //<< (ind >= 0 ? it->second[ind].data : 0.0) << "/"
            //<< (ind >= 0 ? it->second[ind].ssi : 0) << "/"
            //<< (ind >= 0 ? it->second[ind].lli : 0);
            if(ind < 0) {
               data[i] = 0.0;
               ssi[i] = lli[i] = 0;
               // don't flag BAD as there may be empty obs types in this SatPass
            }
            else {
               data[i] = it->second[ind].data;
               ssi[i] = it->second[ind].ssi;
               lli[i] = it->second[ind].lli;
               // NB so one bad obs makes the sat/epoch bad
               // TD does loader keep epochs with no good data?
               if(::fabs(data[i]) < 1.e-8) flag = SatPass::BAD;
            }
         }

         // find the current SatPass for this sat
         satit = indexForSat.find(sat);
         if(satit == indexForSat.end()) {       // create a new one
            SatPass newSP(sat,nominalDT,obsit->second);
            SPList.push_back(newSP);
            npass++;
            indexForSat[sat] = SPList.size()-1;
            satit = indexForSat.find(sat);
         }

         // add the data to the SatPass
         do {
            i = SPList[satit->second].addData(
                  datastore[nds].time, obsit->second, data, lli, ssi, flag);

            if(i == -1) {        // there was a gap - break into two passes
               SatPass newSP(sat,nominalDT,obsit->second);
               SPList.push_back(newSP);
               npass++;
               indexForSat[sat] = SPList.size()-1;
               satit = indexForSat.find(sat);
            }

         } while(i == -1);       // will iterate only once, if there is a gap

      }  // end loop over satellites

   }  // end loop over data store

   return npass;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// Dump the SatObsCount table
// param ostream s to which to write the table
void Rinex3ObsFileLoader::dumpSatObsTable(ostream& s) const
{
   unsigned int i;

   // dump the obs types
   s << "Table of Sat/Obs counts\n    ";
   for(i=0; i<wantedObsTypes.size(); i++)
      s << " " << setw(5) << wantedObsTypes[i];
   s << endl;
   
   // dump the counts
   map<RinexSatID, vector<int> >::const_iterator soit;
   for(soit = SatObsCountMap.begin(); soit != SatObsCountMap.end(); ++soit) {
      s << " " << soit->first;
      for(i=0; i<soit->second.size(); i++)
         s << " " << setw(5) << soit->second[i];
      s << endl;
   }

   s << " TOT";
   for(i=0; i<countWantedObsTypes.size(); i++)
      s << " " << setw(5) << countWantedObsTypes[i];
   s << " total " << nepochs << " epochs" << endl;
}

//------------------------------------------------------------------------------------
// Dump the stored data at one epoch - NB setTimeFormat()
// param ostream s to which to write
void Rinex3ObsFileLoader::dumpStoreEpoch(ostream& s, const Rinex3ObsData& rod) const
{
   s << "Dump of Rinex3ObsData" << " at "
      << printTime(rod.time,timefmt)
      << " epochFlag = " << rod.epochFlag
      << " numSVs = " << rod.numSVs << fixed << setprecision(9)
      << " clk offset = " << rod.clockOffset << endl;

   if(rod.epochFlag == 0 || rod.epochFlag == 1) {
      Rinex3ObsData::DataMap::const_iterator jt;
      for(jt=rod.obs.begin(); jt!=rod.obs.end(); jt++) {
         s << " " << jt->first.toString() << ":"
            << fixed << setprecision(3);
         for(unsigned int j=0; j<jt->second.size(); j++) {
            s << " " << setw(13) << jt->second[j].data
                     << "/" << jt->second[j].lli << "/" << jt->second[j].ssi
                     << "/" << wantedObsTypes[j];
         }
         s << endl;
      }
   }
   else {
      s << "aux. header info:" << endl;
      rod.auxHeader.dump(s);
   }
}

//------------------------------------------------------------------------------------
// Dump the stored data - NB setTimeFormat()
// param ostream s to which to write
void Rinex3ObsFileLoader::dumpStoreData(ostream& s) const
{
   s << "\nDump the ROFL data(" << datastore.size() << "):" << endl;
   for(unsigned int i=0; i<datastore.size(); i++) {
      const Rinex3ObsData& rod(datastore[i]);
      dumpStoreEpoch(s,rod);
   }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// dump a table of all valid RinexObsIDs
// param ostream s to which to write the table
void dumpAllRinex3ObsTypes(ostream& os)
{
   using namespace gpstk::StringUtils;

   // windows compiler truncates long names (!)
   typedef map<string, map<string, map<string, map<char,string> > > > tableMap;
   typedef map<string, map<string, map<char,string> > > obsMap;
   typedef map<string, map<char,string> > codeMap;

   vector<string> goodtags;
   string syss(ObsID::validRinexSystems);
   // build a table: table[sys][band][codedesc][type] = 4-char ObsID;
   //                      char cb..  tc..      ot..
   tableMap table;
   for(size_t s=0; s<syss.size(); s++)
      for(int j=ObsID::cbAny; j<ObsID::cbUndefined; ++j)
         for(int k=ObsID::tcAny; k<ObsID::tcUndefined; ++k)
            for(int i=ObsID::otAny; i<ObsID::otUndefined; ++i)
               try {
                  string tag(string(1,syss[s]) +
                             string(1,ObsID::ot2char[ObsID::ObservationType(i)]) +
                             string(1,ObsID::cb2char[ObsID::CarrierBand(j)]) +
                             string(1,ObsID::tc2char[ObsID::TrackingCode(k)]));
                  ObsID obs(tag, Rinex3ObsBase::currentVersion);
                  string name(asString(obs));
                  if(name.find("Unknown") != string::npos ||
                     name.find("undefined") != string::npos ||
                     name.find("Any") != string::npos ||
                     !isValidRinexObsID(tag)) continue;

                  if(find(goodtags.begin(),goodtags.end(),tag) == goodtags.end()) {
                     goodtags.push_back(tag);
                     string sys(RinexSatID(string(1,tag[0])).systemString3());
                     char type(ObsID::ot2char[ObsID::ObservationType(i)]);
                     string id(tag); // TD keep sys char ? id(tag.substr(1));
                     string desc(
                        asString(ObsID(tag, Rinex3ObsBase::currentVersion)));
                     vector<string> fld(split(desc,' '));
                     string codedesc(fld[1].substr(syss[s]=='S'?4:3));
                     string band(fld[0]);
                     table[sys][band][codedesc][type] = id;
                  }
               }
               catch(InvalidParameter& ) { continue; }

   tableMap::iterator it;
   obsMap::iterator jt;
   codeMap::iterator kt;
   // find field lengths
   size_t len2(4),len3(5),len4(6);  // 3-char len4(7);        // 4-char
   for(it=table.begin(); it!=table.end(); ++it)
      for(jt=it->second.begin(); jt!=it->second.end(); ++jt)
         for(kt=jt->second.begin(); kt!=jt->second.end(); ++kt) {
            if(jt->first.length() > len2) len2 = jt->first.length();
            if(kt->first.length() > len3) len3 = kt->first.length();
         }

   string fres(ObsID::validRinexFrequencies);
   os << "\nAll valid RINEX3 systems   : " << syss << endl;
   os << "All valid RINEX3 frequencies : " << fres << endl;
   os << "All valid RINEX observation codes:" << endl;
                                 // (as sys+code = 1+3 char):";
   os << " Sys " << leftJustify("Freq",len2)
             << " " << center("Track",len3)
             << "Pseudo- Carrier Dopp  Signal" << endl;
   os << "     " << leftJustify("    ",len2)
             << " " << center("     ",len3)
             << " range  phase        Strength" << endl;

   // output loop
   // looping over all valid RINEX3 systems
   for(size_t i=0; i<syss.size(); ++i) {
      char fr('0');
      // Determine if this RINEX3 system is in the table
      it = table.find(RinexSatID(string(1,syss[i])).systemString3());
      // If not, skip this iteration of the loop
      if(it == table.end()) continue;
      // if past the first system listed, add a line break
      if(i > 0) os << endl;
      for(jt=it->second.begin(); jt!=it->second.end(); ++jt) {
         for(kt=jt->second.begin(); kt!=jt->second.end(); ++kt) {
            os << " " << it->first                   // eg. GPS
                    << " " << leftJustify(jt->first,len2) // eg. L1
                    << " " << center(kt->first,len3)      // eg. C/A
             << " " << center((kt->second['C']==""?"----":kt->second['C']),len4)
             << " " << center((kt->second['L']==""?"----":kt->second['L']),len4)
             << " " << center((kt->second['D']==""?"----":kt->second['D']),len4)
             << " " << center((kt->second['S']==""?"----":kt->second['S']),len4);

            if(fr != kt->second['L'][2]) {
               fr = kt->second['L'][2];
               string tc(ObsID::validRinexTrackingCodes[syss[i]][fr]);
               if(!tc.empty())
                  os << "  all codes for " << it->first << " " << jt->first
                           << " = '" << tc << "'";
            }
            os << endl;
         }
      }
   }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

} // end namespace gpstk
