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
 * @file rstats.cpp
 * Read the data in one column of a file, and output robust statistics,
 * two-sample statistics, a stem-and-leaf plot, a quantile-quantile plot,
 * and a robust polynomial fit.
 */

#include <iostream>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

#include "StringUtils.hpp"
#include "Stats.hpp"
#include "PolyFit.hpp"

#include "RobustStats.hpp"

using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
int BadOption(string& arg) {
   cout << "Error: " << arg << " requires argument. Abort.\n";
   return -1;
}
//------------------------------------------------------------------------------------
// TD add option to filter on data value (string)
int main(int argc, char **argv)
{
   try {
      bool help,nostats=false,qplot=false;
      int i,j,col=1,plot=0,xcol=-1,fit=-1;
      string filename;
      ostream *pout;

      help = (argc <= 1);
      for(i=1; i<argc; i++) {
         if(argv[i][0] == '-') { // && argv[i][1] == '-')
            string arg(argv[i]);
            if(arg == string("--help") || arg == string("-h"))
               help = true;
            else if(arg == string("--plot") || arg == string("-p"))
               plot = 1;
            else if(arg == string("--qplot") || arg == string("-q"))
               qplot = true;
            else if(arg == string("--nostats") || arg == string("-n"))
               nostats = true;
            else if(arg == string("--col") || arg == string("-c")) {
               if(i==argc-1) return BadOption(arg);
               col = atoi(argv[++i]);
            }
            else if(arg == string("--xcol") || arg == string("-x")) {
               if(i==argc-1) return BadOption(arg);
               xcol = atoi(argv[++i]);
            }
            else if(arg == string("--fit") || arg == string("-f")) {
               if(i==argc-1) return BadOption(arg);
               fit = atoi(argv[++i]);
            }
            else {
               cout << "Ignore unknown option: " << arg << endl;
            }
         }
         else {
            filename = string(argv[i]);
         }
      }

      if(help) {
         cout << "Usage: rstats <file> --col <col> --plot "
            << "--qplot --xcol <xcol> --fit <N> --nostats\n";
         cout << "   Compute standard and robust statistics on numbers "
            << "in column <col> (default 1)\n"
            << "                of file <file> (or redirect stdin).\n";
         cout << "   If option '--plot' is present, also show a stem-and-leaf plot\n";
         cout << "   Use option --qplot to generate data for a quantile-quantile"
            << " plot\n" << "                (data written to file qplot.out)\n";
         cout << "   Use option --xcol to specify another column, "
            << "and output 2-sample stats\n";
         cout << "   Use option --fit to specify degree of robust polynomial fit "
            << "to data,\n"
            << "                using xcol as independent variable,"
            << " output in rstats.out\n"
            << "                (--nostats to supress stats output to screen)\n";
         return -1;
      }

      if(fit > -1 && xcol == -1) {
         cout << "Error: --fit requires --xcol <xcol>\n";
         return -1;
      }

      istream *pin;                 // do it this way for Windows...
      if(!filename.empty()) {
         pin = new ifstream(filename.c_str());
         if(pin->fail()) {
            cout << "Could not open file " << filename << " .. abort.\n";
            return -2;
         }
      }
      else pin = &cin;

      // 1-line message to screen
      cout << "rstats for ";
      if(pin == &cin) cout << "data from stdin";
      else            cout << "file: " << filename;
      cout << ", stats (col " << col << ")";
      if(xcol > -1) {
         cout << " and 2-sample stats (x-col " << xcol << ")";
         if(fit > -1) {
            cout << ", fit (" << fit << ")";
            if(nostats) cout << " (but no stats)";
         }
      }
      cout << endl;

      const int BUFF_SIZE=1024;
      char buffer[BUFF_SIZE];
      int nd,nxd;
      double d,xd;
      string stuff;
      vector<double> data,wts,xdata;
      Stats<double> S;
      TwoSampleStats<double> TSS;

      nd = nxd = 0;
      while(pin->getline(buffer,BUFF_SIZE)) {
         //if(buffer[0] == '#') continue;
         string line = buffer;
         StringUtils::stripTrailing(line,'\r');
         // remove leading whitespace
         line = StringUtils::stripLeading(line,string(" "));
         // skip comments
         if(line[0] == '#') continue;
         //check that column col is there...
         if(StringUtils::numWords(line) < col) { nd++; continue; }
         // pull it out
         stuff = StringUtils::word(line,col-1);
         // is it a number?
         if(!(StringUtils::isDecimalString(stuff))) { nd++; continue; }
         // convert it to double and save it
         d = StringUtils::asDouble(stuff);
         data.push_back(d);
         S.Add(d);

         // do the same for xcol
         if(xcol > -1) {
            if(StringUtils::numWords(line) < xcol)
               { data.pop_back(); nxd++; continue; }
            stuff = StringUtils::word(line,xcol-1);
            if(!(StringUtils::isDecimalString(stuff)))
               { data.pop_back(); nxd++; continue; }
            xd = StringUtils::asDouble(stuff);
            TSS.Add(xd, d);
            xdata.push_back(xd);
         }
      }

      if(pin != &cin) {
         ((ifstream *)pin)->close();
         delete pin;
      }

      // check that input was good
      if(data.size() == 0) {
         cout << "Abort: no data.";
         if(nd > 0) cout << " [data(col) not found on " << nd << " lines]";
         if(nxd > 0) cout << " [data(xcol) not found on " << nxd << " lines]";
         cout << endl;
         return -3;
      }
      if(nd > data.size()/2)
         cout << "Warning: data(col) not found on " << nd << " lines" << endl;
      if(nxd > xdata.size()/2)
         cout << "Warning: data(xcol) not found on " << nxd << " lines" << endl;

      //cout << "Collected " << data.size() << " data.\n" << fixed;
      //for(i=0; i<data.size(); i++) {
      //   cout << " " << setprecision(3) << data[i];
      //   if(xcol > -1) cout << " : " << xdata[i] << " ";
      //   if(!((i+1)%(xcol > -1 ? 4 : 10))) cout << endl;
      //}

      // process fit
      if(fit > -1) {
         vector<double> savedata(data);
         double *coef,eval,tt,t0;
         wts.resize(data.size());

         coef = new double[fit];
         if(!coef) {
            cout << "Abort: allocate coefficients failed.\n";
            return -4;
         }

         int iret = Robust::RobustPolyFit(&data[0], &xdata[0], data.size(),
                                          fit, &coef[0], &wts[0]);

         cout << "RobustPolyFit returns " << iret << endl;
         if(iret == 0) {
            cout << " Coefficients:" << fixed << setprecision(3);
            for(i=0; i<fit; i++) cout << " " << coef[i];
            cout << endl;

            // output to file rstats.out
            pout = new ofstream("rstats.out");
            if(pout->fail()) {
               cout << "Unable to open file rstats.out - output to screen\n";
               pout = &cout;
            }
            else {
               cout << "Output polynomial fit to file rstats.out\n";
               cout << " try `plot rstats.out -x 1 -y 2,data,points -y 3,fit,lines"
                  << " -y 4,residuals -y2 5,weights --y2range -0.1:1.1 "
                  << "-t \"Robust fit (degree " << fit
                  << "), output of rstats for file " << filename << "\"`" << endl;
            }
            t0 = xdata[0];
            *pout << "#Xdata, Data, fit, resid, weight (" << data.size() << " pts):"
               << fixed << setprecision(3) << endl;
            for(i=0; i<data.size(); i++) {
               eval = savedata[0] + coef[0];
               tt = xdata[i]-t0;
               for(j=1; j<fit; j++) { eval += coef[j]*tt; tt *= (xdata[i]-t0); }
               *pout << fixed << setprecision(3)
                     << xdata[i] << " " << savedata[i]
                     << " " << eval << " " << data[i]
                     << scientific << " " << wts[i] << endl;
            }
            if(pout != &cout) ((ofstream *)pout)->close();
            //QSort(&wts[0],wts.size());
            //Robust::StemLeafPlot(cout, &wts[0], wts.size(), "weights");
         }
         cout << endl;
         delete[] coef;
         if(nostats) return 0;
      }

      double median,mad,mest,Q1,Q3;
      QSort(&data[0],data.size());
      Robust::Quartiles(&data[0],data.size(),Q1,Q3);
      mad = Robust::MedianAbsoluteDeviation(&data[0],data.size(),median);
      wts = data;
      mest = Robust::MEstimate(&data[0], data.size(), median, mad, &wts[0]);
      cout << "Conventional statistics:\n"
         << fixed << setprecision(8) << S << endl;

      if(xcol > -1)
         cout << "Two-sample statistics:\n" << setprecision(8) << TSS << endl;
      
      cout << "Robust statistics:\n";
	   cout << " Number    = " << data.size() << endl;
	   cout << " Quartiles = " << setw(11) << setprecision(8) << Q1
                     << " " << setw(11) << setprecision(8) << Q3 << endl;
	   cout << " Median    = " << setw(11) << setprecision(8) << median << endl;
	   cout << " MEstimate = " << setw(11) << setprecision(8) << mest << endl;
	   cout << " MAD       = " << setw(11) << setprecision(8) << mad << endl;

      if(plot) {
         try {
            Robust::StemLeafPlot(cout, &data[0], data.size(),
               string("Robust stats for column ") + StringUtils::asString(col) +
               (filename.empty() ? string(" of input")
                                 : string(" of file ") + filename)
               );
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
      }

      if(qplot) {
         xdata.resize(data.size());
         Robust::QuantilePlot(&data[0],data.size(),&xdata[0]);
         // output to file rstats.out
         pout = new ofstream("qplot.out");
         if(pout->fail()) {
            cout << "Unable to open file qplot.out - output to screen\n";
            pout = &cout;
         }
         else cout << "Output q-q data to file qplot.out (plot column 2 vs 1)\n";

         TSS.Reset();         // use TSS to get slope and intercept of q-q fit to line
         for(i=0; i<data.size(); i++) TSS.Add(xdata[i],data[i]);
         for(i=0; i<data.size(); i++)
            *pout << xdata[i] << " " << data[i]
               << " " << TSS.Intercept() + TSS.Slope()*xdata[i]
               << endl;

         if(pout != &cout) ((ofstream *)pout)->close();

         cout << "Q-Q data fit to line yields y-intercept (mean) "
            << setprecision(3) << TSS.Intercept()
            << " and slope (std.dev.) " << TSS.Slope() << endl
            << " try `plot qplot.out -x 1 -y 2,data -y 3,line,lines"
            << " -xl quantile -yl data -t \"Quantile plot\"`"
            << endl;
      }

      return 0;
   }
   catch(Exception& e) {
      cout << "GPSTk Exception : " << e;
   }
   catch (...) {
      cout << "Unknown error.  Abort." << endl;
   }
   return -1;
}   // end main()
