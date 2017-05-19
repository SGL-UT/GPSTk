// ======================================================================
// This software was developed by Applied Research Laboratories, The
// University of Texas at Austin under contract to an agency or agencies
// within the U.S. Department of Defense. The U.S. Government retains all
// rights to use, duplicate, distribute, disclose, or release this
// software.
// 
// Copyright 2008 The University of Texas at Austin
// ======================================================================

/// @file OceanLoadTides.cpp

//------------------------------------------------------------------------------------
// system includes
#include <iostream>
#include <fstream>
#include <algorithm>
// GPSTk
#include "StringUtils.hpp"
#include "MiscMath.hpp"
#include "geometry.hpp"
// geomatics
#include "OceanLoadTides.hpp"
#include "RobustStats.hpp"       // for QSort
#include "CubicSpline.hpp"
//#include "logstream.hpp"         // TEMP

using namespace std;

namespace gpstk {

   using namespace StringUtils;

   // Number of standard (Schwiderski) tides read from BLQ file
   const int OceanLoadTides::NSTD=11;
   // Number of derived tides computed by deriveTides()
   const int OceanLoadTides::NDER=342;

   //---------------------------------------------------------------------------------
   // Open and read the given file, containing ocean loading coefficients, and
   // initialize this object for the sites names in the input list that match a
   // name in the file (case sensitive). Return the number of successfully
   // initialized site names, and remove those sites from the input list.
   // Ocean loading files can be obtained from the web. For example all the ITRF
   // sites are found at ftp://maia.usno.navy.mil/conventions/chapter7/olls25.blq
   // Also, at http://www.oso.chalmers.se/~loading one may submit site label and
   // position for one or more sites, and the resulting ocean loading file will be
   // emailed.
   // @param sites      vector<string> On input contains site labels found in the
   //                   file, on output contains only sites that were NOT found.
   //                   If sites is empty, all sites are loaded.
   // @param filename   string Input ocean loading file name.
   // @return the number of sites successfully initialized.
   // @throw if the file could not be opened.
   int OceanLoadTides::initializeSites(vector<string>& sites, string filename)
      throw(Exception)
   {
   try {
      bool allsites = false;
      if(sites.size() == 0) allsites = true; // return 0;
      int i,n;

      ifstream infile(filename.c_str());
      if(!infile || !infile.is_open()) {
         Exception e("File " + filename + " could not be opened.");
         GPSTK_THROW(e);
      }

      n = 0;         // number of successes
      bool looking=true;                        // true if looking for a site name
      double lat,lon;
      vector<double> coeff;
      string site;
      while(1) {                                // read the file
         int count;
         string line,word;

         // get the next line
         getline(infile,line);
         stripTrailing(line,'\r');

         // process line
         if(!line.empty()) {
            word = firstWord(line);
            //LOG(VERBOSE) << "Word is " << word << " and line is " << line;

            if(word == "$$") {         // NB ignore header - assume column order, etc.
               // pick out the lat/lon
               if(!looking) {
                  while(!line.empty()) {
                     word = stripFirstWord(line);
                     if(word == string("lon/lat:")) {
                        lon = asDouble(stripFirstWord(line));
                        lat = asDouble(stripFirstWord(line));
                        break;
                     }
                  }
               }
            }
            // TD should test be line length <= 21 ? ... what if site name = number
            //else if(looking && !isDecimalString(word)) {
            else if(looking && line.length() <= 21) {
               // site name
               site = line;
               stripTrailing(site,string("\n"));
               stripTrailing(site,string("\r"));
               stripTrailing(site);
               stripLeading(site);
               //LOG(VERBOSE) << "Found site " << site;
               if(allsites) {
                  //LOG(VERBOSE) << "Push back " << site;
                  looking = false;
                  sites.push_back(site);
               }
               else for(i=0; i<sites.size(); i++) {
                  //LOG(VERBOSE) << "Compare " << sites[i];
                  if(site == sites[i]) {
                     looking = false;
                     break;
                  }
               }
               if(!looking) {          // found a site
                  count = 0;
                  coeff.clear();
                  lat = lon = 0.0;
               }
            }
            else if(!looking) {        // not comment and not looking - must be data
               if(numWords(line) != 11) {
                  Exception e("File " + filename + " is corrupted for site " + site
                        + " - offending line follows\n" + line);
                  GPSTK_THROW(e);
               }
               //LOG(VERBOSE) << "Push back line " << line;
               for(i=0; i<11; i++)
                  coeff.push_back(
                     asDouble(stripFirstWord(line)));
               count++;
               if(count == 6) {        // success
                  ostringstream oss;
                  oss << fixed;
                  for(i=0; i<coeff.size(); i++) {
                     if(i < 33) oss << " " << setprecision(5) << setw(7) << coeff[i];
                     else       oss << " " << setprecision(1) << setw(7) << coeff[i];
                     if((i+1)%11 == 0) oss << "\n";
                  }
                  //LOG(VERBOSE) << "  Found site " << site << " with coefficients:";
                  //LOG(VERBOSE) << oss.str();

                  // update coeff map
                  coefficientMap[site] = coeff;
                  n++;
                  // update position map
                  coeff.clear();
                  coeff.push_back(lat);
                  coeff.push_back(lon);
                  positionMap[site] = coeff;

                  // erase a vector element
                  if(!allsites) {
                     vector<string>::iterator pos;
                     pos = find(sites.begin(),sites.end(),site);
                     if(pos != sites.end()) sites.erase(pos);
                  }
                  looking = true;
               }
            }

         }  // end if line not empty

         if(infile.eof() || !infile.good()) break;

      }  // end loop over lines in the file

      return n;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
   catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
   }

   //---------------------------------------------------------------------------------
   // Compute the site displacement vector at the given time for the given site.
   // The site must have been successfully initialized; if not an exception is
   // thrown.
   // @param site  string Input name of the site; must be the same as previously
   //              successfully passed to initializeSites().
   // @param t     EphTime Input time of interest.
   // @return Triple containing the North, East and Up components of the site
   //                displacement in meters.
   // @throw if the site has not been initialized.
   Triple OceanLoadTides::computeDisplacement11(string site, EphTime time)
      throw(Exception)
   {
   try {
      if(!isValid(site)) {
         Exception e("Site " + site + " has not been initialized.");
         GPSTK_THROW(e);
      }

      // get the coefficients for this site
      vector<double> coeff = coefficientMap[site];

      // get the astronomical arguments in radians
      double angles[11];
      //inline this SchwiderskiArg(int(t.year())-1900, t.DOY(), t.secOfDay(), angles);
      {
         double fday(time.secOfDay());
         long jday(static_cast<long>(time.lMJD() + MJD_JDAY + fday/SEC_PER_DAY));
         int iyear,imm,iday;
         convertJDtoCalendar(jday,iyear,imm,iday);
         iyear -= 1900;

         // ordering is: M2, S2, N2, K2, K1, O1, P1, Q1, Mf, Mm, Ssa
         // which are : { semi-diurnal }{   diurnal    }{long-period}
         static const double speed[11] = {
            1.40519E-4, 1.45444E-4, 1.37880E-4, 1.45842E-4,
            0.72921E-4, 0.67598E-4, 0.72523E-4, 0.64959E-4,
            0.053234E-4, 0.026392E-4, 0.003982E-4 };
         static const double angfac[44] =
         {
                                    // sun
            2.0,  0.0,  2.0,  2.0,  //  4 : M2, S2, N2, K2
            1.0,  1.0, -1.0,  1.0,  //  8 : K1, O1, P1, Q1
            0.0,  0.0,  2.0,        // 11 : Mf, Mm, Ssa
                                    // moon
           -2.0,  0.0, -3.0,  0.0,  // 15 : M2, S2, N2, K2
            0.0, -2.0,  0.0, -3.0,  // 19 : K1, O1, P1, Q1
            2.0,  1.0,  0.0,        // 22 : Mf, Mm, Ssa
                                    // lunar perigee
            0.0,  0.0,  1.0,  0.0,  // 26 : M2, S2, N2, K2
            0.0,  0.0,  0.0,  1.0,  // 30 : K1, O1, P1, Q1
            0.0, -1.0,  0.0,        // 33 : Mf, Mm, Ssa
                                    // two pi
            0.0,  0.0,  0.0,  0.0,  // 37 : M2, S2, N2, K2
            0.25,-0.25,-0.25,-0.25, // 41 : K1, O1, P1, Q1
            0.0,  0.0,  0.0         // 44 : Mf, Mm, Ssa
         };

         int icapd = iday + 365*(iyear-75)+((iyear-73)/4);

         //double capt = (27392.500528+1.000000035*double(icapd))/36525.0;
         double capt = 0.74996579132101300 + 2.73785088295687885e-5 * double(icapd);

         // mean longitude of sun at beginning of day
         double H0 = 279.69668+(36000.768930485+0.000303*capt)*capt;

         // mean longitude of moon at beginning of day
         double S0 = ((0.0000019*capt-0.001133)*capt+481267.88314137)*capt+270.434358;

         // mean longitude of lunar perigee at beginning of day
         double P0 = ((-0.000012*capt-0.010325)*capt+4069.0340329577)*capt+334.329653;

         // convert to radians
         //static const double dtr = 0.0174532925199;
         H0 *= DEG_TO_RAD;
         S0 *= DEG_TO_RAD;
         P0 *= DEG_TO_RAD;

         //LOG(INFO) << "Schwiderski " << iday << " " << fixed << setprecision(5)
         //<< setw(11) << fday << " " << icapd << " " << capt
         //<< " " << H0 << " " << S0 << " " << P0;

         static const double twopi = 6.28318530718;
         for(int k=0; k<11; k++) {
            angles[k] = speed[k]*fday + angfac[k]*H0
                                    + angfac[11+k]*S0
                                    + angfac[22+k]*P0
                                    + angfac[33+k]*twopi;
            angles[k] = ::fmod(angles[k],twopi);
            if(angles[k] < 0.0) angles[k] += twopi;
         }
      }  // end SchwiderskiArg()

      // compute the radial, west and south components
      // coefficients are stored by rows: radial, west, south; first amp, then phase
      // column order same as in SchwiderskiArg() [ as in the file ]
      Triple dc;
      for(int i=0; i<3; i++) {         // components
         dc[i] = 0.0;
         for(int j=0; j<11; j++)       // tidal modes
            dc[i] += coeff[i*11+j]*::cos(angles[j]-coeff[33+i*11+j]*DEG_TO_RAD);
      }

      // convert radial,west,south to north,east,up
      double temp=dc[0];
      dc[0] = -dc[2];         // N = -S
      dc[1] = -dc[1];         // E = -W
      dc[2] = temp;           // U = rad

      return dc;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e) {
      Exception E("std except: " + string(e.what()));
      GPSTK_THROW(E);
   }
   catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
   }

   //---------------------------------------------------------------------------------
   // Compute the site displacement vector at the given time for the given site.
   // The site must have been successfully initialized; if not an exception is
   // thrown.
   // @param site  string Input name of the site; must be the same as previously
   //              successfully passed to initializeSites().
   // @param t     EphTime Input time of interest.
   // @return Triple containing the North, East and Up components of the site
   //                displacement in meters.
   // @throw if the site has not been initialized, if the time system is unknown,
   //                if there is corruption in the static arrays, or .
   Triple OceanLoadTides::computeDisplacement(string site, EphTime time)
      throw(Exception)
   {
      try {
         ostringstream oss;      // TEMP
         int i;

         if(!isValid(site)) {
            Exception e("Site " + site + " has not been initialized.");
            GPSTK_THROW(e);
         }

         // get the coefficients for this site
         vector<double> coeff = coefficientMap[site];

         // Cartwright-Tayler numbers of Scherneck tides
         // ordering is: M2, S2, N2, K2, K1, O1, P1, Q1, Mf, Mm, Ssa

         // standard 11 Scherneck tides:
         static const NVector SchInd[] = {
            { 2, 0, 0, 0, 0, 0 },         // M2
            { 2, 2,-2, 0, 0, 0 },         // S2
            { 2,-1, 0, 1, 0, 0 },         // N2
            { 2, 2, 0, 0, 0, 0 },         // K2
            { 1, 1, 0, 0, 0, 0 },         // K1
            { 1,-1, 0, 0, 0, 0 },         // O1
            { 1, 1,-2, 0, 0, 0 },         // P1
            { 1,-2, 0, 1, 0, 0 },         // Q1
            { 0, 2, 0, 0, 0, 0 },         // Mf
            { 0, 1, 0,-1, 0, 0 },         // Mm
            { 0, 0, 2, 0, 0, 0 },         // Ssa
         };

         // NB there must be 11 std tides in SchInd[]
         if((int)(sizeof(SchInd) / sizeof(NVector)) != NSTD) {
            Exception e("Static SchInd array is corrupted");
            GPSTK_THROW(e);
         }

         // compute time argument
         EphTime ttag(time);
         ttag.convertSystemTo(TimeSystem::UTC);
         double dayfr(ttag.secOfDay()/86400.0);
         ttag.convertSystemTo(TimeSystem::TT);
         // T = EarthOrientation::CoordTransTime()
         double T((ttag.dMJD() - 51544.5)/36525.0);

         // get the Delauney arguments and frequencies at t
         double Del[5], freqDel[5];       // degrees and cycles/day
         Del[0] =    134.9634025100 +     // EarthOrientation::L()
               T*(477198.8675605000 +
               T*(     0.0088553333 +
               T*(     0.0000143431 +
               T*(    -0.0000000680))));
         Del[1] =    357.5291091806 +     // EarthOrientation::Lp()
               T*( 35999.0502911389 +
               T*(    -0.0001536667 +
               T*(     0.0000000378 +
               T*(    -0.0000000032))));
         Del[2] =     93.2720906200 +     // EarthOrientation::F()
               T*(483202.0174577222 +
               T*(    -0.0035420000 +
               T*(    -0.0000002881 +
               T*(     0.0000000012))));
         Del[3] =    297.8501954694 +     // EarthOrientation::D()
               T*(445267.1114469445 +
               T*(    -0.0017696111 +
               T*(     0.0000018314 +
               T*(    -0.0000000088))));
         Del[4] =    125.0445550100 +     // EarthOrientation::Omega2003()
               T*( -1934.1362619722 +
               T*(     0.0020756111 +
               T*(     0.0000021394 +
               T*(    -0.0000000165))));
         for(i=0; i<5; i++) Del[i] = ::fmod(Del[i],360.0);
         freqDel[0] =  0.0362916471 + 0.0000000013*T;
         freqDel[1] =  0.0027377786;
         freqDel[2] =  0.0367481951 - 0.0000000005*T;
         freqDel[3] =  0.0338631920 - 0.0000000003*T;
         freqDel[4] = -0.0001470938 + 0.0000000003*T;

         // convert to Doodson (Darwin) variables
         double Dood[6], freqDood[6];
         Dood[0] = 360.0*dayfr - Del[3];
         Dood[1] = Del[2] + Del[4];
         Dood[2] = Dood[1] - Del[3];
         Dood[3] = Dood[1] - Del[0];
         Dood[4] = -Del[4];
         Dood[5] = Dood[2] - Del[1];
         for(i=0; i<6; i++) Dood[i] = ::fmod(Dood[i],360.0);

         freqDood[0] = 1.0 - freqDel[3];
         freqDood[1] = freqDel[2] + freqDel[4];
         freqDood[2] = freqDood[1] - freqDel[3];
         freqDood[3] = freqDood[1] - freqDel[0];
         freqDood[4] = -freqDel[4];
         freqDood[5] = freqDood[2] - freqDel[1];

         // find amplitudes and phases for vertical, west and south components,
         // for all 342 derived tides, from standard tides
         double amp[NSTD],phs[NSTD];
         double ampS[NDER],ampW[NDER],ampU[NDER];  // south,west,up component amp.s
         double phsS[NDER],phsW[NDER],phsU[NDER];  // south,west,up component phs.s
         double freq[NDER];                        // frequencies (same for S,W,U)

         // vertical
         int nder;         // number returned, may be < NDER
         for(i=0; i<NSTD; i++) {
            amp[i] = coeff[i];
            phs[i] = -coeff[33+i];
         }
         //oss.str(""); oss << fixed << setprecision(5) << "TEST Amp  1 ";
         //for(i=0; i<NSTD; i++) oss << " " << setw(8) << amp[i];
         //LOG(INFO) << oss.str();
         //oss.str(""); oss << fixed << setprecision(1) << "TEST Phs  1 ";
         //for(i=0; i<NSTD; i++) oss << " " << setw(8) << phs[i];
         //LOG(INFO) << oss.str();
         //LOG(INFO) << "TEST T,DAYFR,DELTA" << fixed << setprecision(15)
         //   << setw(25) << T << setw(25) << dayfr << setw(25) << ttag.secOfDay();
         //LOG(INFO) << "TEST Delauneys " << fixed << setprecision(15)
         //   << setw(22) << Del[0] << setw(22) << Del[1] << setw(22) << Del[2]
         //   << setw(22) << Del[3] << setw(22) << Del[4];
         //LOG(INFO) << "TEST Del freqs " << fixed << setprecision(15)
         //   << setw(22) << freqDel[0] << setw(22) << freqDel[1] << setw(22)
         //   << freqDel[2] << setw(22) << freqDel[3] << setw(22) << freqDel[4];
         //LOG(INFO) << "TEST Doods     " << fixed << setprecision(15)
         //   << setw(22) << Dood[0] << setw(22) << Dood[1] << setw(22)
         //   << Dood[2] << setw(22) << Dood[3] << setw(22) << Dood[4]
         //   << setw(22) << Dood[5];
         //LOG(INFO) << "TEST Dood freqs" << fixed << setprecision(15)
         //   << setw(22) << freqDood[0] << setw(22) << freqDood[1] << setw(22)
         //   << freqDood[2] << setw(22) << freqDood[3] << setw(22) << freqDood[4]
         //   << setw(22) << freqDood[5];
         nder = deriveTides(SchInd, amp, phs, Dood, freqDood, ampU, phsU, freq, NSTD);
         //LOG(INFO) << "Vertical returned " << nder << " derived tides";

         // west
         for(i=0; i<NSTD; i++) {
            amp[i] = coeff[11+i];
            phs[i] = -coeff[44+i];
         }
         //oss.str(""); oss << fixed << setprecision(5) << "TEST Amp  2 ";
         //for(i=0; i<NSTD; i++) oss << " " << setw(8) << amp[i];
         //LOG(INFO) << oss.str();
         //oss.str(""); oss << fixed << setprecision(1) << "TEST Phs  2 ";
         //for(i=0; i<NSTD; i++) oss << " " << setw(8) << phs[i];
         //LOG(INFO) << oss.str();
         nder = deriveTides(SchInd, amp, phs, Dood, freqDood, ampW, phsW, freq, NSTD);
         //LOG(INFO) << "West returned " << nder << " derived tides";

         // south
         for(i=0; i<NSTD; i++) {
            amp[i] = coeff[22+i];
            phs[i] = -coeff[55+i];
         }
         //oss.str(""); oss << fixed << setprecision(5) << "TEST Amp  3 ";
         //for(i=0; i<NSTD; i++) oss << " " << setw(8) << amp[i];
         //LOG(INFO) << oss.str();
         //oss.str(""); oss << fixed << setprecision(1) << "TEST Phs  3 ";
         //for(i=0; i<NSTD; i++) oss << " " << setw(8) << phs[i];
         //LOG(INFO) << oss.str();
         nder = deriveTides(SchInd, amp, phs, Dood, freqDood, ampS, phsS, freq, NSTD);
         //LOG(INFO) << "TEST First 40 South amp, phase";
         //for(i=0; i<40; i++) LOG(INFO) << "TEST " << setw(2) << i+1 << fixed
         //   << setprecision(15) << setw(22) << ampS[i] << setw(22) << phsS[i];

         // sum up
         Triple dc(0.0,0.0,0.0);          // U S W
         for(i=0; i<nder; i++) {
            dc[0] += ampU[i] * ::cos(phsU[i]*DEG_TO_RAD);
            //LOG(INFO)<<"TEST LOOP U " << setw(3) << i+1 << fixed << setprecision(15)
            //  << setw(22) << ampU[i]*::cos(phsU[i]*DEG_TO_RAD) << setw(22) << dc[0];
         }
         //LOG(INFO) << "TEST RECURS result U    " << fixed << setprecision(15)
         //      << setw(22) << dc[0];

         for(i=0; i<nder; i++) {
            dc[1] += ampS[i] * ::cos(phsS[i]*DEG_TO_RAD);
            //LOG(INFO)<<"TEST LOOP S " << setw(3) << i+1 << fixed << setprecision(15)
            //  << setw(22) << ampS[i]*::cos(phsS[i]*DEG_TO_RAD) << setw(22) << dc[1];
         }
         //LOG(INFO) << "TEST RECURS result S    " << fixed << setprecision(15)
         //      << setw(22) << dc[1];

         for(i=0; i<nder; i++) {
            dc[2] += ampW[i] * ::cos(phsW[i]*DEG_TO_RAD);
            //LOG(INFO)<<"TEST LOOP W " << setw(3) << i+1 << fixed << setprecision(15)
            //  << setw(22) << ampW[i]*::cos(phsW[i]*DEG_TO_RAD) << setw(22) << dc[2];
         }
         //LOG(INFO) << "TEST RECURS result W    " << fixed << setprecision(15)
         //      << setw(22) << dc[2];

         //LOG(INFO) << "TEST      " << fixed << setprecision(6)
         //   << " " << dc[0] << "     " << dc[1] << "     " << dc[2];

         // convert vertical,south,west to north,east,up
         double temp=dc[0];
         dc[0] = -dc[1];         // N = -S
         dc[1] = -dc[2];         // E = -W
         dc[2] = temp;           // U = U

         return dc;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
      catch(exception& e) {
         Exception E("std except: " + string(e.what()));
         GPSTK_THROW(E);
      }
      catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }

   }  // end Triple OceanLoadTides::computeDisplacement

   //---------------------------------------------------------------------------------
   int OceanLoadTides::deriveTides(const NVector SchInd[],
                                   const double amp[], const double phs[],
                                   const double Dood[], const double freqDood[],
                                   double ampDer[], double phsDer[], double freqDer[],
                                   const int Nin)
      throw(Exception)
   {
      // indexes for std tides: M2, S2, N2, K2, K1,  O1,  P1,  Q1,  Mf,  Mm, Ssa
      static const int stdindex[] = {
         0,  1,  2,  3,109, 110, 111, 112, 263, 264, 265 };

      static const double DerAmp[] = {
          .632208, .294107, .121046, .079915, .023818,-.023589, .022994,
          .019333,-.017871, .017192, .016018, .004671,-.004662,-.004519,
          .004470, .004467, .002589,-.002455,-.002172, .001972, .001947,
          .001914,-.001898, .001802, .001304, .001170, .001130, .001061,
         -.001022,-.001017, .001014, .000901,-.000857, .000855, .000855,
          .000772, .000741, .000741,-.000721, .000698, .000658, .000654,
         -.000653, .000633, .000626,-.000598, .000590, .000544, .000479,
         -.000464, .000413,-.000390, .000373, .000366, .000366,-.000360,
         -.000355, .000354, .000329, .000328, .000319, .000302, .000279,
         -.000274,-.000272, .000248,-.000225, .000224,-.000223,-.000216,
          .000211, .000209, .000194, .000185,-.000174,-.000171, .000159,
          .000131, .000127, .000120, .000118, .000117, .000108, .000107,
          .000105,-.000102, .000102, .000099,-.000096, .000095,-.000089,
         -.000085,-.000084,-.000081,-.000077,-.000072,-.000067, .000066,
          .000064, .000063, .000063, .000063, .000062, .000062,-.000060,
          .000056, .000053, .000051, .000050, .368645,-.262232,-.121995,
         -.050208, .050031,-.049470, .020620, .020613, .011279,-.009530,
         -.009469,-.008012, .007414,-.007300, .007227,-.007131,-.006644,
          .005249, .004137, .004087, .003944, .003943, .003420, .003418,
          .002885, .002884, .002160,-.001936, .001934,-.001798, .001690,
          .001689, .001516, .001514,-.001511, .001383, .001372, .001371,
         -.001253,-.001075, .001020, .000901, .000865,-.000794, .000788,
          .000782,-.000747,-.000745, .000670,-.000603,-.000597, .000542,
          .000542,-.000541,-.000469,-.000440, .000438, .000422, .000410,
         -.000374,-.000365, .000345, .000335,-.000321,-.000319, .000307,
          .000291, .000290,-.000289, .000286, .000275, .000271, .000263,
         -.000245, .000225, .000225, .000221,-.000202,-.000200,-.000199,
          .000192, .000183, .000183, .000183,-.000170, .000169, .000168,
          .000162, .000149,-.000147,-.000141, .000138, .000136, .000136,
          .000127, .000127,-.000126,-.000121,-.000121, .000117,-.000116,
         -.000114,-.000114,-.000114, .000114, .000113, .000109, .000108,
          .000106,-.000106,-.000106, .000105, .000104,-.000103,-.000100,
         -.000100,-.000100, .000099,-.000098, .000093, .000093, .000090,
         -.000088, .000083,-.000083,-.000082,-.000081,-.000079,-.000077,
         -.000075,-.000075,-.000075, .000071, .000071,-.000071, .000068,
          .000068, .000065, .000065, .000064, .000064, .000064,-.000064,
         -.000060, .000056, .000056, .000053, .000053, .000053,-.000053,
          .000053, .000053, .000052, .000050,-.066607,-.035184,-.030988,
          .027929,-.027616,-.012753,-.006728,-.005837,-.005286,-.004921,
         -.002884,-.002583,-.002422, .002310, .002283,-.002037, .001883,
         -.001811,-.001687,-.001004,-.000925,-.000844, .000766, .000766,
         -.000700,-.000495,-.000492, .000491, .000483, .000437,-.000416,
         -.000384, .000374,-.000312,-.000288,-.000273, .000259, .000245,
         -.000232, .000229,-.000216, .000206,-.000204,-.000202, .000200,
          .000195,-.000190, .000187, .000180,-.000179, .000170, .000153,
         -.000137,-.000119,-.000119,-.000112,-.000110,-.000110, .000107,
         -.000095,-.000095,-.000091,-.000090,-.000081,-.000079,-.000079,
          .000077,-.000073, .000069,-.000067,-.000066, .000065, .000064,
         -.000062, .000060, .000059,-.000056, .000055,-.000051 };
   
      static const NVector DerInd[] = {
         { 2, 0, 0, 0, 0, 0 },  { 2, 2,-2, 0, 0, 0 },  { 2,-1, 0, 1, 0, 0 },//M2,S2,N2
         { 2, 2, 0, 0, 0, 0 },  { 2, 2, 0, 0, 1, 0 },  { 2, 0, 0, 0,-1, 0 },//K2,x,x
         { 2,-1, 2,-1, 0, 0 },  { 2,-2, 2, 0, 0, 0 },  { 2, 1, 0,-1, 0, 0 },  
         { 2, 2,-3, 0, 0, 1 },  { 2,-2, 0, 2, 0, 0 },  { 2,-3, 2, 1, 0, 0 },  
         { 2, 1,-2, 1, 0, 0 },  { 2,-1, 0, 1,-1, 0 },  { 2, 3, 0,-1, 0, 0 },  
         { 2, 1, 0, 1, 0, 0 },  { 2, 2, 0, 0, 2, 0 },  { 2, 2,-1, 0, 0,-1 },  
         { 2, 0,-1, 0, 0, 1 },  { 2, 1, 0, 1, 1, 0 },  { 2, 3, 0,-1, 1, 0 },  
         { 2, 0, 1, 0, 0,-1 },  { 2, 0,-2, 2, 0, 0 },  { 2,-3, 0, 3, 0, 0 },  
         { 2,-2, 3, 0, 0,-1 },  { 2, 4, 0, 0, 0, 0 },  { 2,-1, 1, 1, 0,-1 },  
         { 2,-1, 3,-1, 0,-1 },  { 2, 2, 0, 0,-1, 0 },  { 2,-1,-1, 1, 0, 1 },  
         { 2, 4, 0, 0, 1, 0 },  { 2,-3, 4,-1, 0, 0 },  { 2,-1, 2,-1,-1, 0 },  
         { 2, 3,-2, 1, 0, 0 },  { 2, 1, 2,-1, 0, 0 },  { 2,-4, 2, 2, 0, 0 },  
         { 2, 4,-2, 0, 0, 0 },  { 2, 0, 2, 0, 0, 0 },  { 2,-2, 2, 0,-1, 0 },  
         { 2, 2,-4, 0, 0, 2 },  { 2, 2,-2, 0,-1, 0 },  { 2, 1, 0,-1,-1, 0 },  
         { 2,-1, 1, 0, 0, 0 },  { 2, 2,-1, 0, 0, 1 },  { 2, 2, 1, 0, 0,-1 },  
         { 2,-2, 0, 2,-1, 0 },  { 2,-2, 4,-2, 0, 0 },  { 2, 2, 2, 0, 0, 0 },  
         { 2,-4, 4, 0, 0, 0 },  { 2,-1, 0,-1,-2, 0 },  { 2, 1, 2,-1, 1, 0 },  
         { 2,-1,-2, 3, 0, 0 },  { 2, 3,-2, 1, 1, 0 },  { 2, 4, 0,-2, 0, 0 },  
         { 2, 0, 0, 2, 0, 0 },  { 2, 0, 2,-2, 0, 0 },  { 2, 0, 2, 0, 1, 0 },  
         { 2,-3, 3, 1, 0,-1 },  { 2, 0, 0, 0,-2, 0 },  { 2, 4, 0, 0, 2, 0 },  
         { 2, 4,-2, 0, 1, 0 },  { 2, 0, 0, 0, 0, 2 },  { 2, 1, 0, 1, 2, 0 },  
         { 2, 0,-2, 0,-2, 0 },  { 2,-2, 1, 0, 0, 1 },  { 2,-2, 1, 2, 0,-1 },  
         { 2,-1, 1,-1, 0, 1 },  { 2, 5, 0,-1, 0, 0 },  { 2, 1,-3, 1, 0, 1 },  
         { 2,-2,-1, 2, 0, 1 },  { 2, 3, 0,-1, 2, 0 },  { 2, 1,-2, 1,-1, 0 },  
         { 2, 5, 0,-1, 1, 0 },  { 2,-4, 0, 4, 0, 0 },  { 2,-3, 2, 1,-1, 0 },  
         { 2,-2, 1, 1, 0, 0 },  { 2, 4, 0,-2, 1, 0 },  { 2, 0, 0, 2, 1, 0 },  
         { 2,-5, 4, 1, 0, 0 },  { 2, 0, 2, 0, 2, 0 },  { 2,-1, 2, 1, 0, 0 },  
         { 2, 5,-2,-1, 0, 0 },  { 2, 1,-1, 0, 0, 0 },  { 2, 2,-2, 0, 0, 2 },  
         { 2,-5, 2, 3, 0, 0 },  { 2,-1,-2, 1,-2, 0 },  { 2,-3, 5,-1, 0,-1 },  
         { 2,-1, 0, 0, 0, 1 },  { 2,-2, 0, 0,-2, 0 },  { 2, 0,-1, 1, 0, 0 },  
         { 2,-3, 1, 1, 0, 1 },  { 2, 3, 0,-1,-1, 0 },  { 2, 1, 0, 1,-1, 0 },  
         { 2,-1, 2, 1, 1, 0 },  { 2, 0,-3, 2, 0, 1 },  { 2, 1,-1,-1, 0, 1 },  
         { 2,-3, 0, 3,-1, 0 },  { 2, 0,-2, 2,-1, 0 },  { 2,-4, 3, 2, 0,-1 },  
         { 2,-1, 0, 1,-2, 0 },  { 2, 5, 0,-1, 2, 0 },  { 2,-4, 5, 0, 0,-1 },  
         { 2,-2, 4, 0, 0,-2 },  { 2,-1, 0, 1, 0, 2 },  { 2,-2,-2, 4, 0, 0 },  
         { 2, 3,-2,-1,-1, 0 },  { 2,-2, 5,-2, 0,-1 },  { 2, 0,-1, 0,-1, 1 },  
         { 2, 5,-2,-1, 1, 0 },  { 1, 1, 0, 0, 0, 0 },  { 1,-1, 0, 0, 0, 0 },//x,K1,O1
         { 1, 1,-2, 0, 0, 0 },  { 1,-2, 0, 1, 0, 0 },  { 1, 1, 0, 0, 1, 0 },//P1,Q1,x
         { 1,-1, 0, 0,-1, 0 },  { 1, 2, 0,-1, 0, 0 },  { 1, 0, 0, 1, 0, 0 },  
         { 1, 3, 0, 0, 0, 0 },  { 1,-2, 2,-1, 0, 0 },  { 1,-2, 0, 1,-1, 0 },  
         { 1,-3, 2, 0, 0, 0 },  { 1, 0, 0,-1, 0, 0 },  { 1, 1, 0, 0,-1, 0 },  
         { 1, 3, 0, 0, 1, 0 },  { 1, 1,-3, 0, 0, 1 },  { 1,-3, 0, 2, 0, 0 },  
         { 1, 1, 2, 0, 0, 0 },  { 1, 0, 0, 1, 1, 0 },  { 1, 2, 0,-1, 1, 0 },  
         { 1, 0, 2,-1, 0, 0 },  { 1, 2,-2, 1, 0, 0 },  { 1, 3,-2, 0, 0, 0 },  
         { 1,-1, 2, 0, 0, 0 },  { 1, 1, 1, 0, 0,-1 },  { 1, 1,-1, 0, 0, 1 },  
         { 1, 4, 0,-1, 0, 0 },  { 1,-4, 2, 1, 0, 0 },  { 1, 0,-2, 1, 0, 0 },  
         { 1,-2, 2,-1,-1, 0 },  { 1, 3, 0,-2, 0, 0 },  { 1,-1, 0, 2, 0, 0 },  
         { 1,-1, 0, 0,-2, 0 },  { 1, 3, 0, 0, 2, 0 },  { 1,-3, 2, 0,-1, 0 },  
         { 1, 4, 0,-1, 1, 0 },  { 1, 0, 0,-1,-1, 0 },  { 1, 1,-2, 0,-1, 0 },  
         { 1,-3, 0, 2,-1, 0 },  { 1, 1, 0, 0, 2, 0 },  { 1, 1,-1, 0, 0,-1 },  
         { 1,-1,-1, 0, 0, 1 },  { 1, 0, 2,-1, 1, 0 },  { 1,-1, 1, 0, 0,-1 },  
         { 1,-1,-2, 2, 0, 0 },  { 1, 2,-2, 1, 1, 0 },  { 1,-4, 0, 3, 0, 0 },  
         { 1,-1, 2, 0, 1, 0 },  { 1, 3,-2, 0, 1, 0 },  { 1, 2, 0,-1,-1, 0 },  
         { 1, 0, 0, 1,-1, 0 },  { 1,-2, 2, 1, 0, 0 },  { 1, 4,-2,-1, 0, 0 },  
         { 1,-3, 3, 0, 0,-1 },  { 1,-2, 1, 1, 0,-1 },  { 1,-2, 3,-1, 0,-1 },  
         { 1, 0,-2, 1,-1, 0 },  { 1,-2,-1, 1, 0, 1 },  { 1, 4,-2, 1, 0, 0 },  
         { 1,-4, 4,-1, 0, 0 },  { 1,-4, 2, 1,-1, 0 },  { 1, 5,-2, 0, 0, 0 },  
         { 1, 3, 0,-2, 1, 0 },  { 1,-5, 2, 2, 0, 0 },  { 1, 2, 0, 1, 0, 0 },  
         { 1, 1, 3, 0, 0,-1 },  { 1,-2, 0, 1,-2, 0 },  { 1, 4, 0,-1, 2, 0 },  
         { 1, 1,-4, 0, 0, 2 },  { 1, 5, 0,-2, 0, 0 },  { 1,-1, 0, 2, 1, 0 },  
         { 1,-2, 1, 0, 0, 0 },  { 1, 4,-2, 1, 1, 0 },  { 1,-3, 4,-2, 0, 0 },  
         { 1,-1, 3, 0, 0,-1 },  { 1, 3,-3, 0, 0, 1 },  { 1, 5,-2, 0, 1, 0 },  
         { 1, 1, 2, 0, 1, 0 },  { 1, 2, 0, 1, 1, 0 },  { 1,-5, 4, 0, 0, 0 },  
         { 1,-2, 0,-1,-2, 0 },  { 1, 5, 0,-2, 1, 0 },  { 1, 1, 2,-2, 0, 0 },  
         { 1, 1,-2, 2, 0, 0 },  { 1,-2, 2, 1, 1, 0 },  { 1, 0, 3,-1, 0,-1 },  
         { 1, 2,-3, 1, 0, 1 },  { 1,-2,-2, 3, 0, 0 },  { 1,-1, 2,-2, 0, 0 },  
         { 1,-4, 3, 1, 0,-1 },  { 1,-4, 0, 3,-1, 0 },  { 1,-1,-2, 2,-1, 0 },  
         { 1,-2, 0, 3, 0, 0 },  { 1, 4, 0,-3, 0, 0 },  { 1, 0, 1, 1, 0,-1 },  
         { 1, 2,-1,-1, 0, 1 },  { 1, 2,-2, 1,-1, 0 },  { 1, 0, 0,-1,-2, 0 },  
         { 1, 2, 0, 1, 2, 0 },  { 1, 2,-2,-1,-1, 0 },  { 1, 0, 0, 1, 2, 0 },  
         { 1, 0, 1, 0, 0, 0 },  { 1, 2,-1, 0, 0, 0 },  { 1, 0, 2,-1,-1, 0 },  
         { 1,-1,-2, 0,-2, 0 },  { 1,-3, 1, 0, 0, 1 },  { 1, 3,-2, 0,-1, 0 },  
         { 1,-1,-1, 0,-1, 1 },  { 1, 4,-2,-1, 1, 0 },  { 1, 2, 1,-1, 0,-1 },  
         { 1, 0,-1, 1, 0, 1 },  { 1,-2, 4,-1, 0, 0 },  { 1, 4,-4, 1, 0, 0 },  
         { 1,-3, 1, 2, 0,-1 },  { 1,-3, 3, 0,-1,-1 },  { 1, 1, 2, 0, 2, 0 },  
         { 1, 1,-2, 0,-2, 0 },  { 1, 3, 0, 0, 3, 0 },  { 1,-1, 2, 0,-1, 0 },  
         { 1,-2, 1,-1, 0, 1 },  { 1, 0,-3, 1, 0, 1 },  { 1,-3,-1, 2, 0, 1 },  
         { 1, 2, 0,-1, 2, 0 },  { 1, 6,-2,-1, 0, 0 },  { 1, 2, 2,-1, 0, 0 },  
         { 1,-1, 1, 0,-1,-1 },  { 1,-2, 3,-1,-1,-1 },  { 1,-1, 0, 0, 0, 2 },  
         { 1,-5, 0, 4, 0, 0 },  { 1, 1, 0, 0, 0,-2 },  { 1,-2, 1, 1,-1,-1 },  
         { 1, 1,-1, 0, 1, 1 },  { 1, 1, 2, 0, 0,-2 },  { 1,-3, 1, 1, 0, 0 },  
         { 1,-4, 4,-1,-1, 0 },  { 1, 1, 0,-2,-1, 0 },  { 1,-2,-1, 1,-1, 1 },  
         { 1,-3, 2, 2, 0, 0 },  { 1, 5,-2,-2, 0, 0 },  { 1, 3,-4, 2, 0, 0 },  
         { 1, 1,-2, 0, 0, 2 },  { 1,-1, 4,-2, 0, 0 },  { 1, 2, 2,-1, 1, 0 },  
         { 1,-5, 2, 2,-1, 0 },  { 1, 1,-3, 0,-1, 1 },  { 1, 1, 1, 0, 1,-1 },  
         { 1, 6,-2,-1, 1, 0 },  { 1,-2, 2,-1,-2, 0 },  { 1, 4,-2, 1, 2, 0 },  
         { 1,-6, 4, 1, 0, 0 },  { 1, 5,-4, 0, 0, 0 },  { 1,-3, 4, 0, 0, 0 },  
         { 1, 1, 2,-2, 1, 0 },  { 1,-2, 1, 0,-1, 0 },  { 0, 2, 0, 0, 0, 0 },//x,x,Mf
         { 0, 1, 0,-1, 0, 0 },  { 0, 0, 2, 0, 0, 0 },  { 0, 0, 0, 0, 1, 0 },//Mm,SSa
         { 0, 2, 0, 0, 1, 0 },  { 0, 3, 0,-1, 0, 0 },  { 0, 1,-2, 1, 0, 0 },  
         { 0, 2,-2, 0, 0, 0 },  { 0, 3, 0,-1, 1, 0 },  { 0, 0, 1, 0, 0,-1 },  
         { 0, 2, 0,-2, 0, 0 },  { 0, 2, 0, 0, 2, 0 },  { 0, 3,-2, 1, 0, 0 },  
         { 0, 1, 0,-1,-1, 0 },  { 0, 1, 0,-1, 1, 0 },  { 0, 4,-2, 0, 0, 0 },  
         { 0, 1, 0, 1, 0, 0 },  { 0, 0, 3, 0, 0,-1 },  { 0, 4, 0,-2, 0, 0 },  
         { 0, 3,-2, 1, 1, 0 },  { 0, 3,-2,-1, 0, 0 },  { 0, 4,-2, 0, 1, 0 },  
         { 0, 0, 2, 0, 1, 0 },  { 0, 1, 0, 1, 1, 0 },  { 0, 4, 0,-2, 1, 0 },  
         { 0, 3, 0,-1, 2, 0 },  { 0, 5,-2,-1, 0, 0 },  { 0, 1, 2,-1, 0, 0 },  
         { 0, 1,-2, 1,-1, 0 },  { 0, 1,-2, 1, 1, 0 },  { 0, 2,-2, 0,-1, 0 },  
         { 0, 2,-3, 0, 0, 1 },  { 0, 2,-2, 0, 1, 0 },  { 0, 0, 2,-2, 0, 0 },  
         { 0, 1,-3, 1, 0, 1 },  { 0, 0, 0, 0, 2, 0 },  { 0, 0, 1, 0, 0, 1 },  
         { 0, 1, 2,-1, 1, 0 },  { 0, 3, 0,-3, 0, 0 },  { 0, 2, 1, 0, 0,-1 },  
         { 0, 1,-1,-1, 0, 1 },  { 0, 1, 0, 1, 2, 0 },  { 0, 5,-2,-1, 1, 0 },  
         { 0, 2,-1, 0, 0, 1 },  { 0, 2, 2,-2, 0, 0 },  { 0, 1,-1, 0, 0, 0 },  
         { 0, 5, 0,-3, 0, 0 },  { 0, 2, 0,-2, 1, 0 },  { 0, 1, 1,-1, 0,-1 },  
         { 0, 3,-4, 1, 0, 0 },  { 0, 0, 2, 0, 2, 0 },  { 0, 2, 0,-2,-1, 0 },  
         { 0, 4,-3, 0, 0, 1 },  { 0, 3,-1,-1, 0, 1 },  { 0, 0, 2, 0, 0,-2 },  
         { 0, 3,-3, 1, 0, 1 },  { 0, 2,-4, 2, 0, 0 },  { 0, 4,-2,-2, 0, 0 },  
         { 0, 3, 1,-1, 0,-1 },  { 0, 5,-4, 1, 0, 0 },  { 0, 3,-2,-1,-1, 0 },  
         { 0, 3,-2, 1, 2, 0 },  { 0, 4,-4, 0, 0, 0 },  { 0, 6,-2,-2, 0, 0 },  
         { 0, 5, 0,-3, 1, 0 },  { 0, 4,-2, 0, 2, 0 },  { 0, 2, 2,-2, 1, 0 },  
         { 0, 0, 4, 0, 0,-2 },  { 0, 3,-1, 0, 0, 0 },  { 0, 3,-3,-1, 0, 1 },  
         { 0, 4, 0,-2, 2, 0 },  { 0, 1,-2,-1,-1, 0 },  { 0, 2,-1, 0, 0,-1 },  
         { 0, 4,-4, 2, 0, 0 },  { 0, 2, 1, 0, 1,-1 },  { 0, 3,-2,-1, 1, 0 },  
         { 0, 4,-3, 0, 1, 1 },  { 0, 2, 0, 0, 3, 0 },  { 0, 6,-4, 0, 0, 0 },
      };

      if((int)(sizeof(DerAmp) / sizeof(double)) != NDER
            || (int)(sizeof(DerInd) / sizeof(NVector)) != NDER) {
         Exception e("Static arrays are corrupted");
         GPSTK_THROW(e);
      }
   
      int i,j,k,kk;
      static const double dtr(0.01745329252);

      // get amplitude, phase and frequency for each of the standard tides
      double RealAmp[NSTD], ImagAmp[NSTD], Freq[NSTD];
      double phsrad, freq, phas;
      for(i=0; i<Nin; i++) {       // Nin is NSTD
         // first find the index for this tide
         j = stdindex[i];

         // amplitudes
         phsrad = phs[i] * dtr; //DEG_TO_RAD;          // phase in radians
         RealAmp[i] = amp[i] * ::cos(phsrad) / ::fabs(DerAmp[j]);
         ImagAmp[i] = amp[i] * ::sin(phsrad) / ::fabs(DerAmp[j]);
         //LOG(INFO) << "TEST " << setw(2) << i+1 << fixed << setprecision(15)
         //   << setw(19) << amp[i] << setw(19) << phsrad << setw(19) << DerAmp[j];

         // phase and freq
         freq = phas = 0.0;
         for(k=0; k<6; k++) {
            freq += DerInd[j].n[k] * freqDood[k];
            // not used phas += DerInd[j].n[k] * Dood[k];
         }
         Freq[i] = freq;

         // make 0 <= phas < 360  -- why?
         // not used phas = ::fmod(phas,360.0);
         // not used if(phas < 0.0) phas += 360.0;

         //LOG(INFO) << "Dood " << setw(2) << i << " at index " << setw(3) << j
         //   << " (" << DerInd[j].n[0] << "," << setw(2) << DerInd[j].n[1]
         //   << "," << setw(2) << DerInd[j].n[2] << "," << setw(2) << DerInd[j].n[3]
         //   << "," << setw(2) << DerInd[j].n[4] << "," << setw(2) << DerInd[j].n[5]
         //   << ") rA iA F " << fixed << setprecision(10) << setw(13)
         //   << RealAmp[i] << " " << setw(13) << ImagAmp[i]
         //   << " " << setw(12) << Freq[i];
      }

      // sort the frequency, and keep the key
      int key[NSTD];
      for(i=0; i<Nin; ++i) key[i] = i;
      QSort(Freq, key, Nin);

      // use key to sort amplitudes
      double tmpR[NSTD],tmpI[NSTD];
      for(i=0; i<Nin; ++i) {
         tmpR[i] = RealAmp[i];
         tmpI[i] = ImagAmp[i];
      }
      for(i=0; i<Nin; ++i) {
         RealAmp[i] = tmpR[key[i]];
         ImagAmp[i] = tmpI[key[i]];
      }

      // count the shells
      int nl(0),nm(0),nh(0);
      //LOG(INFO) << "TEST Sorted reamp, imamp, freq\n";
      for(i=0; i<Nin; i++) {       // Nin is NSTD
         //LOG(INFO) << "Sorted Dood " << setw(2) << key[i] << " rA iA F P "
         //   << fixed << setprecision(10) << setw(13) << RealAmp[i]
         //   << " " << setw(13) << ImagAmp[i] << " " << setw(12) << Freq[i];
         //LOG(INFO) << "TEST " << setw(2) << i+1 << fixed << setprecision(15)
         // << setw(19)<< RealAmp[i] << setw(19) << ImagAmp[i] << setw(19) << Freq[i];
         if(     Freq[i] < 0.5) nl++;
         else if(Freq[i] < 1.5) nm++;
         else if(Freq[i] < 2.5) nh++;
         // so freq cannot be >= 2.5??
      }
      //LOG(INFO) << "Shells contain " << nl << " " << nm << " " << nh;

      // split arrays into vector<double> for each shell
      vector<double> Flow,Rlow,Ilow,Fmed,Rmed,Imed,Fhi,Rhi,Ihi;
      for(i=0; i<nl; i++) {
         Flow.push_back(Freq[i]);
         Rlow.push_back(RealAmp[i]);
         Ilow.push_back(ImagAmp[i]);
         //LOG(INFO) << "Low shell Dood " << setw(2) << key[i] << " rA iA F "
         //   << fixed << setprecision(10) << setw(13) << RealAmp[i]
         //   << " " << setw(13) << ImagAmp[i] << " " << setw(12) << Freq[i];
      }
      for(i=nl; i<nl+nm; i++) {
         Fmed.push_back(Freq[i]);
         Rmed.push_back(RealAmp[i]);
         Imed.push_back(ImagAmp[i]);
         //LOG(INFO) << "Med shell Dood " << setw(2) << key[i] << " rA iA F "
         //   << fixed << setprecision(10) << setw(13) << RealAmp[i]
         //   << " " << setw(13) << ImagAmp[i] << " " << setw(12) << Freq[i];
      }
      for(i=nl+nm; i<nl+nm+nh; i++) {
         Fhi.push_back(Freq[i]);
         Rhi.push_back(RealAmp[i]);
         Ihi.push_back(ImagAmp[i]);
         //LOG(INFO) << "Hi shell Dood " << setw(2) << key[i] << " rA iA F "
         //   << fixed << setprecision(10) << setw(13) << RealAmp[i]
         //   << " " << setw(13) << ImagAmp[i] << " " << setw(12) << Freq[i];
      }

      // find splines of amp vs frequency in each shell
      CubicSpline<double> csRlow, csIlow, csRmed, csImed, csRhi, csIhi;
      if(nl > 0) {
         csRlow.Initialize(Flow, Rlow);
         csIlow.Initialize(Flow, Ilow);
      }
      csRmed.Initialize(Fmed, Rmed);
      csImed.Initialize(Fmed, Imed);
      csRhi.Initialize(Fhi, Rhi);
      csIhi.Initialize(Fhi, Ihi);

      // evaluate splines at each of the NDER waves; not all will contribute
      int nout(0);
      for(j=0; j<NDER; j++) {       // loop over 342 derived tides
         // this is why nout may be < NDER
         if(DerInd[j].n[0] == 0 && nl == 0) continue;

         // get phase and freq for this tide
         freqDer[nout] = phsDer[nout] = 0.0;
         for(k=0; k<6; k++) {
            freqDer[nout] += DerInd[j].n[k] * freqDood[k];
            phsDer[nout] += DerInd[j].n[k] * Dood[k];
         }
         phsDer[nout] = ::fmod(phsDer[nout],360.0);
         if(phsDer[nout] < 0.0) phsDer[nout] += 360.0;

         //LOG(INFO) << "TEST TDFRPH "
         //   << setw(3) << j+1 << fixed << setprecision(15)
         //   << setw(22) << freqDer[nout] << setw(22) << phsDer[nout]
         //   << setw(3) << DerInd[j].n[0] << setw(3) << DerInd[j].n[1]
         //   << setw(3) << DerInd[j].n[2] << setw(3) << DerInd[j].n[3]
         //   << setw(3) << DerInd[j].n[4] << setw(3) << DerInd[j].n[5];

         if(     DerInd[j].n[0] == 0) phsDer[nout] += 180.0;
         else if(DerInd[j].n[0] == 1) phsDer[nout] += 90.0;

         // get amplitudes at freq
         freq = freqDer[nout];
         double ramp,iamp;
         if(     DerInd[j].n[0] == 0) {
            if(csRlow.testLimits(freq,ramp)) ramp = csRlow.Evaluate(freq);
            if(csIlow.testLimits(freq,iamp)) iamp = csIlow.Evaluate(freq);
         }
         else if(DerInd[j].n[0] == 1) {
            if(csRmed.testLimits(freq,ramp)) ramp = csRmed.Evaluate(freq);
            if(csImed.testLimits(freq,iamp)) iamp = csImed.Evaluate(freq);
         }
         else if(DerInd[j].n[0] == 2) {
            if(csRhi.testLimits(freq,ramp)) ramp = csRhi.Evaluate(freq);
            if(csIhi.testLimits(freq,iamp)) iamp = csIhi.Evaluate(freq);
         }

         ampDer[nout] = DerAmp[j] * RSS(ramp,iamp);
         phsDer[nout] += ::atan2(iamp,ramp)/dtr; //*RAD_TO_DEG;   // TEMP
         if(phsDer[nout] > 180.0) phsDer[nout] -= 360.0;

         //LOG(INFO) << "TEST RE AM  " << setw(3) << j+1 << fixed
         //   << setprecision(15) << setw(22) << ramp << setw(22) << iamp
         //   << setw(22) << ampDer[nout] << setw(22) << phsDer[nout];

         nout++;
      }

      return nout;

   }  // end int OceanLoadTides::deriveTides()

}  // end namespace gpstk
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
/*
   void OceanLoadTides::deriveTides(const NVector SchInd[],
                                  const double amp[], const double phs[],
                                  double ampDer[], double phsDer[], double freq[],
                                  const int Nin, int Nout)
      PROGRAM HARDISP
*+
*  - - - - - - - - - - -
*   H A R D I S P
*  - - - - - - - - - - -
*
*  This program is part of the International Earth Rotation and
*  Reference Systems Service (IERS) Conventions software collection.
*
*  This program reads in a file of station displacements in the BLQ
*  format used by Scherneck and Bos for ocean loading, and outputs a
*  time series of computed tidal displacements, using an expanded set
*  of tidal constituents, whose amplitudes and phases are found by
*  spline interpolation of the tidal admittance.  A total of 342
*  constituent tides are included, which gives a precision of about
*  0.1%.
*
*  In general, Class 1, 2, and 3 models represent physical effects that
*  act on geodetic parameters while canonical models provide lower-level
*  representations or basic computations that are used by Class 1, 2, or
*  3 models.
* 
*  Status:  Class 1 model
*
*     Class 1 models are those recommended to be used a priori in the
*     reduction of raw space geodetic data in order to determine
*     geodetic parameter estimates.
*     Class 2 models are those that eliminate an observational
*     singularity and are purely conventional in nature.
*     Class 3 models are those that are not required as either Class
*     1 or 2.
*     Canonical models are accepted as is and cannot be classified as
*     a Class 1, 2, or 3 model.
*
*  Given:
*     User provided input ocean loading coefficients (Note 1)
*
*  Returned:
*     DU         d      Radial tidal ocean loading displacement (Note 2)
*     DW         d      West tidal ocean loading displacement (Note 2)
*     DS         d      South tidal ocean loading displacement (Note 2)
*
*     :------------------------------------------:
*     :                                          :
*     :                 IMPORTANT                :
*     :                                          :
*     :  A new version of the ETUTC routine must :
*     :  be produced whenever a new leap second  :
*     :  is announced.  There are three items to :
*     :  change on each such occasion:           :
*     :                                          :
*     :  1) Update the nstep variable            :
*     :  2) Update the arrays st and si          :                              
*     :  3) Change date of latest leap second    :
*     :                                          :
*     :  Latest leap second:  2008 December 31   :
*     :                                          :
*     :__________________________________________:
*
*
*  Notes:
*
*  1) The input ocean loading coefficients were generated by the ocean loading
*     service on 25 June 2009 using http://www.oso.chalmers.se/~loading/ for
*     IGS stations Onsala and Reykjavik using the CSR4.0 model and "NO"
*     geocenter correction.
*
*  2) The site displacement output is written to standard output with the 
*     format 3F14.6.  All units are expressed in meters.
*
*  Called:
*     MDAY              Finds the day number of days before start of a
*                       month and year in Gregorian intercalation
*     ADMINT            Returns the ocean loading displacement amplitude,
*                       frequency, and phase of a set of tidal constituents
*     RECURS            Performs sine and cosine recursion
*
*  Test cases:
*     given input:      The six lines of coefficients listed below entered 
*                       from standard input (e.g. epoch 2009 06 25 00h
*                       00m 00s and for each of the following 23 hours)
*                       with command 'hardisp 2009 6 25 0 0 0 24 3600 < file'
*
*  ONSALA    
*$$ CSR4.0_f_PP ID: 2009-06-25 17:43:24
*$$ Computed by OLMPP by H G Scherneck, Onsala Space Observatory, 2009
*$$ Onsala,                              lon/lat:   11.9264   57.3958    0.00
*  .00352 .00123 .00080 .00032 .00187 .00112 .00063 .00003 .00082 .00044 .00037
*  .00144 .00035 .00035 .00008 .00053 .00049 .00018 .00009 .00012 .00005 .00006
*  .00086 .00023 .00023 .00006 .00029 .00028 .00010 .00007 .00004 .00002 .00001
*   -64.7  -52.0  -96.2  -55.2  -58.8 -151.4  -65.6 -138.1    8.4    5.2    2.1
*    85.5  114.5   56.5  113.6   99.4   19.1   94.1  -10.4 -167.4 -170.0 -177.7
*   109.5  147.0   92.7  148.8   50.5  -55.1   36.4 -170.4  -15.0    2.3    5.2
*
*  REYKJAVIK 
*$$ CSR4.0_f_PP ID: 2009-06-25 20:02:03
*$$ Computed by OLMPP by H G Scherneck, Onsala Space Observatory, 2009
*$$ Reykjavik,                           lon/lat:   64.1388  -21.9555    0.00
*  .02359 .01481 .00448 .00419 .00273 .00033 .00088 .00005 .00081 .00034 .00034
*  .00514 .00280 .00089 .00078 .00106 .00074 .00035 .00018 .00004 .00001 .00003
*  .00209 .00077 .00051 .00021 .00151 .00066 .00047 .00019 .00014 .00008 .00006
*    78.5  102.3   76.3  104.1  -52.0 -160.4  -52.6 -128.0 -174.8 -175.7 -178.1
*    54.2   93.8   38.9   96.9    2.3  -12.5    3.1  -31.6 -144.4 -122.6 -173.5
*   156.2 -167.1  141.9 -164.9  155.9  178.7  155.6 -168.5  177.7  178.7  179.7
*
*     expected output:
*        Onsala:
*         dU            dS            dW
*      0.003513     -0.001893     -0.001513
*      0.003248     -0.001620     -0.001012
*      0.002079     -0.001068     -0.000319
*      0.000498     -0.000374      0.000328
*     -0.000913      0.000298      0.000712
*     -0.001639      0.000800      0.000695
*     -0.001369      0.001032      0.000254
*     -0.000084      0.000966     -0.000510
*      0.001935      0.000654     -0.001401
*      0.004177      0.000208     -0.002171
*      0.006027     -0.000226     -0.002591
*      0.006926     -0.000510     -0.002503
*      0.006519     -0.000548     -0.001871
*      0.004749     -0.000313     -0.000786
*      0.001883      0.000147      0.000549
*     -0.001543      0.000717      0.001864
*     -0.004850      0.001247      0.002889
*     -0.007370      0.001590      0.003416
*     -0.008606      0.001637      0.003347
*     -0.008344      0.001347      0.002718
*     -0.006702      0.000762      0.001686
*     -0.004090     -0.000009      0.000492
*     -0.001113     -0.000808     -0.000600
*      0.001583     -0.001469     -0.001366
*
*        Reykjavik:
*         dU            dS            dW
*     -0.026799     -0.000581     -0.004764
*     -0.009412     -0.001202     -0.000967
*      0.010158     -0.001161      0.002656
*      0.027166     -0.000516      0.005196
*      0.037534      0.000503      0.006044
*      0.038862      0.001557      0.005044
*      0.031016      0.002290      0.002531
*      0.016138      0.002423     -0.000768
*     -0.001916      0.001833     -0.003916
*     -0.018559      0.000586     -0.006015
*     -0.029641     -0.001072     -0.006436
*     -0.032492     -0.002771     -0.004989
*     -0.026579     -0.004114     -0.001969
*     -0.013621     -0.004767      0.001911
*      0.002867     -0.004550      0.005700
*      0.018492     -0.003477      0.008446
*      0.029107     -0.001761      0.009439
*      0.031871      0.000238      0.008391
*      0.025972      0.002101      0.005508
*      0.012828      0.003443      0.001443
*     -0.004277      0.004011     -0.002863
*     -0.021034      0.003733     -0.006421
*     -0.033196      0.002738     -0.008429
*     -0.037657      0.001315     -0.008472
*           
*  References:
*
*     Petit, G. and Luzum, B. (eds.), IERS Conventions (2010),
*     IERS Technical Note No. 36, BKG (2010)
*
*  Revisions:  
*  2005 August  Duncan Agnew     Original code, which is based on the 
*                                program hartid distributed with the 
*                                SPOTL loading package
*  2005 November D. Agnew        Corrected error in reading displacements
*  2005 December D. Agnew        Hartmann-Wenzel harmonics in admint
*                                subroutine
*  2007 December 17 G. Petit     Corrected 'intial' to 'initial' 
*                                (noted by T. Springer)
*  2008 June     D. Agnew        Corrected long-period tides, modernized
*                                control flow, added explicit typing, increased
*                                number of harmonics and added one decimal to
*                                their amplitudes
*  2009 February 16 G. Petit     Updated etutc subroutine for 2009.0 leap
*                                second
*  2009 June     25 B. Stetzler  Initial standardization of code
*  2009 June     26 B. Stetzler  Provided two test cases
*  2009 July     02 B. Stetzler  Capitalization for backwards compatibility
*  2009 August   19 B. Stetzler  Updated test cases
*-----------------------------------------------------------------------

      IMPLICIT NONE
      INTEGER I,IDAY,IDT,IMONTH,IRNT,IRHI,IRLI,IT,LUO,NB,NL,NP,NT,NTIN,
     .        KK,NTOUT,MDAY
*+---------------------------------------------------------------------
*
*  Parameters below set the buffer size for computing the tides
*  recursively (nl), the number of harmonics used in the prediction
*  (nt; this must also be set in the subroutine admint) and the number
*  of harmonics read in (ntin)
*
*----------------------------------------------------------------------
      PARAMETER (NL=600)
      PARAMETER (NT=342)
      PARAMETER (NTIN=11)

      CHARACTER*40 DUMM
      REAL AMP,AS,AW,AZ,DS,DW,DZ,HCS,HCW,HCZ,PHASE,TAMP,TPH,WF,SAMP
      DOUBLE PRECISION F,PZ,PS,PW,SCR
      DOUBLE PRECISION DR,PI

      DIMENSION TAMP(3,NTIN),TPH(3,NTIN)
      DIMENSION IDT(6,NTIN),AMP(NTIN),PHASE(NTIN)
      DIMENSION AZ(NT),PZ(NT),HCZ(2*NT)
      DIMENSION AS(NT),PS(NT),HCS(2*NT)
      DIMENSION AW(NT),PW(NT),HCW(2*NT)
      DIMENSION DZ(NL),DS(NL),DW(NL)
      DIMENSION F(NT),SCR(3*NT),WF(NT)
      COMMON/DATE/IT(5)
      DATA DR/0.01745329252D0/,IRLI/1/
      PARAMETER ( PI = 3.1415926535897932384626433D0 ) 
      DATA LUO/6/

*  Cartwright-Tayler numbers of tides used in Scherneck lists:
*      M2, S2, N2, K2, K1, O1, P1, Q1, Mf, Mm, Ssa

      DATA IDT/
     .  2, 0, 0, 0, 0, 0,   2, 2,-2, 0, 0, 0,   2,-1, 0, 1, 0, 0,
     .  2, 2, 0, 0, 0, 0,   1, 1, 0, 0, 0, 0,   1,-1, 0, 0, 0, 0,
     .  1, 1,-2, 0, 0, 0,   1,-2, 0, 1, 0, 0,   0, 2, 0, 0, 0, 0,
     .  0, 1, 0,-1, 0, 0,   0, 0, 2, 0, 0, 0/
*+----------------------------------------------------------------------
*
*  Check number of arguments from command line, then read them in
*
*-----------------------------------------------------------------------

      IF(IARGC().LT.7.OR.IARGC().GT.8) THEN
         WRITE(LUO,100)
 100     FORMAT(/,'Usage:',/,
     .  '   hardisp yr [d-of-yr | month day] hr min sec num samp',//,
     .  ' Where ',/,
     .  '   the UTC date given is the time of the first term output',/,
     .  '   num is the number of output epochs to be written out',/,
     .  '   samp is the sample interval (seconds)',//,
     .  '  The harmonics file (amp and phase of displacement) is ',/,
     .  '    read from standard input in the BLQ format used by  ',/,
     .  '    Scherneck and Bos                                   ',//,
     .  '  Results are written to standard output (units = m):',/,
     .  '      dU    dS    dW   ',/,
     .  '    using format: 3F14.6 ',/)
         STOP
      ENDIF

      CALL GETARG(1,DUMM)
      READ(DUMM,102) IT(1)
 102  FORMAT(I4)
      IF(IARGC().EQ.7) THEN
        CALL GETARG(2,DUMM)
        READ(DUMM,102) IT(2)
        NB=0
      ENDIF
      IF(IARGC().EQ.8) THEN
        CALL GETARG(2,DUMM)
        READ(DUMM,102) IMONTH
        CALL GETARG(3,DUMM)
        READ(DUMM,102) IDAY
        NB=1
        IT(2) = IDAY + MDAY(IT(1),IMONTH)
      ENDIF
      CALL GETARG(NB+3,DUMM)
      READ(DUMM,102) IT(3)
      CALL GETARG(NB+4,DUMM)
      READ(DUMM,102) IT(4)
      CALL GETARG(NB+5,DUMM)
      READ(DUMM,102) IT(5)
      CALL GETARG(NB+6,DUMM)
      READ(DUMM,104) IRNT
 104  FORMAT(I6)
      CALL GETARG(NB+7,DUMM)
      READ(DUMM,106) SAMP
 106  FORMAT(F7.0)

*+---------------------------------------------------------------------
*  Read in amplitudes and phases, in standard "Scherneck" form, from
*  standard input
*----------------------------------------------------------------------
      DO I=1,3
        READ(5,108) (TAMP(I,KK),KK=1,NTIN)
 108    FORMAT(1X,11F7.5)
      ENDDO
      DO I=1,3
        READ(5,110) (TPH(I,KK),KK=1,NTIN)
 110    FORMAT(1X,11F7.1)

* Change sign for phase, to be negative for lags

        DO KK=1,NTIN
          TPH(I,KK)=-TPH(I,KK)
        ENDDO
      ENDDO
*+---------------------------------------------------------------------
*
*  Find amplitudes and phases for all constituents, for each of the
*  three displacements. Note that the same frequencies are returned 
*  each time.
*
*  BLQ format order is vertical, horizontal EW, horizontal NS
*
*----------------------------------------------------------------------
      DO I=1,NTIN
        AMP(I)=TAMP(1,I)
        PHASE(I)=TPH(1,I)
      ENDDO
      CALL ADMINT(AMP,IDT,PHASE,AZ,F,PZ,NTIN,NTOUT)
      DO I=1,NTIN
        AMP(I)=TAMP(2,I)
        PHASE(I)=TPH(2,I)
      ENDDO
      CALL ADMINT(AMP,IDT,PHASE,AW,F,PW,NTIN,NTOUT)
      DO I=1,NTIN
        AMP(I)=TAMP(3,I)
        PHASE(I)=TPH(3,I)
      ENDDO
      CALL ADMINT(AMP,IDT,PHASE,AS,F,PS,NTIN,NTOUT)

*  set up for recursion, by normalizing frequencies, and converting
*  phases to radians

      DO I=1,NTOUT
        PZ(I) = DR*PZ(I)
        PS(I) = DR*PS(I)
        PW(I) = DR*PW(I)
        F(I) = SAMP*PI*F(I)/43200.D0
        WF(I) = F(I)
      ENDDO

*+---------------------------------------------------------------------
*
*  Loop over times, nl output points at a time. At the start of each
*  such block, convert from amp and phase to sin and cos (hc array) at
*  the start of the block. The computation of values within each
*  block is done recursively, since the times are equi-spaced.
*
*----------------------------------------------------------------------

 11   IRHI = MIN(IRLI+NL-1,IRNT)
      NP = IRHI - IRLI + 1

* Set up harmonic coefficients, compute tide, and write out
      DO I=1,NT
        HCZ(2*I-1) = AZ(I)*DCOS(PZ(I))
        HCZ(2*I)  = -AZ(I)*DSIN(PZ(I))
        HCS(2*I-1) = AS(I)*DCOS(PS(I))
        HCS(2*I)  = -AS(I)*DSIN(PS(I))
        HCW(2*I-1) = AW(I)*DCOS(PW(I))
        HCW(2*I)  = -AW(I)*DSIN(PW(I))
      ENDDO
      CALL RECURS(DZ,NP,HCZ,NTOUT,WF,SCR)
      CALL RECURS(DS,NP,HCS,NTOUT,WF,SCR)
      CALL RECURS(DW,NP,HCW,NTOUT,WF,SCR)
      WRITE(LUO,120) (DZ(I),DS(I),DW(I),I=1,NP)
 120  FORMAT(3F14.6)
      IF(IRHI.EQ.IRNT) STOP
      IRLI = IRHI + 1

*  Reset phases to the start of the new section
      DO I=1,NT
        PZ(I) = DMOD(PZ(I) + NP*F(I),2.D0*PI)
        PS(I) = DMOD(PS(I) + NP*F(I),2.D0*PI)
        PW(I) = DMOD(PW(I) + NP*F(I),2.D0*PI)
      ENDDO
      GO TO 11
      END

*-----------------------------------------------------------------------
      SUBROUTINE ADMINT (AMPIN,IDTIN,PHIN,AMP,F,P,NIN,NOUT)
*+
*  - - - - - - - - - - -
*   A D M I N T
*  - - - - - - - - - - -
*
*  This routine is part of the International Earth Rotation and
*  Reference Systems Service (IERS) Conventions software collection.
*
*  This subroutine returns the ocean loading displacement amplitude,
*  frequency, and phase of a set of tidal constituents generated by
*  the Bos-Scherneck website at http://www.oso.chalmers.se/~loading/.
*  The variable nin is input as the number wanted, and the variable 
*  nout is returned as the number provided.  The constituents used
*  are stored in the arrays idd (Doodson number) and tamp
*  (Cartwright-Edden amplitude).  The actual amp and phase of each
*  of these are determined by spline interpolation of the real and
*  imaginary part of the admittance, as specified at a subset of the
*  constituents.
*
*  In general, Class 1, 2, and 3 models represent physical effects that
*  act on geodetic parameters while canonical models provide lower-level
*  representations or basic computations that are used by Class 1, 2, or
*  3 models.
* 
*  Status:  Class 1 model
*
*     Class 1 models are those recommended to be used a priori in the
*     reduction of raw space geodetic data in order to determine
*     geodetic parameter estimates.
*     Class 2 models are those that eliminate an observational
*     singularity and are purely conventional in nature.
*     Class 3 models are those that are not required as either Class
*     1 or 2.
*     Canonical models are accepted as is and cannot be classified as
*     a Class 1, 2, or 3 model.
*
*  Given:
*     AMPIN       d      Cartwright-Edden amplitude of tidal constituents
*     IDTIN       i      Doodson number of tidal constituents
*     PHIN        d      Phase of tidal constituents
*     NIN         i      Number of harmonics used
*
*  Returned:
*     AMP         d      Amplitude due to ocean loading
*     F           d      Frequency due to ocean loading
*     P           d      Phase due to ocean loading
*     NOUT        i      Number of harmonics returned
*
*  Notes:
*
*  1) The phase is determined for a time set in COMMON block /date/ in
*     the subroutine TDFRPH.
*  
*  2) The arrays F and P must be specified as double precision. 
*
*  Called:
*     TDFRPH             Returns frequency and phase of a tidal
*                        constituent with given Doodson number            
*     SPLINE             Sets up array for cubic spline interpolation
*     EVAL               Performs cubic spline interpolation 
*     SHELLS             Sorts an array using Shell Sort
*
*  Test case:
*     Test cases are provided in the main program HARDISP.F.
*
*  References:
*
*     McCarthy, D. D., Petit, G. (eds.), IERS Conventions (2003),
*     IERS Technical Note No. 32, BKG (2004)
*
*  Revisions:  
*  2009 June 17 B.E. Stetzler  Initial changes to header
*  2009 June 18 B.E. Stetzler  Used IMPLICIT NONE, declared more variables,
*                              and added D0 to DOUBLE PRECISION variables 
*  2009 August 19 B.E.Stetzler Capitalized all variables for FORTRAN 77
*                              compatibility
*-----------------------------------------------------------------------

      IMPLICIT NONE
      INTEGER I,IDD,IDTIN,J,K,KEY,LL,NCON,NDI,NIN,NLP,NOUT,NSD,NT,II,KK

*+----------------------------------------------------------------------
*  The parameters below set the number of harmonics used in the prediction
*  (nt; This must also be set in the main program) and the number of
*  constituents whose amp and phase may be specified (ncon)
*-----------------------------------------------------------------------
      PARAMETER (NT=342)
      PARAMETER (NCON=20)

      REAL AIM,AMP,AMPIN,DI,DR,DTR,PHIN,RF,RL,SCR,SDI,SDR,TAMP,ZDI,ZDR,
     .     EVAL,AM,RE,SF
      DOUBLE PRECISION F,FR,P,PR

      DIMENSION AMPIN(*),IDTIN(6,*),PHIN(*)
      DIMENSION AMP(*),F(*),P(*)

*  Arrays containing information about all stored constituents
      DIMENSION IDD(6,NT),TAMP(NT)

*  Arrays containing information about the subset whose amp and phase may
*  be specified, and scratch arrays for the spline routines for which
*  at most ncon constituents may be specified.

      DIMENSION RL(NCON),AIM(NCON),RF(NCON),KEY(NCON),SCR(NCON),
     . ZDI(NCON),ZDR(NCON),DI(NCON),DR(NCON),SDI(NCON),SDR(NCON)
      DATA DTR/.01745329252/
      DATA RL/NCON*0.0/,AIM/NCON*0.0/,RF/NCON*0.0/
      DATA ZDI/NCON*0.0/,ZDR/NCON*0.0/,DI/NCON*0.0/,DR/NCON*0.0/
      DATA SDI/NCON*0.0/,SDR/NCON*0.0/
      DATA TAMP/
     .  .632208, .294107, .121046, .079915, .023818,-.023589, .022994,
     .  .019333,-.017871, .017192, .016018, .004671,-.004662,-.004519,
     .  .004470, .004467, .002589,-.002455,-.002172, .001972, .001947,
     .  .001914,-.001898, .001802, .001304, .001170, .001130, .001061,
     . -.001022,-.001017, .001014, .000901,-.000857, .000855, .000855,
     .  .000772, .000741, .000741,-.000721, .000698, .000658, .000654,
     . -.000653, .000633, .000626,-.000598, .000590, .000544, .000479,
     . -.000464, .000413,-.000390, .000373, .000366, .000366,-.000360,
     . -.000355, .000354, .000329, .000328, .000319, .000302, .000279,
     . -.000274,-.000272, .000248,-.000225, .000224,-.000223,-.000216,
     .  .000211, .000209, .000194, .000185,-.000174,-.000171, .000159,
     .  .000131, .000127, .000120, .000118, .000117, .000108, .000107,
     .  .000105,-.000102, .000102, .000099,-.000096, .000095,-.000089,
     . -.000085,-.000084,-.000081,-.000077,-.000072,-.000067, .000066,
     .  .000064, .000063, .000063, .000063, .000062, .000062,-.000060,
     .  .000056, .000053, .000051, .000050, .368645,-.262232,-.121995,
     . -.050208, .050031,-.049470, .020620, .020613, .011279,-.009530,
     . -.009469,-.008012, .007414,-.007300, .007227,-.007131,-.006644,
     .  .005249, .004137, .004087, .003944, .003943, .003420, .003418,
     .  .002885, .002884, .002160,-.001936, .001934,-.001798, .001690,
     .  .001689, .001516, .001514,-.001511, .001383, .001372, .001371,
     . -.001253,-.001075, .001020, .000901, .000865,-.000794, .000788,
     .  .000782,-.000747,-.000745, .000670,-.000603,-.000597, .000542,
     .  .000542,-.000541,-.000469,-.000440, .000438, .000422, .000410,
     . -.000374,-.000365, .000345, .000335,-.000321,-.000319, .000307,
     .  .000291, .000290,-.000289, .000286, .000275, .000271, .000263,
     . -.000245, .000225, .000225, .000221,-.000202,-.000200,-.000199,
     .  .000192, .000183, .000183, .000183,-.000170, .000169, .000168,
     .  .000162, .000149,-.000147,-.000141, .000138, .000136, .000136,
     .  .000127, .000127,-.000126,-.000121,-.000121, .000117,-.000116,
     . -.000114,-.000114,-.000114, .000114, .000113, .000109, .000108,
     .  .000106,-.000106,-.000106, .000105, .000104,-.000103,-.000100,
     . -.000100,-.000100, .000099,-.000098, .000093, .000093, .000090,
     . -.000088, .000083,-.000083,-.000082,-.000081,-.000079,-.000077,
     . -.000075,-.000075,-.000075, .000071, .000071,-.000071, .000068,
     .  .000068, .000065, .000065, .000064, .000064, .000064,-.000064,
     . -.000060, .000056, .000056, .000053, .000053, .000053,-.000053,
     .  .000053, .000053, .000052, .000050,-.066607,-.035184,-.030988,
     .  .027929,-.027616,-.012753,-.006728,-.005837,-.005286,-.004921,
     . -.002884,-.002583,-.002422, .002310, .002283,-.002037, .001883,
     . -.001811,-.001687,-.001004,-.000925,-.000844, .000766, .000766,
     . -.000700,-.000495,-.000492, .000491, .000483, .000437,-.000416,
     . -.000384, .000374,-.000312,-.000288,-.000273, .000259, .000245,
     . -.000232, .000229,-.000216, .000206,-.000204,-.000202, .000200,
     .  .000195,-.000190, .000187, .000180,-.000179, .000170, .000153,
     . -.000137,-.000119,-.000119,-.000112,-.000110,-.000110, .000107,
     . -.000095,-.000095,-.000091,-.000090,-.000081,-.000079,-.000079,
     .  .000077,-.000073, .000069,-.000067,-.000066, .000065, .000064,
     . -.000062, .000060, .000059,-.000056, .000055,-.000051/
      DATA IDD/
     .  2, 0, 0, 0, 0, 0,   2, 2,-2, 0, 0, 0,   2,-1, 0, 1, 0, 0,  
     .  2, 2, 0, 0, 0, 0,   2, 2, 0, 0, 1, 0,   2, 0, 0, 0,-1, 0,  
     .  2,-1, 2,-1, 0, 0,   2,-2, 2, 0, 0, 0,   2, 1, 0,-1, 0, 0,  
     .  2, 2,-3, 0, 0, 1,   2,-2, 0, 2, 0, 0,   2,-3, 2, 1, 0, 0,  
     .  2, 1,-2, 1, 0, 0,   2,-1, 0, 1,-1, 0,   2, 3, 0,-1, 0, 0,  
     .  2, 1, 0, 1, 0, 0,   2, 2, 0, 0, 2, 0,   2, 2,-1, 0, 0,-1,  
     .  2, 0,-1, 0, 0, 1,   2, 1, 0, 1, 1, 0,   2, 3, 0,-1, 1, 0,  
     .  2, 0, 1, 0, 0,-1,   2, 0,-2, 2, 0, 0,   2,-3, 0, 3, 0, 0,  
     .  2,-2, 3, 0, 0,-1,   2, 4, 0, 0, 0, 0,   2,-1, 1, 1, 0,-1,  
     .  2,-1, 3,-1, 0,-1,   2, 2, 0, 0,-1, 0,   2,-1,-1, 1, 0, 1,  
     .  2, 4, 0, 0, 1, 0,   2,-3, 4,-1, 0, 0,   2,-1, 2,-1,-1, 0,  
     .  2, 3,-2, 1, 0, 0,   2, 1, 2,-1, 0, 0,   2,-4, 2, 2, 0, 0,  
     .  2, 4,-2, 0, 0, 0,   2, 0, 2, 0, 0, 0,   2,-2, 2, 0,-1, 0,  
     .  2, 2,-4, 0, 0, 2,   2, 2,-2, 0,-1, 0,   2, 1, 0,-1,-1, 0,  
     .  2,-1, 1, 0, 0, 0,   2, 2,-1, 0, 0, 1,   2, 2, 1, 0, 0,-1,  
     .  2,-2, 0, 2,-1, 0,   2,-2, 4,-2, 0, 0,   2, 2, 2, 0, 0, 0,  
     .  2,-4, 4, 0, 0, 0,   2,-1, 0,-1,-2, 0,   2, 1, 2,-1, 1, 0,  
     .  2,-1,-2, 3, 0, 0,   2, 3,-2, 1, 1, 0,   2, 4, 0,-2, 0, 0,  
     .  2, 0, 0, 2, 0, 0,   2, 0, 2,-2, 0, 0,   2, 0, 2, 0, 1, 0,  
     .  2,-3, 3, 1, 0,-1,   2, 0, 0, 0,-2, 0,   2, 4, 0, 0, 2, 0,  
     .  2, 4,-2, 0, 1, 0,   2, 0, 0, 0, 0, 2,   2, 1, 0, 1, 2, 0,  
     .  2, 0,-2, 0,-2, 0,   2,-2, 1, 0, 0, 1,   2,-2, 1, 2, 0,-1,  
     .  2,-1, 1,-1, 0, 1,   2, 5, 0,-1, 0, 0,   2, 1,-3, 1, 0, 1,  
     .  2,-2,-1, 2, 0, 1,   2, 3, 0,-1, 2, 0,   2, 1,-2, 1,-1, 0,  
     .  2, 5, 0,-1, 1, 0,   2,-4, 0, 4, 0, 0,   2,-3, 2, 1,-1, 0,  
     .  2,-2, 1, 1, 0, 0,   2, 4, 0,-2, 1, 0,   2, 0, 0, 2, 1, 0,  
     .  2,-5, 4, 1, 0, 0,   2, 0, 2, 0, 2, 0,   2,-1, 2, 1, 0, 0,  
     .  2, 5,-2,-1, 0, 0,   2, 1,-1, 0, 0, 0,   2, 2,-2, 0, 0, 2,  
     .  2,-5, 2, 3, 0, 0,   2,-1,-2, 1,-2, 0,   2,-3, 5,-1, 0,-1,  
     .  2,-1, 0, 0, 0, 1,   2,-2, 0, 0,-2, 0,   2, 0,-1, 1, 0, 0,  
     .  2,-3, 1, 1, 0, 1,   2, 3, 0,-1,-1, 0,   2, 1, 0, 1,-1, 0,  
     .  2,-1, 2, 1, 1, 0,   2, 0,-3, 2, 0, 1,   2, 1,-1,-1, 0, 1,  
     .  2,-3, 0, 3,-1, 0,   2, 0,-2, 2,-1, 0,   2,-4, 3, 2, 0,-1,  
     .  2,-1, 0, 1,-2, 0,   2, 5, 0,-1, 2, 0,   2,-4, 5, 0, 0,-1,  
     .  2,-2, 4, 0, 0,-2,   2,-1, 0, 1, 0, 2,   2,-2,-2, 4, 0, 0,  
     .  2, 3,-2,-1,-1, 0,   2,-2, 5,-2, 0,-1,   2, 0,-1, 0,-1, 1,  
     .  2, 5,-2,-1, 1, 0,   1, 1, 0, 0, 0, 0,   1,-1, 0, 0, 0, 0,  
     .  1, 1,-2, 0, 0, 0,   1,-2, 0, 1, 0, 0,   1, 1, 0, 0, 1, 0,  
     .  1,-1, 0, 0,-1, 0,   1, 2, 0,-1, 0, 0,   1, 0, 0, 1, 0, 0,  
     .  1, 3, 0, 0, 0, 0,   1,-2, 2,-1, 0, 0,   1,-2, 0, 1,-1, 0,  
     .  1,-3, 2, 0, 0, 0,   1, 0, 0,-1, 0, 0,   1, 1, 0, 0,-1, 0,  
     .  1, 3, 0, 0, 1, 0,   1, 1,-3, 0, 0, 1,   1,-3, 0, 2, 0, 0,  
     .  1, 1, 2, 0, 0, 0,   1, 0, 0, 1, 1, 0,   1, 2, 0,-1, 1, 0,  
     .  1, 0, 2,-1, 0, 0,   1, 2,-2, 1, 0, 0,   1, 3,-2, 0, 0, 0,  
     .  1,-1, 2, 0, 0, 0,   1, 1, 1, 0, 0,-1,   1, 1,-1, 0, 0, 1,  
     .  1, 4, 0,-1, 0, 0,   1,-4, 2, 1, 0, 0,   1, 0,-2, 1, 0, 0,  
     .  1,-2, 2,-1,-1, 0,   1, 3, 0,-2, 0, 0,   1,-1, 0, 2, 0, 0,  
     .  1,-1, 0, 0,-2, 0,   1, 3, 0, 0, 2, 0,   1,-3, 2, 0,-1, 0,  
     .  1, 4, 0,-1, 1, 0,   1, 0, 0,-1,-1, 0,   1, 1,-2, 0,-1, 0,  
     .  1,-3, 0, 2,-1, 0,   1, 1, 0, 0, 2, 0,   1, 1,-1, 0, 0,-1,  
     .  1,-1,-1, 0, 0, 1,   1, 0, 2,-1, 1, 0,   1,-1, 1, 0, 0,-1,  
     .  1,-1,-2, 2, 0, 0,   1, 2,-2, 1, 1, 0,   1,-4, 0, 3, 0, 0,  
     .  1,-1, 2, 0, 1, 0,   1, 3,-2, 0, 1, 0,   1, 2, 0,-1,-1, 0,  
     .  1, 0, 0, 1,-1, 0,   1,-2, 2, 1, 0, 0,   1, 4,-2,-1, 0, 0,  
     .  1,-3, 3, 0, 0,-1,   1,-2, 1, 1, 0,-1,   1,-2, 3,-1, 0,-1,  
     .  1, 0,-2, 1,-1, 0,   1,-2,-1, 1, 0, 1,   1, 4,-2, 1, 0, 0,  
     .  1,-4, 4,-1, 0, 0,   1,-4, 2, 1,-1, 0,   1, 5,-2, 0, 0, 0,  
     .  1, 3, 0,-2, 1, 0,   1,-5, 2, 2, 0, 0,   1, 2, 0, 1, 0, 0,  
     .  1, 1, 3, 0, 0,-1,   1,-2, 0, 1,-2, 0,   1, 4, 0,-1, 2, 0,  
     .  1, 1,-4, 0, 0, 2,   1, 5, 0,-2, 0, 0,   1,-1, 0, 2, 1, 0,  
     .  1,-2, 1, 0, 0, 0,   1, 4,-2, 1, 1, 0,   1,-3, 4,-2, 0, 0,  
     .  1,-1, 3, 0, 0,-1,   1, 3,-3, 0, 0, 1,   1, 5,-2, 0, 1, 0,  
     .  1, 1, 2, 0, 1, 0,   1, 2, 0, 1, 1, 0,   1,-5, 4, 0, 0, 0,  
     .  1,-2, 0,-1,-2, 0,   1, 5, 0,-2, 1, 0,   1, 1, 2,-2, 0, 0,  
     .  1, 1,-2, 2, 0, 0,   1,-2, 2, 1, 1, 0,   1, 0, 3,-1, 0,-1,  
     .  1, 2,-3, 1, 0, 1,   1,-2,-2, 3, 0, 0,   1,-1, 2,-2, 0, 0,  
     .  1,-4, 3, 1, 0,-1,   1,-4, 0, 3,-1, 0,   1,-1,-2, 2,-1, 0,  
     .  1,-2, 0, 3, 0, 0,   1, 4, 0,-3, 0, 0,   1, 0, 1, 1, 0,-1,  
     .  1, 2,-1,-1, 0, 1,   1, 2,-2, 1,-1, 0,   1, 0, 0,-1,-2, 0,  
     .  1, 2, 0, 1, 2, 0,   1, 2,-2,-1,-1, 0,   1, 0, 0, 1, 2, 0,  
     .  1, 0, 1, 0, 0, 0,   1, 2,-1, 0, 0, 0,   1, 0, 2,-1,-1, 0,  
     .  1,-1,-2, 0,-2, 0,   1,-3, 1, 0, 0, 1,   1, 3,-2, 0,-1, 0,  
     .  1,-1,-1, 0,-1, 1,   1, 4,-2,-1, 1, 0,   1, 2, 1,-1, 0,-1,  
     .  1, 0,-1, 1, 0, 1,   1,-2, 4,-1, 0, 0,   1, 4,-4, 1, 0, 0,  
     .  1,-3, 1, 2, 0,-1,   1,-3, 3, 0,-1,-1,   1, 1, 2, 0, 2, 0,  
     .  1, 1,-2, 0,-2, 0,   1, 3, 0, 0, 3, 0,   1,-1, 2, 0,-1, 0,  
     .  1,-2, 1,-1, 0, 1,   1, 0,-3, 1, 0, 1,   1,-3,-1, 2, 0, 1,  
     .  1, 2, 0,-1, 2, 0,   1, 6,-2,-1, 0, 0,   1, 2, 2,-1, 0, 0,  
     .  1,-1, 1, 0,-1,-1,   1,-2, 3,-1,-1,-1,   1,-1, 0, 0, 0, 2,  
     .  1,-5, 0, 4, 0, 0,   1, 1, 0, 0, 0,-2,   1,-2, 1, 1,-1,-1,  
     .  1, 1,-1, 0, 1, 1,   1, 1, 2, 0, 0,-2,   1,-3, 1, 1, 0, 0,  
     .  1,-4, 4,-1,-1, 0,   1, 1, 0,-2,-1, 0,   1,-2,-1, 1,-1, 1,  
     .  1,-3, 2, 2, 0, 0,   1, 5,-2,-2, 0, 0,   1, 3,-4, 2, 0, 0,  
     .  1, 1,-2, 0, 0, 2,   1,-1, 4,-2, 0, 0,   1, 2, 2,-1, 1, 0,  
     .  1,-5, 2, 2,-1, 0,   1, 1,-3, 0,-1, 1,   1, 1, 1, 0, 1,-1,  
     .  1, 6,-2,-1, 1, 0,   1,-2, 2,-1,-2, 0,   1, 4,-2, 1, 2, 0,  
     .  1,-6, 4, 1, 0, 0,   1, 5,-4, 0, 0, 0,   1,-3, 4, 0, 0, 0,  
     .  1, 1, 2,-2, 1, 0,   1,-2, 1, 0,-1, 0,   0, 2, 0, 0, 0, 0,  
     .  0, 1, 0,-1, 0, 0,   0, 0, 2, 0, 0, 0,   0, 0, 0, 0, 1, 0,  
     .  0, 2, 0, 0, 1, 0,   0, 3, 0,-1, 0, 0,   0, 1,-2, 1, 0, 0,  
     .  0, 2,-2, 0, 0, 0,   0, 3, 0,-1, 1, 0,   0, 0, 1, 0, 0,-1,  
     .  0, 2, 0,-2, 0, 0,   0, 2, 0, 0, 2, 0,   0, 3,-2, 1, 0, 0,  
     .  0, 1, 0,-1,-1, 0,   0, 1, 0,-1, 1, 0,   0, 4,-2, 0, 0, 0,  
     .  0, 1, 0, 1, 0, 0,   0, 0, 3, 0, 0,-1,   0, 4, 0,-2, 0, 0,  
     .  0, 3,-2, 1, 1, 0,   0, 3,-2,-1, 0, 0,   0, 4,-2, 0, 1, 0,  
     .  0, 0, 2, 0, 1, 0,   0, 1, 0, 1, 1, 0,   0, 4, 0,-2, 1, 0,  
     .  0, 3, 0,-1, 2, 0,   0, 5,-2,-1, 0, 0,   0, 1, 2,-1, 0, 0,  
     .  0, 1,-2, 1,-1, 0,   0, 1,-2, 1, 1, 0,   0, 2,-2, 0,-1, 0,  
     .  0, 2,-3, 0, 0, 1,   0, 2,-2, 0, 1, 0,   0, 0, 2,-2, 0, 0,  
     .  0, 1,-3, 1, 0, 1,   0, 0, 0, 0, 2, 0,   0, 0, 1, 0, 0, 1,  
     .  0, 1, 2,-1, 1, 0,   0, 3, 0,-3, 0, 0,   0, 2, 1, 0, 0,-1,  
     .  0, 1,-1,-1, 0, 1,   0, 1, 0, 1, 2, 0,   0, 5,-2,-1, 1, 0,  
     .  0, 2,-1, 0, 0, 1,   0, 2, 2,-2, 0, 0,   0, 1,-1, 0, 0, 0,  
     .  0, 5, 0,-3, 0, 0,   0, 2, 0,-2, 1, 0,   0, 1, 1,-1, 0,-1,  
     .  0, 3,-4, 1, 0, 0,   0, 0, 2, 0, 2, 0,   0, 2, 0,-2,-1, 0,  
     .  0, 4,-3, 0, 0, 1,   0, 3,-1,-1, 0, 1,   0, 0, 2, 0, 0,-2,  
     .  0, 3,-3, 1, 0, 1,   0, 2,-4, 2, 0, 0,   0, 4,-2,-2, 0, 0,  
     .  0, 3, 1,-1, 0,-1,   0, 5,-4, 1, 0, 0,   0, 3,-2,-1,-1, 0,  
     .  0, 3,-2, 1, 2, 0,   0, 4,-4, 0, 0, 0,   0, 6,-2,-2, 0, 0,  
     .  0, 5, 0,-3, 1, 0,   0, 4,-2, 0, 2, 0,   0, 2, 2,-2, 1, 0,  
     .  0, 0, 4, 0, 0,-2,   0, 3,-1, 0, 0, 0,   0, 3,-3,-1, 0, 1,  
     .  0, 4, 0,-2, 2, 0,   0, 1,-2,-1,-1, 0,   0, 2,-1, 0, 0,-1,  
     .  0, 4,-4, 2, 0, 0,   0, 2, 1, 0, 1,-1,   0, 3,-2,-1, 1, 0,  
     .  0, 4,-3, 0, 1, 1,   0, 2, 0, 0, 3, 0,   0, 6,-4, 0, 0, 0/

*  Initialize variables.
      K   = 0
      NLP = 0
      NDI = 0
      NSD = 0

      DO LL=1,NIN
*  See if Doodson numbers match
         DO KK=1,NT
            II = 0
            DO I=1,6
               II = II + IABS(IDD(I,KK)-IDTIN(I,LL))
            ENDDO
            IF(II.EQ.0) GO TO 5
         ENDDO
*  If you have a match, put line into array
 5       IF(II.EQ.0.AND.K.LT.NCON) THEN
            K = K + 1
            RL(K) = AMPIN(LL)*COS(DTR*PHIN(LL))/ABS(TAMP(KK))
            AIM(K)= AMPIN(LL)*SIN(DTR*PHIN(LL))/ABS(TAMP(KK))
*+---------------------------------------------------------------------
*  Now have real and imaginary parts of admittance, scaled by Cartwright-
*  Edden amplitude. Admittance phase is whatever was used in the original
*  expression. (Usually phase is given relative to some reference,
*  but amplitude is in absolute units). Next get frequency.
*----------------------------------------------------------------------
            CALL TDFRPH(IDD(1,KK),FR,PR)
            RF(K) = FR
         ENDIF
      ENDDO
*+---------------------------------------------------------------------
*  Done going through constituents; there are k of them.
*  Have specified admittance at a number of points. Sort these by frequency
*  and separate diurnal and semidiurnal, recopying admittances to get them
*  in order using Shell Sort.
*----------------------------------------------------------------------

      CALL SHELLS(RF,KEY,K)
      DO I=1,K
         IF(RF(I).LT.0.5) NLP = NLP + 1
         IF(RF(I).LT.1.5.AND.RF(I).GT.0.5) NDI = NDI + 1
         IF(RF(I).LT.2.5.AND.RF(I).GT.1.5) NSD = NSD + 1
         SCR(I) = RL(KEY(I))
      ENDDO
      DO I=1,K
         RL(I) = SCR(I)
         SCR(I) = AIM(KEY(I))
      ENDDO
      DO I=1,K
         AIM(I) = SCR(I)
      ENDDO
*+---------------------------------------------------------------------
*  now set up splines (8 cases - four species, each real and imaginary)
*  We have to allow for the case when there are no constituent amplitudes
*  for the long-period tides.
*----------------------------------------------------------------------
      IF(NLP.NE.0) CALL SPLINE(NLP,RF,RL,ZDR,SCR)
      IF(NLP.NE.0) CALL SPLINE(NLP,RF,AIM,ZDI,SCR)
      CALL SPLINE(NDI,RF(NLP+1),RL(NLP+1),DR,SCR)
      CALL SPLINE(NDI,RF(NLP+1),AIM(NLP+1),DI,SCR)
      CALL SPLINE(NSD,RF(NLP+NDI+1),RL(NLP+NDI+1),SDR,SCR)
      CALL SPLINE(NSD,RF(NLP+NDI+1),AIM(NLP+NDI+1),SDI,SCR)
*  Evaluate all harmonics using the interpolated admittance
      J = 1
      DO I=1,NT
         IF(IDD(1,I).EQ.0.AND.NLP.EQ.0) GO TO 11
         CALL TDFRPH(IDD(1,I),F(J),P(J))
*  Compute phase corrections to equilibrium tide using function EVAL
         IF(IDD(1,I).EQ.0) P(J) = P(J) + 180.
         IF(IDD(1,I).EQ.1) P(J) = P(J) + 90.
         SF = F(J)
         IF(IDD(1,I).EQ.0) RE = EVAL(SF,NLP,RF,RL,ZDR)
         IF(IDD(1,I).EQ.0) AM = EVAL(SF,NLP,RF,AIM,ZDI)
         IF(IDD(1,I).EQ.1) RE = EVAL(SF,NDI,RF(NLP+1),RL(NLP+1),DR)
         IF(IDD(1,I).EQ.1) AM = EVAL(SF,NDI,RF(NLP+1),AIM(NLP+1),DI)
         IF(IDD(1,I).EQ.2) RE =
     .      EVAL(SF,NSD,RF(NLP+NDI+1),RL(NLP+NDI+1),SDR)
         IF(IDD(1,I).EQ.2) AM =
     .      EVAL(SF,NSD,RF(NLP+NDI+1),AIM(NLP+NDI+1),SDI)
         AMP(J) = TAMP(I)*SQRT(RE**2+AM**2)
         P(J) = P(J) + ATAN2(AM,RE)/DTR
         IF(P(J).GT.180) P(J)=P(J)-360.
         J = J + 1
 11      CONTINUE
      ENDDO
      NOUT = J - 1
      RETURN
      END

*+----------------------------------------------------------------------
      SUBROUTINE TDFRPH (IDOOD,FREQ,PHASE)
*+
*  - - - - - - - - - - -
*   T D F R P H 
*  - - - - - - - - - - -
*
*  This routine is part of the International Earth Rotation and
*  Reference Systems Service (IERS) Conventions software collection.
*
*  This subroutine returns the frequency and phase of a tidal
*  constituent when its Doodson number is given as input. 
*
*  In general, Class 1, 2, and 3 models represent physical effects that
*  act on geodetic parameters while canonical models provide lower-level
*  representations or basic computations that are used by Class 1, 2, or
*  3 models.
* 
*  Status:  Class 1 model
*
*     Class 1 models are those recommended to be used a priori in the
*     reduction of raw space geodetic data in order to determine
*     geodetic parameter estimates.
*     Class 2 models are those that eliminate an observational
*     singularity and are purely conventional in nature.
*     Class 3 models are those that are not required as either Class
*     1 or 2.
*     Canonical models are accepted as is and cannot be classified as
*     a Class 1, 2, or 3 model.
*
*  Given:
*     idood       i      Doodson number of a tidal constituent
*
*  Returned:
*     freq        d      Frequency of a tidal constituent
*     phase       d      Phase of a tidal constituent (Note 1)
*
*  Notes:
*
*  1) The phases must be decreased by 90 degrees if the sum of the order 
*     and the species number is odd (as for the 2nd degree diurnals, and 
*     3rd degree low frequency and semidiurnals).
*     
*     These phases may need further adjustment to allow for the spherical
*     harmonic normalization used; e.g. for that used for the potential
*     by Cartwright and Tayler, 180 degrees must be added for (species,
*     order) = (1,2), (1,3), or (3,3). 
*
*  Called:
*     TOYMD     Converts year-day of year to year-month-day format 
*     LEAP      Returns true if a given year is a leap year
*     JULDAT    Converts Gregorian date to Julian date 
*     ETUTC     Returns difference of Epheremis Time (ET) and 
*               Coordinated Universal Time (UTC) 
*
*  Test case:
*     given input: For June 25, 2009 0 Hr 0 Min, M2 tide
*                  DATA IDOOD = / 2, 0, 0, 0, 0, 0 /  
*
*     expected output: FREQ = 1.93227361605688D0
*                      PHASE = 132.8193176853237674D0
*
*  References:
*
*     Petit, G. and Luzum, B. (eds.), IERS Conventions (2010),
*     IERS Technical Note No. 36, BKG (2010)
*
*  Revisions:  
*  2009 June   15 B.E.Stetzler  Initial changes to code 
*  2009 August 19 B.E.Stetzler  Capitalized all variables for FORTRAN
*                               77 compatibility
*  2010 March  19 B.E.Stetzler  Provided test case
*-----------------------------------------------------------------------

      IMPLICIT NONE
      SAVE ITMSAVE,D,DD
      INTEGER I,IDOOD,INITIAL,ITM,ITM2,ITMSAVE,JD,JULDAT,LEAP
      DOUBLE PRECISION YEAR,DELTA,FREQ,PHASE,
     .                 D,DAYFR,DD,DJD,F1,F2,F3,F4,F5,
     .                 FD1,FD2,FD3,FD4,FD5,T
      DIMENSION IDOOD(6),ITM2(6),ITMSAVE(5),D(6),DD(6)

* Common block 'date' stores time information in Universal Time (UT)

      COMMON/DATE/ITM(5)
      DATA ITMSAVE/5*0/
*------------------------------------------------------------------------
*  Test to see if time has changed; if so, set the phases and frequencies
*  for each of the Doodson arguments
*------------------------------------------------------------------------
      INITIAL=0
      DO I=1,5
        IF(ITM(I).NE.ITMSAVE(I)) INITIAL=1
      ENDDO

      IF(INITIAL.EQ.1) THEN
        DO I=1,5
           ITMSAVE(I) = ITM(I)
        ENDDO

* Convert times to Julian days (UT) then to Julian centuries from J2000.0
*   (ET)

        CALL TOYMD(ITM,ITM2)
        JD = JULDAT(ITM2)
        DAYFR=  ITM(3)/24.0D0 + ITM(4)/1440.0D0 + ITM(5)/84600.0D0
        YEAR=ITM(1)+(ITM(2)+DAYFR)/(365.0D0+LEAP(ITM(1)))
        CALL ETUTC(YEAR,DELTA)
        DJD= JD - 0.5D0 + DAYFR
        T = (DJD - 2451545.0D0 + DELTA/86400.0D0)/36525.0D0


* IERS expressions for the Delaunay arguments, in degrees

        F1 =     134.9634025100D0 +
     .    T*( 477198.8675605000D0 +
     .    T*(      0.0088553333D0 +
     .    T*(      0.0000143431D0 +
     .    T*(     -0.0000000680D0 ))))
        F2 =     357.5291091806D0 +
     .    T*(  35999.0502911389D0 +
     .    T*(     -0.0001536667D0 +
     .    T*(      0.0000000378D0 +
     .    T*(     -0.0000000032D0 ))))
        F3 =      93.2720906200D0 +
     .    T*( 483202.0174577222D0 +
     .    T*(     -0.0035420000D0 +
     .    T*(     -0.0000002881D0 +
     .    T*(      0.0000000012D0 ))))
        F4 =     297.8501954694D0 +
     .    T*( 445267.1114469445D0 +
     .    T*(     -0.0017696111D0 +
     .    T*(      0.0000018314D0 +
     .    T*(     -0.0000000088D0 ))))
        F5 =     125.0445550100D0 +
     .    T*(  -1934.1362619722D0 +
     .    T*(      0.0020756111D0 +
     .    T*(      0.0000021394D0 +
     .    T*(     -0.0000000165D0 ))))

*  Convert to Doodson (Darwin) variables

        D(1) = 360.0D0*DAYFR - F4
        D(2) = F3 + F5
        D(3) = D(2) - F4
        D(4) = D(2) - F1
        D(5) = -F5
        D(6) = D(3) - F2

*  Find frequencies of Delauney variables (in cycles/day), and from these
*  the same for the Doodson arguments

        FD1 =  0.0362916471D0 + 0.0000000013D0*T
        FD2 =  0.0027377786D0
        FD3 =  0.0367481951D0 - 0.0000000005D0*T
        FD4 =  0.0338631920D0 - 0.0000000003D0*T
        FD5 = -0.0001470938D0 + 0.0000000003D0*T
        DD(1) = 1.0D0 - FD4
        DD(2) = FD3 + FD5
        DD(3) = DD(2) - FD4
        DD(4) = DD(2) - FD1
        DD(5) = -FD5
        DD(6) = DD(3) - FD2
      ENDIF

*  End of intialization (likely to be called only once)

*  Compute phase and frequency of the given tidal constituent

      FREQ=0.0D0
      PHASE=0.0D0
      DO I = 1,6
         FREQ =  FREQ + IDOOD(I)*DD(I)
         PHASE = PHASE + IDOOD(I)*D(I)
      ENDDO

* Adjust phases so that they fall in the positive range 0 to 360
      PHASE = DMOD(PHASE,360.0D0)
      IF(PHASE.LT.0.0D0) PHASE = PHASE + 360.0D0

      RETURN
      END

*+----------------------------------------------------------------------
      SUBROUTINE TDFRPH2 (IDOOD,FREQ,PHASE)
*+
*  - - - - - - - - - - -
*   T D F R P H 2
*  - - - - - - - - - - -
*
*  This routine is part of the International Earth Rotation and
*  Reference Systems Service (IERS) Conventions software collection.
*
*  This subroutine returns the frequency and phase of a tidal
*  constituent when its Doodson number is given as input. 
*
*  In general, Class 1, 2, and 3 models represent physical effects that
*  act on geodetic parameters while canonical models provide lower-level
*  representations or basic computations that are used by Class 1, 2, or
*  3 models.
* 
*  Status:  Class 1 model
*
*     Class 1 models are those recommended to be used a priori in the
*     reduction of raw space geodetic data in order to determine
*     geodetic parameter estimates.
*     Class 2 models are those that eliminate an observational
*     singularity and are purely conventional in nature.
*     Class 3 models are those that are not required as either Class
*     1 or 2.
*     Canonical models are accepted as is and cannot be classified as
*     a Class 1, 2, or 3 model.
*
*  Given:
*     idood       i      Doodson number of a tidal constituent
*
*  Returned:
*     freq        d      Frequency of a tidal constituent
*     phase       d      Phase of a tidal constituent (Note 1)
*
*  Notes:
*
*  1) The phases must be decreased by 90 degrees if the sum of the order 
*     and the species number is odd (as for the 2nd degree diurnals, and 
*     3rd degree low frequency and semidiurnals).
*     
*     These phases may need further adjustment to allow for the spherical
*     harmonic normalization used; e.g. for that used for the potential
*     by Cartwright and Tayler, 180 degrees must be added for (species,
*     order) = (1,2), (1,3), or (3,3). 
*
*  Called:
*     TOYMD     Converts year-day of year to year-month-day format 
*     LEAP      Returns true if a given year is a leap year
*     JULDAT    Converts Gregorian date to Julian date 
*     ETUTC     Returns difference of Epheremis Time (ET) and 
*               Coordinated Universal Time (UTC) 
*
*  Test case:
*     given input: For June 25, 2009 0 Hr 0 Min, M2 tide
*                  DATA IDOOD = / 2, 0, 0, 0, 0, 0 /  
*
*     expected output: FREQ = 1.93227361605688D0
*                      PHASE = 132.8193176853237674D0
*
*  References:
*
*     Petit, G. and Luzum, B. (eds.), IERS Conventions (2010),
*     IERS Technical Note No. 36, BKG (2010)
*
*  Revisions:  
*  2009 June   15 B.E.Stetzler  Initial changes to code 
*  2009 August 19 B.E.Stetzler  Capitalized all variables for FORTRAN
*                               77 compatibility
*  2010 March  19 B.E.Stetzler  Provided test case
*-----------------------------------------------------------------------

      IMPLICIT NONE
      SAVE ITMSAVE,D,DD
      INTEGER I,IDOOD,INITIAL,ITM,ITM2,ITMSAVE,JD,JULDAT,LEAP
      DOUBLE PRECISION YEAR,DELTA,FREQ,PHASE,
     .                 D,DAYFR,DD,DJD,F1,F2,F3,F4,F5,
     .                 FD1,FD2,FD3,FD4,FD5,T
      DIMENSION IDOOD(6),ITM2(6),ITMSAVE(5),D(6),DD(6)

* Common block 'date' stores time information in Universal Time (UT)

      COMMON/DATE/ITM(5)
      DATA ITMSAVE/5*0/
*------------------------------------------------------------------------
*  Test to see if time has changed; if so, set the phases and frequencies
*  for each of the Doodson arguments
*------------------------------------------------------------------------
      INITIAL=0
      DO I=1,5
        IF(ITM(I).NE.ITMSAVE(I)) INITIAL=1
      ENDDO

      IF(INITIAL.EQ.1) THEN
        DO I=1,5
           ITMSAVE(I) = ITM(I)
        ENDDO

* Convert times to Julian days (UT) then to Julian centuries from J2000.0
*   (ET)

        CALL TOYMD(ITM,ITM2)
        JD = JULDAT(ITM2)
        DAYFR=  ITM(3)/24.0D0 + ITM(4)/1440.0D0 + ITM(5)/84600.0D0
        YEAR=ITM(1)+(ITM(2)+DAYFR)/(365.0D0+LEAP(ITM(1)))
        CALL ETUTC(YEAR,DELTA)
        DJD= JD - 0.5D0 + DAYFR
        T = (DJD - 2451545.0D0 + DELTA/86400.0D0)/36525.0D0


* IERS expressions for the Delaunay arguments, in degrees

        F1 =     134.9634025100D0 +
     .    T*( 477198.8675605000D0 +
     .    T*(      0.0088553333D0 +
     .    T*(      0.0000143431D0 +
     .    T*(     -0.0000000680D0 ))))
        F2 =     357.5291091806D0 +
     .    T*(  35999.0502911389D0 +
     .    T*(     -0.0001536667D0 +
     .    T*(      0.0000000378D0 +
     .    T*(     -0.0000000032D0 ))))
        F3 =      93.2720906200D0 +
     .    T*( 483202.0174577222D0 +
     .    T*(     -0.0035420000D0 +
     .    T*(     -0.0000002881D0 +
     .    T*(      0.0000000012D0 ))))
        F4 =     297.8501954694D0 +
     .    T*( 445267.1114469445D0 +
     .    T*(     -0.0017696111D0 +
     .    T*(      0.0000018314D0 +
     .    T*(     -0.0000000088D0 ))))
        F5 =     125.0445550100D0 +
     .    T*(  -1934.1362619722D0 +
     .    T*(      0.0020756111D0 +
     .    T*(      0.0000021394D0 +
     .    T*(     -0.0000000165D0 ))))

*  Convert to Doodson (Darwin) variables

        D(1) = 360.0D0*DAYFR - F4
        D(2) = F3 + F5
        D(3) = D(2) - F4
        D(4) = D(2) - F1
        D(5) = -F5
        D(6) = D(3) - F2

*  Find frequencies of Delauney variables (in cycles/day), and from these
*  the same for the Doodson arguments

        FD1 =  0.0362916471D0 + 0.0000000013D0*T
        FD2 =  0.0027377786D0
        FD3 =  0.0367481951D0 - 0.0000000005D0*T
        FD4 =  0.0338631920D0 - 0.0000000003D0*T
        FD5 = -0.0001470938D0 + 0.0000000003D0*T
        DD(1) = 1.0D0 - FD4
        DD(2) = FD3 + FD5
        DD(3) = DD(2) - FD4
        DD(4) = DD(2) - FD1
        DD(5) = -FD5
        DD(6) = DD(3) - FD2
      ENDIF

*  End of intialization (likely to be called only once)

*  Compute phase and frequency of the given tidal constituent

      FREQ=0.0D0
      PHASE=0.0D0
      DO I = 1,6
         FREQ =  FREQ + IDOOD(I)*DD(I)
         PHASE = PHASE + IDOOD(I)*D(I)
      ENDDO

* Adjust phases so that they fall in the positive range 0 to 360
      PHASE = DMOD(PHASE,360.0D0)
      IF(PHASE.LT.0.0D0) PHASE = PHASE + 360.0D0

      RETURN
      END

*+----------------------------------------------------------------------
      SUBROUTINE RECURS(X,N,HC,NF,OM,SCR)
*+
*  - - - - - - - - -
*   R E C U R S
*  - - - - - - - - -
*
*  This routine is part of the International Earth Rotation and
*  Reference Systems Service (IERS) Conventions software collection.
*
*  The purpose of the subroutine is to perform sine and cosine recursion
*  to fill in data x, of length n, for nf sines and cosines with frequencies
*  om. 
*
*  In general, Class 1, 2, and 3 models represent physical effects that
*  act on geodetic parameters while canonical models provide lower-level
*  representations or basic computations that are used by Class 1, 2, or
*  3 models.
* 
*  Status: Canonical model	
* 
*     Class 1 models are those recommended to be used a priori in the
*     reduction of raw space geodetic data in order to determine
*     geodetic parameter estimates.
*     Class 2 models are those that eliminate an observational
*     singularity and are purely conventional in nature.
*     Class 3 models are those that are not required as either Class
*     1 or 2.
*     Canonical models are accepted as is and cannot be classified as a
*     Class 1, 2, or 3 model.
*
*  Given: This is a support routine of the main program HARDISP.F.
*     x              d      data provided from a file given as standard
*                           input from the MAIN program HARDISP.F (Note 1)
*     n              i      length of the data file x
*     hc             d      array containing alternating cosine and sine
*                           coefficients
*     nf             i      number of sine and cosine terms
*     om             d      sine and cosine frequencies (Note 2)  
*
*  Returned:
*     scr            d      scratch array of length 3 times nf which is
*                           returned as the recursion cr
*  Notes:
*
*  1) See the MAIN program HARDISP.F header comments for detailed information.
* 
*  2) The frequencies are normalized so that the Nyquist frequency is pi.
*
*  Called:
*     None
*
*  Test case:
*     Not provided for this subroutine.
*
*  References:
*
*     Petit, G. and Luzum, B. (eds.), IERS Conventions (2010),
*     IERS Technical Note No. 36, BKG (2010)
*
*  Revisions:
*  2009 June 05 B.E. Stetzler    Added header and copyright, used DCOS
*                                and DSIN exclusively, and replaced END 
*                                DO statements with CONTINUE statements
*  2009 August 19 B.E. Stetzler  Capitalized all variables for FORTRAN
*                                77 compatibility
*-----------------------------------------------------------------------

      IMPLICIT NONE
      INTEGER I,J,N,NF
      REAL X,HC,OM
      DOUBLE PRECISION SC,SCR
      DIMENSION X(*),HC(*),SCR(*),OM(*)

*  Set up for start of recursion by computing harmonic values
*  at starting point and just before it

      DO I = 1,NF
         SCR(3*I-2) = HC(2*I-1)
         SCR(3*I-1) = HC(2*I-1)*COS(OM(I)) -HC(2*I)*SIN(OM(I))
         SCR(3*I) = 2.*DCOS(DBLE(OM(I)))
      ENDDO

*  Do recursion over data
      DO I = 1,N
         X(I) = 0.
*  Then do recursive computation for each harmonic
         DO J  = 1,NF
            X(I) = X(I) + SCR(3*J-2)
            SC = SCR(3*J-2)
            SCR(3*J-2) = SCR(3*J)*SC-SCR(3*J-1)
            SCR(3*J-1) = SC
         ENDDO
      ENDDO
      RETURN
      END

*+----------------------------------------------------------------------
      SUBROUTINE SPLINE (NN,X,U,S,A)
*+
*  - - - - - - - - -
*   S P L I N E
*  - - - - - - - - -
*
*  This routine is part of the International Earth Rotation and
*  Reference Systems Service (IERS) Conventions software collection.
*
*  The purpose of the subroutine is to find an array s for the spline
*  interpolator function EVAL.
*
*  In general, Class 1, 2, and 3 models represent physical effects that
*  act on geodetic parameters while canonical models provide lower-level
*  representations or basic computations that are used by Class 1, 2, or
*  3 models.
* 
*  Status: Canonical model	
* 
*     Class 1 models are those recommended to be used a priori in the
*     reduction of raw space geodetic data in order to determine
*     geodetic parameter estimates.
*     Class 2 models are those that eliminate an observational
*     singularity and are purely conventional in nature.
*     Class 3 models are those that are not required as either Class
*     1 or 2.
*     Canonical models are accepted as is and cannot be classified as a
*     Class 1, 2, or 3 model.
*
*  Given: This is a support routine of the main program HARDISP.F.
*     nn             i      number of data points supplied, which may be
*                           negative (Note 1)
*     x              d      array containing x-coordinates where function
*                           is sampled (Note 2)
*     u              d      array containing sample values that are to be
*                           interpolated 
*     a              d      working space array of dimension at least nn
*
*  Returned:
*     s              d      output array of 2nd derivative at sample points 
*
*  Notes:
*
*  1) If the user wishes to force the derivatives at the ends of the series
*     to assume specified values, he or she should put du(1)/dx and du(n)/dx
*     in the variables s1 and s2 and call the subroutine with nn = -(number
*     of terms in the series).  Normally a parabola is fitted through the 
*     1st and last 3 points to find the slopes.  If less than 4 points are
*     supplied, straight lines are fitted.
* 
*  2) The sequence xx(1), xx(2), ... xx(nn) must be strictly increasing.
*
*  Called:
*     None
*
*  Test case:
*     Not provided for this subroutine.
*
*  References:
*
*     Petit, G. and Luzum, B. (eds.), IERS Conventions (2010),
*     IERS Technical Note No. 36, BKG (2010)
*
*  Revisions:
*  2009 June   08 B.E.Stetzler    Added header and copyright
*  2009 August 19 B.E.Stetzler    Capitalized all variables for FORTRAN
*                                 77 compatibility
*  2009 August 26 B.E.Stetzler    Used IMPLICIT NONE and defined all variables
*-----------------------------------------------------------------------

      IMPLICIT NONE
      INTEGER I,J,N,N1,NN,NMAX
      PARAMETER (NMAX = 20)
      REAL A,C,Q,Q1,QN,S,U,X,U1,U2,X1,X2
      DIMENSION X(NMAX),U(NMAX),S(NMAX),A(NMAX)

      Q(U1,X1,U2,X2)=(U1/X1**2-U2/X2**2)/(1.0/X1-1.0/X2)
      

      N = IABS(NN)

      IF (N.LE.3) THEN

*  series too short for cubic spline - use straight lines.
         DO I=1,N
            S(I)=0.0
         ENDDO
         RETURN
      ENDIF

      Q1=Q(U(2)-U(1),X(2)-X(1),U(3)-U(1),X(3)-X(1))
      QN=Q(U(N-1)-U(N),X(N-1)-X(N),U(N-2)-U(N),X(N-2)-X(N))

      IF (NN.LE.0) THEN
         Q1=S(1)
         QN=S(2)
      ENDIF

      S(1)=6.0*((U(2)-U(1))/(X(2)-X(1)) - Q1)
      N1= N - 1

      DO I=2,N1
         S(I)= (U(I-1)/(X(I)-X(I-1)) - U(I)*(1.0/(X(I)-X(I-1))
     .   + 1.0/(X(I+1)-X(I))) + U(I+1)/(X(I+1)-X(I)))*6.0
      ENDDO

      S(N)=6.0*(QN + (U(N1)-U(N))/(X(N)-X(N1)))
      A(1)=2.0*(X(2)-X(1))
      A(2)=1.5*(X(2)-X(1)) + 2.0*(X(3)-X(2))
      S(2)=S(2) - 0.5*S(1)

      DO I=3,N1
         C=(X(I)-X(I-1))/A(I-1)
         A(I)=2.0*(X(I+1)-X(I-1)) - C*(X(I)-X(I-1))
         S(I)=S(I) - C*S(I-1)
      ENDDO

      C=(X(N)-X(N1))/A(N1)
      A(N)=(2.0-C)*(X(N)-X(N1))
      S(N)=S(N) - C*S(N1)

*  Back substitute
      S(N)= S(N)/A(N)

      DO J=1,N1
         I=N-J
         S(I) =(S(I) - (X(I+1)-X(I))*S(I+1))/A(I)
      ENDDO
      RETURN
      END

*+----------------------------------------------------------------------
      REAL FUNCTION EVAL (Y,NN,X,U,S)
*+
*  - - - - - - - - - - -
*   E V A L 
*  - - - - - - - - - - -
*
*  This routine is part of the International Earth Rotation and
*  Reference Systems Service (IERS) Conventions software collection.
*
*  This function performs cubic spline interpolation of a given function
*  sampled at unequally spaced intervals.  The subroutine SPLINE needs
*  to be called beforehand to set up the array s.
* 
*  In general, Class 1, 2, and 3 models represent physical effects that
*  act on geodetic parameters while canonical models provide lower-level
*  representations or basic computations that are used by Class 1, 2, or
*  3 models.
* 
*  Status: Canonical model
*
*     Class 1 models are those recommended to be used a priori in the
*     reduction of raw space geodetic data in order to determine
*     geodetic parameter estimates.
*     Class 2 models are those that eliminate an observational
*     singularity and are purely conventional in nature.
*     Class 3 models are those that are not required as either Class
*     1 or 2.
*     Canonical models are accepted as is and cannot be classified as a
*     Class 1, 2, or 3 model.
*
*  Given:
*     y            d     the coordinate at which a function value is
*                        desired (Note 1)
*     nn           i     number of samples of the original function
*     x            d     array containing sample coordinates x(1),x(2),...
*                        x(nn) (Note 2)
*     s            d     array containing the 2nd derivatives at the sample
*                        points (Note 3)
*
*  Returned:
*     u            d     array containing samples of a function at the
*                        coordinates x(1),x(2),...x(nn)
*
*  Notes:
*
*  1) If y falls outside the range (x(1),x(nn)), the value at the nearest
*     endpoint of the series is used.
*
*  2) The sequence x(1),x(2),...x(nn) must be strictly increasing.
*
*  3) This array is found by the subroutine SPLINE, which must be called
*     once before beginning this interpolation.
*
*  Called:
*     None
*
*  Test case:
*     
*  Not provided for this function.  This is a support routine of the main
*  program HARDISP.F.
*
*  References:
*
*     Petit, G. and Luzum, B. (eds.), IERS Conventions (2010),
*     IERS Technical Note No. 36, BKG (2010)
*
*  Revisions:
*  2009 June   03 B.E.Stetzler Initial standardization of function
*  2009 August 19 B.E.Stetzler Capitalized all variables for FORTRAN 77
*                               compatibility
*-----------------------------------------------------------------------
      
      IMPLICIT NONE
      INTEGER K,K1,K2,NN
      REAL DELI,DK,DY,DY1,F1,F2,F3,FF1,FF2,S,U,X,Y
      DIMENSION X(*),U(*),S(*)

      NN = IABS(NN)

*     If y is out of range, substitute endpoint values

      IF (Y.LE.X(1)) THEN
         EVAL=U(1)
         RETURN
      ENDIF

      IF (Y.GE.X(NN)) THEN
         EVAL=U(NN)
         RETURN
      ENDIF

*    Locate interval (x(k1),x(k2)) which contains y
      DO 100 K=2,NN
         IF(X(K-1).LT.Y.AND.X(K).GE.Y) THEN
           K1=K-1
           K2=K
         ENDIF
100   CONTINUE

*    Evaluate and then interpolate
      DY=X(K2)-Y
      DY1=Y-X(K1)
      DK=X(K2)-X(K1)
      DELI=1.0D0/(6.0D0*DK)
      FF1=S(K1)*DY*DY*DY
      FF2=S(K2)*DY1*DY1*DY1
      F1=(FF1+FF2)*DELI
      F2=DY1*((U(K2)/DK)-(S(K2)*DK)/6.0D0)
      F3= DY*((U(K1)/DK)-(S(K1)*DK)/6.0D0)
      EVAL=F1+F2+F3
      RETURN
      END

*+----------------------------------------------------------------------
      INTEGER FUNCTION JULDAT (IT)
*+
*  - - - - - - - - - - -
*   J U L D A T
*  - - - - - - - - - - -
*
*  This function is part of the International Earth Rotation and
*  Reference Systems Service (IERS) Conventions software collection.
*
*  This function converts a Gregorian date to a Julian date.
* 
*  In general, Class 1, 2, and 3 models represent physical effects that
*  act on geodetic parameters while canonical models provide lower-level
*  representations or basic computations that are used by Class 1, 2, or
*  3 models.
* 
*  Status: Canonical model
*
*     Class 1 models are those recommended to be used a priori in the
*     reduction of raw space geodetic data in order to determine
*     geodetic parameter estimates.
*     Class 2 models are those that eliminate an observational
*     singularity and are purely conventional in nature.
*     Class 3 models are those that are not required as either Class
*     1 or 2.
*     Canonical models are accepted as is and cannot be classified as a
*     Class 1, 2, or 3 model.
*
*  Given:
*     it           i      a Gregorian date (Note 1)
*
*  Returned:
*     juldat       i      a Julian date (Note 2) 
*
*  Notes:
*
*  1)  The format of the Gregorian date should be yyyy-mm-dd. 
*  2)  The date is valid for all positive values.
*
*  Called:
*     None
*
*  Test case:  This is a support routine of the main program HARDISP.F.
*     given input: it(1) = 2008
*                  it(2) = 12
*                  it(3) = 12
*     expected output: juldat = 2454813
*
*  References:
*
*     Explanatory Supplement American Ephemeris & Nautical Almanac
*     (cf Comm CACM, 11, 657 (1968) and 15, 918 (1972)), p. 604
*
*     Petit, G. and Luzum, B. (eds.), IERS Conventions (2010),
*     IERS Technical Note No. 36, BKG (2010)
*
*  Revisions:
*  2009 April  22 B.E.Stetzler  Initial standardization of function
*                               and provided a test case 
*  2009 August 19 B.E.Stetzler  Capitalized all variables for FORTRAN
*                               77 compatibility
*-----------------------------------------------------------------------
      
      IMPLICIT NONE

      INTEGER IT
      DIMENSION IT(*)

      JULDAT = (1461*(IT(1)+4800+(IT(2)-14)/12))/4
     .       + (367*(IT(2)-2-12*((IT(2)-14)/12)))/12
     .       - (3*((IT(1)+4900+(IT(2)-14)/12)/100))/4+IT(3)-32075

      RETURN
      END
  
*+----------------------------------------------------------------------
      INTEGER FUNCTION MDAY (IY, M)
*+
*  - - - - - - - - - - -
*   M D A Y 
*  - - - - - - - - - - -
*
*  This routine is part of the International Earth Rotation and
*  Reference Systems Service (IERS) Conventions software collection.
*
*  This function finds the day number of days before start of month m,
*  of year iy, in Gregorian intercalation.  
* 
*  In general, Class 1, 2, and 3 models represent physical effects that
*  act on geodetic parameters while canonical models provide lower-level
*  representations or basic computations that are used by Class 1, 2, or
*  3 models.
* 
*  Status: Canonical model
*
*     Class 1 models are those recommended to be used a priori in the
*     reduction of raw space geodetic data in order to determine
*     geodetic parameter estimates.
*     Class 2 models are those that eliminate an observational
*     singularity and are purely conventional in nature.
*     Class 3 models are those that are not required as either Class
*     1 or 2.
*     Canonical models are accepted as is and cannot be classified as a
*     Class 1, 2, or 3 model.
*
*  Given:
*     iy           i      a year
*     m            i      a month
*
*  Returned:
*     mday         i      day number of day before start of a month
*
*  Notes:
*
*  1)  This function needs to test for a leap year. 
*
*  Called:
*     None
*
*  Test case:  This is a support function of the main program HARDISP.F.
*     given input: iy = 2009
*                   m = 5 
*     expected output: mday = 120 
*
*  References:
*
*     Petit, G. and Luzum, B. (eds.), IERS Conventions (2010),
*     IERS Technical Note No. 36, BKG (2010)
*
*  Revisions:
*  2009 April 22 B.E. Stetzler Initial standardization of function
*                              and provided a test case 
*  2009 July  29 B.E. Stetzler Capitalized all variables for FORTRAN 77
*                              compatibility 
*-----------------------------------------------------------------------
      
      IMPLICIT NONE

      INTEGER IY, LEAP, M

      LEAP = 1 - (MOD(IY,4)+3)/4
      IF(MOD(IY,100).EQ.0.AND.MOD(IY,400).NE.0) LEAP=0
      MDAY = MOD((367*(M-2-12*((M-14)/12)))/12+29,365) + LEAP*((9+M)/12)

      RETURN
      END
  
*+----------------------------------------------------------------------
      INTEGER FUNCTION LEAP (IY)
*+
*  - - - - - - - - - - -
*   L E A P 
*  - - - - - - - - - - -
*
*  This routine is part of the International Earth Rotation and
*  Reference Systems Service (IERS) Conventions software collection.
*
*  This function determines whether a given integer year is a leap
*  year.
* 
*  In general, Class 1, 2, and 3 models represent physical effects that
*  act on geodetic parameters while canonical models provide lower-level
*  representations or basic computations that are used by Class 1, 2, or
*  3 models.
* 
*  Status: Canonical model
*
*     Class 1 models are those recommended to be used a priori in the
*     reduction of raw space geodetic data in order to determine
*     geodetic parameter estimates.
*     Class 2 models are those that eliminate an observational
*     singularity and are purely conventional in nature.
*     Class 3 models are those that are not required as either Class
*     1 or 2.
*     Canonical models are accepted as is and cannot be classified as a
*     Class 1, 2, or 3 model.
*
*  Given:
*     iy           i      a year (Note 1)
*
*  Returned:
*     0            i      if year is not a leap year
*     1            i      if year is a leap year
*
*  Notes:
*
*  1) The year is a Gregorian year. 
*
*  Called:
*     None
*
*  Test case:  This is a support function of the main program HARDISP.F.
*     given input: IY = 2009
*     
*     expected output: 0
*
*  References:
*
*     Petit, G. and Luzum, B. (eds.), IERS Conventions (2010),
*     IERS Technical Note No. 36, BKG (2010)
*
*  Revisions:
*  2009 April  20 B.E.Stetzler  Initial standardization of function
*                               and provided a test case 
*  2009 August 19 B.E.Stetzler  Capitalized all variables for FORTRAN
*                               77 compatibility
*-----------------------------------------------------------------------

      IMPLICIT NONE
      INTEGER IY

      LEAP = 1 - (MOD(IY,4)+3)/4

      IF(MOD(IY,100).EQ.0.AND.MOD(IY,400).NE.0) LEAP=0

      RETURN
      END
  
*+----------------------------------------------------------------------
      SUBROUTINE ETUTC (YEAR, DELTA)
*+
*  - - - - - - - - -
*   E T U T C
*  - - - - - - - - -
*
*  This routine is part of the International Earth Rotation and
*  Reference Systems Service (IERS) Conventions software collection.
*
*  The purpose of the subroutine is to compute the difference, delta,
*  between Epheremis Time (ET) and Coordinated Universal Time (UTC).
*
*  In general, Class 1, 2, and 3 models represent physical effects that
*  act on geodetic parameters while canonical models provide lower-level
*  representations or basic computations that are used by Class 1, 2, or
*  3 models.
* 
*  Status: Canonical model	
* 
*     Class 1 models are those recommended to be used a priori in the
*     reduction of raw space geodetic data in order to determine
*     geodetic parameter estimates.
*     Class 2 models are those that eliminate an observational
*     singularity and are purely conventional in nature.
*     Class 3 models are those that are not required as either Class
*     1 or 2.
*     Canonical models are accepted as is and cannot be classified as a
*     Class 1, 2, or 3 model.
*
*  Given:
*     year           d      decimal year (Note 1)
*
*  Returned:
*     delta          d      ET - UTC (Note 2)
*
*     :------------------------------------------:
*     :                                          :
*     :                 IMPORTANT                :
*     :                                          :
*     :  A new version of this routine must be   :
*     :  produced whenever a new leap second is  :
*     :  announced.  There are three items to    :
*     :  change on each such occasion:           :
*     :                                          :
*     :  1) Update the nstep variable            :
*     :  2) Update the arrays st and si          :                              
*     :  3) Change date of latest leap second    :
*     :                                          :
*     :  Latest leap second:  2008 December 31   :
*     :                                          :
*     :__________________________________________:
*
*  Notes:
*
*  1) This subroutine is valid only from 1700.-until next leap second.
*     Currently, this is up to 2009.5.
* 
*  2) The expression used in given in seconds.
* 
*  3) Leap second table in GAMIT UTC (and UT) is the time most 
*     often used (e.g. in time signals)
*
*  Test case:
*     given input: year = 2007.0 
*
*     expected output: delta = 65.1840000000000 seconds
*
*  References:
*
*     Broucke, R. A., Explanatory Supplement American Ephemeris &
*     Nautical Almanac (cf Comm CACM, 11, 657 (1968) and 15, 918 (1972)),
*     p. 90 (Tables)
*
*     Petit, G. and Luzum, B. (eds.), IERS Conventions (2010),
*     IERS Technical Note No. 36, BKG (2010)
*
*  Revisions:
*  2009 April 22 B.E. Stetzler    Added header and copyright and
*                                 provided test case
*  2009 August 19 B.E. Stetzler   Capitalized all variables for FORTRAN
*                                 77 compatibility
*-----------------------------------------------------------------------

      IMPLICIT NONE

      INTEGER I,N,NSTEP
      PARAMETER (NSTEP=24)
      REAL D,DELTA,FRAC,ST,SI,TX,TY,YEAR
      DIMENSION D(142),TX(39),TY(39),ST(NSTEP),SI(NSTEP)

* si gives amount of step, at the times given in st

      DATA SI/24*1./
      DATA ST/1972.5,1973.0,1974.0,1975.0,1976.0,1977.0,1978.0,
     .        1979.0,1980.0,1981.5,1982.5,1983.5,1985.5,1988.0,
     .        1990.0,1991.0,1992.5,1993.5,1994.5,1996.0,1997.5,
     .        1999.0,2006.0,2009.0/

      DATA D/ 5.15, 4.64, 5.36, 3.49, 3.27, 2.45, 4.03, 1.76, 3.30,
     .  1.00, 2.42, 0.94, 2.31, 2.27,-0.22, 0.03,-0.05,-0.06,-0.57,
     .  0.03,-0.47, 0.98,-0.86, 2.45, 0.22, 0.37, 2.79, 1.20, 3.52,
     .  1.17, 2.67, 3.06, 2.66, 2.97, 3.28, 3.31, 3.33, 3.23, 3.60,
     .  3.52, 4.27, 2.68, 2.75, 2.67, 1.94, 1.39, 1.66, 0.88, 0.33,
     . -0.17,-1.88,-3.43,-4.05,-5.77,-7.06,-7.36,-7.67,-7.64,-7.93,
     . -7.82,-8.35,-7.91,-8.03,-9.14,-8.18,-7.88,-7.62,-7.17,-8.14,
     . -7.59,-7.17,-7.94,-8.23,-7.88,-7.68,-6.94,-6.89,-7.11,-5.87,
     . -5.04,-3.90,-2.87,-0.58, 0.71, 1.80, 3.08, 4.63, 5.86, 7.21,
     .  8.58,10.50,12.10,12.49,14.41,15.59,15.81,17.52,19.01,18.39,
     . 19.55,20.36,21.01,21.81,21.76,22.35,22.68,22.94,22.93,22.69,
     . 22.94,23.20,23.31,23.63,23.47,23.68,23.62,23.53,23.59,23.99,
     . 23.80,24.20,24.99,24.97,25.72,26.21,26.37,26.89,27.68,28.13,
     . 28.94,29.42,29.66,30.29,30.96,31.09,31.59,32.06,31.82,32.69,
     . 33.05,33.16,33.59/
      DATA TX/61.5,
     .62.     ,62.5     ,63.      ,63.5     ,64.      ,64.5     ,65.   ,
     .65.5    ,66.      ,66.5     ,67.      ,67.5     ,68.      ,68.25 ,
     .68.5    ,68.75    ,69.      ,69.25    ,69.5     ,69.75    ,70.   ,
     .70.25   ,70.5     ,70.75    ,71.      ,71.085   ,71.162   ,71.247,
     .71.329  ,71.414   ,71.496   ,71.581   ,71.666   ,71.748   ,71.833,
     .71.915  ,71.999   ,72.0/
      DATA TY/33.59,
     .34.032  ,34.235   ,34.441   ,34.644   ,34.95    ,35.286   ,35.725,
     .36.16   ,36.498   ,36.968   ,37.444   ,37.913   ,38.39    ,38.526,
     .38.76   ,39.      ,39.238   ,39.472   ,39.707   ,39.946   ,40.185,
     .40.42   ,40.654   ,40.892   ,41.131   ,41.211   ,41.284   ,41.364,
     .41.442  ,41.522   ,41.600   ,41.680   ,41.761   ,41.838   ,41.919,
     .41.996  ,42.184   ,42.184/

*  For the oldest epochs, use approximations

      IF(YEAR.LT.1700.0D0) THEN
        DELTA = 0.0D0
        RETURN
      ENDIF
      IF(YEAR.LT.1785.0D0) THEN
        DELTA = (YEAR-1750.0D0)/5.0D0
        RETURN
      ENDIF
      IF(YEAR.LT.1820.5D0) THEN
        DELTA = 6.0D0
        RETURN
      ENDIF

*  For 1820.5 to 1961.5, data is spaced at yearly intervals

      IF(YEAR.LT.1961.5D0) THEN
         N = YEAR - 1819.5
         FRAC = YEAR - (1819.5 + N)
         DELTA = (D(N+1) - D(N))*FRAC + D(N)
         RETURN
      ENDIF

*  For 1961.5 to 1972.0, interpolate between unequispaced data

      IF(YEAR.LT.1972.0D0) THEN
        DO 150 I = 1,38
           IF(YEAR-1900.0D0.EQ.TX(I)) THEN
              DELTA = TY(I)
              RETURN
           ENDIF
           IF(YEAR-1900.0D0.LT.TX(I)) THEN
              DELTA=TY(I-1) + (TY(I)-TY(I-1))*
     .                ((YEAR-1900.0D0-TX(I-1))/(TX(I)-TX(I-1)))
              RETURN
           ENDIF
150     CONTINUE
      ENDIF

*--------------------------------------------------------------------------*
*   after 1972 et-utc has only step offsets. st is the array of step times,*
*   and si is the step sizes (an added second is +1.)                      *
*--------------------------------------------------------------------------*
      DELTA = 42.184D0
      DO 250 I = 1,NSTEP
         IF(YEAR.GE.ST(I)) DELTA = DELTA + SI(I)
         IF(YEAR.LT.ST(I)) RETURN
250   CONTINUE
      RETURN
      END

*+----------------------------------------------------------------------
      SUBROUTINE TOYMD (IT1,IT2)
*+
*  - - - - - - - - - - -
*   T O Y M D
*  - - - - - - - - - - -
*
*  This routine is part of the International Earth Rotation and
*  Reference Systems Service (IERS) Conventions software collection.
*
*  This subroutine converts times given in it1 expressed in year and
*  day of year to year-month-day in it2.
*
*  In general, Class 1, 2, and 3 models represent physical effects that
*  act on geodetic parameters while canonical models provide lower-level
*  representations or basic computations that are used by Class 1, 2, or
*  3 models.
* 
*  Status: Canonical model
*
*     Class 1 models are those recommended to be used a priori in the
*     reduction of raw space geodetic data in order to determine
*     geodetic parameter estimates.
*     Class 2 models are those that eliminate an observational
*     singularity and are purely conventional in nature.
*     Class 3 models are those that are not required as either Class
*     1 or 2.
*     Canonical models are accepted as is and cannot be classified as a
*     Class 1, 2, or 3 model.
*
*  Given:
*     it1           i(2)      time given in year and day of year (Note 1)
*
*  Returned:
*     it2           i(3)      time given in year-month-day format
*
*  Notes:
*
*  1) The time is split into a year, given as it1(1) and the day of the
*     year, given as it1(2). 
*
*  Called:
*    LEAP 
*
*  Test case:
*    Given input:  it1(1) = 2008
*                  it1(2) = 120
*
*    Expected output: it2(1) = 2008
*                     it2(2) = 4
*                     it2(3) = 29
*
*  References:
*
*     Petit, G. and Luzum, B. (eds.), IERS Conventions (2010),
*     IERS Technical Note No. 36, BKG (2010)
*
*  Revisions:
*  2009 April   20 B.E.Stetzler Initial standardization of subroutine 
*  2009 April   23 B.E.Stetzler Provided test case
*  2009 August  19 B.E.Stetzler Capitalized all variables for FORTRAN
*                               77 compatibility
*-----------------------------------------------------------------------

      IMPLICIT NONE
      INTEGER IDN,IT1,IT2,JJ,M,MON,LEAP
      DIMENSION IT1(*),IT2(*)

      IDN(M) = MOD((367*(M-2-12*((M-14)/12)))/12+29,365)
      MON(JJ,M) = (12*(JJ-29-M))/367 + 2 + (JJ-200)/169
      IT2(1) = IT1(1)
      IT2(2) = MON(IT1(2),LEAP(IT1(1)))
      IT2(3) = IT1(2) - IDN(IT2(2)) - LEAP(IT2(1))*((9+IT2(2))/12)

      RETURN
      END

*+----------------------------------------------------------------------
      SUBROUTINE SHELLS (X,K,N)
*+
*  - - - - - - - - -
*   S H E L L S
*  - - - - - - - - -
*
*  This routine is part of the International Earth Rotation and
*  Reference Systems Service (IERS) Conventions software collection.
*
*  The subroutine sorts an array x, of length n, sorting upward,
*  and returns an array k which may be used to key another array
*  to the sorted pattern (i.e., if we had an array f to which x
*  corresponded before sorting, then after calling SHELLS,
*  f(k(1)) will be the element of f corresponding to the
*  smallest x, f(k(2)) the next smallest, and so on).
*
*  In general, Class 1, 2, and 3 models represent physical effects that
*  act on geodetic parameters while canonical models provide lower-level
*  representations or basic computations that are used by Class 1, 2, or
*  3 models.
* 
*  Status: Canonical model	
* 
*     Class 1 models are those recommended to be used a priori in the
*     reduction of raw space geodetic data in order to determine
*     geodetic parameter estimates.
*     Class 2 models are those that eliminate an observational
*     singularity and are purely conventional in nature.
*     Class 3 models are those that are not required as either Class
*     1 or 2.
*     Canonical models are accepted as is and cannot be classified as a
*     Class 1, 2, or 3 model.
*
*  Given:
*     x              d      array to be sorted (Note 1)
*     n              i      length of the input array x
*
*  Returned:
*     k              i      sorted array that may be used to key another 
*                           array
*  Notes:
*
*  1) See the subroutine ADMINT.F header comments for detailed information.
* 
*  Called:
*     None
*
*  Test case:
*     Not provided for this subroutine.
*
*  References:
*
*     Petit, G. and Luzum, B. (eds.), IERS Conventions (2010),
*     IERS Technical Note No. 36, BKG (2010)
*
*  Revisions:
*  1982 December 29              Revised so that array k is sorted in turn
*                                
*  2009 June   05 B.E. Stetzler    Added header and copyright
*  2009 August 19 B.E. Stetzler    Capitalized all variables for FORTRAN
*                                  77 compatibility
*-----------------------------------------------------------------------

      IMPLICIT NONE

      INTEGER I,IGAP,IEX,IK,IMAX,IPL,J,K,L,N
      REAL SV,X
      DIMENSION X(N),K(N)

      IGAP = N

      DO 1 I = 1,N
 1    K(I) = I
 5    IF(IGAP.LE.1) GO TO 25

      IGAP = IGAP/2
      IMAX = N - IGAP
 10   IEX = 0
      DO 20 I = 1,IMAX
      IPL = I + IGAP
      IF(X(I).LE.X(IPL)) GO TO 20
      SV = X(I)
      IK = K(I)
      X(I) = X(IPL)
      K(I) = K(IPL)
      X(IPL) = SV
      K(IPL) = IK
      IEX = IEX + 1
 20   CONTINUE

      IF(IEX.GT.0) GO TO 10
      GO TO 5

*  Now sort k's (for identical values of x, if any)

 25   J = 1
 30   IF(J.GE.N) RETURN
      IF(X(J).EQ.X(J+1)) GO TO 33
      J = J + 1
      GO TO 30
*  Have at least two x's with the same value. See how long this is true
 33   L = J
 35   IF(X(L).NE.X(L+1)) GO TO 38
      L = L + 1
      IF(L.LT.N) GO TO 35
*  j and l are the indices within which x(i) does not change - sort k
 38   IGAP = L - J + 1
 40   IF(IGAP.LE.1) J = L + 1
      IF(IGAP.LE.1) GO TO 30

      IGAP = IGAP/2
      IMAX = L-J+1 - IGAP
 45   IEX = 0

      DO 50 I=1,IMAX
      IPL = I + IGAP + J - 1
      IF(K(I+J-1).LE.K(IPL)) GO TO 50
      IK = K(I+J-1)
      K(I+J-1) = K(IPL)
      K(IPL) = IK
      IEX = IEX + 1
 50   CONTINUE
      IF(IEX.GT.0) GO TO 45
      GO TO 40
      END
*+----------------------------------------------------------------------
*+----------------------------------------------------------------------
*/
