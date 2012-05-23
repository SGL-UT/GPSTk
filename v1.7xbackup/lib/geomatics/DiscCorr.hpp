#pragma ident "$Id$"

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

/**
 * @file DiscCorr.hpp
 * GPS phase discontinuity correction. Given a SatPass object
 * containing dual-frequency pseudorange and phase for an entire satellite pass,
 * and a configuration object (as defined herein), detect discontinuities in
 * the phase and, if possible, estimate their size.
 * Output is in the form of Rinex editing commands (see class RinexEditor).
 */

#ifndef GPSTK_DISCONTINUITY_CORRECTOR_INCLUDE
#define GPSTK_DISCONTINUITY_CORRECTOR_INCLUDE

#include "DayTime.hpp"
#include "GSatID.hpp"
#include "RinexObsHeader.hpp"
#include "SatPass.hpp"
#include "Exception.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

namespace gpstk {

   /** @addtogroup rinexutils */
   //@{

   /// class GDCconfiguration encapsulates the configuration for input to the
   /// GPSTK Discontinuity Corrector.
   class GDCconfiguration {
   public:
         /// constructor; this sets a full default set of parameters.
      GDCconfiguration(void) { initialize(); }
         // destructor
      ~GDCconfiguration(void) { CFG.clear(); CFGdescription.clear(); }

         /// Set a parameter in the configuration; the input string 'cmd'
         /// is of the form '[--DC]<id><s><value>' where the separator s is
         /// one of (:=,) and leading '-','--', or '--DC' are optional.
      void setParameter(std::string cmd) throw(gpstk::Exception);

         /// Set a parameter in the configuration using the label and the value,
         /// for booleans use (T,F)=(non-zero,zero).
      void setParameter(std::string label, double value) throw(gpstk::Exception);

         /// Get the parameter in the configuration corresponding to label
      double getParameter(std::string label) throw() { return CFG[label]; }

         /// Tell GDCconfiguration to which stream to send debugging output.
      void setDebugStream(std::ostream& os) { p_oflog = &os; }

         /// Print help page, including descriptions and current values of all
         /// the parameters, to the ostream. If 'advanced' is true, also print
         /// advanced parameters.
      void DisplayParameterUsage(std::ostream& os, bool advanced=false)
         throw(gpstk::Exception);

         /// Return version string
      std::string Version() throw() { return GDCVersion; }

   protected:

         /// map containing configuration labels and their values
      std::map <std::string,double> CFG;

         /// map containing configuration labels and their descriptions
      std::map <std::string,std::string> CFGdescription;

         /// Stream on which to write debug output.
      std::ostream *p_oflog;

      void initialize(void);

      static std::string GDCVersion;

   }; // end class GDCconfiguration

   /// class GDCreturn encapsulates the information in the 'message' returned by
   /// the GPSTK Discontinuity Corrector. Create it using the string created by
   /// a call to DiscontinuityCorrector(SP,config,EditCmds,retMsg), then use it to
   /// access specific information about the results of the GDC.
   class GDCreturn {
   public:
         /// constructor; this parses the string
      explicit GDCreturn(std::string msg) {
         passN = -1;
         nGFslips = nWLslips = 0;
         nGFslipGross = nWLslipGross = 0;
         nGFslipSmall = nWLslipSmall = 0;
         WLsig = GFsig = 0.0;

         std::string::size_type pos;
         std::string word,line;
         std::vector<std::string> lines,words;
         if(msg.empty()) return;
         // split into lines
         while(1) {
            pos = msg.find_first_not_of("\n");
            if(pos > 0) msg.erase(0,pos);
            if(msg.empty()) break;
            pos = msg.find_first_of("\n");
            if(pos > 0) {
               word = msg.substr(0,pos);
               msg.erase(0,pos);
            }
            else {
               word = msg;
               msg.clear();
            }
            lines.push_back(word);
         }

         for(int i=0; i<lines.size(); i++) {
            line = lines[i];
            if(line.empty()) continue;
            // split line into words
            words.clear();
            while(1) {
               pos = line.find_first_not_of(" \t\n");
               if(pos != 0 && pos != std::string::npos) line.erase(0,pos);
               if(line.empty()) break;
               pos = line.find_first_of(" \t\n");
               if(pos == std::string::npos) word = line;
               else word = line.substr(0,pos);
               if(!word.empty()) {
                  words.push_back(word);
                  line.erase(0,word.length()+1);
               }
            }

            //std::cout << "Line " << i << ":";
            //for(int j=0; j<words.size(); j++) std::cout << " /" << words[j] << "/";
            //std::cout << std::endl;

            line = lines[i];
            //std::cout << line << std::endl;
            if(line.find("insufficient data",0) != std::string::npos)
               passN = strtol(words[1].c_str(),0,10);
            if(line.find("list of Segments",0) != std::string::npos)
               passN = strtol(words[1].c_str(),0,10);
            if(line.find("WL slip gross",0) != std::string::npos)
               nWLslipGross = strtol(words[3].c_str(),0,10);
            if(line.find("WL slip small",0) != std::string::npos)
               nWLslipSmall = strtol(words[3].c_str(),0,10);
            if(line.find("GF slip gross",0) != std::string::npos)
               nGFslipGross = strtol(words[3].c_str(),0,10);
            if(line.find("GF slip small",0) != std::string::npos)
               nGFslipSmall = strtol(words[3].c_str(),0,10);
            if(line.find("sigma GF variation",0) != std::string::npos)
               GFsig = strtod(words[3].c_str(),0);
            if(line.find("WL sigma in cycles",0) != std::string::npos)
               WLsig = strtod(words[3].c_str(),0);
         }
         nWLslips = nWLslipGross + nWLslipSmall;
         nGFslips = nGFslipGross + nGFslipSmall;
      }  // end constructor/parser

      int passN,nGFslips,nWLslips,nGFslipGross,nGFslipSmall,nWLslipGross,nWLslipSmall;
      double WLsig,GFsig;
   }; // end class GDCreturn

   /** GPSTK Discontinuity Corrector. Find, and fix if possible, discontinuities
   * in the GPS carrier phase data, given dual-frequency pseudorange and phase
   * data for an entire satellite pass. Input is the SatPass object holding the
   * data, and a GDCconfiguration object giving the parameter values for
   * the corrector. Output is in the form of a list of strings - editing commands
   * - that can be parsed and applied using the GPSTK Rinex Editor (see Prgm
   * EditRinex and the RinexEditor class). Also, the L1 and L2 arrays in the input
   * SatPass are corrected. The routine will mark bad points in the input data
   * using the SatPass flag.
   * @param SP       SatPass object containing the input data.
   * @param config   GDCconfiguration object.
   * @param EditCmds vector<string> (output) containing RinexEditor commands.
   * @param retMsg   string summary of results: see output 'GDC' and parseGDCReturn()
   * @return 0 for success, otherwise return an Error code;
   * codes are defined as follows.
   * const int BadInput = -5      input data does not have the required obs types
   * const int NoData = -4        insufficient input data, or all data is bad
   * const int FatalProblem = -3  DT is not set, or memory problem
   * const int Singularity = -1   polynomial fit fails
   * const int ReturnOK = 0       normal return
   */
   int DiscontinuityCorrector(SatPass& SP,
                              GDCconfiguration& config,
                              std::vector<std::string>& EditCmds,
                              std::string& retMsg)
      throw(Exception);

   //@}

}  // end namespace gpstk

//------------------------------------------------------------------------------------
#endif
