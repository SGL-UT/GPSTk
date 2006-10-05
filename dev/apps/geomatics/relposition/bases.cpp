// bases.cpp  Read positions from the input file and compute all baselines

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

#include "StringUtils.hpp"
#include "Position.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char **argv)
{
   try {
      bool ok;
      int i,j,col[4]={0,1,2,3},llen;
      string filename,stuff;
      vector<Position> positions;
      vector<string> labels;

      for(i=1; i<argc; i++) {
         if(argv[i][0] == '-') { // && argv[i][1] == '-')
            string arg(argv[i]);
            if(arg == string("--help") || arg == string("-h")) {
               cout << "Usage: bases [<file>]\n";
               cout << "   Read positions from input file and compute baselines\n";
               // TD add options to rearrainge columns
               return -1;
            }
            else if(arg == string("--col") || arg == string("-c")) {
               //col = atoi(argv[++i]);
               //cout << "Column is " << col << endl;
            }
            else if(arg == string("--plot") || arg == string("-p")) {
               //plot = 1;
               //cout << "Turn on plot" << endl;
            }
            else {
               cout << "Ignore unknown option: " << arg << endl;
            }
         }
         else {
            filename = string(argv[i]);
            //cout << "Filename is " << filename << endl;
         }
      }

      istream *pin;                 // do it this way for Windows...
      if(!filename.empty()) {
         pin = new ifstream(filename.c_str());
         if(pin->fail()) {
            cout << "Could not open file " << filename << " .. abort.\n";
            return -2;
         }
         else cout << "Open file " << filename << endl;
      }
      else pin = &cin;

      const int BUFF_SIZE=1024;
      char buffer[BUFF_SIZE];

      llen = -1;
      while(pin->getline(buffer,BUFF_SIZE)) {
         //if(buffer[0] == '#') continue;
         string line = buffer;
         StringUtils::stripTrailing(line,'\r');
         // remove leading whitespace
         line = StringUtils::stripLeading(line,string(" "));
         // skip comments
         if(line[0] == '#') continue;
         string label;
         double xyz[3];
         for(ok=true,i=0; i<4; i++) {
            //check that column is there
            if(StringUtils::numWords(line) < col[i]) { ok=false; break; }
            // pull it out
            stuff = StringUtils::word(line,col[i]);
            if(i == 0) {                  // label
               label = stuff;
            }
            else {
               // is it a number?
               if(!(StringUtils::isDecimalString(stuff))) { ok=false; break; }
               // convert it to double and save it
               xyz[i-1] = StringUtils::asDouble(stuff);
            }
         }
         if(!ok) continue;
         Position pos(xyz[0],xyz[1],xyz[2]);
         positions.push_back(pos);
         if(int(label.length()) > llen) llen=label.length();
         labels.push_back(label);
      }

      for(i=0; i<positions.size(); i++) {
         cout << StringUtils::rightJustify(labels[i],llen)
            << " " << StringUtils::rightJustify(positions[i].printf("%16.6x %16.6y %16.6z"),50)
            << " = "
            << StringUtils::rightJustify(positions[i].printf("(%9.6A N  %10.6L E  %12.6h m)"),45)
            << endl;
      }
      cout << endl;
      for(i=0; i<positions.size(); i++) {
         for(j=i+1; j<positions.size(); j++) {
            Position p = positions[i]-positions[j];
            cout << StringUtils::rightJustify(labels[i],llen)
               << " - " << StringUtils::rightJustify(labels[j],llen)
               << " " << p.printf("%16.6x %16.6y %16.6z")
               << " " << fixed << setw(16) << setprecision(6) << range(positions[i],positions[j])
               << endl;
         }
      }

      if(pin != &cin) {
         ((ifstream *)pin)->close();
         delete pin;
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
