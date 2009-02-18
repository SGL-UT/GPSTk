#pragma ident "$Id$"

/**
 * @file IonexStore.cpp
 * Read and store Ionosphere maps. It computes TEC and RMS values with respect
 * to time and receiver position. Based on extracted TEC values, it calculates
 * the ionospheric delay.
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
//  Octavian Andrei - FGI ( http://www.fgi.fi ). 2008
//
//============================================================================


#include "IonexStore.hpp"
#include "GPSGeoid.hpp"                   // geoid.a() = R earth


using namespace gpstk::StringUtils;
using namespace gpstk;

namespace gpstk
{

      // coefficient. See more in Seeber G.(2003), Satellite Geodesy
      // 2nd edition, Walter de Gruyter, p.52-54.
   static const double C2_FACT   = 40.3e+16;


      // Load the given IONEX file
   void IonexStore::loadFile( const std::string& filename )
      throw(FileMissingException)
   {

      try
      {

            // Stream creation
         IonexStream strm(filename.c_str(), std::ios::in);

         if (!strm)
         {

            FileMissingException e( "File " + filename +
                                    " could not be opened.");
            GPSTK_THROW(e);

         }

            // create the header object
         IonexHeader header;
         strm >> header;

         if ( !header.valid )
         {

            FileMissingException e( "File " + filename +
                                    " could not be opened. Check again " +
                                    "the path or the name provided!");
            GPSTK_THROW(e);

         }

            // keep an inventory of the loaded files 
         addFile(filename,header);

            // this map is useful in finding DCB value
         inxDCBMap[header.firstEpoch] = header.svsmap;

            // object data. If valid, add to the map
         IonexData iod;
         while ( strm >> iod && iod.isValid() )
         {
            addMap(iod);
         }

      }
      catch (gpstk::Exception& e)
      {
         GPSTK_RETHROW(e);
      }

   }  // End of method 'IonexStore::loadFile()'



      // Insert a new IonexData object into the store
   void IonexStore::addMap(const IonexData& iod)
      throw()
   {

      DayTime t(iod.time);
      IonexData::IonexValType type(iod.type);

      if (type != IonexData::UN)
      {
         inxMaps[t][type] = iod;
      }

      if (t < initialTime)
      {
         initialTime = t;
      }
      else if (t > finalTime)
      {
         finalTime = t;
      }

   }  // End of method 'IonexStore::addMap()'



      /* Dump the store to the provided std::ostream (std::cout by default).
       *
       * @param s       std::ostream object to dump the data to.
       * @param detail  Determines how much detail to include in the output:
       *                0 list of filenames with their start and stop times.
       *                1 list of filenames with their start, stop times,
       *                  type of data and for how many epochs.
       */
   void IonexStore::dump( std::ostream& s,
                          short detail ) const
      throw()
   {

      s << "IonexStore dump() function" << std::endl;

      std::vector<std::string> fileNames = getFileNames();
      std::vector<std::string>::const_iterator f;

      for (f = fileNames.begin(); f != fileNames.end(); f++)
      {
         s << *f << std::endl;
      }

      s << std::endl;


      if (detail >= 0)
      {

         s << "Data stored for: " << std::endl;
         s << "  # " << fileNames.size() << " files." << std::endl;
         s << "  # " << inxMaps.size() << " epochs" << std::endl;
         s << "  # " << "over time span "<< getInitialTime()
           << " to " << getFinalTime() << "." << std::endl;

         s << std::endl;

         if (detail == 0)
         {
            return;
         }

         s << "--------------------" << std::endl;
         s << "EPOCH"
           << std::setw(21) << "TEC"
           << std::setw(5) << "RMS"<< std::endl;
         s << "--------------------" << std::endl;

         int ntec(0), nrms(0);

         IonexMap::const_iterator it;

         for (it=inxMaps.begin(); it != inxMaps.end(); it++)
         {

            s << it->first << "   ";

            if ( it->second.count(IonexData::TEC) )
            {
               ntec++;
               s << " YES ";
            }
            else
            {
               s << "   ";
            }

            if ( it->second.count(IonexData::RMS) )
            {
               nrms++;
               s << " YES ";
            }
            else
            {
               s << "     ";
            }

            s << std::endl;

         }  // End of 'for (it=inxMaps.begin(); it != inxMaps.end(); it++)...'


         s << "--------------------" << std::endl;
         s << "Total epochs:        "
           << std::setw(5) << ntec
           << std::setw(5) << nrms << std::endl;
         s << "--------------------" << std::endl;

      }  // End of 'if (detail >= 0)...'


      return;


   }  // End of method 'IonexStore::dump()'



      // Remove all data
   void IonexStore::clear()
      throw()
   {

      inxMaps.clear();

      initialTime = DayTime::END_OF_TIME;
      finalTime = DayTime::BEGINNING_OF_TIME;

      return;

   }  // End of method 'IonexStore::clear()'



      /* Get IONEX TEC, RMS and ionosphere height values as a function of
       * epoch and receiver's position.
       *
       * Four interpolation strategies are suported  (see also Ionex manual:
       * http://igscb.jpl.nasa.gov/igscb/data/format/ionex1.pdf )
       *
       * A simple 4-point formula is applied to interpolate between the grid
       * points. See more at IonexData::getValue()
       *
       * @param t          Time tag of signal (DayTime object)
       * @param RX         Receiver position in ECEF cartesian coordinates
       *                   (meters).
       * @param strategy   Interpolation strategy
       *                   (1) take neareast map,
       *                   (2) interpolate between two consecutive maps,
       *                   (3) interpolate between two consecutive rotated
       *                       maps or,
       *                   (4) take neareast rotated map.
       *
       * @return values    TEC, RMS and ionosphere height values
       *                   (Vector object with 3 elements: TEC and RMS are in
       *                   TECU and ionosphere height in KM)
       */
   Vector<double> IonexStore::getIonexValue( const DayTime& t,
                                             const Position& RX,
                                             int strategy ) const
      throw(InvalidRequest)
   {

         // Here we store the necessary IONEX-extracted values
      Vector<double> tecval(3,0.0);
      double ionexHeight;

         // current time check
      if (t < getInitialTime())
      {
         InvalidRequest e("Inadequate data before requested time");
         GPSTK_THROW(e);
      }

      if (t > getFinalTime() )
      {
         InvalidRequest e("Inadequate data after requested time");
         GPSTK_THROW(e);
      }

         //let's define the number of maps to be considered
      int nmap;
      if      (strategy == 1) nmap = 1;
      else if (strategy == 2) nmap = 2;
      else if (strategy == 3) nmap = 2;
      else if (strategy == 4) nmap = 1;
      else
      {
         InvalidRequest e("Invalid interpolation stategy");
         GPSTK_THROW(e);
      }

         // look for valid Ionex maps
      DayTime T[2];

      IonexMap::const_iterator itm = inxMaps.find(t);

      if (itm != inxMaps.end())              // exact match of t
      {

         itm = inxMaps.lower_bound(t);

         T[0] = itm->first;
         T[1] = (++itm)->first;

      }
      else
      {

         itm = inxMaps.lower_bound(t);

         T[1] = itm->first;
         T[0] = (--itm)->first;

      }

         // factors (As in Eq.(3), pag.2 of the manual)
      double f[2];
      try
      {

         f[0] = (T[1]-t) / (T[1]-T[0]);
         f[1] = (t-T[0]) / (T[1]-T[0]);
      }
      catch(...)
      {
         InvalidRequest e("Problems computing 'f[]' parameters.");
         GPSTK_THROW(e);
      }


         // loop over the number of maps considered
      for(int imap = 0; imap < nmap; imap++)
      {

         itm = inxMaps.find(T[imap]);

         IonexValTypeMap ivtm = (*itm).second;

         IonexData iod;
         Position pos;

         if (strategy == 1 || strategy == 2)    // keep fixed position
         {

            pos = Position( RX.geodeticLatitude(),
                            RX.getLongitude(),
                            RX.getHeight(),
                            Position::Geodetic );
         }
         else     // take into account the rotation around the Sun
         {

               // seconds of time to degree (360.0 / 86400.0)
            double sec2deg( 4.16666666666667e-3 );

            pos = Position( RX.geodeticLatitude(),
                            RX.getLongitude() + ( t - T[imap] ) * sec2deg,
                            RX.getHeight(),
                            Position::Geodetic );

         }  // End of 'if (strategy == 1 || strategy == 2)...'


            // Compute TEC value
         if ( ivtm.find(IonexData::TEC) != ivtm.end() )
         {

            iod = ivtm[IonexData::TEC];
            tecval[0] = tecval[0] + f[imap]*iod.getValue(pos,ionexHeight);

         }

            // Compute RMS value
         if ( ivtm.find(IonexData::RMS) != ivtm.end() )
         {

            iod = ivtm[IonexData::RMS];
            tecval[1] = tecval[1] + f[imap]*iod.getValue(pos, ionexHeight);

         }

      }  // End of 'for(int imap = 0; imap < nmap; imap++)...'


         // ionosphere height
      tecval[2] = ionexHeight;

      return tecval;

   }  // End of method 'IonexStore::getIonexValue()'



      /* Get ionospheric slant delay for a given frequency
       *
       * @param elevation     Time tag of signal (DayTime object)
       * @param tecval        TEC value as derived from IONEX file (TECU)
       * @param ionoHeight    Ionosphere height as derived from IONEX file
       *                      (KM).
       * @param freq          Frequency value, in Hz
       *
       * @return              Ionosphere slant delay (meters)
       */
   double IonexStore::getIono( const double& elevation,
                               const double& tecval,
                               const double& ionoHeight,
                               const double& freq ) const
      throw (InvalidParameter)
   {

      if (tecval < 0)
      {
         InvalidParameter e("Invalid TEC parameter.");
         GPSTK_THROW(e);
      }

      if (ionoHeight < 0)
      {
         InvalidParameter e("Invalid IONEX height of the ionosphere.");
         GPSTK_THROW(e);
      }

      if (elevation < 0.0)
      {

         return 0.0;

      }
      else
      {

         return ( C2_FACT / (freq * freq) * tecval
                         * iono_mapping_function(elevation, ionoHeight) );

      }

   }  // End of method 'IonexStore::getIono()'



      // ionosphere mapping function
   double IonexStore::iono_mapping_function( const double& elevation,
                                             const double& ionoHeight) const
   {

         // Need Earth's radius to compute zenith angle at the observing site
      GPSGeoid geoid;
      double Re = geoid.a();          // Earth's radius is in meters
      double z0 = 90.0 - elevation;

         // zenith angle of the ionospheric point (IP)
         // As explained in: Hofmann-Wellenhof et al. (2004) - GPS Theory and
         // practice, 5th edition, SpringerWienNewYork, Chapter 6.3, pg. 102
         //
         // NB: ionoHeight is in km, thus it has to be converted to meters
      double sinzip  = Re / (Re + ionoHeight*1000.0) * std::sin(z0*DEG_TO_RAD);
      double ziprad  = std::asin(sinzip);
      double map     = 1.0/std::cos(ziprad);

      return map;

   }  // End of method 'IonexStore::iono_mapping_function()'



      /* Find a DCB value
       *
       * @param sat     SatID of satellite of interest
       * @param t       Time to search for DCB
       *
       * @return        DCB value found (nanoseconds).
       *
       * @throw InvalidRequest object thrown when no DCB value is found
       */
   double IonexStore::findDCB( const SatID sat,
                               const DayTime& time ) const
      throw(InvalidRequest)
   {

         // current time check. This is passed even if there are gaps
      if ( time < getInitialTime() )
      {
         InvalidRequest e("Inadequate data before requested time");
         GPSTK_THROW(e);
      }

      if ( time > getFinalTime() )
      {
         InvalidRequest e("Inadequate data after requested time");
         GPSTK_THROW(e);
      }

      double dt(0.0);
      IonexDCBMap::const_iterator itm = inxDCBMap.begin();

         // looping through the map
      while ( itm != inxDCBMap.end() )
      {

            // let's get the relative reference
         dt = time - itm->first;

            // this means we dont have maps for this day and there is a gap
         if (dt < 0 )
         {

            InvalidRequest e( "Inadequate data after requested time: " +
                              time.asString() );
            GPSTK_THROW(e);

         }  // works fine for consecutive files, otherwise last epoch is thrown
         else
         {

            if (dt < 86400)
            {

               IonexHeader::SatDCBMap satdcb( itm->second );
               IonexHeader::SatDCBMap::const_iterator iprn( satdcb.find(sat) );

                  // if satellite is not found, throw
               if ( iprn == satdcb.end() )
               {

                  InvalidRequest e( "There is no DCB value for satellite " +
                                    asString(sat) );
                  GPSTK_THROW(e);

               }
               else     // ... otherwise, fetch the value
               {

                  return iprn->second.bias;

               }  // End of 'if ( iprn == satdcb.end() ) ... else ...'

            }
            else  // If everything is fine, then move forward in the map
            {

               itm++;

            }  // End of 'if (dt < 86400) ... else ...'

         }  // End of 'if (dt < 0 ) ... else ...'

      }  // End of 'while ( itm != inxDCBMap.end() )'


         // You should never get here, but just in case
      return 0.0;

   }  // End of method 'IonexStore::findDCB()'



}  // End of namespace gpstk
