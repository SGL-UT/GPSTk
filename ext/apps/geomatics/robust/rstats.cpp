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

/// @file rstats.cpp
/// Read the data in one [or two] column(s) of a file, and output robust statistics,
/// two-sample statistics, a stem-and-leaf plot, a quantile-quantile plot,
/// and a robust polynomial fit. Options perform a variety of other analysis tasks.

//------------------------------------------------------------------------------------
// system includes
#include <ctime>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
// GPSTk
#include "Exception.hpp"
#include "StringUtils.hpp"
#include "Stats.hpp"
#include "StatsFilter.hpp"
//#include "FDiffFilter.hpp"
#include "singleton.hpp"
//#include "icd_200_constants.hpp"
// geomatics
#include "CommandLine.hpp"
#include "RobustStats.hpp"
#include "MostCommonValue.hpp"
#include "expandtilde.hpp"
#include "logstream.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
// NB Version below class GlobalData

//------------------------------------------------------------------------------------
// prototypes
int GetCommandLine(int argc, char **argv) throw(Exception);
int Prepare(void) throw(Exception);
int ReadAndCompute(void) throw(Exception);

int StemAndLeaf(void) throw(Exception);
int QuantilePlot(void) throw(Exception);
int FindBins(void) throw(Exception);
int ComputeSum(void) throw(Exception);
int FitPoly(void) throw(Exception);
int Sequential(void) throw(Exception);
int Discontinuity(void) throw(Exception);
int FDFilter(void) throw(Exception);
int WindFilter(void) throw(Exception);
//int FixFilter(void) throw(Exception);
int OutputStats(void) throw(Exception);
int Outliers(void) throw(Exception);

// utility for develop and debug
int DumpData(string msg=string("DUMP")) throw(Exception);

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// Class GlobalData encapsulates global static data.
class GlobalData : public Singleton<GlobalData> {
public:
   // Default and only constructor, sets defaults.
   GlobalData() throw() { SetDefaults(); }

   // prgm housekeeping
   static const std::string Version;   // see below
   std::string PrgmName;               // this program (must match Jamfile)
   std::string Title;                  // name, version and runtime
   std::ofstream oflog;                // output log file stream

   // command line input ----------------------------------------------------------
   std::string cmdlineErrors,cmdlineDump,cmdlineUsage;  // strings filled by parser
   std::vector<std::string> cmdlineUnrecognized;

   //std::string logfile;                // name of log file
   std::string inpath;                 // files and paths
   std::string inputfile;              // input file

   // input
   int col,xcol,wcol;
   double xbeg,xend,dmin,dmax;
   bool doxbeg,doxend,dodmin,dodmax;
   string begstr,endstr,minstr,maxstr; // use strings so no default shows
   double debias;                      // specify bias to remove
   string debstr;
   bool dodebias, debias0;             // set dodebias using CommandLine::count()
   // plots
   bool doStemLeaf;                    // stem and leaf plot
   bool doQplot;                       // quantile plot

   bool doBin;
   string binstr;                      // string for input of bin option(s)
   int whichbin;                       // which input option: 1 n, 2 w, or 3 n,w,f
   int nbin;                           // binning for histogram - number of bins
   double widbin,firstbin;             // width of bins, center of first bin

   // analysis
   bool doSum,doSumPlus,doFit;         // sum, find range, step, gaps
   string fitstr;                      // polynomial fit
   unsigned int nfit;                  // degree of fit
   vector<double> xevalfit;            // evaluate fit at each x
   bool doSeq;                         // sequential stats
   bool doDisc;                        // discontinuities
   string discstr;                     // 
   double ytol,xtol;                   // tolerances to discontinuities

   // filters
   bool doFDF,doFDF2,doWF,doXWF;       // first difference and window filters
   double fdfstep,fdfsig,fdfrat;       // step, sigma and ratio limit for FDF(2)
   string fdfstr,fdf2str;              // fdf and fdf2 filters
   string windstr,xwindstr;            // window filters
   int windwidth;                      // width of window filter
   double windstep, windratio;         // slip tol. and ratio limit for WindowFilter
   string fixfstr;                     // input for fixer filter
   int fixN;                           // width of fixer filter
   double fixlim,fixsig;               // step and sigma limits of fixer filter

   // output
   bool quiet;                         // suppress title, timing and other output
   bool brief;                         // 1-line output stats (conv,rob,2-samp,wted)
   bool bc,br,bw,b2,brw;               // 1-line output of only 1 stats
   string label;                       // label added to brief/fit/disc/seq
   bool nostats;                       // suppress output of stats
   bool doKS;                          // do Anderson-Darling
   bool doOuts;                        // outlier lists
   string outstr;                      // use string so no default shows
   double outscale;                    // outlier scaling for --outs
   int prec;                           // precision of floating output

   bool verbose,help;                  // help, etc
   int debug;
   std::string timefmt;                // output format
   // end command line input ------------------------------------------------------

   // data, x-data and weights
   std::vector<double> data,xdata,wdata,robwts;
   // stats
   Stats<double> cstats;
   WtdStats<double> wstats,robwtstats;
   TwoSampleStats<double> tsstats;
   // robust
   double median,mad,mest,Q1,Q3,KS;

   // results
   std::string msg;                    // msg for output
   std::string longfmt;                // times in error messages, etc.

private:
   void SetDefaults(void) throw()
   {
      PrgmName = std::string("rstats");

      // command line input -------------------------------------------
      // files and paths
      //logfile = std::string("");
      inputfile = std::string("");
      // input
      col = 1;
      xcol = wcol = -1;
      doxbeg = doxend = dodmin = dodmax = false;
      begstr = endstr = minstr = maxstr = string("");
      debstr = string("");
      dodebias = debias0 = false;
      // plots
      doStemLeaf = doQplot = false;
      doBin = false;
      binstr = string("");
      whichbin = 0;
      // analysis
      doSum = doSumPlus = doFit = false;
      fitstr = string("");
      nfit = 0;
      doSeq = false;
      discstr = string("");
      // filters
      {
         vector<double> xdata,data;
         vector<int> flags;
         FirstDiffFilter<double> fdf(xdata,data,flags);
         fdfstep = fdf.getLimit();
         fdfsig = 0.02; fdfrat = 2.0;             // TD need a default
         fdfstr = asString(fdfstep,0);
         fdf2str = asString(fdfstep,0) + string(",")
                          + asString(fdfsig,2) + string(",")
                          + asString(fdfrat,0);

         WindowFilter<double> wf(xdata,data,flags);
         windwidth = wf.getWidth();
         windstep = wf.getMinStep();
         windratio = wf.getMinRatio();
         xwindstr = windstr = asString(windwidth) + string(",")
                            + asString(windstep,1) + string(",")
                            + asString(windratio,0);

         fixfstr = "4,0.8,0.2";
         fixN = 4;
         fixlim = 0.8;
         fixsig = 0.2;
      }
      // output
      outstr = string("");
      quiet = nostats = doKS = doOuts = brief = bc = br = bw = b2 = brw = false;
      label = string("");
      prec = 3;
      // help
      debug = -1;
      help = verbose = false;
      timefmt = std::string("%4F %10.3g");
      //timefmt = std::string("%.6Q");         // 1.5/86400 = 1.7e-5

      // end command line input ---------------------------------------

      longfmt = std::string("%04F %10.3g %04Y/%02m/%02d %02H:%02M:%06.3f %P");
   }

}; // end class GlobalData

//------------------------------------------------------------------------------------
const string GlobalData::Version(string("3.0 9/9/18 rev"));

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
   string PrgmName;        // for catch
try {
   // begin counting time
   clock_t totaltime = clock();

   // get (create) the global data object (a singleton);
   // since this is the first instance, this will also set default values
   GlobalData& GD=GlobalData::Instance();
   PrgmName = GD.PrgmName;

   // Build title
   GD.Title = GD.PrgmName + ", Ver. " + GD.Version;
 
   // display title on screen -- see below after cmdline input

   // process : loop once -----------------------------------------------------
   int iret;
   for(bool go=true; go; go=false)  {

      // process the command line ------------------------------------
      iret = GetCommandLine(argc,argv);
      if(iret) break;

      // Title, debug output cmdline, define GD.msg, include_path(input file)
      iret = Prepare();
      if(iret) break;

      // read input and compute stats
      iret = ReadAndCompute();
      if(iret) break;

      //DumpData("INI");

      // plots
      if(GD.doStemLeaf) { iret = StemAndLeaf(); if(iret) break; }
      if(GD.doQplot) { iret = QuantilePlot(); if(iret) break; }
      // uses GD.cstats, sets nostats
      if(GD.doBin) { iret = FindBins(); if(iret) break; }

      // analysis
      if(GD.doSum || GD.doSumPlus) { iret = ComputeSum(); if(iret) break; }
      if(GD.doFit) { iret = FitPoly(); if(iret) break; }
      if(GD.doSeq) { iret = Sequential(); if(iret) break; }
      if(GD.doDisc) { iret = Discontinuity(); if(iret) break; }

      // filters - all set nostats
      if(GD.doFDF || GD.doFDF2) { iret = FDFilter(); if(iret) break; }
      if(GD.doWF || GD.doXWF) { iret = WindFilter(); if(iret) break; }
      //if(GD.doFixF) { iret = FixFilter(); if(iret) break; }

      // output stats
      if(!GD.nostats) { iret = OutputStats(); if(iret) break; }
      if(GD.doOuts) { iret = Outliers(); if(iret) break; }

   }  // end loop once

   // error condition ---------------------------------------------------------
   // return codes: 0 ok
   //               1 help
   //               2 cmd line errors
   //               3 requested validation
   //               4 invalid input
   //               5 open fail
   //               6 requested dump
   //               7 other...
   //              -3 cmd line definition invalid (CommandLine)
   //cout << "Return code is " << iret << endl;
   if(iret != 0) {
      if(iret != 1) {
         string tmsg;
         tmsg = GD.PrgmName + string(" is terminating with code ")
                           + StringUtils::asString(iret);
         cout << tmsg << endl;
      }

      if(iret < 5) cout << "# " << GD.Title << endl;

      if(iret == 1) { cout << GD.cmdlineUsage; }
      else if(iret == 2) { cout << GD.cmdlineErrors; }
      else if(iret == 3) { cout << "The user requested input validation."; }
      else if(iret == 4) { cout << "The input is invalid."; }
      else if(iret == 5) { cout << "There is not enough data."; }
      else if(iret == 7) { cout << "The input file could not be opened."; }
      else if(iret == -3) { // cmd line definition invalid
         cout << "The command line definition is invalid.\n" << GD.cmdlineErrors;
      }
      else                 // fix this
         cout << "temp - Some other return code..." << iret;
      cout << endl;
   }

   // compute and print run time ----------------------------------------------
   if(iret != 1 && !GD.quiet) {
      totaltime = clock()-totaltime;
      ostringstream oss;
      cout << "# " << PrgmName << " timing: " << fixed << setprecision(3)
         << double(totaltime)/double(CLOCKS_PER_SEC)
         << " seconds.\n";
   }

   if(iret == 0) return 0; else return -1;
}
catch(Exception& e) {
   cerr << PrgmName << " caught Exception:\n" << e.what() << endl;
}
catch (...) {
   cerr << "Unknown error in " << PrgmName << ".  Abort." << endl;
}
   return -1;
}   // end main()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int GetCommandLine(int argc, char **argv) throw(Exception)
{
try {
   int i;
   GlobalData& GD=GlobalData::Instance();

   // create list of command line options, and fill it
   // put required options first - they will get listed first anyway
   CommandLine opts;

   // build the options list == syntax page
   string PrgmDesc =
      "Program " + GD.PrgmName + " reads one or more columns of numbers in an "
      "ascii input file and\n computes standard and robust statistics on the data, "
      "with options to perform\n a variety of other analysis tasks as well. Output "
      "is to stdout or file rstats.out.\n "
      "Input is on the command line, and/or of the same format in a file\n "
      "(see --file below); lines in the file that begin with '#' are ignored."
      "\n Options are shown below, followed by a description, with default in ();\n "
      "however {*} means option must be present to activate - defaults suggested.";

   string pad("                   ");
   // opts.Add(char, opt, arg, repeat?, required?, &target, pre-descript, descript.);
   bool req(false);
   string dummy("");         // dummy for --file
   opts.Add('f', "file", "name", true, req, &dummy, "\n# File I/O:",
            "Name of file with options [#-EOL = comment] [-f]");
   //opts.Add(0, "log", "name", false, req, &GD.logfile, "",
   //         "Name of output log file");
   //opts.Add(0, "logpath", "path", false, req, &GD.logpath, "",
   //         "Path for output log file");
   opts.Add('i', "input", "name", true, req, &GD.inputfile, "",
            "Name of input file (-i|--input optional) [-i]");
   opts.Add(0, "path", "dir", false, req, &GD.inpath, "",
            "Path for input file");
   // input
   opts.Add('y', "col", "c", false, req, &GD.col, "\n# input:",
            "read data in this column [-y|-c]");
   opts.Add('x', "xcol", "c", false, req, &GD.xcol, "",
            "also read 'x' data in this column [-x]");
   opts.Add('w', "wcol", "c", false, req, &GD.wcol, "",
            "weight with fabs() of data in this column [-w]");

   // modify input
   opts.Add(0, "beg", "xb", false, req, &GD.begstr, "\n# modify input:",
            "include only input that satisfies x > xb");
   opts.Add(0, "end", "xe", false, req, &GD.endstr, "",
            "include only input that satisfies x < xe");
   opts.Add(0, "min", "dmin", false, req, &GD.minstr, "",
            "include only input that satisfies data > dmin");
   opts.Add(0, "max", "dmax", false, req, &GD.maxstr, "",
            "include only input that satisfies data < dmax");
   opts.Add(0, "debias", "d", false, req, &GD.debstr, "",
            "remove bias d from data to compute stats");
   opts.Add(0, "debias0", "", false, req, &GD.debias0, "",
            "remove bias = (1st data pt) from data to compute stats");
   // plots
   opts.Add(0, "plot", "", false, req, &GD.doStemLeaf, "\n# plots:",
            "generate a stem-and-leaf plot from the data");
   opts.Add(0, "qplot", "", false, req, &GD.doQplot, "",
            "generate data for data vs. quantile plot [-> rstats.out]");
   opts.Add(0, "bin", "", false, req, &GD.binstr, "",
            "histogram: define bins and count data [ignore x, set nostats]\n"
            +pad+"  arg = <n> (int) compute approximately n bins\n"
            +pad+"  arg = <w> (float) compute bins of width w\n"
            +pad+"  arg = <n,w,cf> set bin number,width,center-of-first\n"
            +pad+"  (hint: pipe into plot -x 2 -y 3 --hist 0 -g 640x480)");
   // analysis
   opts.Add(0, "sum", "", false, req, &GD.doSum, "\n# analysis:",
            "compute sum of data [ignore x]");
   opts.Add(0, "sum+", "", false, req, &GD.doSumPlus, "",
            "compute sum, range, gaps and estimated stepsize [ignore x]");
   opts.Add(0, "fit", "f[,x]", false, req, &GD.fitstr, "",
            "fit a robust polynomial of degree f to data(xdata) [-> rstats.out]\n"
            +pad+"  if 1 or more <,x> are present, also evaluate fit at x(s)");
   opts.Add(0, "seq", "", false, req, &GD.doSeq, "",
            "output data, in input order, with sequential stats");
   opts.Add(0, "disc", "y[,x]", false, req, &GD.discstr, "",
            "output data, first differences, and discontinuities with stats\n"
            +pad+"  per segment; use y,x for data,xdata limits");
   // filters
   opts.Add(0, "fdf", "x", false, req, &GD.fdfstr, "\n# stats filters {*}:",
            "first-difference filter slip limit x");
   opts.Add(0, "fdf2", "x,s,r", false, req, &GD.fdf2str, "",
            "first-diff(2) filter, limits: slip x, sig s, ratio r");
   opts.Add(0, "wind", "n[,s,r]", false, req, &GD.windstr, "",
            "window filter width n, limits: slip s, ratio r");
   opts.Add(0, "xwind", "n[,s,r]", false, req, &GD.xwindstr, "",
            "window filter (--wind) with 2-sample stats");
   //opts.Add(0, "fix", "n[,x,s]", false, req, &GD.fixfstr, "",
   //         "SD fix filter width n, limits: slip x sigma s");
   // output
   opts.Add(0, "nostats", "", false, req, &GD.nostats, "\n# output:",
            "supress total stats output (for analyses)");
   opts.Add('q', "quiet", "", false, req, &GD.quiet, "",
            "supress title, timing and other non-essential output [-q]");
   opts.Add(0, "KS", "", false, req, &GD.doKS, "",
            "also output the Anderson-Darling statistic, a KS-test,\n"
            +pad+"  where AD > 0.752 implies non-normal");
   opts.Add(0, "outs", "s", false, req, &GD.outstr, "",
            "explicitly list all data outside s*(outlier limits)");
   opts.Add('b', "brief", "", false, req, &GD.brief, "",
            "single-line quiet stats output (conv, rob, wtd, r-wtd, 2-samp) [-b]\n"
            +pad+"  [or -bc -br -bw -brw -b2 for single quiet brief output]");
   opts.Add('l', "label", "L", false, req, &GD.label, "",
            "add label L to the (brief/disc/seq/fdf/wind/fft) outputs [-l]");
   opts.Add('p', "prec", "p", false, req, &GD.prec, "\n# format and help:",
            "specify precision of all float outputs [-p]");
   // help,verbose,debug handled by CommandLine

   // add options that are ignored (true if it has an arg)
   //opts.Add_ignore("--PRSoutput",true);

   // deprecated args
   opts.Add_deprecated("-c","-y");

   // --------------------------------------------------------------------------
   // declare it and parse it; write all errors to string GD.cmdlineErrors
   int iret = opts.ProcessCommandLine(argc,argv,PrgmDesc,
                         GD.cmdlineUsage,GD.cmdlineErrors,GD.cmdlineUnrecognized);
   if(iret == -2) return iret;      // bad alloc
   if(iret == -3) return iret;      // cmd line definition invalid

   // --------------------------------------------------------------------------
   // do extra parsing -- append errors to GD.cmdlineErrors
   vector<string> fields;
   ostringstream oss;

   // unrecognized arguments - few exceptions or an error
   if(GD.cmdlineUnrecognized.size() > 0) {
      vector<string> unrecogs;
      // backward compatibility...
      for(i=0; i<GD.cmdlineUnrecognized.size(); i++) {
         string str(GD.cmdlineUnrecognized[i]);
         if(str == "-bc")      GD.bc = true;
         else if(str == "-br") GD.br = true;
         else if(str == "-bw") GD.bw = true;
         else if(str == "-brw") GD.brw = true;
         else if(str == "-b2") GD.b2 = true;
         else {
            if(!GD.inputfile.empty()) unrecogs.push_back(str);
            else GD.inputfile = str;
         }
      }
      if(unrecogs.size()) {
         oss << " Error - unrecognized arguments\n";
         for(i=0; i<unrecogs.size(); i++)
            oss << unrecogs[i] << "\n";
         oss << " End of unrecognized arguments\n";
      }
   }

   // check inconsistent cmdline input
   if(GD.bw && GD.wcol == -1) {
      oss << " Warning - brief(w) but no --wcol - ignore bw output";
      GD.bw = false;
   }
   if(GD.b2 && GD.xcol == -1) {
      oss << " Warning - brief(2) but no --xcol - ignore b2 output";
      GD.b2 = false;
   }

   if(!GD.begstr.empty()) { GD.xbeg = asDouble(GD.begstr); GD.doxbeg = true; }
   if(!GD.endstr.empty()) { GD.xend = asDouble(GD.endstr); GD.doxend = true; }
   if(!GD.minstr.empty()) { GD.dmin = asDouble(GD.minstr); GD.dodmin = true; }
   if(!GD.maxstr.empty()) { GD.dmax = asDouble(GD.maxstr); GD.dodmax = true; }
   if(!GD.debstr.empty()) { GD.debias = asDouble(GD.debstr); GD.dodebias = true; }
   if(!GD.outstr.empty()) { GD.outscale = asDouble(GD.outstr); GD.doOuts = true; }

   // set switches using CommandLine::count()
   GD.doFit = (opts.count("fit") > 0);
   GD.doFDF = (opts.count("fdf") > 0);
   GD.doFDF2 = (opts.count("fdf2") > 0);
   GD.doWF = (opts.count("wind") > 0);
   GD.doXWF = (opts.count("xwind") > 0);
   //GD.doFixF = (opts.count("fix") > 0);

   // bin
   if(!GD.binstr.empty()) {
      GD.doBin = true;
      fields = split(GD.binstr,',');
      if(fields.size() == 1) {
         if(isDigitString(fields[0])) {
            GD.nbin = asInt(fields[0]);
            GD.whichbin = 1;
         }
         else if(isDecimalString(fields[0])) {
            GD.widbin = asDouble(fields[0]);
            GD.whichbin = 2;
         }
      }
      else if(fields.size() == 3) {
         GD.nbin = asInt(fields[0]);
         GD.widbin = asDouble(fields[1]);
         GD.firstbin = asDouble(fields[2]);
         GD.whichbin = 3;
      }
      else {
         oss << " Error - invalid argument to --bin " << GD.binstr << "\n";
         GD.doBin = false;
      }
   }

   // fit
   if(GD.doFit) {
      fields = split(GD.fitstr,',');
      GD.nfit = asInt(fields[0]);
      if(fields.size() > 1) {
         for(i=1; i<fields.size(); i++)
            GD.xevalfit.push_back(asDouble(fields[i]));
      }
   }

   // discontinuities
   if(!GD.discstr.empty()) {
      GD.doDisc = true;
      fields = split(GD.discstr,',');
      if(fields.size() == 1) {
         GD.ytol = asDouble(fields[0]);
         GD.xtol = -1.0;
      }
      else if(fields.size() == 2) {
         GD.ytol = asDouble(fields[0]);
         GD.xtol = asDouble(fields[1]);
      }
      else {
         oss << " Error - invalid argument to --disc " << GD.discstr << "\n";
         GD.doDisc = false;
      }
   }

   // fdf filter
   if(GD.doFDF) {
      fields = split(GD.fdfstr,',');
      if(fields.size() == 1) {
         GD.fdfstep = asDouble(fields[0]);
      }
      else {
         oss << " Error - invalid argument to --fdf " << GD.fdfstr << "\n";
         GD.doFDF = false;
      }
   }

   // fdf2 filter
   if(GD.doFDF2) {
      fields = split(GD.fdf2str,',');
      if(fields.size() == 3) {
         GD.fdfstep = asDouble(fields[0]);
         GD.fdfsig = asDouble(fields[1]);
         GD.fdfrat = asDouble(fields[2]);
      }
      else {
         oss << " Error - invalid argument to --fdf2 " << GD.fdf2str << "\n";
         GD.doFDF2 = false;
      }
   }

   //// fix filter
   //if(GD.doFixF) {
   //   fields = split(GD.fixfstr,',');
   //   if(fields.size() == 1) {
   //      GD.fixN = asInt(fields[0]);
   //   }
   //   else if(fields.size() == 3) {
   //      GD.fixN = asInt(fields[0]);
   //      GD.fixlim = asDouble(fields[1]);
   //      GD.fixsig = asDouble(fields[2]);
   //   }
   //   else {
   //      oss << " Error - invalid argument to --fix " << GD.fixfstr << "\n";
   //      GD.doFixF = false;
   //   }
   //}

   // window filters
   if(GD.doWF || GD.doXWF) {
      fields = split((GD.doWF ? GD.windstr : GD.xwindstr),',');
      if(fields.size() == 1)
         GD.windwidth = asInt(fields[0]);
      else if(fields.size() == 3) {
         GD.windstep = asDouble(fields[1]);
         GD.windratio = asDouble(fields[2]);
      }
      else
         oss << " Error - invalid argument to --" << (GD.doWF ? "":"x") << "wind "
               << (GD.doWF ? GD.windstr : GD.xwindstr) << "\n";
   }

   // set nostats
   if(GD.doBin || GD.doFDF || GD.doFDF2 || GD.doWF || GD.doXWF); // || GD.doFixF);
      GD.nostats = true;

   // set -b flags
   if(GD.brief) {
      GD.bc = GD.br = GD.brw = true;
      if(GD.wcol > -1) GD.bw = true;
      if(GD.xcol > -1) GD.b2 = true;
   }

   // set quiet when brief
   GD.quiet = (GD.quiet || GD.brief || GD.bc || GD.br || GD.bw || GD.brw || GD.b2);

   // --------------------------------------------------------------------------
   // add to command line errors
   GD.cmdlineErrors += oss.str();

   // set verbose and debug (CommandLine sets LOGlevel)
   GD.verbose = (LOGlevel >= VERBOSE);
   GD.debug = (LOGlevel - DEBUG);

   // --------------------------------------------------------------------------
   // dump it
   oss.str("");         // clear it
   oss << "------ Summary of " << GD.PrgmName
      << " command line configuration --------" << endl;
   opts.DumpConfiguration(oss);
      // perhaps dump the 'extra parsing' things
   oss << "------ End configuration summary --------" << endl;
   GD.cmdlineDump = oss.str();

   if(opts.hasHelp()) return 1;
   if(opts.hasErrors()) return 2;
   if(!GD.cmdlineErrors.empty()) return 2;
   return iret;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
// Title, debug output of cmdline, define GD.msg, include_path for input file
int Prepare(void) throw(Exception)
{
try {
   int i,j;
   GlobalData& GD=GlobalData::Instance();

   // display title
   if(!GD.quiet) cout << "# " << GD.Title << endl;

   if(GD.debug > -1) {
      cout << "Found debug switch at level " << GD.debug << endl;
      cout << "\n" << GD.cmdlineUsage << endl;  // this will contain list of args
      GD.verbose = true;
   }
   if(GD.verbose && !GD.quiet) cout << GD.cmdlineDump << endl;

   // set msg
   ostringstream oss;
   oss << "Data of col " << GD.col;
   if(GD.xcol > -1) oss << ", x of col " << GD.xcol;
   oss << ", file " << GD.inputfile;
   GD.msg = oss.str();

   //include_path(GD.logpath,GD.logfile);
   //expand_filename(GD.logfile);

   if(GD.inputfile.empty()) {
      GD.inputfile = string("stdin");
      if(GD.verbose && !GD.quiet)
         cout << "# Input from stdin\n";
   }
   else {
      include_path(GD.inpath,GD.inputfile);
      expand_filename(GD.inputfile);
      if(GD.verbose && !GD.quiet)
         cout << "# Found input file name " << GD.inputfile << endl;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
int ReadAndCompute(void) throw(Exception)
{
try {
   GlobalData& GD=GlobalData::Instance();

   // open input file -------------------------------------------
   istream *pin;
   if(GD.inputfile != string("stdin")) {
      pin = new ifstream(GD.inputfile.c_str());
      if(pin->fail()) {
         cout << "Could not open file " << GD.inputfile << " .. abort.\n";
         return -2;
      }
   }
   else {
      pin = &cin;
   }

   // read input file -------------------------------------------
   unsigned int i,nd(0),nxd(0);
   {
      int j;                              // not unsigned!
      double d,x(-1.0),w(-1.0),bias;
      string line;
      vector<string> words;
      while(1) {
         getline(*pin,line);
         if(pin->eof() || !pin->good()) break;

         StringUtils::stripLeading(line," ");
         if(line[0] == '#') continue;

         StringUtils::stripTrailing(line,"\n");
         StringUtils::stripTrailing(line,"\r");
         StringUtils::stripTrailing(line," ");
         StringUtils::change(line,"\t"," ");

         if(line.empty()) continue;

         words = StringUtils::split(line,' ');
         j = words.size();

         //cout << "LINE (" << j << ") " << line << endl;

         // check input   NB col numbers start at 1, indexes start at 0
         if(j == 0) continue;
         if(GD.col > j) { nd++; continue; }
         if(GD.xcol > j) { nxd++; continue; }
         if(GD.wcol > j) continue;
         if(!(isScientificString(words[GD.col-1]))) { nd++; continue; }
         if(GD.xcol > -1 && !(isScientificString(words[GD.xcol-1])))
               { nd++; continue; }
         if(GD.wcol > -1 && !(isScientificString(words[GD.wcol-1]))) continue;

         // user limits on data
         d = asDouble(words[GD.col-1]);
         if(GD.dodmin && d < GD.dmin) continue;
         if(GD.dodmax && d > GD.dmax) continue;

         if(GD.xcol > -1) {
            x = asDouble(words[GD.xcol-1]);
            // user limits on x
            if(GD.doxbeg && x < GD.xbeg) continue;
            if(GD.doxend && x > GD.xend) continue;
         }
         if(GD.wcol > -1)
            w = asDouble(words[GD.wcol-1]);

         // debias
         if(GD.debias0 && GD.data.size() == 0) { GD.debias = d; GD.dodebias = true; }
         if(GD.dodebias) d -= GD.debias;

         //cout << "READ " << i << " " << d << " " << x << endl;
         GD.data.push_back(d);
         if(GD.xcol > -1) GD.xdata.push_back(x);
         if(GD.wcol > -1) GD.wdata.push_back(w);

      }

      if(pin != &cin) {
         ((ifstream *)pin)->close();
         delete pin;
      }
   }

   // check that input is good ----------------------------------
   if(GD.data.size() < 2) {
      cout << "Abort: not enough data: " << GD.data.size() << " data read";
      if(nd > 0) cout << " [data(col) not found on " << nd << " lines]";
      if(nxd > 0) cout << " [data(xcol) not found on " << nxd << " lines]";
      cout << endl;
      return 5;
   }
   if(GD.xcol != -1 && GD.xdata.size() == 0) {
      cout << "Abort: No data found in 'x' column." << endl;
      return 5;
   }
   if(nd > GD.data.size()/2)
      cout << "Warning: data(col) not found on " << nd << " lines" << endl;
   if(nxd > GD.xdata.size()/2)
      cout << "Warning: data(xcol) not found on " << nxd << " lines" << endl;

   if(GD.verbose) cout << "Found " << GD.data.size() << " data.\n";

   // compute stats ---------------------------------------------
   const int N(GD.data.size());
   for(i=0; i<N; i++) {
      GD.cstats.Add(GD.data[i]);
      if(GD.xcol > -1) GD.tsstats.Add(GD.xdata[i],GD.data[i]);
      if(GD.wcol > -1) GD.wstats.Add(GD.data[i], GD.wdata[i]);
   }

   // compute robust stats --------------------------------------------------
   {
      // NB do not sort GD.data and GD.xdata
      vector<double> data(GD.data),xdata(GD.xdata),robwts(N);

      if(xdata.size() > 0) QSort(&data[0],&xdata[0],N);
      else                 QSort(&data[0],N);

      Robust::Quartiles(&data[0],data.size(),GD.Q1,GD.Q3);
      // outlier limit (high) 2.5Q3-1.5Q1
      // outlier limit (low ) 2.5Q1-1.5Q3
      GD.mad = Robust::MedianAbsoluteDeviation(&data[0],N,GD.median);
      GD.mest = Robust::MEstimate(&data[0], N, GD.median, GD.mad, &robwts[0]);

      for(i=0; i<N; i++)
         GD.robwtstats.Add(data[i],robwts[i]);
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
int OutputStats(void) throw(Exception)
{
try {
   GlobalData& GD=GlobalData::Instance();

   const int N(GD.data.size());
   cout << fixed << setprecision(GD.prec);

   // output stats ----------------------------------------------------------
   string fitmsg(GD.nfit==0 ? "" : "(fit resid)");
   string label(GD.label.empty() ? "" : " "+GD.label);
   // TD handle bias output better

   if(GD.bc || GD.b2) {
      cout << "rstats(con):" << label
         << " N " << setw(GD.prec) << GD.cstats.N()
         << "  Ave " << setw(GD.prec+3) << GD.cstats.Average()
         << "  Std " << setw(GD.prec+3) << GD.cstats.StdDev()
         << "  Var " << setw(GD.prec+3) << GD.cstats.Variance()
         << "  Min " << setw(GD.prec+3) << GD.cstats.Minimum()
         << "  Max " << setw(GD.prec+3) << GD.cstats.Maximum()
         << "  P2P " << setw(GD.prec+3) << GD.cstats.Maximum()-GD.cstats.Minimum();
      if(GD.dodebias) cout << " Bias " << GD.debias;
      cout << endl;
   }
   else if(!GD.quiet) {
      cout << "Conventional statistics: " << GD.msg << ":\n";
      cout << GD.cstats << "  Median = " << GD.median << endl;
      if(GD.dodebias) cout << " Bias    = " << GD.debias << endl;
   }

   if(GD.xcol > -1) {
      if(GD.b2) {
         cout << "rstats(two):" << label
            << " N " << setw(GD.prec) << GD.data.size()
            //<< " VarX " << setprecision(GD.prec) << GD.tsstats.VarianceX()
            //<< " VarY " << setprecision(GD.prec) << GD.tsstats.VarianceY()
            << "  Int " << setprecision(GD.prec) << GD.tsstats.Intercept()
            << "  Slp " << setprecision(GD.prec) << GD.tsstats.Slope()
            << " +- " << setprecision(GD.prec) << GD.tsstats.SigmaSlope()
            << "  CSig " << setprecision(GD.prec) << GD.tsstats.SigmaYX()
            << "  Corr " << setprecision(GD.prec) << GD.tsstats.Correlation();
         if(GD.dodebias) cout << "  Bias " << GD.debias;
         cout << endl;
      }
      else if(!GD.quiet) {
         cout << "Two-sample statistics: " << GD.msg << ":\n"
            << setprecision(GD.prec) << GD.tsstats << endl;
         if(GD.dodebias) cout << "  Bias " << GD.debias;
      }
   }

   if(GD.bw && GD.wcol > -1) {
      cout << "rstats(wtd):" << label
         << " N " << setw(GD.prec) << GD.wstats.N()
         << "  Ave " << setw(GD.prec+3) << GD.wstats.Average()
         << "  Std " << setw(GD.prec+3) << GD.wstats.StdDev()
         << "  Var " << setw(GD.prec+3) << GD.wstats.Variance()
         << "  Min " << setw(GD.prec+3) << GD.wstats.Minimum()
         << "  Max " << setw(GD.prec+3) << GD.wstats.Maximum()
         << "  P2P " << setw(GD.prec+3) << GD.wstats.Maximum()-GD.wstats.Minimum();
      if(GD.dodebias) cout << " Bias " << GD.debias;
      cout << endl;
   }
   else if(!GD.quiet && GD.wcol > -1) {
      cout << "Conventional weighted statistics: "
            << GD.msg << ", wt of col " << GD.wcol << ":\n"
            << fixed << setprecision(GD.prec) << GD.wstats << endl;
      if(GD.dodebias) cout << " Bias    = " << GD.debias << endl;
   }

   if(GD.brw) {
      cout << "rstats(rwt):" << label
         << " N " << setw(GD.prec) << GD.robwtstats.N()
         << "  Ave " << setw(GD.prec+3) << GD.robwtstats.Average()
         << "  Std " << setw(GD.prec+3) << GD.robwtstats.StdDev()
         << "  Var " << setw(GD.prec+3) << GD.robwtstats.Variance()
         << "  Min " << setw(GD.prec+3) << GD.robwtstats.Minimum()
         << "  Max " << setw(GD.prec+3) << GD.robwtstats.Maximum()
         << "  P2P " << setw(GD.prec+3)
                                 << GD.robwtstats.Maximum()-GD.robwtstats.Minimum();
      if(GD.dodebias) cout << " Bias " << GD.debias;
      cout << endl;
   }
   else if(!GD.quiet) {
      cout << "Conventional statistics with robust weighting: " << GD.msg << ":\n"
         << fixed << setprecision(GD.prec) << GD.robwtstats << endl;
      if(GD.dodebias) cout << " Bias    = " << GD.debias << endl;
   }

   if(GD.br) {
      cout << "rstats(rob):" << label
         << " N " << setw(GD.prec) << GD.data.size()
         << "  Med " << setw(GD.prec+3) << GD.median << "  MAD " << GD.mad
         << "  Min " << setw(GD.prec+3) << GD.cstats.Minimum()
         << "  Max " << GD.cstats.Maximum()
         << "  P2P " << setw(GD.prec+3) << GD.cstats.Maximum()-GD.cstats.Minimum()
         << "  Q1 " << setw(GD.prec+3) << GD.Q1 << "  Q3 " << setw(GD.prec+3)<< GD.Q3
         << "  QL " << setw(GD.prec+3) << 2.5*GD.Q1-1.5*GD.Q3
         << "  QH " << setw(GD.prec+3) << 2.5*GD.Q3-1.5*GD.Q1;
      if(GD.dodebias) cout << "  Bias " << GD.debias;
      cout << endl;
   }
   else if(!GD.quiet) {
      cout << "Robust statistics: " << GD.msg << ":\n";
	   cout << " Number    = " << GD.data.size() << endl;
	   cout << " Quartiles = " << setw(11) << setprecision(GD.prec) << GD.Q1
                     << "(1) " << setw(11) << GD.Q3
                     << "(3) " << setw(11) << 2.5*GD.Q3-1.5*GD.Q1
                     << "(H) " << setw(11) << 2.5*GD.Q1-1.5*GD.Q3
                     << "(L)" << endl;
	   cout << " Median = " << GD.median << "   MEstimate = " << GD.mest
	        << "   MAD = " << GD.mad << endl;
      if(GD.dodebias) cout << " Bias      = " << GD.debias << endl;
   }

   if(GD.doKS) {
      GD.KS = ADtest(&GD.data[0],N,GD.cstats.Average(),GD.cstats.StdDev(),false);
	   cout << "rstats KS test = " << setprecision(GD.prec) << GD.KS << endl;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
int StemAndLeaf(void) throw(Exception)
{
try {
   GlobalData& GD=GlobalData::Instance();

   try {
      vector<double> data(GD.data);
      QSort(&data[0],data.size());

      // NB assumes array is sorted
      Robust::StemLeafPlot(cout, &data[0], data.size(), GD.msg);
   }
   catch(Exception& e) {
      if(e.getText(0) == string("Invalid input") ||
         e.getText(0) == string("Array has zero range")) {
         cout << "(No stem and leaf plot; data is trivial)\n";
         return 0;
      }
      //if(e.getText(0) == string("Array is not sorted"))
      GPSTK_RETHROW(e);
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
int QuantilePlot(void) throw(Exception)
{
try {
   unsigned int i;
   GlobalData& GD=GlobalData::Instance();

   // get quantiles
   vector<double> qdata(GD.data.size());
   Robust::Quantiles(&qdata[0],qdata.size());

   // output to file qplot.out
   ostream *pout = new ofstream("qplot.out");
   if(pout->fail()) {
      cout << "Unable to open file qplot.out - output to screen\n";
      pout = &cout;
   }
   else cout << "Output quantiles, data to file qplot.out\n";

   // get TS stats
   TwoSampleStats<double> TSS;
   for(i=0; i<GD.data.size(); i++) TSS.Add(qdata[i],GD.data[i]);

   *pout << "# Quantile plot mean " << setprecision(GD.prec) << TSS.Intercept()
      << " std (slope) " << TSS.Slope() << " quantile data line follow:" << endl;
   for(i=0; i<GD.data.size(); i++)
      *pout << qdata[i] << " " << GD.data[i]
         << " " << TSS.Intercept() + TSS.Slope()*qdata[i]
         << endl;

   if(pout != &cout) ((ofstream *)pout)->close();

   cout << "Data vs quantiles fit to line yields y-intercept (=mean) "
      << setprecision(3) << TSS.Intercept()
      << " and slope (=std.dev.) " << TSS.Slope() << endl;
      //<< " try `plot qplot.out -x 1 -y 2,data -y 3,line,lines"
      //<< " -xl quantile -yl data -t \"Quantile plot\"`"
      //<< endl;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
// bins - given min, max and number of bins, find "pretty" bin boundaries
// (first-bin,step,<have nbins>) where first-bin is the *center* of the first bin,
// return 0 if ok
int Bins(const double& min, const double& max,
            int& nbins, double& firstbin, double& binstep, int& sexp) throw(Exception)
{
   if(nbins <= 2) GPSTK_THROW(Exception("Too few bins"));
   firstbin = binstep = 0.0;

   double amin(min),amax(max);
   if(amin > amax) { amin = max; amax = min; }
   else if(amin == amax) GPSTK_THROW(Exception("Equal limits"));

   //cout << "# Min Max " << fixed << setprecision(3) << min << " " << max << endl;
   binstep = (amax-amin)/double(nbins);
   //cout << "# Raw binstep " << binstep << endl;
   double tmp = log10(double(binstep)) - 1.0;
   sexp = int(tmp + (tmp > 0 ? 0.5 : -0.5));
   //cout << "# tmp is " << tmp << " and initial scale exp is " << sexp << endl;
   double scal = pow(10.0,sexp);
   //cout << "# scal binstep/scal " << scal << " " << binstep/scal <<endl;
   while(binstep/scal < 1.0) { scal /= 10.0; sexp--; }
   while(binstep/scal >= 10.0) { scal *= 10.0; sexp++; }
   //cout << "# Scale exponent is " << sexp << endl;
   binstep = double(int(0.5+binstep/scal)*scal);
   //cout << "# binstep " << binstep << endl;
   if(::fabs(binstep) < 1.e-3) { cout << " Error - binstep 0\n"; return -1; }
   double half(binstep/2.0);
   firstbin = binstep * int((amin + (amin > 0 ? 0.5 : -0.5))/binstep);
   while(firstbin-half > min) firstbin -= binstep;
   while(firstbin+half < min) firstbin += binstep;
   nbins = int((max-firstbin+half)/binstep);
   while(firstbin+(nbins-1.5)*binstep > max) nbins--;
   while(firstbin+(nbins-0.5)*binstep < max) nbins++;
   //cout << "# Check: first bin " << fixed << setprecision(2) << firstbin-0.5*binstep
   //   << " < " << min << " < " << firstbin+0.5*binstep << endl;
   //cout << "# Check: last bin " << fixed << setprecision(2)
   //   << firstbin+(nbins-1.5)*binstep << " < " << max << " < "
   //   << firstbin+(nbins-0.5)*binstep << endl;
   return 0;
}

//------------------------------------------------------------------------------------
int FindBins(void) throw(Exception)
{
try {
   unsigned int i,j,k;
   GlobalData& GD=GlobalData::Instance();

   int binprec;
   const double min(GD.cstats.Minimum()), max(GD.cstats.Maximum());
   if(GD.whichbin == 1) {        // only n user input
      i = Bins(min,max,GD.nbin,GD.firstbin,GD.widbin,binprec);       // bins
      if(i) return -1;
      if(binprec >= 0) binprec = 0; else binprec = -binprec;
   }
   else if(GD.whichbin == 2) {   // compute firstbin from min, max and GD.widbin
      GD.nbin = 1+int(0.5+(max-min)/GD.widbin);
      GD.firstbin = int((min/GD.widbin)+(min>0.0 ? 0.5:-0.5))*GD.widbin;
      if(min < GD.firstbin-GD.widbin/2.0) { GD.nbin++; GD.firstbin -= GD.widbin; }
      if(max > GD.firstbin+(GD.nbin-0.5)*GD.widbin) { GD.nbin++; }
   }
   cout << fixed << setprecision(binprec);
   //cout << "# Bins: n " << GD.nbin << " start " << GD.firstbin
   //         << " step " << GD.widbin << " prec " << binprec << endl;
   //cout << "Bin centers: (edges are center +- GD.widbin/2)";
   //for(i=0; i<GD.nbin; i++) cout << " " << GD.firstbin + i*GD.widbin; cout << endl;
   if(GD.nbin > 100) {
      cout << "Error - too many bins: " << GD.nbin << endl;
      return 0;
   }

   double half(GD.widbin/2.0);
   vector<int> bins(GD.nbin,0);
   for(k=0,i=0; i<GD.data.size(); i++) {
      j = int((GD.data[i]-GD.firstbin+half)/GD.widbin);
      //double cen = GD.firstbin+j*GD.widbin;
      //cout << " Data " << i << " in bin " << j << " : "
      //   << cen-half << " < " << GD.data[i] << " < " << cen+half << " "
      //   << ((cen-half < GD.data[i] && GD.data[i] < cen+half) ? "ok":"oops")
      //   << endl;
      if(j < 0 || j > GD.nbin-1) {
         cout << "# Warning - invalid bin " << j << " for data " << GD.data[i]
                  << endl;
         continue;
      }
      bins[j]++;
      k++;
   }

   // write out data (bin centers)
   cout << "# bins: N,width,first " << GD.nbin
         << "," << GD.widbin << "," << GD.firstbin << endl;
   cout << "# n center samples (low_edge to high_edge)" << endl;
   cout << "# total number of samples within bins " << k << endl;
   for(i=0; i<GD.nbin; i++)
      cout << setw(3) << i+1 << " " << setprecision(binprec)
         << GD.firstbin+i*GD.widbin << " " << setw(3) << bins[i]
         << setprecision(binprec+1)
         << "    (" << GD.firstbin+(i-0.5)*GD.widbin << " to "
         << GD.firstbin+(i+0.5)*GD.widbin << ")" << endl;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
int ComputeSum(void) throw(Exception)
{
try {
   unsigned int i,j,k;
   GlobalData& GD=GlobalData::Instance();

   // compute sum, dt, gaps -------------------------------------------------
   // NB e.g. for files dgar3650.12o.Z etc try
   // ls -1 dgar*.12o.Z | perl -nl -e 'print substr($_,4,3)' | rstats --sum
   // bash requires either '..$_..' or "..\$_.." on the perl cmd
   bool first(true);
   const int ndtmax(9);
   int jj,kk,nleast,ndt[ndtmax];
   double sum(0.0),prev,dt,bestdt[ndtmax];
   vector<int> gapcounts;
   if(GD.doSumPlus) for(j=0; j<ndtmax; j++) ndt[j] = -1;

   // compute dt and sum
   prev = sum = GD.data[0];
   for(i=1; i<GD.data.size(); i++) {
      sum += GD.data[i];
      if(GD.doSumPlus) {
         dt = GD.data[i] - prev;
         for(j=0; j<ndtmax; j++) {
            if(ndt[j] <= 0) { bestdt[j]= dt; ndt[j]=1; break; }
            if(dt != 0.0 && fabs((dt-bestdt[j])/dt) < 0.01) { ndt[j]++; break; }
            if(j == ndtmax-1) {
               kk = 0;
               nleast = ndt[kk];
               for(k=1; k<ndtmax; k++)
                  if(ndt[k]<=nleast) { kk=k; nleast=ndt[k]; }
               ndt[kk]=1; bestdt[kk]=dt;
            }
         }
         prev = GD.data[i];
      }
   }

   if(GD.doSumPlus) {
      for(j=0,i=1; i<ndtmax; i++) { if(ndt[i]>ndt[j]) j=i; }
      bestdt[0] = bestdt[j]; ndt[0] = ndt[j];

      // find gaps
      for(i=0; i<GD.data.size(); i++) {
         dt = GD.data[i] - GD.data[0];
         jj = int(0.5+dt/bestdt[0])+1;    // compute the gapcount

         // update gap count for epochs
         if(gapcounts.size() == 0) {      // create the list
            gapcounts.push_back(jj);      // start time
            gapcounts.push_back(jj-1);    // end time
         }
         // update existing list
         k = gapcounts.size() - 1;        // index of the current end time
         if(jj == gapcounts[k] + 1) {     // no gap
            gapcounts[k] = jj;
         }
         else {                           // found a gap
            gapcounts.push_back(jj);      // start time
            gapcounts.push_back(jj);      // end time
         }
      }
   }

   // output
   cout << "Sum = " << fixed << setprecision(GD.prec) << sum << endl;
   if(GD.doSumPlus) {
      k = gapcounts.size()-1;          // size() is at least 2
      cout << "Best step = " << bestdt[0]
         << " (" << 100.*ndt[0]/(GD.data.size()-1) << "%)"
         << " : total " << GD.data.size() << " data, counts "
         << gapcounts[0] << " thru " << gapcounts[k] << endl;
      cout << "Range " << GD.data[0] << " thru " << GD.data[GD.data.size()-1] << endl;
      if(k <= 2)
         cout << "No gaps." << endl;
      else
         for(i=1; i<=k-2; i+=2) {
         jj = gapcounts[i+1]-gapcounts[i]-1;
         cout << "Gap at cnt " << gapcounts[i]+1
            << " = data " << GD.data[0]+gapcounts[i]*bestdt[0]
            << " : size " << jj << " cnts = " << jj*bestdt[0] << " data" << endl;
      }
      cout << endl;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
int FitPoly(void) throw(Exception)
{
try {
   unsigned int i,j;
   GlobalData& GD=GlobalData::Instance();

   vector<double> savedata(GD.data), robwts(GD.data.size());
   double *coef,eval,xx,x0;

   coef = new double[GD.nfit];
   if(!coef) {
      cout << "Abort: allocate coefficients failed.\n";
      return -4;
   }

   int iret = Robust::RobustPolyFit(&GD.data[0], &GD.xdata[0], GD.data.size(),
                                    GD.nfit, &coef[0], &robwts[0]);

   cout << "RobustPolyFit returns " << iret << endl;
   if(iret == 0) {
      cout << " Coefficients:" << setprecision(GD.prec);
      for(i=0; i<GD.nfit; i++) {
         if(fabs(coef[i]) < 0.001)
            cout << " " << scientific;
         else
            cout << " " << fixed;
         cout << coef[i];
      }
      cout << endl << fixed << setprecision(GD.prec);
      cout << " Offsets: Y(col " << GD.col << ") " << savedata[0]
         << " X(col " << GD.xcol << ") " << GD.xdata[0] << endl;

      // output to file rstats.out
      ostream *pout = new ofstream("rstats.out");
      if(pout->fail()) {
         cout << "Unable to open file rstats.out - output to screen\n";
         pout = &cout;
      }
      else {
         cout << " Output polynomial fit to file rstats.out (try plotrfit)\n";
         //cout << " Try the command plot rstats.out -x 1 -y 4,residuals "
         //   << "-y2 2,data,points -y2 3,fit,lines -xl X -yl Residuals \\\n     "
         //   << "-y2l \"Data and fit\" -t \"Robust fit (degree " << GD.nfit
         //   << "), output of rstats for file " << GD.inputfile << "\"\n";
      }

      // write to rstats.out
      x0 = GD.xdata[0];
      *pout << "#Xdata, Data, fit, resid, weight (" << GD.data.size() << " pts):"
         << fixed << setprecision(GD.prec) << endl;
      for(i=0; i<GD.data.size(); i++) {
         eval = savedata[0] + coef[0];
         xx = GD.xdata[i]-x0;
         for(j=1; j<GD.nfit; j++) { eval += coef[j]*xx; xx *= (GD.xdata[i]-x0); }
         *pout << fixed << setprecision(GD.prec)
               << GD.xdata[i] << " " << savedata[i]
               << " " << eval << " " << GD.data[i]
               << scientific << " " << robwts[i] << endl;
      }
      for(i=0; i<GD.xevalfit.size(); i++) {
         eval = savedata[0] + coef[0];
         xx = GD.xevalfit[i]-x0;
         for(j=1; j<GD.nfit; j++) { eval += coef[j]*xx; xx *= (GD.xevalfit[i]-x0); }
         *pout << fixed << setprecision(GD.prec) << "#Evaluate_Fit(X): X = "
               << GD.xevalfit[i] << " F(X) = " << eval << endl;
         cout << fixed << setprecision(GD.prec) << " Evaluate Fit("
               << GD.xevalfit[i] << ") = " << eval << endl;
      }

      if(pout != &cout) ((ofstream *)pout)->close();

      //QSort(&robwts[0],robwts.size());
      //Robust::StemLeafPlot(cout, &robwts[0], robwts.size(), "weights");
   }
   //cout << endl;
   delete[] coef;

   ostringstream oss;
   oss << "Residuals of fit (deg " << GD.nfit << ") col " << GD.col
      << " vs x col " << GD.xcol << ", file " << GD.inputfile;
   GD.msg = oss.str();

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
int Sequential(void) throw(Exception)
{
try {
   GlobalData& GD=GlobalData::Instance();

   Stats<double> stats;
   cout << "Data and sequential stats ([lab] [xdata] data n ave std)\n";
   cout << fixed << setprecision(GD.prec);
   for(unsigned int i=0; i<GD.data.size(); i++) {
      stats.Add(GD.data[i]);
      if(!GD.label.empty()) cout << GD.label << " ";
      if(GD.xdata.size()>0) cout << GD.xdata[i] << " ";
      cout << GD.data[i] << "   " << stats.N() << " "
           << stats.Average() << " "
           << (stats.N() > 1 ? stats.StdDev() : 0.0) << endl;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
int Discontinuity(void) throw(Exception)
{
try {
   unsigned int i,j,k;
   GlobalData& GD=GlobalData::Instance();

   j = GD.data.size();
   k = GD.xdata.size();

   cout << fixed << setprecision(GD.prec);
   cout << "# Output " << (k > 0 ? "xdata,":"")
         << " data, 1st diff, and stats;\n# discontinuity tolerance y="
         << GD.ytol << ", x=" << GD.xtol << endl;
   cout << "#" << (GD.label.empty() ? "" : " [lab]") << (k > 0 ? " xdata" : "")
      << " data 1st-diff-y n ave std MSG(for disc.s)\n";
   cout << "# MSG=DISC" << (k > 0 ? " del-x" : "") << " del-data"
      << " (stats-for-prev-seg:) N ave std" << (k > 0 ? " xbeg xend" : "")
      << " gap/slip/EOD" << endl;

   double xlast(k > 0 ? GD.xdata[0] : 0);
   Stats<double> segstats;
   ostringstream oss;

   segstats.Add(GD.data[0]);
   if(k > 0) cout << GD.xdata[0] << " ";
   cout << GD.data[0] << " " << 0.0 << "   "
        << 1 << " " << GD.data[0] << " " << 0.0 << endl;

   for(i=1; i<j; i++) {                // loop over data (starting at 1)
      bool slip(false), gap(false);
      double fd(GD.data[i]-GD.data[i-1]);
      oss.str("");

      // is there a gap?
      if(k > 0 && GD.xtol > 0 && fabs(GD.xdata[i]-GD.xdata[i-1]) > GD.xtol)
         gap=true;

      // is there a potential discontinuity?
      // TD eliminate outliers, which produce adjacent fd's of opposite sign
      if(fabs(fd) > GD.ytol)
         slip=true;

      if(gap || slip || i==j-1) {
         oss << " DISC" << fixed << setprecision(GD.prec);
         if(k > 0) {
            if(i < j-1) oss << " " << GD.xdata[i]-GD.xdata[i-1];
            else        oss << " " << "00";
         }
         if(i < j-1) oss << " " << fd;
         else        oss << " " << "00";

         if(i == j-1) segstats.Add(GD.data[i]);

         oss << " " << segstats.N()
            << " " << segstats.Average()
            << " " << (segstats.N()>1 ? segstats.StdDev() : 0.0);
         if(k > 0) oss << " " << xlast << " " << GD.xdata[i==j-1 ? i:i-1];

         if(slip) oss << " slip";
         if(gap) oss << " gap";
         if(i==j-1) oss << " EOD";

         segstats.Reset();
      }

      if(gap) xlast = GD.xdata[i];

      segstats.Add(GD.data[i]);

      // output data
      if(!GD.label.empty()) cout << GD.label << " ";
      if(k > 0) cout << GD.xdata[i] << " ";
      cout << GD.data[i] << " " << fd << "   "
           << segstats.N() << " "
           << segstats.Average() << " "
           << (segstats.N() > 1 ? segstats.StdDev() : 0.0)
           << oss.str()
           << endl;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
int FDFilter(void) throw(Exception)
{
try {
   unsigned int i,j,k;
   GlobalData& GD=GlobalData::Instance();

   // for output (TD make option) fill flags first
   vector<int> flags(GD.data.size(),0);

   // xdata and flags must exist but may be empty
   FirstDiffFilter<double> fdf(GD.xdata, GD.data, flags);
   fdf.setprecision(GD.prec);
   fdf.setLimit(GD.fdfstep);
   i = fdf.filter();
   cout << "# FD Filter returns " << i << endl;
   if(i < 0) cout << "# FD Filter failed (" << i << ")" << endl;
   else {
      if(GD.doFDF2) {
         string str;
         i = fdf.analyze2(GD.fdfrat,GD.fdfsig,str);
         cout << "#" << str << endl;
         return 0;
      }
      else i = fdf.analyze();

      if(i < 0) cout << "# FD Filter analysis failed (" << i << ")" << endl;

      for(i=0; i<fdf.results.size(); i++)
         fdf.getStats(fdf.results[i]);
      fdf.setDumpNoAnal(GD.verbose);
      fdf.dump(cout, GD.label);

      // clean the data based on results of filter
      vector< FilterHit<double> > hit=fdf.getResults();
      for(j=0; j<hit.size(); j++) {
         if(hit[j].type == FilterHit<double>::BOD) continue;
         else if(hit[j].type == FilterHit<double>::outlier) {
            for(k=0; k<hit[j].npts; k++)
               flags[hit[j].index+k] = -1;      // flag for outlier
         }
         else if(hit[j].type == FilterHit<double>::slip) {
            for(k=hit[j].index; k<GD.data.size(); k++)
               GD.data[k] -= hit[j].step;
         }
      }

      // write cleaned data to rstats.out
      ostream *pout = new ofstream("rstats.out");
      if(pout->fail()) {
         cout << "Unable to open file rstats.out - output to screen\n";
         pout = &cout;
      }

      for(i=0; i<GD.data.size(); i++)
         *pout << fixed << setprecision(GD.prec) << i
               << " " << (GD.xdata.size() ? GD.xdata[i] : (double)(i))
               << " " << GD.data[i] << " " << flags[i] << endl;
      if(pout != &cout) ((ofstream *)pout)->close();
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
int WindFilter(void) throw(Exception)
{
try {
   unsigned int i;
   GlobalData& GD=GlobalData::Instance();

   // for output fill flags first
   vector<int> flags(GD.data.size(),0);

   // xdata and flags must exist but may be empty
   WindowFilter<double> wf(GD.xdata, GD.data, flags);
   wf.setTwoSample(GD.doXWF);
   wf.setWidth(GD.windwidth);
   wf.setMinRatio(GD.windratio);
   wf.setMinStep(GD.windstep);
   wf.setprecision(GD.prec);
   i = wf.filter();
   if(i < 0) cout << "# window filter failed (" << i << ")" << endl;
   else {
      if(GD.debug > -1) wf.setDebug(true);
      wf.analyze();           // ignore return values
      if(GD.verbose) wf.setDumpAnalMsg(true);
      wf.dump(cout, GD.label);
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
//int FixFilter(void) throw(Exception)
//{
//try {
//   unsigned int i,j,k;
//   GlobalData& GD=GlobalData::Instance();
//
//   cout << "# fix filter with width " << GD.fixN << ", limit "
//         << fixed << setprecision(3)
//         << GD.fixlim << " and siglim " << GD.fixsig << endl;
//
//   // iterate filter-Rstats-analysis
//   double slim;
//   unsigned iter(0),itermax(3);
//   vector<int> flags(GD.data.size(),0);
//   while(iter < itermax) {
//      // must redefine filter each time since arrays (const in fdf) change
//      FDiffFilter<double> fdf(GD.xdata, GD.data, flags);
//      fdf.setWidth(GD.fixN);
//      fdf.setSigma(GD.fixsig);
//      fdf.setLimit(GD.fixlim);
//      fdf.setprecision(5);
//      fdf.setw(10);
//
//      // filter the data
//      i = fdf.filter();
//      //cout << "# filter returned " << i << endl;
//      if(i <= 2) { cout << "Not enough data, abort.\n"; return 0; }
//
//      //TEMP?if(iter > 0 && fdf.getNhighSigma() == 0)
//      //TEMP?   { cout << "Done.\n"; break; }
//
//      // change sig lim?
//      i = fdf.RstatsOnSigma(slim);
//      cout << "# Estimated sigma limit " << fixed << setprecision(5)
//                  << slim << " with " << i << " hi-sigma points " << endl;
//
//      if(slim > GD.fixsig)
//         fdf.setSigma(slim);        // use the new sigma limit
//
//      fdf.analysis();               // get the outliers and slips
//
//      // run over results - NB flags is const in fdf
//      for(i=0; i<fdf.results.size(); i++) {
//         cout << "# Result # " << i << " " << fdf.results[i].asString() << endl;
//
//         // mark outliers
//         if(fdf.results[i].type == FDFResult<double>::OUT) {
//            k = fdf.results[i].index;
//            for(j=0; j<fdf.results[i].npts; j++) {
//               flags[k+j] = 1;
//            }
//         }
//
//         // fix slips
//         else if(fdf.results[i].type == FDFResult<double>::SLIP) {
//            if(::fabs(fdf.results[i].step) < GD.fixlim) {
//               cout << "# Slip too small: " << fixed << setprecision(3)
//                  << fdf.results[i].step << " < " << GD.fixlim << endl;
//               continue;
//            }
//
//            int islip(fdf.results[i].step
//                        + (fdf.results[i].step >= 0.0 ? 0.5:-0.5));
//            if(islip) {
//               cout << "# Fix slip " << islip
//                        << " " << fdf.results[i].asString() << endl;
//
//               for(j=fdf.results[i].index; j<GD.data.size(); j++)
//                  GD.data[j] -= islip;
//            }
//         }
//      }
//
//      fdf.dump(cout,"FIX"+StringUtils::asString(iter));
//
//      //fdf.setSigma(fixsig);         // reset it
//
//      ++iter;
//   }  // end iteration
//
//   return 0;
//}
//catch(Exception& e) { GPSTK_RETHROW(e); }
//}

//------------------------------------------------------------------------------------
int Outliers(void) throw(Exception)
{
try {
   unsigned int i,j;
   GlobalData& GD=GlobalData::Instance();

   double ave(GD.cstats.Average()),sig(GD.cstats.StdDev());
   // normally GD.outscale=1: 2.5*Q3 - 1.5*Q1;
   double OH = GD.Q3 + GD.outscale*1.5*(GD.Q3-GD.Q1);
   // normally GD.outscale=1: 2.5*Q1 - 1.5*Q3;
   double OL = GD.Q1 - GD.outscale*1.5*(GD.Q3-GD.Q1);
   vector<int> outhi,outlo;
   for(i=0; i<GD.data.size(); i++) {
      if(GD.data[i] > OH)
         outhi.push_back(i);
      else if(GD.data[i] < OL)
         outlo.push_back(i);
   }
   cout << "There are " << outhi.size()+outlo.size() << " outliers; "
      << outlo.size() << " low (< " << setprecision(GD.prec) << OL << ") and "
      << outhi.size() << " high (> " << setprecision(GD.prec) << OH << ")."
      << endl << "     n  " << (GD.xdata.size() > 0 ? "x-value" : "")
      << "   value  val/outlim  val-ave (val-ave)/sig" << endl;
   // NB data and xdata have been sorted together
   for(j=1,i=0; i<outlo.size(); i++,j++) {
      cout << " OTL " << j << " ";
      if(GD.xdata.size() > 0) cout << GD.xdata[outlo[i]] << " ";
      cout << GD.data[outlo[i]] << " " << GD.data[outlo[i]]/OL
         << " " << GD.data[outlo[i]]-ave
         << " " << (GD.data[outlo[i]]-ave)/sig << endl;
   }
   for(i=0; i<outhi.size(); i++,j++) {
      cout << " OTH " << j << " ";
      if(GD.xdata.size() > 0) cout << GD.xdata[outhi[i]] << " ";
      cout << GD.data[outhi[i]] << " " << GD.data[outhi[i]]/OH
         << " " << GD.data[outhi[i]]-ave
         << " " << (GD.data[outhi[i]]-ave)/sig << endl;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
int DumpData(string msg) throw(Exception)
{
try {
   unsigned int i;
   GlobalData& GD=GlobalData::Instance();

   for(i=0; i<GD.data.size(); i++) {
      cout << msg << " " << fixed << setprecision(GD.prec) << i;
      if(GD.xcol > -1) cout << " " << GD.xdata[i];
      cout << " " << GD.data[i];
      if(GD.wcol > -1) cout << " " << GD.wdata[i];
      cout << endl;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
