#ifndef GPSTK_TRACKINGCODE_HPP
#define GPSTK_TRACKINGCODE_HPP

#include <string>
#include "EnumIterator.hpp"

namespace gpstk
{
      /** The code used to collect the observation. Each of these
       * should uniquely identify a code that was correlated
       * against to track the signal. While the notation generally
       * follows section 5.1 of RINEX 3, due to ambiguities in that
       * specification some extensions are made. Note that as
       * concrete specifications for the codes are released, this
       * list may need to be adjusted. Specifically, this lists
       * assumes that the same I & Q codes will be used on all
       * three of the Galileo carriers. If that is not true, more
       * identifiers need to be allocated */
   enum class TrackingCode
   {
      Unknown,   ///< Uninitialized value
      Any,       ///< Used to match any tracking code
      CA,        ///< Legacy GPS civil code
      P,         ///< Legacy GPS precise code
      Y,         ///< Encrypted legacy GPS precise code
      W,         ///< Encrypted legacy GPS precise code, codeless Z tracking
      N,         ///< Encrypted legacy GPS precise code, squaring codeless tracking
      D,         ///< Encrypted legacy GPS precise code, other codeless tracking
      M,         ///< Modernized GPS military unique code
      C2M,       ///< Modernized GPS L2 civil M code
      C2L,       ///< Modernized GPS L2 civil L code
      C2LM,      ///< Modernized GPS L2 civil M+L combined tracking
      I5,        ///< Modernized GPS L5 civil in-phase
      Q5,        ///< Modernized GPS L5 civil quadrature
      IQ5,       ///< Modernized GPS L5 civil I+Q combined tracking
      G1P,       ///< Modernized GPS L1C civil code tracking (pilot)
      G1D,       ///< Modernized GPS L1C civil code tracking (data)
      G1X,       ///< Modernized GPS L1C civil code tracking (pilot + data)

      GCA,       ///< Legacy Glonass civil signal
      GP,        ///< Legacy Glonass precise signal
      IR3,       ///< Glonass L3 I code
      QR3,       ///< Glonass L3 Q code
      IQR3,      ///< Glonass L3 I+Q combined tracking
      L1OCD,     ///< GLONASS L1 OCd code
      L1OCP,     ///< GLONASS L1 OCp code
      L1OC,      ///< GLONASS L1 OCd+OCp combined tracking
      L2CSIOCp,  ///< GLONASS L2 CSI+OCp combined tracking
      L2CSI,     ///< GLONASS L2 CSI code
      L2OCP,     ///< GLONASS L2 OCp code

      A,         ///< Galileo L1 PRS code
      B,         ///< Galileo E1-B signal, supporting OS/HAS/SoL
      C,         ///< Galileo E1 Dataless code
      BC,        ///< Galileo E1 B+C combined tracking
      ABC,       ///< Galileo E1 A+B+C combined tracking
      IE5,       ///< Galileo E5 I code
      QE5,       ///< Galileo E5 Q code
      IQE5,      ///< Galileo E5 I+Q combined tracking
      IE5a,      ///< Galileo E5a I code
      QE5a,      ///< Galileo E5a Q code
      IQE5a,     ///< Galileo E5a I+Q combined tracking
      IE5b,      ///< Galileo E5b I code
      QE5b,      ///< Galileo E5b Q code
      IQE5b,     ///< Galileo E5b I+Q combined tracking
      A6,        ///< Galileo E6 PRS code
      B6,        ///< Galileo E6-b signal
      C6,        ///< Galileo E6 Dataless code
      BC6,       ///< Galileo E6 B+C combined tracking
      ABC6,      ///< Galileo E6 A+B+C combined tracking

      SCA,       ///< SBAS civil code
      SI5,       ///< SBAS L5 I code
      SQ5,       ///< SBAS L5 Q code
      SIQ5,      ///< SBAS L5 I+Q code

      JCA,       ///< QZSS civil code
      JD1,       ///< QZSS L1C(D)
      JP1,       ///< QZSS L1C(P)
      JX1,       ///< QZSS L1C(D+P)
      JZ1,       ///< QZSS L1-SAIF
      JM2,       ///< QZSS L2C(M)
      JL2,       ///< QZSS L2C(L)
      JX2,       ///< QZSS L2C(M+L)
      JI5,       ///< QZSS L5 in-phase
      JQ5,       ///< QZSS L5 quadrature
      JIQ5,      ///< QZSS L5 I+Q combined tracking
      JI5S,      ///< QZSS L5S in-phase
      JQ5S,      ///< QZSS L5S I+Q combined tracking
      JIQ5S,     ///< QZSS L5S quadrature
      JI6,       ///< QZSS LEX(6) short
      JQ6,       ///< QZSS LEX(6) long
      JIQ6,      ///< QZSS LEX(6) combined tracking
      JD6,       ///< QZSS L6 Block II D code
      JE6,       ///< QZSS L6 Block II E code
      JDE6,      ///< QZSS L6 Block II D+E combined tracking

      CI1,       ///< BeiDou B1 I code
      CQ1,       ///< BeiDou B1 Q code
      CIQ1,      ///< BeiDou B1 I+Q code
      CI7,       ///< BeiDou B2 I code
      CQ7,       ///< BeiDou B2 Q code
      CIQ7,      ///< BeiDou B2 I+Q code
      CI6,       ///< BeiDou B3 I code
      CQ6,       ///< BeiDou B3 Q code
      CIQ6,      ///< BeiDou B3 I+Q code
      CA1,       ///< BeiDou B1A code
      CCD1,      ///< BeiDou B1C D code
      CCDP1,     ///< BeiDou B1C D+P code
      CCP1,      ///< BeiDou B1C P code
      CI2ab,     ///< BeiDou B2a+b I code
      CIQ2ab,    ///< BeiDou B2a+B I+Q code
      CQ2ab,     ///< BeiDou B2a+B Q code
      CI2a,      ///< BeiDou B2a I code
      CIQ2a,     ///< BeiDou B2a I+Q code
      CQ2a,      ///< BeiDou B2a Q code
      CI2b,      ///< BeiDou B2b I code
      CIQ2b,     ///< BeiDou B2b I+Q code
      CQ2b,      ///< BeiDou B2b Q code
      CodelessC, ///< BeiDou codeless tracking
      CIQ3A,     ///< BeiDou B3A I+Q code

      IA5,       ///< IRNSS L5 SPS
      IB5,       ///< IRNSS L5 RS(D)
      IC5,       ///< IRNSS L5 RS(P)
      IX5,       ///< IRNSS L5 B+C
      IA9,       ///< IRNSS S-band SPS
      IB9,       ///< IRNSS S-band RS(D)
      IC9,       ///< INRSS S-band RS(P)
      IX9,       ///< IRNSS S-band B+C

      Undefined, ///< Code is known to be undefined (as opposed to unknown)
      Last,      ///< Used to verify that all items are described at compile time
   };

      /** Define an iterator so C++11 can do things like
       * for (TrackingCode tc : TrackingCodeIterator()) */
   typedef EnumIterator<TrackingCode, TrackingCode::Unknown, TrackingCode::Last> TrackingCodeIterator;

   namespace StringUtils
   {
         /// Convert a TrackingCode to a whitespace-free string name.
      std::string asString(TrackingCode e) throw();
         /// Convert a string name to an TrackingCode
      TrackingCode asTrackingCode(const std::string& s) throw();
   }

} // namespace gpstk

#endif // GPSTK_TRACKINGCODE_HPP
