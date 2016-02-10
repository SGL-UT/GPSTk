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

/**
* @file EOPDataStore.hpp
* 
*/

#ifndef GPSTK_EOPDATASTORE_HPP
#define GPSTK_EOPDATASTORE_HPP

#include "EpochDataStore.hpp"
#include "Triple.hpp"

namespace gpstk
{
      /// @ingroup Precise Orbit Determination 
      //@{

      /**
       * Class to store and manage Earth Orientation data.
       *
       */
   class EOPDataStore : public EpochDataStore
   {
   public:
     
      typedef struct EOPData
      {
         double xp;        /// arcseconds
         double yp;        /// arcseconds
         double UT1mUTC;   /// seconds
         double dPsi;      /// arcseconds
         double dEps;      /// arcseconds
         
         EOPData() : xp(0.0), yp(0.0), UT1mUTC(0.0),dPsi(0.0), dEps(0.0)
         {}

         EOPData(double x, double y, double ut1_utc, double dpsi = 0.0, double deps = 0.0) 
            : xp(x), yp(y), UT1mUTC(ut1_utc), dPsi(dpsi), dEps(deps)
         {}
      } EOPData;

         /// Default constructor
      EOPDataStore() : EpochDataStore(2)
      {}

         /// Default deconstructor
      virtual ~EOPDataStore() {}
      
         /// Add to the store directly
      void addEOPData(const CommonTime& utc,const EOPData& data)
         throw(InvalidRequest);

         /// Get the data at the given epoch and return it.
      EOPData getEOPData(const CommonTime& utc) const
         throw(InvalidRequest);


         /// Add EOPs to the store via a flat IERS file. 
         /// get finals.data from http://maia.usno.navy.mil/
      void loadIERSFile(std::string iersFile)
         throw(FileMissingException);


      void loadIGSFile(std::string igsFile)
         throw(FileMissingException);

         /** Add EOPs to the store via a flat STK file. 
          *  EOP-v1.1.txt
          *  http://celestrak.com/SpaceData/EOP-format.asp
          *
          *  @param stkFile  Name of file to read, including path.
          */
      void loadSTKFile(std::string stkFile)
         throw(FileMissingException);

   protected:


   }; // End of class 'EOPDataStore'

   
   std::ostream& operator<<(std::ostream& s, const EOPDataStore::EOPData& d);

      // @}

}  // End of namespace 'gpstk'


#endif   // GPSTK_EOPDATASTORE_HPP
