//==============================================================================
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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file OrbAlmGen.cpp
 * OrbAlmGen data encapsulated in engineering terms
 */
#include <iomanip>
#include <cmath>

#include "OrbAlmGen.hpp"
#include "BDSWeekSecond.hpp"
#include "CivilTime.hpp"
#include "GNSSconstants.hpp"
#include "GPSEllipsoid.hpp"
#include "GPSWeekSecond.hpp"
#include "IRNWeekSecond.hpp"
#include "NavID.hpp"
#include "StringUtils.hpp"
#include "TimeConstants.hpp"
#include "TimeString.hpp"
#include "TimeSystem.hpp"
#include "YDSTime.hpp"

using namespace std;

namespace gpstk
{
   const unsigned long OrbAlmGen::ALMANAC_PERIOD_LNAV = 720;     // 12.5 minutes for GPS LNAV
   const unsigned long OrbAlmGen::FRAME_PERIOD_LNAV   =  30;     //  30 seconds for GPS LNAV

   bool OrbAlmGen::WN_set = false; 
   unsigned int OrbAlmGen::WNa_full;
   double OrbAlmGen::t_oa;

   OrbAlmGen::OrbAlmGen()
      : OrbAlm(),
        AHalf(0.0), A(0.0), af0(0.0), af1(0.0),
        OMEGA0(0.0), ecc(0.0), deltai(0.0), OMEGAdot(0.0),
        w(0.0), M0(0.0), health(0)
   {}

   OrbAlmGen::OrbAlmGen( const PackedNavBits& pnb,
                         const unsigned short hArg )
      : OrbAlm(),
        AHalf(0.0), A(0.0), af0(0.0), af1(0.0),
        OMEGA0(0.0), ecc(0.0), deltai(0.0), OMEGAdot(0.0),
        w(0.0), M0(0.0), health(0)
   {
      loadData( pnb, hArg );
   }

     /// Clone method
   OrbAlmGen* OrbAlmGen::clone() const
   {
      return new OrbAlmGen (*this);
   }

   void OrbAlmGen::loadData( const PackedNavBits& pnb,
                    const unsigned short hArg)
   {
      const SatID& sidr = pnb.getsatSys();
      const ObsID& oidr = pnb.getobsID();
      NavID nid(sidr,oidr);

      try
      {
      switch(nid.navType)
      {
         case NavID::ntGPSLNAV:
         {
            loadDataGpsLNAV(pnb);
            break;
         }

         case NavID::ntGPSCNAVL2:
         case NavID::ntGPSCNAVL5: 
         {
            loadDataGpsCNAV(pnb);
            break;
         }
	 
         default:
         {
            stringstream ss;
            ss << "Inappropriate navigation message type: " << nid;
            InvalidParameter ip(ss.str());
            GPSTK_THROW(ip);
         }
      }   // end switch
      }   // end try
      catch(InvalidParameter ip)
      {
         GPSTK_RETHROW(ip);
      }

         // After all this is done, declare that data has been loaded
         // into this object (so it may be used).
      dataLoadedFlag = true;
   }

   //------------------------------------------------------------
   //  When the calling program receives the following pages, 
   //  this should be called to update the almanac time parameters.   
   //    LNAV SF4/Pg 25  
   //    BDS D1 SF5/Pg 8
   //    BDS D2 SF5/Pg 36
   //  This "maintenance" is not required for GPS CNAV
   void OrbAlmGen::loadWeekNumber(const unsigned int WNa, const double toa)
   {
      WNa_full = WNa;
      t_oa = toa;
      WN_set = true;   
   }

      // Alternate entry point for situation in which the WNa/toa
      // are already available as a CommonTime
   void OrbAlmGen::loadWeekNumber(const CommonTime& ct)
   {
      unsigned int week = 0;
      double sow = 0.0; 
      bool successfullySet = false;
      if (ct.getTimeSystem()==TimeSystem::GPS)
      {
         week = static_cast<GPSWeekSecond>(ct).week;
         sow = static_cast<GPSWeekSecond>(ct).sow;
         successfullySet = true;
      }
      else if (ct.getTimeSystem()==TimeSystem::BDT)
      {
         unsigned int week = static_cast<BDSWeekSecond>(ct).week;
         double sow = static_cast<BDSWeekSecond>(ct).sow;
         successfullySet = true;
      }
      if (successfullySet)
         loadWeekNumber(week,sow);
   }

   //------------------------------------------------------------
   //  If the first almanac data page is received prior to the
   //  first definition of the WNa and Toa, estimate the 
   //  WNa_full based on the transmit time of the message.
   //  This assumes that the almanac WNa/toa is "in the future"
   //  from the current time by at least a day. 
   void OrbAlmGen::estimateWeekNumber(const CommonTime& currTime)
   {
      CommonTime adjustedTime = currTime + SEC_PER_DAY;
      unsigned int WN_est = static_cast<GPSWeekSecond>(adjustedTime).week;
      double toa_est = static_cast<GPSWeekSecond>(adjustedTime).sow;
      loadWeekNumber(WN_est, toa_est);
   }

   //------------------------------------------------------------
   //  NOTE: The following block of sv?????? methods are very
   //  similar to the methods is OrbElem, however the additional
   //  terms that are asseumed to be zero (0.0) for almanac messages
   //  have been removed. 
   //
   double OrbAlmGen::svClockBias(const gpstk::CommonTime& t) const
   {
      if (!dataLoaded())
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      double dtc,elaptc;
      elaptc = t - ctToe;
      dtc = af0 + elaptc * af1;

      return dtc;      
   }

   double OrbAlmGen::svClockBiasM(const gpstk::CommonTime& t) const
   {
      if (!dataLoaded())
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      double ret = svClockBias(t);
      ret = ret*C_MPS;
      return (ret);
   }

   double OrbAlmGen::svClockDrift(const gpstk::CommonTime& t) const 
   {
      if (!dataLoaded())
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      double drift;
      drift = af1;
      return drift;
   }

   gpstk::Xvt OrbAlmGen::svXvt(const gpstk::CommonTime& t) const
   {
      if (!dataLoaded())
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      Xvt sv;

      GPSWeekSecond gpsws = (ctToe);
      double ToeSOW = gpsws.sow;
      double ea;              // eccentric anomaly //
      double delea;           // delta eccentric anomaly during iteration */
      double elapte;          // elapsed time since Toe
      //double elaptc;          // elapsed time since Toc
      double q,sinea,cosea;
      double GSTA,GCTA;
      double amm;
      double meana;           // mean anomaly
      double F,G;             // temporary real variables
      double alat,talat,c2al,s2al,du,dr,di,U,R,truea,AINC;
      double ANLON,cosu,sinu,xip,yip,can,san,cinc,sinc;
      double xef,yef,zef,dek,dlk,div,domk,duv,drv;
      double dxp,dyp,vxef,vyef,vzef;
      GPSEllipsoid ell;

         // Compute time since ephemeris & clock epochs
      elapte = t - ctToe;

      /* debug 
      string tstr("%D %w:%02H:%02M:%4.1f %8.2g");
      cout << " t " << printTime(t,tstr)
           << ", ctToe " << printTime(ctToe,tstr)
           << ", adj " << printTime(adj,tstr)
           << ", elapte " << elapte << endl;
      */

      double sqrtgm = SQRT(ell.gm());

         // Compute A at time of interest
      double Ak = A; //+ Adot * elapte;    No Adot term in almanac

      double twoPI = 2.0e0 * PI;
      double lecc;               // eccentricity
      double tdrinc;            // dt inclination

      lecc = ecc;
      tdrinc = 0.0;   //idot;        No idot term in almanac

         // Compute mean motion
      double dnA = 0;    // dn + 0.5 * dndot * elapte;   No correction to mean motion in almanac
      double Ahalf = SQRT(A); 
      amm  = (sqrtgm / (A*Ahalf)) + dnA;  // NOT Ak because this equation
                                          // specifies A0, not Ak.  
         // In-plane angles
         //     meana - Mean anomaly
         //     ea    - Eccentric anomaly
         //     truea - True anomaly

      meana = M0 + elapte * amm;
      meana = fmod(meana, twoPI);

      ea = meana + lecc * ::sin(meana);

      int loop_cnt = 1;
      do  {
         F = meana - ( ea - lecc * ::sin(ea));
         G = 1.0 - lecc * ::cos(ea);
         delea = F/G;
         ea = ea + delea;
         loop_cnt++;
      } while ( (fabs(delea) > 1.0e-11 ) && (loop_cnt <= 20) );

      // Compute clock corrections
      sv.relcorr = svRelativity(t);
      sv.clkbias = svClockBias(t);
      sv.clkdrift = svClockDrift(t);
      sv.frame = ReferenceFrame::WGS84;

         // Compute true anomaly
      q     = SQRT( 1.0e0 - lecc*lecc);
      sinea = ::sin(ea);
      cosea = ::cos(ea);
      G     = 1.0e0 - lecc * cosea;

         //  G*SIN(TA) AND G*COS(TA)
      GSTA  = q * sinea;
      GCTA  = cosea - lecc;

         //  True anomaly
      truea = atan2 ( GSTA, GCTA );

         // Argument of lat and correction terms (2nd harmonic)
      alat  = truea + w;
      talat = 2.0e0 * alat;
      c2al  = ::cos( talat );
      s2al  = ::sin( talat );

         // No harmonic perturbations in the almanac
      du  = 0.0; //c2al * Cuc +  s2al * Cus;
      dr  = 0.0; //c2al * Crc +  s2al * Crs;
      di  = 0.0; //c2al * Cic +  s2al * Cis;

         // U = updated argument of lat, R = radius, AINC = inclination
      U    = alat + du;
      R    = Ak*G  + dr;
      AINC = i0 + tdrinc * elapte  +  di;

         //  Longitude of ascending node (ANLON)
      ANLON = OMEGA0 + (OMEGAdot - ell.angVelocity()) *
              elapte - ell.angVelocity() * ToeSOW;

         // In plane location
      cosu = ::cos( U );
      sinu = ::sin( U );

      xip  = R * cosu;
      yip  = R * sinu;

         //  Angles for rotation to earth fixed
      can  = ::cos( ANLON );
      san  = ::sin( ANLON );
      cinc = ::cos( AINC  );
      sinc = ::sin( AINC  );

         // Earth fixed - meters
      xef  =  xip*can  -  yip*cinc*san;
      yef  =  xip*san  +  yip*cinc*can;
      zef  =              yip*sinc;

      sv.x[0] = xef;
      sv.x[1] = yef;
      sv.x[2] = zef;

         // Compute velocity of rotation coordinates
      dek = amm / G;
      dlk = amm * q / (G*G);
      div = 0.0; 
      /*
      *  idot and Harmonic perturbations set to zero in almanac
      div = tdrinc - 2.0e0 * dlk *
         ( Cic  * s2al - Cis * c2al );
      */

      domk = OMEGAdot - ell.angVelocity();

      duv = dlk; 
      /*
      *  Harmonic perturbations set to zero in almanac
      duv = dlk*(1.e0+ 2.e0 * (Cus*c2al - Cuc*s2al) );
      */ 

      drv = Ak * lecc * dek * sinea; 
      /*
      *  Harmonic perturbations set to zero in almanac
      drv = Ak * lecc * dek * sinea - 2.e0 * dlk *
         ( Crc * s2al - Crs * c2al );
      */

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
      sv.health = health == 0 ? Xvt::Healthy : Xvt::Unhealthy;

      return sv;
   }

   double OrbAlmGen::svRelativity(const gpstk::CommonTime& t) const
   {
      if (!dataLoaded())
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      GPSEllipsoid ell;
      double twoPI  = 2.0e0 * PI;
      double sqrtgm = SQRT(ell.gm());
      double elapte = t - ctToe;
      
         // Compute A at time of interest
      double Ak = A;  // + Adot * elapte;   // No adot in almanac
      
      double dnA = 0;   // dn + 0.5 * dndot * elapte;  No correction to mean motion in almanac
      double Ahalf = SQRT(A);
      double amm    = (sqrtgm / (A*Ahalf)) + dnA;// NOT Ak because this equation
                                                 // specifies A0, not Ak.  
      double meana,F,G,delea;

      meana = M0 + elapte * amm;
      meana = fmod(meana, twoPI);
      double ea = meana + ecc * ::sin(meana);

      int loop_cnt = 1;
      do {
         F     = meana - ( ea - ecc * ::sin(ea));
         G     = 1.0 - ecc * ::cos(ea);
         delea = F/G;
         ea    = ea + delea;
         loop_cnt++;
      } while ( (ABS(delea) > 1.0e-11 ) && (loop_cnt <= 20) );
      
         // Use Ak as we are interested in semi-major axis at this moment.
      double dtr = REL_CONST * ecc * SQRT(Ak) * ::sin(ea); 
      return dtr;      
   }

   bool OrbAlmGen::isSameData(const OrbElemBase* right) const
   {
         // If the right pointer doesn't point to an OrbAlmGen, then 
         // they can't be the same data. 
      if (const OrbAlmGen* rp = dynamic_cast<const OrbAlmGen*>(right)) 
      {       
            // Compare the contents of the basic OrbElemBase data members
            // This is NOT done via the base class because the assumption for
            // almanac are different than in the base.  To be specific:
            // 1. beginValid is set to xmit time, so that comparision would be invalid
            // 2. satID is the transmitting SV.  We're interested in the almanac data and
            //    the satellite that is the subject of that almanac.  
            // 3. The same almanac could be collected on either frequency and still
            //    be the same data.
            // 4. However we can (and will) confirm that the navigation
            //    message type is the same. 
            // 5. endValid is not useful as it is always set to END_OF_TIME.
         if (dataLoadedFlag != right->dataLoadedFlag) return false;
         NavID nid(satID,obsID);
         NavID rightNid(right->satID, right->obsID);
         if (nid.navType    != rightNid.navType)      return false; 
         //if (satID          != right->satID)          return false;
         //if (obsID          != right->obsID)          return false;
         if (ctToe          != right->ctToe)          return false;
         if (isHealthy()    != right->isHealthy())    return false;
         //if (beginValid     != right->beginValid)     return false;
         //if (endValid       != right->endValid)       return false;

            // Don't compare 
            // ctXmit is OK if different
         //if (ctXmit  != rp->ctXmit)  return false;
         if (subjectSV  != rp->subjectSV)  return false; 
         if (AHalf      != rp->AHalf)      return false;
         if (af1        != rp->af1)        return false;
         if (af0        != rp->af0)        return false;
         if (OMEGA0     != rp->OMEGA0)     return false; 
         if (ecc        != rp->ecc)        return false;
         if (deltai     != rp->deltai)     return false;
         if (OMEGAdot   != rp->OMEGAdot)   return false;
         if (w          != rp->w)          return false;
         if (M0         != rp->M0)         return false;
         if (health     != rp->health)     return false; 
         return true;
      }
      return false;       
   }            

   //-----------------------------------------------------------------
   // Following method is untested.  Just as it was written to support
   // debug of a problem, the problem was resolved. 
   std::string OrbAlmGen::listDifferences(const OrbElemBase* right) const
   {
      string retVal;

         // If the right pointer doesn't point to an OrbAlmGen, then 
         // they can't be the same data. 
      if (const OrbAlmGen* rp = dynamic_cast<const OrbAlmGen*>(right)) 
      {       
            // Compare the contents of the basic OrbElemBase data members
            // and return a list of differences.
         if (dataLoadedFlag != right->dataLoadedFlag) retVal += " dataLoaded";
         NavID nid(satID,obsID);
         NavID rightNid(right->satID, right->obsID);
         if (nid.navType    != rightNid.navType)      retVal += " navType"; 
         if (ctToe          != right->ctToe)          retVal += " ctToe";
         if (isHealthy()    != right->isHealthy())    retVal += " healthy";
         if (subjectSV  != rp->subjectSV)  retVal += " subjectSV"; 
         if (AHalf      != rp->AHalf)      retVal += " AHalf";
         if (af1        != rp->af1)        retVal += " af1";
         if (af0        != rp->af0)        retVal += " af0";
         if (OMEGA0     != rp->OMEGA0)     retVal += " OMEGA0"; 
         if (ecc        != rp->ecc)        retVal += " ecc";
         if (deltai     != rp->deltai)     retVal += " deltai";
         if (OMEGAdot   != rp->OMEGAdot)   retVal += " OMEGADot";
         if (w          != rp->w)          retVal += " w";
         if (M0         != rp->M0)         retVal += " M0";
         if (health     != rp->health)     retVal += " health";
      }
      return retVal;       
   }


   void OrbAlmGen::dumpBody(ostream& s) const
   {
      s  << "Parameter              Value" << endl;

      s.setf(ios::scientific, ios::floatfield);
      s.setf(ios::right, ios::adjustfield);
      s.setf(ios::uppercase);
      s.precision(8);
      s.fill(' ');

      s << "AHalf       " << setw(16) << AHalf     << " m**0.5" << endl;
      s << "af1         " << setw(16) << af1       << " sec/sec" << endl;
      s << "af0         " << setw(16) << af0       << " sec" << endl;
      s << "Omega0      " << setw(16) << OMEGA0    << " rad" << endl;
      s << "e           " << setw(16) << ecc         << " dimensionless" << endl;
      s << "deltai      " << setw(16) << deltai    << " rad" << endl;
      s << "Omega_Dot   " << setw(16) << OMEGAdot << " rad/sec" << endl;
      s << "w           " << setw(16) << w         << " rad" << endl;
      s << "M0          " << setw(16) << M0        << " rad" << endl;

      if (subjectSV.system==SatID::systemBeiDou)
      {
         s << "Health                 0x" << hex << setfill('0') << setw(3) 
                        << health << " 9 bits"
                        << setfill(' ') << dec << endl;
           // Health decode
         if ((health & 0x01FF)==0x01FF)
         {
            s << "                            ";
            s << " Satellite permanently off" << endl;
         }
         else
         {
            s << "                            ";
            if (!(health & 0x0100)) s << " Satellite clock OK";
              else s << " Satellite clock status reserved";
            s << endl;
            s << "                            ";
            if (health & 0x0002) s << " NAV Message bad (IOD over limit)";
              else s << " NAV Message OK";
            s << endl;
            s << "                            ";
            if (health & 0x080) s << " B1I signal weak";
              else s << " B1I signal OK";
            s << endl;
            s << "                            ";
            if (health & 0x0040) s << " B2I signal weak";
              else s << " B2I signal OK";
            s << endl;                //  65 4321 bits
            if ((health & 0x003D)!=0) // 011 1101 bits 1 and 3-6 are reserved
            {
               s << "                            ";
               s << " Reserved condition indicated" << endl;
            }
         }
      }
      else
      {
         s << "Health                  0x" << hex << setfill('0') << setw(2) 
                        << health << " 8 bits"
                        << setfill(' ') << dec << endl;
      }
      s << "Xmit by PRN " << setw(16) << satID.id  << endl;
   }

   void OrbAlmGen :: dumpTerse(ostream& s) const
   {

       // Check if the subframes have been loaded before attempting
       // to dump them.
      if (!dataLoaded())
      {
         InvalidRequest exc("No data in the object");
         GPSTK_THROW(exc);
      }
      string tform2("%02m/%02d/%4Y %03j %02H:%02M:%02S");
      stringstream ss; 
      string ssys = SatID::convertSatelliteSystemToString(subjectSV.system); 
      ss << setw(7) << ssys;
      ss << " " << setw(2) << subjectSV.id;
      ss << "  AL ";
      ss << printTime(beginValid,tform2);
      ss << "  toa: ";
      ss << printTime(ctToe,tform2);
      if (isHealthy())
      {
         ss << ",   Healthy";
      }
      else
      {
         ss << ", UNhealthy"; 
      }
      ss << "  xmit PRN: " << setw(2) << satID.id;
      s << ss.str(); 

    } // end of dumpTerse()

   void OrbAlmGen :: dump(ostream& s) const
   {
      ios::fmtflags oldFlags = s.flags();
      dumpHeader(s);
      dumpBody(s);
      dumpFooter(s);
      s.flags(oldFlags);

   } // end of dump()

   void OrbAlmGen::loadDataGpsLNAV(const gpstk::PackedNavBits& msg)
   {
      unsigned short subframe = (unsigned short) msg.asUnsignedLong(49, 3, 1);
      unsigned short SVID = (unsigned short) msg.asUnsignedLong(62,6,1);

      if ( (subframe!=4 && subframe!=5))
      {
         stringstream ss;
         ss << "Expected GPS LNAV subframe 4/5.  Found subframe " << subframe;
         InvalidParameter ip(ss.str());
         GPSTK_THROW(ip); 
      }
      if (SVID>32)
      {
         stringstream ss; 
         ss << "Expected GPS LNAV almanac with SV ID 1-32.  Found SV ID ";
         ss << SVID;
         InvalidParameter ip(ss.str());
         GPSTK_THROW(ip); 
      }

         // If the almanac time parameters are not yet set, estimate 
         // them based on the current transmit time. 
      if (!WN_set)
         estimateWeekNumber(msg.getTransmitTime());

         // Store the transmitting SV
      satID = msg.getsatSys();
      if (satID.id>=MIN_PRN_QZS && satID.id<=MAX_PRN_QZS)
      {
         satID.system = SatID::systemQZSS; 
      }

         // Set the subjectSV (found in OrbAlm.hpp)
      int subjectPRN = SVID;
      if (subjectPRN>0 && satID.system==SatID::systemQZSS)
      {
         subjectPRN += (MIN_PRN_QZS - 1); 
      }
      subjectSV = SatID(subjectPRN, satID.system);


         // Test for default nav data.  It's probably NOT default, so we want this test to 
         // terminate and move on quickly if that's the case. 
      if (subjectSV.id==0)
      {
         unsigned long  sow = (unsigned long) static_cast<GPSWeekSecond>(msg.getTransmitTime()).sow;
         unsigned long offsetInCycle = sow % ALMANAC_PERIOD_LNAV;
         int pageInCycle = (offsetInCycle / FRAME_PERIOD_LNAV) + 1; 
         stringstream ss;
         ss << "Found dummy almanac data from " << satID << " for subframe " << subframe
            << " page " << pageInCycle; 
         InvalidParameter ip(ss.str());
         GPSTK_THROW(ip); 
      }

         // Crack the bits into engineering units. 
      ecc = msg.asUnsignedDouble(68, 16, -21);
      toa = msg.asUnsignedLong(90, 8, 4096);
      deltai = msg.asDoubleSemiCircles(98, 16, -19);
      OMEGAdot = msg.asDoubleSemiCircles(120, 16, -38);
      health = (unsigned short) msg.asUnsignedLong(136, 8, 1);
      AHalf = msg.asUnsignedDouble(150, 24, -11);
      OMEGA0 = msg.asDoubleSemiCircles(180, 24, -23);
      w = msg.asDoubleSemiCircles(210, 24, -23);
      M0 = msg.asDoubleSemiCircles(240, 24, -23);

      const unsigned startBits[] = {270, 289};
      const unsigned numBits[]   = {  8,   3};
      af0 = msg.asSignedDouble(startBits, numBits, 2, -20);
      af1 = msg.asSignedDouble(278, 11, -38); 
      
         // Now work out the derived parameters
      A = AHalf * AHalf;
      i0 = (0.3 * PI) + deltai;   // deltai is already in radians. 

      setHealthy(health==0);

         // Determine fully qualified toa.
         // Assume the toa found in this almanac is either 
         // equal to the WNa, t_oa provided, or within a 
         // week of that time. 
      unsigned short wk = WNa_full;

      if (toa!=t_oa)
      {
         double diff =  (double) toa - (double) t_oa;
         if (diff < -HALFWEEK) wk++;
         if (diff >  HALFWEEK) wk--;
      }
      ctToe = GPSWeekSecond(wk,toa,TimeSystem::GPS); 

         // Determine beginValid.  This is set to the transmit
         // time of this page. 
      beginValid = msg.getTransmitTime();
      beginValid.setTimeSystem(TimeSystem::GPS);

         // Determine endValid.   Based on IS-GPS-200 Table 20-XIII this is set to
         // toa - 70 hours + 144 hours = toa + 74 hours = toa + 266400.
      endValid   = ctToe + 266400.0;
      endValid.setTimeSystem(TimeSystem::GPS);
   }

   void OrbAlmGen::loadDataGpsCNAV(const gpstk::PackedNavBits& msg)
   {
      unsigned short msgType = (unsigned short) msg.asUnsignedLong(14, 6, 1);
      unsigned short SVID = (unsigned short) msg.asUnsignedLong(148,6,1);

      if ( msgType!=37 )
      {
         stringstream ss;
         ss << "Expected GPS CNAV message 37.  Found message " << msgType;
         InvalidParameter ip(ss.str());
         GPSTK_THROW(ip); 
      }

         // Store the transmitting SV
      satID = msg.getsatSys();

         // Set the subjectSV (found in OrbAlm.hpp)
      subjectSV = SatID(SVID, SatID::systemGPS);

         // Crack the bits into engineering units. 
      unsigned short wna = (unsigned short) msg.asUnsignedLong(127,13,1);
      toa = msg.asUnsignedLong(140, 8, 4096);
      health = (unsigned short) msg.asUnsignedLong(154, 3, 1);
      ecc = msg.asUnsignedDouble(157, 11, -16);
      deltai = msg.asDoubleSemiCircles(168, 11, -14);
      OMEGAdot = msg.asDoubleSemiCircles(179, 11, -33);
      AHalf = msg.asSignedDouble(190, 17, -4);
      OMEGA0 = msg.asDoubleSemiCircles(207, 16, -15);
      w = msg.asDoubleSemiCircles(223, 16, -15);
      M0 = msg.asDoubleSemiCircles(239, 16, -15);
      af0 = msg.asSignedDouble(255, 11, -20);
      af1 = msg.asSignedDouble(266, 10, -37); 
      
         // Now work out the derived parameters
      A = AHalf * AHalf;
      i0 = (0.3 * PI) + deltai;   // deltai is already in radians. 

      setHealthy(false); 
      const ObsID& oidr = msg.getobsID();
      if (oidr.band==ObsID::cbL2 &&
           !(health & 0x02))
      {
         setHealthy(true);
      }
      if (oidr.band==ObsID::cbL5 &&
          !(health & 0x01))
      {
         setHealthy(false); 
      }
      ctToe = GPSWeekSecond(wna,toa,TimeSystem::GPS); 

         // Determine beginValid.  This is set to the transmit
         // time of this page. 
      beginValid = msg.getTransmitTime();
      beginValid.setTimeSystem(TimeSystem::GPS);

         // Determine endValid.   This is set to "end of time" 
      endValid   = CommonTime::END_OF_TIME;
      endValid.setTimeSystem(TimeSystem::GPS);
   }

   ostream& operator<<(ostream& s, const OrbAlmGen& eph)
   {
      try
      {
         eph.dump(s);
      }
      catch(gpstk::Exception& ex)
      {
         GPSTK_RETHROW(ex);
      }
      return s;

   } // end of operator<<

} // namespace
