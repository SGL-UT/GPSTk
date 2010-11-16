#pragma ident "$Id$"

/**
 * @file ComputeIonoModel.cpp
 * This is a class to compute the main values related to a given
 * GNSS ionospheric model.
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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010, 2011
//
//============================================================================


#include "ComputeIonoModel.hpp"


namespace gpstk
{

   using namespace std;

      // Index initially assigned to this class
   int ComputeIonoModel::classIndex = 5500000;


      // Returns an index identifying this object.
   int ComputeIonoModel::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string ComputeIonoModel::getClassName() const
   { return "ComputeTropModel"; }



      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling a modeling object.
       *
       * @param time      Epoch.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ComputeIonoModel::Process( const DayTime& time,
                                         satTypeValueMap& gData )
      throw(ProcessingException)
   {

      try
      {
         
         SatIDSet satRejectedSet;

         Position rxPos(nominalPos[0],nominalPos[1],nominalPos[2],
            Position::Cartesian);

         Geodetic rxGeo(rxPos.getGeodeticLatitude(),
            rxPos.getLongitude(),
            rxPos.getAltitude());
                
            // Loop through all the satellites
         satTypeValueMap::iterator stv;
         for(stv = gData.begin(); stv != gData.end(); ++stv) 
         {

            Position svPos(0.0, 0.0, 0.0, Position::Cartesian);
            
               // Check if satellite position is not already computed
            if( ( (*stv).second.find(TypeID::satX) == (*stv).second.end() ) ||
               ( (*stv).second.find(TypeID::satY) == (*stv).second.end() ) ||
               ( (*stv).second.find(TypeID::satZ) == (*stv).second.end() ) )
            {

               // If satellite position is missing, then schedule this 
               // satellite for removal
               satRejectedSet.insert( (*stv).first );

               continue;

            }
            else
            {
               // Get satellite position out of GDS
               svPos[0] = (*stv).second[TypeID::satX];
               svPos[1] = (*stv).second[TypeID::satY];
               svPos[2] = (*stv).second[TypeID::satZ];

            }  // End of 'if( ( (*it).second.find(TypeID::satX) == ...'
            
            

            double elevation = rxPos.elevation(svPos);
            double azimuth = rxPos.azimuth(svPos);

            double ionL1 = 0.0;
            
            if(ionoType == GridTec)
            {
               try
               {
                  Position IPP = rxPos.getIonosphericPiercePoint(elevation,
                     azimuth,
                     506700.0);

                  Position pos(IPP);
                  pos.transformTo(Position::Geocentric);

                  Triple val = gridStore.getIonexValue( time, pos );

                  double tecval = val[0];

                  double ionMap = gridStore.iono_mapping_function(elevation,
                     "MSLM");
                  
                  ionL1 = gridStore.getIonoL1(elevation,
                     tecval,
                     "MSLM");
               }
               catch(InvalidRequest& e)
               {
                  satRejectedSet.insert(stv->first);
                  continue;
               }

            }

            if(ionoType == Klobuchar)
            {
               ionL1 = klbStore.getCorrection(time,rxGeo,elevation,azimuth);
            }

            double ionL2 = ionL1 * (L1_FREQ/L2_FREQ) * (L1_FREQ/L2_FREQ);
            double ionL5 = ionL1 * (L1_FREQ/L5_FREQ) * (L1_FREQ/L5_FREQ);
            
            // TODO
            // more frequency later

               // Now we have to add the new values to the data structure
            (*stv).second[TypeID::ionoL1] = ionL1;
            (*stv).second[TypeID::ionoL2] = ionL2;
            (*stv).second[TypeID::ionoL5] = ionL5;

         }  // End of loop 'for(stv = gData.begin()...'

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         return gData;

      }   // End of try...
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   } // End ComputeIonoModel::Process()


   ComputeIonoModel& ComputeIonoModel::setKlobucharModel(const double a[4], 
                                                         const double b[4])
   {
      IonoModel ionModel(a,b);
      klbStore.addIonoModel(DayTime::BEGINNING_OF_TIME,ionModel);
      
      ionoType = Klobuchar;

      return (*this);
   }

   ComputeIonoModel& ComputeIonoModel::setIonosphereMap(const string& ionexFile)
   {
      gridStore.clear();
      gridStore.loadFile(ionexFile);

      ionoType = GridTec;

      return (*this);
   }

} // End of namespace gpstk
