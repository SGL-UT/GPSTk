#pragma ident "$Id$"

/**
 * @file AntexData.hpp
 * Encapsulate data from ANTEX (Antenna Exchange) format files, including both
 * receiver and satellite antennas, ANTEX file I/O, discrimination between different
 * satellite antennas based on system, PRN and time, and computation of phase center
 * offsets and variations.
 */

#ifndef ANTEX_DATA_HPP
#define ANTEX_DATA_HPP

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

#include <string>
#include <vector>
#include <map>

#include "AntexBase.hpp"
#include "FFStream.hpp"
#include "DayTime.hpp"
#include "Triple.hpp"

namespace gpstk
{
   /** @addtogroup Antex */
   //@{

      /// Antex antenna data record: PCOs and PCVs for one antenna.
      /// Do not attempt to use an object that is not valid (cf. isValid()).
      ///
      /// NB. Optional data should be accessed only if the corresponding 'valid'
      /// string is true; e.g. if(valid & validFromValid) then validFrom may be used.
      ///
      /// NB. In calls to the 'get' routines,
      ///        double total_PCO = getTotalPhaseCenterOffset(freq, az, el_nad)
      ///        Triple PCO = getPhaseCenterOffset(freq)
      ///        double PCV = getPhaseCenterVariation(freq, az, el_nad),
      /// receivers and satellites (transmitters) are treated differently, in that
      /// receivers call with elevation angle (from North-East plane toward Up) while
      /// satellites call with nadir angle (from Z axis - the bore-sight direction).
      ///
      /// NB. The return value of getPhaseCenterOffset is a vector (Triple) PCO, in
      /// the appropriate coordinate system (NEU for Rx, XYZ or body for SV), that is
      /// defined as the vector from the reference point (RP) to the actual phase
      /// center (PC). The RP is the ARP (Antenna Reference Point) for receivers and
      /// the COM (Center Of Mass) for satellites.
      ///
      /// NB. The PCV and total_PCO value returned by the other two routines has the
      /// same sense as the PCO vector; that is the total offset is defined as
      ///   PCO vector - PCV * LOS
      /// where LOS is a unit vector along the line of sight (defined by the azimuth
      /// and elevation/nadir angle passed into the routines), all in the appropriate
      /// coordinate system (receiver NEU or satellite body XYZ).
      ///
      /// NB. Thus when correcting a measured range for the Receiver's total phase
      /// center offset one would subtract the total_PCO value (along the line of
      /// sight) from the measured range, that is (scalar equation, millimeters):
      ///   Range(corr) = Range(meas) + total_PCO; // OR
      ///                                -->     -->
      ///   Range(corr) = Range(meas) + [PCO dot LOS - PCV];
      /// however when correcting the satellite (COM) position for the Satellite's
      /// total phase center offset one would add the total vector offset
      /// (PCO - PCV*LOS) to the satellite COM (vector) position (in a consistent
      /// coordinate system, e.g. ECEF XYZ), that is (vector equation):
      ///   -->        -->        -->         -->
      ///   SV(corr) = SV(COM) + [PCO - PCV * LOS]
      ///
      /// NB. the PCV data is stored in a map <zenith angle, value> and the
      /// getPhaseCenterVariation() routine simply interpolates this map WITHOUT
      /// changing the sign of the value - it is the same as that in the ANTEX file.
      ///
      /// @sa gpstk::AntexStream and gpstk::AntexHeader.
   class AntexData : public AntexBase
   {
   public:
      /// @name AntexDataFormatStrings
      /// ANTEX Data Formatting Strings
      //@{
      static const std::string startAntennaString;    ///< "START OF ANTENNA"
      static const std::string typeSerNumString;      ///< "TYPE / SERIAL NO"
      static const std::string methodString;          ///< "METH / BY / # / DATE"
      static const std::string daziString;            ///< "DAZI"
      static const std::string zenithString;          ///< "ZEN1 / ZEN2 / DZEN"
      static const std::string numFreqString;         ///< "# OF FREQUENCIES"
      static const std::string validFromString;       ///< "VALID FROM"
      static const std::string validUntilString;      ///< "VALID UNTIL"
      static const std::string sinexCodeString;       ///< "SINEX CODE"
      static const std::string dataCommentString;     ///< "COMMENT"
      static const std::string startFreqString;       ///< "START OF FREQUENCY"
      static const std::string neuFreqString;         ///< "NORTH / EAST / UP"
      static const std::string endOfFreqString;       ///< "END OF FREQUENCY"
      static const std::string startFreqRMSString;    ///< "START OF FREQ RMS"
      static const std::string neuFreqRMSString;      ///< "NORTH / EAST / UP"
      static const std::string endOfFreqRMSString;    ///< "END OF FREQ RMS"
      static const std::string endOfAntennaString;    ///< "END OF ANTENNA"
      //@}

         /// Validity bits for the ANTEX Data
         /// NB. if version is updated, add allValid<ver> and update isValid()
      enum validBits
      {
         startAntennaValid = 0x00001,  ///< "START OF ANTENNA"       Required
         typeSerNumValid   = 0x00002,  ///< "TYPE / SERIAL NO"       Required
         methodValid       = 0x00004,  ///< "METH / BY / # / DATE"   Required
         daziValid         = 0x00008,  ///< "DAZI"                   Required
         zenithValid       = 0x00010,  ///< "ZEN1 / ZEN2 / DZEN"     Required
         numFreqValid      = 0x00020,  ///< "# OF FREQUENCIES"       Required
         validFromValid    = 0x00040,  ///< "VALID FROM"
         validUntilValid   = 0x00080,  ///< "VALID UNTIL"
         sinexCodeValid    = 0x00100,  ///< "SINEX CODE"
         dataCommentValid  = 0x00200,  ///< "COMMENT"
         startFreqValid    = 0x00400,  ///< "START OF FREQUENCY"     Required
         neuFreqValid      = 0x00800,  ///< "NORTH / EAST / UP"      Required
         endOfFreqValid    = 0x01000,  ///< "END OF FREQUENCY"       Required
         startFreqRMSValid = 0x02000,  ///< "START OF FREQ RMS"
         neuFreqRMSValid   = 0x04000,  ///< "NORTH / EAST / UP"
         endOfFreqRMSValid = 0x08000,  ///< "END OF FREQ RMS"
         endOfAntennaValid = 0x10000,  ///< "END OF ANTENNA"         Required
         allValid13        = 0x11C3F   ///< mask for all required valid fields
      };

      /// Values of 'type' that are satellites
      /// NB. keep this updated from the IGS file 'rcvr_ant.tab'
      static const std::vector<std::string> SatelliteTypes;

      /// map from zenith angle (degrees) to PC offset (millimeters)
      typedef std::map<double, double> zenOffsetMap;

      /// map from azimuth angle (deg) to zenOffsetMap
      /// the zenOffsetMap WITHOUT azimuth dependence (NOAZI) will be
      /// azimZenMap[-1.0] (this may be the only entry)
      typedef std::map<double, zenOffsetMap> azimZenMap;

      /// class encapsulating the PCOs and PCVs of the antenna. See the ANTEX
      /// documentation for discussion of how the PCO/Vs are defined, sign conventions
      /// and how to apply the PCOs.
      class antennaPCOandPCVData {
      public:
         /// nominal phase center offsets in mm, and RMS values,
         /// in NEU coordinates (for Receiver antennas)
         /// or XYZ (for Satellite antennas); from "NORTH / EAST / UP" record
         /// RMS values are OPTIONAL
         double PCOvalue[3],PCOrms[3];

         /// if false, there is no azimuth dependence in the PCVs
         /// and only PCV[0.0] is defined.
         bool hasAzimuth;

         /// map from azimuth to <zenith,offset> map:
         /// PCVvalues[azim][zen] = offset in mm from the nominal
         /// PCVrms[azim][zen] = RMS of these values, also in mm.
         /// if there is no azimuth dependence, there will be
         /// only one entry in this map, with azimuth = -1.0
         /// RMS values are OPTIONAL
         azimZenMap PCVvalue, PCVrms;

      }; // end of class antennaPCOandPCVData

      // member data
      /// Bits of valid are set when corresponding labels are found and data defined
      unsigned long valid;

      /// if true, PCOs are absolute, else they are relative to another antenna
      bool absolute;

      /// if true, this is a receiver antenna, otherwise its a satellite;
      /// this flag is set based on the IGS codes kept in array SatelliteTypes.
      /// NB. this flag need not be used, if you know which antenna you have;
      ///     however if used, the array SatelliteTypes must be kept updated.
      bool isRxAntenna;

      /// PRN and SVN numbers; used only in the case of satellite antennas, and
      /// may not be present, in which case these are both -1.
      /// NB. PRNs apply to GLONASS as well as GPS
      int PRN, SVN;

      /// system character: G or blank GPS, R GLONASS, E GALILEO, M MIXED
      /// taken from START OF FREQUENCY record (not header)
      char systemChar;

      /// number of frequencies stored, equal to number of keys in map
      /// from "# OF FREQUENCIES" record
      unsigned int nFreq;

      /// delta azimuth (degrees) stored in azimZenMap
      /// equal to 0 if there is no azimuth dependence
      /// from "DAZI" record
      double azimDelta;

      /// minimum, maximum and delta zenith (degrees) stored in zenOffsetMap
      /// from "ZEN1 / ZEN2 / DZEN" record
      double zenRange[3];

      /// time limits of validity (OPTIONAL); otherwise set to BEGINNING and END
      /// from "VALID FROM" and "VALID UNTIL" records
      /// keep the string version for file I/O b/c sometimes the time is of the form
      /// 1994     4    17    23    59   59.9999999                 VALID UNTIL
      /// and converting this to DayTime replaces it with ... 24 0 0.000
      DayTime validFrom,validUntil;
      std::string stringValidFrom, stringValidUntil;

      /// map from frequency (1,2,...nFreq) to antennaPCOandPCVData
      std::map<int, antennaPCOandPCVData> freqPCVmap;

      std::string type;     ///< antenna type from "TYPE / SERIAL NO"
      std::string serialNo; ///< antenna serial number from "TYPE / SERIAL NO"
      std::string satCode;  ///< satellite code from "TYPE / SERIAL NO"
      std::string cospar;   ///< satellite COSPAR ID from "TYPE / SERIAL NO"
      std::string method;   ///< calibration method from "METH / BY / # / DATE"
      std::string agency;   ///< agency from "METH / BY / # / DATE"
      int noAntCalibrated;  ///< num. of ant. calibrated from "METH / BY / # / DATE"
      std::string date;     ///< date from "METH / BY / # / DATE"
      std::string sinexCode;///< name of ant. cal. model from "SINEX CODE" OPTIONAL

      /// comments found in the data portion of the file
      std::vector<std::string> commentList;  ///< Comments in data (OPTIONAL)

      //------------------------------------------------------------------------------
      // member functions

      /// Constructor.
      AntexData() : validFrom(DayTime::BEGINNING_OF_TIME),
                    validUntil(DayTime::END_OF_TIME),
                    valid(0), PRN(0), SVN(0), nFreq(0), absolute(true) {}

      /// Destructor
      virtual ~AntexData() {}

      /// AntexData is a "data", so this function always returns true.
      virtual bool isData() const {return true;}

      /// Convenience function returns true only if a valid object
      bool isValid(void) const { return ((valid & allValid13) == allValid13); }

      /// @return true if the antenna object is valid at the given time.
      /// Base on the 'validFrom' and 'validUntil' fields.
      /// @return true if the input time is either BEGINNING_ or END_OF_TIME
      /// @return true if the 'valid' time limits are not given.
      /// NB. useful when adding satellite antennas for processing with a dataset;
      /// pass any time tag from the dataset.
      bool isValid(DayTime& time) const throw();

      /// Generate a name from type and serial number
      inline std::string name(void) const throw()
      {
         if(!isValid())
            return std::string("invaild");
         if(isRxAntenna)
            return (type);
         else
            return (type + std::string("/") + serialNo);
      }

      /// Compute the total phase center offset at the given azimuth and elev_nadir,
      /// including both nominal offset (PCO) and variation (PCV).
      /// NB. see documentation of the class for coordinates, signs and application.
      /// @param freq frequency (usually 1 or 2)
      /// @param azimuth the azimuth angle in degrees, from N going toward E for
      ///        receivers, or from X going toward Y for satellites
      /// @param elev_nadir elevation in deg from horizontal (North-East) plane for
      //         receivers, or nadir angle in degrees from Z axis for satellites
      /// @return total phase center offset in millimeters
      /// @throw  if this object is invalid
      ///         if frequency does not exist for this data
      ///         if azimuth is out of range; azimuth is replaced with azim mod 360
      double getTotalPhaseCenterOffset(const int freq,
                                       const double azimuth,
                                       const double elevation) const
         throw(Exception);

      /// Get the PC offset values in mm (only, NOT the phase center variations, which
      /// should be computed using getPhaseCenterVariations() and added to the PCOs
      /// to get the total phase center offset).
      /// NB. see documentation of the class for coordinates, signs and application.
      /// @param freq frequency (usually 1 or 2)
      /// @return Triple containing offsets in millimeters, in appropriate coordinate
      ///                system (satellite-based XYZ or receiver-based NEU).
      /// @throw  if this object is invalid
      ///         if frequency does not exist for this data
      Triple getPhaseCenterOffset(const int freq) const
         throw(Exception);

      /// Compute the phase center variation at the given azimuth and elev_nadir
      /// NB. see documentation of the class for coordinates, signs and application.
      /// @param freq frequency (usually 1 or 2)
      /// @param azimuth the azimuth angle in degrees, from N going toward E for
      ///        receivers, or from X going toward Y for satellites
      /// @param elev_nadir elevation in deg from horizontal (North-East) plane for
      //         receivers, or nadir angle in degrees from Z axis for satellites
      /// @return phase center offset in millimeters
      /// @throw  if this object is invalid
      ///         if frequency does not exist for this data
      ///         if azimuth is out of range, azimuth is replaced with azim % 360
      double getPhaseCenterVariation(const int freq,
                                     const double azimuth,
                                     const double elev_nadir) const
         throw(Exception);

      /// Dump AntexData. Set detail = 0 for type, serial no., sat codes only;
      /// = 1 for all information except phase center offsets, = 2 for all data.
      virtual void dump(std::ostream& s, int detail=0) const;

   protected:
      /// Find zenith angles bracketing the input zenith angle within the given map,
      /// and the corresponding PCOs.
      void evaluateZenithMap(const double& zen,
                             const zenOffsetMap& eomap,
                             double& zen_lo, double& zen_hi,
                             double& pco_lo, double& pco_hi) const throw();

      /// Writes a correctly formatted record from this data to stream \a s.
      virtual void reallyPutRecord(FFStream& s) const 
         throw(std::exception, FFStreamError,
            StringUtils::StringException);

      /// This functions obtains Antex antenna record from the given FFStream.
      /// If there is an error in reading from the stream, it is reset
      /// to its original position and its fail-bit is set.
      /// @throws StringException when a StringUtils function fails
      /// @throws FFStreamError when exceptions(failbit) is set and
      ///  a read or formatting error occurs.  This also resets the
      ///  stream to its pre-read position.
      virtual void reallyGetRecord(FFStream& s) 
         throw(std::exception, FFStreamError,
               StringUtils::StringException);

   private:
      /// helper routine to throw when records are out of order
      /// throws if valid contains test (test & valid), otherwise does nothing
      void throwRecordOutOfOrder(unsigned long test, std::string& label);

      /// parse a line from the Antex file, filling the data object
      void ParseDataRecord(std::string& line)
         throw(FFStreamError);

      /// Writes the daytime object into Antex ('VALID FROM') format.
      /// If it's a bad time, it will return blanks.
      std::string writeTime(const DayTime& dt) const
         throw(StringUtils::StringException);

      /// This function constructs a DayTime object from the line for VALID FROM
      /// and VALID UNTIL records; default is to return BEGINNING_OF_TIME
      /// @param line the encoded time string found in the Antex record.
      DayTime parseTime(const std::string& line) const
         throw(FFStreamError);

   }; // class AntexData

   //@}

} // namespace

#endif
