#pragma ident "$Id$"


#include <sstream>
#include <StringUtils.hpp>
#include <StringUtils.hpp>
#include <BinUtils.hpp>
#include <gps_constants.hpp>

#include "MDPObsEpoch.hpp"
#include "MDPStream.hpp"

using gpstk::StringUtils::asString;
using gpstk::BinUtils::hostToNet;
using gpstk::BinUtils::netToHost;
using gpstk::BinUtils::encodeVar;
using gpstk::BinUtils::decodeVar;
using namespace std;

namespace gpstk
{
   //---------------------------------------------------------------------------
   MDPObsEpoch::MDPObsEpoch()
      throw()
   {
      id = myId;
   } // MDPObsEpoch::MDPObsEpoch()


   //---------------------------------------------------------------------------
   string MDPObsEpoch::encode() const
      throw()
   {
      string str;
      unsigned short ustemp;
      unsigned char svsobs = (numSVs & 0x0f);
      svsobs <<= 4;
      svsobs |= ((unsigned char)obs.size() & 0x0f);
      
      str += encodeVar( static_cast<uint8_t>(svsobs));
      str += encodeVar( static_cast<uint8_t>(channel));
      str += encodeVar( static_cast<uint8_t>(prn));
      str += encodeVar( static_cast<uint8_t>(status & 0x3f));
      str += encodeVar( static_cast<uint16_t>(elevation * 100));
      str += encodeVar( static_cast<uint16_t>(azimuth * 100));

      for(ObsMap::const_iterator oli = obs.begin(); 
          oli != obs.end(); oli++)
      {
         MDPObsEpoch::Observation mdpobs(oli->second);
         str += mdpobs.encode();
      }
      return str;
   } // MDPObsEpoch::encode()

      
   //---------------------------------------------------------------------------
   void MDPObsEpoch::decode(string str)
      throw()
   {
      if (str.length() < myLength)
         return;

      clearstate(lenbit);

      unsigned obsSVs;
      obsSVs    = decodeVar<uint8_t>(str);
      channel   = decodeVar<uint8_t>(str);
      prn       = decodeVar<uint8_t>(str);
      status    = decodeVar<uint8_t>(str);
      elevation = decodeVar<uint16_t>(str) * 0.01 ;
      azimuth   = decodeVar<uint16_t>(str) * 0.01;
         
      numSVs = obsSVs >> 4;
      numSVs &= 0x0f;
      obsSVs &= 0x0f;

      bool obsError=false;

      obs.erase(obs.begin(), obs.end());
      for(int j=0; j<obsSVs; j++)
      {
         if (str.length() < myObsLength)
         {
            clear(lenbit);
            if (debugLevel)
               cout << "MDP Obs block decode requires at least " << myObsLength
                    << " bytes.  Received " << str.length() << " bytes" << endl;
            return;
         }
         MDPObsEpoch::Observation o;      
         o.decode(str);

         if (o.carrier >= ccMax || o.range >= rcMax || o.snr > 65 || o.bw > 100)
            obsError=true;

         if (obsError && debugLevel)
         {            
            if (o.carrier >= ccMax)
               cout << "Carrier code out of range: " << o.carrier << endl;
            if (o.range >= rcMax)
               cout << "Range code out of range: " << o.range << endl;
            if (o.snr > 65)
               cout << "SNR out of range: " << o.snr << endl;
            if (o.bw > 100)
               cout << "BW out of range: " << o.snr << endl;
         }
  
         if (o.carrier < ccMax && o.range < rcMax)
         {
            ObsKey key(o.carrier, o.range);
            obs[key] = o;
         }
      }

      if (prn > gpstk::MAX_PRN || elevation>90 || azimuth > 360 || obsError)
      {
         if (debugLevel)
         {
            if (prn > gpstk::MAX_PRN)
               cout << "PRN of range: " << prn << endl;
            if (elevation > 90)
               cout << "Elevation of range: " << elevation << endl;
            if (azimuth > 360)
               cout << "Azimuth of range: " << azimuth << endl;
         }
         return;
      }

      clearstate(fmtbit);
   } // MDPObsEpoch::decode()


   //---------------------------------------------------------------------------
   std::string MDPObsEpoch::Observation::encode() const
      throw()
   {
      string str;

      uint8_t ccrc = carrier & 0x0f;
      ccrc <<= 4;
      ccrc |= range & 0x0f;

      str += encodeVar( (uint8_t)  ccrc);
      str += encodeVar( (uint8_t)  bw);
      str += encodeVar( (uint16_t) std::max(static_cast<int>(snr*100),0));
      str += encodeVar( (uint32_t) lockCount);
      str += encodeVar( (double)   pseudorange);
      str += encodeVar( (double)   phase);
      str += encodeVar( (double)   doppler);
      return str;
   } // MDPObservation::encode()


   //---------------------------------------------------------------------------
   void MDPObsEpoch::Observation::decode(std::string& str)
      throw()
   {
      unsigned char ccrc;

      ccrc        = decodeVar<uint8_t>(str);      
      bw          = decodeVar<uint8_t>(str);
      snr         = decodeVar<uint16_t>(str) * 0.01;
      lockCount   = decodeVar<uint32_t>(str);
      pseudorange = decodeVar<double>(str);
      phase       = decodeVar<double>(str);
      doppler     = decodeVar<double>(str);

      range = RangeCode(ccrc & 0x0f);
      ccrc >>= 4;
      carrier = CarrierCode(ccrc & 0x0f);
   }  // MDPObservation::decode()


   bool MDPObsEpoch::haveObservation(
      const CarrierCode cc,
      const RangeCode rc) const
   {
      return (obs.find(ObsKey(cc,rc)) != obs.end());
   }

   MDPObsEpoch::Observation MDPObsEpoch::getObservation(
      const CarrierCode cc, 
      const RangeCode rc) const
   {
      if (haveObservation(cc, rc))
         return obs.find(ObsKey(cc,rc))->second;
      else
         return Observation();
   }


   //---------------------------------------------------------------------------
   void MDPObsEpoch::dump(ostream& out) const
      throw()
   {
      ostringstream oss;

      MDPHeader::dump(oss);

      oss << getName() << "0:"
          << " #SV:" << (int)numSVs
          << " Ch:" << (int)channel
          << " PRN:" << (int)prn
          << " El:" << fixed << setprecision(2) <<elevation
          << " Az:" << azimuth
          << " H:0x" << hex << (int)status
          << dec << endl;

      int j=1;
      for (ObsMap::const_iterator i = obs.begin(); i != obs.end(); i++)
      {
         oss << getName() << j++ << ":";
         i->second.dump(oss);
         oss << endl;
      }
      
      out << oss.str();

   } // MDPObsEpoch::dump()


   //---------------------------------------------------------------------------
   void MDPObsEpoch::Observation::dump(std::ostream& out) const 
      throw()
   {
      using gpstk::StringUtils::asString;

      ostringstream oss;
      oss << " "     << asString(carrier)
          << " "     << asString(range)
          << " BW:"  << bw
          << " SNR:" << snr
          << " LC:"  << lockCount
          << " PR:"  << asString(pseudorange, 3)
          << " PH:"  << asString(phase, 3)
          << " Dop:" << asString(doppler, 3);      
      out << oss.str();
   } // MDPObservation::dump()

} // namespace sglmsn
