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
//  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
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
// TD i don't think weighted stddev is right...
int main(int argc, char **argv)
{
   clock_t totaltime;            // for timing tests
   // begin counting time
   totaltime = clock();
   try {
      bool help,nostats=false,plot=false,qplot=false,domin=false,domax=false;
      bool doKS=false,dobeg=false,doend=false,doseq=false;
      int i,j,col=1,xcol=-1,fit=0,prec=3,brief=0;
      double out=0.0,min,max,beg,end;
      string filename,label=string();
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
            else if(arg == string("--seq") || arg == string("-s")) {
               doseq = true;
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
            else if(arg == string("--beg")) {
               if(i==argc-1) return BadOption(arg);
               beg = atof(argv[++i]);
               dobeg = true;
            }
            else if(arg == string("--end")) {
               if(i==argc-1) return BadOption(arg);
               end = atof(argv[++i]);
               doend = true;
            }
            else if(arg == string("--KS")) {
               doKS = true;
            }
            else if(arg == string("--label") || arg == string("-l")) {
               if(i==argc-1) return BadOption(arg);
               label = string(argv[++i]);
            }
            else if(arg.substr(0,7) == string("--brief") ||
                    arg.substr(0,2) == string("-b")) {
               if(arg == string("--brief") || arg == string("-b")) brief=1;
               else if(arg == string("--briefc") || arg == string("-bc")) brief=1;
               else if(arg == string("--briefw") || arg == string("-bw")) brief=2;
               else if(arg == string("--briefr") || arg == string("-br")) brief=3;
               else if(arg == string("--brief2") || arg == string("-b2")) brief=4;
               else cout << "Ignore unknown option: " << arg << endl;
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
         cout <<
"Usage: rstats [options] <file>\n"
"  Compute standard and robust statistics on numbers in one column of <file>.\n"
"  Options (default):\n"
"   --col <c>   use data in column <c> (1)\n"
"   --xcol <cx> specify another column, and output 2-sample stats ()\n"
"   --beg <b>   include only data that satisfies x > b\n"
"   --end <e>   include only data that satisfies x < e\n"
"   --min <lo>  include only data that satisfies d > lo\n"
"   --max <hi>  include only data that satisfies d < hi\n"
"   --plot      generate a stem-and-leaf plot\n"
"   --qplot     generate data for a quantile-quantile plot\n"
"                (data written to file qplot.out)\n"
"   --fit <f>   fit a robust polynomial of degree <f> (>0) to data,\n"
"                using xcol as independent variable, output in rstats.out\n"
"   --seq       output data, in input order, with sequential stats\n"
"   --nostats   supress total stats output (for --fit and --seq)\n"
"   --outs <s>  explicitly list all data outside s*outlier limits\n"
"   --prec <p>  specify precision of fit and data outputs (" << prec << ")\n"
"   --KS        output the Anderson-Darling statistic (a form of the KS-test),\n"
"                where AD>0.752 means non-normal\n"
"   --label <l> add label l to the output\n"
"   --brief     brief output (conventional stats)\n"
"                (use --briefw for weighted, --briefr for robust --brief2 for 2-sample)\n"
"   --help      print this and quit\n"
         ;
         return -1;
      }

      if(fit > 0 && xcol == -1) {
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
      if(brief) {
         ;
      }
      else {
         cout << "rstats for ";
         if(pin == &cin) cout << "data from stdin";
         else            cout << "file: " << filename;
         cout << ", stats (col " << col << ")";
         if(xcol > -1) {
            cout << " and 2-sample stats (x-col " << xcol << ")";
            if(fit > 0) {
               cout << ", fit (" << fit << ")";
               if(nostats) cout << " (but no stats)";
            }
         }
         cout << endl;
      }

      const int BUFF_SIZE=1024;
      char buffer[BUFF_SIZE];
      int nd,nxd;
      double d,xd;
      string stuff;
      vector<double> data,wts,xdata;
      Stats<double> cstats;
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
         // tab separated
         change(line,"\t"," ");
         // check that column col is there...
         if(numWords(line) < col) { nd++; continue; }
         // pull it out
         stuff = word(line,col-1);
         // is it a number?
         if(!(isScientificString(stuff))) { nd++; continue; }
         // convert it to double and save it
         d = asDouble(stuff);
         if(domin && d <= min) continue;
         if(domax && d >= max) continue;

         // do the same for xcol
         if(xcol > -1) {
            if(numWords(line) < xcol)
               { data.pop_back(); nxd++; continue; }
            stuff = word(line,xcol-1);
            if(!(isScientificString(stuff)))
               { data.pop_back(); nxd++; continue; }
            xd = asDouble(stuff);
            if(dobeg && xd <= beg) continue;
            if(doend && xd >= end) continue;
            TSS.Add(xd, d);
            xdata.push_back(xd);
         }

         data.push_back(d);
         cstats.Add(d);
      }

      if(pin != &cin) {
         ((ifstream *)pin)->close();
         delete pin;
      }

      // check that input was good
      if(data.size() < 2) {
         cout << "Abort: not enough data: " << data.size() << " data read";
         if(nd > 0) cout << " [data(col) not found on " << nd << " lines]";
         if(nxd > 0) cout << " [data(xcol) not found on " << nxd << " lines]";
         cout << "." << endl;
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
      //} cout << endl;
      //
      //cout << "Conventional median is " << median(data) << endl;

      // process fit
      if(fit > 0) {
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
                  << "-y2 2,data,points -y2 3,fit,lines -xl X -yl Residuals \\\n     "
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

      if(doseq) {
         cstats.Reset();
         cout << fixed << setprecision(prec);
         for(i=0; i<data.size(); i++) {
            cstats.Add(data[i]);
            if(!label.empty()) cout << label << " ";
            if(xdata.size()>0) cout << xdata[i] << " ";
            cout << data[i] << "   " << cstats.N() << " "
                 << cstats.Average() << " "
                 << (cstats.N() > 1 ? cstats.StdDev() : 0.0) << endl;
         }
         if(nostats) return 0;
      }

      double median,mad,mest,Q1,Q3,KS;
      QSort(&data[0],data.size());
      Robust::Quartiles(&data[0],data.size(),Q1,Q3);
      // outlier limit (high) 2.5Q3-1.5Q1
      // outlier limit (low ) 2.5Q1-1.5Q3
      mad = Robust::MedianAbsoluteDeviation(&data[0],data.size(),median);
      wts.resize(data.size());
      mest = Robust::MEstimate(&data[0], data.size(), median, mad, &wts[0]);

      cout << fixed << setprecision(prec);

      if(brief==1 || brief==4)
         cout << "rstats(con):" << (label.empty() ? "" : " "+label)
            << " N " << cstats.N()
            << "  Ave " << setw(prec+3) << cstats.Average()
            << "  Std " << setw(prec+3) << cstats.StdDev()
            << "  Var " << setw(prec+3) << cstats.Variance()
            << "  Min " << setw(prec+3) << cstats.Minimum()
            << "  Max " << setw(prec+3) << cstats.Maximum()
            << "  P2P " << setw(prec+3) << cstats.Maximum()-cstats.Minimum()
            << endl;
      else if(brief==0)
         cout << "Conventional statistics:\n" << cstats << endl;

      if(doKS) {
         KS = ADtest(&data[0],data.size(),cstats.Average(),cstats.StdDev(),false);
	      cout << " KS test = " << setprecision(prec) << KS << endl;
      }

      cstats.Reset();
      for(i=0; i<data.size(); i++) {
         //cout << "WTD " << i << fixed << setprecision(8)
            //<< " " << data[i] << " " << wts[i] << endl;
         cstats.Add(data[i],wts[i]);
      }

      if(brief==2) cout << "rstats(wtd)" << (fit==0 ? "" : "(fit resid)")
         << ":" << (label.empty() ? "" : " "+label)
         << " N " << cstats.N()
         << "  Ave " << setw(prec+3) << cstats.Average()
         << "  Std " << setw(prec+3) << cstats.StdDev()
         << "  Var " << setw(prec+3) << cstats.Variance()
         << "  Min " << setw(prec+3) << cstats.Minimum()
         << "  Max " << setw(prec+3) << cstats.Maximum()
         << "  P2P " << setw(prec+3) << cstats.Maximum()-cstats.Minimum()
         << endl;
      else if(brief==0)
         cout << "Conventional statistics with robust weighting"
            << (fit==0 ? "" : " (fit resids)") << ":\n"
            << fixed << setprecision(prec) << cstats << endl;

      if(xcol > -1) {
         if(brief==4) cout << "rstats(2sm)"
            << ":" << (label.empty() ? "" : " "+label)
            << " N " << data.size()
            //<< " VarX " << setprecision(prec) << TSS.VarianceX()
            //<< " VarY " << setprecision(prec) << TSS.VarianceY()
            << "  Int " << setprecision(prec) << TSS.Intercept()
            << "  Slp " << setprecision(prec) << TSS.Slope()
            << " +- " << setprecision(prec) << TSS.SigmaSlope()
            << "  CSig " << setprecision(prec) << TSS.SigmaYX()
            << "  Corr " << setprecision(prec) << TSS.Correlation()
            << endl;
         else cout << "Two-sample statistics:\n" << setprecision(prec) << TSS << endl;
      }
      
      if(brief==3) cout << "rstats(rob)" << (fit==0 ? "" : "(fit resid)")
         << ":" << (label.empty() ? "" : " "+label)
         << " N " << data.size()
         << "  Med " << setw(prec+3) << median << "  MAD " << mad
         << "  Min " << setw(prec+3) << cstats.Minimum()
         << "  Max " << cstats.Maximum()
         << "  P2P " << setw(prec+3) << cstats.Maximum()-cstats.Minimum()
         << "  Q1 " << setw(prec+3) << Q1 << "  Q3 " << setw(prec+3) << Q3
         << endl;
      else if(brief==0) {
         cout << "Robust statistics" << (fit==0 ? "" : "(fit resids)") << ":\n";
	      cout << " Number    = " << data.size() << endl;
	      cout << " Quartiles = " << setw(11) << setprecision(prec) << Q1
                        << " " << setw(11) << setprecision(prec) << Q3 << endl;
	      cout << " Median    = " << setw(11) << setprecision(prec) << median << endl;
	      cout << " MEstimate = " << setw(11) << setprecision(prec) << mest << endl;
	      cout << " MAD       = " << setw(11) << setprecision(prec) << mad << endl;
      }

      if(plot) {
         try {
            Robust::StemLeafPlot(cout, &data[0], data.size(),
            string("Robust stats for column ")+asString(col)+string(" of ")+filename);
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
      if(brief==0) cout << "rstats timing: " << fixed << setprecision(3)
         << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds." << endl;

      return 0;
   }
   catch(Exception& e) { cout << "GPSTk Exception : " << e.what(); }
   catch(exception& e) { cout << "standard exception : " << e.what(); }
   catch (...) { cout << "Unknown error." << endl; }
   return -1;
}   // end main()
