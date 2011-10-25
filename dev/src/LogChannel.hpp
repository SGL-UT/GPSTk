#pragma ident "$Id$"

/**
 * @file LogChannel.hpp
 * 
 */

#ifndef GPSTK_LOGCHANNEL_HPP
#define GPSTK_LOGCHANNEL_HPP

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
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================

#include <string>
#include <fstream>
#include "LogMessage.hpp"

namespace gpstk
{
 
   /// This Formatter allows for custom formatting of
   /// log messages based on format patterns.
   ///
   /// The format pattern is used as a template to format the message and
   /// is copied character by character except for the following special characters,
   /// which are replaced by the corresponding value.
   ///
   ///   * %s - message source
   ///   * %t - message text
   ///   * %l - message priority level (1 .. 7)
   ///   * %p - message priority (Fatal, Critical, Error, Warning, Notice, Information, Debug, Trace)
   ///   * %q - abbreviated message priority (F, C, E, W, N, I, D, T)
   ///   * %U - message source file path (empty string if not set)
   ///   * %u - message source line number (0 if not set)
   ///   * %d - message date/time zero-padded day of month (01 .. 31)
   ///   * %e - message date/time day of month (1 .. 31)
   ///   * %f - message date/time space-padded day of month ( 1 .. 31)
   ///   * %m - message date/time zero-padded month (01 .. 12)
   ///   * %n - message date/time month (1 .. 12)
   ///   * %o - message date/time space-padded month ( 1 .. 12)
   ///   * %y - message date/time year without century (70)
   ///   * %Y - message date/time year with century (1970)
   ///   * %H - message date/time hour (00 .. 23)
   ///   * %M - message date/time minute (00 .. 59)
   ///   * %S - message date/time second (00 .. 59)
   ///   * %[name] - the value of the message parameter with the given name
   ///   * %% - percent sign

   class LogChannel   
   {
   public:

      LogChannel(const std::string& fmt = "%t") 
         : ostrm(std::clog), pattern(fmt) {}      
      
      LogChannel(std::ostream& strm, const std::string& fmt="%t") 
         : ostrm(strm), pattern(fmt) {}      
      
      virtual ~LogChannel(){}

      std::string getLogText(const LogMessage& msg);

      virtual void log(const LogMessage& msg){}

      void setPattern(const std::string& fmt="%t")
      {
         pattern = fmt;
      }
      
   protected:
      std::ostream& ostrm;
      std::string pattern;

   };   // End of class 'LogChannel'

   class ConsoleLogChannel : public LogChannel   
   {
   public:

      ConsoleLogChannel(const std::string& fmt = "%t") 
         : LogChannel(fmt), ostrm(std::clog) {}      

      ConsoleLogChannel(std::ostream& strm, const std::string& fmt="%t") 
         : LogChannel(fmt), ostrm(strm) {}      

      virtual ~ConsoleLogChannel(){}

      virtual void log(const LogMessage& msg)
      {
         ostrm << getLogText(msg) << std::endl;
      }

      void setPattern(const std::string& fmt="%t")
      {
         pattern = fmt;
      }

   public:
      std::ostream& ostrm;
      std::string pattern;

   };   // End of class 'LogChannel'


   class FileLogChannel : public LogChannel
   {
   public:
      FileLogChannel(const std::string& filename, const std::string& fmt = "%t")
         : LogChannel(fmt)  { setFile(filename); }      

      virtual ~FileLogChannel(){fstrm.close();}


      virtual void setFile(const std::string& filename)
      {
         fstrm.open(filename.c_str());
         logFile = filename;
      }

      virtual void log(const LogMessage& msg)
      {
         fstrm << getLogText(msg) << std::endl;        
      }

   protected:
      std::string logFile;
      std::ofstream fstrm;
   };
}   // End of namespace gpstk


#endif  //GPSTK_LOGCHANNEL_HPP

