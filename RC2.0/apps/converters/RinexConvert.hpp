#pragma ident "$Id: RinexConvert.hpp 2009-08-25 17:32:36 tvarney $"

#ifndef RINEX_CONVERTER_HPP
#define RINEX_CONVERTER_HPP

//============================================================================//
//                                  License                                   //
//                                                                            //
//  This file is part of GPSTk, the GPS Toolkit.                              //
//                                                                            //
//  The GPSTk is free software; you can redistribute it and/or modify it      //
//  under the terms of the GNU Lesser General Public License as published     //
//  by the Free Software Foundation; either version 2.1 of the License, or    //
//  any later version.                                                        //
//                                                                            //
//  The GPSTk is distributed in the hope that it will be useful, but          //
//  WITHOUT ANY WARRANTY; without even the implied warranty of                //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                      //
//  See the GNU Lesser General Public License for more details.               //
//                                                                            //
//  You should have received a copy of the GNU Lesser General Public License  //
//  along with GPSTk; if not, write to the Free Software Foundation, Inc.,    //
//  51 Franklin Street, Fifth Floor, Boston, MA 02110, USA                   //
//                                                                            //
//  Copyright 2009, The University of Texas at Austin                         //
//                                                                            //
//============================================================================//
//                                  Includes                                  //
//----------------------------------------------------------------------------//
//                               Command Options                              //
#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"
//----------------------------------------------------------------------------//
//                                 Data Types                                 //
#include "RinexObsBase.hpp"
#include "Rinex3ObsBase.hpp"

#include "RinexObsStream.hpp"
#include "Rinex3ObsStream.hpp"

#include "RinexObsHeader.hpp"
#include "Rinex3ObsHeader.hpp"

#include "RinexObsData.hpp"
#include "Rinex3ObsData.hpp"
//----------------------------------------------------------------------------//
//                                Other GPSTk                                 //
#include "RinexConverter.hpp"
#include "RinexUtilities.hpp"
#include "StringUtils.hpp"
//----------------------------------------------------------------------------//
//                              Standard Library                              //
#include <iostream>
#include <vector>
//============================================================================//
//                             Function Prototypes                            //
void printTitle(ostream* out);
int parseCommandLine(int argc, char** argv);

bool convertRinex2ObsFile(std::string& fileName, std::string& outFile);
bool convertRinex3ObsFile(std::string& fileName, std::string& outFile);

bool convertRinex2NavFile(std::string& fileName, std::string& outFile);
bool convertRinex3NavFile(std::string& fileName, std::string& outFile);
//============================================================================//
//                                 Global Data                                //
std::string programName("RinexConvert");
std::string license("This file licensed under the GNU Lesser General Public "
                    "License.\n\nYou should have received a copy of the GNU "
                    "Lesser General Public License with\nthis software; if not,"
                    " write to the Free Software Foundation, Inc.,\n59 Temple "
                    "Place, Suite 330, Boston, MA  02111-1307  USA\n");
std::string author("Troy Varney");
std::string date("8/25/09");
std::string description("Rinex Convert takes one or more observation or "
                        "navigation files in the\nRINEX 2.11 or RINEX 3.0 "
                        "format and converts them to the other format.\n");

std::vector<std::string> inputFiles, outputFiles;
std::string inputPath, outputPath;
bool verbose;
bool printExceptions;
bool debug = true;
//============================================================================//
//                                 Exit Codes                                 //
const int NO_INPUT = 1;
const int BAD_ARG = 2;
//============================================================================//

#endif
