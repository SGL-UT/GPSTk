//swig interface for AstronomicalFunctions.hpp

%module gpstk_AstronomicalFunctions

%{
#include "../../../src/AstronomicalFunctions.hpp"

using namespace std;
using namespace gpstk;
%}

%inline %{
/*// Astronomical Unit value (AU), in meters
   const double AU_CONST=(1.49597870e11);
      /// Mean Earth-Moon barycenter (EMB) distance (AU)
   const double MeanEarthMoonBary=(3.12e-5);
      /// Ratio of mass Sun to Earth
   const double MU_SUN=(332946.0);
      /// Ratio of mass Moon to Earth
   const double MU_MOON=(0.01230002);
      /// Earth gravity acceleration on surface (m/s^2)
   const double EarthGrav=(9.80665);
      /// Degrees to radians
   const double D2R=(0.0174532925199432957692369);
      /// Arcseconds to radians
   const double DAS2R=(4.848136811095359935899141e-6);
      /// Seconds of time to radians
   const double DS2R=(7.272205216643039903848712e-5);
      /// Julian epoch of B1950
   const double B1950=(1949.9997904423);
      /// Earth equatorial radius in AU ( 6378.137 km / 149597870 km)
   const double ERADAU=(4.2635212653763e-5);*/
extern Triple CIS2CTS(const Triple posCIS,const DayTime& t);
extern double UTC2SID(const DayTime& t);

%}


%include "../../../src/AstronomicalFunctions.hpp"

/*   const double AU_CONST=(1.49597870e11);
   const double MeanEarthMoonBary=(3.12e-5); 
   const double MU_SUN=(332946.0); 
   const double MU_MOON=(0.01230002);  
   const double EarthGrav=(9.80665); 
   const double D2R=(0.0174532925199432957692369);    
   const double DAS2R=(4.848136811095359935899141e-6);    
   const double DS2R=(7.272205216643039903848712e-5);    
   const double B1950=(1949.9997904423);   
   const double ERADAU=(4.2635212653763e-5);*/
extern Triple CIS2CTS(const Triple posCIS,const DayTime& t);
extern double UTC2SID(const DayTime& t);
