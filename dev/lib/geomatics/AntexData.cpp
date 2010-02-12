#pragma ident "$Id$"

/**
 * @file AntexData.cpp
 * Encapsulate data from ANTEX (Antenna Exchange) format files, including both
 * receiver and satellite antennas, ANTEX file I/O, discrimination between different
 * satellite antennas based on system, PRN and time, and computation of phase center
 * offsets and variations.
 */

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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

#include "AntexData.hpp"
#include "AntexStream.hpp"
#include "StringUtils.hpp"
#include "geometry.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
   const string AntexData::startAntennaString = "START OF ANTENNA";
   const string AntexData::typeSerNumString   = "TYPE / SERIAL NO";
   const string AntexData::methodString       = "METH / BY / # / DATE";
   const string AntexData::daziString         = "DAZI";
   const string AntexData::zenithString       = "ZEN1 / ZEN2 / DZEN";
   const string AntexData::numFreqString      = "# OF FREQUENCIES";
   const string AntexData::validFromString    = "VALID FROM";
   const string AntexData::validUntilString   = "VALID UNTIL";
   const string AntexData::sinexCodeString    = "SINEX CODE";
   const string AntexData::dataCommentString  = "COMMENT";
   const string AntexData::startFreqString    = "START OF FREQUENCY";
   const string AntexData::neuFreqString      = "NORTH / EAST / UP";
   const string AntexData::endOfFreqString    = "END OF FREQUENCY";
   const string AntexData::startFreqRMSString = "START OF FREQ RMS";
   const string AntexData::neuFreqRMSString   = "NORTH / EAST / UP";
   const string AntexData::endOfFreqRMSString = "END OF FREQ RMS";
   const string AntexData::endOfAntennaString = "END OF ANTENNA";

   // NB. this dimension must be updated with the list
   /// Number of types that are used for satellites
   const int Nsattype=11;
   string sattype[Nsattype] =
   { 
      string("BLOCK I"),         // 1
      string("BLOCK II"),        // 2
      string("BLOCK IIA"),       // 3
      string("BLOCK IIR"),       // 4
      string("BLOCK IIR-A"),     // 5
      string("BLOCK IIR-B"),     // 6
      string("BLOCK IIR-M"),     // 7
      string("BLOCK IIF"),       // 8
      string("GLONASS"),         // 9
      string("GLONASS-M"),       // 10
      string("GLONASS-K")        // 11
   };
   /// vector of type strings that identify satellites; must be kept updated.
   const vector<string> AntexData::SatelliteTypes(sattype,sattype+Nsattype);

   // ----------------------------------------------------------------------------
   bool AntexData::isValid(DayTime& time) const throw()
   {
      if(!isValid())
         return false;
      if(time == DayTime::BEGINNING_OF_TIME ||
            (!(valid & validFromValid) && !(valid & validUntilValid))) {
         return true;
      }
      if((valid & validFromValid) && time < validFrom) {
         return false;
      }
      if((valid & validUntilValid) && time > validUntil) {
         return false;
      }

      return true;
   }

   // ----------------------------------------------------------------------------
   double AntexData::getTotalPhaseCenterOffset(const int freq,
                                               const double azim,
                                               const double elev_nadir) const
      throw(Exception)
   {
      try {
         // these do all the checking and throwing
         double pcv = getPhaseCenterVariation(freq, azim, elev_nadir);
         Triple pco = getPhaseCenterOffset(freq);

         double elev = elev_nadir;
         if(!isRxAntenna)              // satellite : elev_nadir is 'nadir' angle:
            elev = 90. - elev_nadir;   //             from Z axis toward XY plane.

         double cosel = ::cos(elev * DEG_TO_RAD);
         double sinel = ::sin(elev * DEG_TO_RAD);
         double cosaz = ::cos(azim * DEG_TO_RAD);
         double sinaz = ::sin(azim * DEG_TO_RAD);
         
         // see doc for class AntexData for signs, etc
         return (-pcv + pco[0]*cosel*cosaz
                      + pco[1]*cosel*sinaz
                      + pco[2]*sinel);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   // Access the PCO (only) values in the antenna coordinate system
   // (This does NOT include the phase center variations, which should
   // be computed using getPhaseCenterVariation() and added to the PCOs to get
   // the total phase center offset).
   Triple AntexData::getPhaseCenterOffset(const int freq) const
      throw(Exception)
   {
      if(!isValid()) {
         gpstk::Exception e("Invalid object");
         GPSTK_THROW(e);
      }
      if(freq <= 0 || freq > nFreq) {
         gpstk::Exception e("Invalid frequency");
         GPSTK_THROW(e);
      }

      // get the antennaPCOandPCVData for this frequency
      map<int, antennaPCOandPCVData>::const_iterator it = freqPCVmap.find(freq);
      if(it == freqPCVmap.end()) {
         gpstk::Exception e("Frequency " + gpstk::StringUtils::asString(freq)
               + " not found! object must be corrupted.");
         GPSTK_THROW(e);
      }
      //const antennaPCOandPCVData& antpco = it->second;
      
      Triple retval;
      for(int i=0; i<3; i++)
         retval[i] = it->second.PCOvalue[i];

      return retval;
   }

   // Compute the phase center variation (only) at the given azimuth and elevation
   // (receiver) or nadir (satellite) angles
   double AntexData::getPhaseCenterVariation(const int freq,
                                             const double azimuth,
                                             const double elev_nadir) const
      throw(Exception)
   {
      if(!isValid()) {
         Exception e("Invalid object");
         GPSTK_THROW(e);
      }
      if(freq <= 0 || freq > nFreq) {
         Exception e("Invalid frequency");
         GPSTK_THROW(e);
      }
      if(elev_nadir < 0.0 || elev_nadir > 90.0) {
         Exception e("Invalid elevation/nadir angle");
         GPSTK_THROW(e);
      }

      double retpco, azim, zen;
      zen = elev_nadir;             // satellite: elev_nadir is a zenith (nadir) angle
      if(isRxAntenna)               // receiver: elev_nadir is an elevation
         zen = 90. - elev_nadir;

      // ensure azim is within range
      if(azimuth < 0.0 || azimuth >= 360.0)
         azim = fmod(azimuth,360.0);
      else
         azim = azimuth;

      // find four points bracketing the point (azim,zen)
      //       zen
      //       ^
      // zn_hi | 0     1          have pco at 0,1,2,3
      //       |    x             have (azim,zen) at x, want retpco at x
      // zn_lo | 2     3
      //       |----------> azim
      //       az_lo  az_hi
      //
      // find bracketing azims within the map, then find bracketing zeniths and PCOs
      double az_lo,az_hi,zn_lo,zn_hi,pco[4];
      map<int, antennaPCOandPCVData>::const_iterator it;
      map<double, zenOffsetMap>::const_iterator jt_lo,jt_hi;

      it = freqPCVmap.find(freq);
      if(it == freqPCVmap.end()) {
         Exception e("Frequency " + StringUtils::asString(freq)
               + " not found! object must be corrupted.");
         GPSTK_THROW(e);
      }

      const antennaPCOandPCVData& antpco = it->second;
      const azimZenMap& azzenmap = antpco.PCVvalue;

      if(!antpco.hasAzimuth)
         jt_hi = azzenmap.begin(); // this will be only entry
      else
         jt_hi = azzenmap.find(azim); // find() returns end() unless exact match

      // either azimuth is not there, or there is an exact match in azimuth
      if(jt_hi != azzenmap.end()) {
         //az_lo = az_hi = azim;
         const zenOffsetMap& zenoffmap = jt_hi->second;
         // bracket zenith angle
         evaluateZenithMap(zen, zenoffmap, zn_lo, zn_hi, pco[2], pco[0]);
         if(zn_lo == zn_hi)                  // exact match in zenith angle
            retpco = pco[0];                 // TD use fabs(diff) < tolerance ?
         else                                // linear interpolation in zenith angle
            retpco = (pco[0]*(zen - zn_lo) + pco[2]*(zn_hi - zen))/(zn_hi - zn_lo);
      }

      else {                                 // must bracket in azimuth
         // lower_bound() returns first value >= azim (but == case handled above)
         jt_lo = jt_hi = azzenmap.lower_bound(azim);

         if(jt_lo == azzenmap.end()) {        // beyond the last value
            jt_lo--;                         // last value
            az_lo = jt_lo->first;
            jt_hi = azzenmap.begin();         // wrap around to beginning
            az_hi = jt_hi->first + 360.;
         }
         else if(jt_hi == azzenmap.begin()) { // before the first value
            az_hi = jt_hi->first;
            (jt_lo = azzenmap.end())--;       // wrap around to end
            az_lo = jt_lo->first - 360.;
         }
         else {                              // azim is bracketed
            az_hi = jt_hi->first;
            jt_lo--;
            az_lo = jt_lo->first;
         }

         // get zenith angles and pcos at upper and lower azimuths
         evaluateZenithMap(zen, jt_hi->second, zn_lo, zn_hi, pco[3], pco[1]);
         evaluateZenithMap(zen, jt_lo->second, zn_lo, zn_hi, pco[2], pco[0]);

         // interpolation
         if(zn_hi == zn_lo)   // zen exact match, linear interpolate in azimuth
            retpco = (pco[2]*(az_hi - azim) + pco[3]*(azim - az_lo))/(az_hi - az_lo);
         else                 // bi-linear interpolation
            retpco = ( pco[0] * (az_hi - azim)*(zen - zn_lo)
                     + pco[1] * (azim - az_lo)*(zen - zn_lo)
                     + pco[2] * (az_hi - azim)*(zn_hi - zen)
                     + pco[3] * (azim - az_lo)*(zn_hi - zen) )
                           / ( (az_hi - az_lo)*(zn_hi - zn_lo) );
      }

      // do not change the sign; just interpolate the map
      return retpco;
   }

   void AntexData::dump(ostream& s, int detail) const
   {
      map<int, antennaPCOandPCVData>::const_iterator it;
      map<double, zenOffsetMap>::const_iterator jt;
      map<double, double>::const_iterator kt;

      s << "Antenna Type/SN: [" << name() << "]";
      if(isRxAntenna)
         s << " (Receiver)" << endl;
      else {
         if(PRN != -1 || SVN != -1) {
           s << " (" << (type == string("GLONASS") ? "GLONASS" : "GPS");
           if(PRN != -1) s << string(" PRN ") + asString(PRN);
           if(SVN != -1) s << string(" SVN ") + asString(SVN);
           s << ")";
        }
        s << " Sat. code: " << satCode
          << " COSPAR ID: " << cospar << endl;
      }

      if(detail <= 0) return;

      s << "Method: " << method << "   Agency: " << agency
         << "   #Cal.Ant.s: " << noAntCalibrated << "   Date: " << date << endl;
      if(azimDelta > 0.0)
         s << "Azimuth dependence, delta azimuth = "
            << fixed << setprecision(1) << azimDelta << endl;
      else
         s << "No azimuth dependence" << endl;
      s << "Elevation dependence: from "
         << fixed << setprecision(1) << zenRange[0] << " to " << zenRange[1]
         << " in steps of " << zenRange[2] << " degrees." << endl;
      s << "Frequencies stored (" << nFreq << "): ";

      it = freqPCVmap.begin();
      while(it != freqPCVmap.end()) {
         s << " " << (serialNo[0]=='G' ? "L" : "") << it->first;
         it++;
      }
      s << endl;
      s << "Valid FROM "
         << (validFrom == DayTime::BEGINNING_OF_TIME ? " (all time) "
              : validFrom.printf("%02m/%02d/%04Y %02H:%02M:%.7s"))
         << " TO "
         << (validUntil == DayTime::END_OF_TIME ? " (all time) "
              : validUntil.printf("%02m/%02d/%04Y %02H:%02M:%.7s"))
         << endl;
      if(!sinexCode.empty())
         s << "SINEX code: " << sinexCode << endl;
      for(int i=0; i<commentList.size(); i++) {
         //if(i==0) s << "Comments:\n";
         s << "Comment " << setw(2) << i+1 << ": " << commentList[i] << endl;
      }

      if(detail == 1) {
         for(it = freqPCVmap.begin(); it != freqPCVmap.end(); it++) {
            const antennaPCOandPCVData& antpco = it->second;
            s << "PCO (" << (isRxAntenna ? "NEU from antenna reference position"
                                  : "body XYZ from center-of-mass") << ") (mm):"
              << " (freq " << it->first << ") "
              << fixed << setprecision(2) << setw(10) << antpco.PCOvalue[0]
              << ", " << setw(10) << antpco.PCOvalue[1]
              << ", " << setw(10) << antpco.PCOvalue[2] << endl;
         }
         return;
      }

      // loop over frequency
      for(it = freqPCVmap.begin(); it != freqPCVmap.end(); it++) {
         s << "Offset values for frequency: " << it->first
            << " (" << (it->second.hasAzimuth ? "has" : "does not have")
            << " azimuths)" << endl;
         const antennaPCOandPCVData& antpco = it->second;

         // PCOs
         s << "  PCO (" << (isRxAntenna ? "NEU from antenna reference position"
                                 : "body XYZ from center-of-mass") << ") (mm):"
           << fixed << setprecision(2) << setw(10) << antpco.PCOvalue[0]
           << ", " << setw(10) << antpco.PCOvalue[1]
           << ", " << setw(10) << antpco.PCOvalue[2] << endl;

         // RMS PCOs
         if(valid & neuFreqRMSValid)
            s << "  RMS PCO ("
              << (isRxAntenna ? "NEU from antenna reference position"
                              : "body XYZ from center-of-mass") << " (mm):"
              << fixed << setprecision(2) << setw(10) << antpco.PCOrms[0]
              << ", " << setw(10) << antpco.PCOrms[1]
              << ", " << setw(10) << antpco.PCOrms[2] << endl;

         // PCV array(s)
         const azimZenMap& azel = antpco.PCVvalue;
         s << fixed << setprecision(2);
         // header line
         jt = azel.begin();
         const zenOffsetMap& zenoffmap = jt->second;
         s << "  PCVs follow, one azimuth per row: AZ(deg) { PCVs(EL)(mm) .. .. }\n";
         s << "  EL(deg)";
         for(kt = zenoffmap.begin(); kt != zenoffmap.end(); kt++)
            s << setw(8) << kt->first;
         s << endl;
         // data values
         for(jt = azel.begin(); jt != azel.end(); jt++) {
            double azimuth = jt->first;
            const zenOffsetMap& zenoffmap = jt->second;
            if(azimuth == -1.0) s << "  (NOAZI)";
            else s << setw(9) << azimuth;
            for(kt = zenoffmap.begin(); kt != zenoffmap.end(); kt++)
               s << setw(8) << kt->second;
            s << endl;
         }
      }  // end loop over frequency

   }

   // ----------------------------------------------------------------------------
   // protected routines
   //
   // Find zenith angles bracketing the input zenith angle within the given map,
   // and the corresponding PCOs.
   void AntexData::evaluateZenithMap(const double& zen,
                                     const zenOffsetMap& eomap,
                                     double& zen_lo, double& zen_hi,
                                     double& pco_lo, double& pco_hi) const
      throw()
   {
      map<double, double>::const_iterator kt;

      // find() returns end() unless there is an exact match
      kt = eomap.find(zen);
      if(kt != eomap.end()) {                // exact match
         zen_lo = zen_hi = zen;
         pco_lo = pco_hi = kt->second;
         return;
      }

      // lower_bound() returns first value >= zen (but == case handled above)
      kt = eomap.lower_bound(zen);

      // zen is above the last, or below the first value - just take that value
      if(kt == eomap.end() || kt == eomap.begin()) {
         if(kt == eomap.end()) kt--;         // last value
         zen_lo = zen_hi = zen;
         pco_lo = pco_hi = kt->second;
         return;
      }

      // zen is bracketed
      zen_hi = kt->first;
      pco_hi = kt->second;
      kt--;
      zen_lo = kt->first;
      pco_lo = kt->second;

      return;
   }

   void AntexData::reallyPutRecord(FFStream& ffs) const 
      throw(exception, FFStreamError, StringException)
   {
      if(!isValid()) {
         FFStreamError fse(string("Cannot write invalid AntexData"));
         GPSTK_THROW(fse);
      }

      int i;
      string line;
      map<int, antennaPCOandPCVData>::const_iterator it;
      map<double, zenOffsetMap>::const_iterator jt;
      map<double, double>::const_iterator kt;
      AntexStream& strm = dynamic_cast<AntexStream&>(ffs);

      line = rightJustify(leftJustify(startAntennaString,20),80);
      strm << line << endl;
      strm.lineNumber++;
   
      line  = leftJustify(type,20);
      line += leftJustify(serialNo,20);
      line += leftJustify(satCode,10);
      line += leftJustify(cospar,10);
      line += typeSerNumString;     //"TYPE / SERIAL NO";
      strm << leftJustify(line,80) << endl;
      strm.lineNumber++;

      line  = leftJustify(method,20);
      line += leftJustify(agency,20);
      line += leftJustify(rightJustify(asString(noAntCalibrated),6),10);
      line += leftJustify(date,10);
      line += methodString;         // "METH / BY / # / DATE";
      strm << leftJustify(line,80) << endl;
      strm.lineNumber++;

      line  = string("  ");
      line += rightJustify(asString(azimDelta,1),6);
      line  = leftJustify(line,60);
      line += daziString;           // "DAZI";
      strm << leftJustify(line,80) << endl;
      strm.lineNumber++;

      line  = string("  ");
      line += rightJustify(asString(zenRange[0],1),6);
      line += rightJustify(asString(zenRange[1],1),6);
      line += rightJustify(asString(zenRange[2],1),6);
      line  = leftJustify(line,60);
      line += zenithString;         // "ZEN1 / ZEN2 / DZEN";
      strm << leftJustify(line,80) << endl;
      strm.lineNumber++;

      line  = rightJustify(asString(nFreq),6);
      line  = leftJustify(line,60);
      line += numFreqString;        // "# OF FREQUENCIES";
      strm << leftJustify(line,80) << endl;
      strm.lineNumber++;

      if(valid & validFromValid) {
         if(stringValidFrom.empty())
            line  = writeTime(validFrom);
         else
            line = stringValidFrom;
         line  = leftJustify(line,60);
         line += validFromString;      // "VALID FROM";
         strm << leftJustify(line,80) << endl;
         strm.lineNumber++;
      }

      if(valid & validUntilValid) {
         if(stringValidUntil.empty())
            line  = writeTime(validUntil);
         else
            line = stringValidUntil;
         line  = leftJustify(line,60);
         line += validUntilString;    // "VALID UNTIL";
         strm << leftJustify(line,80) << endl;
         strm.lineNumber++;
      }

      if(valid & sinexCodeValid) {
         line  = leftJustify(rightJustify(sinexCode,10),60);
         line += sinexCodeString;      // "SINEX CODE";
         strm << leftJustify(line,80) << endl;
         strm.lineNumber++;
      }

      for(i=0; i<commentList.size(); i++) {
         line  = leftJustify(commentList[i],60);
         line += dataCommentString;    // "COMMENT";
         strm << leftJustify(line,80) << endl;
         strm.lineNumber++;
      }

      // loop over frequency
      string freqStr;
      for(it = freqPCVmap.begin(); it != freqPCVmap.end(); it++) {
         const antennaPCOandPCVData& antpco = it->second;

         ostringstream oss; // b/c asString does not allow set fill char
         oss << systemChar << setfill('0') << setw(2) << it->first;
         freqStr = oss.str();
         line  = string("   ");
         line += freqStr;
         line  = leftJustify(line,60);
         line += startFreqString;      // "START OF FREQUENCY";
         strm << leftJustify(line,80) << endl;
         strm.lineNumber++;

         line = string();
         for(i=0; i<3; i++)
            line += rightJustify(asString(antpco.PCOvalue[i],2),10);
         line  = leftJustify(line,60);
         line += neuFreqString;        // "NORTH / EAST / UP";
         strm << leftJustify(line,80) << endl;
         strm.lineNumber++;

         // PCVs
         const azimZenMap& azel = antpco.PCVvalue;
         for(jt = azel.begin(); jt != azel.end(); jt++) {
            const zenOffsetMap& zenoffmap = jt->second;
            if(antpco.hasAzimuth && jt->first > -1.0)
               line = rightJustify(asString(jt->first,1),8);
            else
               line = string("   NOAZI");
            for(kt = zenoffmap.begin(); kt != zenoffmap.end(); kt++) {
               line += rightJustify(asString(kt->second,2),8);
            }
            strm << line << endl;
            strm.lineNumber++;
         }

         line  = string("   ");
         line += freqStr;
         line  = leftJustify(line,60);
         line += endOfFreqString;      // "END OF FREQUENCY";
         strm << leftJustify(line,80) << endl;
         strm.lineNumber++;
      }

      if(valid & (startFreqRMSValid|neuFreqRMSValid|endOfFreqRMSValid)) {
         // loop over frequency again
         for(it = freqPCVmap.begin(); it != freqPCVmap.end(); it++) {
            const antennaPCOandPCVData& antpco = it->second;
            ostringstream oss; // b/c asString does not allow set fill char
            oss << systemChar << setfill('0') << setw(2) << it->first;
            freqStr = oss.str();
            line  = string("   ");
            line += freqStr;
            line  = leftJustify(line,60);
            line += startFreqRMSString;   // "START OF FREQ RMS";
            strm << leftJustify(line,80) << endl;
            strm.lineNumber++;

            line = string();
            for(i=0; i<3; i++)
               line += rightJustify(asString(antpco.PCOrms[i],2),10);
            line  = leftJustify(line,60);
            line += neuFreqRMSString;     // "NORTH / EAST / UP";
            strm << leftJustify(line,80) << endl;
            strm.lineNumber++;

            // PCVs
            const azimZenMap& azel = antpco.PCVrms;
            for(jt = azel.begin(); jt != azel.end(); jt++) {
               const zenOffsetMap& zenoffmap = jt->second;
               if(antpco.hasAzimuth)
                  line = rightJustify(asString(jt->first,1),8);
               else
                  line = string("   NOAZI");
               for(kt = zenoffmap.begin(); kt != zenoffmap.end(); kt++) {
                  line += rightJustify(asString(kt->second,2),8);
               }
               strm << line << endl;
               strm.lineNumber++;
            }

            line  = string("   ");
            line += freqStr;
            line  = leftJustify(line,60);
            line += endOfFreqRMSString;   // "END OF FREQ RMS";
            strm << leftJustify(line,80) << endl;
            strm.lineNumber++;
         }
      }

      line = rightJustify(leftJustify(endOfAntennaString,20),80);
      strm << line << endl;
      strm.lineNumber++;

   }   // end AntexData::reallyPutRecord


   void AntexData::reallyGetRecord(FFStream& ffs) 
      throw(exception, FFStreamError, StringUtils::StringException)
   {
      AntexStream& strm = dynamic_cast<AntexStream&>(ffs);
      
         // If the header hasn't been read, read it...
      if(!strm.headerRead) strm >> strm.header;
      
         // Clear out this object
      AntexData ad;
      *this = ad;
      
      string line;
      
      while(!(valid & endOfAntennaValid)) {
         strm.formattedGetLine(line, true);
         stripTrailing(line);

         if(line.length() == 0)
            continue;

         try {
            ParseDataRecord(line);
         }
         catch(FFStreamError& e) {
            GPSTK_THROW(e);
         }
      }
      
   } // end of reallyGetRecord()

   // ----------------------------------------------------------------------------
   // private routines
   //
   // helper routine for ParseDataRecord
   // throw if valid contains test, i.e. !(test & valid)
   void AntexData::throwRecordOutOfOrder(unsigned long test, string& label)
   {
      if(test & valid) {
         FFStreamError fse(string("Records are out of order: detected at ") + label);
         GPSTK_THROW(fse);
      }
   }

   // for reallyGetRecord
   void AntexData::ParseDataRecord(string& line)
      throw(FFStreamError)
   {
   try {
      static bool hasAzim;
      static int freq;
      static string freqStr;
      string label(line, 60, 20);

      if(label == startAntennaString) {        // "START OF ANTENNA"
         throwRecordOutOfOrder(typeSerNumValid,label);
         valid |= startAntennaValid;
      }
      else if(label == typeSerNumString) {     // "TYPE / SERIAL NO"
         throwRecordOutOfOrder(methodValid,label);
         type = stripTrailing(stripLeading(line.substr(0,20)));
         // determine if satellite
         isRxAntenna = true;
         for(int i=0; i<AntexData::SatelliteTypes.size(); i++)
            if(type == AntexData::SatelliteTypes[i]) { isRxAntenna = false; break; }
         serialNo = stripTrailing(stripLeading(line.substr(20,20)));
         satCode = stripTrailing(stripLeading(line.substr(40,10)));
         cospar = stripTrailing(stripLeading(line.substr(50,10)));
         if(!isRxAntenna) {         // get the PRN and SVN numbers
            if(serialNo.length() > 1) PRN = asInt(serialNo.substr(1,2));
            else PRN = -1;
            if(satCode.length() > 1) SVN = asInt(satCode.substr(1,3));
            else SVN = -1;
         }
         valid |= typeSerNumValid;
      }
      else if(label == methodString) {         // "METH / BY / # / DATE"
         throwRecordOutOfOrder(daziValid,label);
         method = stripTrailing(stripLeading(line.substr(0,20)));
         agency = stripTrailing(stripLeading(line.substr(20,20)));
         noAntCalibrated = asInt(line.substr(40,6));
         date = stripTrailing(stripLeading(line.substr(50,10)));
         valid |= methodValid;
      }
      else if(label == daziString) {           // "DAZI"
         throwRecordOutOfOrder(zenithValid,label);
         azimDelta = asDouble(line.substr(2,6));
         if(azimDelta > 0.0) hasAzim = true; else hasAzim = false;
         valid |= daziValid;
      }
      else if(label == zenithString) {         // "ZEN1 / ZEN2 / DZEN"
         throwRecordOutOfOrder(numFreqValid,label);
         zenRange[0] = asDouble(line.substr(2,6)); // NB. zenith angles
         zenRange[1] = asDouble(line.substr(8,6)); // not elevation angles
         zenRange[2] = asDouble(line.substr(14,6));
         valid |= zenithValid;
      }
      else if(label == numFreqString) {        // "# OF FREQUENCIES"
         throwRecordOutOfOrder(validFromValid|validUntilValid|sinexCodeValid|
               dataCommentValid|startFreqValid,label);
         nFreq = (unsigned int)(asInt(line.substr(0,6)));
         valid |= numFreqValid;
      }
      else if(label == validFromString) {      // "VALID FROM"
         throwRecordOutOfOrder(validUntilValid|sinexCodeValid|
               dataCommentValid|startFreqValid,label);
         stringValidFrom = line.substr(0,43);
         validFrom = parseTime(line);
         valid |= validFromValid;
      }
      else if(label == validUntilString) {     // "VALID UNTIL"
         throwRecordOutOfOrder(sinexCodeValid|dataCommentValid|startFreqValid,label);
         stringValidUntil = line.substr(0,43);
         validUntil = parseTime(line);
         if(validUntil == DayTime::BEGINNING_OF_TIME)
            validUntil = DayTime::END_OF_TIME;
         valid |= validUntilValid;
      }
      else if(label == sinexCodeString) {      // "SINEX CODE"
         throwRecordOutOfOrder(dataCommentValid|startFreqValid,label);
         sinexCode = stripTrailing(stripLeading(line.substr(0,10)));
         valid |= sinexCodeValid;
      }
      else if(label == dataCommentString) {    // "COMMENT"
         throwRecordOutOfOrder(startFreqValid,label);
         string str = stripTrailing(line.substr(0,60));
         commentList.push_back(str);
         valid |= dataCommentValid;
      }
      else if(label == startFreqString) {      // "START OF FREQUENCY"
         throwRecordOutOfOrder(startFreqRMSValid|neuFreqRMSValid|endOfFreqRMSValid|
               endOfAntennaValid,label);
         freqStr = line.substr(3,3);
         systemChar = line[3];
         if(systemChar == ' ') systemChar = 'G';
         freq = asInt(line.substr(4,2));
         valid |= startFreqValid;
      }
      else if(label == neuFreqString) {        // "NORTH / EAST / UP"
         throwRecordOutOfOrder(startFreqRMSValid|neuFreqRMSValid|endOfFreqRMSValid|
               endOfAntennaValid,label);
         freqPCVmap[freq].PCOvalue[0] = asDouble(line.substr(0,10));
         freqPCVmap[freq].PCOvalue[1] = asDouble(line.substr(10,10));
         freqPCVmap[freq].PCOvalue[2] = asDouble(line.substr(20,10));
         valid |= neuFreqValid;
         // set flag here
         freqPCVmap[freq].hasAzimuth = hasAzim;
      }
      else if(label == endOfFreqString) {      // "END OF FREQUENCY"
         throwRecordOutOfOrder(startFreqRMSValid|neuFreqRMSValid|endOfFreqRMSValid|
               endOfAntennaValid,label);
         if(freqStr != line.substr(3,3)) {
            FFStreamError fse("START/END OF FREQ confused: "
                  + freqStr + " != " + line.substr(3,3));
            GPSTK_THROW(fse);
         }
         valid |= endOfFreqValid;
      }
      else if(label == startFreqRMSString) {   // "START OF FREQ RMS"
         throwRecordOutOfOrder(endOfAntennaValid,label);
         freqStr = line.substr(3,3);
         freq = asInt(line.substr(4,2));
         valid |= startFreqRMSValid;
      }
      else if(label == neuFreqRMSString) {     // "NORTH / EAST / UP"
         throwRecordOutOfOrder(endOfAntennaValid,label);
         freqPCVmap[freq].PCOrms[0] = asDouble(line.substr(0,10));
         freqPCVmap[freq].PCOrms[1] = asDouble(line.substr(10,10));
         freqPCVmap[freq].PCOrms[2] = asDouble(line.substr(20,10));
         valid |= neuFreqRMSValid;
      }
      else if(label == endOfFreqRMSString) {   // "END OF FREQ RMS"
         throwRecordOutOfOrder(endOfAntennaValid,label);
         if(freqStr != line.substr(3,3)) {
            FFStreamError fse("START/END OF FREQ RMS confused: "
                  + freqStr + " != " + line.substr(3,3));
            GPSTK_THROW(fse);
         }
         valid |= endOfFreqRMSValid;
      }
      else if(label == endOfAntennaString) {   // "END OF ANTENNA"
         valid |= endOfAntennaValid;
      }
      else {
         int i,n;
         string noazi = line.substr(3,5);
         double azim = asDouble(line.substr(0,8));
         if(!hasAzim && noazi != string("NOAZI")) {
            FFStreamError fse("Invalid format; zero delta azimuth without NOAZI");
            GPSTK_THROW(fse);
         }

         // NOAZI : data stored under azimuth = -1.0
         if(noazi == string("NOAZI")) azim = -1.0;

         n = StringUtils::numWords(line) - 1;
         if(n != 1+int((zenRange[1]-zenRange[0])/zenRange[2])) {
            FFStreamError fse("Invalid format; wrong number of zenith/offset values");
            GPSTK_THROW(fse);
         }

         // loop over values; format is 3x,a5,mf8.2
         for(i=1; i<=n; i++) {
            double value = asDouble(line.substr(8*i,8));
            double zen = zenRange[0] + (i-1)*zenRange[2];
            if(valid & neuFreqRMSValid)
               freqPCVmap[freq].PCVrms[azim][zen] = value;
            else if(valid & neuFreqValid)
               freqPCVmap[freq].PCVvalue[azim][zen] = value;
         }

      }  // end if/else if/else on record type

   }  // end try
   catch(FFStreamError& fse) { GPSTK_RETHROW(fse); }
   }  // end AntexData::ParseDataRecord

   DayTime AntexData::parseTime(const string& line) const
      throw(FFStreamError)
   {
      try
      {
         // default value
         DayTime time = DayTime::BEGINNING_OF_TIME;

         if (line.substr(0,42) == string(42,' '))
            return time;

         // check if the spaces are in the right place - an easy
         // way to check if there's corruption in the file
         // --YYYY----MM----DD----HH----MMsssss.sssssss-----------------
         // 012345678901234567890123456789012345678901234567890123456789
         if((line.substr( 0,2) != string(2,' ')) ||
            (line.substr( 6,4) != string(4,' ')) ||
            (line.substr(12,4) != string(4,' ')) ||
            (line.substr(18,4) != string(4,' ')) ||
            (line.substr(24,4) != string(4,' ')) ||
            (line[43] != ' '))
         {
            FFStreamError e("Invalid time format");
            GPSTK_THROW(e);
         }

         // parse the time
         int year, month, day, hour, min;
         double sec;
   
         year  = asInt(   line.substr( 2, 4));
         month = asInt(   line.substr(10, 4));
         day   = asInt(   line.substr(16, 4));
         hour  = asInt(   line.substr(22, 4));
         min   = asInt(   line.substr(28, 4));
         sec   = asDouble(line.substr(30,13));

         time.setYMDHMS(year, month, day, hour, min, sec);

         return time;
      }
      // string exceptions for substr are caught here
      catch (exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }
      catch (Exception& e)
      {
         string text;
         for(int i=0; i<e.getTextCount(); i++) text += e.getText(i);
         FFStreamError err("Exception in parseTime(): " + text);
         GPSTK_THROW(err);
      }
   }

   string AntexData::writeTime(const DayTime& dt) const
      throw(StringException)
   {
      if(dt == DayTime::BEGINNING_OF_TIME || dt == DayTime::END_OF_TIME)
         return string(43,' ');

      // --YYYY----MM----DD----HH----MMsssss.sssssss-----------------
      // 012345678901234567890123456789012345678901234567890123456789
      string line;
      line  = string(2,' ');
      line += rightJustify(asString<short>(dt.year()),4);
      line += string(4,' ');
      line += rightJustify(asString<short>(dt.month()),2);
      line += string(4,' ');
      line += rightJustify(asString<short>(dt.day()),2);
      line += string(4,' ');
      line += rightJustify(asString<short>(dt.hour()),2);
      line += string(4,' ');
      line += rightJustify(asString<short>(dt.minute()),2);
      line += rightJustify(asString(dt.second(),7),13);

      return line;
   }

} // namespace
