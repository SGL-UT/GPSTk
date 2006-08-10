#pragma ident "$Id$"
//
/** @file This is a class that reads in GPS data obs or nav data without the
    caller needing to know the format the data is suppllied in. The
    observation data formats that are are supported: rinex, smoothmdf, mdp. The 
    navigation data formats that are supported: rinex nav, fic, sp3, mdp
*/

//lgpl-license START
//lgpl-license END

#ifndef DATAREADER_HPP
#define DATAREADER_HPP

#include <string>
#include <vector>

#include "CommandOption.hpp"
#include "EphemerisStore.hpp"
#include "RinexObsData.hpp"
#include "DayTime.hpp"


namespace gpstk
{
   class DataReader
   {
   public:
      DataReader();

      int verbosity;
      std::string timeFormat;

      // a store of epochs for a single receiver/antenna
      typedef std::map<DayTime, RinexObsData> RODEpochMap;

      void read(CommandOption& files);
      void read_msc_file(const std::string& fn);

      unsigned long msid;
      DayTime startTime;
      DayTime stopTime;

      std::vector<std::string> filesRead;

      RODEpochMap rem;
      bool firstHeader; 
      RinexObsHeader roh;
      EphemerisStore* eph;

      bool haveEphData;
      bool haveObsData;
      bool havePosData;

   private:
      void read_rinex_obs_data(const std::string& fn);
      void read_smo_data(const std::string& fn);
      void read_mdp_data(const std::string& fn);
      void read_rinex_nav_data(const std::string& fn);
      void read_fic_data(const std::string& fn);
      void read_sp3_data(const std::string& fn);

      void merge_rinex_obs_header(const RinexObsHeader& newHeader);
   };
}
#endif
