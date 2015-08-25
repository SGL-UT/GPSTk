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

/**
 * @file StochasticModels.cpp
 * Define stochastic model for measurement errors used in estimation.
 */

//------------------------------------------------------------------------------------
// system includes

// GPSTk
#include "Vector.hpp"
#include "Matrix.hpp"
#include "Namelist.hpp"
#include "GNSSconstants.hpp"
#include "RobustStats.hpp"
#include "format.hpp"

// DDBase
#include "DDBase.hpp"
#include "index.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// prototypes -- this module only
// called by ConfigureEstimation(), which is Configure(3)
int ConfigureStochasticModel(void) throw(Exception);

// called by Estimation() -- inside the loop
void BuildStochasticModel(int count, Namelist& DNL, Matrix<double>& MCov)
   throw(Exception);

// called by BuildStochasticModel(), inside the estimation loop
double StochasticWeight(OWid & owid) throw(Exception);

// called by BuildStochasticModel()
void DecomposeName(const string& label, string& site1, string& site2,
                    GSatID& sat1, GSatID& sat2) throw(Exception); // Estimation.cpp

// ElevationMask.cpp
bool ElevationMask(double elevation, double azimuth) throw(Exception);

//------------------------------------------------------------------------------------
// local data
// simple cos squared model
double eps;       // 'fudge' factor to avoid blowup at zenith
double sig0;      // sigma at the minimum elevation
double d0;        // weight at the minimum elevation
// other models...

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// Called by Configure(3) or ConfigureEstimation(), just before Estimation loop.
// Configure the stochastic model.
// TD MinElevation here should be a separate parameter, not necessarily the mask angle
int ConfigureStochasticModel(void) throw(Exception)
{
try {
   if(CI.Verbose) oflog << "BEGIN ConfigureStochasticModel() with model "
      << CI.StochasticModel
      << " at total time " << fixed << setprecision(3)
      << double(clock()-totaltime)/double(CLOCKS_PER_SEC) << " seconds."
      << endl;

   if(CI.StochasticModel == string("cos") || CI.StochasticModel == string("cos2")) {
      // ----------------------------------------
      // simple cosine or cosine squared model
      double coselev;   // eps + cosine(minimum elevation)
      //eps = 0.0000001;   seems to have no effect
      eps = 0.001;                                             // TD new input param?
         // d needs to have units meters and be realistic ~ sigma(phase)
         // =sig0(m) at min elev, smaller at higher elevation
      sig0 = 1.0e-3; // smaller than e-2, else little effect   // TD new input param?
      coselev = eps + cos(CI.MinElevation * DEG_TO_RAD);       // TD new input param?
      if(CI.StochasticModel == string("cos2"))
         d0 = sig0 / (coselev*coselev);   // cosine squared model
      if(CI.StochasticModel == string("cos"))
         d0 = sig0 / coselev;            // cosine model

      return 0;
   }
   else {
      Exception e("Unknown stochastic model identifier: " + CI.StochasticModel);
      GPSTK_THROW(e);
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// compute the weight for a single one-way id (one site, one satellite) at count
double StochasticWeight(OWid & owid, int count) throw(Exception)
{
try {
   int j;
   double cosine;

   j = index(Stations[owid.site].RawDataBuffers[owid.sat].count,count);
   if(j == -1) {
      ostringstream oss;
      oss << "Error -- count " << count << " not found in buffer for " << owid;
      Exception e(oss.str());
      GPSTK_THROW(e);
   }

   if(CI.StochasticModel == string("cos") || CI.StochasticModel == string("cos2")) {
      // ----------------------------------------
      // simple cosine squared model
      cosine = eps + cos(Stations[owid.site].RawDataBuffers[owid.sat].elev[j]
                        * DEG_TO_RAD);       // eps + cosine(minimum elevation)
      if(CI.StochasticModel == string("cos2"))
         return (d0 * cosine * cosine);      // cosine squared model
      if(CI.StochasticModel == string("cos"))
         return (d0 * cosine);               // cosine model
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// called by Estimation() - inside the data loop, inside the iteration loop
// Input is Namelist DNL, the double difference data Namelist (DataNL)
// Output is MCov, the measurement covariance matrix for this data (MeasCov).
// Let:
//  d = vector of one-way data (one site, one satellite)
// sd = vector of single difference data (two sites, one satellite)
// dd = vector of double difference data (two sites, two satellites)
// DD and SD are matricies with elements 0,1,-1 which transform d to sd to dd:
// sd = SD * d
// dd = DD * sd
// dd = DD * SD * d
// The covariance matrix will be MC = (DD*SD)*transpose(DD*SD)
//                                  = DD*SD*transpose(SD)*transpose(DD)
// If the one-way data has a measurement covariance, then fill the vector d with
// them; then MC = DD*SD* d * transpose(SD)*transpose(DD).
// Building DD and SD is just a matter of lists:
// loop through the dd namelist, keeping lists of:
// one-way data (site-satellite pairs) (d)
// single differences (site-site-satellite sets) (sd)
// and you have a list of double differences (DNL)
//
void BuildStochasticModel(int count, Namelist& DNL, Matrix<double>& MCov)
   throw(Exception)
{
try {
   unsigned int m=DNL.size();
   if(m==0) return;

   int jn,kn;
   size_t i, in;
   string site1,site2;
   GSatID sat1,sat2;
   vector<double> d;    // weights of one-way data
   vector<OWid> ld;     // labels of d
   vector<SDid> sd;

   for(i=0; i<DNL.size(); i++) {
      // break the label into site1,site2,sat1,sat2
      DecomposeName(DNL.getName(i), site1, site2, sat1, sat2);
      if(index(ld,OWid(site1,sat1)) == -1) ld.push_back(OWid(site1,sat1));
      if(index(ld,OWid(site1,sat2)) == -1) ld.push_back(OWid(site1,sat2));
      if(index(ld,OWid(site2,sat1)) == -1) ld.push_back(OWid(site2,sat1));
      if(index(ld,OWid(site2,sat2)) == -1) ld.push_back(OWid(site2,sat2));
      if(index(sd,SDid(site1,site2,sat1)) == -1) sd.push_back(SDid(site1,site2,sat1));
      if(index(sd,SDid(site1,site2,sat2)) == -1) sd.push_back(SDid(site1,site2,sat2));
   }

      // fill d with the weights
   d = vector<double>(ld.size());
   for(i=0; i<ld.size(); i++) d[i] = StochasticWeight(ld[i], count);

   // temp
   //format f113s(11,3,2);
   //oflog << "DDs are (" << DNL.size() << "):\n" << setw(20) << DNL << endl;
   //oflog << "SDs are: (" << sd.size() << ")" << fixed << endl;
   //for(i=0; i<sd.size(); i++) oflog << " / " << sd[i];
   //oflog << endl;
   //oflog << "OWs are: (" << ld.size() << ")" << endl;
   //for(i=0; i<ld.size(); i++) oflog << " / " << ld[i];
   //oflog << endl;
   //oflog << "OW wts are: (" << d.size() << ")" << endl;
   //for(i=0; i<d.size(); i++) oflog << " " << f113s << d[i];
   //oflog << endl;

   Matrix<double> SD(sd.size(),ld.size(),0.0);
   Matrix<double> DD(m,sd.size(),0.0);
   // TD need to account for signs here ... sd[.] may be site2,site1,sat1 ...
   for(in=0; in<DNL.size(); in++) {
      DecomposeName(DNL.getName(in), site1, site2, sat1, sat2);
      jn = index(sd,SDid(site1,site2,sat1));        // site1-site2, sat1
      DD(in,jn) = 1;
      kn = index(ld,OWid(site1,sat1));              // site1, sat1
      SD(jn,kn) = d[kn];
      kn = index(ld,OWid(site2,sat1));              // site2, sat1
      SD(jn,kn) = -d[kn];

      jn = index(sd,SDid(site1,site2,sat2));        // site1-site2, sat2
      DD(in,jn) = -1;
      kn = index(ld,OWid(site1,sat2));              // site1, sat2
      SD(jn,kn) = d[kn];
      kn = index(ld,OWid(site2,sat2));              // site2, sat2
      SD(jn,kn) = -d[kn];
   }

   //oflog << " SD is\n" << fixed << setw(3) << SD << endl;
   //oflog << " DD is\n" << fixed << setw(3) << DD << endl;

   Matrix<double> T;
   T = DD * SD;
   MCov = T * transpose(T);

   static bool once=true;
   if(once) {
      oflog << "Measurement covariance (model " << CI.StochasticModel << ") is\n"
      << scientific << setw(8) << setprecision(3) << MCov << endl;
      once = false;
   }

}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
