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

const char* bash_linux_normal = "\e[0m";
const char* bash_linux_red= "\e[1;31m";
const char* bash_linux_green = "\e[1;32m";
const char* bash_linux_yellow = "\e[1;33m";
const char* bash_linux_blue = "\e[1;34m";
const char* seperatorEquals = "==============================================="
                              "=================================";

bool color;
bool fancy;

bool all;
bool nrnsnt;
bool arbitraryTranslation;
bool scaleOnly;
bool xAxisRot;
bool yAxisRot;
bool zAxisRot;
bool smallRot;
bool translationScaling;
bool rotationScaling;
bool rotationTranslation;
bool realData;

int testNoRotationNoScaleNoTranslation();
int testArbitraryTranslation();
int testScaling();
int testSmallRotation();
int testXAxisRotation();
int testYAxisRotation();
int testZAxisRotation();
int testTranslationScaling();
int testRotationScaling();
int testRotationTranslation();
int testRealData();
int isEverythingDocumented() { return true; }

/*I don't have to worry about option arguments, so why not do it myself?*/
void parseArguments(int argc, char** argv);
void printHelp();
void printTitle(const char* str);

std::ostream& printBool(const bool print, std::ostream& out);
std::ostream& operator<<(std::ostream& out, gpstk::Position& pos);
std::ostream& operator<<(std::ostream& out, const bool right);
bool compare(gpstk::Position& left, gpstk::Position& right);

#endif
