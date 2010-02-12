#pragma ident "$Id$"

/**
 * @file AntexHeader.cpp
 * Encapsulate header of Rinex observation file, including I/O
 */

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

#include "StringUtils.hpp"
#include "AntexHeader.hpp"
#include "AntexStream.hpp"

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{
   const string AntexHeader::versionString =       "ANTEX VERSION / SYST";
   const string AntexHeader::pcvTypeString =       "PCV TYPE / REFANT";
   const string AntexHeader::headerCommentString = "COMMENT";
   const string AntexHeader::endOfHeaderString =   "END OF HEADER";

   void AntexHeader::reallyPutRecord(FFStream& ffs) const 
      throw(std::exception, FFStreamError, StringException)
   {
      AntexStream& strm = dynamic_cast<AntexStream&>(ffs);
      
      strm.header = *this;
      
      unsigned long allValid;
      if (version == 1.3)        allValid = allValid13;
      else {
         FFStreamError err("Unknown Antex version: " + asString(version,2));
         err.addText("Make sure to set the version correctly.");
         GPSTK_THROW(err);
      }
      
      if ((valid & allValid) != allValid) {
         FFStreamError err("Incomplete or invalid header.");
         err.addText("Set all header valid bits for all of the available data.");
         GPSTK_THROW(err);
      }
      
      try {
         WriteHeaderRecords(strm);
      }
      catch(FFStreamError& e) { GPSTK_RETHROW(e); }
      catch(StringException& e) { GPSTK_RETHROW(e); }

   }  // end AntexHeader::reallyPutRecord
      

      // this function writes all valid header records
   void AntexHeader::WriteHeaderRecords(FFStream& ffs) const
      throw(FFStreamError, StringException)
   {
      AntexStream& strm = dynamic_cast<AntexStream&>(ffs);
      string line;

      if(valid & versionValid|systemValid) {
         line  = rightJustify(asString(version,1), 8);
         line += string(12,' ');
         line += system;
         line = leftJustify(line, 60);
         line += versionString;
         strm << leftJustify(line,80) << endl;
         strm.lineNumber++;
      }
      if(valid & pcvTypeValid) {
         line  = pcvType;
         line += string(19,' ');
         line += leftJustify(refAntType, 20);
         line += leftJustify(refAntSerNum, 20);
         line += pcvTypeString;
         strm << leftJustify(line,80) << endl;
         strm.lineNumber++;
      }
      if(valid & commentValid) {
         vector<string>::const_iterator itr = commentList.begin();
         while(itr != commentList.end()) {
            line  = leftJustify((*itr), 60);
            line += headerCommentString;
            strm << leftJustify(line,80) << endl;
            strm.lineNumber++;
            itr++;
         }
      }
      if(valid & endValid) {
         line  = string(60, ' ');
         line += endOfHeaderString;
         strm << leftJustify(line,80) << endl;
         strm.lineNumber++;               
      }   
   }   // end AntexHeader::WriteHeaderRecords()

      // this function parses a single header record
   void AntexHeader::ParseHeaderRecord(string& line)
      throw(FFStreamError)
   {
      string label(line, 60, 20);
         
      if(label == versionString) {
         version = asDouble(line.substr(0,8));
         system = line[20];
         if(system != ' ' && system != 'G' &&
            system != 'R' && system != 'E' && system != 'M')
         {
            FFStreamError e("Satellite system is invalid: " + system);
            GPSTK_THROW(e);
         }
         valid |= versionValid;
         valid |= systemValid;
      }
      else if(label == pcvTypeString) {
         pcvType = line[0];
         if(pcvType != 'A' && pcvType != 'R') {
            FFStreamError e("PCV type is invalid: " + pcvType);
            GPSTK_THROW(e);
         }
         refAntType = line.substr(20,20);
         refAntSerNum = line.substr(40,20);
         valid |= pcvTypeValid;
      }
      else if(label == headerCommentString) {
         commentList.push_back(stripTrailing(line.substr(0,60)));
         valid |= commentValid;
      }
      else if(label == endOfHeaderString) {
         valid |= endValid;
      }
      else {
         FFStreamError e("Unidentified label: " + label);
         GPSTK_THROW(e);
      }
   }   // end of AntexHeader::ParseHeaderRecord(string& line)


      // This function parses the entire header from the given stream
   void AntexHeader::reallyGetRecord(FFStream& ffs)
      throw(std::exception, FFStreamError, StringException)
   {
      AntexStream& strm = dynamic_cast<AntexStream&>(ffs);
      
         // if already read, just return
      if (strm.headerRead == true)
         return;

         // since we're reading a new header, we need to reinitialize
         // all our list structures.  all the other objects should be ok.
         // this also applies if we threw an exception the first time we read
         // the header and are now re-reading it. some of these could be full
         // and we need to empty them.
      commentList.clear();
      version = 1.3;
      valid = 0;
      
      string line;
      while(!(valid & endValid)) {
         strm.formattedGetLine(line);
         stripTrailing(line);

         if (line.length()==0)
            continue;
         else if(line.length()<60 || line.length()>80) {
            FFStreamError e("Invalid line length");
            GPSTK_THROW(e);
         }

         try { ParseHeaderRecord(line); }
         catch(FFStreamError& e) { GPSTK_RETHROW(e); }
         
      }   // end while(not end of header)

      unsigned long allValid;
      if (version == 1.3)
         allValid = allValid13;
      else {
         FFStreamError e("Unknown or unsupported Antex version " + asString(version));
         GPSTK_THROW(e);
      }
            
      if((allValid & valid) != allValid) {
         FFStreamError e("Incomplete or invalid header");
         GPSTK_THROW(e);               
      }
            
         // If we get here, we should have reached the end of header line
      strm.header = *this;
      strm.headerRead = true;
            
   }  // end of reallyGetRecord()

   void AntexHeader::dump(ostream& s) const
   {
      s << "Dump of AntexHeader, version " << fixed << setprecision(1)
         << version << " system " << system << endl;
      s << "These are " << (pcvType == 'A' ? "absolute" : "relative")
         << " phase center offsets.\n";
      s << "Reference antenna: type " << refAntType
         << ", serial no. " << refAntSerNum << endl;

      for(int i=0; i<commentList.size(); i++) {
         if(i==0) s << "Comments:\n";
         s << "Comment " << setw(2) << i+1 << ": " << commentList[i] << endl;
      }
      s << "End of AntexHeader dump" << endl;
   }

} // namespace gpstk
