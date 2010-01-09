#pragma ident "$Id: GloFreqIndex.hpp 1900 2009-05-19 21:59:34Z raindave $"

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

//============================================================================
//
// This software developed by Applied Research Laboratories at the University
// of Texas at Austin, under contract to an agency or agencies within the U.S. 
// Department of Defense. The U.S. Government retains all rights to use,
// duplicate, distribute, disclose, or release this software. 
//
// Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//============================================================================

/**
 * @file GloFreqIndex.hpp
 * Calculate GLONASS SV frequency index from range & phase data and store it.
 */

#ifndef GPSTK_GLOFREQINDEX_HPP
#define GPSTK_GLOFREQINDEX_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "RinexSatID.hpp"
#include "StringUtils.hpp"

namespace gpstk
{
   //@{

   /**
    * This class 
    *
    */

   class GloFreqIndex
   {
   public:

      /// Constructor -- takes no arguments.
      GloFreqIndex()
      {}

      /// Destructor
      virtual ~GloFreqIndex()
      {}

      /// Method to calculate frequency index from range & phase data
      /// for G1 and G2 bands.  The integer returned is the band index
      /// determined from data (also appended to the internal data vector).
      /// This method assumes clean data, i.e. no need to edit the pass.
      int calcIndex( const std::vector<double>& r1, const std::vector<double>& p1,
                     const std::vector<double>& r2, const std::vector<double>& p2 )
         throw();

      /// Method to calculate frequency index from range & phase data
      /// for G1 band only.  This method provides empty G2 vectors to
      /// the actual method above.  Note that if one provides range &
      /// phase data for only one band using this method, it is assumed
      /// to be G1!
      int calcIndex( const std::vector<double>& r1, const std::vector<double>& p1 )
         throw()
      {
         std::vector<double> r2, p2;
         r2.clear();
         p2.clear();
         return calcIndex( r1, p1, r2, p2 );
      }

      /// This method returns the GLONASS frequency for a given SV and band.
      double getGloFreq( const RinexSatID& id, const int& band )
         throw();

      /// Dump the contents of the data store in a nice format.
      void dump( std::ostream& s ) const;

   protected:



   private: /// All data goes here -- use public accessors to add & view data.

      struct IndexData
      {
         int    pG1, pG2; // number of points in pass
         double fG1, fG2; // float   index solutions
         int    nG1, nG2; // integer index solutions
         double sG1, sG2; // standard deviations on the float solutions
      };

      /// Vector of Data structs to store multiple passes.
      typedef std::vector<IndexData> Data;

                     /// Map of data by SV ID.
      std::map< RinexSatID, Data > dataMap;

   }; // class GloFreqIndex

   //@}

} // namespace

#endif // GPSTK_GLOFREQINDEX_HPP
