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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2009, 2010, 2011
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
 * @file ComputeIonoModel.cpp
 * This is a class to compute the main values related to a given
 * GNSS ionospheric model.
 */

#include "ComputeIonoModel.hpp"
#include "RinexUtilities.hpp"
#include "RinexNavStream.hpp"
#include "RinexNavHeader.hpp"
//#include "Logger.hpp"

namespace gpstk
{

   using namespace std;

      // Returns a string identifying this object.
   std::string ComputeIonoModel::getClassName() const
   { return "ComputeIonoModel"; }



      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling a modeling object.
       *
       * @param time      Epoch.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ComputeIonoModel::Process( const CommonTime& time,
                                               satTypeValueMap& gData )
      throw(ProcessingException)
   {

      try
      {
         
         SatIDSet satRejectedSet;

         Position rxPos(nominalPos[0],nominalPos[1],nominalPos[2],
            Position::Cartesian);

            // Loop through all the satellites
         satTypeValueMap::iterator stv;
         for(stv = gData.begin(); stv != gData.end(); ++stv) 
         {

            Position svPos(0.0, 0.0, 0.0, Position::Cartesian);
            
               // If elevation or azimuth is missing, then remove satellite
            if( stv->second.find(TypeID::elevation) == stv->second.end() ||
               stv->second.find(TypeID::azimuth)   == stv->second.end() )
            {

               satRejectedSet.insert( stv->first );

               continue;

            }
            
            const double elevation = (*stv).second[TypeID::elevation];
            const double azimuth = (*stv).second[TypeID::azimuth]; 

            double ionL1 = 0.0;
            
            if(ionoType == Ionex)
            {
               try
               {
                  //const string mapType = "SLM";
                  //const double ionoHeight = 450000.0;

                  const string mapType = "MSLM";
                  const double ionoHeight = 506700.0;

                  Position IPP = rxPos.getIonosphericPiercePoint(elevation,
                     azimuth,
                     ionoHeight);

                  Position pos(IPP);
                  pos.transformTo(Position::Geocentric);

                  Triple val = gridStore.getIonexValue( time, pos );

                  double tecval = val[0];

                  (void)gridStore.iono_mapping_function(elevation, mapType);
                  ionL1 = gridStore.getIonoL1(elevation, tecval, mapType);
               }
               catch(InvalidRequest& e)
               {
                  satRejectedSet.insert(stv->first);
                  continue;
               }
            }
            else if(ionoType == Klobuchar)
            {
               ionL1 = klbStore.getCorrection(time,rxPos,elevation,azimuth);
            }
            else if(ionoType == DualFreq)
            {
               const double gamma = (L1_FREQ_GPS/L2_FREQ_GPS) * (L1_FREQ_GPS/L2_FREQ_GPS);

               double P1(0.0);
               if(stv->second.find(TypeID::P1)==stv->second.end())
               {
                  if(stv->second.find(TypeID::C1)!=stv->second.end())
                  {
                     P1 = stv->second[TypeID::C1];
                  }
               }
               else
               {
                  P1 = stv->second[TypeID::P1];
               }

               double P2(0.0);
               if(stv->second.find(TypeID::P2)!=stv->second.end())
               {
                  P2 = stv->second[TypeID::P2];
               }
               
               if( P1!=0 && P2!=0 )
               {
                  ionL1 = (P1-P2)/(1.0-gamma);
               }
            }

            double ionL2 = ionL1 * (L1_FREQ_GPS/L2_FREQ_GPS) * (L1_FREQ_GPS/L2_FREQ_GPS);
            double ionL5 = ionL1 * (L1_FREQ_GPS/L5_FREQ_GPS) * (L1_FREQ_GPS/L5_FREQ_GPS);
            
               // TODO: more frequency later

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
                                + u.what() );

         GPSTK_THROW(e);

      }

   } // End ComputeIonoModel::Process()


   ComputeIonoModel& ComputeIonoModel::setKlobucharModel(const double a[4], 
                                                         const double b[4])
   {
      IonoModel ionModel(a,b);
      klbStore.addIonoModel(CommonTime::BEGINNING_OF_TIME,ionModel);
      ionoType = Klobuchar;

      return (*this);
   }

   ComputeIonoModel& ComputeIonoModel::setKlobucharModel(const IonoModel& im)
   {
      klbStore.addIonoModel(CommonTime::BEGINNING_OF_TIME, im);
      ionoType = Klobuchar;

      return (*this);
   }

   ComputeIonoModel& ComputeIonoModel::setklobucharModel(const std::string& brdcFile)
   {
      if( isRinexNavFile( brdcFile ) )
      {
         RinexNavStream nstrm(brdcFile.c_str());
         nstrm.exceptions(fstream::failbit);

         try
         {
            RinexNavHeader rnh;
            nstrm >> rnh;
            nstrm.close();

            setKlobucharModel(rnh.ionAlpha,rnh.ionBeta);
         }
         catch (Exception& e)
         {
            nstrm.close();

            GPSTK_RETHROW(e);
         }
      }
      else
      {
         Exception e("The input is not a rinex nav file:" + brdcFile);
         GPSTK_THROW(e);
      }
      

      return (*this);
   }

   ComputeIonoModel& ComputeIonoModel::setIonosphereMap(const string& ionexFile)
   {
      gridStore.clear();
      gridStore.loadFile(ionexFile);
      ionoType = Ionex;

      return (*this);
   }

} // End of namespace gpstk
