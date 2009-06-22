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
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>

#include "StringUtils.hpp"
#include "Stats.hpp"
#include "PolyFit.hpp"

#include "RobustStats.hpp"

using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
int BadOption(string& arg) {
   cout << "Error: " << arg << " requires argument. Abort.\n";
   return -1;
}
//------------------------------------------------------------------------------------
// TD add option to filter on data value (string)
int main(int argc, char **argv)
{
   clock_t totaltime;            // for timing tests
   // begin counting time
   totaltime = clock();
   try {
      bool help,nostats=false,plot=false,qplot=false,domin=false,domax=false;
      int i,j,col=1,xcol=-1,fit=-1,prec=3;
      double out=0.0,min,max;
      string filename;
      ostream *pout;

      help = (argc <= 1);
      for(i=1; i<argc; i++) {
         if(argv[i][0] == '-') { // && argv[i][1] == '-')
            string arg(argv[i]);
            if(arg == string("--help") || arg == string("-h"))
               help = true;
            else if(arg == string("--plot") || arg == string("-p"))
               plot = true;
            else if(arg == string("--qplot") || arg == string("-q"))
               qplot = true;
            else if(arg == string("--outliers") || arg == string("--outs")
                  || arg == string("-o")) {
               if(i==argc-1) return BadOption(arg);
               out = atof(argv[++i]);
            }
            else if(arg == string("--nostats") || arg == string("-n"))
               nostats = true;
            else if(arg == string("--col")
                 || arg == string("-c") || arg == string("-y")) {
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
            else if(arg == string("--prec")) {
               if(i==argc-1) return BadOption(arg);
               prec = atoi(argv[++i]);
            }
            else if(arg == string("--min")) {
               if(i==argc-1) return BadOption(arg);
               min = atof(argv[++i]);
               domin = true;
            }
            else if(arg == string("--max")) {
               if(i==argc-1) return BadOption(arg);
               max = atof(argv[++i]);
               domax = true;
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
         cout << "Usage: rstats <file> --col <col> --xcol <xcol> --plot --qplot --fit <N>\n"
            << "      --min <lo> --max <hi> --nostats --prec <n> --outliers <s>\n";
         cout << "   Compute standard and robust statistics on numbers "
            << "in column <col> (default 1)\n"
            << "                of file <file> (or redirect stdin).\n    Use:\n";
         cout << "   option --min <lo> to include only data that satisfies d > lo\n";
         cout << "   option --max <hi> to include only data that satisfies d < hi\n";
         cout << "   option --plot to generate a stem-and-leaf plot\n";
         cout << "   option --qplot to generate data for a quantile-quantile"
            << " plot\n" << "                (data written to file qplot.out)\n";
         cout << "   option --xcol to specify another column, "
            << "and output 2-sample stats\n";
         cout << "   option --fit to specify degree of robust polynomial fit "
            << "to data,\n"
            << "                using xcol as independent variable,"
            << " output in rstats.out\n"
            << "                (--nostats to supress stats output to screen)\n";
         cout << "   option --outs <s> to list all data outside s*outlier limits\n";
         cout << "   option --prec (default " << prec
            << ") to specify precision of fit and data output\n";
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
      else {
         filename = string("stdin");
         pin = &cin;
      }

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
         stripTrailing(line,'\r');
         // remove leading whitespace
         line = stripLeading(line,string(" "));
         // skip comments
         if(line[0] == '#') continue;
         //check that column col is there...
         if(numWords(line) < col) { nd++; continue; }
         // pull it out
         stuff = word(line,col-1);
         // is it a number?
         if(!(isDecimalString(stuff))) { nd++; continue; }
         // convert it to double and save it
         d = asDouble(stuff);
         if(domin && d <= min) continue;
         if(domax && d >= max) continue;
         data.push_back(d);
         S.Add(d);

         // do the same for xcol
         if(xcol > -1) {
            if(numWords(line) < xcol)
               { data.pop_back(); nxd++; continue; }
            stuff = word(line,xcol-1);
            if(!(isDecimalString(stuff)))
               { data.pop_back(); nxd++; continue; }
            xd = asDouble(stuff);
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
      //   cout << " " << setprecision(prec) << data[i];
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
            cout << " Coefficients:" << setprecision(prec);
            for(i=0; i<fit; i++) {
               if(fabs(coef[i]) < 0.001)
                  cout << " " << scientific;
               else
                  cout << " " << fixed;
               cout << coef[i];
            }
            cout << endl << fixed << setprecision(prec);
            cout << " Offsets: Y(col " << col << ") " << savedata[0]
               << " X(col " << xcol << ") " << xdata[0] << endl;

            // output to file rstats.out
            pout = new ofstream("rstats.out");
            if(pout->fail()) {
               cout << "Unable to open file rstats.out - output to screen\n";
               pout = &cout;
            }
            else {
               cout << "Output polynomial fit to file rstats.out\n";
               //cout << " try `plot rstats.out -x 1 -y 2,data,points -y 3,fit,lines"
               //<< " -y 4,residuals -y2 5,weights --y2range -0.1:1.1 "
               //<< "-t \"Robust fit (degree " << fit
               //<< "), output of rstats for file " << filename << "\"`" << endl;
               cout << "try the command plot rstats.out -x 1 -y 4,residuals "
                  << "-y2 2,data,points -y2 3,fit,lines -xl X -yl Residuals "
                  << "-y2l \"Data and fit\" -t \"Robust fit (degree " << fit
                  << "), output of rstats for file " << filename << "\"" << endl;
            }
            t0 = xdata[0];
            *pout << "#Xdata, Data, fit, resid, weight (" << data.size() << " pts):"
               << fixed << setprecision(prec) << endl;
            for(i=0; i<data.size(); i++) {
               eval = savedata[0] + coef[0];
               tt = xdata[i]-t0;
               for(j=1; j<fit; j++) { eval += coef[j]*tt; tt *= (xdata[i]-t0); }
               *pout << fixed << setprecision(prec)
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
      // outlier limit (high) 2.5Q3-1.5Q1
      // outlier limit (low ) 2.5Q1-1.5Q3
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
               string("Robust stats for column ") + asString(col) + string(" of ")
               + filename);
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

      if(out) {
         double OH = Q3 + out*1.5*(Q3-Q1); // normally 2.5*Q3 - 1.5*Q1;
         double OL = Q1 - out*1.5*(Q3-Q1); // normally 2.5*Q1 - 1.5*Q3;
         vector<int> outhi,outlo;
         for(i=0; i<data.size(); i++) {
            if(data[i] > OH)
               outhi.push_back(i);
            else if(data[i] < OL)
               outlo.push_back(i);
         }
         cout << "There are " << outhi.size()+outlo.size() << " outliers; "
            << outlo.size() << " low (< " << setprecision(prec) << OL << ") and "
            << outhi.size() << " high (> " << setprecision(prec) << OH << ")."
            << endl;
         for(i=0; i<outlo.size(); i++)
            cout << " OTL " << fixed << setprecision(prec)
                << xdata[outlo[i]] << " " << data[outlo[i]] << endl;
         for(i=0; i<outhi.size(); i++)
            cout << " OTH " << fixed << setprecision(prec)
                << xdata[outhi[i]] << " " << data[outhi[i]] << endl;
      }

      if(qplot) {
         xdata.resize(data.size());    // replace xcol data with quantiles.
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

      // compute and print run time
      totaltime = clock()-totaltime;
      cout << "rstats timing: " << fixed << setprecision(3)
         << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.";

      return 0;
   }
   catch(Exception& e) { cout << "GPSTk Exception : " << e.what(); }
   catch(exception& e) { cout << "standard exception : " << e.what(); }
   catch (...) { cout << "Unknown error." << endl; }
   return -1;
}   // end main()
