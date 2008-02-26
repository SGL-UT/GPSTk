#pragma ident "$Id: $"

/**
 * @file ComputeWindUp.cpp
 * This class computes the wind-up effect on the phase observables, in radians.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007
//
//============================================================================


#include "ComputeWindUp.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int ComputeWindUp::classIndex = 1400000;


      // Returns an index identifying this object.
   int ComputeWindUp::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string ComputeWindUp::getClassName() const
   { return "ComputeWindUp"; }



      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ComputeWindUp::Process(const DayTime& time,
                                           satTypeValueMap& gData)
   {

         // Compute Sun position at this epoch
      SunPosition sunPosition;
      Triple sunPos(sunPosition.getPosition(time));

         // Define a Triple that will hold satellite position, in ECEF
      Triple svPos(0.0, 0.0, 0.0);

      SatIDSet satRejectedSet;

         // Loop through all the satellites
      satTypeValueMap::iterator it;
      for (it = gData.begin(); it != gData.end(); ++it) 
      {

            // Use ephemeris if satellite position is not already computed
         if( ( (*it).second.find(TypeID::satX) == (*it).second.end() ) ||
             ( (*it).second.find(TypeID::satY) == (*it).second.end() ) ||
             ( (*it).second.find(TypeID::satZ) == (*it).second.end() ) )
         {
            if(pEphemeris==NULL)
            {
                  // If ephemeris is missing, then remove all satellites
               satRejectedSet.insert( (*it).first );
               continue;
            }
            else
            {
                  // Try to get satellite position
                  // if it is not already computed
               try
               {
                     // For our purposes, position at receive time 
                     // is fine enough
                  Xvt svPosVel(pEphemeris->getXvt( (*it).first, time ));

                     // If everything is OK, then continue processing.
                  svPos[0] = svPosVel.x.theArray[0];
                  svPos[1] = svPosVel.x.theArray[1];
                  svPos[2] = svPosVel.x.theArray[2];

               }
               catch(...)
               {
                     // If satellite is missing, then schedule it for removal
                  satRejectedSet.insert( (*it).first );
                  continue;
               }
            }

         }
         else
         {
               // Get satellite position out of GDS
            svPos[0] = (*it).second[TypeID::satX];
            svPos[1] = (*it).second[TypeID::satY];
            svPos[2] = (*it).second[TypeID::satZ];
         }

            // Let's get wind-up value in radians, and insert it
            // in GNSS data structure.
         (*it).second[TypeID::windUp] =
            getWindUp((*it).first, time, svPos, sunPos);

      }
         // Remove satellites with missing data
      gData.removeSatID(satRejectedSet);

      return gData;
   }



      /* Sets name of "PRN_GPS"-like file containing satellite data.
       * @param name      Name of satellite data file.
       */
   ComputeWindUp& ComputeWindUp::setFilename(const string& name)
   {
      fileData = name;
      satData.open(fileData);

      return (*this);
   };



      /* Compute the value of the wind-up, in radians.
       * @param sat       Satellite IDmake
       * @param time      Epoch of interest
       * @param satpos    Satellite position, as a Triple
       * @param sunpos    Sun position, as a Triple
       * @return Wind-up computation, in radians
       */
   double ComputeWindUp::getWindUp(const SatID& satid,
                                   const DayTime& time,
                                   const Triple& sat,
                                   const Triple& sunPosition)
   {

         // Unitary vector from satellite to Earth mass center
      Triple rk( ( (-1.0)*(sat.unitVector()) ) );

         // Unitary vector from Earth mass center to Sun
      Triple ri( sunPosition.unitVector() );

         // rj = rk x ri
      Triple rj(rk.cross(ri));

         // ri = rj x rk
      ri = rj.cross(rk);

         // Let's convert ri, rj to unitary vectors.
         // Now ri, rj, rk form a base in the ECEF reference frame
      ri = ri.unitVector();
      rj = rj.unitVector();


         // Get satellite rotation angle

         // Get vector from Earth mass center to receiver
      Triple rxPos(nominalPos.X(), nominalPos.Y(), nominalPos.Z());

         // Compute unitary vector vector from satellite to RECEIVER
      Triple rrho( (rxPos-sat).unitVector() );

         // Vector from SV to Sun center of mass
      Triple gps_sun( sunPosition-sat );

         // Redefine rk: Unitary vector from SV to Earth mass center
      rk = (-1.0)*(sat.unitVector());

         // Redefine rj: rj = rk x gps_sun, then make sure it is unitary
      rj = (rk.cross(gps_sun)).unitVector();

         // Redefine ri: ri = rj x rk, then make sure it is unitary
         // Now, ri, rj, rk form a base in the satellite body reference 
         // frame, expressed in the ECEF reference frame
      ri = (rj.cross(rk)).unitVector();


         // Projection of "rk" vector to line of sight vector (rrho)
      double zk(rrho.dot(rk));

         // Get a vector without components on rk (i.e., belonging 
         // to ri, rj plane)
      Triple dpp(rrho-zk*rk);

         // Compute dpp components in ri, rj plane
      double xk(dpp.dot(ri));
      double yk(dpp.dot(rj));

         // Compute satellite rotation angle, in radians
      double alpha1(std::atan2(yk,xk));


         // Get receiver rotation angle

         // Redefine rk: Unitary vector from Receiver to Earth mass center
      rk = (-1.0)*(rxPos.unitVector());

         // Let's define a NORTH unitary vector in the Up, East, North 
         // (UEN) topocentric reference frame
      Triple delta(0.0, 0.0, 1.0);

         // Rotate delta to XYZ reference frame
      delta = 
         (delta.R2(nominalPos.geodeticLatitude())).R3(-nominalPos.longitude());


         // Computation of reference trame unitary vectors for receiver
         // rj = rk x delta, and make it unitary
      rj = (rk.cross(delta)).unitVector();

         // ri = rj x rk, and make it unitary
      ri = (rj.cross(rk)).unitVector();

         // Projection of "rk" vector to line of sight vector (rrho)
      zk = rrho.dot(rk);

         // Get a vector without components on rk (i.e., belonging 
         // to ri, rj plane)
      dpp = rrho-zk*rk;

         // Compute dpp components in ri, rj plane
      xk = dpp.dot(ri);
      yk = dpp.dot(rj);

         // Compute receiver rotation angle, in radians
      double alpha2(std::atan2(yk,xk));

      double wind_up(0.0);

         // Find out if satellite belongs to block "IIR", because
         // satellites of block IIR have a 180 phase shift
      if(satData.getBlock( satid, time ) == "IIR")
      {
         wind_up = PI;
      }

      alpha1 = alpha1 + wind_up; 

      double da1(alpha1-phase_satellite[satid].previousPhase);

      double da2(alpha2-phase_station[satid].previousPhase);

         // Let's avoid problems when passing from 359 to 0 degrees.
      phase_satellite[satid].previousPhase =
         phase_satellite[satid].previousPhase + std::atan2( std::sin(da1),
                                                            std::cos(da1) );

      phase_station[satid].previousPhase =
         phase_station[satid].previousPhase + std::atan2( std::sin(da2),
                                                          std::cos(da2) );

         // Compute wind up effect in radians
      wind_up =
         phase_satellite[satid].previousPhase -
         phase_station[satid].previousPhase;

      return wind_up;

   } // End of ComputeWindUp::getWindUp()


} // end namespace gpstk
