//------------------------------------------------------------------------------------
// solar.cpp Solar ephemeris, angular size and shadow factor.
//------------------------------------------------------------------------------------
#include "DayTime.hpp"
#include "icd_200_constants.hpp"
#include "geometry.hpp"
#include "Position.hpp"

using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
double GMST(DayTime t)
{
      // days' since epoch = +/-(integer+0.5)
   double days = t.JD() - 2451545;
   int d=int(days);
   if(d < 0 && days==double(d)) d++;
   days = d + (days<0.0 ? -0.5 : 0.5);
   double Tp = days/36525.0;

      // Compute GMST
   double G;
   //G = 24060.0 + 50.54841 + 8640184.812866*Tp;  // seconds (24060s = 6h 41min)
   //G /= 86400.0; // instead, divide the above equation by 86400.0 manually...
   G = 0.27847222 + 0.00058505104167 + 100.0021390378009*Tp;
   G += (0.093104 - 6.2e-6*Tp)*Tp*Tp/86400.0;      // seconds/86400 = circles
   double r=1.002737909350795 + (5.9006e-11 - 5.9e-15*Tp)*Tp;
   G += r*t.secOfDay()/86400.0;                   // circles
   G *= 360.0;                                    // degrees
   //G = fmod(G,360.0);
   //if(G < -180.0) G += 360.0;
   //if(G >  180.0) G -= 360.0;

   return G;
}

//------------------------------------------------------------------------------------
// accuracy is about 1 arcminute, when t is within 2 centuries of 2000.
// Ref. Astronomical Almanac pg C24, as presented on USNO web site.
// input
//    t             epoch of interest
// output
//    lat,lon,R     latitude, longitude and distance (deg,deg,m in ECEF) of sun at t.
//    AR            apparent angular radius of sun as seen at Earth (deg) at t.
//void SolarPosition(DayTime t, double& lat, double& lon, double& R, double& AR)
Position SolarPosition(DayTime t, double& AR)
{
   //const double mPerAU = 149598.0e6;
   double D;     // days since J2000
   double g,q;   // q is mean longitude of sun, corrected for aberration
   double L;     // sun's geocentric apparent ecliptic longitude (deg)
   //double b=0; // sun's geocentric apparent ecliptic latitude (deg)
   double e;     // mean obliquity of the ecliptic (deg)
   //double R;   // sun's distance from Earth (m)
   double RA;    // sun's right ascension (deg)
   double DEC;   // sun's declination (deg)
   //double AR;  // sun's apparent angular radius as seen at Earth (deg)

   D = t.JD() - 2451545.0;
   g = (357.529 + 0.98560028 * D) * DEG_TO_RAD;
   // AA 1990 has g = (357.528 + 0.9856003 * D) * DEG_TO_RAD;
   q = 280.459 + 0.98564736 * D;
   // AA 1990 has q = 280.460 + 0.9856474 * D;
   L = (q + 1.915 * sin(g) + 0.020 * sin(2*g)) * DEG_TO_RAD;

   e = (23.439 - 0.00000036 * D) * DEG_TO_RAD;
   // AA 1990 has e = (23.439 - 0.0000004 * D) * DEG_TO_RAD;
   RA = atan2(cos(e)*sin(L),cos(L)) * RAD_TO_DEG;
   DEC = asin(sin(e)*sin(L)) * RAD_TO_DEG;

   //equation of time = apparent solar time minus mean solar time
   //= [q-RA (deg)]/(15deg/hr)

   // compute the hour angle of the vernal equinox = GMST and convert RA to lon
   double lon = fmod(RA-GMST(t),360.0);
   if(lon < -180.0) lon += 360.0;
   if(lon >  180.0) lon -= 360.0;

   double lat = DEC;

   // ECEF unit vector in direction Earth to sun
   double xhat = cos(lat*DEG_TO_RAD)*cos(lon*DEG_TO_RAD);
   double yhat = cos(lat*DEG_TO_RAD)*sin(lon*DEG_TO_RAD);
   double zhat = sin(lat*DEG_TO_RAD);

   // R in AU
   double R = 1.00014 - 0.01671 * cos(g) - 0.00014 * cos(2*g);
   // apparent angular radius in degrees
   AR = 0.2666/R;
   // convert to meters
   R *= 149598.0e6;

   Position ES;
   ES.setECEF(R*xhat,R*yhat,R*zhat);
   return ES;
}

//------------------------------------------------------------------------------------
// Compute the position (latitude and longitude, in degrees) of the sun
// given the day of year and the hour of the day.
// Adapted from sunpos by D. Coco 12/15/94
void SolarPosition0(DayTime t, double& lat, double& lon)
{
   int doy = t.DOY();
   int hod = int(t.secOfDay()/3600.0 + 0.5);
   lat = sin(23.5*DEG_TO_RAD)*sin(TWO_PI*double(doy-83)/365.25);
   lat = lat / sqrt(1.0-lat*lat);
   lat = RAD_TO_DEG*atan(lat);
   lon = 180.0 - hod*15.0;
}

//------------------------------------------------------------------------------------
// Consider the sun and the earth as seen from the satellite. Let the sun be a circle
// of angular radius r, center in direction s, and the earth be a (larger) circle
// of angular radius R, center in direction e. The circles overlap if |e-s| < R+r;
// complete overlap if |e-s| < R. Let L == |e-s|.
//    What is the area of overlap if R-r < L < R+r ?
// Call the two points where the circles intersect p1 and p2. Draw a line from e to s;
// call the points where this line intersects the two circles r1 and R1, respectively.
// Draw lines from e to s, e to p1, e to p2, s to p1 and s to p2. Call the angle
// between e-s and e-p1 alpha, and that between s-e and s-p1, beta.
// Draw a rectangle with top and bottom parallel to e-s passing through p1 and p2,
// and with sides passing through s and r1. Similarly for e and R1. Note that the
// area of intersection lies within the intersection of these two rectangles.
// Call the area of the rectangle outside the circles A and B. The height H of the
// rectangles is
// H = 2Rsin(alpha) = 2rsin(beta)
// also L = rcos(beta)+Rcos(alpha)
// The area A will be the area of the rectangle
//              minus the area of the wedge formed by the angle 2*alpha
//              minus the area of the two triangles which meet at s :
// A = RH - (2alpha/2pi)*pi*R*R - 2*(1/2)*(H/2)Rcos(alpha)
// Similarly
// B = rH - (2beta/2pi)*pi*r*r  - 2*(1/2)*(H/2)rcos(beta)
// The area of intersection will be the area of the rectangular intersection
//                            minus the area A
//                            minus the area B
// Intersection = H(R+r-L) - A - B
//              = HR+Hr-HL -HR+alpha*R*R+(H/2)Rcos(alpha) -Hr+beta*r*r+(H/2)rcos(beta)
// Cancel terms, and substitute for L using above equation L = ..
//              = -(H/2)rcos(beta)-(H/2)Rcos(alpha)+alpha*R*R+beta*r*r
// substitute for H/2
//              = -R*R*sin(alpha)cos(alpha)-r*r*sin(beta)cos(beta)+alpha*R*R+beta*r*r
// Intersection = R*R*[alpha-sin(alpha)cos(alpha)]+r*r*[beta-sin(beta)cos(beta)]
// Solve for alpha and beta in terms of R, r and L using the H and L relations above
// (r/R)cos(beta)=(L/R)-cos(alpha)
// (r/R)sin(beta)=sin(alpha)
// so
// (r/R)^2 = (L/R)^2 - (2L/R)cos(alpha) + 1
// cos(alpha) = (R/2L)(1+(L/R)^2-(r/R)^2)
// cos(beta) = (L/r) - (R/r)cos(alpha)
// and 0 <= alpha or beta <= pi
//
// Rearth    angular radius of the earth as seen at the satellite
// Rsun      angular radius of the sun as seen at the satellite
// dES       angular distance of the sun from the earth
// return    fraction (0 <= f <= 1) of area of sun covered by earth
// units only need be consistent
double shadowFactor(double Rearth, double Rsun, double dES)
{
   if(dES >= Rearth+Rsun) return 0.0;
   if(dES <= fabs(Rearth-Rsun)) return 1.0;
   double r=Rsun, R=Rearth, L=dES;
   if(Rsun > Rearth) { r=Rearth; R=Rsun; }
   double cosalpha = (R/L)*(1.0+(L/R)*(L/R)-(r/R)*(r/R))/2.0;
   double cosbeta = (L/r) - (R/r)*cosalpha;
   double sinalpha = ::sqrt(1-cosalpha*cosalpha);
   double sinbeta = ::sqrt(1-cosbeta*cosbeta);
   double alpha = ::asin(sinalpha);
   double beta = ::asin(sinbeta);
   double shadow = r*r*(beta-sinbeta*cosbeta)+R*R*(alpha-sinalpha*cosalpha);
   shadow /= ::acos(-1.0)*Rsun*Rsun;
   return shadow;
}

//------------------------------------------------------------------------------------
// From AA 1990 D46
//void LunarPosition(DayTime t, double& lat, double& lon, double& R, double& AR)
Position LunarPosition(DayTime t, double& AR)
{
   // days since J2000
   double N = t.JD()-2451545.0;
   // centuries since J2000
   double T = N/36525.0;
   // ecliptic longitude
   double lam = DEG_TO_RAD*(218.32 + 481267.883*T
              + 6.29 * ::sin(DEG_TO_RAD*(134.9+477198.85*T))
              - 1.27 * ::sin(DEG_TO_RAD*(259.2-413335.38*T))
              + 0.66 * ::sin(DEG_TO_RAD*(235.7+890534.23*T))
              + 0.21 * ::sin(DEG_TO_RAD*(269.9+954397.70*T))
              - 0.19 * ::sin(DEG_TO_RAD*(357.5+ 35999.05*T))
              - 0.11 * ::sin(DEG_TO_RAD*(259.2+966404.05*T)));
   // ecliptic latitude
   double bet = DEG_TO_RAD*(5.13 * ::sin(DEG_TO_RAD*( 93.3+483202.03*T))
                          + 0.28 * ::sin(DEG_TO_RAD*(228.2+960400.87*T))
                          - 0.28 * ::sin(DEG_TO_RAD*(318.3+  6003.18*T))
                          - 0.17 * ::sin(DEG_TO_RAD*(217.6-407332.20*T)));
   // horizontal parallax
   double par = DEG_TO_RAD*(0.9508
              + 0.0518 * ::cos(DEG_TO_RAD*(134.9+477198.85*T))
              + 0.0095 * ::cos(DEG_TO_RAD*(259.2-413335.38*T))
              + 0.0078 * ::cos(DEG_TO_RAD*(235.7+890534.23*T))
              + 0.0028 * ::cos(DEG_TO_RAD*(269.9+954397.70*T)));

   // obliquity of the ecliptic
   double eps = (23.439 - 0.00000036 * N) * DEG_TO_RAD;

   // convert ecliptic lon,lat to geocentric lon,lat
   double l = ::cos(bet)*::cos(lam);
   double m = ::cos(eps)*::cos(bet)*::sin(lam) - ::sin(eps)*::sin(bet);
   double n = ::sin(eps)*::cos(bet)*::sin(lam) + ::cos(eps)*::sin(bet);

   // convert to right ascension and declination,
   // (referred to mean equator and equinox of date)
   double RA = atan2(m,l) * RAD_TO_DEG;
   double DEC = asin(n) * RAD_TO_DEG;

   // compute the hour angle of the vernal equinox = GMST and convert RA to lon
   double lon = fmod(RA-GMST(t),360.0);
   if(lon < -180.0) lon += 360.0;
   if(lon >  180.0) lon -= 360.0;

   double lat = DEC;

   // apparent semidiameter of moon (in radians)
   AR = 0.2725 * par;
   // moon distance in meters
   double R = 1.0 / ::sin(par);
   R *= 6378137.0;

   // ECEF vector in direction Earth to moon
   double x = R*cos(lat*DEG_TO_RAD)*cos(lon*DEG_TO_RAD);
   double y = R*cos(lat*DEG_TO_RAD)*sin(lon*DEG_TO_RAD);
   double z = R*sin(lat*DEG_TO_RAD);

   Position EM;
   EM.setECEF(x,y,z);
   return EM;
}

//------------------------------------------------------------------------------------
// find a zero of the function f(T&) between the two T values t1 and t2.
// f(t1) and f(t2) must have different signs.
// quit when the precision is prec or better: d(return value) <= prec*abs(t2-t1)
template<class T> T findZero(T t1, T t2,
                             double (*f)(T& t),
                             double prec=0.0001) 
{
   T tzero;
   tzero = T();
   if(f(t1)*f(t2) >= 0.0) return tzero;

   T tlo(t1),thi(t2);
   if(f(tlo) > f(thi)) { tlo = t2; thi = t1; }
   double ftlo=f(tlo),fthi=f(thi),ft,ftprev,dft;
   ft = ftlo;
   dft = fthi-ftlo;
   do {
      ftprev = ft;
      tzero = tlo + (thi-tlo)/2.0;
      ft = f(tzero);
      if(ft == 0) return tzero;
      if(ft*ftlo < 0) { thi = tzero; fthi = ft; }
      else            { tlo = tzero; ftlo = ft; }
   } while(fabs(ft-ftprev) > prec*dft);
   return tzero;
}

//------------------------------------------------------------------------------------
Position Here;
double SolarElevation(DayTime& t);
double LunarElevation(DayTime& t);
int main(int argc, char **argv)
{
   try {
      bool verbose=false,debug=false;
      int i,hrs=5;          // GMT-Zone of interest (5 CDT, 6 CST)
      double size,sunel,moonel,dt;
      string timeformat("%Y,%m,%d,%H,%M,%S"),positionformat("%x,%y,%z");
      DayTime Now,tnow,tbeg,tend,sunrise,sunset,moonrise,moonset;
      Position Sun,Moon;

      // ARL:UT
      Here.setToString("-740289.807,-5457071.744,3207245.649",positionformat);
      Now.setLocalTime();

      for(i=1; i<argc; i++) {
         if(argv[i][0] == '-') { // && argv[i][1] == '-')
            string arg(argv[i]);
            if(arg == string("--help") || arg == string("-h")) {
               cout << "Compute sun and moon rise and set times "
                  << "for given day and place.\n";
               cout << "Usage: sunrise [options]; options are:\n";
               cout << "  --time <time>          Time to consider (see timeformat)\n";
               cout << "  --position <position>  Place to consider (see posformat)\n";
               cout << "  --timeformat <form>    Format for time input ("
                  << timeformat << ")\n";
               cout << "  --posformat <form>     Format for position input ("
                  << positionformat << ")\n";
               cout << "  --verbose              Output hourly information as well\n";
               cout << "  --help                 Print this message and quit\n";
               cout << " Default = current time at ARL:UT; "
                  << "NB input format before data.\n";
               return -1;
            }
            else if(arg == string("--time") || arg == string("-t")) {
               arg = string(argv[++i]);
               Now.setToString(arg,timeformat);
            }
            else if(arg == string("--position") || arg == string("-p")) {
               arg = string(argv[++i]);
               Here.setToString(arg,positionformat);
            }
            else if(arg == string("--timeformat") || arg == string("-tf"))
               timeformat = string(argv[++i]);
            else if(arg == string("--posformat") || arg == string("-pf"))
               positionformat = string(argv[++i]);
            else if(arg == string("--debug") || arg == string("-d")) debug = true;
            else if(arg == string("--verbose") || arg == string("-v")) verbose = true;
            else {
               cout << "Ignore unknown option: " << arg << endl;
            }
         }
         //else { }
      }
      if(debug) cout << "Time " << Now << " and Place " << Here << endl;

      tbeg = tend = Now + hrs*3600.0;        // UTC
      tend += 3600.0;
      i = 0;
      while(i < 15) {
         if(verbose) cout << tbeg-hrs*3600.0 << fixed << setprecision(3);
         Sun = SolarPosition(tbeg,size);
         if(verbose) cout << " " << setw(8) << Sun.getGeodeticLatitude()
            << " " << setw(8) << Sun.getLongitude()
            << " " << setw(7) << setprecision(2) << Here.elevation(Sun)
            << " " << setw(7) << Here.azimuth(Sun);
         if(Here.elevation(Sun) < 0.0 &&
               Here.elevation(SolarPosition(tend,size)) > 0.0) {
            sunrise = findZero(tbeg,tend,SolarElevation);
            i += 1;
         }
         if(Here.elevation(Sun) > 0.0 &&
               Here.elevation(SolarPosition(tend,size)) < 0.0) {
            sunset = findZero(tbeg,tend,SolarElevation);
            i += 2;
         }
         Moon = LunarPosition(tbeg,size);
         if(verbose) cout << " " << setw(8) << Moon.getGeodeticLatitude()
            << " " << setw(8) << Moon.getLongitude()
            << " " << setw(7) << setprecision(2) << Here.elevation(Moon)
            << " " << setw(7) << Here.azimuth(Moon) << endl;
         if(Here.elevation(Moon) < 0.0 &&
               Here.elevation(LunarPosition(tend,size)) > 0.0) {
            moonrise = findZero(tbeg,tend,LunarElevation);
            i += 4;
         }
         if(Here.elevation(Moon) > 0.0 &&
               Here.elevation(LunarPosition(tend,size)) < 0.0) {
            moonset = findZero(tbeg,tend,LunarElevation);
            i += 8;
         }

         tbeg = tend;
         tend.addSeconds(3600L);
      }

      Sun = SolarPosition(sunrise,size);
      Moon = SolarPosition(sunset,size);
      cout << "Sunrise  " << sunrise - hrs*3600.0
         << fixed << setprecision(2)
         //<< " @ " << setw(6) << Here.azimuth(Sun) << " deg azimuth"
         << " @ " << setw(6) << fabs(90-Here.azimuth(Sun)) << " deg "
         << (90.0-Here.azimuth(Moon) > 0 ? "S" : "N") << " of E"
         << "\nSunset   " << sunset - hrs*3600.0
         //<< " @ " << setw(6) << Here.azimuth(Moon) << " deg azimuth"
         << " @ " << setw(6) << fabs(270-Here.azimuth(Moon)) << " deg "
         << (270.0-Here.azimuth(Moon) > 0 ? "N" : "S") << " of W"
         << endl;

      Sun = LunarPosition(moonrise,size);
      Moon = LunarPosition(moonset,size);
      cout << "Moonrise " << moonrise - hrs*3600.0
         //<< " @ " << setw(6) << Here.azimuth(Sun) << " deg azimuth"
         << " @ " << setw(6) << fabs(90-Here.azimuth(Sun)) << " deg "
         << (90.0-Here.azimuth(Moon) > 0 ? "S" : "N") << " of E"
         << "\nMoonset  " << moonset - hrs*3600.0
         //<< " @ " << setw(6) << Here.azimuth(Moon) << " deg azimuth"
         << " @ " << setw(6) << fabs(270-Here.azimuth(Moon)) << " deg "
         << (270.0-Here.azimuth(Moon) > 0 ? "N" : "S") << " of W"
         << endl;

      Sun = SolarPosition(Now+hrs*3600.0,size);
      Moon = LunarPosition(Now+hrs*3600.0,size);
      cout << "Phase of the moon: " << ::acos(Sun.cosVector(Moon))*RAD_TO_DEG
         << " deg " << endl;
   }
   catch(Exception& e) {
      cerr << "gpstk exception: " << e << endl;
   }
   catch(...) {
      cerr << "unknown exception." << endl;
   }
}

//------------------------------------------------------------------------------------
double SolarElevation(DayTime& t)
{
   double size;
   return Here.elevation(SolarPosition(t,size));
}
double LunarElevation(DayTime& t)
{
   double size;
   return Here.elevation(LunarPosition(t,size));
}

//------------------------------------------------------------------------------------
