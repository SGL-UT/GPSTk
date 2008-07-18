#pragma ident "$Id: PhaseCodeAlignment.cpp $"

/**
 * @file PhaseCodeAlignment.cpp
 * This class aligns phase with code measurements.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008
//
//============================================================================


#include "PhaseCodeAlignment.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int PhaseCodeAlignment::classIndex = 2950000;


      // Returns an index identifying this object.
   int PhaseCodeAlignment::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string PhaseCodeAlignment::getClassName() const
   { return "PhaseCodeAlignment"; }


      /* Common constructor
       *
       * @param phase            Phase TypeID.
       * @param code             Code TypeID.
       * @param wavelength       Phase wavelength, in meters.
       * @param useArc           Whether satellite arcs will be used or not.
       */
   PhaseCodeAlignment::PhaseCodeAlignment( const TypeID& phase,
                                           const TypeID& code,
                                           const double wavelength,
                                           bool useArc )
      : phaseType(phase), codeType(code), useSatArcs(useArc),
        watchCSFlag(TypeID::CSL1)
   {

         // Check that wavelength is bigger than zero
      if (wavelength > 0.0)
      {
         phaseWavelength = wavelength;
      }
      else
      {
         phaseWavelength = 0.107;   // Be default, LC wavelength
      }

      setIndex();

   }  // End of 'PhaseCodeAlignment::PhaseCodeAlignment()'


      /* Method to set the phase wavelength to be used.
       *
       * @param wavelength       Phase wavelength, in meters.
       */
   PhaseCodeAlignment& PhaseCodeAlignment::setPhaseWavelength(double wavelength)
   {

         // Check that wavelength is bigger than zero
      if (wavelength > 0.0)
      {
         phaseWavelength = wavelength;
      }
      else
      {
         phaseWavelength = 0.107;   // Be default, LC wavelength
      }

      return (*this);

   }// End of 'PhaseCodeAlignment::setPhaseWavelength()'


      /* Returns a satTypeValueMap object, adding the new data generated
       *  when calling this object.
       *
       * @param epoch     Time of observations.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& PhaseCodeAlignment::Process( const DayTime& epoch,
                                           satTypeValueMap& gData )
   {

      SatIDSet satRejectedSet;

         // Loop through all the satellites
      satTypeValueMap::iterator it;
      for (it = gData.begin(); it != gData.end(); ++it)
      {

         bool csflag(false);

            // Check if we want to use satellite arcs of cycle slip flags
         if(useSatArcs)
         {

               // Check if satellite currently has entries
            std::map<SatID, alignData>::const_iterator itDat;
            itDat = svData.find( (*it).first );
            if( itDat == svData.end() )
            {
                  // If it doesn't have an entry, insert one
               alignData a;

               svData[ (*it).first ] = a;
            };

            double arcN(0.0);

            try
            {
                  // Try to extract the satellite arc value
               arcN = (*it).second(TypeID::satArc);
            }
            catch(...)
            {
                  // If satellite arc is missing, then schedule this satellite
                  // for removal
               satRejectedSet.insert( (*it).first );
               continue;
            }


               // Check if satellite arc has changed
            if( svData[(*it).first].arcNumber < arcN )
            {
                  // Set flag
               csflag = true;

                  // Update satellite arc information
               svData[(*it).first].arcNumber = arcN;
            }

         }
         else
         {

            double flag(0.0);

            try
            {
                  // Try to extract the CS flag value
               flag = (*it).second(watchCSFlag);
            }
            catch(...)
            {
                  // If flag is missing, then schedule this satellite
                  // for removal
               satRejectedSet.insert( (*it).first );
               continue;
            }

               // Check if there was a cycle slip
            if( flag > 0.0)
            {
                  // Set flag
               csflag = true;
            }

         }  // End of 'if(useSatArcs)...'


            // If there was an arc change or cycle slip, let's compute offset
         if(csflag)
         {
               // Compute difference between code and phase measurements
            double diff( (*it).second(codeType) -(*it).second(phaseType) );

               // Convert 'diff' to cycles
            diff = diff/phaseWavelength;

               // Convert 'diff' to an INTEGER number of cycles
            diff = std::floor(diff);

               // The new offset is the INTEGER number of cycles, in meters
            svData[(*it).first].offset = diff * phaseWavelength;
         }

            // Let's align the phase measurement using the corresponding offset
         (*it).second[phaseType] = (*it).second[phaseType]
                                   + svData[(*it).first].offset;

      }

         // Remove satellites with missing data
      gData.removeSatID(satRejectedSet);

      return gData;

   }  // End of 'PhaseCodeAlignment::Process()'


      /* Returns a gnnsRinex object, adding the new data generated when
       *  calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& PhaseCodeAlignment::Process(gnssRinex& gData)
   {

      Process(gData.header.epoch, gData.body);

      return gData;

   }// End of 'PhaseCodeAlignment::Process()'


} // End of namespace gpstk
