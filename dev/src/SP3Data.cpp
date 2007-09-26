#pragma ident "$Id$"

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


/**
 * @file SP3Data.cpp
 * Encapsulate SP3 file data, including I/O
 */

#include "SP3Stream.hpp"
#include "SP3Header.hpp"
#include "SP3Data.hpp"
#include "StringUtils.hpp"
#include "DayTime.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
   void SP3Data::reallyPutRecord(FFStream& ffs) const 
      throw(std::exception, FFStreamError, StringException)
   {
      SP3Stream& strm = dynamic_cast<SP3Stream&>(ffs);
      
      string line;
      if(flag == '*') {// output Epoch Header Record
         line = "* ";
         line += time.printf(" %4Y %2m %2d %2H %2M");
         line += " " + rightJustify(time.printf("%.8f"),11);
      }
      else {           // output Position and Clock OR Velocity and Clock Rate Record
         line = flag;
         if(version == 'c')
            line += SP3SatID(sat).toString();
         else
            line += rightJustify(asString(sat.id),3);
         line += rightJustify(asString(x[0],6),14);
         line += rightJustify(asString(x[1],6),14);
         line += rightJustify(asString(x[2],6),14);
         line += rightJustify(asString(clk,6),14);
         if(version == 'c') {
            line += rightJustify(asString(sig[0]),3);
            line += rightJustify(asString(sig[1]),3);
            line += rightJustify(asString(sig[2]),3);
            line += rightJustify(asString(sig[3]),4);
            if(flag == 'P') {
               line += string(" ");
               line += (clockEventFlag ? string("E") : string(" "));
               line += (clockPredFlag ? string("P") : string(" "));
               line += string("  ");
               line += (orbitManeuverFlag ? string("M") : string(" "));
               line += (orbitPredFlag ? string("P") : string(" "));
            }
            //else {
            //   line += string("       ");
            //}
         }

         // if version is 'c' and correlation flag is set,
         // output P|V Correlation Record
         if(version == 'c' && correlationFlag) {
            // first output the P|V record
            strm << line << endl;

            // now output the correlation record
            if(flag == 'P') line = "EP ";
            else line = "EV ";
            line += rightJustify(asString(sdev[0]),5);
            line += rightJustify(asString(sdev[1]),5);
            line += rightJustify(asString(sdev[2]),5);
            line += rightJustify(asString(sdev[3]),8);
            for(int i=0; i<6; i++)
               line += rightJustify(asString(correlation[i]),9);
         }
      }

      // write the line
      strm << line << endl;
   }

   void SP3Data::dump(ostream& s) const 
   {
      s << flag << " " << sat
         << " " << time.printf("%Y/%02m/%02d %2H:%02M:%06.3f = %F/%10.3g");
      if(flag != '*') {
         s << fixed << setprecision(6)
           << " X=" << setw(14) << x[0]
           << " Y=" << setw(14) << x[1]
           << " Z=" << setw(14) << x[2]
           << " C=" << setw(14) << clk;

         if(version == 'c') {
            s << " sX=" << setw(2) << sig[0]
              << " sY=" << setw(2) << sig[1]
              << " sZ=" << setw(2) << sig[2]
              << " sC=" << setw(3) << sig[3];
            if(flag == 'P')
              s << " " << (clockEventFlag ? "clockEvent" : "-")
                << " " << (clockPredFlag ? "clockPrediction" : "-")
                << " " << (orbitManeuverFlag ? "orbitManeuver" : "-")
                << " " << (orbitPredFlag ? "orbitPrediction" : "-");
            if(correlationFlag)
               s << endl
                 << 'E' << flag
                 << " cXX=" << setw(4) << sdev[0]
                 << " cYY=" << setw(4) << sdev[1]
                 << " cZZ=" << setw(4) << sdev[2]
                 << " cCC=" << setw(7) << sdev[3]
                 << " cXY=" << setw(8) << correlation[0]
                 << " cXZ=" << setw(8) << correlation[1]
                 << " cXC=" << setw(8) << correlation[2]
                 << " cYZ=" << setw(8) << correlation[3]
                 << " cYC=" << setw(8) << correlation[4]
                 << " cZC=" << setw(8) << correlation[5];
         }
      }
      s << endl;
   };

   void SP3Data::reallyGetRecord(FFStream& ffs)
      throw(std::exception, FFStreamError, StringException)
   {
      SP3Stream& strm = dynamic_cast<SP3Stream&>(ffs);

      correlationFlag = false;
      int status = 0;                                          // initial status = 0
      while(1) {
         // set the time in the record
         time = strm.currentEpoch;

         // ---------------------------------------------------------
         // process the buffer containing the last line read
         if(strm.buffer.size() < 3) {
            // nothing in buffer - do nothing here, get another line
            ;
         }

         else if(strm.buffer.substr(0,3) == string("EOF")) {      // 'EOF' record
            // if a data record has been processed during this call, then
            // return and let the next call process this EOF.
            //if(status == 1) throw - found EOF right after an epoch record
            if(status > 1) break;

            // this next read had better fail - if it does, an exception will
            // be thrown, and the FFStreamError created next won't get thrown
            strm.formattedGetLine(strm.buffer, true);
         
            FFStreamError err("EOF text found but file didn't end");
            GPSTK_THROW(err);
         }

         else if(strm.buffer[0] == '*') {                         // Epoch record
            // if another record has been process during this call, quit now
            if(status > 0) break;
            status = 1;                                           // epoch status = 1

            // throw if the line is short
            if(strm.buffer.size() <= 30) {
               FFStreamError err("Invalid line length "+asString(strm.buffer.size()));
               GPSTK_THROW(err);                  
            }

            // parse the epoch line
            int year = asInt(strm.buffer.substr(3,4));
            int month = asInt(strm.buffer.substr(8,2));
            int dom = asInt(strm.buffer.substr(11,2));
            int hour = asInt(strm.buffer.substr(14,2));
            int minute = asInt(strm.buffer.substr(17,2));
            double second = asInt(strm.buffer.substr(20,10));
            DayTime t;
            try {
               t = DayTime(year, month, dom, hour, minute, second);
            } catch (DayTime::DayTimeException& e) {
               FFStreamError e("Invalid time in:" + strm.buffer);
               GPSTK_THROW(e);
            }               
            time = strm.currentEpoch = t;
         }

         else if(strm.buffer[0] == 'P' || strm.buffer[0] == 'V') {// P|V record
            // if nothing, or epoch record, was processed during this call,
            // process this P|V, otherwise (P|V or EP|V were processed), quit now
            if(status > 1) break;
            status = 2;                                           // P|V status = 2

            flag = strm.buffer[0];     // P or V

            // 6/20/07 IGS SP3c files sometimes have short lines, apparently implying
            // zero sigma and no flags...add the following to be tolerant of this.
            if (version == 'c' && strm.buffer.size() < 73 && strm.buffer.size() > 59)
               leftJustify(strm.buffer,73);

            // throw if the line is short
            if ((version == 'a' && strm.buffer.size() < 60) ||
               (version == 'c' && strm.buffer.size() < 73) ) {
               FFStreamError err("Invalid line length "+asString(strm.buffer.size()));
               GPSTK_THROW(err);
            }

            // parse the line
            if(version == 'a')
               sat = SatID(asInt(strm.buffer.substr(1, 3)), SP3SatID::systemGPS);
            else
               sat = SP3SatID(strm.buffer.substr(1,3));

            x[0] = asDouble(strm.buffer.substr(4,14));
            x[1] = asDouble(strm.buffer.substr(18,14));
            x[2] = asDouble(strm.buffer.substr(32,14));
            clk = asDouble(strm.buffer.substr(46,14));

            if(version == 'c') {
               // get sigmas from P|V record
               sig[0] = asInt(strm.buffer.substr(61,2));
               sig[1] = asInt(strm.buffer.substr(64,2));
               sig[2] = asInt(strm.buffer.substr(67,2));
               sig[3] = asInt(strm.buffer.substr(70,3));

               // get flags
               if(flag == 'P') {
                  clockEventFlag = clockPredFlag
                     = orbitManeuverFlag = orbitPredFlag = false;
                  if(strm.buffer.size() >= 75 && strm.buffer[74] == 'E')
                     clockEventFlag = true;
                  if(strm.buffer.size() >= 76 && strm.buffer[75] == 'P')
                     clockPredFlag = true;
                  if(strm.buffer.size() >= 79 && strm.buffer[78] == 'M')
                     orbitManeuverFlag = true;
                  if(strm.buffer.size() >= 80 && strm.buffer[79] == 'P')
                     orbitPredFlag = true;
               }
            }

         }
         else if(strm.buffer[0] == 'E' &&                      // EP|EV record
               (strm.buffer[1] == 'P' || strm.buffer[1] == 'V')) {

            // always process an EP|V immediately, since it must follow P|V
            status = 3;                                        // EP|V status = 3

            // throw if correlation record did not follow corresponding P|V record
            if(strm.buffer[1] != flag) {
               Exception e("SP3c correlation record mismatched with previous P|V");
               GPSTK_THROW(e);
            }

            // throw if line is short
            if(strm.buffer.size()<80) {
               FFStreamError err("Invalid SP3c correlation line length "
                  + asString(strm.buffer.size()));
               GPSTK_THROW(err);
            }

            sdev[0] = abs(asInt(strm.buffer.substr(4,4)));
            sdev[1] = abs(asInt(strm.buffer.substr(9,4)));
            sdev[2] = abs(asInt(strm.buffer.substr(14,4)));
            sdev[3] = abs(asInt(strm.buffer.substr(19,7)));
            correlation[0] = asInt(strm.buffer.substr(27,8));
            correlation[1] = asInt(strm.buffer.substr(36,8));
            correlation[2] = asInt(strm.buffer.substr(45,8));
            correlation[3] = asInt(strm.buffer.substr(54,8));
            correlation[4] = asInt(strm.buffer.substr(63,8));
            correlation[5] = asInt(strm.buffer.substr(72,8));

            // tell the caller that correlation data is now present
            correlationFlag = true;
         }

         else {                              // Unknown record
            FFStreamError err("Unknown line label " + strm.buffer.substr(0,2));
            GPSTK_THROW(err);
         }

         // ---------------------------------------------------------
         // read next line into the buffer
         strm.formattedGetLine(strm.buffer);

         // ---------------------------------------------------------
         // quit if EP|EV was processed
         if(status == 3) break;
         // go back if buffer was empty    (0)
         // go back if epoch was processed (1)
         // go back if P|V was processed   (2)
      }

   }   // end reallyGetRecord()

} // namespace
