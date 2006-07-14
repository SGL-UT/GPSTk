#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/MDPtools/FormatConversionFunctions.hpp#3 $"

/** @file Translates between various similiar objects */

//lgpl-license START
//lgpl-license END

#ifndef FORMATCONVERSIONFUNCTIONS_HPP
#define FORMATCONVERSIONFUNCTIONS_HPP

#include "RinexObsData.hpp"
#include "RinexNavData.hpp"

#include "EngAlmanac.hpp"

#include "MDPNavSubframe.hpp"
#include "MDPObsEpoch.hpp"

namespace gpstk
{
   // A translation from SNR in dB-Hz to the rinex signal strength indicator
   // values were taken from a header written by teqc
   short snr2ssi(float x);

   // Generates a single prn's worth of rinex data
   gpstk::RinexObsData::RinexObsTypeMap makeRinexObsTypeMap(
      const MDPObsEpoch& moe);

   // generates a single epoch of rinex obs data from mdp data.
   // Updates the rinex obs header as appropriate
   void makeRinexObsData(RinexObsHeader& roh, RinexObsData& rod, 
                         const MDPEpoch& mdp);

   // Try to convert the given pages into an EngAlmanc object. Returns true
   // upon success.
   bool makeEngAlmanac(EngAlmanac& alm,
                       const AlmanacPages& pages);

   // Try to convert the given pages into an EngEphemeris object. Returns true
   // upon success.
   bool makeEngEphemeris(EngEphemeris& eph,
                         const EphemerisPages& pages);
}

#endif
