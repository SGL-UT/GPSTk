#pragma ident "$Id$"


#ifndef MDPPVTSOLUTION_HPP
#define MDPPVTSOLUTION_HPP

#include "MDPHeader.hpp"
#include <Xvt.hpp>

namespace gpstk
{
   /// This class represents a Position-Velocity-Time Solution.
   class MDPPVTSolution : public MDPHeader
   {
   public:
      MDPPVTSolution() throw();

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
         
      virtual std::string getName() const {return "pvt";};

      /** Dump some debugging information to the given ostream.
       * @param out the ostream to write to
       */
      void dump(std::ostream& out) const throw();

      gpstk::ECEF x;              ///< receiver position (x,y,z), earth fixed, in meters
      gpstk::Triple v;            ///< receiver velocity, Earth-fixed, including rotation, in meters/sec
      double dtime;               ///< receiver clock offset from GPS in sec
      double ddtime;              ///< receiver clock drift rate in sec/sec
      gpstk::DayTime timep;       ///< PVT time: dtime = timep - time.
      unsigned char numSVs;       ///< Number of SVs in track.
      unsigned char fom;          ///< Figure of Merit (receiver dependent).
      unsigned char pvtMode;      ///< Type of PVT solution (receiver dependent).
      unsigned char corrections;  ///< Corrections (also receiver dependent).

      static const unsigned myLength = 66;
      static const unsigned myId = 301;

   }; // class MDPPVTSolution

} // namespace gpstk

#endif //MDPPVTSOLUTION_HPP
