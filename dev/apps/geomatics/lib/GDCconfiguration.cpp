#pragma ident "$Id: $"

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
 * @file GDCconfiguration.cpp
 * class GDCconfiguration encapsulates the configuration for input to the
 * GPSTK Discontinuity Corrector.
 */

#include "GDCconfiguration.hpp"
#include "StringUtils.hpp"

using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// version number for the entire gpstk discontinuity corrector code,
// most convenient to keep it here as GDCpass inherits this
string GDCconfiguration::GDCVersion = string("5.0 9/01/2006");

//------------------------------------------------------------------------------------
// Set a parameter in the configuration; the input string 'cmd' is of the form
// '[--DC]<id><s><value>' : separator s is one of ':=,' and leading --DC is optional.
void GDCconfiguration::setParameter(string cmd) throw(Exception)
{
try {
   if(cmd.empty()) return;
      // remove leading --DC
   while(cmd[0] == '-') cmd.erase(0,1);
   if(cmd.substr(0,2) == "DC") cmd.erase(0,2);

   string label, value;
   string::size_type pos=cmd.find_first_of(",=:");
   if(pos == string::npos) {
      label = cmd;
   }
   else {
      label = cmd.substr(0,pos);
      value = cmd;
      value.erase(0,pos+1);
   }

   setParameter(label, StringUtils::asDouble(value));
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Set a parameter in the configuration using the label and the value,
// for booleans use (T,F)=(non-zero,zero).
void GDCconfiguration::setParameter(string label, double value) throw(Exception)
{
try {
   if(CFG.find(label) == CFG.end())
      ; // throw
   else {
      *(p_oflog) << "GDCconfiguration::setParameter sets "
         << label << " to " << value << endl;
      CFG[label] = value;
   }
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Print help page, including descriptions and current values of all
// the parameters, to the ostream.
void GDCconfiguration::DisplayParameterUsage(ostream& os, bool advanced)
   throw(Exception)
{
try {
   os << "\nGPSTk Discontinuity Corrector (GDC) v." << GDCVersion
      << " configuration, with current values"
      //<< "\n  [ pass setParameter() a string '<label><sep><value>';"
      //<< " <sep> is one of ,=: ]"
      << endl;

   map<string,double>::const_iterator it;
   for(it=CFG.begin(); it != CFG.end(); it++) {
      if(CFGdescription[it->first][0] == '*')      // advanced options
         continue;  
      ostringstream stst;
      stst << it->first                            // label
         << "=" << it->second;                     // value
      os << " " << StringUtils::leftJustify(stst.str(),18)
         << " : " << CFGdescription[it->first]     // description
         << endl;
   }
   if(advanced) {
   os << "   Advanced options:\n";
   for(it=CFG.begin(); it != CFG.end(); it++) {
      if(CFGdescription[it->first][0] != '*')      // ordinary options
         continue;  
      ostringstream stst;
      stst << it->first                            // label
         << "=" << it->second;                     // value
      os << " " << StringUtils::leftJustify(stst.str(),25)
         << " : " << CFGdescription[it->first].substr(2)  // description
         << endl;
   }
   }
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}


//------------------------------------------------------------------------------------
#define setcfg(a,b,c) { CFG[#a]=b; CFGdescription[#a]=c; }
// initialize with default values
void GDCconfiguration::initialize(void)
{
try {
   p_oflog = &cout;

   // use cfg(DT) NOT dt -  dt is part of SatPass...
   setcfg(DT, -1, "nominal timestep of data (seconds) [required - no default!]");
   setcfg(Debug, 0, "level of diagnostic output to log, from none(0) to extreme(7)");
   setcfg(MaxGap, 180, "maximum allowed time gap within a segment (seconds)");
   setcfg(MinPts, 13, "minimum number of good points in phase segment ()");
   setcfg(WLSigma, 1.5, "expected WL sigma (WL cycle) [NB = ~0.83*p-range noise(m)]");
   setcfg(GFVariation, 16,                    // about 300 5.4-cm wavelengths
      "expected maximum variation in GF phase in time DT (meters)");
   // output
   setcfg(OutputGPSTime, 0,
      "if 0: Y,M,D,H,M,S  else: W,SoW (GPS) in editing commands");
   setcfg(OutputDeletes, 1,
      "if non-zero, include delete commands in the output cmd list");

   // -------------------------------------------------------------------------
   // advanced options - ordinary user will most likely NOT change
   setcfg(RawBiasLimit, 100, "* change in raw R-Ph that triggers bias reset (m)");
   // WL editing
   setcfg(WLNSigmaDelete, 2, "* delete segments with sig(WL) > this * WLSigma ()");
   setcfg(WLWindowWidth, 10, "* sliding window width for WL slip detection (points)");
   setcfg(WLNWindows, 2.5,
      "* minimum segment size for WL small slip search (WLWindowWidth)");
   setcfg(WLobviousLimit, 3,
      "* minimum delta(WL) that produces an obvious slip (WLSigma)");
   setcfg(WLNSigmaStrip, 3.5, "* delete points with WL > this * computed sigma ()");
   setcfg(WLNptsOutlierStats, 200,
      "* maximum segment size to use robust outlier detection (pts)");
   setcfg(WLRobustWeightLimit, 0.35,
      "* minimum good weight in robust outlier detection (0<wt<=1)");
   // WL small slips
   setcfg(WLSlipEdge, 3,
      "* minimum separating WL slips and end of segment, else edit (pts)");
   setcfg(WLSlipSize, 0.67, "* minimum WL slip size (WL wavelengths)");
   setcfg(WLSlipExcess, 0.1,
      "* minimum amount WL slip must exceed noise (WL wavelengths)");
   setcfg(WLSlipSeparation, 1.2, "* minimum excess/noise ratio of WL slip ()");
   // GF small slips
   setcfg(GFSlipWidth, 5,
      "* minimum segment length for GF small slip detection (pts)");
   setcfg(GFSlipEdge, 3,
      "* minimum separating GF slips and end of segment, else edit (pts)");
   setcfg(GFobviousLimit, 1,
      "* minimum delta(GF) that produces an obvious slip (GFVariation)");
   setcfg(GFSlipOutlier, 5, "* minimum GF outlier magnitude/noise ratio ()");
   setcfg(GFSlipSize, 0.8, "* minimum GF slip size (5.4cm wavelengths)");
   setcfg(GFSlipStepToNoise, 2, "* maximum GF slip step/noise ratio ()");
   setcfg(GFSlipToStep, 3, "* minimum GF slip magnitude/step ratio ()");
   setcfg(GFSlipToNoise, 3, "* minimum GF slip magnitude/noise ratio ()");
   // GF fix
   setcfg(GFFixNpts, 15,
      "* maximum number of points on each side to fix GF slips ()");
   setcfg(GFFixDegree, 3, "* degree of polynomial used to fix GF slips ()");
   setcfg(GFFixMaxRMS, 100,
      "* limit on RMS fit residuals to fix GF slips, else delete (5.4cm)");

}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
