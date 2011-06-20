#pragma ident "$Id:$"



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
 * @file BrcKeplerOrbit.cpp
 * Ephemeris data encapsulated in engineering terms
 */

#include "StringUtils.hpp"
#include "icd_gps_constants.hpp"
#include "GPSEllipsoid.hpp"
#include "BrcKeplerOrbit.hpp"

//Additional Added classes
#include "MathBase.hpp"
#include "TimeSystem.hpp"
#include "GPSWeekSecond.hpp"
#include "YDSTime.hpp"
#include "CivilTime.hpp"

#include <cmath>

namespace gpstk
{
   using namespace std;
   using namespace gpstk;

   BrcKeplerOrbit::BrcKeplerOrbit()
      throw()
   {
      dataLoaded = false;

      PRNID = weeknum = 0;

      SatSystemID = ' ';

      healthy = false;     
     
      Cuc = Cus = Crc = Crs = Cic = Cis = Toe = M0 = dn = dndot = 
	   ecc = A = Ahalf =Adot = OMEGA0 = i0 = w = OMEGAdot = idot = accuracy = 0.0;
   }

   BrcKeplerOrbit::BrcKeplerOrbit(const char SatSystemIDArg, const ObsID obsIDArg, const short PRNIDArg,
		  const double ToeArg, const short weeknumArg,
	     const double accuracyArg, const bool healthyArg,
		  const double CucArg, const double CusArg,
		  const double CrcArg, const double CrsArg,
		  const double CicArg, const double CisArg,
		  const double M0Arg, const double dnArg, const double dndotArg,
		  const double eccArg, const double AArg, const double AhalfArg,
        const double AdotArg, const double OMEGA0Arg, const double i0Arg,
        const double wArg, const double OMEGAdotARg, const double idotArg )
   {
      loadData(SatSystemIDArg, obsIDArg, PRNIDArg,
		  ToeArg, weeknumArg,
	     accuracyArg, healthyArg,
		  CucArg, CusArg,
		  CrcArg, CrsArg,
		  CicArg, CisArg,
		  M0Arg, dnArg, dndotArg,
		  eccArg, AArg, AhalfArg, AdotArg,
		  OMEGA0Arg, i0Arg, wArg, 
		  OMEGAdotARg, idotArg );

   }

		/// Legacy GPS Subframe 1-3  
   BrcKeplerOrbit::BrcKeplerOrbit(const ObsID obsIDArg, const short PRNID,const short fullweeknum,
		      const long subframe1[10],
		      const long subframe2[10],
		      const long subframe3[10] )
   {
     loadData(obsIDArg, PRNID,fullweeknum,
		      subframe1,
		      subframe2,
		      subframe3 );
   }

   void BrcKeplerOrbit::loadData(const char SatSystemIDArg, const ObsID obsIDArg, const short PRNIDArg,
		  const double ToeArg, const short weeknumArg,
	     const double accuracyArg, const bool healthyArg,
		  const double CucArg, const double CusArg,
		  const double CrcArg, const double CrsArg,
		  const double CicArg, const double CisArg,
		  const double M0Arg, const double dnArg, const double dndotArg,
		  const double eccArg, const double AArg, const double AhalfArg, 
        const double AdotArg, const double OMEGA0Arg, const double i0Arg,
        const double wArg, const double OMEGAdotARg, const double idotArg )
   {
	SatSystemID = SatSystemIDArg;
	obsID       = obsIDArg;
	PRNID       = PRNIDArg;
	Toe         = ToeArg;
	weeknum     = weeknumArg;
	accuracy    = accuracyArg;
	healthy     = healthyArg;
	Cuc         = CucArg;
	Cus         = CusArg;
	Crc         = CrcArg;
	Crs         = CrsArg;
	Cic         = CicArg;
	Cis         = CisArg;
	M0          = M0Arg;
	dn          = dnArg;
	dndot       = dndotArg;
	ecc         = eccArg;
	A           = AArg;
   Ahalf       = AhalfArg;
	Adot        = AdotArg;
	OMEGA0      = OMEGA0Arg;
	i0          = i0Arg;
	w           = wArg;
	OMEGAdot    = OMEGAdotARg;
	idot        = idotArg;
	dataLoaded  = true;
   }

   void BrcKeplerOrbit::loadData(const ObsID obsIDArg, const short PRNIDArg, const short fullweeknum,
		const long subframe1[10],
		const long subframe2[10],
		const long subframe3[10])
		throw(InvalidParameter)
    {
         double ficked[60];

 	    //Load overhead members
  	 SatSystemID = 'G';
	 obsID = obsIDArg;
	 PRNID = PRNIDArg;

	    //Convert Subframe 1
	if (!subframeConvert(subframe1, fullweeknum, ficked))
	{
	   InvalidParameter exc("Subframe 1 not valid.");
	   GPSTK_THROW(exc);
	}

	     weeknum       = static_cast<short>( ficked[5] );
	     short accFlag = static_cast<short>( ficked[7] );
	     short health        = static_cast<short>( ficked[8] );
	     //Convert the accuracy flag to a value...
	     accuracy = gpstk::ura2accuracy(accFlag);
	     healthy = false;
	     if (health == 0)
	     healthy = true;
	  
	    //Convert Subframe 2
	if (!subframeConvert(subframe2, fullweeknum, ficked))
	{
	   InvalidParameter exc("Subframe 2 not valid.");
	   GPSTK_THROW(exc);
	}

	     Crs    = ficked[6];
	     dn     = ficked[7];
	     M0     = ficked[8];
	     Cuc    = ficked[9];
	     ecc    = ficked[10];
	     Cus    = ficked[11];
	     Ahalf  = ficked[12];
	     A      = Ahalf*Ahalf;
	     Toe    = ficked[13];

	    //Convert Subframe 3
	if (!subframeConvert(subframe3, fullweeknum, ficked))
	{
	   InvalidParameter exc("Subframe3 not valid.");
	   GPSTK_THROW(exc);
	}

	     Cic      = ficked[5];
	     OMEGA0   = ficked[6];
	     Cis      = ficked[7];
	     i0       = ficked[8];
	     Crc      = ficked[9];
	     w        = ficked[10];
	     OMEGAdot = ficked[11];
	     idot     = ficked[13];

	     dndot      = 0.0;
	     Adot       = 0.0;
	     dataLoaded = true;
	 
	return;
    }
	     

   bool BrcKeplerOrbit :: hasData() const
   {
      return(dataLoaded);
   }

   Xv BrcKeplerOrbit :: svXv(const CommonTime& t) const
      throw(InvalidRequest)
   {
      Xv sv;

      double ea;              // eccentric anomaly //
      double delea;           // delta eccentric anomaly during iteration */
      double elapte;          // elapsed time since Toe 
      double elaptc;          // elapsed time since Toc 
      double dtc,dtr,q,sinea,cosea;
      double GSTA,GCTA;
      double amm;
      double meana;           // mean anomaly 
      double F,G;             // temporary real variables 
      double alat,talat,c2al,s2al,du,dr,di,U,R,truea,AINC;
      double ANLON,cosu,sinu,xip,yip,can,san,cinc,sinc;
      double xef,yef,zef,dek,dlk,div,domk,duv,drv;
      double dxp,dyp,vxef,vyef,vzef;
      GPSEllipsoid ell;

      double sqrtgm = sqrt(ell.gm());

         // Check for ground transmitter
      double twoPI = 2.0e0 * PI;
      double lecc;               // eccentricity
      double tdrinc;            // dt inclination

         lecc = ecc;
         tdrinc = idot;

         // Compute time since ephemeris & clock epochs
      elapte = t - getOrbitEpoch();
      //CommonTime orbEp = getOrbitEpoch();
      //elapte = t - orbEp;

         // Compute mean motion
       amm  = (sqrtgm / (A*Ahalf)) + dn;


         // In-plane angles
         //     meana - Mean anomaly
         //     ea    - Eccentric anomaly
         //     truea - True anomaly

      meana = M0 + elapte * amm;
      meana = fmod(meana, twoPI);
   
      ea = meana + lecc * sin(meana);

      int loop_cnt = 1;
      do  {
         F = meana - ( ea - lecc * sin(ea));
         G = 1.0 - lecc * cos(ea);
         delea = F/G;
         ea = ea + delea;
         loop_cnt++;
      } while ( (fabs(delea) > 1.0e-11 ) && (loop_cnt <= 20) );
   
         // Compute true anomaly
      q     = sqrt ( 1.0e0 - lecc*lecc);
      sinea = sin(ea);
      cosea = cos(ea);
      G     = 1.0e0 - lecc * cosea;
   
         //  G*SIN(TA) AND G*COS(TA)
      GSTA  = q * sinea;
      GCTA  = cosea - lecc;

         //  True anomaly
      truea = atan2 ( GSTA, GCTA );

         // Argument of lat and correction terms (2nd harmonic)
      alat  = truea + w;
      talat = 2.0e0 * alat;
      c2al  = cos( talat );
      s2al  = sin( talat );

      du  = c2al * Cuc +  s2al * Cus;
      dr  = c2al * Crc +  s2al * Crs;
      di  = c2al * Cic +  s2al * Cis;

         // U = updated argument of lat, R = radius, AINC = inclination
      U    = alat + du;
      R    = A*G  + dr;
      AINC = i0 + tdrinc * elapte  +  di;

         //  Longitude of ascending node (ANLON)
         ANLON = OMEGA0 + (OMEGAdot - ell.angVelocity()) *
                 elapte - ell.angVelocity() * Toe;

         // In plane location
      cosu = cos( U );
      sinu = sin( U );

      xip  = R * cosu;
      yip  = R * sinu;

         //  Angles for rotation to earth fixed
      can  = cos( ANLON );
      san  = sin( ANLON );
      cinc = cos( AINC  );
      sinc = sin( AINC  );
 
         // Earth fixed - meters
      xef  =  xip*can  -  yip*cinc*san;
      yef  =  xip*san  +  yip*cinc*can;
      zef  =              yip*sinc;

      sv.x[0] = xef;
      sv.x[1] = yef;
      sv.x[2] = zef;

         // Compute velocity of rotation coordinates
      dek = amm * A / R;
      dlk = Ahalf * q * sqrtgm / (R*R);
      div = tdrinc - 2.0e0 * dlk *
         ( Cic  * s2al - Cis * c2al );
      domk = OMEGAdot - ell.angVelocity();
      duv = dlk*(1.e0+ 2.e0 * (Cus*c2al - Cuc*s2al) );
      drv = A * lecc * dek * sinea - 2.e0 * dlk *
         ( Crc * s2al - Crs * c2al );

      dxp = drv*cosu - R*sinu*duv;
      dyp = drv*sinu + R*cosu*duv;

         // Calculate velocities
      vxef = dxp*can - xip*san*domk - dyp*cinc*san
         + yip*( sinc*san*div - cinc*can*domk);
      vyef = dxp*san + xip*can*domk + dyp*cinc*can
         - yip*( sinc*can*div + cinc*san*domk);
      vzef = dyp*sinc + yip*cinc*div;

         // Move results into output variables
      sv.v[0] = vxef;
      sv.v[1] = vyef;
      sv.v[2] = vzef;

      return sv;
   }

   double BrcKeplerOrbit::svRelativity(const CommonTime& t) const
      throw( InvalidRequest )
   {
      GPSEllipsoid ell;
      double twoPI  = 2.0e0 * PI;
      double sqrtgm = SQRT(ell.gm());
      double elapte = t - getOrbitEpoch();
      double amm    = (sqrtgm / (A*Ahalf)) + dn;
      double meana,F,G,delea;
      

      meana = M0 + elapte * amm; 
      meana = fmod(meana, twoPI);
      double ea = meana + ecc * sin(meana);

      int loop_cnt = 1;
      do  {
         F     = meana - ( ea - ecc * sin(ea));
         G     = 1.0 - ecc * cos(ea);
         delea = F/G;
         ea    = ea + delea;
         loop_cnt++;
      } while ( (ABS(delea) > 1.0e-11 ) && (loop_cnt <= 20) );
      double dtr = REL_CONST * ecc * Ahalf * sin(ea);
      return dtr;
   }

   CommonTime BrcKeplerOrbit::getOrbitEpoch() const
      throw(InvalidRequest)
   {
      CommonTime toReturn;
      if (SatSystemID == 'G' )
         toReturn = GPSWeekSecond(weeknum, Toe, TimeSystem::GPS);
      else if (SatSystemID == 'E' )
         toReturn = GPSWeekSecond(weeknum, Toe, TimeSystem::GAL);
      else
      {
         InvalidRequest exc("Invalid Time System in BrcKeplerOrbit::getOrbitEpoch()");
         GPSTK_THROW(exc);
      }
      return toReturn;
   }
   
   short BrcKeplerOrbit::getPRNID() const
      throw(InvalidRequest)
   {
      if(!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return PRNID;
   }
   
  
   short BrcKeplerOrbit::getFullWeek()  const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return weeknum;
   }
   
  
   double BrcKeplerOrbit::getAccuracy()  const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return accuracy;
   }   
         
   double BrcKeplerOrbit::getCus() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return Cus;
   }
   
   double BrcKeplerOrbit::getCrs() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return Crs;
   }
   
   double BrcKeplerOrbit::getCis() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return Cis;
   }
   
   double BrcKeplerOrbit::getCrc() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return Crc;
   }
   
   double BrcKeplerOrbit::getCuc() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return Cuc;
   }
  
   double BrcKeplerOrbit::getCic() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return Cic;
   }
   
   double BrcKeplerOrbit::getToe() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return Toe;
   }
   
   double BrcKeplerOrbit::getM0() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return M0;
   }
   
   double BrcKeplerOrbit::getDn() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return dn;
   }
   
   double BrcKeplerOrbit::getEcc() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return ecc;
   }
   
   
   double BrcKeplerOrbit::getA() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return A;
   }

   double BrcKeplerOrbit::getAhalf() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return Ahalf;
   }

   double BrcKeplerOrbit::getAdot() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return Adot;
   }

   double BrcKeplerOrbit::getDnDot() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return dndot;
   }
   
   double BrcKeplerOrbit::getOmega0() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return OMEGA0;
   }
   
   double BrcKeplerOrbit::getI0() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return i0;
   }
   
   double BrcKeplerOrbit::getW() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return w;
   }
   
   double BrcKeplerOrbit::getOmegaDot() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return OMEGAdot;
   }
   
   double BrcKeplerOrbit::getIDot() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return idot;
   }

   static void timeDisplay( ostream & os, const CommonTime& t )
   {
         // Convert to CommonTime struct from GPS wk,SOW to M/D/Y, H:M:S.
      GPSWeekSecond dummyTime;
      dummyTime = GPSWeekSecond(t);
//      os << setw(4) << dummyTime.week;
//      os << "(     )  ";
      os << setw(4) << dummyTime.week << "(";
      os << setw(4) << (dummyTime.week & 0x03FF) << ")  ";
      os << setw(6) << setfill(' ') << dummyTime.sow << "   ";

      switch (dummyTime.getDayOfWeek())
      {
         case 0: os << "Sun-0"; break;
         case 1: os << "Mon-1"; break;
         case 2: os << "Tue-2"; break;
         case 3: os << "Wed-3"; break;
         case 4: os << "Thu-4"; break;
         case 5: os << "Fri-5"; break;
         case 6: os << "Sat-6"; break;
         default: break;
      }
      os << "   " << (static_cast<YDSTime>(t)).printf("%3j   %5.0s  ") 
         << (static_cast<CivilTime>(t)).printf("%02m/%02d/%04Y   %02H:%02M:%02S");
   }

   static void shortcut(ostream & os, const long HOW )
   {
      short DOW, hour, min, sec;
      long SOD, SOW;
      short SOH;
      
      SOW  = static_cast<long>( HOW );
      DOW  = static_cast<short>( SOW / SEC_PER_DAY );
      SOD  = SOW - static_cast<long>( DOW * SEC_PER_DAY );
      hour = static_cast<short>( SOD/3600 );

      SOH = static_cast<short>( SOD - (hour*3600) );
      min = SOH/60;

      sec = SOH - min * 60;
      switch (DOW)
      {
         case 0: os << "Sun-0"; break;
         case 1: os << "Mon-1"; break;
         case 2: os << "Tue-2"; break;
         case 3: os << "Wed-3"; break;
         case 4: os << "Thu-4"; break;
         case 5: os << "Fri-5"; break;
         case 6: os << "Sat-6"; break;
         default: break;
      }

      os << ":" << setfill('0')
         << setw(2) << hour
         << ":" << setw(2) << min
         << ":" << setw(2) << sec
         << setfill(' ');
   }

   void BrcKeplerOrbit :: dump(ostream& s) const
      throw()
   {
      ios::fmtflags oldFlags = s.flags();
   
      s.setf(ios::fixed, ios::floatfield);
      s.setf(ios::right, ios::adjustfield);
      s.setf(ios::uppercase);
      s.precision(0);
      s.fill(' ');
      
      s << "****************************************************************"
        << "************" << endl
        << "Broadcast Ephemeris (Engineering Units)" << endl
        << endl
        << "PRN : " << setw(2) << PRNID << endl
        << endl;
  

      s << "              Week(10bt)     SOW     DOW   UTD     SOD"
        << "   MM/DD/YYYY   HH:MM:SS\n";
      
      s << endl;
      s << "Eph Epoch:    ";
      timeDisplay(s, getOrbitEpoch());
      s << endl;
  

      
      s.setf(ios::scientific, ios::floatfield);
      s.precision(8);
       
      s << endl
        << "           ORBIT PARAMETERS"
        << endl
        << endl
        << "Semi-major axis:       " << setw(16) << Ahalf  << " m**.5" << endl
        << "Motion correction:     " << setw(16) << dn     << " rad/sec"
        << endl
        << "Eccentricity:          " << setw(16) << ecc    << endl
        << "Arg of perigee:        " << setw(16) << w      << " rad" << endl
        << "Mean anomaly at epoch: " << setw(16) << M0     << " rad" << endl
        << "Right ascension:       " << setw(16) << OMEGA0 << " rad    "
        << setw(16) << OMEGAdot << " rad/sec" << endl
        << "Inclination:           " << setw(16) << i0     << " rad    "
        << setw(16) << idot     << " rad/sec" << endl;
      
      s << endl
        << "           HARMONIC CORRECTIONS"
        << endl
        << endl
        << "Radial        Sine: " << setw(16) << Crs << " m    Cosine: "
        << setw(16) << Crc << " m" << endl
        << "Inclination   Sine: " << setw(16) << Cis << " rad  Cosine: "
        << setw(16) << Cic << " rad" << endl
        << "In-track      Sine: " << setw(16) << Cus << " rad  Cosine: "
        << setw(16) << Cuc << " rad" << endl;    
      
      s << endl;
      
   } // end of SF123::dump()
   
   ostream& operator<<(ostream& s, const BrcKeplerOrbit& eph)
   {
      eph.dump(s);
      return s;

/* this appears to be more like the dump_eph_table routine of gappc
 * which dumped the bce table.

      s.setf(ios::right);
      s << "prn:" << setw(2) << eph.PRNID
        << ", HOW[0]:" << hex  << setfill('0') << setw(5) << eph.getHOWTime(1)
        << ", IODC:" << hex << setw(3) << eph.getIODC()
        << dec << setfill(' ') << setw(0)
        << ", Toe: [" << bcClock.getToc()-1800*eph.getFitInt()
        << "..." << bcClock.getToc()+1800*eph.getFitInt()
        << ")";
*/

   } // end of operator<<
     
} // namespace
