#pragma ident "$Id$"

/**
 * @file ComputeSatPCenter.hpp
 * This class computes the satellite antenna phase correction, in meters.
 */

#ifndef COMPUTESATPCENTER_HPP
#define COMPUTESATPCENTER_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008
//
//============================================================================



#include <string>
#include "ProcessingClass.hpp"
#include "Triple.hpp"
#include "Position.hpp"
#include "SunPosition.hpp"
#include "XvtStore.hpp"
#include "SatDataReader.hpp"
#include "geometry.hpp"



namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class computes the satellite antenna phase correction, in meters.
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *      // Create the input obs file stream
       *   RinexObsStream rin("ebre0300.02o");
       *
       *      // Loads precise ephemeris object with file data
       *   SP3EphemerisStore SP3EphList;
       *   SP3EphList.loadFile("igs11513.sp3");
       *
       *      // Sets nominal position of receiver
       *   Position nominalPos(4833520.3800, 41536.8300, 4147461.2800);
       *
       *   gnssRinex gRin;
       *
       *   ComputeSatPCenter svPcenter( SP3EphList,
       *                                nominalPos );
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> svPcenter;
       *   }
       * @endcode
       *
       * The "ComputeSatPCenter" object will visit every satellite in the GNSS
       * data structure that is "gRin" and will compute the corresponding
       * satellite antenna phase correction, in meters.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the "satPCenter" TypeID inserted in it.
       * Be warned that if a given satellite does not have the required data,
       * it will be summarily deleted from the data structure.
       *
       * \warning The ComputeSatPCenter objects generate corrections that are
       * interpreted as an "advance" in the signal, instead of a delay.
       * Therefore, those corrections always hava a negative sign.
       *
       */
   class ComputeSatPCenter : public ProcessingClass
   {
   public:

         /// Default constructor
      ComputeSatPCenter()
         : pEphemeris(NULL), nominalPos(0.0, 0.0, 0.0),
           satData("PRN_GPS"), fileData("PRN_GPS")
      { setIndex(); };


         /** Common constructor
          *
          * @param ephem     Satellite ephemeris.
          * @param stapos    Nominal position of receiver station.
          * @param filename  Name of "PRN_GPS"-like file containing
          *                  satellite data.
          *
          * @warning If filename is not given, this class will look for a
          * file named "PRN_GPS" in the current directory.
          */
      ComputeSatPCenter( XvtStore<SatID>& ephem,
                         const Position& stapos,
                         string filename="PRN_GPS" )
         : pEphemeris(&ephem), nominalPos(stapos), satData(filename),
           fileData(filename)
      { setIndex(); };


         /** Common constructor
          *
          * @param stapos    Nominal position of receiver station.
          * @param filename  Name of "PRN_GPS"-like file containing
          *                  satellite data.
          *
          * @warning If filename is not given, this class will look for a
          * file named "PRN_GPS" in the current directory.
          */
      ComputeSatPCenter( const Position& stapos,
                         string filename="PRN_GPS" )
         : pEphemeris(NULL), nominalPos(stapos), satData(filename),
           fileData(filename)
      { setIndex(); };


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param time      Epoch corresponding to the data.
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const DayTime& time,
                                        satTypeValueMap& gData )
         throw(ProcessingException);


         /** Returns a gnnsSatTypeValue object, adding the new data
          *  generated when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException)
      { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnnsRinex object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException)
      { Process(gData.header.epoch, gData.body); return gData; };


         /// Returns name of "PRN_GPS"-like file containing satellite data.
      virtual string getFilename(void) const
      { return fileData; };


         /** Sets name of "PRN_GPS"-like file containing satellite data.
          * @param name      Name of satellite data file.
          */
      virtual ComputeSatPCenter& setFilename(const string& name);


         /// Returns nominal position of receiver station.
      virtual Position getNominalPosition(void) const
      { return nominalPos; };


         /** Sets  nominal position of receiver station.
          * @param stapos    Nominal position of receiver station.
          */
      virtual ComputeSatPCenter& setNominalPosition(const Position& stapos)
        { nominalPos = stapos; return (*this); };


         /// Returns a pointer to the satellite ephemeris object
         /// currently in use.
      virtual XvtStore<SatID> *getEphemeris(void) const
      { return pEphemeris; };


         /** Sets satellite ephemeris object to be used.
          * @param ephem     Satellite ephemeris object.
          */
      virtual ComputeSatPCenter& setEphemeris(XvtStore<SatID>& ephem)
      { pEphemeris = &ephem; return (*this); };


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~ComputeSatPCenter() {};


   private:


         /// Satellite ephemeris to be used
      XvtStore<SatID> *pEphemeris;


         /// Receiver position
      Position nominalPos;


         /// Object to read satellite data file (PRN_GPS)
      SatDataReader satData;


         /// Name of "PRN_GPS"-like file containing satellite data.
      string fileData;


         /** Compute the value of satellite antenna phase correction, in meters
          * @param satid     Satellite ID
          * @param time      Epoch of interest
          * @param satpos    Satellite position, as a Triple
          * @param sunpos    Sun position, as a Triple
          *
          * @return Satellite antenna phase correction, in meters.
          */
      virtual double getSatPCenter( const SatID& satid,
                                    const DayTime& time,
                                    const Triple& satpos,
                                    const Triple& sunPosition );


         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


   }; // End of class 'ComputeSatPCenter'

      //@}

}  // End of namespace gpstk
#endif // COMPUTESATPCENTER_HPP
