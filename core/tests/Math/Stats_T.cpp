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

#include "Stats.hpp"
#include "Vector.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <sstream>

class Stats_T
{
public: 
   Stats_T() // Default Constructor, set the precision values
   {
      floatPrecision = 1e-7f;
      doublePrecision = 1e-14;
      longDoublePrecision = 1e-30;
   }
   ~Stats_T() {} // Default Desructor

//=============================================================================
//      floatStatsTest tests the accuracy of the Stats class with floats
//=============================================================================
   int floatStatsTest( void )
   {
      TestUtil testFramework( "Stats", "Add", __FILE__, __LINE__ );
      std::string failMesg;

      gpstk::Stats<float> floatStatsObject;


         //----------------------------------------------------------
         // Add values to the statistics
         //----------------------------------------------------------

         //Add a single value
      floatStatsObject.Add((float)1.0, (float)1.0);
      int expectedNAdd = 1;
      testFramework.assert(floatStatsObject.N() == expectedNAdd, "Value was not added", __LINE__);

         //Add a gpstk::Vector of values
      testFramework.changeSourceMethod("AddGpstkVector");
      gpstk::Vector<float> input(3), weight(3);
      for (int i = 0; i < 3; i++)
      {
         input[i] = (float)i+2;
         weight[i] = (float)i+2;
      }
      floatStatsObject.Add(input, weight); 
      int expectedNVectorAdd = 4;
      testFramework.assert(floatStatsObject.N() == expectedNVectorAdd, "GPSTk Vector of values was not added", __LINE__);


         //----------------------------------------------------------
         // Check statistics
         //----------------------------------------------------------
      float expectedAverage  = 3.0;
      float expectedMaximum  = 4.0;
      float expectedMinimum  = 1.0;
      float expectedVariance = 1.0;
      float expectedStdDev   = 1.0;
      float expectedNormalization = 10.0;
      float relativeError;

      testFramework.changeSourceMethod("Average");
      TFASSERTFE(expectedAverage, floatStatsObject.Average());

      testFramework.changeSourceMethod("Maximum");
      TFASSERTFE(expectedMaximum, floatStatsObject.Maximum());

      testFramework.changeSourceMethod("Minimum");
      TFASSERTFE(expectedMinimum, floatStatsObject.Minimum());

      testFramework.changeSourceMethod("Variance");
      TFASSERTFE(expectedVariance, floatStatsObject.Variance());

      testFramework.changeSourceMethod("StdDev");
      TFASSERTFE(expectedStdDev, floatStatsObject.StdDev());

      testFramework.changeSourceMethod("Normalization");
      TFASSERTFE(expectedNormalization, floatStatsObject.Normalization());

      testFramework.changeSourceMethod("Weighted");
      testFramework.assert(floatStatsObject.Weighted(), "Weighted returned false for weighted sample", __LINE__);


         //----------------------------------------------------------
         // Subtract Data
         //----------------------------------------------------------   
      testFramework.changeSourceMethod("Subtract");     
      int expectedNSubtract = 3;
      floatStatsObject.Subtract((float)1.0, (float)1.0);
      testFramework.assert(floatStatsObject.N() == expectedNSubtract, "Value was not subtracted", __LINE__);

      testFramework.changeSourceMethod("SubtractGpstkVector");  
      int expectedNSubtractVector = 0;
      float expectedAvgSubtracted = 0.0;
      floatStatsObject.Subtract(input);
      testFramework.assert(floatStatsObject.N() == expectedNSubtractVector, "GPSTk Vector was not subtracted", __LINE__);
      relativeError = fabs(floatStatsObject.Average() - expectedAvgSubtracted);
      testFramework.assert(relativeError < floatPrecision, "Subtraction did not remove all of the data. Average != 0", __LINE__);       
      relativeError = fabs(floatStatsObject.Normalization() - float(0.0));
      testFramework.assert(relativeError < floatPrecision, "Subtraction did not remove all of the data. Normalization != 0", __LINE__);
         //----------------------------------------------------------
         // Load Data into new Stats Object
         //----------------------------------------------------------   
      gpstk::Stats<float> loadedStatsObject;    
      testFramework.changeSourceMethod("Load");
      loadedStatsObject.Load(expectedNVectorAdd, expectedMinimum, expectedMaximum, expectedAverage, expectedVariance, true, expectedNormalization);

      testFramework.assert(loadedStatsObject.N() == expectedNVectorAdd, "Load did not set n properly", __LINE__);
      relativeError = fabs(loadedStatsObject.Average() - expectedAverage)/expectedAverage;
      testFramework.assert(relativeError < floatPrecision, "Load did not set the correct average", __LINE__);
      relativeError = fabs(loadedStatsObject.Maximum() - expectedMaximum)/expectedMaximum;
      testFramework.assert(relativeError < floatPrecision, "Load did not set the correct maximum", __LINE__);
      relativeError = fabs(loadedStatsObject.Minimum() - expectedMinimum)/expectedMinimum;
      testFramework.assert(relativeError < floatPrecision, "Load did not set the correct minimum", __LINE__);   
      relativeError = fabs(loadedStatsObject.Variance() - expectedVariance)/expectedVariance;
      testFramework.assert(relativeError < floatPrecision, "Load did not set the correct variance", __LINE__);
      relativeError = fabs(loadedStatsObject.Normalization() - expectedNormalization)/expectedNormalization;
      testFramework.assert(relativeError < floatPrecision, "Load did not set the correct normalization", __LINE__);
      testFramework.assert(loadedStatsObject.Weighted(), "Load did not set Weighted properly", __LINE__);


         //----------------------------------------------------------
         // Add the Stats objects and dump the information
         //----------------------------------------------------------   
      testFramework.changeSourceMethod("OperatorAddAndAssign");                 
      floatStatsObject += loadedStatsObject;

      testFramework.assert(floatStatsObject.N() == expectedNVectorAdd, "Addition of stats objects did not set n properly", __LINE__);
      relativeError = fabs(floatStatsObject.Average() - expectedAverage)/expectedAverage;
      testFramework.assert(relativeError < floatPrecision, "Addition of stats objects did not set the correct average", __LINE__);
      relativeError = fabs(floatStatsObject.Maximum() - expectedMaximum)/expectedMaximum;
      testFramework.assert(relativeError < floatPrecision, "Addition of stats objects did not set the correct maximum", __LINE__);
      relativeError = fabs(floatStatsObject.Minimum() - expectedMinimum)/expectedMinimum;
      testFramework.assert(relativeError < floatPrecision, "Addition of stats objects did not set the correct minimum", __LINE__);      
      relativeError = fabs(floatStatsObject.Variance() - expectedVariance)/expectedVariance;
      testFramework.assert(relativeError < floatPrecision, "Addition of stats objects did not set the correct variance", __LINE__);
      relativeError = fabs(floatStatsObject.Normalization() - expectedNormalization)/expectedNormalization;
      testFramework.assert(relativeError < floatPrecision, "Addition of stats objects did not set the correct normalization", __LINE__);
      testFramework.assert(floatStatsObject.Weighted(), "Addition of stats objects did not set Weighted properly", __LINE__);

      std::stringstream obtainedOutput;
      std::stringstream expectedOutput;

      obtainedOutput << floatStatsObject;

      std::ofstream savefmt;
      savefmt.copyfmt(expectedOutput);
      expectedOutput << " N       = " << floatStatsObject.N() << (floatStatsObject.Weighted() ? " ":" not") << " weighted\n";
      expectedOutput << " Minimum = ";  expectedOutput.copyfmt(savefmt);  expectedOutput << floatStatsObject.Minimum();
      expectedOutput << " Maximum = ";  expectedOutput.copyfmt(savefmt);  expectedOutput << floatStatsObject.Maximum() << "\n";
      expectedOutput << " Average = ";  expectedOutput.copyfmt(savefmt);  expectedOutput << floatStatsObject.Average();
      expectedOutput << " Std Dev = ";  expectedOutput.copyfmt(savefmt);  expectedOutput << floatStatsObject.StdDev();
      expectedOutput << " Variance = "; expectedOutput.copyfmt(savefmt);  expectedOutput << floatStatsObject.Variance();

      testFramework.changeSourceMethod("OperatorRedirect");
      testFramework.assert(obtainedOutput.str() == expectedOutput.str(), "Dump of Stats output is not what was expected", __LINE__);

      return testFramework.countFails();                                        
   }



//=============================================================================
//      doubleStatsTest tests the accuracy of the Stats class with doubles
//=============================================================================
   int doubleStatsTest( void )
   {
      TestUtil testFramework( "Stats", "Add", __FILE__, __LINE__ );
      std::string failMesg;

      gpstk::Stats<double> doubleStatsObject;


         //----------------------------------------------------------
         // Add values to the statistics
         //----------------------------------------------------------

         //Add a single value
      doubleStatsObject.Add((double)1.0, (double)1.0);
      int expectedNAdd = 1;
      testFramework.assert(doubleStatsObject.N() == expectedNAdd, "Value was not added", __LINE__);

         //Add a gpstk::Vector of values
      testFramework.changeSourceMethod("AddGpstkVector");
      gpstk::Vector<double> input(3), weight(3);
      for (int i = 0; i < 3; i++)
      {
         input(i) = (double)i+2;
         weight(i) = (double)i+2;
      }
      doubleStatsObject.Add(input, weight); 
      int expectedNVectorAdd = 4;
      testFramework.assert(doubleStatsObject.N() == expectedNVectorAdd, "GPSTk Vector of values was not added", __LINE__);


         //----------------------------------------------------------
         // Check statistics
         //----------------------------------------------------------
      double expectedAverage  = 3.0;
      double expectedMaximum  = 4.0;
      double expectedMinimum  = 1.0;
      double expectedVariance = 1.0;
      double expectedStdDev   = 1.0;
      double expectedNormalization = 10.0;
      double relativeError;

      testFramework.changeSourceMethod("Average");
      relativeError = fabs(doubleStatsObject.Average() - expectedAverage)/expectedAverage;
      testFramework.assert(relativeError < doublePrecision, "Average found was not correct", __LINE__);

      testFramework.changeSourceMethod("Maximum");
      relativeError = fabs(doubleStatsObject.Maximum() - expectedMaximum)/expectedMaximum;
      testFramework.assert(relativeError < doublePrecision, "Maximum found was not correct", __LINE__);

      testFramework.changeSourceMethod("Minimum");
      relativeError = fabs(doubleStatsObject.Minimum() - expectedMinimum)/expectedMinimum;
      testFramework.assert(relativeError < doublePrecision, "Minimum found was not correct", __LINE__); 

      testFramework.changeSourceMethod("Variance");
      relativeError = fabs(doubleStatsObject.Variance() - expectedVariance)/expectedVariance;
      testFramework.assert(relativeError < doublePrecision, "Variance found was not correct", __LINE__);

      testFramework.changeSourceMethod("StdDev");
      relativeError = fabs(doubleStatsObject.StdDev() - expectedStdDev)/expectedStdDev;
      testFramework.assert(relativeError < doublePrecision, "StdDev found was not correct", __LINE__);

      testFramework.changeSourceMethod("Normalization");
      relativeError = fabs(doubleStatsObject.Normalization() - expectedNormalization)/expectedNormalization;
      testFramework.assert(relativeError < doublePrecision, "Normalization found was not correct", __LINE__);

      testFramework.changeSourceMethod("Weighted");
      testFramework.assert(doubleStatsObject.Weighted(), "Weighted returned false for weighted sample", __LINE__);


         //----------------------------------------------------------
         // Subtract Data
         //----------------------------------------------------------   
      testFramework.changeSourceMethod("Subtract");     
      int expectedNSubtract = 3;
      doubleStatsObject.Subtract((double)1.0, (double)1.0);
      testFramework.assert(doubleStatsObject.N() == expectedNSubtract, "Value was not subtracted", __LINE__);

      testFramework.changeSourceMethod("SubtractGpstkVector");  
      int expectedNSubtractVector = 0;
      double expectedAvgSubtracted = 0.0;
      doubleStatsObject.Subtract(input);
      testFramework.assert(doubleStatsObject.N() == expectedNSubtractVector, "GPSTk Vector was not subtracted", __LINE__);
      relativeError = fabs(doubleStatsObject.Average() - expectedAvgSubtracted);
      testFramework.assert(relativeError < doublePrecision, "Subtraction did not remove all of the data. Average != 0", __LINE__);      
      relativeError = fabs(doubleStatsObject.Normalization() - double(0.0));
      testFramework.assert(relativeError < doublePrecision, "Subtraction did not remove all of the data. Normalization != 0", __LINE__);
         //----------------------------------------------------------
         // Load Data into new Stats Object
         //----------------------------------------------------------   
      gpstk::Stats<double> loadedStatsObject;   
      testFramework.changeSourceMethod("Load");
      loadedStatsObject.Load(expectedNVectorAdd, expectedMinimum, expectedMaximum, expectedAverage, expectedVariance, true, expectedNormalization);

      testFramework.assert(loadedStatsObject.N() == expectedNVectorAdd, "Load did not set n properly", __LINE__);
      relativeError = fabs(loadedStatsObject.Average() - expectedAverage)/expectedAverage;
      testFramework.assert(relativeError < doublePrecision, "Load did not set the correct average", __LINE__);
      relativeError = fabs(loadedStatsObject.Maximum() - expectedMaximum)/expectedMaximum;
      testFramework.assert(relativeError < doublePrecision, "Load did not set the correct maximum", __LINE__);
      relativeError = fabs(loadedStatsObject.Minimum() - expectedMinimum)/expectedMinimum;
      testFramework.assert(relativeError < doublePrecision, "Load did not set the correct minimum", __LINE__);  
      relativeError = fabs(loadedStatsObject.Variance() - expectedVariance)/expectedVariance;
      testFramework.assert(relativeError < doublePrecision, "Load did not set the correct variance", __LINE__);
      relativeError = fabs(loadedStatsObject.Normalization() - expectedNormalization)/expectedNormalization;
      testFramework.assert(relativeError < doublePrecision, "Load did not set the correct normalization", __LINE__);
      testFramework.assert(loadedStatsObject.Weighted(), "Load did not set Weighted properly", __LINE__);


         //----------------------------------------------------------
         // Add the Stats objects and dump the information
         //----------------------------------------------------------   
      testFramework.changeSourceMethod("OperatorAddAndAssign");                 
      doubleStatsObject += loadedStatsObject;

      testFramework.assert(doubleStatsObject.N() == expectedNVectorAdd, "Addition of stats objects did not set n properly", __LINE__);
      relativeError = fabs(doubleStatsObject.Average() - expectedAverage)/expectedAverage;
      testFramework.assert(relativeError < doublePrecision, "Addition of stats objects did not set the correct average", __LINE__);
      relativeError = fabs(doubleStatsObject.Maximum() - expectedMaximum)/expectedMaximum;
      testFramework.assert(relativeError < doublePrecision, "Addition of stats objects did not set the correct maximum", __LINE__);
      relativeError = fabs(doubleStatsObject.Minimum() - expectedMinimum)/expectedMinimum;
      testFramework.assert(relativeError < doublePrecision, "Addition of stats objects did not set the correct minimum", __LINE__);     
      relativeError = fabs(doubleStatsObject.Variance() - expectedVariance)/expectedVariance;
      testFramework.assert(relativeError < doublePrecision, "Addition of stats objects did not set the correct variance", __LINE__);
      relativeError = fabs(doubleStatsObject.Normalization() - expectedNormalization)/expectedNormalization;
      testFramework.assert(relativeError < doublePrecision, "Addition of stats objects did not set the correct normalization", __LINE__);
      testFramework.assert(doubleStatsObject.Weighted(), "Addition of stats objects did not set Weighted properly", __LINE__);

      std::stringstream obtainedOutput;
      std::stringstream expectedOutput;

      obtainedOutput << doubleStatsObject;

      std::ofstream savefmt;
      savefmt.copyfmt(expectedOutput);
      expectedOutput << " N       = " << doubleStatsObject.N() << (doubleStatsObject.Weighted() ? " ":" not") << " weighted\n";
      expectedOutput << " Minimum = ";  expectedOutput.copyfmt(savefmt);  expectedOutput << doubleStatsObject.Minimum();
      expectedOutput << " Maximum = ";  expectedOutput.copyfmt(savefmt);  expectedOutput << doubleStatsObject.Maximum() << "\n";
      expectedOutput << " Average = ";  expectedOutput.copyfmt(savefmt);  expectedOutput << doubleStatsObject.Average();
      expectedOutput << " Std Dev = ";  expectedOutput.copyfmt(savefmt);  expectedOutput << doubleStatsObject.StdDev();
      expectedOutput << " Variance = "; expectedOutput.copyfmt(savefmt);  expectedOutput << doubleStatsObject.Variance();

      testFramework.changeSourceMethod("OperatorRedirect");
      testFramework.assert(obtainedOutput.str() == expectedOutput.str(), "Dump of Stats output is not what was expected", __LINE__);

      return testFramework.countFails();                                        
   }



//=============================================================================
//      longDoubleStatsTest tests the accuracy of the Stats class with doubles
//=============================================================================
   int longDoubleStatsTest( void )
   {
      TestUtil testFramework( "Stats", "Add", __FILE__, __LINE__ );
      std::string failMesg;

      gpstk::Stats<long double> longDoubleStatsObject;


         //----------------------------------------------------------
         // Add values to the statistics
         //----------------------------------------------------------

         //Add a single value
      longDoubleStatsObject.Add((long double)1.0, (long double)1.0);
      int expectedNAdd = 1;
      testFramework.assert(longDoubleStatsObject.N() == expectedNAdd, "Value was not added", __LINE__);

         //Add a gpstk::Vector of values
      testFramework.changeSourceMethod("AddGpstkVector");
      gpstk::Vector<long double> input(3), weight(3);
      for (int i = 0; i < 3; i++)
      {
         input(i) = (long double)i+2;
         weight(i) = (long double)i+2;
      }
      longDoubleStatsObject.Add(input, weight); 
      int expectedNVectorAdd = 4;
      testFramework.assert(longDoubleStatsObject.N() == expectedNVectorAdd, "GPSTk Vector of values was not added", __LINE__);


         //----------------------------------------------------------
         // Check statistics
         //----------------------------------------------------------
      double expectedAverage  = 3.0;
      double expectedMaximum  = 4.0;
      double expectedMinimum  = 1.0;
      double expectedVariance = 1.0;
      double expectedStdDev   = 1.0;
      double expectedNormalization = 10.0;
      double relativeError;

      testFramework.changeSourceMethod("Average");
      relativeError = fabs(longDoubleStatsObject.Average() - expectedAverage)/expectedAverage;
      testFramework.assert(relativeError < doublePrecision, "Average found was not correct", __LINE__);

      testFramework.changeSourceMethod("Maximum");
      relativeError = fabs(longDoubleStatsObject.Maximum() - expectedMaximum)/expectedMaximum;
      testFramework.assert(relativeError < doublePrecision, "Maximum found was not correct", __LINE__);

      testFramework.changeSourceMethod("Minimum");
      relativeError = fabs(longDoubleStatsObject.Minimum() - expectedMinimum)/expectedMinimum;
      testFramework.assert(relativeError < doublePrecision, "Minimum found was not correct", __LINE__); 

      testFramework.changeSourceMethod("Variance");
      relativeError = fabs(longDoubleStatsObject.Variance() - expectedVariance)/expectedVariance;
      testFramework.assert(relativeError < doublePrecision, "Variance found was not correct", __LINE__);

      testFramework.changeSourceMethod("StdDev");
      relativeError = fabs(longDoubleStatsObject.StdDev() - expectedStdDev)/expectedStdDev;
      testFramework.assert(relativeError < doublePrecision, "StdDev found was not correct", __LINE__);

      testFramework.changeSourceMethod("Normalization");
      relativeError = fabs(longDoubleStatsObject.Normalization() - expectedNormalization)/expectedNormalization;
      testFramework.assert(relativeError < doublePrecision, "Normalization found was not correct", __LINE__);

      testFramework.changeSourceMethod("Weighted");
      testFramework.assert(longDoubleStatsObject.Weighted(), "Weighted returned false for weighted sample", __LINE__);


         //----------------------------------------------------------
         // Subtract Data
         //----------------------------------------------------------   
      testFramework.changeSourceMethod("Subtract");     
      int expectedNSubtract = 3;
      longDoubleStatsObject.Subtract((long double)1.0, (long double)1.0);
      testFramework.assert(longDoubleStatsObject.N() == expectedNSubtract, "Value was not subtracted", __LINE__);

      testFramework.changeSourceMethod("SubtractGpstkVector");  
      int expectedNSubtractVector = 0;
      double expectedAvgSubtracted = 0.0;
      longDoubleStatsObject.Subtract(input);
      testFramework.assert(longDoubleStatsObject.N() == expectedNSubtractVector, "GPSTk Vector was not subtracted", __LINE__);
      relativeError = fabs(longDoubleStatsObject.Average() - expectedAvgSubtracted);
      testFramework.assert(relativeError < doublePrecision, "Subtraction did not remove all of the data. Average != 0", __LINE__);      
      relativeError = fabs(longDoubleStatsObject.Normalization() - double(0.0));
      testFramework.assert(relativeError < doublePrecision, "Subtraction did not remove all of the data. Normalization != 0", __LINE__);
         //----------------------------------------------------------
         // Load Data into new Stats Object
         //----------------------------------------------------------   
      gpstk::Stats<long double> loadedStatsObject;      
      testFramework.changeSourceMethod("Load");
      loadedStatsObject.Load(expectedNVectorAdd, expectedMinimum, expectedMaximum, expectedAverage, expectedVariance, true, expectedNormalization);

      testFramework.assert(loadedStatsObject.N() == expectedNVectorAdd, "Load did not set n properly", __LINE__);
      relativeError = fabs(loadedStatsObject.Average() - expectedAverage)/expectedAverage;
      testFramework.assert(relativeError < doublePrecision, "Load did not set the correct average", __LINE__);
      relativeError = fabs(loadedStatsObject.Maximum() - expectedMaximum)/expectedMaximum;
      testFramework.assert(relativeError < doublePrecision, "Load did not set the correct maximum", __LINE__);
      relativeError = fabs(loadedStatsObject.Minimum() - expectedMinimum)/expectedMinimum;
      testFramework.assert(relativeError < doublePrecision, "Load did not set the correct minimum", __LINE__);  
      relativeError = fabs(loadedStatsObject.Variance() - expectedVariance)/expectedVariance;
      testFramework.assert(relativeError < doublePrecision, "Load did not set the correct variance", __LINE__);
      relativeError = fabs(loadedStatsObject.Normalization() - expectedNormalization)/expectedNormalization;
      testFramework.assert(relativeError < doublePrecision, "Load did not set the correct normalization", __LINE__);
      testFramework.assert(loadedStatsObject.Weighted(), "Load did not set Weighted properly", __LINE__);


         //----------------------------------------------------------
         // Add the Stats objects and dump the information
         //----------------------------------------------------------   
      testFramework.changeSourceMethod("OperatorAddAndAssign");                 
      longDoubleStatsObject += loadedStatsObject;

      testFramework.assert(longDoubleStatsObject.N() == expectedNVectorAdd, "Addition of stats objects did not set n properly", __LINE__);
      relativeError = fabs(longDoubleStatsObject.Average() - expectedAverage)/expectedAverage;
      testFramework.assert(relativeError < doublePrecision, "Addition of stats objects did not set the correct average", __LINE__);
      relativeError = fabs(longDoubleStatsObject.Maximum() - expectedMaximum)/expectedMaximum;
      testFramework.assert(relativeError < doublePrecision, "Addition of stats objects did not set the correct maximum", __LINE__);
      relativeError = fabs(longDoubleStatsObject.Minimum() - expectedMinimum)/expectedMinimum;
      testFramework.assert(relativeError < doublePrecision, "Addition of stats objects did not set the correct minimum", __LINE__);     
      relativeError = fabs(longDoubleStatsObject.Variance() - expectedVariance)/expectedVariance;
      testFramework.assert(relativeError < doublePrecision, "Addition of stats objects did not set the correct variance", __LINE__);
      relativeError = fabs(longDoubleStatsObject.Normalization() - expectedNormalization)/expectedNormalization;
      testFramework.assert(relativeError < doublePrecision, "Addition of stats objects did not set the correct normalization", __LINE__);
      testFramework.assert(longDoubleStatsObject.Weighted(), "Addition of stats objects did not set Weighted properly", __LINE__);

      std::stringstream obtainedOutput;
      std::stringstream expectedOutput;

      obtainedOutput << longDoubleStatsObject;

      std::ofstream savefmt;
      savefmt.copyfmt(expectedOutput);
      expectedOutput << " N       = " << longDoubleStatsObject.N() << (longDoubleStatsObject.Weighted() ? " ":" not") << " weighted\n";
      expectedOutput << " Minimum = ";  expectedOutput.copyfmt(savefmt);  expectedOutput << longDoubleStatsObject.Minimum();
      expectedOutput << " Maximum = ";  expectedOutput.copyfmt(savefmt);  expectedOutput << longDoubleStatsObject.Maximum() << "\n";
      expectedOutput << " Average = ";  expectedOutput.copyfmt(savefmt);  expectedOutput << longDoubleStatsObject.Average();
      expectedOutput << " Std Dev = ";  expectedOutput.copyfmt(savefmt);  expectedOutput << longDoubleStatsObject.StdDev();
      expectedOutput << " Variance = "; expectedOutput.copyfmt(savefmt);  expectedOutput << longDoubleStatsObject.Variance();

      testFramework.changeSourceMethod("OperatorRedirect");
      testFramework.assert(obtainedOutput.str() == expectedOutput.str(), "Dump of Stats output is not what was expected", __LINE__);

      return testFramework.countFails();                                        
   }

private:
   float       floatPrecision;
   double      doublePrecision;
   long double longDoublePrecision;
};



int main() //Main function to initialize and run all tests above
{
   int check, errorCounter = 0;
   Stats_T testClass;

   check = testClass.floatStatsTest();
   errorCounter += check;

   check = testClass.doubleStatsTest();
   errorCounter += check;

   check = testClass.longDoubleStatsTest();
   errorCounter += check;

   std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

   return errorCounter; //Return the total number of errors
}
