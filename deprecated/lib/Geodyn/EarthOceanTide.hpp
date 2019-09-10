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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
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
* @file EarthOceanTide.hpp
* 
*/

#ifndef GPSTK_OCEAN_TIDE_HPP
#define GPSTK_OCEAN_TIDE_HPP

#include <string>


namespace gpstk
{
      /// @ingroup GeoDynamics 
      //@{

      /**
       * Solid Earth Ocean Tide
       * reference: IERS Conventions 2003
       */
   class EarthOceanTide
   {
   public:
         /// Default constructor
      EarthOceanTide()
         : maxN(4),
           minX(0.05), 
           isLoaded(false)
      {
         fileName = "InputData\\Earth\\OT_CSRC.TID";
      }
         /// Default destructor
      ~EarthOceanTide(){}

     
         /// struct to hold Ocean Tide information
      struct CSR_OTIDE
      {
         double   KNMP[20];
         int      NTACT;
         int      NDOD[1200][6];
         double   CSPM[1200][4];
         int      NM[1200][2];  
      };

         
          /// load ocean data file, reference to Bernese5.0-"OT_CSRC.TID"

      void loadTideFile(std::string fileName, int NMAX = 4, double XMIN = 0.05);

         /** Solid pole tide to normalized earth potential coefficients
          *
          * @param mjdUtc UTC in MJD
          * @param dC     Correction to normalized coefficients dC
          * @param dS     Correction to normalized coefficients dS
          *    C20 C21 C22 C30 C31 C32 C33 C40 C41 C42 C43 C44
          */
      void getOceanTide(double mjdUtc, double dC[], double dS[] );

      void setTideFile(std::string file)
      {
         fileName = file;
         isLoaded = false;
      }

      void test();

   protected:
      //Get Doson  FUNDAMENTAL ARGUMENTS
      //void NUTARG(double mjdUtc, double BETA[6]);   

   protected:
      std::string fileName;

      int maxN;
      double minX;

      bool isLoaded;

      double FAC[41];

      /// line 2
      int    NWAV;   // number of lines to skip
      int    NTOT;   // number of data lines
      int    NMX;      // max degree
      int    MMX;     // max order 
      /// line 4
      double RRE;
      double RHOW;
      double XME;
      double PFCN;
      double XXX;

      CSR_OTIDE  tideData;

      /// GRAVITY CONSTANT G, GRAV ACC ON EARTH SURFACE GE
      /// see IERS Conventions 2003 Chapter 1
      static const double G;  //= 6.67259e-11;
      static const double GE; //= 9.780327;

   }; // End of class 'EarthOceanTide'

      // @}

}  // End of namespace 'gpstk'


#endif   // GPSTK_OCEAN_TIDE_HPP
