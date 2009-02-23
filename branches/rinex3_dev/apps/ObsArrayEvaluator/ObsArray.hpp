#pragma ident "$Id$"

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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

/**
 * @file ObsArray.hpp
 * Provides ability to operate mathematically on large, logical groups of
 * observations
 */

#ifndef OBSARRAY_HPP
#define OBSARRAY_HPP

#include<map>
#include<vector>
#include<string>
#include<valarray>

#include "DayTime.hpp"
#include "ObsReader.hpp"
#include "EphReader.hpp"
#include "Exception.hpp" 
#include "Expression.hpp"
#include "SatID.hpp"
#include "ObsID.hpp"

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
      NEW_EXCEPTION_CLASS(ObsArrayException, Exception);

      ObsArray();

      /**
       * This function notifies the object to track a particular 
       * data type. This function must be called before loading
       * observations from file.
       */
      ObsIndex add(const ObsID type) throw();

      /**
       * This function notifies the object to track functions of
       * RINEX data types, e.g., "P1-C1". This function must be called
       * before loading observations from a file.
       */
      ObsIndex add(const std::string& expression)
         throw();

      ObsIndex getNumObsTypes(void) const throw()
      { return numObsTypes; }

      size_t getNumSatEpochs(void) const throw()
      { return numSatEpochs; } 


      // This loads this object with the indicated observation data.
      // The ephemeris and rxPosition is required for the computation
      // of azimuth and elevation data only (well, at least at this time)
      void load(const std::vector<std::string>& obsFiles,
                EphReader::EphemerisStore& eph,
                Position& rxPos)
         throw(FileMissingException, ObsArrayException);

      /**
       * This function removes observations which the input
       * vallarray is "true".
       */
      void edit(const std::valarray<bool> strikeList)
         throw(ObsArrayException);
      
      double getPassLength(long passNo) throw();
      
      // This is to give this object the appearance of an array
      double& operator() (size_t r, size_t c)
      {  return observation[r*numObsTypes + c]; }


      // This is the data storage for the class. All arrays *must* be kept 
      // at the same length because they are all indexed together
      std::valarray<DayTime>  epoch;
      std::valarray<SatID>    satellite;
      std::valarray<double>   observation;
      std::valarray<bool>     lli;
      std::valarray<double>   azimuth;
      std::valarray<double>   elevation;
      std::valarray<long>     pass;
      std::valarray<bool>     validAzEl;

      /// The rate in seconds which observations were recorded
      double interval;

      int debugLevel;

   private:
      void loadObsFile(const std::string& fn,
                       EphReader::EphemerisStore& eph,
                       Position& rxPos)
         throw(FileMissingException, ObsArrayException);

      ObsIndex numObsTypes;
      std::map<ObsIndex, ObsID > basicTypeMap;
      std::map<ObsIndex, bool> isBasic;
      std::map<ObsIndex, Expression > expressionMap;      

      /**
       *  The number observation sets stored. Each set is derived
       *  from unique combination of satellite and nominal epoch.
       */ 
      size_t numSatEpochs;

      // These are used to help detect passes when loading data
      long highestPass;
      std::map<SatID, DayTime> lastObsTime;
      std::map<SatID, long> currPass;
   }; // End class ObsArray
   
      //@}   
} // End namespace gpstk

#endif // OBSARRAY_HPP  
