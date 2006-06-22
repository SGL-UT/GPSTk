#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/receiver/AshtechMessage.cpp#1 $"

/**
 * @file AshtechMessage.cpp
 * Containers for Ashtech data, conversions to RINEX - definitions.
 */

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
 
#include <string>
#include <iostream>
#include <iomanip>
#include <string.h>

#include "BinUtils.hpp"
#include "StringUtils.hpp"
#include "DayTime.hpp"
#include "EngEphemeris.hpp"
#include "RinexObsHeader.hpp"
#include "icd_200_constants.hpp"
#include "AshtechMessage.hpp"

static bool debug=false;

namespace gpstk
{

   static const double CFF=C_GPS_M/OSC_FREQ;
   static const double wl1=CFF/L1_MULT;
   static const double wl2=CFF/L2_MULT;

   AshtechMessage::AshtechMessage(const std::string& ibuff, ObsSource src, 
                                  ObsFormat fmt)
         : msgSource(src), msgFormat(fmt), buffer(ibuff)
   {
      using namespace std;
      
      // Determine the type of observation
      string label=buffer.substr(0,3);
      if (label=="MCA") msgType = MCA;
      if (label=="MCL") msgType = MCL;
      if (label=="MP1") msgType = MP1;
      if (label=="MP2") msgType = MP2;
      if (label=="MPC") msgType = MPC;      
      if (label=="PBN") msgType = PBEN;
      if (label=="SNV") msgType = SNAV;
      if (label=="EPB") msgType = EPB;
      if (label=="SAL") msgType = SALM;
      if (label=="ALB") msgType = ALB;
      if (label=="ION") msgType = ION;
   }

   bool AshtechMessage::isObs(void) const
   {
     return ( (msgType==MCA) || (msgType==MCL) || (msgType==MP1) ||
              (msgType==MP2) || (msgType==MPC) );
   }

   int AshtechMessage::getSequence(void) const
   {
      int result=-1;

      if ((msgType == MPC) && (msgFormat == ASCII))
      {
         result= 
            StringUtils::asInt(StringUtils::word(buffer,1,','));
      }  
 
      return result;
   }

   int AshtechMessage::getPRN(void) const
   {
      int result=-1;

      if   ((msgType == MPC) && (msgFormat == ASCII)) 
      {
         result= 
            StringUtils::asInt(StringUtils::word(buffer,3,','));
      }  

      if   (msgType==EPB)
      {
         result=StringUtils::asInt(buffer.substr(4,5));
      }  
 
      return result;
   }

   int AshtechMessage::getTracker(void) const
   {
      int result=-1;

      if ((msgType == MPC) && (msgFormat == ASCII))
      {
         result= 
            StringUtils::asInt(StringUtils::word(buffer,6,','));
      }  
 
      return result;
   }

   DayTime AshtechMessage::getEpoch(const DayTime& prevTime) const
   {
      short oldweek = prevTime.GPSfullweek();
      short newweek = oldweek;
      
      double oldsow = prevTime.GPSsecond();
      double newsow;

      DayTime thisTime(prevTime);

      if ((msgType == PBEN)&&(msgFormat == ASCII))
      {
         newsow = 
            StringUtils::asDouble(StringUtils::word(buffer,1,','));

            // A test for week rollover
         if ((newsow+6*DayTime::SEC_DAY)<oldsow)
            newweek++;

         thisTime = DayTime(newweek, newsow);
      }

      if ((msgType == MPC)&&(msgFormat == ASCII))
      {
         int seqDiff = getSequence() - calculateSequenceNumber(prevTime);
         // Nominally seqDiff should be small. Check if there is a rollover
         // between the two inputs sequences numbers.
         while (seqDiff > (1800*50/2))
		seqDiff -= 1800*50;

         thisTime.addMilliSeconds(50 * seqDiff);
      }

      return thisTime;
   }

   float AshtechMessage::engSNR(short value, float equivalentNoiseBW)
   {
      const int   n = 20000; // number of samples in 1 ms
      const float m = 4.14;  // magnitude of the carrier estimate;

      const float d = gpstk::PI/(n*n*m*m*4.0);      
      float snr;

      if (value)
      {
         snr = exp(((float)value)/25.0);
         snr = snr*snr*equivalentNoiseBW*d;
         snr = 10 * log10(snr);
      }
      else
         snr = 0;

      return snr;
   }
   

   RinexObsData 
      AshtechMessage::convertToRinexObsData(
            const std::list<AshtechMessage> obsMsgs, 
            const DayTime& recentEpoch) 
   throw(gpstk::Exception)
   {
      RinexObsData rod;
         // TODO: Should check to make sure this is really a PBEN
      DayTime epoch;
      std::list<AshtechMessage>::const_iterator first = obsMsgs.begin();
      epoch = (*first).getEpoch(recentEpoch);

      rod.time = epoch;
      rod.numSvs = 0;
      rod.epochFlag = 0;

      std::list<AshtechMessage>::const_iterator i;
      for (i=obsMsgs.begin(); i!=obsMsgs.end(); i++)
      {
         if ((i->msgType == MPC)&&(i->msgFormat == ASCII))
         {
            
            int prn      = StringUtils::asInt(    StringUtils::word(i->buffer,3,','));
            double C1    = StringUtils::asDouble( StringUtils::word(i->buffer,13,',')) * C_GPS_M / 1000.0;
            double P1    = StringUtils::asDouble( StringUtils::word(i->buffer,23,',')) * C_GPS_M / 1000.0;
            double P2    = StringUtils::asDouble( StringUtils::word(i->buffer,33,',')) * C_GPS_M / 1000.0;
            double L1    = StringUtils::asDouble( StringUtils::word(i->buffer,22,','));
            double L2    = StringUtils::asDouble( StringUtils::word(i->buffer,32,','));
            double D1    = StringUtils::asDouble( StringUtils::word(i->buffer,24,','));
            double D2    = StringUtils::asDouble( StringUtils::word(i->buffer,34,','));
            int snrL1    = StringUtils::asInt(    StringUtils::word(i->buffer,20,','));
            int snrL2    = StringUtils::asInt(    StringUtils::word(i->buffer,30,','));

            int warning  = StringUtils::asInt(    StringUtils::word(i->buffer,27,','));

            double S1    = engSNR(snrL1, 9.21e6);
            double S2    = engSNR(snrL2, 9.21e6);

               // Debug check
               // std::cout << prn << " " << D1 << " " << D2 << " " << L1 << " " << L2 << std::endl;
               // exit(0);

            RinexPrn thisSat(prn, systemGPS);
            RinexObsData::RinexObsTypeMap datamap;

            datamap[RinexObsHeader::C1].data = C1; 
            datamap[RinexObsHeader::P1].data = P1; 
            datamap[RinexObsHeader::P2].data = P2;
            datamap[RinexObsHeader::L1].data = L1;
            datamap[RinexObsHeader::L2].data = L2;
            datamap[RinexObsHeader::D1].data = -D1; // Note sign convention for Doppler is opposite that of RINEX.
            datamap[RinexObsHeader::D2].data = -D2;
            datamap[RinexObsHeader::S1].data = S1; 
            datamap[RinexObsHeader::S2].data = S2;
           
            datamap[RinexObsHeader::L1].lli = 0;
            datamap[RinexObsHeader::L2].lli = 0;
            if (warning & 128) 
            {
               datamap[RinexObsHeader::L1].lli = 1;   
               datamap[RinexObsHeader::L2].lli = 1;   
            }

            datamap[RinexObsHeader::L1].ssi = mapSNRtoSSI(snrL1);
            datamap[RinexObsHeader::L2].ssi = mapSNRtoSSI(snrL2);            
            
            rod.obs[thisSat]=datamap;
            rod.numSvs++;
         }
         
      }

      return rod;   
   }

   int AshtechMessage::calculateSequenceNumber(const DayTime& t)
   {
         // TODO: Throw if not a MBEN
      double secondsOfHour = t.minute()*60+t.second();
      double secondsOfSequence = secondsOfHour;
      while (secondsOfSequence >= 1800.) 
         secondsOfSequence -= 1800.;
      double milliSecondsOfSequence = secondsOfSequence*1000.;
      // There is sequence tick every 50 milliseconds
      return (int)(milliSecondsOfSequence / 50.);
   }

   short AshtechMessage::mapSNRtoSSI(float snr)
   {
      if (snr>34) return 9;
      if (snr>29) return 8;
      if (snr>20) return 5;
      if (snr>10) return 1;
      return 0;
   }

   RinexNavData AshtechMessage::convertToRinexNavData(const AshtechMessage& msg, const DayTime& epoch)
   {
         // TODO: throw if not an EPB type
      using namespace BinUtils;

      int offset=5;
      const char *dptr = msg.buffer.data();

      EngEphemeris eph;
      short PRN = StringUtils::asInt(msg.buffer.substr(4,2));

      long subframe[30];

         //using namespace std;
      
      for (int i=0;i<30;i++)
      {
         subframe[i]=*((long *)(dptr+7+i*4));
#if BYTE_ORDER == LITTLE_ENDIAN
         BinUtils::twiddle(subframe[i]);
#endif
            //    cout << "Word " << dec << i << ": 0x" << hex << subframe[i] << endl << flush << dec;
      }
      
// TODO: throw an exception if these calls fail
      eph.addSubframe(subframe, epoch.GPSfullweek(), PRN, 0);
         //    cout << "sf1" << endl << flush;
      
      eph.addSubframe(subframe+10, epoch.GPSfullweek(), PRN, 0);
         //     cout << "sf2" << endl << flush;

      eph.addSubframe(subframe+20, epoch.GPSfullweek(), PRN, 0);
         // cout << "sf3" << endl << flush;

         // eph.dump(std::cout);

         // exit(0);
      
      return eph;
         //return RinexNavData();
   }

   void AshtechMessage::updateNavHeader(const AshtechMessage& ionMsg, RinexNavHeader& hdr)
   {     
         // TODO: the ION message interpreter is broken. Make it work. Then have the main programm
         // regularly request an ION msg.


         // TODO: Throw if not an ION type... hmm, need an ASSERT macro for types
      using namespace BinUtils;
      using namespace std;

      // Offset between location in the buffer and the location defined in the
      // Ashtech document ZFamily.pdf -- ZFamily GPS Receivers, Technical Ref. Manual, dated 2002
      int offset=5;
      const char *dptr = ionMsg.buffer.data();
      
      // Alpha parameters of Klobuchar model-------------------------------------------------
      float alpha0, alpha1, alpha2, alpha3;
      
      memmove(&alpha0, dptr+offset+0,  4); 
      memmove(&alpha1, dptr+offset+4,  4); 
      memmove(&alpha2, dptr+offset+8,  4); 
      memmove(&alpha3, dptr+offset+12, 4);

#if BYTE_ORDER == LITTLE_ENDIAN
      twiddle(alpha0);
      twiddle(alpha1);
      twiddle(alpha2);
      twiddle(alpha3);
#endif

      hdr.ionAlpha[0] = alpha0;
      hdr.ionAlpha[1] = alpha1;
      hdr.ionAlpha[2] = alpha2;
      hdr.ionAlpha[3] = alpha3;

      hdr.valid |= RinexNavHeader::ionAlphaValid;

      // Beta parameters of Klobuchar model-------------------------------------------------
      float beta0, beta1, beta2, beta3;
      
      memmove(&beta0, dptr+offset+16, 4); 
      memmove(&beta1, dptr+offset+20, 4); 
      memmove(&beta2, dptr+offset+24, 4); 
      memmove(&beta3, dptr+offset+28, 4); 

#if BYTE_ORDER == LITTLE_ENDIAN
      twiddle(beta0);
      twiddle(beta1);
      twiddle(beta2);
      twiddle(beta3);
#endif

      hdr.ionBeta[0] = beta0;
      hdr.ionBeta[1] = beta1;
      hdr.ionBeta[2] = beta2;
      hdr.ionBeta[3] = beta3;

      hdr.valid |= RinexNavHeader::ionBetaValid;
      
      // Ref time parameters of Klobuchar model-------------------------------------------------

      double A0, A1;
      long UTCseconds;
      short UTCweek;
      
      memmove(&A1,         dptr+offset+32, 8);
      memmove(&A0,         dptr+offset+40, 8);
      memmove(&UTCseconds, dptr+offset+48, 4);
      memmove(&UTCweek,    dptr+offset+52, 2);
      
#if BYTE_ORDER == LITTLE_ENDIAN
      twiddle(A1);
      twiddle(A0);
      twiddle(UTCseconds);
      twiddle(UTCweek);
#endif
      
      hdr.A0 = A0;
      hdr.A1 = A1;
      hdr.UTCRefWeek = UTCweek;
      hdr.UTCRefTime = UTCseconds;
      
      hdr.valid |= RinexNavHeader::deltaUTCValid;
      
      // Leap seconds --------------------------------------------------------------------------
      
      short leapSeconds;
      
      memmove(&leapSeconds, dptr+52, 2);

#if BYTE_ORDER == LITTLE_ENDIAN
      twiddle(leapSeconds);
#endif

      hdr.leapSeconds = leapSeconds;

      hdr.valid |= RinexNavHeader::leapSecondsValid;      

      return;   
   }
   
} // End namespace
