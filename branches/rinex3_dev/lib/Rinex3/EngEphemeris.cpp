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
 * @file EngEphemeris.cpp
 * Ephemeris data encapsulated in engineering terms
 */

#include "StringUtils.hpp"
#include "icd_200_constants.hpp"
#include "GPSGeoid.hpp"
#include "EngEphemeris.hpp"
#include "GPSWeekSecond.hpp"
#include "YDSTime.hpp"
#include "CivilTime.hpp"
#include "TimeSystem.hpp"

#include <cmath>

using namespace gpstk;

namespace Rinex3
{
   using namespace std;
   using namespace gpstk;

   EngEphemeris::EngEphemeris()
      throw()
   {
      haveSubframe[0] = haveSubframe[1] = haveSubframe[2] = false;

      tlm_message[0] = tlm_message[1] = tlm_message[2] = 0;

      PRNID = tracker = ASalert[0] = ASalert[1] = ASalert[2] = weeknum =
	codeflags = accFlag = health = L2Pdata = 0;

      HOWtime[0] = HOWtime[1] = HOWtime[2] = 0;

      IODC = IODE = 0;
      Toc = af0 = af1 = af2 = Tgd = Cuc = Cus = Crc = Crs =
         Cic = Cis = Toe = M0 = dn = ecc = Ahalf = OMEGA0 = i0 = w =
         OMEGAdot = idot = accuracy = 0.0;

      fitint = 0;
   }

   bool EngEphemeris::addSubframe(const long subframe[10], const int gpsWeek,
                                  short PRN, short track)
      throw( gpstk::InvalidParameter )
   {
      double ficked[60];

      if (!subframeConvert(subframe, gpsWeek, ficked))
         return false;

      int sfnum = static_cast<int>( ficked[4] );
      InvalidParameter exc("Subframe "+StringUtils::asString(sfnum)+
                           " not ephemeris subframe.");

      switch (sfnum)
      {
         case 1:
            tlm_message[0] = (subframe[0] >> 8) & 0x3fff;
            HOWtime[0] = static_cast<long>( ficked[2] );
            ASalert[0] = static_cast<short>( ficked[3] );
            weeknum    = static_cast<short>( ficked[5] );
            codeflags  = static_cast<short>( ficked[6] );
            accFlag    = static_cast<short>( ficked[7] );
            health     = static_cast<short>( ficked[8] );
            IODC       = static_cast<short>( ldexp( ficked[9], -11 ) );
            L2Pdata    = static_cast<short>( ficked[10] );
            Tgd        = ficked[11];
            Toc        = ficked[12];
            af2        = ficked[13];
            af1        = ficked[14];
            af0        = ficked[15];
            tracker    = track;
            PRNID      = PRN;
            haveSubframe[0] = true;
            // convert the accuracy flag to a value...
            accuracy = gpstk::ura2accuracy(accFlag);
            break;

         case 2:
            tlm_message[1] = (subframe[0] >> 8) & 0x3fff;
            HOWtime[1] = static_cast<long>( ficked[2] );
            ASalert[1] = static_cast<short>( ficked[3] );
            IODE       = static_cast<short>( ldexp( ficked[5], -11 ) );
            Crs        = ficked[6];
            dn         = ficked[7];
            M0         = ficked[8];
            Cuc        = ficked[9];
            ecc        = ficked[10];
            Cus        = ficked[11];
            Ahalf      = ficked[12];
            Toe        = ficked[13];
            fitint     = static_cast<short>( ficked[14] );
            AODO       = static_cast<long>( ficked[15] );
            haveSubframe[1] = true;
            break;

         case 3:
            tlm_message[2] = (subframe[0] >> 8) & 0x3fff;
            HOWtime[2] = static_cast<long>( ficked[2] );
            ASalert[2] = static_cast<short>( ficked[3] );
            Cic        = ficked[5];
            OMEGA0     = ficked[6];
            Cis        = ficked[7];
            i0         = ficked[8];
            Crc        = ficked[9];
            w          = ficked[10];
            OMEGAdot   = ficked[11];
            idot       = ficked[13];
            haveSubframe[2] = true;
            break;

         default:
            GPSTK_THROW(exc);
            break;
      } // switch (sfnum)

      return true;
   }

   bool EngEphemeris::addSubframeNoParity(const long subframe[10],
                                          const long gpsWeek,
                                          const short PRN,
                                          const short track)
      throw( gpstk::InvalidParameter )
   {
      long paddedSF[10];
      short PRNArg;
      short trackArg;

      for (int i=0;i<10;++i) 
      {
         paddedSF[i] = subframe[i];
         paddedSF[i] <<= 6;
         paddedSF[i] &= 0x3FFFFFC0;    // Guarantee 2 msb and 6 lsb are zeroes
      }
      PRNArg = PRN;                  
      trackArg = track; 
      return( addSubframe( paddedSF, gpsWeek, PRNArg, trackArg ));
   }
   
   bool EngEphemeris::addIncompleteSF1Thru3(
      const long sf1[8], const long sf2[8], const long sf3[8], 
      const long sf1TransmitSOW, const int gpsWeek,
      const short PRN, const short track)
      throw()
   {
      double ficked[60];

         // Need to provide a valid subframe number in the handover word.
         // While we're at it, we'll fake the A-S bit such that it
         // appears A-S is ON, even though we warn the user NOT to trust
         // returns from the getASAlert() method.
      const long sf1Lead[2] = { 0x00000000, 0x00000900 };
      const long sf2Lead[2] = { 0x00000000, 0x00000A00 };
      const long sf3Lead[2] = { 0x00000000, 0x00000B00 };
      long subframe[10]; 
      
         // Handover word times represent the time of the leading edge of the
         // NEXT subframe.  Therefore, HOW should always correspond to
         //   :06/:36 for SF 1
         //   :12/:42 for SF 2
         //   :18/:48 for SF 3
         // This method hasn't a clue about the accuracy of the SOW input by the
         // user, but it WILL enforce this relationship.
      long frameCount = sf1TransmitSOW / 30;
      long SF1HOWTime = (frameCount * 30) + 6;
      
         // Convert subframe 1 parameters
      subframe[0] = sf1Lead[0];
      subframe[1] = sf1Lead[1];
      int i;
      for (i=0; i<8; ++i) subframe[i+2] = sf1[i];
      
      if (!subframeConvert(subframe, gpsWeek, ficked))
         return false;

      tlm_message[0] = 0;
      HOWtime[0] = SF1HOWTime;
      ASalert[0] = (short)ficked[3];
      weeknum    = (short)ficked[5];
      codeflags  = (short)ficked[6];
      accFlag    = (short)ficked[7];
      health     = (short)ficked[8];
      IODC       = (short)ldexp(ficked[9],-11);
      L2Pdata    = (short)ficked[10];
      Tgd        = ficked[11];
      Toc        = ficked[12];
      af2        = ficked[13];
      af1        = ficked[14];
      af0        = ficked[15];
      tracker    = track;
      PRNID      = PRN;
      haveSubframe[0] = true;
         // convert the accuracy flag to a value...
      accuracy = gpstk::ura2accuracy(accFlag);

      
         // Convert subframe 2 parameters
      subframe[0] = sf2Lead[0];
      subframe[1] = sf2Lead[1];
      for (i=0; i<8; ++i) subframe[i+2] = sf2[i];
      
      if (!subframeConvert(subframe, gpsWeek, ficked))
         return false;
      
      tlm_message[1] = 0;
      HOWtime[1] = SF1HOWTime + 6;
      ASalert[1] = (short)ficked[3];
      IODE       = (short)ldexp(ficked[5],-11);
      Crs        = ficked[6];
      dn         = ficked[7];
      M0         = ficked[8];
      Cuc        = ficked[9];
      ecc        = ficked[10];
      Cus        = ficked[11];
      Ahalf      = ficked[12];
      Toe        = ficked[13];
      fitint     = (short)ficked[14];
      haveSubframe[1] = true;
      
         // Convert subframe 3 parameters
      subframe[0] = sf3Lead[0];
      subframe[1] = sf3Lead[1];
      for (i=0; i<8; ++i) subframe[i+2] = sf3[i];
      
      if (!subframeConvert(subframe, gpsWeek, ficked))
         return false;
      
      tlm_message[2] = 0;
      HOWtime[2] = SF1HOWTime + 12;
      ASalert[2] = (short)ficked[3];
      Cic        = ficked[5];
      OMEGA0     = ficked[6];
      Cis        = ficked[7];
      i0         = ficked[8];
      Crc        = ficked[9];
      w          = ficked[10];
      OMEGAdot   = ficked[11];
      idot       = ficked[13];
      haveSubframe[2] = true;

      return true;
   }

   bool EngEphemeris::isData(short subframe) const
      throw( gpstk::InvalidRequest )
   {
      if ((subframe < 1) || (subframe > 3))
      {
         InvalidRequest exc("Subframe "+StringUtils::asString(subframe)+
                            " is not a valid ephemeris subframe.");
         GPSTK_THROW(exc);
      }

      return haveSubframe[subframe-1];
   }

   void EngEphemeris::setAccuracy(const double& acc)
      throw( gpstk::InvalidParameter )
   {
      if( acc < 0 )
      {
         InvalidParameter exc("SV Accuracy of " + StringUtils::asString(acc) +
                              " meters is invalid.");
         GPSTK_THROW(exc);
      }
      accuracy = acc;
      accFlag = gpstk::accuracy2ura(acc);
   }

   short EngEphemeris :: getFitInterval() const
      throw( gpstk::InvalidRequest )
   {
      short iodc = getIODC();
      short fiti = getFitInt();

         /* check the IODC */
      if (iodc < 0 || iodc > 1023)
      {
            /* error in iodc, return minimum fit */
         return 4;
      }
      
      if (((fiti == 0) &&
           (iodc & 0xFF) < 240 || (iodc & 0xFF) > 255 ))
      {
            /* fit interval of 4 hours */
         return 4;
      }
      else if (fiti == 1)
      {
         if( ((iodc & 0xFF) < 240 || (iodc & 0xFF) > 255))
         {
               /* fit interval of 6 hours */
            return 6;
         }
         else if(iodc >=240 && iodc <=247)
         {
               /* fit interval of 8 hours */
            return 8;
         }
         else if(iodc >= 248 && iodc <= 255 || iodc == 496)
         {
               /* fit interval of 14 hours */
            return 14;
         }
         else if(iodc >= 497 && iodc <=503)
         {
               /* fit interval of 26 hours */
            return 26;
         }
         else if(iodc >= 504 && iodc <=510)
         {
               /* fit interval of 50 hours */
            return 50;
         }
         else if(iodc == 511 || iodc >= 752 && iodc <= 756)
         {
               /* fit interval of 74 hours */
            return 74;
         }
         else if(iodc >= 757 && iodc <= 763)
         {
               /* fit interval of 98 hours */
            return 98;
         }
         else if(iodc >= 764 && iodc <=767 || iodc >=1008 && iodc <=1010)
         {
               /* fit interval of 122 hours */
            return 122;
         }
         else if(iodc >= 1011 && iodc <=1020)
         {
               /* fit interval of 146 hours */
            return 146;
         }
         else
         {
               /* error in the iodc or ephemeris, return minimum
                  fit */
            return 4;
         }
      }
      else
      {
            /* error in ephemeris/iodc, return minimum fit */
         return 4;
      }
      
      return 0; // never reached
   }

   Xvt EngEphemeris::svXvt(const CommonTime& t) const
      throw( gpstk::InvalidRequest )
   {
      Xvt sv;

      double ea;              /* eccentric anomaly */
      double delea;           /* delta eccentric anomaly during iteration */
      double elapte;          /* elapsed time since Toe */
      double elaptc;          /* elapsed time since Toc */
      double dtc,dtr,q,sinea,cosea;
      double GSTA,GCTA;
      double A;               /* semi-major axis */
      double amm;
      double meana;           /* mean anomaly */
      double F,G;             /* temporary real variables */
      double alat,talat,c2al,s2al,du,dr,di,U,R,truea,AINC;
      double ANLON,cosu,sinu,xip,yip,can,san,cinc,sinc;
      double xef,yef,zef,dek,dlk,div,domk,duv,drv;
      double dxp,dyp,vxef,vyef,vzef;
      GPSGeoid geoid;

      double sqrtgm = sqrt(geoid.gm());

         // Check for ground transmitter
      double twoPI = 2.0e0 * PI;
      bool igtran;              // ground transmitter flag
      double lecc;               // eccentricity
      double tdrinc;            // dt inclination
      if ( getAhalf() < 2550.0e0 )
      {
         igtran = true;
         lecc = 0.0e0;
         tdrinc = 0.0e0;
      }
      else
      {
         igtran = false;
         lecc = getEcc();
         tdrinc = getIDot();
      }

         // Compute time since ephemeris & clock epochs
      elapte = t - getEphemerisEpoch();
      elaptc = t - getEpochTime();
   

         // Compute mean motion
      A = getA();
      amm  = (sqrtgm / (A*getAhalf())) + getDn();


         // In-plane angles
         //     meana - Mean anomaly
         //     ea    - Eccentric anomaly
         //     truea - True anomaly
      if (!igtran)
         meana = getM0() + elapte * amm;
      else
         meana = getM0();
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

         // Compute clock corrections
      sv.ddtime = getAf1() + elaptc * getAf2();
      dtc = getAf0() + elaptc * ( sv.ddtime );
      dtr = REL_CONST * lecc * getAhalf() * sin(ea);
      sv.dtime = dtc + dtr;
   
         // Compute true anomaly
      q = sqrt ( 1.0e0 - lecc*lecc);
      sinea = sin(ea);
      cosea = cos(ea);
      G = 1.0e0 - lecc * cosea;
   
         //  G*SIN(TA) AND G*COS(TA)
      GSTA  = q * sinea;
      GCTA  = cosea - lecc;

         //  True anomaly
      truea = atan2 ( GSTA, GCTA );

         // Argument of lat and correction terms (2nd harmonic)
      alat = truea + getW();
      talat = 2.0e0 * alat;
      c2al = cos( talat );
      s2al = sin( talat );

      du  = c2al * getCuc() +  s2al * getCus();
      dr  = c2al * getCrc() +  s2al * getCrs();
      di  = c2al * getCic() +  s2al * getCis();

         // U = updated argument of lat, R = radius, AINC = inclination
      U    = alat + du;
      R    = getA()*G  + dr;
      AINC = getI0() + tdrinc * elapte  +  di;

         //  Longitude of ascending node (ANLON)
      if (!igtran)
         ANLON = getOmega0() + (getOmegaDot() - geoid.angVelocity()) *
            elapte - geoid.angVelocity() * getToe();
      else
         ANLON = getOmega0() - getOmegaDot() * getToe();

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
      dlk = getAhalf() * q * sqrtgm / (R*R);
      div = tdrinc - 2.0e0 * dlk *
         ( getCic()  * s2al - getCis() * c2al );
      domk = getOmegaDot() - geoid.angVelocity();
      duv = dlk*(1.e0+ 2.e0 * (getCus()*c2al - getCuc()*s2al) );
      drv = A * lecc * dek * sinea - 2.e0 * dlk *
         ( getCrc() * s2al - getCrs() * c2al );

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

   double EngEphemeris::svRelativity(const CommonTime& t) const
      throw( gpstk::InvalidRequest )
   {
      GPSGeoid geoid;
      double twoPI = 2.0e0 * PI;
      double sqrtgm = sqrt(geoid.gm());
      double elapte = t - getEphemerisEpoch();
      double elaptc = t - getEpochTime();
      double A = getA();
      double amm  = (sqrtgm / (A*getAhalf())) + getDn();
      double meana,lecc,F,G,delea;
      
      if (getAhalf() < 2550.0e0 ) { lecc = 0.0e0; meana = getM0(); }
      else { lecc = getEcc(); meana = getM0() + elapte * amm; }
      meana = fmod(meana, twoPI);
      double ea = meana + lecc * sin(meana);

      int loop_cnt = 1;
      do  {
         F = meana - ( ea - lecc * sin(ea));
         G = 1.0 - lecc * cos(ea);
         delea = F/G;
         ea = ea + delea;
         loop_cnt++;
      } while ( (fabs(delea) > 1.0e-11 ) && (loop_cnt <= 20) );
      double dtr = REL_CONST * lecc * getAhalf() * sin(ea);
      return dtr;
   }

   double EngEphemeris::svClockBias(const CommonTime& t) const
      throw( gpstk::InvalidRequest )
   {
      double dtc,elaptc;
      elaptc = t - getEpochTime();
      dtc = getAf0() + elaptc * ( getAf1() + elaptc * getAf2() );

      return dtc;
   }

   double EngEphemeris::svClockDrift(const CommonTime& t) const
      throw( gpstk::InvalidRequest )
   {
      double drift,elaptc;
      elaptc = t - getEpochTime();
      drift = getAf1() + elaptc * getAf2();
      return drift;
   }

   unsigned EngEphemeris::getTLMMessage(short subframe) const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[subframe-1])
      {
         InvalidRequest exc("Subframe "+StringUtils::asString(subframe)+
                            " not stored.");
         GPSTK_THROW(exc);
      }
      return tlm_message[subframe-1];
   }
      
   CommonTime EngEphemeris::getTransmitTime() const
      throw()
   {
      CommonTime toReturn;
      toReturn = GPSWeekSecond(getFullWeek(), static_cast<double>(getTot()), TimeSys::GPS);
      return toReturn;
   }

   CommonTime EngEphemeris::getEpochTime() const
      throw( gpstk::InvalidRequest )
   {
      CommonTime toReturn;
      if ( (getToc() - getHOWTime(1)) < -HALFWEEK)
         toReturn = GPSWeekSecond(getFullWeek() + 1, getToc(), TimeSys::GPS);
      else if ( (getToc() - getHOWTime(1)) > HALFWEEK)
         toReturn = GPSWeekSecond(getFullWeek() - 1, getToc(), TimeSys::GPS);
      else
         toReturn = GPSWeekSecond(getFullWeek(), getToc(), TimeSys::GPS);
      return toReturn;
   }

   CommonTime EngEphemeris::getEphemerisEpoch() const
      throw( gpstk::InvalidRequest )
   {
      CommonTime toReturn;
      if ( (getToe() - getHOWTime(1)) < -HALFWEEK)
         toReturn = GPSWeekSecond(getFullWeek() + 1, getToe(), TimeSys::GPS);
      else if ( (getToe() - getHOWTime(1)) > HALFWEEK)
         toReturn = GPSWeekSecond(getFullWeek() - 1, getToe(), TimeSys::GPS);
      else
         toReturn = GPSWeekSecond(getFullWeek(), getToe(), TimeSys::GPS);
      return toReturn;
   }

   short EngEphemeris::getPRNID() const
      throw( gpstk::InvalidRequest )
   {
      if(!haveSubframe[0])
      {
         InvalidRequest exc("Required subframe 1 not stored.");
         GPSTK_THROW(exc);
      }
      return PRNID;
   }
   
   short EngEphemeris::getTracker() const
      throw( gpstk::InvalidRequest )
   {
      if(!haveSubframe[0])
      {
         InvalidRequest exc("Required subframe 1 not stored.");
         GPSTK_THROW(exc);
      }
      return tracker;
   }
   
   double EngEphemeris::getHOWTime(short subframe) const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[subframe-1])
      {
         InvalidRequest exc("Subframe "+StringUtils::asString(subframe)+
                            " not stored.");
         GPSTK_THROW(exc);
      }
         // This return as a double is necessary for sets into CommonTime 
         // to not get confused.  Ints are Zcounts whereas doubles are seconds.
         // This should still return a double after DayTime->CommonTime
         // conversion, for backwards compatibility. [DR]
      return static_cast<double>(HOWtime[subframe-1]);
   }

   short EngEphemeris::getASAlert(short subframe)  const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[subframe-1])
      {
         InvalidRequest exc("Subframe "+StringUtils::asString(subframe)+
                            " not stored.");
         GPSTK_THROW(exc);
      }
      return ASalert[subframe-1];
   }
   
   short EngEphemeris::getFullWeek()  const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[0])
      {
         InvalidRequest exc("Required subframe 1 not stored.");
         GPSTK_THROW(exc);
      }
      return weeknum;
   }
   
   short EngEphemeris::getCodeFlags()  const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[0])
      {
         InvalidRequest exc("Required subframe 1 not stored.");
         GPSTK_THROW(exc);
      }
      return codeflags;
   }
   
   double EngEphemeris::getAccuracy()  const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[0])
      {
         InvalidRequest exc("Required subframe 1 not stored.");
         GPSTK_THROW(exc);
      }
      return accuracy;
   }
   
   short EngEphemeris::getAccFlag()  const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[0])
      {
         InvalidRequest exc("Required subframe 1 not stored.");
         GPSTK_THROW(exc);
      }
      return accFlag;
   }
   
   short EngEphemeris::getHealth() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[0])
      {
         InvalidRequest exc("Required subframe 1 not stored.");
         GPSTK_THROW(exc);
      }
      return health;
   }
   
   short EngEphemeris::getL2Pdata() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[0])
      {
         InvalidRequest exc("Required subframe 1 not stored.");
         GPSTK_THROW(exc);
      }
      return L2Pdata;
   }
   
   short EngEphemeris::getIODC() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[0])
      {
         InvalidRequest exc("Required subframe 1 not stored.");
         GPSTK_THROW(exc);
      }
      return (short)IODC;
   }
   
   short EngEphemeris::getIODE() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[1])
      {
         InvalidRequest exc("Required subframe 2 not stored.");
         GPSTK_THROW(exc);
      }
      return (short)IODE;
   }
   
   long EngEphemeris::getAODO() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[1])
      {
         InvalidRequest exc("Required subframe 2 not stored.");
         GPSTK_THROW(exc);
      }
      return AODO;
   }
   
   double EngEphemeris::getToc() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[0])
      {
         InvalidRequest exc("Required subframe 1 not stored.");
         GPSTK_THROW(exc);
      }
      return Toc;
   }
   
   double EngEphemeris::getAf0() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[0])
      {
         InvalidRequest exc("Required subframe 1 not stored.");
         GPSTK_THROW(exc);
      }
      return af0;
   }
   
   double EngEphemeris::getAf1() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[0])
      {
         InvalidRequest exc("Required subframe 1 not stored.");
         GPSTK_THROW(exc);
      }
      return af1;
   }
   
   double EngEphemeris::getAf2() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[0])
      {
         InvalidRequest exc("Required subframe 1 not stored.");
         GPSTK_THROW(exc);
      }
      return af2;
   }
   
   double EngEphemeris::getTgd() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[0])
      {
         InvalidRequest exc("Required subframe 1 not stored.");
         GPSTK_THROW(exc);
      }
      return Tgd;
   }
   
   double EngEphemeris::getCus() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[1])
      {
         InvalidRequest exc("Required subframe 2 not stored.");
         GPSTK_THROW(exc);
      }
      return Cus;
   }
   
   double EngEphemeris::getCrs() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[1])
      {
         InvalidRequest exc("Required subframe 2 not stored.");
         GPSTK_THROW(exc);
      }
      return Crs;
   }
   
   double EngEphemeris::getCis() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[2])
      {
         InvalidRequest exc("Required subframe 3 not stored.");
         GPSTK_THROW(exc);
      }
      return Cis;
   }
   
   double EngEphemeris::getCrc() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[2])
      {
         InvalidRequest exc("Required subframe 3 not stored.");
         GPSTK_THROW(exc);
      }
      return Crc;
   }
   
   double EngEphemeris::getCuc() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[1])
      {
         InvalidRequest exc("Required subframe 2 not stored.");
         GPSTK_THROW(exc);
      }
      return Cuc;
   }
  
   double EngEphemeris::getCic() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[2])
      {
         InvalidRequest exc("Required subframe 3 not stored.");
         GPSTK_THROW(exc);
      }
      return Cic;
   }
   
   double EngEphemeris::getToe() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[1])
      {
         InvalidRequest exc("Required subframe 2 not stored.");
         GPSTK_THROW(exc);
      }
      return Toe;
   }
   
   double EngEphemeris::getM0() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[1])
      {
         InvalidRequest exc("Required subframe 2 not stored.");
         GPSTK_THROW(exc);
      }
      return M0;
   }
   
   double EngEphemeris::getDn() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[1])
      {
         InvalidRequest exc("Required subframe 2 not stored.");
         GPSTK_THROW(exc);
      }
      return dn;
   }
   
   double EngEphemeris::getEcc() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[1])
      {
         InvalidRequest exc("Required subframe 2 not stored.");
         GPSTK_THROW(exc);
      }
      return ecc;
   }
   
   double EngEphemeris::getAhalf() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[1])
      {
         InvalidRequest exc("Required subframe 2 not stored.");
         GPSTK_THROW(exc);
      }
      return Ahalf;
   }
   
   double EngEphemeris::getA() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[1])
      {
         InvalidRequest exc("Required subframe 2 not stored.");
         GPSTK_THROW(exc);
      }
      return Ahalf * Ahalf;
   }
   
   double EngEphemeris::getOmega0() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[2])
      {
         InvalidRequest exc("Required subframe 3 not stored.");
         GPSTK_THROW(exc);
      }
      return OMEGA0;
   }
   
   double EngEphemeris::getI0() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[2])
      {
         InvalidRequest exc("Required subframe 3 not stored.");
         GPSTK_THROW(exc);
      }
      return i0;
   }
   
   double EngEphemeris::getW() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[2])
      {
         InvalidRequest exc("Required subframe 3 not stored.");
         GPSTK_THROW(exc);
      }
      return w;
   }
   
   double EngEphemeris::getOmegaDot() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[2])
      {
         InvalidRequest exc("Required subframe 3 not stored.");
         GPSTK_THROW(exc);
      }
      return OMEGAdot;
   }
   
   double EngEphemeris::getIDot() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[2])
      {
         InvalidRequest exc("Required subframe 3 not stored.");
         GPSTK_THROW(exc);
      }
      return idot;
   }
   
   short EngEphemeris::getFitInt() const
      throw( gpstk::InvalidRequest )
   {
      if (!haveSubframe[1])
      {
         InvalidRequest exc("Required subframe 2 not stored.");
         GPSTK_THROW(exc);
      }
      return fitint;
   }

   long EngEphemeris::getTot() const 
      throw( gpstk::InvalidRequest )
   {
      if(!haveSubframe[0])
      {
         InvalidRequest exc("Required subframe 1 not stored.");
         GPSTK_THROW(exc);
      }
      if(!haveSubframe[1])
      {
         InvalidRequest exc("Required subframe 2 not stored.");
         GPSTK_THROW(exc);
      }
      if(!haveSubframe[2])
      {
         InvalidRequest exc("Required subframe 3 not stored.");
         GPSTK_THROW(exc);
      }
      
      // MSVC
#ifdef _MSC_VER
      long foo = static_cast<long>( getHOWTime(1) < getHOWTime(2) ) ? getHOWTime(1) : getHOWTime(2);
      foo = ( foo < getHOWTime(3) ) ? foo : getHOWTime(3) ;
#else
      long foo = 
         static_cast<long>( min( getHOWTime(1), 
                                 min( getHOWTime(2), getHOWTime(3) ) ) );
#endif
         // The ephemeris comes on 30 second boundaries, so...
      foo/=30;  
      foo*=30;
      return foo;
   }
   
   EngEphemeris& EngEphemeris::setSF1( unsigned tlm, double how, short asalert, 
                                       short fullweek, short cflags, short acc, 
                                       short svhealth, short iodc, short l2pdata,
                                       double tgd, double toc, double Af2,
                                       double Af1, double Af0, short Tracker, 
                                       short prn )
      throw()
   {

      tlm_message[0] = tlm;
      HOWtime[0] = static_cast<long>( how );
      ASalert[0] = asalert;
      weeknum    = fullweek;
      codeflags  = cflags;
      accFlag    = acc;
      health     = svhealth;
      IODC       = iodc;
      L2Pdata    = l2pdata;
      Tgd        = tgd;
      Toc        = toc;
      af2        = Af2;
      af1        = Af1;
      af0        = Af0;
      tracker    = Tracker;
      PRNID      = prn;
      haveSubframe[0] = true;
      // convert the accuracy flag to a value... 
      accuracy = gpstk::ura2accuracy(accFlag);
      return *this;
   }

   EngEphemeris& EngEphemeris::setSF2( unsigned tlm, double how, short asalert,
                                       short iode, double crs, double Dn, 
                                       double m0, double cuc, double Ecc, 
                                       double cus, double ahalf, double toe, 
                                       short fitInt )
      throw()
   {
      tlm_message[1] = tlm;
      HOWtime[1] = static_cast<long>( how );
      ASalert[1] = asalert;
      IODE       = iode;
      Crs        = crs;
      dn         = Dn;
      M0         = m0;
      Cuc        = cuc;
      ecc        = Ecc;
      Cus        = cus;
      Ahalf      = ahalf;
      Toe        = toe;
      fitint     = fitInt;
      haveSubframe[1] = true;
      return *this;
   }


   EngEphemeris& EngEphemeris::setSF3( unsigned tlm, double how, short asalert,
                                       double cic, double Omega0, double cis, 
                                       double I0, double crc, double W, 
                                       double OmegaDot, double IDot )
      throw()
   {
      tlm_message[2] = tlm;
      HOWtime[2] = static_cast<long>( how );
      ASalert[2] = asalert;
      Cic        = cic;
      OMEGA0     = Omega0;
      Cis        = cis;
      i0         = I0;
      Crc        = crc;
      w          = W;
      OMEGAdot   = OmegaDot;
      idot       = IDot;
      haveSubframe[2] = true;
      return *this;
   }
   
   static void timeDisplay( ostream & os, const Rinex3::CommonTime& t )
   {
         // Convert to CommonTime struct from GPS wk,SOW to M/D/Y, H:M:S.
      GPSWeekSecond dummyTime;
      dummyTime = GPSWeekSecond(t);
      os << setw(4) << dummyTime.week;
//      os << setw(4) << dummyTime.week << "(";
//      os << setw(4) << t.GPS10bitweek() << ")  ";
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
      
      SOW = static_cast<long>( HOW );
      DOW = static_cast<short>( SOW / SEC_PER_DAY );
      SOD = SOW - static_cast<long>( DOW * SEC_PER_DAY );
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

   void EngEphemeris :: dump(ostream& s) const
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
      s << "Clock Epoch:  ";
      timeDisplay(s, getEpochTime());
      s << endl;
      s << "Eph Epoch:    ";
      timeDisplay(s, getEphemerisEpoch());
      s << endl;
  
#if 0
         // FIX when moved from sf123, the tot got zapped.. because in
         // order for engephemeris to be able to use such a thing, it
         // needs to be pulled out of as-broadcast bits somehow.
      s << "Transmit time:" << setw(4) << weeknum << ", sow=" << Tot.GPSsecond() << endl
        << "Fit interval flag :  " << setw(2) << fitint
        << " (" << fitintlen << " hours)" << endl;
#elsif 0
      s << "Transmit time:" << setw(4) << weeknum << endl
        << "Fit interval flag :  " << setw(2) << fitint
        << " (" << getFitInt() << " hours)" << endl;
#endif
         // nuts to the above, let's just make it look like navdump output
      s << "Transmit Week:" << setw(4) << weeknum << endl
        << "Fit interval flag :  " << fitint << endl;      
      
      s << endl
        << "          SUBFRAME OVERHEAD"
        << endl
        << endl
        << "               SOW    DOW:HH:MM:SS     IOD    ALERT   A-S\n";
      for (int i=0;i<3;i++)
      {
         s << "SF" << setw(1) << (i+1)
           << " HOW:   " << setw(7) << HOWtime[i]
           << "  ";
         
         shortcut( s, HOWtime[i]);
         if (i==0)
            s << "   ";
         else
            s << "    ";
         
         s << "0x" << setfill('0') << hex;
         
         if (i==0)
            s << setw(3) << IODC;
         else
            s << setw(2) << IODE;
         
         s << dec << "      " << setfill(' ');
         
         if (ASalert[i] & 0x0002)    // "Alert" bit handling
            s << "1     ";
         else
            s << "0     ";
         
         if (ASalert[i] & 0x0001)     // A-S flag handling
            s << " on";
         else
            s << "off";
         s << endl;
      }
      
      s.setf(ios::scientific, ios::floatfield);
      s.precision(8);
      
      s << endl
        << "           CLOCK"
        << endl
        << endl
        << "Bias T0:     " << setw(16) << af0 << " sec" << endl
        << "Drift:       " << setw(16) << af1 << " sec/sec" << endl
        << "Drift rate:  " << setw(16) << af2 << " sec/(sec**2)" << endl
        << "Group delay: " << setw(16) << Tgd << " sec" << endl;
      
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
      
      s << endl
        << "           SV STATUS"
        << endl
        << endl
        << "Health bits:   0x" << setfill('0')  << setw(2) << health
        << "      URA index: " << setfill(' ') << setw(4) << accFlag << endl
        << "Code on L2:   ";
      
      switch (codeflags)
      {
         case 0:
            s << "reserved ";
            break;
            
         case 1:
            s << " P only  ";
            break;
            
         case 2:
            s << " C/A only";
            break;
            
         case 3:
            s << " P & C/A ";
            break;
            
         default:
            break;
            
      }
      
      s << "    L2 P Nav data:          ";
      if (L2Pdata!=0)
         s << "off";
      else
         s << "on";
      
      s << endl
        << endl;
      s.flags(oldFlags);
      
   } // end of SF123::dump()
   
   ostream& operator<<(ostream& s, const EngEphemeris& eph)
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
        << ", Toe: [" << eph.getToc()-1800*eph.getFitInt()
        << "..." << eph.getToc()+1800*eph.getFitInt()
        << ")";
*/
   } // end of operator<<

} // namespace
