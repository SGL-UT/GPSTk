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

/// @file SolarSystemEphemeris.cpp
/// Implement JPL solar system ephemeris handling, including reading and writing of
/// ASCII and binary files and the computation of position and velocity of the sun,
/// the moon and the 9 planets, plus nutation and lunar libration and their rate.
/// JPL ephemeris files may be obtained from ftp://ssd.jpl.nasa.gov/pub/eph/planets.
/// Generally you should download the ASCII files and use tools based on this code
/// to convert to your own binary files; this avoids compiler- and platform-dependent
/// differences in the binary files.

//------------------------------------------------------------------------------------
#include "SolarSystemEphemeris.hpp"
// GPSTk
#include "StringUtils.hpp"
#include "TimeConverters.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk {
//------------------------------------------------------------------------------------
void SolarSystemEphemeris::readASCIIheader(string filename) throw(Exception)
{
try {
   //cout << "Reporting in SolarSystemEphemeris is "
   //   << ConfigureLOG::ToString(ConfigureLOG::ReportingLevel()) << endl;

   // open the file
   ifstream strm;
   strm.open(filename.c_str());
   if(!strm.is_open()) {
      Exception e("Failed to open input file " + filename + ". Abort.");
      GPSTK_THROW(e);
   }

   // clear existing data
   constants.clear();

   // read the file one line at a time, process depending on the value of group
   int group=0,n=0;        // n will count lines/items within a group
   string line,word;
   vector<string> const_names;      // Temporary - name,value stored in map constants
   while(1) {
      getline(strm,line);
      stripTrailing(line,'\r');

      // catch new groups
      if(line.substr(0,5) == "GROUP") {
         word = stripFirstWord(line);
         group = asInt(stripFirstWord(line));
         LOG(DEBUG) << "Group is " << group; // temp
         n = 0;            // n will count lines/items within a group
         continue;
      }

      // skip blank lines
      stripLeading(line," ");
      if(line.empty()) {
         if(strm.eof() || !strm.good()) break;     // if the last line is blank
         else continue;
      }

      // process entire line at once
      // first line (no GROUP)
      if(group == 0) {
         word = stripFirstWord(line);
         word = stripFirstWord(line);        // ignore KSIZE
         word = stripFirstWord(line);
         if(word == "NCOEFF=") {
            Ncoeff = asInt(stripFirstWord(line));
            LOG(DEBUG) << "Ncoeff is " << Ncoeff; // temp
            continue;
         }
         else {
            Exception e("Confused on the first line - 3rd word is not NCOEFF=");
            GPSTK_THROW(e);
         }
      }
      // GROUP 1010
      else if(group == 1010) {
         if(n > 2) {                         // this should not happen
            Exception e("Too many labels under GROUP 1010");
            GPSTK_THROW(e);
         }
         else {
            stripTrailing(line," ");
            label[n++] = line;
            LOG(VERBOSE) << "label " << n << " is " << line; // temp
            continue;
         }
      }
      // GROUP 1030
      else if(group == 1030) {
         // start and stop times. These are meaningless here, because they will be
         // determined by the data that follows this header, and so are meaningful
         // only in the binary file.
         startJD = for2doub(stripFirstWord(line));
         endJD = for2doub(stripFirstWord(line));
         // interval in days covered by each block of coefficients
         interval = for2doub(stripFirstWord(line));
         LOG(VERBOSE) << "Times JD " << fixed << setprecision(3)
            << startJD << " to JD " << endJD << " = " << interval << " days";
      }
      // GROUP 1070 - end-of-header
      else if(group == 1070) {
         break;
      }

      // process the line one (whitespace-separated) word at a time
      while(!line.empty()) {
         word = stripFirstWord(line);

         if(group == 1040) {
            if(n++ == 0) {
               Nconst = asInt(word);
               LOG(DEBUG) << "Nconst is " << Nconst;
            }
            else {
               const_names.push_back(word);
            }
         }
         else if(group == 1041) {
            if(n++ == 0) {
               if(Nconst != asInt(word)) {
                  Exception e("Nconst does not match N in GROUP 1041 : " +
                              asString(Nconst) + " != " + word);
                  GPSTK_THROW(e);
               }
               LOG(DEBUG) << "Nconst matches: " << Nconst << " = " << word;
            }
            else
               constants[const_names[n-2]] = for2doub(word);
         }
         else if(group == 1050) {
            if(n < 13) {
               c_offset[n] = asInt(word);
               LOG(DEBUG) << "c_offset[" << n << "] = " << c_offset[n];
            }
            else if(n < 26) {
               c_ncoeff[n-13] = asInt(word);
               LOG(DEBUG) << "c_ncoeff[" << n-13 << "] = " << c_ncoeff[n-13];
            }
            else {
               c_nsets[n-26] = asInt(word);
               LOG(DEBUG) << "c_nsets[" << n-26 << "] = " << c_nsets[n-26];
            }
            n++;
         }
         else {
            Exception e("Confused about GROUP : " + asString(group));
            GPSTK_THROW(e);
         }
      }  // end loop over words

      if(strm.eof() || !strm.good()) break;     // if the last line is not blank

   }  // end read loop over lines

   strm.clear();
   strm.close();

   // test that we got a full header
   if(group != 1070) {
      Exception e("Premature end of header");
      GPSTK_THROW(e);
   }

   // EphemerisNumber != -1 means the header is complete
   EphemerisNumber = int(constants["DENUM"]);

   // clear the data arrays
   store.clear();
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}  // end void SolarSystemEphemeris::readASCIIheader(string filename) throw(Exception)

//------------------------------------------------------------------------------------
int SolarSystemEphemeris::readASCIIdata(vector<string>& filenames) throw(Exception)
{
try {
   int i,n;
   double jd;

   if(filenames.size() == 0) return 0;

   // read the files, in any order; the data map will be sorted on time
   for(i=0; i<filenames.size(); i++) {
      int iret = readASCIIdata(filenames[i]);
      if(iret) return iret;
   }

   // set the start and stop times in the header
   map<double,vector<double> >::iterator it = store.begin();
   startJD = it->second[0];
   it = store.end(); it--;
   endJD = it->second[1];

   LOG(VERBOSE) << "After reading data files, store size is " << store.size()
      << " and new start and stop times are JD " << fixed << setprecision(9)
      << startJD << " and JD " << endJD;

   // Mod the header labels to reflect the new time limits
   ostringstream oss;
   int yy,mm,dd;
   convertJDtoCalendar(static_cast<long>(startJD+0.5),yy,mm,dd);
   oss << "Start Epoch: JED= " << fixed << setw(10) << setprecision(1) << startJD
      << " = " << yy << "/" << mm << "/" << dd;
   label[1] = leftJustify(oss.str(),81);
   oss.seekp(ios_base::beg);

   convertJDtoCalendar(static_cast<long>(endJD+0.5),yy,mm,dd);
   oss.str("");
   oss << "Final Epoch: JED= " << fixed << setw(10) << setprecision(1) << endJD
      << " = " << yy << "/" << mm << "/" << dd;
   label[2] = leftJustify(oss.str(),81);

   LOG(VERBOSE) << "New label 1 is " << stripTrailing(label[1]);
   LOG(VERBOSE) << "New label 2 is " << stripTrailing(label[2]);

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}  // end int SolarSystemEphemeris::readASCIIdata(vector<string>& filenames)

//------------------------------------------------------------------------------------
int SolarSystemEphemeris::readASCIIdata(string filename) throw(Exception)
{
try {
   if(EphemerisNumber < 0) {
      Exception e("readASCIIdata called before header read");
      GPSTK_THROW(e);
   }

   int iret=0;
   string line,word;
   ifstream strm;

   // open the file
   strm.open(filename.c_str());
   if(!strm.is_open()) {
      Exception e("Could not open file " + filename);
      GPSTK_THROW(e);
   }

   // expect this many lines per record
   int nmax = Ncoeff/3 + (Ncoeff % 3 ? 1 : 0);

   // loop over lines in the file
   int ntot=0;                      // counts the total number of lines
   int n=0;                         // counts the lines within a set of coefficients
   int nc=0;                        // count coefficients within a record
   int rec;
   vector<double> data_vector;
   while(1) {
      getline(strm,line);
      stripTrailing(line,'\r');

      if(line.empty()) {
         if(strm.eof()) break;
         if(!strm.good()) { iret = -1; break; }
         continue;
      }

      if(n == 0) {
         rec = asInt(stripFirstWord(line));           // 1st word is the record number
         if(rec%25 == 0) { LOG(VERBOSE) << "Record number " << rec; }
         int ncc = asInt(stripFirstWord(line));       // 2nd word is ncoeff
         if(ncc != Ncoeff) {
            Exception e("readASCIIdata finds conflicting sizes in header ("
                  + asString(Ncoeff) + ") and data (" + asString(ncc) + ") in file "
                  + filename + " at line #" + asString(ntot));
            GPSTK_THROW(e);
         }
         nc = 0;
      }
      else {
         for(int j=0; j<3; j++) {
            double coeff = for2doub(stripFirstWord(line));
            nc++;
            data_vector.push_back(coeff);
            if(nc >= Ncoeff) {
               vector<double> dtemp=data_vector;
               store[data_vector[0]] = dtemp;
               data_vector.clear();
               break;
            }
         }
      }

      if(strm.eof()) break;
      if(!strm.good()) { iret = -1; break; }

      if(n == nmax) n=0; else n++;
      ntot++;
   }
   LOG(INFO) << "Read " << rec << " records from file " << filename;

   strm.close();

   return iret;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}  // end int SolarSystemEphemeris::readASCIIdata(string filename) throw(Exception)

//------------------------------------------------------------------------------------
int SolarSystemEphemeris::writeASCIIheader(ostream& os) throw(Exception)
{
try {
   if(EphemerisNumber < 0) return -4;

   int i;
   string str;
   string blank(81,' '); blank += string("\n");
   ostringstream oss;

   oss << "KSIZE= 0000    NSIZE=" << setw(5) << Ncoeff << blank;
   os << leftJustify(oss.str(),81) << endl << blank; oss.seekp(ios_base::beg);

   os << leftJustify("GROUP   1010",81) << endl << blank;
   for(i=0; i<3; i++) {
      str = label[i];
      os << leftJustify(str,81) << endl;
   }
   os << blank;

   os << leftJustify("GROUP   1030",81) << endl << blank;
   oss << fixed << setprecision(2) << setw(12) << startJD
      << setw(12) << endJD << setw(12) << interval << blank;
   os << leftJustify(oss.str(),81) << endl << blank; oss.seekp(ios_base::beg);

   os << leftJustify("GROUP   1040",81) << endl << blank;
   oss << setw(6) << Nconst << blank;
   os << leftJustify(oss.str(),81) << endl; oss.seekp(ios_base::beg);

   map<string,double>::const_iterator it=constants.begin();
   for(i=0; it != constants.end(); it++,i++) {
      oss << leftJustify("  " + it->first,8);
      if((i+1)%10 == 0) {
         oss << blank;
         os << leftJustify(oss.str(),81) << endl;
         oss.seekp(ios_base::beg);
      }
   }
   if(Nconst%10 != 0) {
      oss << blank;
      os << leftJustify(oss.str(),81) << endl;
      oss.seekp(ios_base::beg);
   }
   os << blank;

   os << leftJustify("GROUP   1041",81) << endl << blank;
   oss << setw(6) << Nconst << blank;
   os << leftJustify(oss.str(),81) << endl; oss.seekp(ios_base::beg);

   for(i=0,it=constants.begin(); it != constants.end(); it++,i++) {
      oss << leftJustify("  " + doub2for(it->second,24,2),26);
      if((i+1)%3 == 0) {
         oss << blank;
         os << leftJustify(oss.str(),81) << endl;
         oss.seekp(ios_base::beg);
      }
   }
   if(Nconst%3 != 0) {
      i--;
      while((i+1)%3 != 0) { oss << leftJustify("  " + doub2for(0.0,24,2),26); i++; }
      oss << blank;
      os << leftJustify(oss.str(),81) << endl;
      oss.seekp(ios_base::beg);
   }
   os << blank;

   os << leftJustify("GROUP   1050",81) << endl << blank;
   for(i=0; i<13; i++) oss << rightJustify(asString(c_offset[i]),6);
   oss << blank; os << leftJustify(oss.str(),81) << endl; oss.seekp(ios_base::beg);
   for(i=0; i<13; i++) oss << rightJustify(asString(c_ncoeff[i]),6);
   oss << blank; os << leftJustify(oss.str(),81) << endl; oss.seekp(ios_base::beg);
   for(i=0; i<13; i++) oss << rightJustify(asString(c_nsets[i]),6);
   oss << blank; os << leftJustify(oss.str(),81) << endl; oss.seekp(ios_base::beg);
   os << blank;

   os << leftJustify("GROUP   1070",81) << endl << blank;
   os << blank;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int SolarSystemEphemeris::writeASCIIdata(ostream& os) throw(Exception)
{
try {
   if(EphemerisNumber < 0) return -4;

   string blank(81,' '); blank += string("\n");
   int i,nrec;
   ostringstream oss;

   map<double,vector<double> >::iterator jt;
   for(nrec=1,jt=store.begin(); jt != store.end(); jt++,nrec++) {
      os << setw(6) << nrec << setw(6) << Ncoeff << " " << endl;
      for(i=0; i<Ncoeff; i++) {
         oss << leftJustify("  " + doub2for(jt->second[i],24,2),26);
         if((i+1)%3 == 0) {
            oss << blank;
            os << leftJustify(oss.str(),81) << endl;
            oss.seekp(ios_base::beg);
         }
      }
      if(Ncoeff % 3 != 0) {
         i--;
         while((i+1)%3 != 0) {
            oss << leftJustify("  " + doub2for(0.0,24,2),26);
            i++;
         }
         oss << blank;
         os << leftJustify(oss.str(),81) << endl;
         oss.seekp(ios_base::beg);
      }
   }

   // TD clear the array after writing
   //store.clear();

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int SolarSystemEphemeris::writeBinaryFile(string filename) throw(Exception)
{
try {
   int i,recLength;
   string str;

   if(EphemerisNumber <= 0) return -4;

   // open the output file
   ofstream strm;
   strm.open(filename.c_str(),ios::out | ios::binary);
   if(!strm.is_open()) {
      Exception e("Failed to open output file " + filename + ". Abort.");
      GPSTK_THROW(e);
   }

   // write the header records: two of them, both of length Ncoeff*sizeof(double)
   // the structure and ordering taken from the JPL code
   recLength = 0;

   // first header record -------------------------------------------------
   // 1. 3 labels each of length 84
   for(i=0; i<3; i++) {
      str = label[i];
      writeBinary(strm,leftJustify(str,84).c_str(),84);
      recLength += 84;
      LOG(VERBOSE) << "writeBinaryFile label " << i << " " << label[i];
   }

   // 2. 400 keys from the const array, each of length 6
   map<string,double>::const_iterator it=constants.begin();
   for(i=0; i<400; i++) {
      if(it != constants.end()) {
         str = it->first;
         writeBinary(strm,leftJustify(str,6).c_str(),6);
         it++;
      }
      else
         writeBinary(strm,"      ",6);
      recLength += 6;
   }

   // 3. the three times
   writeBinary(strm,(char *)&startJD,sizeof(double));
   writeBinary(strm,(char *)&endJD,sizeof(double));
   writeBinary(strm,(char *)&interval,sizeof(double));
   recLength += 3*sizeof(double);

   // 4. Ncoeff
   writeBinary(strm,(char *)&Ncoeff,sizeof(int));
   recLength += sizeof(int);

   // 5. AU and EMRAT
   writeBinary(strm,(char *)&constants["AU"],sizeof(double));
   writeBinary(strm,(char *)&constants["EMRAT"],sizeof(double));
   recLength += 2*sizeof(double);
   
   // 6. c_arrays for the first 12 planets
   for(i=0; i<12; i++) {
      writeBinary(strm,(char *)&c_offset[i],sizeof(int));
      writeBinary(strm,(char *)&c_ncoeff[i],sizeof(int));
      writeBinary(strm,(char *)&c_nsets[i],sizeof(int));
      recLength += 3*sizeof(int);
   }

   // 7. DENUM
   writeBinary(strm,(char *)&constants["DENUM"],sizeof(double));
   recLength += sizeof(double);
   LOG(VERBOSE) << "WriteBinary outputs DENUM " << constants["DENUM"];

   // 8. c_arrays for libration
   writeBinary(strm,(char *)&c_offset[12],sizeof(int));
   writeBinary(strm,(char *)&c_ncoeff[12],sizeof(int));
   writeBinary(strm,(char *)&c_nsets[12],sizeof(int));
   recLength += 3*sizeof(int);
  
   // 9. pad
   LOG(DEBUG) << "Pad length 1 = " << Ncoeff*sizeof(double)-recLength;
   char c=' ';
   for(i=0; i < Ncoeff*sizeof(double) - recLength; i++)
      writeBinary(strm,&c,1);

   // second header record -------------------------------------------------
   // the second header record: 400 values from the const array
   double z=0.0;
   it = constants.begin();
   for(i=0; i<400; i++) {
      if(it != constants.end()) {
         writeBinary(strm,(char *)&(it->second),sizeof(double));
         it++;
      }
      else
         writeBinary(strm,(char *)&z,sizeof(double));
   }
   LOG(DEBUG) << "Pad length 2 = " << (400-Nconst)*sizeof(double);
   for(i=0; i < (400-Nconst)*sizeof(double); i++)
      writeBinary(strm,&c,1);

   // data records ---------------------------------------------------------
   // the data, in time order
   int nrec=1;
   map<double,vector<double> >::iterator jt;
   for(jt=store.begin(); jt != store.end(); jt++) {
      LOG(DEBUG) << "writeBinaryFile writes " << nrec
                << " " << fixed << setprecision(6) << jt->second[0];
      for(i=0; i<jt->second.size(); i++)
         writeBinary(strm,(char *)&jt->second[i],sizeof(double));
      nrec++;
   }

   // TD after writing it out, clear the store array
   //store.clear();

   strm.clear();
   strm.close();

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int SolarSystemEphemeris::readBinaryFile(string filename) throw(Exception)
{
try {
   int iret;
   readBinaryHeader(filename);
   iret = readBinaryData(true);  // true: store ALL the data in map

   istrm.clear();
   istrm.close();

   return iret;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int SolarSystemEphemeris::initializeWithBinaryFile(string filename) throw(Exception)
{
try {
   int iret;

   //cout << "Reporting in SolarSystemEphemeris is "
   //   << ConfigureLOG::ToString(ConfigureLOG::ReportingLevel()) << endl;

   readBinaryHeader(filename);
   iret = readBinaryData(false);    // false: don't store data in map
   if(iret == 0) {
      // EphemerisNumber == -1 means the header has not been read
      // EphemerisNumber ==  0 means the fileposMap has not been read (binary)
      // EphemerisNumber == constants["DENUM"] means object has been initialized
      //                       (binary file), or header read (ASCII file)
      EphemerisNumber = int(constants["DENUM"]);
      LOG(DEBUG) << "initialize sets EphemerisNumber " << EphemerisNumber;
   }

   return iret;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// get an inertial position of one body relative to another.
void SolarSystemEphemeris::RelativeInertialPositionVelocity(const double MJD,
                                                SolarSystemEphemeris::Planet target,
                                                SolarSystemEphemeris::Planet center,
                                                double pv[6], bool kilometers)
   throw(Exception)
{
try {
   int iret,i;
   
   // initialize
   for(i=0; i<6; i++) pv[i] = 0.0;

   // trivial; return
   if(target == center) return;

   // get the right record from the file
   double JD(MJD + MJD_TO_JD);
   iret = seekToJD(JD);
   // -1 out of range : input time is before the first time in file
   // -2 out of range : input time is after the last time in file, or in a gap
   // -3 stream is not open or not good, or EOF was found prematurely
   // -4 EphemerisNumber is not defined
   if(iret) {
      if(iret == -1 || iret == -2) {
         Exception e(string("Requested time is ")
                  + (iret==-1 ? string("before") : string("after"))
                  + string(" the range spanned by the ephemeris."));
         GPSTK_THROW(e);
      }
      else if(iret == -3) {
         Exception e(string("Stream error on ephemeris binary file"));
         GPSTK_THROW(e);
      }
      else if(iret == -4) {
         Exception e(string("Ephemeris not initialized"));
         GPSTK_THROW(e);
      }
      else {
         Exception e(string("Unknown error on ephemeris binary file"));
         GPSTK_THROW(e);
      }
   }

   // compute Nutations or Librations
   if(target == idNutations || target == idLibrations) {
      InertialPositionVelocity(MJD, target==idNutations ? NUTATIONS : LIBRATIONS, pv);
      return;
   }

   // define computeID's for target and center
   computeID TARGET,CENTER;

   if(target <= idSun)                        TARGET = computeID(target-1);
   else if(target == idSolarSystemBarycenter) TARGET = NONE;
   else if(target == idEarthMoonBarycenter)   TARGET = EMBARY;
   // (Nutations and Librations are done above)
   if(center <= idSun)                        CENTER = computeID(center-1);
   else if(center == idSolarSystemBarycenter) CENTER = NONE;
   else if(center == idEarthMoonBarycenter)   CENTER = EMBARY;

   // Earth and Moon need special treatment - get moon and Earth-moon barycenter
   double pvmoon[6],pvembary[6],Eratio,Mratio;

   // special cases of Earth AND Moon: Moon result is always geocentric
   if(target == idEarth && center == idMoon)  TARGET = NONE;
   if(center == idEarth && target == idMoon)  CENTER = NONE;

   // special cases of Earth OR Moon, but not both:
   if((target==idEarth && center!=idMoon) || (center==idEarth && target!=idMoon)) {
      Eratio = 1.0/(1.0 + constants["EMRAT"]);
      InertialPositionVelocity(MJD, MOON, pvmoon);
   }
   if((target==idMoon && center!=idEarth) || (center==idMoon && target!=idEarth)) {
      Mratio = constants["EMRAT"]/(1.0 + constants["EMRAT"]);
      InertialPositionVelocity(MJD, EMBARY, pvembary);
   }

   // compute states for target and center
   double pvtarget[6],pvcenter[6];
   InertialPositionVelocity(MJD, TARGET, pvtarget);
   InertialPositionVelocity(MJD, CENTER, pvcenter);

   // handle the Earth/Moon special cases
   // convert from E-M barycenter to Earth
   if(target == idEarth && center != idMoon)
      for(i=0; i<6; i++) pvtarget[i] -= pvmoon[i]*Eratio;
   if(center == idEarth && target != idMoon)
      for(i=0; i<6; i++) pvcenter[i] -= pvmoon[i]*Eratio;

   if(target == idMoon && center != idEarth)
      for(i=0; i<6; i++) pvtarget[i] = pvembary[i] + pvtarget[i]*Mratio;
   if(center == idMoon && target != idEarth)
      for(i=0; i<6; i++) pvcenter[i] = pvembary[i] + pvcenter[i]*Mratio;
   
   // final relative result
   for(i=0; i<6; i++) pv[i] = pvtarget[i] - pvcenter[i];
   
   if(!kilometers) {
      double AU = constants["AU"];
      for(i=0; i<6; i++) pv[i] /= AU;
   }
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// private
void SolarSystemEphemeris::writeBinary(ofstream& strm, const char *ptr, size_t size)
   throw(Exception)
{
try {
   strm.write(ptr,size);
   if(!strm.good()) {
      Exception e("Stream error");
      GPSTK_THROW(e);
   }
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// private
void SolarSystemEphemeris::readBinary(char *ptr, size_t size) throw(Exception)
{
try {
   istrm.read(ptr,size);
   if(istrm.eof() || !istrm.good()) {
      Exception e("Stream error or premature EOF");
      GPSTK_THROW(e);
   }
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// private
void SolarSystemEphemeris::readBinaryHeader(std::string filename) throw(Exception)
{
try {
   int i,DENUM,recLength;
   char buffer[100];
   double AU,EMRAT;
   string word;

   // open the input binary file
   istrm.open(filename.c_str(), ios::in | ios::binary);
   if(!istrm.is_open()) {
      Exception e("Failed to open input binary file " + filename + ". Abort.");
      GPSTK_THROW(e);
   }

   // initialize
   EphemerisNumber = -1;
   constants.clear();
   store.clear();
   recLength = 0;

   // ----------------------------------------------------------------
   // read the first header record
   // 1. 3 labels each of length 84
   for(i=0; i<3; i++) {
      readBinary(buffer,84); //istrm.read(buffer,84);
      recLength += 84;
      buffer[84] = '\0';
      label[i] = stripTrailing(stripLeading(buffer," ")," ");
      LOG(DEBUG) << "readBinaryHeader reads label " << label[i];
   }

   // 2. 400 keys from the const array, each of length 6
   vector<string> consts_names;
   buffer[6] = '\0';
   for(i=0; i<400; i++) {
      readBinary(buffer,6);
      recLength += 6;
      word = stripLeading(string(buffer));
      if(!word.empty()) {
         consts_names.push_back(word);
         LOG(DEBUG) << "readBinaryHeader reads constant label " << word;
      }
   }
   Nconst = consts_names.size();

   // 3. the three times
   readBinary((char *)&startJD,sizeof(double));
   LOG(DEBUG) << "readBinaryHeader reads start JD "
      << fixed << setprecision(2) << startJD;

   readBinary((char *)&endJD,sizeof(double));
   LOG(DEBUG) << "readBinaryHeader reads end JD "
      << fixed << setprecision(2) << endJD;

   readBinary((char *)&interval,sizeof(double));
   LOG(DEBUG) << "readBinaryHeader reads interval " << interval;

   recLength += 3*sizeof(double);

   // 4. Ncoeff
   readBinary((char *)&Ncoeff,sizeof(int));
   recLength += sizeof(int);
   LOG(DEBUG) << "readBinaryHeader reads number of coefficients " << Ncoeff;

   // 5. AU and EMRAT
   buffer[sizeof(double)] = '\0';
   readBinary((char *)&AU,sizeof(double));
   recLength += sizeof(double);
   LOG(DEBUG) << "readBinaryHeader reads AU " << fixed << setprecision(4) << AU;

   readBinary((char *)&EMRAT,sizeof(double));
   recLength += sizeof(double);
   LOG(DEBUG) << "readBinaryHeader reads EMRAT " << EMRAT;

   // 6. c_arrays for the first 12 planets
   for(i=0; i<12; i++) {
      readBinary((char *)&c_offset[i],sizeof(int));
      readBinary((char *)&c_ncoeff[i],sizeof(int));
      readBinary((char *)&c_nsets[i],sizeof(int));
      recLength += 3*sizeof(int);
      LOG(DEBUG) << "readBinaryHeader reads " << i << " " << c_offset[i] << " "
         << c_ncoeff[i] << " " << c_nsets[i];
   }

   // 7. DENUM
   double denum;
   readBinary((char *)&denum,sizeof(double));
   recLength += sizeof(double);
   LOG(DEBUG) << "readBinaryHeader reads DENUM directly " << denum;

   // 8. c_arrays for libration
   readBinary((char *)&c_offset[12],sizeof(int));
   readBinary((char *)&c_ncoeff[12],sizeof(int));
   readBinary((char *)&c_nsets[12],sizeof(int));
   recLength += 3*sizeof(int);
   LOG(DEBUG) << "readBinaryHeader reads " << 12 << " " << c_offset[12] << " "
      << c_ncoeff[12] << " " << c_nsets[12];

   // 9. pad - records are padded to be the same length as the data records b/c
   //          JPL does it (for Fortran reasons) - not necessary
   LOG(DEBUG) << "Pad length 1 = " << Ncoeff*sizeof(double)-recLength;
   for(i=0; i < Ncoeff*sizeof(double)-recLength; i++)
      readBinary(buffer,1);

   // ----------------------------------------------------------------
   // the second header record: 400 values from the const array
   double d;
   for(i=0; i<400; i++) {
      readBinary((char *)&d,sizeof(double));
      if(i < Nconst) {
         constants[stripTrailing(consts_names[i])] = d;
         LOG(DEBUG) << "readBinaryHeader reads " << consts_names[i] << " = "
            << fixed << setprecision(18) << setw(24) << d;
      }
   }
   // pad
   LOG(DEBUG) << "Pad length 2 = " << (400-Nconst)*sizeof(double);
   for(i=0; i < (400-Nconst)*sizeof(double); i++)
      readBinary(buffer,1);

   // ----------------------------------------------------------------
   // test the header
   if(denum == constants["DENUM"]) {
      LOG(DEBUG) << "DENUM agrees " << denum;

      // EphemerisNumber == -1 means the header has not been read
      // EphemerisNumber ==  0 means the fileposMap has not been read (binary)
      // EphemerisNumber == constants["DENUM"] means object has been initialized
      //                       (binary file), or header read (ASCII file)
      EphemerisNumber = 0;

      // clear the data arrays
      store.clear();
   }
   else {
      LOG(WARNING) << "DENUM (" << denum << ") does not equal the array value ("
         << constants["DENUM"] << ")";
   }
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// private
// return 0 ok, or -4 EphemerisNumber is not defined
int SolarSystemEphemeris ::readBinaryData(bool save) throw(Exception)
{
try {
   // has the header been read?
   if(EphemerisNumber == -1) return -4;

   // read the data, optionally storing it all; fill the file position map
   int iret=-1,nrec=1;
   double prev=0.0;
   vector<double> data_vector;
   while(!istrm.eof() && istrm.good()) {
      long filepos = istrm.tellg();
      iret = readBinaryRecord(data_vector);
      if(iret == -2) { iret = 0; break; }       // EOF
      if(iret) break;

      // if saving all in store, add it here
      if(save)
         store[data_vector[0]] = data_vector;

      // put the first record in coefficients array
      if(nrec == 1)
         coefficients = data_vector;

      // build the positions map
      fileposMap[data_vector[0]] = filepos;

      if(nrec > 1 && data_vector[0] != prev) {
         ostringstream oss;
         oss << "ERROR: found gap in data at " << nrec << fixed << setprecision(6)
            << " : prev end = " << prev << " != new beg = " << data_vector[0];
         Exception e(oss.str());
         GPSTK_THROW(e);
      }

      // remember the end time for next record
      prev = data_vector[1];

      // count records
      nrec++;
   }

   istrm.clear();

   return iret;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// private
// return 0 ok, or
// -3 stream is not open or not good
// -4 EphemerisNumber is not defined
// For -3,-4 : initializeWithBinaryFile() has not been called, or reading failed.
int SolarSystemEphemeris::readBinaryRecord(vector<double>& data_vector)
   throw(Exception)
{
try {
   if(!istrm) return -3;
   if(istrm.eof() || !istrm.good()) return -3;
   if(EphemerisNumber <= -1) return -4;

   data_vector.clear();

   double d;
   for(int i=0; i<Ncoeff; i++) {
      istrm.read((char *)&d,sizeof(double)); // don't use readBinary(), to catch EOF
      if(istrm.eof()) return -2;
      if(!istrm.good()) return -3;
      data_vector.push_back(d);
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// private
// return 0 ok, or
// -1 out of range : input time is before the first time in file
// -2 out of range : input time is after the last time in file, or in a gap
// -3 stream is not open or not good, or EOF was found prematurely
// -4 EphemerisNumber is not defined
// For -3,-4 : initializeWithBinaryFile() has not been called, or reading failed.
int SolarSystemEphemeris ::seekToJD(double JD) throw(Exception)
{
try {
   if(!istrm) return -3;
   if(istrm.eof() || !istrm.good()) return -3;
   if(EphemerisNumber != int(constants["DENUM"])) return -4;

   if(coefficients[0] <= JD && JD <= coefficients[1]) return 0;

   map<double,long>::const_iterator it; // key >= input
   it = fileposMap.lower_bound(JD); // it points to first element with JD <= time

   if(it == fileposMap.begin() && JD < it->first)
      return -1;                    // failure: JD is before the first record

   if(it == fileposMap.end()        // if beyond the found record, go to previous;
         || JD < it->first) it--;   // but beware the "lower_bound found the =" case

   istrm.seekg(it->second,ios_base::beg);         // get the record
   int iret = readBinaryRecord(coefficients);
   if(iret == -2) iret = -3;        // this means EOF during data read
   if(iret) return iret;            // reading failed

   if(JD > coefficients[1])
      return -2;                    // failure: JD is after the last record, or
                                    // JD is in a gap between records
   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// private
void SolarSystemEphemeris::InertialPositionVelocity(const double MJD,
                                  SolarSystemEphemeris::computeID which, double PV[6])
   throw(Exception)
{
try {
   int i,j,i0,ncomp,offset;

   for(i=0; i<6; i++) PV[i]=0.0;
   if(which == NONE) return;

   // coefficients[0,1] give span of JD's in which coefficients[2,...] are applicable
   // coefficients[0,1] are even days JDs - 2452xxx.5 => secOfDay() for these == 0.
   double T,Tbeg,Tspan,Tspan0;
   Tbeg = coefficients[0];
   Tspan0 = Tspan = coefficients[1] - coefficients[0];
   i0 = c_offset[which]-1;                      // index of first coefficient in array
   ncomp = (which == NUTATIONS ? 2 : 3);        // number of components returned

   // if more than one set, find the right set
   if(c_nsets[which] > 1) {
      Tspan /= double(c_nsets[which]);
      for(j=c_nsets[which]; j>0; j--) {
         Tbeg = coefficients[0] + double(j-1)*Tspan;
         if(MJD > Tbeg-MJD_TO_JD) {    // == with j==1 is the default
            i0 += (j-1)*ncomp*c_ncoeff[which];
            break;
         }
      }
   }

   // normalized time
   T = 2.0*(MJD-(Tbeg-MJD_TO_JD))/Tspan - 1.0;

   // interpolate
   unsigned int N=c_ncoeff[which];
   vector<double> C(N,0.0);     // Chebyshev
   vector<double> U(N,0.0);     // derivative of Chebyshev
   for(i=0; i<ncomp; i++) {     // loop over components

      // seed the Chebyshev recursions
      C[0] = 1; C[1] = T; //C[2] = 2*T*T-1;
      U[0] = 0; U[1] = 1; //U[2] = 4*T;

      // generate the Chebyshevs
      for(j=2; j<N; j++) {
         C[j] = 2*T*C[j-1] - C[j-2];
         U[j] = 2*T*U[j-1] + 2*C[j-1] - U[j-2];
      }

      // compute P and V
      // done above PV[i] = PV[i+3] = 0.0;
      for(j=N-1; j>-1; j--)                              // POS
         PV[i] += coefficients[i0+j+i*N] * C[j];
      for(j=N-1; j>0; j--) // j>0 b/c U[0]=0             // VEL
         PV[i+ncomp] += coefficients[i0+j+i*N] * U[j];

      // convert velocity to 'per day'
      PV[i+ncomp] *= 2*double(c_nsets[which])/Tspan0;
   }
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
}  // end namespace gpstk
