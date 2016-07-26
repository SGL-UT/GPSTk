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
 * @file ObsArray.hpp
 * Provides ability to operate mathematically on large, logical groups of observations
 * Class declarations.
 */

#ifndef OBSARRAY_HPP
#define OBSARRAY_HPP

#include<map>
#include<vector>
#include<string>
#include<valarray>
#include<iostream>

#include "ValarrayUtils.hpp"
#include "CommonTime.hpp"
#include "Rinex3EphemerisStore.hpp"
#include "Exception.hpp"
#include "Expression.hpp"
#include "RinexObsHeader.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsData.hpp"
#include "SatID.hpp"

namespace gpstk
{
   typedef int ObsIndex;

   /** @defgroup ObsGroup Storage and manipulation of general observations. */
   //@{

      /**
       * This class provides the ability to quickly access and manipulate
       * logical groups of observations. Observations can be any basic
       * type, e.g., "P1", or a function of types, e.g., "P1-C1". The
       * observations can be accessed by pass, by PRN, by time, or via
       * as user-defined mask.
       *
       * Access to observations is provided via std::valarray .
       * Therefore indexing
       * can be performed by the user using standard mechanisms: operator[],
       * slice_array, gslice_array, mask_array and indirect_array. Valarray
       * which was designed "specifically for speed of the usual numeric
       * operations." That quote is from p. 662 of Stroustrup's book
       * "The C++ Programming Language," 3rd edition. That book explains
       * valarray and how to use it. Such material explains how ObsArray
       * works internally and how advanced use of ObsArray is accomplished.
       */

   class ObsArray
   {
   public:

      NEW_EXCEPTION_CLASS(ObsArrayException, gpstk::Exception);

         /**
          * Constructor.
          */
      ObsArray(void);

         /**
          * This function notifies the object to track a particular RINEX
          * data type. This function must be called before loading
          * observations from file.
          */
      ObsIndex add(RinexObsType type);

         /**
          * This function notifies the object to track functions of
          * RINEX data types, e.g., "P1-C1". This function must be called
          * before loading observations from a file.
          */
      ObsIndex add(const std::string& expression);

      ObsIndex getNumObsTypes(void) const
         { return numObsTypes; }

      size_t getNumSatEpochs(void) const
         { return numSatEpochs; }  // { return numSatEpochs }

         /**
          * This functions loads a RINEX obs and nav file. Both files
          * should be from the same period.
          */
      void load(const std::string& obsfilename,
                const std::string& navfilename);
      void load(const char* obsfilename,
                const char* navfilename)
      {
         load(std::string(obsfilename),std::string(navfilename));
      }
      void load(const std::vector<std::string>& obsList,
                const std::vector<std::string>& navList);

      void scanObsFile(const std::string& obsfilename,
                       long& numEpochsObs,
                       double& dataRate,
                       Triple& antennaPos);

         /**
          * This function removes observations which the input
          * vallarray is "true".
          */
      void edit(const std::valarray<bool> strikeList)
         throw(ObsArrayException);

      double getPassLength(long passNo);

      double& operator() (size_t r, size_t c)
      {  return observation[r*numObsTypes + c]; }

      std::valarray<CommonTime>  epoch;
      std::valarray<SatID>    satellite;
      std::valarray<double>   observation;
      std::valarray<bool>     lli;
      std::valarray<double>   azimuth;
      std::valarray<double>   elevation;
      std::valarray<long>     pass;
      std::valarray<bool>     validAzEl;

         /// The rate in second which observations were recorded
      double interval;

         /**
          *  This is true if the interval was not read from the header but
          *  instead calculated from the data.
          */
      bool intervalInferred;

   private:

      void loadObsFile(const std::string& obsfilename);

      ObsIndex numObsTypes;
      std::map<ObsIndex, RinexObsType > basicTypeMap;
      std::map<ObsIndex, bool> isBasic;
      std::map<ObsIndex, Expression > expressionMap;

         /**
          *  The number observation sets stored. Each set is derived
          *  from unique combination of satellite and nominal epoch.
          */
      size_t numSatEpochs;

      int numFiles;
      int counter;
      std::valarray<long int> numSatEpochsTotal;




      Rinex3EphemerisStore ephStore;

   }; // End class ObsArray

      //@}
} // End namespace gpstk

#endif // OBSARRAY_HPP
