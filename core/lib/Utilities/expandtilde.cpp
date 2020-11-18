//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

// expandtilde.cpp Expand tilde (~) in filenames.

#include <iostream>
#include <fstream>
#include "expandtilde.hpp"
#include "StringUtils.hpp"

using namespace std;
using namespace gpstk;

void expand_filename(string& filename)
{
#ifndef _WIN32
   static char *chome = getenv("HOME");
   static string home = string(chome);

   // assume tilde occurs only once
   string::size_type pos = filename.find_first_of("~");
   if(pos == string::npos) return;
   string newname;
   if(pos > 0) newname = filename.substr(0,pos);
   filename = filename.substr(pos+1);
   StringUtils::stripLeading(filename,"/");
   StringUtils::stripTrailing(home,"/");
   newname += home + string("/") + filename;
   filename = newname;
#endif
}

void expand_filename(vector<string>& sarray)
{
   for(size_t i=0; i<sarray.size(); i++) expand_filename(sarray[i]);
}

void include_path(string path, string& file)
{
   if(!path.empty()) {
      StringUtils::stripTrailing(path,"/");
      StringUtils::stripTrailing(path,"\\");
      file = path + string("/") + file;
   }
}

void include_path(string path, vector<string>& sarray)
{
   if(!path.empty()) {
      StringUtils::stripTrailing(path,"/");
      StringUtils::stripTrailing(path,"\\");
      for(size_t i=0; i<sarray.size(); i++)
         sarray[i] = path + string("/") + sarray[i];
   }
}

// return false if file cannot be opened
bool expand_list_file(string& filename, vector<string>& values)
{
   string line,word;
   // DO NOT clear values, add to it

   // open list file
   ifstream infile;
   infile.open(filename.c_str());
   if(!infile.is_open()) return false;

   // read the list file
   while(1) {
      getline(infile,line);
      StringUtils::stripTrailing(line,'\r');
      StringUtils::stripLeading(line);
      while(!line.empty()) {
         word = StringUtils::stripFirstWord(line);
         if(word.substr(0,1) == "#") break;        // skip '#...' to end of line
         values.push_back(word);
      }
      if(infile.eof() || !infile.good()) break;
   }

   infile.close();

   return true;
}
