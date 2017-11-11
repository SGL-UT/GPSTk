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

/**
 * @file StringUtils.cpp
 * Implementation of GPSTK string utility functions.
 */

#include "StringUtils.hpp"

/* The DEBUG_COL macro is used to help debug issues with column
 * alignment and positioning.  If enabled, it will print, to stderr, a
 * string of characters representing what's being appended to the
 * stream in each column of text.  To use it, rename the first macro
 * with the cerr statement to DEBUG_COL and the second macro to
 * DEBUG_COL_NO.  To disable, do the opposite. */
#define DEBUG_COL_NO(LABEL,ADDED)                                       \
   if (ADDED > 0)                                                       \
      std::cerr << std::string(ADDED,LABEL) << std::flush;

#define DEBUG_COL(LABEL,ADDED)

namespace gpstk
{
   namespace StringUtils
   {
      void hexDumpData(const std::string& data, std::ostream& s,
                       const HexDumpDataConfig& cfg)
      {
            // save the state in a stream object that doesn't need an
            // externally defined buffer or any of that crap.
         std::ostringstream oldState;
         oldState.copyfmt(s);
         std::string ascii="";
         int col = 0;
         int datasize=data.size();
         unsigned linesize;

         if (cfg.groupBy && ((cfg.bytesPerLine % cfg.groupBy) != 0))
         {
            s << "hexDumpData: cfg.bytesPerLine % cfg.groupBy != 0"
              << std::endl;
            s.copyfmt(oldState);
            return;
         }
         if (cfg.group2By && ((cfg.bytesPerLine % cfg.group2By) != 0))
         {
            s << "hexDumpData: cfg.bytesPerLine % cfg.group2By != 0"
              << std::endl;
            s.copyfmt(oldState);
            return;
         }
         if (cfg.groupBy && ((cfg.group2By % cfg.groupBy) != 0))
         {
            s << "hexDumpData: cfg.group2By % cfg.groupBy != 0"
              << std::endl;
            s.copyfmt(oldState);
            return;
         }

            // line format:
            // <prefix><index><indexSep><group1byte1>...<group1byte[groupBy]><groupSep>...<group[group2By]byte1>...<group[group2By]byte[groupBy]><group2Sep>....<byte[bytesPerLine]><dataEndSep/dataFinal><preText><separator><text><separator>\n
            // make sure our default formatting options are set
         s << std::hex << std::internal << std::noshowbase << std::setw(0);

         unsigned bytesOnLastLine = datasize % cfg.bytesPerLine;
         if (bytesOnLastLine == 0)
            bytesOnLastLine = cfg.bytesPerLine;
         linesize = std::max(
            cfg.computeLineSize(cfg.bytesPerLine, false),
            cfg.computeLineSize(bytesOnLastLine, true));

         for (int i=0; i<datasize; i++)
         {
            if (i%cfg.bytesPerLine==0)
            {
                  // add the prefix text at the beginning of each line
               s << cfg.prefix;
               col = cfg.prefix.length();
               DEBUG_COL('P',cfg.prefix.length());
               if (cfg.showIndex)
               {
                     // print the data index in either hex or decimal,
                     // with or without a radix indicator all
                     // according to cfg
                  std::string indexBase(cfg.baseIndex());
                  s << std::setfill('0')
                    << (cfg.upperHex ? std::uppercase : std::nouppercase)
                    << cfg.baseIndex()
                    << (cfg.hexIndex ? std::hex : std::dec)
                    << std::setw(cfg.idxDigits)
                    << i << cfg.indexSep
                    << std::noshowbase << std::dec << std::nouppercase;
                  col += indexBase.length() + cfg.idxDigits +
                     cfg.indexSep.length();
                  DEBUG_COL('I',indexBase.length() + cfg.idxDigits + cfg.indexSep.length());
               }
            }
            unsigned char c=data[i];
               // construct the ASCII representation using only
               // printable characters
            if (isprint(c))
               ascii += c;
            else
               ascii += '.';
            if (cfg.groupBy && ((i % cfg.groupBy) == 0) && cfg.showBaseData)
            {
                  // print the hex radix indicator if requested
               s << cfg.baseData();
               col += 2;
               DEBUG_COL('R',2);
            }
               // print the byte value in hex
            s << (cfg.upperHex ? std::uppercase : std::nouppercase)
              << std::hex << std::setw(2) << std::setfill('0') << (int)c
              << std::dec << std::nouppercase << std::noshowbase;
            col += 2;
            DEBUG_COL('B',2);
            if (((i % cfg.bytesPerLine) == (cfg.bytesPerLine-1)) ||
                (i == (datasize-1)))
            {
               if (i == (datasize-1))
               {
                     // this is the very last byte of data, print the
                     // final terminator text dataFinal
                  s << cfg.dataFinal;
                  col += cfg.dataFinal.length();
                  DEBUG_COL('F',cfg.dataFinal.length());
               }
               else if ((i % cfg.bytesPerLine) == (cfg.bytesPerLine-1))
               {
                     // this is the last byte on the line of text,
                     // print the end-of-line terminator text
                     // dataEndSep
                  s << cfg.dataEndSep;
                  col += cfg.dataEndSep.length();
                  DEBUG_COL('E',cfg.dataEndSep.length());
               }
               if (cfg.showText)
               {
                     // print the ASCII representation of the data
                  int extra = linesize-col;
                  std::string space(extra, ' ');
                  s << space << cfg.preText;
                  DEBUG_COL('s', extra);
                  DEBUG_COL('T', cfg.preText.length());
                  s << ascii;
                  DEBUG_COL('A', ascii.length());
                  s << cfg.postText;
                  DEBUG_COL('t', cfg.postText.length());
               }
               s << std::endl;
               DEBUG_COL('\n', 1);
               ascii.erase();
            }
            else if (cfg.group2By && ((i % cfg.group2By) == (cfg.group2By-1)))
            {
                  // level 2 group separator
               s << cfg.group2Sep;
               col += cfg.group2Sep.length();
               DEBUG_COL('O',cfg.group2Sep.length());
            }
            else if (cfg.groupBy && ((i % cfg.groupBy) == (cfg.groupBy-1)))
            {
                  // level 1 group separator
               s << cfg.groupSep;
               col += cfg.groupSep.length();
               DEBUG_COL('G',cfg.groupSep.length());
            }
         }
         s.copyfmt(oldState);
      }
   } // namespace StringUtils
} // namespace gpstk
