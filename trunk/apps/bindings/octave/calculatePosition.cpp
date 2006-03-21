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






#include <string>
#include <vector>

#include <octave/oct.h>
#include "oct-map.h"

#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"

#include "RinexNavBase.hpp"
#include "RinexNavData.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavStream.hpp"
#include "TropModel.hpp" 
#include "BCEphemerisStore.hpp"
#include "RAIMSolution.hpp"

using namespace std;
using namespace gpstk;

DEFUN_DLD (calculatePosition, args, , "\n[pvt, editedObs]=calculatePosition(obsfile, navfile)\n\n\tReads named RINEX obs, nav files and returns a list of position\n\tsolutions and edited observations.\n")
{
   using ::Matrix;
   Matrix pvtMatrix, editedMatrix;
   int numObsTypes=-1, numPrns;
   long nepochs = 0;

   BCEphemerisStore bcestore;
   RAIMSolution raimSolver;

   //raimSolver.Solution(0)=3.9817800e+06;
   //aimSolver.Solution(1)=-8.9237618e+04;
   //raimSolver.Solution(2)=4.9652914e+06;


   ZeroTropModel tropModel;

   if (args.length() != 2)
   {
      print_usage("calculatePosition");

      octave_value_list retval;
      retval(0) = octave_value( Matrix());
      retval(1) = octave_value( Matrix());
      return retval;
   }

   try
   {  
      // Read nav file and build unique list of ephemeredes
      RinexNavStream rnffs(args(1).string_value().data());
      RinexNavHeader rnh;
      RinexNavData rne;

      rnffs >> rnh;

      while (rnffs >> rne)
      {
         bcestore.addEphemeris(rne);
      }

      string obsfilename=args(0).string_value();

      RinexObsStream roffs(obsfilename.data());
      roffs.exceptions(ios::failbit);
      RinexObsHeader roh;
      RinexObsData rod;
      
      roffs >> roh;

      long nepochs = 0;

      // Scan through file the first time to get a count of epochs
      while (roffs >> rod)
      {
         // Apply any editing criteria.
	 if  (rod.epochFlag == 0 || rod.epochFlag == 1)
            nepochs++;
      }

      pvtMatrix.resize(nepochs,3,0); // Will have to resize _again_ after solutions.
  
      // Second scan
      RinexObsStream roffs2(obsfilename.data());
      roffs2.exceptions(ios::failbit);
      int currentRec = 0;
      roffs2 >> roh;

      while (roffs2>>rod) // Loop through each epoch
      {
         // Apply editing criteria (again)
         if  (rod.epochFlag == 0 || rod.epochFlag == 1) // Begin usable data
	 {
	    vector<RinexPrn> prnVec;
            vector<double> rangeVec;

	    RinexObsData::RinexPrnMap::const_iterator it;
            for (it = rod.obs.begin(); it!= rod.obs.end(); it++)
	    {
	       RinexObsData::RinexObsTypeMap otmap;
	       RinexObsData::RinexObsTypeMap::const_iterator mit; 
               RinexObsData::RinexDatum meas;
               otmap = (*it).second;
               mit = otmap.find(RinexObsHeader::P1);
     
               if (mit!=otmap.end())
	       {
                  prnVec.push_back((*it).first);
                  rangeVec.push_back((*mit).second.data);
	       }           

            }         
            bcestore.dump();
            raimSolver.RMSLimit = 3e6;
	    cout << raimSolver.Compute(rod.time,prnVec,rangeVec, bcestore, \
			       &tropModel);
            cout << raimSolver.RMSResidual;

            if (raimSolver.isValid())
	   {
	      pvtMatrix(nepochs,0)=raimSolver.Solution[0];
	      pvtMatrix(nepochs,1)=raimSolver.Solution[1];
	      pvtMatrix(nepochs,2)=raimSolver.Solution[2];
              cout << raimSolver.Solution[0] << "," ;
              cout << raimSolver.Solution[0] << "," ;
              cout << raimSolver.Solution[0] << endl ;

              nepochs++;
           } else cout << "Not valid!";
 
 	 } // End usable data 

      } // End loop through each epoch


     //  exit(0);
   }
   catch(FFStreamError& e)
   {
   }
   catch(Exception& e)
   {
   }
   catch (...)
   {
   }

   //pvtMatrix.resize(nepochs-1,3,0); // Will have to resize _again_ after solutions.

   octave_value_list retval;
   retval(0) = octave_value( pvtMatrix);
   retval(1) = octave_value( editedMatrix);

   return retval;
}





