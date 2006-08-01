#pragma ident "$Id$"


#ifndef MDPOBSEPOCH_HPP
#define MDPOBSEPOCH_HPP

#include <map>
#include "MDPHeader.hpp"
#include "miscenum.hpp"

namespace gpstk
{
   /// This class represents a set of observations from a single SV at a 
   /// single point in time
   class MDPObsEpoch : public MDPHeader
   {
   public:
      // A helper class to hold a single code/carrier block of observations
      class Observation
      {
      public:
         void dump(std::ostream& out) const throw();
         std::string encode() const throw();

         // Note that this is a destructive decode, unlike the others in this set 
         // of classes.
         void decode(std::string& str) throw();
  
         CarrierCode carrier;     ///< This observation's carrier frequency code.
         RangeCode range;         ///< This observation's range code.
         unsigned bw;             ///< The Effective Tracking Loop Bandwidth. (Hz)
         double snr;              ///< The Signal-to-Noise Ratio. (dB-Hz)
         unsigned long lockCount; ///< The number of consecutive observations (0 implies a cycle slip or beginning of track).
         double pseudorange;      ///< Pseudorange Measurement (meters)
         double phase;            ///< Phase Measurement (cycles)
         double doppler;          ///< Doppler Measurement (Hz, positive for approaching satellites)
      };

      MDPObsEpoch() throw();

      /**  Encode this object to a string.
       * @return this object as a string.
       */
      virtual std::string encode() const
         throw();
         
      /** Decode this object from a string.
       * @param str the string to read from.
       * @note This is a non-destructive decode.
       */
      virtual void decode(std::string str)
         throw();
         
      virtual std::string getName() const {return "obs";};

      /** Dump some debugging information to the given ostream.
       * @param out the ostream to write to
       */
      void dump(std::ostream& out) const throw();

      typedef std::pair<CarrierCode, RangeCode> ObsKey;
      typedef std::map<ObsKey, Observation> ObsMap;
   
      unsigned numSVs;      ///< The number of SVs in track for this epoch.
      unsigned channel;     ///< The channel this SV is tracked on.
      unsigned prn;         ///< The SV's PRN.
      unsigned status;      ///< SV Health status.
      double elevation;     ///< The SV's elevation in degrees.
      double azimuth;       ///< The SV's azimuth in degrees.

      ObsMap obs;          ///< The list of Observations made at this Epoch.
      bool haveObservation(const CarrierCode cc, const RangeCode rc) const;
      Observation getObservation(const CarrierCode cc, const RangeCode rc) const;

      static const unsigned myId = 300;
      static const unsigned myLength = 8;  // This is just the pre obs_block stuff
      static const unsigned myObsLength=32;  // This is the size of a single obs_block

   }; // class MDPObsEpoch

   // The key is intended to the PRN of the MDPObsEpoch
   typedef std::map<int, gpstk::MDPObsEpoch> MDPEpoch;
      
} // namespace gpstk

#endif //MDPOBSEPOCH_HPP
