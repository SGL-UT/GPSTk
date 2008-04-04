/**
 * @file lsfilt.cpp
 * Read the data in one column of a file, and output a least-squares filtered
 * version of the data.
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>

#include "StringUtils.hpp"
#include "Matrix.hpp"

using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// Least squares (Savitzky-Golay) filter.
// Given a data stream sampled at even spacing dt; i.e. given data
// f(i) at ti=t0+i*dt, with i=...-2,-1,0,1,2,...,
// a smoothed version of the data is produced by the digital filter
// 			 nr
// 	g(i) = Sum C(n) * f(i+n)
// 			n=-nl
// This routine computes the filter coefficients C(n) such that the filter
// output at each point is the value of a least-squares-fit polynomial to
// the nl+nr+1 data points surrounding the point.  Derivatives of the data
// stream may also be computed with this routine; then g = dt**N * Nth deriv,
// where N=lderiv.
// 
// Usual values are m=2,4; nl+nr = 1-2 times desired feature widths in data.
// Coefficients c[0,nl+nr] are in order -nl,...0...nr.
//------------------------------------------------------------------------------------
// c must be of length nl+nr+1. nl(nr) is the number of points before(after) the
// smoothed point that are included in the fit. poly is the polynomial order,
// typically 2 or 4, lderiv is the derivative order - lderiv>0 to compute a deriv.
int lsfilterCoefficients(double* c, int nl, int nr, int poly, int lderiv)
{
	if(poly <= 0 || nl < 0 || nr < 0 || lderiv > poly || nl+nr < poly)
      return -1;
   try {
	   int i,k,j,jj;
	   double sum,fac;

	   Matrix<double> LU(poly+1,poly+1,0.0); // Matrix lu=SquareMatrix(poly+1);

	   for(i=0; i<=(poly<<1); i++) {
		   sum = (i ? 0.0 : 1.0);
		   for(k=0; k<nr; k++) sum += ::pow(double(k+1), double(i));
		   for(k=0; k<nl; k++) sum += ::pow(double(-k-1), double(i));
		   if(i < 2*poly-i) jj = i; else jj = 2*poly-i;
		   for(j = -jj; j<=jj; j+=2) LU((i+j)/2,(i-j)/2)=sum;
	   }

	   LUDecomp<double> LUD;
      LUD(LU);
	   Vector<double> rhs(poly+1,0.0); //Vector rhs=Vector(poly+1);
	   rhs(lderiv) = 1.0;
	   LUD.backSub(rhs);

	   for(k=-nl; k<=nr; k++) {
		   sum = rhs(0);
		   fac = 1.0;
		   for(j=0; j<poly; j++) sum += rhs(j+1)*(fac *= k);
		   c[k+nl] = sum;
	   }
   
	   return 0;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e) {
      Exception E("std except: " + string(e.what()));
      GPSTK_THROW(E);
   }
   catch(...) {
      Exception e("Unknown exception");
      GPSTK_THROW(e);
   }
}

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
      bool help,domin=false,domax=false,verbose=true;
      int i,j,k,col=1,xcol=-1,deg=11,npts=99,prec=3;
      double min,max;
      string filename;
      ostream *pout;

      help = (argc <= 1);
      for(i=1; i<argc; i++) {
         if(argv[i][0] == '-') { // && argv[i][1] == '-')
            string arg(argv[i]);
            if(arg == string("--help") || arg == string("-h"))
               help = true;
            else if(arg == string("--col")
                 || arg == string("-c") || arg == string("-y")) {
               if(i==argc-1) return BadOption(arg);
               col = atoi(argv[++i]);
            }
            else if(arg == string("--xcol") || arg == string("-x")) {
               if(i==argc-1) return BadOption(arg);
               xcol = atoi(argv[++i]);
            }
            else if(arg == string("--deg") || arg == string("-d")) {
               if(i==argc-1) return BadOption(arg);
               deg = atoi(argv[++i]);
            }
            else if(arg == string("--npts") || arg == string("-n")) {
               if(i==argc-1) return BadOption(arg);
               npts = atoi(argv[++i]);
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
            else if(arg == string("--prec") || arg == string("-p")) {
               if(i==argc-1) return BadOption(arg);
               prec = atoi(argv[++i]);
            }
            else if(arg == string("--quiet") || arg == string("-q"))
               verbose = false;
            else {
               cout << "Ignore unknown option: " << arg << endl;
            }
         }
         else {
            filename = string(argv[i]);
         }
      }

      if(help) {
         cout << "Usage: lsfilt [filename] [options]\n";
         cout << " Apply a least-squares filter to the data in column <col> "
            << "(default 1)\n"
            << "     of file <filename> (else redirect stdin).\nOptions (default):\n";
         cout << "   --col <c>  use data from column c of the input (" <<col<< ")\n";
         cout << "   --deg <d>  degree of fit (typically 3 or 5) (" << deg << ")\n";
         cout << "   --npts <n> number of points in fit (choose 1-2x width of "
            << "features in data) (" << npts << ")\n";
         cout << "   --min <lo> include only data that satisfies d > lo\n";
         cout << "   --max <hi> include only data that satisfies d < hi\n";
         cout << "   --prec <p> specify precision of data output "
            << "(" << prec << ")\n";
         cout << "   --quiet    output data only\n";
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
      if(verbose) {
         cout << "lsfilt for ";
         if(pin == &cin) cout << "data from stdin";
         else            cout << "file: " << filename;
         cout << ", col " << col << ", deg " << deg << ", npts " << npts
            << ", prec " << prec << "";
         if(domin) cout << ", min " << min << "";
         if(domax) cout << ", max " << max << "";
         cout << endl;
      }

      const int BUFF_SIZE=1024;
      char buffer[BUFF_SIZE];
      int nd,nxd;
      double d,xd;
      string stuff;
      vector<double> data,xdata,fdata;

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
         if(domin && d <= min) continue;
         if(domax && d >= max) continue;
         data.push_back(d);

         // do the same for xcol
         if(xcol > -1) {
            if(StringUtils::numWords(line) < xcol)
               { data.pop_back(); nxd++; continue; }
            stuff = StringUtils::word(line,xcol-1);
            if(!(StringUtils::isDecimalString(stuff)))
               { data.pop_back(); nxd++; continue; }
            xd = StringUtils::asDouble(stuff);
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

      // generate coefficients
      int nl,nr,order=deg-1,deriv=0;
      if(npts % 2) nl = nr = npts/2;
      else nl = nr = (npts-1)/2;
      vector<double> coeffs(nl+nr+1,0.0),cshort;
      i = lsfilterCoefficients(&coeffs[0], nl, nr, order, deriv);
      if(i) {
         cout << "Failed to generate coefficients (" << i << "). Abort.\n";
         exit(1);
      }

      //cout << "Coefficients (" << coeffs.size() << "):" << endl
      //   << fixed << setprecision(prec);
      //for(i=0; i<coeffs.size(); i++)
      //   cout << " " << setw(2) << i << " " << coeffs[i] << endl;

      cout << fixed << setprecision(prec);
      for(i=0; i<data.size(); i++) {
         int NL = i<nl ? i : nl;
         int NR = i+nr > data.size()-1 ? data.size()-i-1 : nr;
         double *ptr = &coeffs[0];
         if(NL != nl || NR != nr) {                // data does not cover i-nl to i+nr
            cshort = vector<double>(NL+NR+1,0.0);
            ptr = &cshort[0];
            j = lsfilterCoefficients(ptr, NL, NR, order, deriv);
            if(j) continue;
         }

         // compute the filtered value at i
         d = 0.0;
         for(k=-NL; k<=NR; k++) {
            j = i+k;
            //if(j < 0 || j >= data.size()) continue;
            d += *ptr * data[j];
            ptr++;
         }

         cout << " " << i;
         if(xcol > -1) cout << " " << xdata[i];
         cout << " " << data[i] << " " << d << " " << data[i]-d << endl;
      }

      // compute and print run time
      totaltime = clock()-totaltime;
      if(verbose) cout << "lsfilt timing: " << fixed << setprecision(3)
         << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.";

      return 0;
   }
   catch(Exception& e) { cout << "GPSTk Exception : " << e.what(); }
   catch(exception& e) { cout << "standard exception : " << e.what(); }
   catch (...) { cout << "Unknown error." << endl; }
   return -1;
}   // end main()
