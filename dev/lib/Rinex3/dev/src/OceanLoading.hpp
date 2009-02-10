#pragma ident "$Id: OceanLoading.hpp 1346 2008-08-05 14:59:55Z architest $"

/**
 * @file OceanLoading.hpp
 * This class computes the effect of ocean tides at a given position
 * and epoch.
 */

#ifndef OCEANLOADING_HPP
#define OCEANLOADING_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008
//
//============================================================================



#include <string>
#include "Triple.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"
#include "DayTime.hpp"
#include "BLQDataReader.hpp"
#include "icd_200_constants.hpp"
#include "geometry.hpp"


namespace gpstk
{

      /** @addtogroup GPSsolutions */
      //@{


      /** This class computes the effect of ocean tides at a given position
       *  and epoch.
       *
       * A typical way to use this class follows:
       *
       * @code
       *      // Create a time object
       *   DayTime time(2004, 25, 0.0);
       *
       *      // Object to store results
       *   Triple tides;
       *
       *      // Load ocean loading object with ocean tides harmonics data
       *   OceanLoading ocean("EBRE.BLQ");
       *
       *      // Compute ocean loading effect in Up-East-North [UEN]
       *      // reference frame
       *   tides = ocean.getOceanLoading("EBRE", time);
       * @endcode
       *
       * This model neglects minor tides and nodal modulations, which may
       * lead to errors up to 5 mm (RMS) at high latitutes. For more details,
       * please see:
       *
       * http://tai.bipm.org/iers/convupdt/convupdt_c7.html
       *
       */
   class OceanLoading
   {
   public:

         /** Common constructor
          *
          * @param filename  Name of BLQ file containing ocean tide
          *                  harmonics data.
          *
          * @warning If filename is not given, this class will look for
          * a file named "oceanloading.blq" in the current directory.
          */
      OceanLoading(string filename="oceanloading.blq")
         : blqData(filename), fileData(filename) {};


         /** Returns the effect of ocean tides loading (meters) at the given
          *  station and epoch, in the Up-East-North (UEN) reference frame.
          *
          * @param name  Station name (case is NOT relevant).
          * @param time  Epoch to look up
          *
          * @return a Triple with the ocean tidas loading effect, in meters
          * and in the UEN reference frame.
          *
          * @throw InvalidRequest If the request can not be completed for any
          * reason, this is thrown. The text may have additional information
          * about the reason the request failed.
          */
      Triple getOceanLoading( const string& name,
                              const DayTime& t )
         throw(InvalidRequest);


         /// Returns the name of BLQ file containing ocean tides harmonics data.
      virtual string getFilename(void) const
      { return fileData; };


         /** Sets the name of BLQ file containing ocean tides harmonics data.
          *
          * @param name      Name of BLQ tides harmonics data file.
          */
      virtual OceanLoading& setFilename(const string& name);


         /// Destructor
      virtual ~OceanLoading() {};


   private:


         /// Object to read BLQ ocean tides harmonics data file
      BLQDataReader blqData;


         /// Name of BLQ file containing ocean tides harmonics data.
      string fileData;


         /** Compute the value of the corresponding astronomical arguments,
          * in radians. This routine is based on IERS routine ARG.f.
          *
          * @param time      Epoch of interest
          *
          * @return A Vector<double> of 11 elements with the corresponding
          * astronomical arguments to be used in ocean loading model.
          */
      virtual Vector<double> getArg(const DayTime& time);


   }; // End of class 'OceanLoading'

      //@}

}  // End of namespace gpstk
#endif   // OCEANLOADING_HPP
