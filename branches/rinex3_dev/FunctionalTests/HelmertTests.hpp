#ifndef GPSTK_HELMERT_TRANSFORM_TESTS
#define GPSTK_HELMERT_TRANSFORM_TESTS

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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include <iostream>

#include "HelmertTransform.hpp"
#include "ReferenceFrame.hpp"
#include "Position.hpp"
#include "Exception.hpp"

bool testNoRotationNoScaleNoTranslation();
bool testArbitraryTranslation();
bool testScaling();
bool testSmallRotation();
bool testXAxisRotation();
bool testYAxisRotation();
bool testZAxisRotation();
bool testTranslationScaling();
bool testRotationScaling();
bool testRotationTranslation();
bool realData();
bool isEverythingDocumented() { return false; }

std::ostream& printBool(const bool print, std::ostream& out);
std::ostream& operator<<(std::ostream& out, gpstk::Position& pos);
std::ostream& operator<<(std::ostream& out, const bool right);
bool compare(gpstk::Position& left, gpstk::Position& right);

#endif
