#pragma ident "$Id$"

/**
* @file SatOrbitStore.hpp
* 
*/

#ifndef GPSTK_SATORBITSTORE_HPP
#define GPSTK_SATORBITSTORE_HPP


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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010, 2010
//
//============================================================================

#include "SatID.hpp"
#include "PvtStore.hpp"

namespace gpstk
{
      /*
         -----------------------------------------
         Satellite             Cospar          SP3
         -----------------------------------------
         TOPEX-POSEIDON        92052A          L01
         GPS/MET               95017C          L02
         CHAMP                 00039B          L03
         SAC-C                 00075B          L04
         GRACE                   ...           L05
         -----------------------------------------
      */
   class SatOrbitStore 
   {
   public:
         /// Handy type definition
      typedef std::map<SatID, PvtStore> SvEphMap;

         // Default constructor
      SatOrbitStore() : orbit("LEO")
      {}
      
         /// Default destructor
      ~SatOrbitStore()
      { pe.clear(); }


         /// Load the given SP3 file
      void loadSP3File(const std::string& filename)
         throw(FileMissingException);


         /// Write data to a SP3 File
      void writeSP3File(const std::string& filename, 
                        bool sp3c = false);
      
         /// Load JPL POD solution
      void loadGNV1BFile(const std::string& filename);


         /// Get satellite state in specific reference frame
         /// t should be GPST
         /// if J2000 was wanted, you should have load earth orientation
         /// data globally.
      PvtStore::Pvt getPvt(const SatID sat,
                           const CommonTime& t, 
                           bool j2k = false )
         throw(InvalidRequest) ;
      

         /// Get epoch list for specific satellite
      PvtStore::EpochList epochList(const SatID sat);

      
         /// TODO: Compare two satellite orbit loaded in the object
      static void compareOrbit();
      
         /// delete all but specific sat
      void keepOnly(const SatID sat);
         
         /// delete specific sat
      void deleteOnly(const SatID sat);

         /// Easy debuging
      void test();

   protected:

      SvEphMap pe;

      std::string agency;
      std::string orbit;
      
   };

}  // End of namespace 'gpstk'


#endif   // GPSTK_SATORBITSTORE_HPP



