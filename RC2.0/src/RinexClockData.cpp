#pragma ident "$Id$"

/**
 * @file RinexClockData.cpp
 * Encapsulate RinexClock file data, including I/O
 */

#include "RinexClockStream.hpp"
#include "RinexClockHeader.hpp"
#include "RinexClockData.hpp"
#include "StringUtils.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
   void RinexClockData::reallyPutRecord(FFStream& ffs) const 
      throw(exception, FFStreamError, StringException)
   {
      // cast the stream to be an RinexClockStream
      RinexClockStream& strm = dynamic_cast<RinexClockStream&>(ffs);

      int i;
      string line;

      line = datatype;
      line += string(1,' ');

      if(datatype == string("AR")) {
         line += rightJustify(site,4);
      }
      else if(datatype == string("AS")) {
         line += string(1,sat.systemChar());
         line += rightJustify(asString(sat.id),2);
         if(line[4] == ' ') line[4] = '0';
         line += string(1,' ');
      }
      else {
         FFStreamError e("Unknown data type: " + datatype);
         GPSTK_THROW(e);
      }
      line += string(1,' ');

      line += time.printf("%4Y %02m %02d %02H %02M %9.6f");
      //line += rightJustify(asString(time.year()),4);
      //line += rightJustify(asString(time.month()),3);
      //line += rightJustify(asString(time.day()),3);
      //line += rightJustify(asString(time.hour()),3);
      //line += rightJustify(asString(time.minute()),3);
      //line += rightJustify(asString(time.second(),6),10);

      // must count the data to output
      int n(2);
      if(drift != 0.0) n=3;
      if(sig_drift != 0.0) n=4;
      if(accel != 0.0) n=5;
      if(sig_accel != 0.0) n=6;
      line += rightJustify(asString(n),3);
      line += string(3,' ');

      line += doubleToScientific(bias, 19, 12, 2);
      line += string(1,' ');
      line += doubleToScientific(sig_bias, 19, 12, 2);

      strm << line << endl;
      strm.lineNumber++;

      // continuation line
      if(n > 2) {
         line = doubleToScientific(drift, 19, 12, 2);
         line += string(1,' ');
         if(n > 3) {
            line += doubleToScientific(sig_drift, 19, 12, 2);
            line += string(1,' ');
         }
         if(n > 4) {
            line += doubleToScientific(accel, 19, 12, 2);
            line += string(1,' ');
         }
         if(n > 5) {
            line += doubleToScientific(sig_accel, 19, 12, 2);
            line += string(1,' ');
         }
         strm << line << endl;
         strm.lineNumber++;
      }

   }  // end reallyPutRecord()

   void RinexClockData::reallyGetRecord(FFStream& ffs)
      throw(exception, FFStreamError, StringException)
   {
      // cast the stream to be an RinexClockStream
      RinexClockStream& strm = dynamic_cast<RinexClockStream&>(ffs);

      clear();

      string line;
      strm.formattedGetLine(line,true);      // true means 'expect possible EOF'
      stripTrailing(line);
      if(line.length() < 59) {
         FFStreamError e("Short line : " + line);
         GPSTK_THROW(e);
      }

      //cout << "Data Line: /" << line << "/" << endl;
      datatype = line.substr(0,2);
      site = line.substr(3,4);
      if(datatype == string("AS")) {
         strip(site);
         int prn(asInt(site.substr(1,2)));
         if(site[0] == 'G') sat = RinexSatID(prn,RinexSatID::systemGPS);
         else if(site[0] == 'R') sat = RinexSatID(prn,RinexSatID::systemGlonass);
         else {
            FFStreamError e("Invalid sat : /" + site + "/");
            GPSTK_THROW(e);
         }
         site = string();
      }

      time.setYMDHMS(asInt(line.substr( 8,4)),
                     asInt(line.substr(12,3)),
                     asInt(line.substr(15,3)),
                     asInt(line.substr(18,3)),
                     asInt(line.substr(21,3)),
                     asDouble(line.substr(24,10)));

      int n(asInt(line.substr(34,3)));
      bias = asDouble(line.substr(40,19));
      if(n > 1 && line.length() >= 59) sig_bias = asDouble(line.substr(60,19));

      if(n > 2) {
         strm.formattedGetLine(line,true);
         stripTrailing(line);
         if(line.length() < (n-2)*20-1) {
            FFStreamError e("Short line : " + line);
            GPSTK_THROW(e);
         }
         drift =     asDouble(line.substr( 0,19));
         if(n > 3) sig_drift = asDouble(line.substr(20,19));
         if(n > 4) accel     = asDouble(line.substr(40,19));
         if(n > 5) sig_accel = asDouble(line.substr(60,19));
      }

   }   // end reallyGetRecord()

   void RinexClockData::dump(ostream& s) const throw()
   {
      // dump record type, sat id / site, current epoch, and data
      s << " " << datatype;
      if(datatype == string("AR")) s << " " << site;
      else s << " " << sat.toString();
      s << " " << time.printf("%Y/%02m/%02d %2H:%02M:%06.3f = %F/%10.3g");
      s << scientific << setprecision(12)
         << " " << setw(19) << bias
         << " " << setw(19) << sig_bias;
      if(drift != 0.0) s << " " << setw(19) << drift; else s << " 0.0";
      if(sig_drift != 0.0) s << " " << setw(19) << sig_drift; else s << " 0.0";
      if(accel != 0.0) s << " " << setw(19) << accel; else s << " 0.0";
      if(sig_accel != 0.0) s << " " << setw(19) << sig_accel; else s << " 0.0";
      s << endl;

   }  // end dump()

} // namespace

//------------------------------------------------------------------------------------
