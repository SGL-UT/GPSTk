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

/**
 * @file mergeSRI.cpp
 * Read files containing SRIs with labels, merge into a single SRI.
 */

//------------------------------------------------------------------------------------
#include <iostream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include "StringUtils.hpp"
#include "SRI.hpp"

using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// Read all the files on the command line, they should contain covariance and state
// with labels. Merge all these SRIs and output the final covariance and state.
int main(int argc, char **argv)
{
   try {
      bool verbose=false;
      int i,n,N,nfile,nline,nword;
      string line,word;
      Matrix<double> cov;
      Vector<double> state;
      Namelist name;
      SRI S;

      if(argc <= 1) {
         cout << "Prgm mergeSRI combines solution and covariance results from "
              << "different sources\n   into a single result. Each file named on the "
              << "command line consists of lines,\n   one per row of the covariance "
              << "matrix, of the form\n      label(i) cov(i,0) cov(i,1) ... cov(i,n) "
              << "solution(i)\n   where there are n lines in the file (i.e. the "
              << "covariance matrix is square)\n   and labels are used consistently "
              << "among all the results in all the files.\n   Results are output as "
              << "a single combined namelist, covariance and solution.\n";
         return 0;
      }

      nfile = 0;
      for(i=1; i<argc; i++) {
         if(string(argv[i]) == string("-v") ||
            string(argv[i]) == string("--verbose")) {
            verbose = true;
            continue;
         }

         ifstream ifs(argv[i]);
         if(!ifs) {
            cout << "Could not open file " << argv[i] << endl;
            continue;
         }
         if(verbose) cout << "Opened file " << argv[i] << endl;

         // read the file
         N = nline = 0;            // N is the dimension of cov and state and name
         while(!ifs.eof() && ifs.good()) {
            getline(ifs,line);
            StringUtils::stripTrailing(line,'\r');
            if(ifs.bad()) break;
            StringUtils::stripLeading(line);
            if(line.empty()) break;
            n = StringUtils::numWords(line);
            if(N == 0) {
               N = n-2;
               cov = Matrix<double>(N,N,0.0);
               state = Vector<double>(N,0.0);
               name.clear();
            }
            else if(n-2 != N) {
               cerr << "Warning - dimensions are wrong in file " << argv[i]
                  << " : " << n-2 << " != " << N << endl;
            }
            nword = 0;
            while(1) {
               word = StringUtils::stripFirstWord(line);
               if(word.empty()) break;
               if(nword == 0) {
                  name += word;
               }
               else if(nword < N+1) {
                  cov(nline,nword-1) = StringUtils::asDouble(word);
               }
               else if(nword == N+1) {
                  state(nline) = StringUtils::asDouble(word);
               }
               nword++;
            };
            nline++;
            if(nline > N) break;
         }
         ifs.close();

         if(N <= 0 || name.size() <= 0) {
            cout << "Empty file - ignore : " << argv[i] << endl;
            continue;
         }

         name.resize(N);
         cout << "Add file " << argv[i] << " : state names " << name << endl;
         if(verbose) {
            LabelledVector Lstate(name,state);
            Lstate.fixed().setw(16).setprecision(6);
            cout << "State" << endl << Lstate << endl;
            LabelledMatrix Lcov(name,cov);
            Lcov.scientific().setw(16).setprecision(6);
            cout << "Covariance" << endl << Lcov << endl;
         }

         SRI S1(name);
         S1.addAPriori(cov,state);
         S += S1;

         nfile++;
      }

      if(nfile <= 0) {
         cout << "No files!\n";
         return 0;
      }

      double small,big;
      S.getStateAndCovariance(state,cov,&small,&big);
      cout << endl;
      LabelledVector Ls(name,state);
      Ls.fixed().setw(16).setprecision(6);
      cout << "Final state" << endl << Ls << endl;
      LabelledMatrix Lc(name,cov);
      Lc.scientific().setw(16).setprecision(6);
      cout << endl << "Final covariance" << endl << Lc << endl;
   }
   catch(MatrixException& me) {
      cerr << "Exception: " << me << endl;
      return -1;
   }
   return 0;
}
