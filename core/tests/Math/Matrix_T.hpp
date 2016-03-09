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

#ifndef GPSTK_MATRIX_T_HPP
#define GPSTK_MATRIX_T_HPP

#include "Matrix.hpp"
#include "Vector.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

class Matrix_T
{
public:
   Matrix_T() // Default Constructor, set values that will be used frequently
   {
      eps = 1e-12;
      double dat1[6] = {2,5,5,
                        -3,-7,2};

      double dat2[12] = {1,0,-2,1,
                         3,1,-2,2,
                         -5,-1,9,3};
      double dat3[20] = {2, 3, 1, 5, 1, 
                         1, 0, 3, 1, 2, 
                         0, 2, -3, 2, 3, 
                         0, 2, 3, 1, 4};
      double dat4[24] = {8,5,18,-2,1.5,1./6,
                         7,-9,5,0,7,0,
                         1,7,10,11,47,52,
                         -78,24,20,-68,0,0};


      //Temp values needed b/c can't assign matrix size to public members at initialization
      //The temps can give the matrices the sizes (and values) needed
      gpstk::Matrix<double> A1augTemp(2,3), A2augTemp(3,4), A3augTemp(4,5), A4augTemp(4,6);

      A1augTemp = dat1;
      A2augTemp = dat2;
      A3augTemp = dat3;
      A4augTemp = dat4;

      gpstk::Matrix<double> A1Temp(A1augTemp, 0, 0, 2, 2);
      gpstk::Matrix<double> A2Temp(A2augTemp, 0, 0, 3, 3);
      gpstk::Matrix<double> A3Temp(A3augTemp, 0, 0, 4, 4);
      gpstk::Matrix<double> A4Temp(A4augTemp, 0, 0, 4, 5);

      A1aug = A1augTemp;
      A2aug = A2augTemp;
      A3aug = A3augTemp;
      A4aug = A4augTemp;

      A1 = A1Temp;
      A2 = A2Temp;
      A3 = A3Temp;
      A4 = A4Temp;

      gpstk::Vector<double> B1Temp(A1aug.colCopy(2));
      gpstk::Vector<double> B2Temp(A2aug.colCopy(3));
      gpstk::Vector<double> B3Temp(A3aug.colCopy(4));

      B1 = B1Temp;
      B2 = B2Temp;
      B3 = B3Temp;
   }
   
   ~Matrix_T() // Default Desructor
   {       // finally delete objects

   } 

   double eps;
   gpstk::Matrix<double> A1aug, A2aug, A3aug, A4aug, A1, A2, A3, A4;
   gpstk::Vector<double> B1, B2, B3;

   //Functions in Matrix_Initialization_T.cpp
   int initializeConstantsTest(void);
   int initializeVectorsTest(void);
   int initializeArrayTest(void);

   //Functions in Matrix_Sizing_T.cpp
   int sizeTest(void);

   //Functions in Matrix_Operators_T.cpp
   int operatorTest(void);

   //Functions in Matrix_InverseTranspose_T.cpp
   int inverseTest(void);
   int transposeTest(void);
   int solutionTest(void);
   int determinantTest(void);

   //Functions in Matrix_LUDecomp_T.cpp
   int LUinitializationTest(void);
   int LUdeterminantTest(void);	
   int LUATest(void);
   int LUbackSubTest(void);

   //Functions in Matrix_SVD_T.cpp
   int SVDATest(void);
   int SVDBackSubTest(void);
   int SVDDeterminantTest(void);


private:
   std::stringstream failDescriptionStream;
   std::string       failDescriptionString;
   std::string       failMesg;

};

#endif
