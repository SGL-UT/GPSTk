
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
//  Dagoberto Salazar - gAGE. 2007
//
//============================================================================

#include "DataStructures.hpp"
//#include "DataHeaders.hpp"
//#include "StringUtils.hpp"
//#include "TypeID.hpp"


/**
 * @file DataStructures.cpp
 * gpstk::DataStructures: Set of several data structures to be used by other GPSTk classes.
 */

using namespace gpstk::StringUtils;
using namespace std;


namespace gpstk
{


    // Input for gnssSatTypeValue from RinexObsHeader
    gnssSatTypeValue& operator>>(const RinexObsHeader& roh, gnssSatTypeValue& f)
    {

        // First, select the right system the data came from
        f.header.source.type = SatIDsystem2SourceIDtype(roh.system);

        // Set the proper name for the receiver
        f.header.source.sourceName = roh.markerName;

        return f;
    }


    // Input for gnssRinex from RinexObsHeader
    gnssRinex& operator>>(const RinexObsHeader& roh, gnssRinex& f)
    {

        // First, select the right system the data came from
        f.header.source.type = SatIDsystem2SourceIDtype(roh.system);

        // Set the proper name for the receiver
        f.header.source.sourceName = roh.markerName;

        // Set the proper antenna type for the receiver
        f.header.antennaType = roh.antType;

        // Set the proper antenna position
        f.header.antennaPosition = roh.antennaPosition;

        return f;
    }


    // Input for gnssSatTypeValue from RinexObsData
    gnssSatTypeValue& operator>>(const RinexObsData& rod, gnssSatTypeValue& f)
    {

        // Fill header epoch with the proper value
        f.header.epoch = rod.time;

        // Extract the observations map and store it in the body
        f.body = FillsatTypeValueMapwithRinexObsData(rod);

        return f;
    }


    // Input for gnssRinex from RinexObsData
    gnssRinex& operator>>(const RinexObsData& rod, gnssRinex& f)
    {

        // Fill header epoch with the proper value
        f.header.epoch = rod.time;

        // Fill header epoch with the proper value
        f.header.epochFlag = rod.epochFlag;

        // Extract the observations map and store it in the body
        f.body = FillsatTypeValueMapwithRinexObsData(rod);

        return f;
    }




    // stream input for gnssSatTypeValue
    std::istream& operator>>(std::istream& i, gnssSatTypeValue& f)
        throw(FFStreamError, gpstk::StringUtils::StringException)
    {
        FFStream* ffs = dynamic_cast<FFStream*>(&i);
        if (ffs)
        {
        try
          {         
            RinexObsStream& strm = dynamic_cast<RinexObsStream&>(*ffs);
      
            // If the header hasn't been read, read it...
            if(!strm.headerRead) strm >> strm.header;

            // Clear out this object
            RinexObsHeader& hdr = strm.header;

            hdr >> f;
      
            std::string line;

            strm.formattedGetLine(line, true);
      
            if (line.size()>80 || line[0] != ' ' || line[3] != ' ' || line[6] != ' ')
            {
                FFStreamError e("Bad epoch line");
                GPSTK_THROW(e);
            }

            // process the epoch line, including SV list and clock bias
            short epochFlag = asInt(line.substr(28,1));
            if ((epochFlag < 0) || (epochFlag > 6))
            {
                FFStreamError e("Invalid epoch flag: " + asString(epochFlag));
                GPSTK_THROW(e);
            }

            f.header.epoch = parseTime(line, hdr);

            short numSvs = asInt(line.substr(29,3));

            RinexObsData rod;

            // Now read the observations ...
            if(epochFlag==0 || epochFlag==1 || epochFlag==6) {
                int isv, ndx, line_ndx;
                vector<SatID> satIndex(numSvs);
                int col=30;
                for (isv=1, ndx=0; ndx<numSvs; isv++, ndx++) {
                    if(! (isv % 13)) {
                        strm.formattedGetLine(line);
                        isv = 1;
                        if(line.size() > 80) {
                            FFStreamError err("Invalid line size:" + asString(line.size()));
                            GPSTK_THROW(err);
                        }
                    }
                    try {
                        satIndex[ndx] = RinexSatID(line.substr(col+isv*3-1, 3));
                    }
                    catch (Exception& e)
                    { 
                        FFStreamError ffse(e);
                        GPSTK_THROW(ffse);
                    }
                }

                for (isv=0; isv < numSvs; isv++)
                {
                    short numObs = hdr.obsTypeList.size();
                    for (ndx=0, line_ndx=0; ndx < numObs; ndx++, line_ndx++)
                    {
                        SatID sat = satIndex[isv];
                        RinexObsHeader::RinexObsType obs_type = hdr.obsTypeList[ndx];
                        if (! (line_ndx % 5))
                        {
                            strm.formattedGetLine(line);
                            line_ndx = 0;
                            if (line.size() > 80)
                            {
                                FFStreamError err("Invalid line size:" + asString(line.size()));
                                GPSTK_THROW(err);
                            }
                        }
               
                        line.resize(80, ' ');
               
                        rod.obs[sat][obs_type].data = asDouble(line.substr(line_ndx*16,   14));
                        rod.obs[sat][obs_type].lli = asInt(    line.substr(line_ndx*16+14, 1));
                        rod.obs[sat][obs_type].ssi = asInt(    line.substr(line_ndx*16+15, 1));
                    }
                }


            }

            f.body = FillsatTypeValueMapwithRinexObsData(rod);

            return i;
          }  // End of "try" block
          ////
          //// ATENTION: This part is VERY UGLY
          ////   Help from the guys who wrote FFStream::tryFFStreamGet(FFData& rec)
          ////   will be very appreciated
          ////
          // EOF - do nothing - eof causes fail() to be set which
          // is handled by std::fstream
          catch (EndOfFile& e)
          {
            return i;
          }
          catch (...)
          {
            return i;
          }

        }  // End of block: "if (ffs)..."
        else
        {
            FFStreamError e("operator<< stream argument must be an FFStream");
            GPSTK_THROW(e);
        }
        
    }  // End of stream input for gnssSatTypeValue




    // stream input for gnssRinex
    std::istream& operator>>(std::istream& i, gnssRinex& f)
        throw(FFStreamError, gpstk::StringUtils::StringException)
    {
        FFStream* ffs = dynamic_cast<FFStream*>(&i);
        if (ffs)
        {
        try
          {         
            RinexObsStream& strm = dynamic_cast<RinexObsStream&>(*ffs);
      
            // If the header hasn't been read, read it...
            if(!strm.headerRead) strm >> strm.header;

            // Clear out this object
            RinexObsHeader& hdr = strm.header;

            hdr >> f;
      
            std::string line;

            strm.formattedGetLine(line, true);
      
            if (line.size()>80 || line[0] != ' ' || line[3] != ' ' || line[6] != ' ')
            {
                FFStreamError e("Bad epoch line");
                GPSTK_THROW(e);
            }

            // process the epoch line, including SV list and clock bias
            short epochFlag = asInt(line.substr(28,1));
            if ((epochFlag < 0) || (epochFlag > 6))
            {
                FFStreamError e("Invalid epoch flag: " + asString(epochFlag));
                GPSTK_THROW(e);
            }

            f.header.epochFlag = epochFlag;

            f.header.epoch = parseTime(line, hdr);

            short numSvs = asInt(line.substr(29,3));

            RinexObsData rod;

            // Now read the observations ...
            if(epochFlag==0 || epochFlag==1 || epochFlag==6) {
                int isv, ndx, line_ndx;
                vector<SatID> satIndex(numSvs);
                int col=30;
                for (isv=1, ndx=0; ndx<numSvs; isv++, ndx++) {
                    if(! (isv % 13)) {
                        strm.formattedGetLine(line);
                        isv = 1;
                        if(line.size() > 80) {
                            FFStreamError err("Invalid line size:" + asString(line.size()));
                            GPSTK_THROW(err);
                        }
                    }
                    try {
                        satIndex[ndx] = RinexSatID(line.substr(col+isv*3-1, 3));
                    }
                    catch (Exception& e)
                    { 
                        FFStreamError ffse(e);
                        GPSTK_THROW(ffse);
                    }
                }

                for (isv=0; isv < numSvs; isv++)
                {
                    short numObs = hdr.obsTypeList.size();
                    for (ndx=0, line_ndx=0; ndx < numObs; ndx++, line_ndx++)
                    {
                        SatID sat = satIndex[isv];
                        RinexObsHeader::RinexObsType obs_type = hdr.obsTypeList[ndx];
                        if (! (line_ndx % 5))
                        {
                            strm.formattedGetLine(line);
                            line_ndx = 0;
                            if (line.size() > 80)
                            {
                                FFStreamError err("Invalid line size:" + asString(line.size()));
                                GPSTK_THROW(err);
                            }
                        }
               
                        line.resize(80, ' ');
               
                        rod.obs[sat][obs_type].data = asDouble(line.substr(line_ndx*16,   14));
                        rod.obs[sat][obs_type].lli = asInt(    line.substr(line_ndx*16+14, 1));
                        rod.obs[sat][obs_type].ssi = asInt(    line.substr(line_ndx*16+15, 1));
                    }
                }


            }

            f.body = FillsatTypeValueMapwithRinexObsData(rod);

            return i;
          }  // End of "try" block
          ////
          //// ATENTION: This part is VERY UGLY
          ////   Help from the guys who wrote FFStream::tryFFStreamGet(FFData& rec)
          ////   will be very appreciated
          ////
          // EOF - do nothing - eof causes fail() to be set which
          // is handled by std::fstream
          catch (EndOfFile& e)
          {
            return i;
          }
          catch (...)
          {
            return i;
          }

        }  // End of block: "if (ffs)..."
        else
        {
            FFStreamError e("operator<< stream argument must be an FFStream");
            GPSTK_THROW(e);
        }
        
    }  // End of stream input for gnssRinex



}
