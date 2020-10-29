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

/// @file DiscCorr.hpp
/// GPS phase discontinuity correction. Given a SatPass object
/// containing dual-frequency pseudorange and phase for an entire satellite pass,
/// and a configuration object (as defined herein), detect discontinuities in
/// the phase and, if possible, estimate their size.
/// Output is in the form of Rinex editing commands (see class RinexEditor).

#ifndef GPSTK_DISCONTINUITY_CORRECTOR_INCLUDE
#define GPSTK_DISCONTINUITY_CORRECTOR_INCLUDE

#include "Epoch.hpp"
#include "RinexSatID.hpp"
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
         /// @throw Exception
      void setParameter(std::string cmd);

         /// Set a parameter in the configuration using the label and the value,
         /// for booleans use (T,F)=(non-zero,zero).
         /// @throw Exception
      void setParameter(std::string label, double value);

         /// Get the parameter in the configuration corresponding to label
      double getParameter(std::string label) throw()
      {
         if(CFG.find(label) == CFG.end()) return 0.0;    // TD throw?
         return CFG[label];
      }

         /// Get the description of a parameter
      std::string getDescription(std::string label) throw()
      {
         if(CFGdescription.find(label) == CFGdescription.end())
            return std::string("Invalid label");
         return CFGdescription[label];
      }

         /// Tell GDCconfiguration to which stream to send debugging output.
      void setDebugStream(std::ostream& os) { p_oflog = &os; }

         /// Print help page, including descriptions and current values of all
         /// the parameters, to the ostream. If 'advanced' is true, also print
         /// advanced parameters.
         /// @throw Exception
      void DisplayParameterUsage(std::ostream& os, bool advanced=false);

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

      static const std::string GDCVersion;

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
         sat = "";
         GLOn = -99;

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

         ptsdeleted = ptsgood = 0;
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
            if(line.find("WL sigma in cycles",0) != std::string::npos)
               passN = strtol(words[1].c_str(),0,10);
            else if(line.find("insufficient data",0) != std::string::npos)
               passN = strtol(words[1].c_str(),0,10);
            else if(line.find("list of Segments",0) != std::string::npos)
               passN = strtol(words[1].c_str(),0,10);
            if(line.find("bias(wl)",0) != std::string::npos) {
               sat = words[2];
               word = words[5];
               pos = word.find_first_of("/");
               if(pos > 0) word = words[5].substr(0,pos);
               ptsgood += strtol(word.c_str(),0,10);
            }
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
            if(line.find("points deleted",0) != std::string::npos)
               ptsdeleted += strtol(words[3].c_str(),0,10);
            if(line.find("GLONASS frequency channel",0) != std::string::npos)
               GLOn = strtod(words[3].c_str(),0);
         }

         nWLslips = nWLslipGross + nWLslipSmall;
         nGFslips = nGFslipGross + nGFslipSmall;

      }  // end constructor/parser

      int passN,GLOn;
      int nGFslips,nWLslips,nGFslipGross,nGFslipSmall,nWLslipGross,nWLslipSmall;
      int ptsdeleted,ptsgood;
      double WLsig,GFsig;
      std::string sat;

   }; // end class GDCreturn

   /// GPSTK Discontinuity Corrector. Find, and fix if possible, discontinuities
   /// in the GPS or GLONASS carrier phase data, given dual-frequency pseudorange and
   /// phase data for an entire satellite pass.
   /// Input is the SatPass object holding the data, and a GDCconfiguration object
   /// giving the parameter values for the corrector.
   /// Output is in the form of a list of strings - editing commands - that can be
   /// parsed and applied using the GPSTK Rinex Editor (see Prgm EditRinex and the
   /// RinexEditor class). Also, the L1 and L2 arrays in the input SatPass are
   /// corrected. The routine will mark bad points in the input data using
   /// the SatPass flag.
   /// Glonass satellites require a frequency channel integer; the caller may pass
   /// this in, or let the GDC compute it from the data - if it fails it returns -6.
   ///
   /// @param SP       SatPass object containing the input data.
   /// @param config   GDCconfiguration object.
   /// @param EditCmds vector<string> (output) containing RinexEditor commands.
   /// @param retMsg   string summary of results: see 'GDC' in output, class GDCreturn
   ///      if retMsg is not empty on call, replace 'GDC' with retMsg.
   /// @param GLOn     GLONASS frequency channel (-7<=n<7), -99 means UNKNOWN
   /// @return 0 for success, otherwise return an Error code;
   ///
   /// codes are defined as follows.
   /// const int GLOfail = -6       failed to find the Glonass frequency channel
   /// const int BadInput = -5      input data does not have the required obs types
   /// const int NoData = -4        insufficient input data, or all data is bad
   /// const int FatalProblem = -3  DT is not set, or memory problem
   /// const int Singularity = -1   polynomial fit fails
   /// const int ReturnOK = 0       normal return
   /// @throw Exception
   int DiscontinuityCorrector(SatPass& SP,
                              GDCconfiguration& config,
                              std::vector<std::string>& EditCmds,
                              std::string& retMsg,
                              int GLOn=-99);

   //@}

}  // end namespace gpstk

//------------------------------------------------------------------------------------
#endif
