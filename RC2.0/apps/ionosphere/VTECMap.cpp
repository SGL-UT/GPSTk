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
 * @file VTECMap.cpp
 * Program VTECMap will TD...
 *   a simple ionospheric model using least squares and slant TEC values
 *   from multiple stations.
 */

#include "Exception.hpp"
#include "Position.hpp"
#include "geometry.hpp"              // for DEG_TO_RAD and RAD_TO_DEG
#include "icd_200_constants.hpp"     // for TWO_PI
#include "MiscMath.hpp"

#include "VTECMap.hpp"
#include "RinexUtilities.hpp"

using namespace gpstk;
using namespace std;

//------------------------------------------------------------------------------------
const double VTECMap::VTECErrorMultipath = 4.0;
const double VTECMap::VTECErrorSat = 0.9;
const double VTECMap::VTECErrorCFC[4] = {-0.000045, 0.0096, -0.6755, 15.84};
const double VTECMap::ObliqCoef[4] = {-4.316e-06, 0.001043, -0.08771, 3.57};

//------------------------------------------------------------------------------------
void VTECMap::CopyInputData(VTECMap &right)
{
   Decorrelation = right.Decorrelation;
   MinElevation = right.MinElevation;
   IonoHeight = right.IonoHeight;
   gridtype = right.gridtype;
   fittype = right.fittype;
   BeginLat = right.BeginLat;
   DeltaLat = right.DeltaLat;
   NumLat = right.NumLat;
   BeginLon = right.BeginLon;
   DeltaLon = right.DeltaLon;
   NumLon = right.NumLon;
   RefStation = right.RefStation;
}

//------------------------------------------------------------------------------------
void VTECMap::SetDefaults()
{
   Decorrelation = 3.0;    // TECU/1000km
   MinElevation = 10.0;
   gridtype = UniformLatLon;
   fittype = Constant;
   BeginLat = 21.0;
   BeginLon = 230.0;
   DeltaLat = 0.25;
   DeltaLon = 1.0;
   NumLat = NumLon = 40;
   IonoHeight = 350. * 1000.0;       // 350km in meters
}

//------------------------------------------------------------------------------------
void VTECMap::reallyMakeGrid(Station& refS, int factor)
   throw(Exception)
{
   RefStation = refS;

   grid = new GridData[NumLat*NumLon];
   if(!grid) throw Exception("VTECMap::reallyMakeGrid failed to allocate grid");

try {
   int i,j,ishift,jshift,k;

   if(gridtype == UniformSpace) {
      int ii;
      Position ptx1,ptx2,pty1,pty2,pt3,DeltaX,DeltaY;

         // azimuth = 0 : x1
      ptx1 = refS.llr.getIonosphericPiercePoint(MinElevation,  0.0,IonoHeight);
      ptx1.transformTo(Position::Cartesian);

         // azimuth = 180 : x2
      ptx2 = refS.llr.getIonosphericPiercePoint(MinElevation,180.0,IonoHeight);
      ptx2.transformTo(Position::Cartesian);

         // azimuth = 90 : y1
      pty1 = refS.llr.getIonosphericPiercePoint(MinElevation, 90.0,IonoHeight);
      pty1.transformTo(Position::Cartesian);

         // azimuth = 270 : y2
      pty2 = refS.llr.getIonosphericPiercePoint(MinElevation,270.0,IonoHeight);
      pty2.transformTo(Position::Cartesian);

         // find the center of the sheet
      pt3 = (ptx1 + ptx2)*0.5;

         // get orthogonal vectors in the plane, and compute step size info
      DeltaX = (ptx1 - ptx2)*(1.0/double(NumLon-1));
      DeltaY = (pty1 - pty2)*(1.0/double(NumLat-1));

         // create the grid
      for(i=0; i<NumLon; i++) {           // i == x == lon
         ishift = i - (NumLon/2);
         for(j=0; j<NumLat; j++) {        // j == y == lat
            k = i * NumLat + j;           // k is the index in grid array
            jshift = j - (NumLat/2);
            grid[k].XYZ = pt3 + (DeltaX*ishift + DeltaY*jshift)*factor;
            grid[k].LLR = grid[k].XYZ;
            grid[k].LLR.transformTo(Position::Geocentric);
         }  // end j loop over lon
      }  // end i loop over lat
   }

   if(gridtype == UniformLatLon) {
      double LatCenter = BeginLat + NumLat * DeltaLat/2.0;
      double LonCenter = BeginLon + NumLon * DeltaLon/2.0;
      double rad;
      {
            // this is a trick to get the radius of the ionosphere
         Position IPP = refS.llr.getIonosphericPiercePoint(90,0,IonoHeight);
         rad = IPP.radius();
      }
         // create the grid
      for(i=0; i<NumLon; i++) {           // i == x == lon
         ishift = i - (NumLon/2);
         for(j=0; j<NumLat; j++) {        // j == y == lat
            jshift = j - (NumLat/2);
            k = i * NumLat + j;           // k is the index in grid array
            grid[k].LLR.setGeocentric(
               LatCenter + factor * jshift * DeltaLat, // lat (deg)
               LonCenter + factor * ishift * DeltaLon, // lon (deg)
               rad);                                   // radius (m)
            grid[k].XYZ = grid[k].LLR;
            grid[k].XYZ.transformTo(Position::Cartesian);
         }  // end j loop over lon
      }  // end i loop over lat
   }
}
catch(gpstk::Exception& e) {
      cerr << "VTECMap:reallyMakeGrid caught an exception\n" << e;
      GPSTK_RETHROW(e);
}
catch (...) {
      cerr << "VTECMap:reallyMakeGrid caught an unknown exception\n";
}
}

//------------------------------------------------------------------------------------
void VTECMap::OutputGrid(ostream& os)
{
   int i,j,k;
   os << fixed << setprecision(3);
   for(j=0; j<NumLat; j++) {
      for(i=0; i<NumLon; i++) {
         k = i * NumLat + j;
         os << grid[k].LLR.printf(" %7.3a %8.3l %11.3r");
         os << grid[k].XYZ.printf(" %13.3x %13.3y %13.3z");
         os << " " << setw(3) << i << " " << setw(3) << j << endl;
      }
   }
}

//------------------------------------------------------------------------------------
void VTECMap::ComputeMap(DayTime& epoch, vector<ObsData>& data, double bias)
{
   int i,j,k,n;
      // first compute the average value
   n = 0;
   ave = 0.0;
   for(k=0; k<data.size(); k++) {
      n++;
      ave *= double(n-1)/double(n);
      ave += data[k].VTEC/double(n);
   }
      // now compute the value at each grid point
   for(i=0; i<NumLon; i++) {
      for(j=0; j<NumLat; j++) {
         k = i * NumLat + j;
         ComputeGridValue(grid[k],data,bias);
      }
   }
}

//------------------------------------------------------------------------------------
// Compute the grid values. Called by ComputeMap.
void VTECMap::ComputeGridValue(GridData& gridpt, vector<ObsData>& data, double bias)
{
   double gridLat = gridpt.LLR.getGeocentricLatitude() * DEG_TO_RAD;
   double gridLon = gridpt.LLR.longitude();
   if(gridLon > 180.0) gridLon -= 360.0;
   gridLon *= DEG_TO_RAD;

   double destLat, destLon,dLat,dLon;
   double sg,cg,sd,dist,range,bear,d;
   vector<double> vtec,xtmp,ytmp,sigma;

      // loop over all data
   for(int k=0; k<data.size(); k++) {
      //if(data[k].elevation < MinElevation) continue;   // here?
      destLat = data[k].latitude * DEG_TO_RAD;
      destLon = data[k].longitude * DEG_TO_RAD;
      // compute distance in the plane from grid to dest(data)
      dLon = destLon - gridLon;
      sg = sin(gridLat);
      cg = cos(gridLat);
      sd = sin(destLat);
      d = sg*sd + cg*cos(destLat)*cos(dLon);
      dist = acos(d);
      // TD what is range? where does the 1.852 come from?
      // TD what are the units of range? assume km, then Decorrelation = TECU/1000km
      // decor error = range * Decorrelation
      range = 1.852 * 60 * dist * RAD_TO_DEG;   // 1.852 * distance in min of arc
      if(ABS(dist) < 0.01) dist = 0.01;
      d = (sd - sg*cos(dist)) / sin(dist)*cg;   // d = cos(bearing)
      if(ABS(d) > 1) {
         if(d > 0) d = 1.0;
         else d = -1.0;
      }
      bear = acos(d);
      if(dLon > 0) bear = TWO_PI - bear;

      vtec.push_back(data[k].VTEC);
      xtmp.push_back(range * d);       // this is probably slow
      ytmp.push_back(range * sin(bear));
      // sigma = RSS(measurement error, term ~ range = decorrelation)
      d = data[k].VTECerror * data[k].VTECerror
         + range * range * (Decorrelation/1000) * (Decorrelation/1000);
      sigma.push_back(SQRT(d));

   }  // end loop over all data

   d = ChiSqPlane(vtec,xtmp,ytmp,sigma) + bias;
   if(d < 0) {
      //std::cout << "Negative TEC " << d << std::endl;
      //if(d < -0.5) output warning: negative TEC set to 0
      d = 0.0;
   }
   
   gridpt.value = d;
}

//------------------------------------------------------------------------------------
// Chi squared calculation ... TD
// z is VTEC
double VTECMap::ChiSqPlane(vector<double>& z, vector<double>& x, vector<double>& y,
   vector<double>& sigma)
{
   int k;

   double invSigma,XinvSigma,YinvSigma,ZinvSigma;
   double s,sx,sy,sz,sxx,sxy,syy,sxz,syz;

   invSigma = 1.0/(sigma[0]*sigma[0]);
   s = invSigma;
   ZinvSigma = (z[0]-ave) * invSigma;
   sz = ZinvSigma;
   if(fittype == Linear) {
      XinvSigma = x[0] * invSigma;
      sx = XinvSigma;
      YinvSigma = y[0] * invSigma;
      sy = YinvSigma;
      sxx = x[0] * XinvSigma;
      sxy = x[0] * YinvSigma;
      syy = y[0] * YinvSigma;
      sxz = x[0] * ZinvSigma;
      syz = y[0] * ZinvSigma;
   }

   double invS2,XinvS2,YinvS2,ZinvS2;
   for(k=1; k<z.size(); k++) {
      // constant and linear
      invS2 = 1.0 / (sigma[k]*sigma[k]);
      s += invS2;
      ZinvS2 = (z[k]-ave) * invS2;
      sz += ZinvS2;
      // linear only
      if(fittype == Linear) {
         XinvS2 = x[k] * invS2;
         YinvS2 = y[k] * invS2;
         sx += XinvS2;
         sy += YinvS2;
         sxx += x[k] * XinvS2;
         sxy += x[k] * YinvS2;
         syy += y[k] * YinvS2;
         sxz += x[k] * ZinvS2;
         syz += y[k] * ZinvS2;
      }
   }

   double value;
   if(fittype == Linear) {
      double delta = sxy*(s*sxy-2*sx*sy) + sxx*sy*sy + syy*(sx*sx-s*sxx);
      value = ( sxz*(sx*syy-sxy*sy) + syz*(sxx*sy-sx*sxy)
            + sz*(sxy*sxy-sxx*syy) )/delta;
   }
   else
      value = (sz/s);
   return (ave + value);
}

//------------------------------------------------------------------------------------
void VTECMap::OutputMap(ostream& os, bool format)
{
   int i,j,k;
   os << fixed << setprecision(3);
   for(j=0; j<NumLat; j++) {
      for(i=0; i<NumLon; i++) {
         k = i * NumLat + j;
         if(format) os << grid[k].LLR[0] << " " << grid[k].LLR[1];
         os << " " << grid[k].value;
         if(format) os << endl;
      }
      os << endl;
   }
}

//------------------------------------------------------------------------------------
void MUFMap::ComputeMap(DayTime& epoch, vector<ObsData>& data, double bias)
{
   int i,k;
   double lvect1,lvect2,tmp,cosin;;
   GridData center,reflect;
   Position MUFearth;

   for(k=0; k<NumLat*NumLon; k++) {
         // Comment in the original code is:
         // "convert the lat/lon from the MUF grid
         // to XYZ positions on the surface of Earth"
         // then code uses grid[k].XYZ where MUFearth is here...
      MUFearth = grid[k].LLR;
      MUFearth[2] = MUFearth.radiusEarth();
      MUFearth.transformTo(Position::Cartesian);

      center.XYZ = (MUFearth + RefStation.xyz)*0.5;
      center.LLR = center.XYZ;
      center.LLR.transformTo(Position::Geocentric);

      reflect = center;
      reflect.LLR[2] = reflect.LLR.radiusEarth() + IonoHeight;

      ComputeGridValue(reflect, data, bias);

      reflect.XYZ = reflect.LLR;
      reflect.XYZ.transformTo(Position::Cartesian);

      lvect1 = lvect2 = 0.0;
      for(i=0; i<3; i++) {
         tmp = MUFearth[i] - reflect.XYZ[i];
         lvect1 += tmp*tmp;
         tmp = reflect.XYZ[i] - center.XYZ[i];
         lvect2 += tmp*tmp;
      }
      cosin = SQRT(lvect2/lvect1);
      grid[k].value =
         VTECtoF0F2(0,reflect.value,epoch,reflect.LLR.longitude()) / cosin;
   }
}

//------------------------------------------------------------------------------------
// First cut at foF2 assuming constant slab thickness of 280 km and 
// TEC = 1.24e10 (foF2)^2 tau / 10^16
void F0F2Map::ComputeMap(DayTime& epoch, vector<ObsData>& data, double bias)
{
   int i,j,k;
   for(i=0; i<NumLon; i++) {
      for(j=0; j<NumLat; j++) {
         k = i * NumLat + j;
         ComputeGridValue(grid[k],data, bias);
         grid[k].value = VTECtoF0F2(1,grid[k].value,epoch,grid[k].LLR.longitude());
      }
   }
}

//------------------------------------------------------------------------------------
double VTECMap::VTECtoF0F2(int method, double vtec, DayTime& epoch, double lon)
{
try {
   double fof2,tau,dt;
   static DayTime computeTime=DayTime::BEGINNING_OF_TIME;
   const double con[4]={0.019600827088077529, -1.549245071973630372,
      29.890989537102175433, 237.467144625490760745};

   if(method == 0) {
      tau = 280;
   }
   else if(method == 1) {
      if(epoch != computeTime) {
         computeTime = epoch;
         dt = epoch.hour()+epoch.minute()/60.;
         dt += (lon - 262.2743352)/15;
         if(dt > 24) dt -= 24;
         if(dt <  0) dt += 24;
         tau = con[0];
         for(int i=1; i<4; i++) tau = tau * dt + con[i];
      }
   }
   else {
      throw Exception("VTECtoF0F2 finds unknown method");
   }

   fof2 = SQRT(806.4 * vtec / tau);

   return fof2;
}
catch(Exception& e) {
   GPSTK_RETHROW(e);
}
}

//------------------------------------------------------------------------------------
double VTECMap::VTECError(double t, double el, double vtec)
{
   double slant = VTECErrorMultipath * VTECErrorMultipath / (1+2*t/3);
   slant += VTECErrorSat * VTECErrorSat;
   //slant += BiasError * BiasError;
   double q = Obliquity(el);
   double delta = VTECErrorCFC[0];
   for(int i=1; i<4; i++)
      delta = delta * el + VTECErrorCFC[i];
   delta *= 0.01;
   double vert = slant/(q*q);
   vert += delta * vtec * delta * vtec;
   vert = SQRT(vert);
   return vert;
}

//------------------------------------------------------------------------------------
double VTECMap::Obliquity(double el)
{
   if(el > 85) return 1.0;
   double obq = ObliqCoef[0];
   for(int i=1; i<4; i++)
      obq = obq * el + ObliqCoef[i];
   return obq;
}

//------------------------------------------------------------------------------------
void gpstk::PlaneCoefficients(double cof[3], double p1[3], double p2[3], double p3[3])
   throw(Exception)
{
   double denom=(p2[0]-p3[0])*p1[1]+(p3[0]-p1[0])*p2[1]+(p1[0]-p2[0])*p3[1];
   if(denom == 0) throw Exception("PlaneCoefficients finds singular problem");
   denom = 1.0/denom;
   cof[0] = ((p3[1]-p2[1])*p1[2]+(p1[1]-p3[1])*p2[2]+(p2[1]-p1[1])*p3[2])*denom;
   cof[1] = ((p2[0]-p3[0])*p1[2]+(p3[0]-p1[0])*p2[2]+(p1[0]-p2[0])*p3[2])*denom;
   cof[2] = ((p3[0]*p2[1]-p2[0]*p3[1])*p1[2]+(p1[0]*p3[1]-p3[0]*p1[1])*p2[2]+
             (p2[0]*p1[1]-p1[0]*p2[1])*p3[2])*denom;
}

//------------------------------------------------------------------------------------
ostream& gpstk::operator<<(ostream& os, const Station& s)
{
   os << "Station filename: " << s.filename << endl << fixed;
   os << s.xyz.printf("   %.3x m %.3y m %.3z m (ECEF)\n"
                      "   %.9a degN %.9l degE %.3r m (Geocentric)");
   return os;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
