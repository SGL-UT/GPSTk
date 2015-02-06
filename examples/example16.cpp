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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2010
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

// Example program Nro 16 for GPSTk
//
// This program shows how to use GNSS Data Structures (GDS) and other classes
// to build a reasonable complete application that implements the "Modified
// Kennedy method" to compute velocity and acceleration of a rover receiver.
//
// For details on the original Kennedy algorithm please consult:
//
//    Kennedy, S. (2002). Acceleration Estimation from GPS Carrier Phases for
//       Airborne Gravimetry. PhD thesis, Department of Geomatics Engineering,
//       University of Clagary, Calgary, Alberta, Canada. Report No. 20160.
//
//    Kennedy, S. (2002). Precise Acceleration Determination from Carrier Phase
//       Measurements. In: Proceedings of the 15th International Technical
//       Meeting of the Satellite Division of the Institute of Navigation.
//       ION GPS 2002, Portland, Oregon, USA, pp. 962–972.
//
// For details in the modifications done to the Kennedy method please consult:
//
//    Salazar, D. (2010). "Precise GPS-based position, velocity and
//       acceleration determination: Algorithms and tools". Ph.D. dissertation.
//       Doctoral Program in Aerospace Science & Technology, Universitat
//       Politecnica de Catalunya, Barcelona, Spain.
//
//    Salazar, D., Hernandez-Pajares, M., Juan, J.M., Sanz J. and
//       A. Aragon-Angel. "EVA: GPS-based extended velocity and acceleration
//       determination". Journal of Geodesy. Volume 85, Issue 6, pp. 329-340,
//       DOI: 10.1007/s00190-010-0439-6, 2011.
//
// The modifications done to the original method are two:
//
//  a) The satellite velocity and acceleration are ALWAYS computed using a
//     FIR differentiation filter, because it was demonstrated that the method
//     originally proposed (algebraic differentiation of the Lagrange fitting
//     polynomial) introduced severe biases in the rover velocity estimation.
//
//  b) The covariance model is substituted by a simpler one where only the
//     values in the diagonal of the covariance matrix are computed. The
//     original covariance model described by Kennedy may be used if properly
//     enabled in the configuration file, but it tends to produce problems
//     when the covariance matrix is inverted.
//
// Please note that this program is written to handle a static 'Rover'.
// However, it may handle real rover receivers with minor changes.
//


   // Basic input/output C++ classes
#include <iostream>
#include <iomanip>
#include <fstream>

   // Class for handling satellite observation parameters RINEX files
#include "RinexObsStream.hpp"

   // Class in charge of the GPS signal modelling
#include "BasicModel.hpp"

   // Class to store satellite precise navigation data
#include "SP3EphemerisStore.hpp"

   // Class to model the tropospheric delays
#include "TropModel.hpp"

   // Class to solve the equation system using Least Mean Squares
#include "SolverLMS.hpp"

   // Class to solve the equation system using Weighted-Least Mean Squares
#include "SolverWMS.hpp"

   // Class to filter pseudoranges
#include "SimpleFilter.hpp"

   // Class defining the GNSS data structures
#include "DataStructures.hpp"

   // Class to detect cycle slips using LI combination
#include "LICSDetector2.hpp"

   // Class to detect cycle slips using the Melbourne-Wubbena combination
#include "MWCSDetector.hpp"

   // Class to keep track of satellite arcs
#include "SatArcMarker.hpp"

   // Class to read configuration files.
#include "ConfDataReader.hpp"

   // Classes to compute several combinations
#include "ComputeLC.hpp"

   // Class to compute single differences between receiver stations
#include "DeltaOp.hpp"

   // Class to compute single differences between satellites
#include "NablaOp.hpp"

   // Class to synchronize two GNSS Data Structures data streams.
#include "Synchronize.hpp"

   // Class to require some observables.
#include "RequireObservables.hpp"

   // Class providing some handy linear combinations.
#include "LinearCombinations.hpp"

   // Class to compute linear combinations.
#include "ComputeLinear.hpp"

   // Class to correct observables to monument
#include "CorrectObservables.hpp"

   // Class to compute the tropospheric effect
#include "ComputeTropModel.hpp"

   // Class to carry out numerical differencing.
#include "Differentiator.hpp"

   // Class to compute statistics.
#include "Stats.hpp"


#include "GNSSconstants.hpp"                   // DEG_TO_RAD


using namespace std;
using namespace gpstk;



   // Ionospheric mapping function (Skone, 1998)
double mI( double elevation )
{

      // Ratio between ionospheric shell height and Earth radius, plus one.
   const double k(1.0 + 350.0/6378.1370);

   double temp( (cos(elevation*DEG_TO_RAD))/k );

   return ( 1.0/(sqrt( 1.0 - temp*temp )) );

}  // End of function 'mI()'



   /* Method to compute sigmaIono value, according to Kennedy, 2002:
      The following procedure is carried out for the highest satellite from
      rover's point of view:

      1) Compute LC (ionosphere-free phase combination) for each epoch.
      2) Compute (L1 - LC) to get first order ionospheric error.
      3) Substract from the former a 'line of best fit' to eliminate ionosphere
         first order trend. This will leave ionospheric second and third order
         effects, and carrier-phase noise (increased by LC computation).
      4) The highest satellite in sky is used for the former, and its
         ionospheric variation (according to steps 1 to 3) is mapped to zenith
         using Skone mapping function.
      5) Variance of former results will be taken as ionospheric variance.
   */
double computeSigmaIono( const SourceID& source,
                         gnssDataMap gdsMap )
{

      // Find satellite with highest elevation
      // Get a copy of the first element in the map.
   gnssRinex gRin( gdsMap.getGnssRinex( source ) );

      // I'll start looking for the highest satellite only when the receiver
      // has a reasonable number of satellites in view
   while( gRin.numSats() < 4 )
   {
         // Delete the first element
      gdsMap.pop_front_epoch();

         // Get a copy of the first element in the map.
      gRin = gdsMap.getGnssRinex( source );
   }

   const int nDump(10);

      // Now, dump the first 'nDump' epochs to let the satellites to stabilize
   for( int i = 0; i < nDump; ++i )
   {
         // Delete the first element
      gdsMap.pop_front_epoch();
   }

      // Get a copy of the first element in the map.
   gRin = gdsMap.getGnssRinex( source );


      // Declare variables to hold SatID and elevation
   SatID highSat;
   double elev(0.0);


   for( satTypeValueMap::const_iterator it = gRin.body.begin();
        it != gRin.body.end();
        ++it )
   {

      typeValueMap::const_iterator itObs((*it).second.find(TypeID::elevation));
      if ( itObs != (*it).second.end() )
      {
         if( (*itObs).second > elev )
         {
            highSat = (*it).first;
            elev = (*itObs).second;
         }
      }

   }  // End of 'for( satTypeValueMap::const_iterator it = gRin.body.begin();'


      // Create vectors
   vector<double> y;
   vector<double> t;
   vector<double> el;
   vector<double> arc;

      // We need to compute LC
   ComputeLC getLC;

   gRin >> getLC;


      // Add first values to vectors
   t.push_back( YDSTime(gRin.header.epoch).sod );
   y.push_back( gRin.getValue( highSat, TypeID::L1 ) -
                gRin.getValue( highSat, TypeID::LC ) );
   el.push_back( gRin.getValue( highSat, TypeID::elevation ) );

      // Get arc number
   double arcNumber( gRin.getValue( highSat, TypeID::satArc ) );
   arc.push_back( arcNumber );

      // Initial arc number is the same as 'arcNumber'
   const double initialArcNumber( arcNumber );

      // Delete the first element
   gdsMap.pop_front_epoch();

      // Visit the rest of the GDS
   while( !gdsMap.empty() &&
          ( initialArcNumber == arcNumber ) )
   {
         // Get first element
      gRin = gdsMap.getGnssRinex( source );

         // Delete the first element
      gdsMap.pop_front_epoch();

      if( gRin.numSats() > 0 )
      {

         gRin >> getLC;

         try
         {

               // This procedure is less efficient but should be safer

               // Try to get the values
            double ttemp( YDSTime(gRin.header.epoch).sod );
            double ytemp( gRin.getValue( highSat, TypeID::L1 ) -
                          gRin.getValue( highSat, TypeID::LC ) );
            double eltemp( gRin.getValue( highSat, TypeID::elevation ) );
            arcNumber = gRin.getValue( highSat, TypeID::satArc );

               // Add values to vectors
            t.push_back( ttemp );
            y.push_back( ytemp );
            el.push_back( eltemp );
            arc.push_back( arcNumber );

         }
         catch(...)
         {
               // In case the satellite is no longer visible, quit
            break;
         }

      }  // End of 'if( gRin.numSats() > 0 )'

   }  // End of 'while( !gdsMap.empty() && ..'


      // Declare Vectors and Matrices
   Vector<double> Y( t.size() );
   Matrix<double> A( t.size(), 2, 1.0 );


      // Introduce values
   for(size_t i = 0; i < t.size(); ++i)
   {
      Y[i] = y[i];
      A( i, 0 ) = t[i];
   }

      // Let's declare a LMS solver
   SolverLMS solver;

      // Compute adjustment
   solver.Compute( Y, A );


   double a( solver.solution[0] );
   double b( solver.solution[1] );

   Stats<double> st;

   for(size_t i = 0; i < t.size(); ++i)
   {
      double adjust( a*(t[i]) + b );

      st.Add( mI(el[i]) * ( y[i] - adjust ) );
   }


      // Return the standard deviation
   return ( st.StdDev() );

}  // End of method 'computeSigmaIono()'



   // Original method to compute sigmas according to Kennedy's work
   // If you find any error in the implementation of this routine with respect
   // to Kennedy's suggestions, please get in contact with me.
double getVariance( const gnssDataMap& gMap,
                    SatID pivotSat,
                    SourceID pivotSource,
                    SatID satellite,
                    SourceID source,
                    double sigmaIono,
                    double distance )
{

      // Value to be returned
   double variance(0.0);

      // Declare some important constants
   double tropoVar( 0.0004 );    // (0.02 m)^2
   double multiVar( 0.000025 );  // (0.005 m)^2
   double corrAng( 40.0*DEG_TO_RAD ); // Correlation angle, in radians
   double D( 350000.0 );         // Correlation distance, in meters

      // Get other important variables
   double ionoVar( sigmaIono*sigmaIono );

      // I need a NBTropModel initialized with dummy values
   NBTropModel tropoObj(0.0, 0.0, 1);


   if( pivotSat == satellite )
   {

      if( pivotSource == source )
      {
            // Get data from 'pivot' receiver
         gnssRinex gData( gMap.getGnssRinex( pivotSource ) );

         double elevP( gData.getValue( pivotSat, TypeID::elevation ) );
         double mt( tropoObj.dry_mapping_function(elevP) );
         double mi( mI(elevP) );

         variance = mt*mt*tropoVar + mi*mi*ionoVar + multiVar;
      }
      else
      {
            // Get data from 'pivot' receiver
         gnssRinex gData( gMap.getGnssRinex( pivotSource ) );

         double elevP( gData.getValue( pivotSat, TypeID::elevation ) );
         double mt( tropoObj.dry_mapping_function(elevP) );
         double mi( mI(elevP) );
         double expD( exp(-distance/D) );

         variance = mt*mt*expD*tropoVar + mi*mi*expD*ionoVar;
      }

   }
   else
   {

      if( pivotSource == source )
      {
            // Get data from 'pivot' receiver
         gnssRinex gData( gMap.getGnssRinex( pivotSource ) );

         double elevP( gData.getValue( pivotSat, TypeID::elevation ) );
         double azimP( gData.getValue( pivotSat, TypeID::azimuth ) );
         double mtP( tropoObj.dry_mapping_function(elevP) );
         double miP( mI(elevP) );

            // Get the data for the 'other' satellite
         double elevQ( gData.getValue( satellite, TypeID::elevation ) );
         double azimQ( gData.getValue( satellite, TypeID::azimuth ) );
         double mtQ( tropoObj.dry_mapping_function(elevQ) );
         double miQ( mI(elevQ) );

         double diffAzim( std::abs( azimP - azimQ ) );

         // Check for azimuth ambiguity
         if( diffAzim > 180.0 )
         {
            diffAzim = 360.0 - diffAzim;
         }

            // Compute separation angle, in radians
         double theta( std::abs( acos( sin(elevP)*sin(elevQ)
                     + cos(elevP)*cos(elevQ)*cos(diffAzim*DEG_TO_RAD) ) ) );

         double expA( exp(-theta/corrAng) );

         variance = mtP*mtQ*expA*tropoVar + miP*miQ*expA*ionoVar;
      }
      else
      {

         // Get data from 'pivot' receiver
         gnssRinex gData( gMap.getGnssRinex( pivotSource ) );

         double elevP( gData.getValue( pivotSat, TypeID::elevation ) );
         double azimP( gData.getValue( pivotSat, TypeID::azimuth ) );
         double mtP( tropoObj.dry_mapping_function(elevP) );
         double miP( mI(elevP) );

            // Get the data for the 'other' satellite
         double elevQ( gData.getValue( satellite, TypeID::elevation ) );
         double azimQ( gData.getValue( satellite, TypeID::azimuth ) );
         double mtQ( tropoObj.dry_mapping_function(elevQ) );
         double miQ( mI(elevQ) );

         double diffAzim( std::abs( azimP - azimQ ) );

            // Check for azimuth ambiguity
         if( diffAzim > 180.0 )
         {
            diffAzim = 360.0 - diffAzim;
         }

            // Compute separation angle, in radians
         double theta( std::abs( acos( sin(elevP)*sin(elevQ)
                     + cos(elevP)*cos(elevQ)*cos(diffAzim*DEG_TO_RAD) ) ) );

         double expA( exp(-theta/corrAng) );
         double expD( exp(-distance/D) );

         variance = mtP*mtQ*expA*expD*tropoVar + miP*miQ*expA*expD*ionoVar;

      }

   }  // End of 'if( pivotSat == satellite )'

   return variance;

}  // End of method 'getVariance()'



   // Method to compute sigmas in a simple way (only diagonal part)
   // This is the routine used in the modified-Kennedy method
double getSimpleVariance( const gnssDataMap& gMap,
                          SatID pivotSat,
                          SourceID pivotSource,
                          SatID satellite,
                          SourceID source,
                          double sigmaIono,
                          double distance )
{

      // Value to be returned
   double variance(0.0);

      // Declare some important constants
   double tropoVar( 0.0004 );    // (0.02 m)^2
   double multiVar( 0.000025 );  // (0.005 m)^2

      // Get other important variables
   double ionoVar( sigmaIono*sigmaIono );

      // I need a NBTropModel initialized with dummy values
   NBTropModel tropoObj(0.0, 0.0, 1);


   if( pivotSat == satellite )
   {

      if( pivotSource == source )
      {
            // Get data from 'pivot' receiver
         gnssRinex gData( gMap.getGnssRinex( pivotSource ) );

         double elevP( gData.getValue( pivotSat, TypeID::elevation ) );
         double mt( tropoObj.dry_mapping_function(elevP) );
         double mi( mI(elevP) );

         variance = mt*mt*tropoVar + mi*mi*ionoVar + multiVar;
      }
      else
      {
         variance = 0.0;
      }

   }
   else
   {
      variance = 0.0;
   }  // End of 'if( pivotSat == satellite )'

   return variance;

}  // End of method 'getSimpleVariance()'



   // Method to compute the double-differences covariance matrix
Matrix<double> computeCDD( vector<SatID> satVector,
                           SourceID roverSource,
                           SourceID refSource,
                           const gnssDataMap& gMap,
                           double sigmaIono,
                           double distance )
{

      // If we are using a 5th order Taylor-based differencing filter, the
      // corresponding scale factor to convert from covariance matrix to
      // double-differenced covariance matrix is 1.509551839.
   double scaleFact( 1.509551839 );

      // Reference satellite is the first one in satVector
   SatID highSat( satVector[0] );

   size_t halfSize( satVector.size() );

      // We need twice the satellites... this is ugly, I know...
   for( size_t i = 0; i < halfSize; ++i )
   {
      satVector.push_back( satVector[i] );
   }


      // Matrix size is twice the number of common satellites
   size_t matrixSize( 2*halfSize );
   Matrix<double> CD( matrixSize, matrixSize, 0.0 );


      // DD matrix rows are the number of common satellites minus reference SV
   size_t ddMatrixSize( halfSize - 1 );
   Matrix<double> DDM( matrixSize, ddMatrixSize, 0.0 );

      // We need this for DD matrix
   size_t columnDDM(0);

   for( size_t column = 0; column < matrixSize; ++column )
   {

         // Get satellite that will be 'pivot'
      SatID pivotSat( satVector[ column ] );

         // SourceID belonging to 'pivot'
      SourceID pivotSource;

      if( column < halfSize )
      {
         pivotSource = roverSource;
      }
      else
      {
         pivotSource = refSource;
      }


         // Take care of the first half of the matrix
      for( size_t row = column; row < halfSize; ++row )
      {

         CD( row, column ) = getVariance( gMap,
                                          pivotSat,
                                          pivotSource,
                                          satVector[ row ],
                                          roverSource,
                                          sigmaIono,
                                          distance );

         CD( column, row ) = CD( row, column );

      }

         // Take care of the second half of the matrix
      for( size_t row = halfSize; row < matrixSize; ++row )
      {

         CD( row, column ) = getVariance( gMap,
                                          pivotSat,
                                          pivotSource,
                                          satVector[ row ],
                                          refSource,
                                          sigmaIono,
                                          distance );

         CD( column, row ) = CD( row, column );

      }


         // Now, let's build the DD matrix
         // We must skip reference satellite
      if( ( satVector[column] != highSat ) &&
          ( columnDDM < ddMatrixSize ) )
      {

            // Build a full column, row by row
         for( size_t row = 0; row < matrixSize; ++row )
         {
            if( satVector[row] == pivotSat )
            {

               if( row < halfSize )
               {
                  DDM( row, columnDDM ) = 1.0;
               }
               else
               {
                  DDM( row, columnDDM ) = -1.0;
               }
            }

            if( satVector[row] == highSat )
            {

               if( row < halfSize )
               {
                  DDM( row, columnDDM ) = -1.0;
               }
               else
               {
                  DDM( row, columnDDM ) = 1.0;
               }
            }

         }  // End of 'for( size_t row = 0; row < halfSize; ++row )...'

         ++columnDDM;

      }  // End of 'if( satVector[column] != highSat )'


   }  // End of 'for( size_t column = 0; column < matrixSize; ++column )'


   Matrix<double> DDMT = transpose(DDM);

   return ( scaleFact * (DDMT * ( CD * DDM ) ) );

}  // End of method 'computeCDD()'


   // Method to compute the double-differences covariance matrix
Matrix<double> computeSimpleCDD( vector<SatID> satVector,
                                 SourceID roverSource,
                                 SourceID refSource,
                                 const gnssDataMap& gMap,
                                 double sigmaIono,
                                 double distance )
{

      // If we are using a 5th order Taylor-based differencing filter, the
      // corresponding scale factor to convert from covariance matrix to
      // double-differenced covariance matrix is 1.509551839.
   double scaleFact( 1.509551839 );

      // Reference satellite is the first one in satVector
   SatID highSat( satVector[0] );

   size_t halfSize( satVector.size() );

      // We need twice the satellites... this is ugly, I know...
   for( size_t i = 0; i < halfSize; ++i )
   {
      satVector.push_back( satVector[i] );
   }


      // Matrix size is twice the number of common satellites
   size_t matrixSize( 2*halfSize );
   Matrix<double> CD( matrixSize, matrixSize, 0.0 );


      // DD matrix rows are the number of common satellites minus reference SV
   size_t ddMatrixSize( halfSize - 1 );
   Matrix<double> DDM( matrixSize, ddMatrixSize, 0.0 );

      // We need this for DD matrix
   size_t columnDDM(0);

   for( size_t column = 0; column < matrixSize; ++column )
   {

         // Get satellite that will be 'pivot'
      SatID pivotSat( satVector[ column ] );

         // SourceID belonging to 'pivot'
      SourceID pivotSource;

      if( column < halfSize )
      {
         pivotSource = roverSource;
      }
      else
      {
         pivotSource = refSource;
      }


         // Take care of the first half of the matrix
      for( size_t row = column; row < halfSize; ++row )
      {

         CD( row, column ) = getSimpleVariance( gMap,
                                                pivotSat,
                                                pivotSource,
                                                satVector[ row ],
                                                roverSource,
                                                sigmaIono,
                                                distance );

         CD( column, row ) = CD( row, column );

      }

         // Take care of the second half of the matrix
      for( size_t row = halfSize; row < matrixSize; ++row )
      {

         CD( row, column ) = getSimpleVariance( gMap,
                                                pivotSat,
                                                pivotSource,
                                                satVector[ row ],
                                                refSource,
                                                sigmaIono,
                                                distance );

         CD( column, row ) = CD( row, column );

      }


         // Now, let's build the DD matrix
         // We must skip reference satellite
      if( ( satVector[column] != highSat ) &&
          ( columnDDM < ddMatrixSize ) )
      {

            // Build a full column, row by row
         for( size_t row = 0; row < matrixSize; ++row )
         {
            if( satVector[row] == pivotSat )
            {

               if( row < halfSize )
               {
                  DDM( row, columnDDM ) = 1.0;
               }
               else
               {
                  DDM( row, columnDDM ) = -1.0;
               }
            }

            if( satVector[row] == highSat )
            {

               if( row < halfSize )
               {
                  DDM( row, columnDDM ) = -1.0;
               }
               else
               {
                  DDM( row, columnDDM ) = 1.0;
               }
            }

         }  // End of 'for( size_t row = 0; row < halfSize; ++row )...'

         ++columnDDM;

      }  // End of 'if( satVector[column] != highSat )'


   }  // End of 'for( size_t column = 0; column < matrixSize; ++column )'


   Matrix<double> DDMT = transpose(DDM);

   return ( scaleFact * (DDMT * ( CD * DDM ) ) );

}  // End of method 'computeSimpleCDD()'



   // Computes a double-differences covariance matrix with constant variances
Matrix<double> computeConstCDD( vector<SatID> satVector )
{

      // Declare some important constants
   double constVar( 0.0004 + 0.000025 );    // (0.02 m)^2 + (0.005 m)^2

      // If we are using a 5th order Taylor-based differencing filter, the
      // corresponding scale factor to convert from covariance matrix to
      // double-differenced covariance matrix is 1.509551839.
   double scaleFact( 1.509551839 );

      // Reference satellite is the first one in satVector
   SatID highSat( satVector[0] );

   size_t halfSize( satVector.size() );

      // We need twice the satellites... this is ugly, I know...
   for( size_t i = 0; i < halfSize; ++i )
   {
      satVector.push_back( satVector[i] );
   }


      // Matrix size is twice the number of common satellites
   size_t matrixSize( 2*halfSize );
   Matrix<double> CD( matrixSize, matrixSize, 0.0 );


      // DD matrix rows are the number of common satellites minus reference SV
   size_t ddMatrixSize( halfSize - 1 );
   Matrix<double> DDM( matrixSize, ddMatrixSize, 0.0 );

      // We need this for DD matrix
   size_t columnDDM(0);

   for( size_t column = 0; column < matrixSize; ++column )
   {

         // The CD matrix is diagonal, with constant values
      CD( column, column ) = constVar;

         // Get satellite that will be 'pivot'
      SatID pivotSat( satVector[ column ] );

         // Now, let's build the DD matrix
         // We must skip reference satellite
      if( ( satVector[column] != highSat ) &&
          ( columnDDM < ddMatrixSize ) )
      {

            // Build a full column, row by row
         for( size_t row = 0; row < matrixSize; ++row )
         {
            if( satVector[row] == pivotSat )
            {

               if( row < halfSize )
               {
                  DDM( row, columnDDM ) = 1.0;
               }
               else
               {
                  DDM( row, columnDDM ) = -1.0;
               }
            }

            if( satVector[row] == highSat )
            {

               if( row < halfSize )
               {
                  DDM( row, columnDDM ) = -1.0;
               }
               else
               {
                  DDM( row, columnDDM ) = 1.0;
               }
            }

         }  // End of 'for( size_t row = 0; row < halfSize; ++row )...'

         ++columnDDM;

      }  // End of 'if( satVector[column] != highSat )'


   }  // End of 'for( size_t column = 0; column < matrixSize; ++column )'


   Matrix<double> DDMT = transpose(DDM);

   return ( scaleFact * (DDMT * ( CD * DDM ) ) );

}  // End of method 'computeSimpleCDD()'



////////// MAIN FUNCTION //////////
int main( int argc, char **argv )
{

      ////////// Initialization phase //////////


//// vvvv Configuration file handling vvvv

      // Configuration file reader
   ConfDataReader confReader;

      // Check if the user provided a configuration file name
   if( argc == 2 )
   {

         // Enable exceptions
      confReader.exceptions(ios::failbit);

         // Get configuration file name
      const string fileName( argv[1] );

      try
      {

            // Try to open the provided configuration file
         confReader.open( fileName );

      }
      catch(...)
      {

         cerr << "Problem opening file "
              << fileName
              << endl;
         cerr << "Maybe it doesn't exist or you don't have proper "
              << "read permissions." << endl;

         exit (-1);

      }  // End of 'try-catch' block

   }
   else
   {

      cerr << endl << "You MUST provide a configuration file. For instance:"
           << endl << endl
           << "   ./program file.conf"
           << endl << endl;

         exit (-1);

   }  // End of 'if ( argc == 2 )'


      // If a given variable is not found in the provided section, then
      // 'confReader' will look for it in the 'DEFAULT' section.
   confReader.setFallback2Default(true);


//// ^^^^ Configuration file handling ^^^^


      // Declare a data structure to store data for velocity and acceleration
      // determination
   gnssDataMap gdsMap;

      // Declare some important objects
   SourceID rover;
   SourceID reference;
   Position roverNominalPos;


   //// vvvv Ephemeris handling vvvv

      // Declare a "SP3EphemerisStore" object to handle precise ephemeris
   SP3EphemerisStore SP3EphList;

      // Set flags to reject satellites with bad or absent positional
      // values or clocks
   SP3EphList.rejectBadPositions(true);
   SP3EphList.rejectBadClocks(true);

      // Read if we should check for data gaps.
   if ( confReader.getValueAsBoolean( "checkGaps", "DEFAULT" ) )
   {
      SP3EphList.enableDataGapCheck();
      SP3EphList.setGapInterval(
                  confReader.getValueAsDouble("SP3GapInterval", "DEFAULT" ) );
   }

      // Read if we should check for too wide interpolation intervals
   if ( confReader.getValueAsBoolean( "checkInterval", "DEFAULT" ) )
   {
      SP3EphList.enableIntervalCheck();
      SP3EphList.setMaxInterval(
                  confReader.getValueAsDouble("maxSP3Interval", "DEFAULT" ) );
   }


      // Load all the SP3 ephemerides files from variable list
   string sp3File;
   while ( (sp3File = confReader.fetchListValue("SP3List", "DEFAULT" ) ) != "" )
   {

         // Try to load each ephemeris file
      try
      {

         SP3EphList.loadFile( sp3File );

      }
      catch (FileMissingException& e)
      {
            // If file doesn't exist, issue a warning
         cerr << "SP3 file '" << sp3File << "' doesn't exist or you don't "
              << "have permission to read it. Skipping it." << endl;

         continue;

      }

   }  // End of 'while ( (sp3File = confReader.fetchListValue( ... "

   //// ^^^^ Ephemeris handling ^^^^



   //// vvvv RINEX observations handling vvvv

      // Create the input observation file stream for the rover
   RinexObsStream rosRover;

      // Enable exceptions
   rosRover.exceptions(ios::failbit);

      // Try to open Rinex observations file
   try
   {

         // Open Rinex observations file in read-only mode
      rosRover.open( confReader("roverObsFile"), std::ios::in );

   }
   catch(...)
   {

      cerr << "Problem opening file '"
           << confReader.getValue("roverObsFile")
           << "'." << endl;

      cerr << "Maybe it doesn't exist or you don't have "
           << "proper read permissions."
           << endl;

      cerr << "Aborting processing."
           << endl;

         // Close current Rinex observation stream
      rosRover.close();

      exit(-1);

   }  // End of 'try-catch' block


      // Create the input observation file stream for the reference station
   RinexObsStream rosRef;

      // Enable exceptions
   rosRef.exceptions(ios::failbit);

      // Try to open Rinex observations file
   try
   {

         // Open Rinex observations file in read-only mode
      rosRef.open( confReader("refObsFile"), std::ios::in );

   }
   catch(...)
   {

      cerr << "Problem opening file '"
           << confReader.getValue("refObsFile")
           << "'." << endl;

      cerr << "Maybe it doesn't exist or you don't have "
           << "proper read permissions."
           << endl;

      cerr << "Aborting processing."
           << endl;

         // Close current Rinex observation stream
      rosRef.close();

      exit(-1);

   }  // End of 'try-catch' block

   //// ^^^^ RINEX observations handling ^^^^


      // Load rover nominal position
   double xnRover( confReader.fetchListValueAsDouble("roverNominalPosition") );
   double ynRover( confReader.fetchListValueAsDouble("roverNominalPosition") );
   double znRover( confReader.fetchListValueAsDouble("roverNominalPosition") );
         // The former peculiar code is possible because each time we
         // call a 'fetchListValue' method, it takes out the first element
         // and deletes it from the given variable list.

      // Set nominal position
   roverNominalPos.setECEF( xnRover, ynRover, znRover );

      // Store nominal position
   Position roverOriginalPosition( roverNominalPos );

      // Load reference nominal position
   double xnRef( confReader.fetchListValueAsDouble("refNominalPosition") );
   double ynRef( confReader.fetchListValueAsDouble("refNominalPosition") );
   double znRef( confReader.fetchListValueAsDouble("refNominalPosition") );

   Position refNominalPos( xnRef, ynRef, znRef );


      // This object will check that all required observables are present
      // in the rover data
   RequireObservables roverRequireObs;
   roverRequireObs.addRequiredType(TypeID::P2);
   roverRequireObs.addRequiredType(TypeID::L1);
   roverRequireObs.addRequiredType(TypeID::L2);

      // Read if we should use C1 instead of P1
   const bool roverUsingC1 ( confReader.getValueAsBoolean( "roverUseC1" ) );
   if ( roverUsingC1 )
   {
      roverRequireObs.addRequiredType(TypeID::C1);
   }
   else
   {
      roverRequireObs.addRequiredType(TypeID::P1);
   }


      // This object will check that all required observables are present
      // in the reference station data
   RequireObservables refRequireObs;
   refRequireObs.addRequiredType(TypeID::P2);
   refRequireObs.addRequiredType(TypeID::L1);
   refRequireObs.addRequiredType(TypeID::L2);

      // Read if we should use C1 instead of P1
   const bool refUsingC1 ( confReader.getValueAsBoolean( "refUseC1" ) );
   if ( refUsingC1 )
   {
      refRequireObs.addRequiredType(TypeID::C1);
   }
   else
   {
      refRequireObs.addRequiredType(TypeID::P1);
   }


      // Set up a pseudorange filter
   TypeIDSet typeSet;
   typeSet.insert(TypeID::P2);
   if ( refUsingC1 )
   {
      typeSet.insert(TypeID::C1);
   }
   else
   {
      typeSet.insert(TypeID::P1);
   }

   SimpleFilter myFilter(typeSet);
   myFilter.setMaxLimit(45000000.0);


      // This object defines several handy linear combinations
   LinearCombinations comb;


      // Object to compute linear combinations for cycle slip detection (rover)
   ComputeLinear roverLinear1;

      // Read if we should use C1 instead of P1
   if ( roverUsingC1 )
   {
      roverLinear1.addLinear(comb.pdeltaCombWithC1);
      roverLinear1.addLinear(comb.mwubbenaCombWithC1);
   }
   else
   {
      roverLinear1.addLinear(comb.pdeltaCombination);
      roverLinear1.addLinear(comb.mwubbenaCombination);
   }
   roverLinear1.addLinear(comb.ldeltaCombination);
   roverLinear1.addLinear(comb.liCombination);


      // Object to compute linear combinations for cycle slip detection
      // (reference station)
   ComputeLinear refLinear1;

      // Read if we should use C1 instead of P1
   if ( refUsingC1 )
   {
      refLinear1.addLinear(comb.pdeltaCombWithC1);
      refLinear1.addLinear(comb.mwubbenaCombWithC1);
   }
   else
   {
      refLinear1.addLinear(comb.pdeltaCombination);
      refLinear1.addLinear(comb.mwubbenaCombination);
   }
   refLinear1.addLinear(comb.ldeltaCombination);
   refLinear1.addLinear(comb.liCombination);


   const double maxGap( confReader.getValueAsDouble( "maximumGapCS" ) );

      // Objects to mark cycle slips (rover)
   LICSDetector2 roverMarkCSLI2;                      // Checks LI cycle slips
   roverMarkCSLI2.setDeltaTMax( maxGap );             // Set maximum allowed gap

   MWCSDetector roverMarkCSMW;         // Checks Merbourne-Wubbena cycle slips
   roverMarkCSMW.setDeltaTMax( maxGap );              // Set maximum allowed gap

      // Objects to mark cycle slips (reference station)
   LICSDetector2 refMarkCSLI2;                        // Checks LI cycle slips
   refMarkCSLI2.setDeltaTMax( maxGap );               // Set maximum allowed gap

   MWCSDetector refMarkCSMW;         // Checks Merbourne-Wubbena cycle slips
   refMarkCSMW.setDeltaTMax( maxGap );                // Set maximum allowed gap


   const double unstablePeriod(confReader.getValueAsDouble("unstablePeriodCS"));

      // Objects to keep track of satellite arcs
   SatArcMarker roverMarkArc;
   roverMarkArc.setDeleteUnstableSats(true);
   roverMarkArc.setUnstablePeriod( unstablePeriod );

   SatArcMarker refMarkArc;
   refMarkArc.setDeleteUnstableSats(true);
   refMarkArc.setUnstablePeriod( unstablePeriod );


      // Get the minimum elevation
   const double minElev( confReader.getValueAsDouble( "cutOffElevation" ) );

      // Declare a basic modeler for the rover
      // This object will compute:
      //    TypeID::dx     As geometric coefficient for the geometry matrix
      //    TypeID::dy     As geometric coefficient for the geometry matrix
      //    TypeID::dz     As geometric coefficient for the geometry matrix
      //    TypeID::cdt
      //    TypeID::rho
      //    TypeID::rel
      //    TypeID::elevation
      //    TypeID::azimuth
      //    TypeID::satX
      //    TypeID::satY
      //    TypeID::satZ
      //    TypeID::satVX                 Don't use this!
      //    TypeID::satVY                 Don't use this!
      //    TypeID::satVZ                 Don't use this!
      //    TypeID::instC1
   BasicModel roverBasic( roverNominalPos, SP3EphList );

      // Set the minimum elevation
   roverBasic.setMinElev( minElev );

      // If we are going to use P1 instead of C1, we must reconfigure 'basic'
   if ( !roverUsingC1 )
   {
      roverBasic.setDefaultObservable(TypeID::P1);
   }


      // Declare a basic modeler for the reference station
   BasicModel refBasic( refNominalPos, SP3EphList );

      // Set the minimum elevation
   refBasic.setMinElev( minElev );

      // If we are going to use P1 instead of C1, we must reconfigure 'basic'
   if ( !refUsingC1 )
   {
      refBasic.setDefaultObservable(TypeID::P1);
   }


      // Vector from monument to antenna ARP [UEN], in meters, for the rover
   double uARP(confReader.fetchListValueAsDouble( "roverOffsetARP" ) );
   double eARP(confReader.fetchListValueAsDouble( "roverOffsetARP" ) );
   double nARP(confReader.fetchListValueAsDouble( "roverOffsetARP" ) );
   const Triple roverOffsetARP( uARP, eARP, nARP );


      // Declare an object to correct observables to monument, for the rover
   CorrectObservables roverCorr( SP3EphList );
   roverCorr.setNominalPosition( roverNominalPos );
   roverCorr.setMonument( roverOffsetARP );


      // Vector from monument to antenna ARP [UEN], in meters, for the
      // reference station
   uARP = confReader.fetchListValueAsDouble( "refOffsetARP" );
   eARP = confReader.fetchListValueAsDouble( "refOffsetARP" );
   nARP = confReader.fetchListValueAsDouble( "refOffsetARP" );
   const Triple refOffsetARP( uARP, eARP, nARP );


      // Declare an object to correct observables to monument, for the
      // reference station
   CorrectObservables refCorr( SP3EphList );
   refCorr.setNominalPosition( refNominalPos );
   refCorr.setMonument( refOffsetARP );


      // Get day of year
   const int dayOfYear( confReader.getValueAsInt( "dayOfYear" ) );

      // Declare a NBTropModel object for the rover, setting its parameters
   NBTropModel roverNBTM( roverNominalPos.getAltitude(),
                          roverNominalPos.getGeodeticLatitude(),
                          dayOfYear );


      // Object to compute the tropospheric effect for the rover
   ComputeTropModel roverComputeTropo( roverNBTM );


      // Declare a NBTropModel object for the reference station
   NBTropModel refNBTM( refNominalPos.getAltitude(),
                        refNominalPos.getGeodeticLatitude(),
                        dayOfYear );


      // Object to compute the tropospheric effect for the reference station
   ComputeTropModel refComputeTropo( refNBTM );


      // Objects to compute prefit-residuals
   ComputeLinear roverLinear2;

      // Read if we should use C1 instead of P1
   if ( roverUsingC1 )
   {
      roverLinear2.addLinear(comb.c1Prefit);
   }
   else
   {
      roverLinear2.addLinear(comb.p1Prefit);
   }

      // Now, for the reference station
   ComputeLinear refLinear2;

      // Read if we should use C1 instead of P1
   if ( refUsingC1 )
   {
      refLinear2.addLinear(comb.c1Prefit);
   }
   else
   {
      refLinear2.addLinear(comb.p1Prefit);
   }


      // Declare a SolverLMS object and configure it to solve a
      // double-differenced equation system.
      // First, let's define a set with the typical unknowns
   TypeIDSet tempSet;
   tempSet.insert(TypeID::dx);
   tempSet.insert(TypeID::dy);
   tempSet.insert(TypeID::dz);
      // Please note that receiver clock offset is NOT included here

      // Now, we build the default definition for a common GNSS,
      // double-differenced code-based equation
   gnssEquationDefinition newEq(TypeID::prefitC, tempSet);

   SolverLMS solver(newEq);    // DD reconfiguration


      // Object to compute single satellite-based differences of prefitC and
      // dx, dy, dz coefficients.
   NablaOp nabla;

      // Object to compute single ground-based differences of prefitC.
   DeltaOp delta;


      // This is the GNSS data structure that will hold rover data
   gnssRinex gRover;

      // We'll use a backup copy of rover data
   gnssRinex gRoverBak;

      // This is the GNSS data structure that will hold reference station data
   gnssRinex gRef;

      // Create an object to synchronize rover and reference station
      // data streams. This object will take data out from "rosRef" until
      // it is synchronized with data in "gRover". Default synchronization
      // tolerance is 1 s.
   Synchronize synchro( rosRef, gRover );
   synchro.setTolerance(0.1);             // Set a proper value for tolerance


      // Declare a data structure to compute ionospheric sigma
   gnssDataMap gIono;

      // Map to store rover positions
   std::map<CommonTime, Position> posMap;

      // Prepare for printing
   int precision( confReader.getValueAsInt( "precision", "DEFAULT" ) );
   cout << fixed << setprecision( precision );


      //////// End of initialization phase  ////////


      //////// Processing phase ////////

   cerr << "Start of Preprocessing" << endl;

      // Extract the SourceID of each data stream
   rosRover >> gRover;
   rover = gRover.header.source;

   rosRef >> gRef;
   reference = gRef.header.source;

   int iterCount(0);
   
      // Loop over all data epochs
   while( rosRover >> gRover )
   {

         // Get current epoch
      CommonTime curEpoch( gRover.header.epoch );

         // Object to store the reference satellite
      SatID refSat;

         // At each iteration we must be sure that 'nabla' uses the highest
         // satellite from reference station's point of view
      nabla.useHighestSat();

         // First, let's synchronize and process reference station data
      try
      {

         gRef >> synchro
              >> refRequireObs
              >> myFilter
              >> refLinear1
              >> refMarkCSLI2
              >> refMarkCSMW
              >> refMarkArc
              >> refBasic
              >> refCorr;
            // Please note that the FIRST STEP is to synchronize "gRef", the
            // reference station data stream, with "gRover", the rover receiver
            // data stream.

            // We store the data generated so far
         gdsMap.addGnssRinex( gRef );

         gRef >> refComputeTropo
              >> refLinear2
              >> nabla;

            // We need to store this to use the same reference satellite with
            // the rover receiver
         refSat = nabla.getRefSat();


            // The "delta" object will take care of proper differencing.
            // We must tell it which GNSS data structure will be used
            // as reference
         delta.setRefData( gRef.body );

      }
      catch(SynchronizeException& e)   // THIS IS VERY IMPORTANT IN ORDER TO
      {                                // MANAGE A POSSIBLE DESYNCHRONIZATION!!
         continue;
      }
      catch(...)
      {

         cerr << "Exception when processing reference station data "
              << "at epoch: "
              << gRef.header.epoch << endl;

         continue;

      }


         // Rover data processing is done here:
      try
      {

         gRover >> roverRequireObs
                >> myFilter
                >> roverLinear1
                >> roverMarkCSLI2
                >> roverMarkCSMW
                >> roverMarkArc;

            // We need to store data generated so far. It will be reprocessed
            // later, when we have a more precise position
         gRoverBak = gRover;

            // Continue processing
         gRover >> roverBasic
                >> roverCorr;

            // Store the data so far. We'll use it to compute sigma Iono
         gIono.addGnssRinex( gRover );

            // Let's make sure we use the same reference satellite for the rover
         nabla.setRefSat( refSat );


            // Continue processing
         gRover >> roverComputeTropo
                >> roverLinear2
                >> nabla
                >> delta
                >> solver;

      }
      catch(...)
      {

         cerr << "Exception when processing rover data at epoch: "
              << gRover.header.epoch
              << endl;

         continue;

      }


         // Now that we have a better estimation of the position, let's
         // reprocess rover data to get a better model

         // Get the position correction for this epoch
      Position correction( solver.getSolution(TypeID::dx),
                           solver.getSolution(TypeID::dy),
                           solver.getSolution(TypeID::dz) );

         // Update rover position with updates
      roverNominalPos += correction;

         // Store the correct rover position for later use
      posMap[ curEpoch ] = roverNominalPos;

         // Reconfigure some important objects with the new position information
         // Reconfigure the basic modeler
      roverBasic.rxPos = roverNominalPos;

         // Reconfigure the object correcting from eccentricity
      roverCorr.setNominalPosition( roverNominalPos );

         // Reprocess
      gRoverBak >> roverBasic
                >> roverCorr;

         // We store the reprocessed data
      gdsMap.addGnssRinex( gRoverBak );

         // Mark the pass of time...
      ++iterCount;
      if ( iterCount > 100 )
      {
         cerr << ".";
         iterCount = 0;
      }

   }  // End of 'while( rosRover >> gRover )'


      // Compute sigmaIono
   double sigmaIono( computeSigmaIono( rover, gIono ) );


   cerr << endl << "End of Preprocessing" << endl;


      ///// Start of derivatives computation

   double samplingP( confReader.getValueAsDouble( "samplingPeriod",
                                                  "DEFAULT" ) );

   double diffTol( confReader.getValueAsDouble( "diffTolerance",
                                                "DEFAULT" ) );

      // Declare 'Differentiator' objects
   Differentiator diff( TypeID::L1, TypeID::L1dot, samplingP, diffTol );

   Differentiator diff2( TypeID::L1dot,  TypeID::L1dot2, samplingP, diffTol );

   Differentiator diff3( TypeID::rho, TypeID::rhodot, samplingP, diffTol );

   Differentiator diff4( TypeID::rhodot, TypeID::rhodot2, samplingP, diffTol );


      // Objects to compute satellite velocities
   Differentiator diff5( TypeID::satX, TypeID::satVX, samplingP, diffTol );

   Differentiator diff6( TypeID::satY, TypeID::satVY, samplingP, diffTol );

   Differentiator diff7( TypeID::satZ, TypeID::satVZ, samplingP, diffTol );


      // Objects to compute satellite accelerations
   Differentiator diff8( TypeID::satVX, TypeID::satAX, samplingP, diffTol );

   Differentiator diff9( TypeID::satVY, TypeID::satAY, samplingP, diffTol );

   Differentiator diff10( TypeID::satVZ, TypeID::satAZ, samplingP, diffTol );


   cerr << "Starting to compute derivatives" << endl;


      // Compute derivatives
   diff.Process(gdsMap);
   cerr << "   -> L1dot" << endl;
   diff2.Process(gdsMap);
   cerr << "   -> L1dot2" << endl;

   diff3.Process(gdsMap);
   cerr << "   -> rhodot" << endl;
   diff4.Process(gdsMap);
   cerr << "   -> rhodot2" << endl;

      // In this modified-Kennedy version, satellite velocity and acceleration
      // will ALWAYS be found using the FIR differentiator filters.
   diff5.Process(gdsMap);
   cerr << "   -> satVX" << endl;
   diff6.Process(gdsMap);
   cerr << "   -> satVY" << endl;
   diff7.Process(gdsMap);
   cerr << "   -> satVZ" << endl;

   diff8.Process(gdsMap);
   cerr << "   -> satAX" << endl;
   diff9.Process(gdsMap);
   cerr << "   -> satAY" << endl;
   diff10.Process(gdsMap);
   cerr << "   -> satAZ" << endl;

      ///// End of derivatives computation

   cerr << "End of derivatives computation" << endl;


      ///// Start of velocity and acceleration computation

   cerr << "Start of velocity and acceleration computation" << endl;

      // Objects to compute statistics
   Stats<double> stvx;
   Stats<double> stvy;
   Stats<double> stvz;
   Stats<double> stax;
   Stats<double> stay;
   Stats<double> staz;

   Stats<double> stvU;
   Stats<double> stvE;
   Stats<double> stvN;
   Stats<double> staU;
   Stats<double> staE;
   Stats<double> staN;


      // We need a counter...
   int counter1( 0 );

      // ... and the number of samples
   int numberSamples( confReader.getValueAsInt( "numSamples", "DEFAULT" ) );


      // Prepare file to print average results
   string avgName;
   ofstream fAverage;

   avgName = confReader.getValue( "avgFile", "DEFAULT" );
   fAverage.open( avgName.c_str(), ios::out );
   fAverage << fixed << setprecision( precision );


      // Let's check if we are going to use the covariance model
   bool useCovModel(
               confReader.getValueAsBoolean( "useCovModel", "DEFAULT" ) );

      // Now, check if we are going to use the FULL covariance model,not just
      // the diagonals
   bool useFullCovModel(
               confReader.getValueAsBoolean( "useFullCovModel", "DEFAULT" ) );



   while( !gdsMap.empty() )
   {

         // Increment counter
      ++counter1;

         // Extract the first epoch of data from the map
      gnssDataMap gdsData( gdsMap.frontEpoch() );

         // Extract the data structures for rover and reference station
      gnssRinex gRover( gdsData.getGnssRinex( rover ) );
      gnssRinex gRef( gdsData.getGnssRinex( reference ) );

         // Check if we have a minimum number of visible satellites
      if( gRover.numSats() < 4 ||
          gRef.numSats()   < 4 )
      {

            // Remove first element
         gdsMap.pop_front_epoch();

            // Skip to next epoch
         continue;
      }



            // Create vectors
      vector<double> y;
      vector<double> dx;
      vector<double> dy;
      vector<double> dz;


         // Get a set with the satellites in view from the rover
      SatIDSet satSet( gRover.getSatID() );

         // This set will hold common satellites for both receivers
      SatIDSet commonSatSet;

         // This vector will have the common satellites
      vector<SatID> satVector;


         // Object for reference satellite
      SatID highSat;
      double elev(0.0);

         // Find higher satellite
      for( SatIDSet::const_iterator itSat = satSet.begin();
           itSat != satSet.end();
           ++itSat )
      {

         double tempElev(0.0);

         try
         {
               // Try to get the elevation of this SV from REFERENCE STATION
               // This way of getting highest satellite guarantees that the
               // chosen satellite is in view from both receivers
            tempElev = gRef.getValue( (*itSat), TypeID::elevation );
         }
         catch(...)
         {
            continue;
         }

            // If we are here, it means the satellite is common to both RX
         commonSatSet.insert( (*itSat) );

         if( tempElev > elev )
         {
            highSat = (*itSat);
            elev = tempElev;
         }

      }  // End of 'for( SatIDSet::const_iterator itSat = satSet.begin();...'


         // Update 'satSet' so it only holds common satellites
      satSet = commonSatSet;

         // Remove reference satellite from satellite set
      satSet.erase( highSat );

         // Include reference satellite in satellite vector
      satVector.push_back( highSat );


         // Declare some important variables for velocity
      double L1dotPM( 0.0 );
      double L1dotPK( 0.0 );
      double rhodotPK( 0.0 );

      double dxPM( 0.0 );
      double dyPM( 0.0 );
      double dzPM( 0.0 );

      double satVXP( 0.0 );
      double satVYP( 0.0 );
      double satVZP( 0.0 );

         // Declare some important variables for acceleration
      double L1dot2PM( 0.0 );
      double L1dot2PK( 0.0 );
      double rhodot2PK( 0.0 );

      double rhoPM( 0.0 );
      double rhodotPM( 0.0 );

      double satAXP( 0.0 );
      double satAYP( 0.0 );
      double satAZP( 0.0 );

         // Extract values related to reference satellite
      try
      {
            // Values to compute velocity
         L1dotPM  = gRover.getValue( highSat, TypeID::L1dot );
         L1dotPK  = gRef.getValue(   highSat, TypeID::L1dot );
         rhodotPK = gRef.getValue(   highSat, TypeID::rhodot );

            // Please note the minus sign (-) because 'dx', 'dy' and 'dz'
            // parameters are computed from satellite to receiver, and here
            // they are needed in the opposite direction
         dxPM     = - gRover.getValue( highSat, TypeID::dx );
         dyPM     = - gRover.getValue( highSat, TypeID::dy );
         dzPM     = - gRover.getValue( highSat, TypeID::dz );

         satVXP   = gRover.getValue( highSat, TypeID::satVX );
         satVYP   = gRover.getValue( highSat, TypeID::satVY );
         satVZP   = gRover.getValue( highSat, TypeID::satVZ );

            // Values to compute acceleration
         L1dot2PM  = gRover.getValue( highSat, TypeID::L1dot2 );
         L1dot2PK  = gRef.getValue(   highSat, TypeID::L1dot2 );
         rhodot2PK = gRef.getValue(   highSat, TypeID::rhodot2 );

         rhoPM = gRover.getValue( highSat, TypeID::rho );
         rhodotPM = gRover.getValue( highSat, TypeID::rhodot );

         satAXP   = gRover.getValue( highSat, TypeID::satAX );
         satAYP   = gRover.getValue( highSat, TypeID::satAY );
         satAZP   = gRover.getValue( highSat, TypeID::satAZ );

      }
      catch(...)
      {

            // Remove first element
         gdsMap.pop_front_epoch();

            // Skip this epoch if highSat doesn't have all the information
         continue;
      }


         // Now that we have the highest satellite, let's compute equations
         // prefilter residuals and geometric parameters for each satellite
      for( SatIDSet::const_iterator it = satSet.begin();
           it != satSet.end();
           ++it )
      {

            // Declare a variable for 'prefit'
         double prefit( 0.0 );

         try
         {

               // Get L1dot-related values
            double L1dotQM( gRover.getValue( (*it), TypeID::L1dot ) );
            double L1dotQK( gRef.getValue(   (*it), TypeID::L1dot ) );

               // Get rhodot-related values
            double rhodotQK( gRef.getValue( (*it), TypeID::rhodot ) );

               // Get geometry-related values
            double dxQM( - gRover.getValue( (*it),   TypeID::dx ) );
            double dyQM( - gRover.getValue( (*it),   TypeID::dy ) );
            double dzQM( - gRover.getValue( (*it),   TypeID::dz ) );

               // Get satellite velocity-related values
            double satVXQ( gRover.getValue( (*it), TypeID::satVX ) );
            double satVYQ( gRover.getValue( (*it), TypeID::satVY ) );
            double satVZQ( gRover.getValue( (*it), TypeID::satVZ ) );

               // Let's compute prefilter residual for this satellite
            prefit = + ( L1dotQM - L1dotPM - L1dotQK + L1dotPK )
                     + ( rhodotQK - rhodotPK )
                     + ( dxPM*satVXP + dyPM*satVYP + dzPM*satVZP )
                     - ( dxQM*satVXQ + dyQM*satVYQ + dzQM*satVZQ );

               // Add values to vectors
            y.push_back( prefit );
            dx.push_back( dxPM - dxQM );
            dy.push_back( dyPM - dyQM );
            dz.push_back( dzPM - dzQM );


               // Include current satellite in satellite vector. 'satVector'
               // contains all common satellites, ordered but the fact that
               // reference satellite is the first one.
            satVector.push_back( (*it) );

         }
         catch(...)
         {
               // Skip this satellite if it doesn't have all the information
            continue;
         }

      }  // End of 'for( SatIDSet::const_iterator itSat = satSet.begin();...'


         // Proceed only if we have enough equations
      if ( y.size() >= 3 )
      {

            // Get rover position at this epoch
         Position rovPos( posMap[ gRover.header.epoch ] );


            // Latitude and longitude of Rover at current epoch
         double roverLat( rovPos.getGeodeticLatitude() );
         double roverLon( rovPos.getLongitude() );


            // Substract reference station position
         rovPos -= refNominalPos;

            // Compute distance between rover and reference station
         double dist( rovPos.mag() );


         // Declare double differences covariance matrix
         Matrix<double> CDD;

         if( useCovModel )
         {

            if( useFullCovModel )
            {

                  // Compute the full double differences covariance matrix
               CDD = computeCDD( satVector,
                                 rover,
                                 reference,
                                 gdsMap,
                                 sigmaIono,
                                 dist );

            }
            else
            {

                  // Compute a simple double differences covariance matrix
               CDD = computeSimpleCDD( satVector,
                                       rover,
                                       reference,
                                       gdsMap,
                                       sigmaIono,
                                       dist );

            }  // End of 'if( useFullCovModel )'

         }
         else
         {

               // Compute a constant double differences covariance matrix
            CDD = computeConstCDD( satVector );

         }  // End of 'if( useCovModel )'



            // Let's try to invert CDD  matrix, because we need 'weights'
         try
         {
            CDD = inverseChol( CDD );
         }
         catch(...)
         {

            cerr << "Unable to invert matrix CDD matrix at epoch: "
                 << YDSTime(gRover.header.epoch).sod()
                 << ". Skipping it." << endl;

               // Remove first element
            gdsMap.pop_front_epoch();

               // Skip to next epoch
            continue;
         }


            // Weights matrix does not include sampling period factor yet
         CDD = (samplingP * samplingP) * CDD;

            // We are ready to build the vector and matrix of the equations
            // Declare Vectors and Matrices
         Vector<double> Y( y.size() );
         Matrix<double> A( y.size(), 3, 0.0 );

            // Introduce values
         for( size_t i = 0; i < y.size(); ++i )
         {
            Y[i] = y[i];
            A( i, 0 ) = dx[i];
            A( i, 1 ) = dy[i];
            A( i, 2 ) = dz[i];

         }

            // Let's declare a WMS solver
         SolverWMS solverV;


            // Try to solve the equation system for velocities
         try
         {
               // Compute the velocity solution
            solverV.Compute( Y, A, CDD );
         }
         catch(...)
         {

               // Remove first element
            gdsMap.pop_front_epoch();

               // Skip to next epoch
            continue;
         }


            // Extract solutions
         double vx( solverV.solution[0] );
         double vy( solverV.solution[1] );
         double vz( solverV.solution[2] );

            // This part will rotate covariances matrix to ENU frame
         double ang(roverLon*DEG_TO_RAD);
         double sinangle(std::sin(ang));
         double cosangle(std::cos(ang));
         Matrix<double> R3( 3, 3, 0.0 );
         R3( 0, 0 ) = cosangle;
         R3( 0, 1 ) = sinangle;
         R3( 1, 0 ) = -sinangle;
         R3( 1, 1 ) = cosangle;
         R3( 2, 2 ) = 1.0;

         ang = -roverLat*DEG_TO_RAD;
         sinangle = std::sin(ang);
         cosangle = std::cos(ang);
         Matrix<double> R2( 3, 3, 0.0 );
         R2( 0, 0 ) = cosangle;
         R2( 0, 2 ) = -sinangle;
         R2( 1, 1 ) = 1.0;
         R2( 2, 0 ) = sinangle;
         R2( 2, 2 ) = cosangle;

         Matrix<double> R(R2*R3);
         Matrix<double> RT( transpose(R) );

            // Covariance matrix for acceleration in ENU reference frame
         Matrix<double> enuCovMatV( R * solverV.covMatrix * RT );


            //// Acceleration computation

            // Let's start clearing prefit-related vectors
         y.clear();


            // Time to compute equations prefilter residuals and geometric
            // parameters for each satellite to get the acceleration solution
         for( SatIDSet::const_iterator it = satSet.begin();
              it != satSet.end();
              ++it )
         {

               // Declare a variable for 'prefit'
            double prefit( 0.0 );

            try
            {

                  // Get L1dot2-related values
               double L1dot2QM( gRover.getValue( (*it), TypeID::L1dot2 ) );
               double L1dot2QK( gRef.getValue(   (*it), TypeID::L1dot2 ) );

                  // Get rhodot2-related values
               double rhodot2QK( gRef.getValue( (*it), TypeID::rhodot2 ) );

                  // Get geometry-related values
               double rhoQM( gRover.getValue( (*it), TypeID::rho ) );
               double rhodotQM( gRover.getValue( (*it), TypeID::rhodot ) );

               double dxQM( - gRover.getValue( (*it),   TypeID::dx ) );
               double dyQM( - gRover.getValue( (*it),   TypeID::dy ) );
               double dzQM( - gRover.getValue( (*it),   TypeID::dz ) );

                  // Get satellite velocity-related values
               double satVXQ( gRover.getValue( (*it), TypeID::satVX ) );
               double satVYQ( gRover.getValue( (*it), TypeID::satVY ) );
               double satVZQ( gRover.getValue( (*it), TypeID::satVZ ) );

                  // Get satellite acceleration-related values
               double satAXQ( gRover.getValue( (*it), TypeID::satAX ) );
               double satAYQ( gRover.getValue( (*it), TypeID::satAY ) );
               double satAZQ( gRover.getValue( (*it), TypeID::satAZ ) );

                  // Compute relative speeds
               double vPM2( ( satVXP - vx ) * ( satVXP - vx )
                          + ( satVYP - vy ) * ( satVYP - vy )
                          + ( satVZP - vz ) * ( satVZP - vz ) );

               double vQM2( ( satVXQ - vx ) * ( satVXQ - vx )
                          + ( satVYQ - vy ) * ( satVYQ - vy )
                          + ( satVZQ - vz ) * ( satVZQ - vz ) );


                  // Let's compute prefilter residual for this satellite
               prefit = + ( L1dot2QM - L1dot2PM - L1dot2QK + L1dot2PK )
                        + ( rhodot2QK - rhodot2PK )
                        + ( dxPM * satAXP + dyPM * satAYP + dzPM * satAZP )
                        - ( dxQM * satAXQ + dyQM * satAYQ + dzQM * satAZQ )
                        + ( vPM2 - ( rhodotPM * rhodotPM ) ) / rhoPM
                        - ( vQM2 - ( rhodotQM * rhodotQM ) ) / rhoQM;

                  // Add values to vectors
               y.push_back( prefit );

            }
            catch(...)
            {
                  // Skip this satellite if it doesn't have all the information
               continue;
            }

         }  // End of 'for( SatIDSet::const_iterator itSat = satSet.begin();...'


            // Put prefits in place
         Y = y;

            // Update weights matrix, multiplying by appropriate factor
         CDD = ( (samplingP * samplingP) / 1.509551839 ) * CDD;

            // Try to solve the equation system for accelerations
         try
         {
               // Compute the velocity solution
            solverV.Compute( Y, A, CDD );
         }
         catch(...)
         {

               // Remove first element
            gdsMap.pop_front_epoch();

               // Skip to next epoch
            continue;
         }


            // Extract solutions
         double ax( solverV.solution[0] );
         double ay( solverV.solution[1] );
         double az( solverV.solution[2] );

            // Covariance matrix for acceleration in ENU reference frame
         Matrix<double> enuCovMatA( R * solverV.covMatrix * RT );


         Triple vECEF(vx, vy, vz);
         Triple vUEN( (vECEF.R3(roverLon)).R2(-roverLat) );

         
         Triple aECEF(ax, ay, az);
         Triple aUEN( (aECEF.R3(roverLon)).R2(-roverLat) );


            // These statistics objects will compute the totals
         stvU.Add( vUEN[0] );
         stvE.Add( vUEN[1] );
         stvN.Add( vUEN[2] );

         staU.Add( aUEN[0] );
         staE.Add( aUEN[1] );
         staN.Add( aUEN[2] );


         Position dispECEF( posMap[ gRover.header.epoch ]
                               -roverOriginalPosition );
         Triple posUEN( (dispECEF.R3(roverLon)).R2(-roverLat) );


         // Check counter
         if( counter1 < numberSamples )
         {

               // Add data to statistics objects
            stvx.Add( vUEN[0] );
            stvy.Add( vUEN[1] );
            stvz.Add( vUEN[2] );
            stax.Add( aUEN[0] );
            stay.Add( aUEN[1] );
            staz.Add( aUEN[2] );

         }
         else
         {

               // Output average values to file
            fAverage << gRover.header.epoch.DOYsecond()
                     << "  VU: " << stvx.Average() << " VUSig: " << stvx.StdDev()
                     << "  VE: " << stvy.Average() << " VESig: " << stvy.StdDev()
                     << "  VN: " << stvz.Average() << " VNSig: " << stvz.StdDev()
                     << "  AU: " << stax.Average() << " AUSig: " << stax.StdDev()
                     << "  AE: " << stay.Average() << " AESig: " << stay.StdDev()
                     << "  AN: " << staz.Average() << " ANSig: " << staz.StdDev() << endl;

               // Clear statistics objects
            stvx.Reset();
            stvy.Reset();
            stvz.Reset();
            stax.Reset();
            stay.Reset();
            staz.Reset();

            counter1 = 0;

            cerr << ".";

         }


            // Print results
         cout << gRover.header.epoch.DOYsecond()   << "  "        // #1
              << vUEN[0]                           << "  "        // #2
              << sqrt( enuCovMatV( 0, 0 ) )        << "  "        // #3
              << vUEN[1]                           << "  "        // #4
              << sqrt( enuCovMatV( 1, 1 ) )        << "  "        // #5
              << vUEN[2]                           << "  "        // #6
              << sqrt( enuCovMatV( 2, 2 ) )        << "  "        // #7
              << aUEN[0]                           << "  "        // #8
              << sqrt( enuCovMatA( 0, 0 ) )        << "  "        // #9
              << aUEN[1]                           << "  "        // #10
              << sqrt( enuCovMatA( 1, 1 ) )        << "  "        // #11
              << aUEN[2]                           << "  "        // #12
              << sqrt( enuCovMatA( 2, 2 ) )        << "  "        // #13
              << posUEN[0]                         << "  "        // #14
              << posUEN[1]                         << "  "        // #15
              << posUEN[2]                         << endl;       // #16



      }  // End of 'if ( y.size() >= 3 )...'


         // Now that we have the velocity, let's compute the acceleration


         // Remove first element
      gdsMap.pop_front_epoch();


   }  // End of 'while( !gdsMap.empty() )'


      // Close averages file
   fAverage.close();

      // Add end of line
   cerr << "." << endl;

   cerr << "Velocity results in m/s:" << endl
        << "VUavg: " << stvU.Average() << "  VUsig: " << stvU.StdDev() << endl
        << "VEavg: " << stvE.Average() << "  VEsig: " << stvE.StdDev() << endl
        << "VNavg: " << stvN.Average() << "  VNsig: " << stvN.StdDev() << endl
        << "Acceleration results in m/s^2:" << endl
        << "AUavg: " << staU.Average() << "  AUsig: " << staU.StdDev() << endl
        << "AEavg: " << staE.Average() << "  AEsig: " << staE.StdDev() << endl
        << "ANavg: " << staN.Average() << "  ANsig: " << staN.StdDev() << endl;


   exit(0);

}
