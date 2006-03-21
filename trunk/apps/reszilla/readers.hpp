#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/reszilla/readers.hpp#7 $"
//
// Utilitiy functions for reading in various types of data
//

#include <string>

#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"
#include "EphemerisStore.hpp"

#include "util.hpp"


void read_obs_data(gpstk::CommandOption& files,
                   unsigned long msid,
                   RODEpochMap& rem,
                   gpstk::RinexObsHeader& roh);

void read_rinex_data(std::string& fn, 
                     RODEpochMap& rem, 
                     gpstk::RinexObsHeader& roh);

void read_msc_data(std::string& fn, 
                   unsigned long msid,
                   gpstk::RinexObsHeader& roh);

void read_smo_data(std::string& fn,
                   unsigned long msid,
                   RODEpochMap& rem, 
                   gpstk::RinexObsHeader& roh);

void read_pec_data(std::string& fn,
                   unsigned long msid,
                   RODEpochMap& rem);

gpstk::EphemerisStore& read_eph_data(gpstk::CommandOption& files);

gpstk::WxObsData& read_met_data(gpstk::CommandOption& files);


