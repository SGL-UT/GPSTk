#pragma ident "$Id$"

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
                   gpstk::RinexObsHeader& roh,
                   const gpstk::DayTime& startTime,
                   const gpstk::DayTime& stopTime);

void read_msc_data(std::string& fn, 
                   unsigned long msid,
                   gpstk::RinexObsHeader& roh);

gpstk::EphemerisStore& read_eph_data(gpstk::CommandOption& files);

gpstk::WxObsData& read_met_data(gpstk::CommandOption& files);
