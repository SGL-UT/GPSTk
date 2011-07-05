#pragma ident "$Id$"

/**
* @file Logger.cpp
* Logging framework
*/

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
//  Wei Yan - Chinese Academy of Sciences . 2009~2015
//
//============================================================================

#include "Logger.hpp"
#include "StringUtils.hpp"
#include "Epoch.hpp"
#include "TimeString.hpp"

namespace gpstk
{
   using namespace std;

   std::map<std::string, Logger*> Logger::loggerMap;

   const std::string Logger::DEFAULT = "";

   const std::string Logger::LogLevelName[MAX_LEVEL]=
   {
      "","Fatal","Critical","Error","Warning","Notice","Information",
      "Debug","Trace"
   };

   Logger::Logger(const Logger& right)
   {
      this->name = right.name;
      this->level = right.level;
      this->pstrm = right.pstrm;
   }

   Logger& Logger::operator = (const Logger& right)
   {
      this->name = right.name;
      this->level = right.level;
      this->pstrm = right.pstrm;

      return *this;
   }

   void Logger::log(const LogMessage& msg)
   {
      if( (msg.level <= level) && (pstrm->good()) )
      {
         string slevel = LogLevelName[msg.level];

         ostringstream ss;
         ss << msg.file <<":"
            << msg.line <<":"
            << msg.function;
         string slocation = ss.str();

         if(printInDetail)
         {
            (*pstrm) << "@ [" << StringUtils::lowerCase(slevel) << "] "
               << printTime(msg.time,"%04Y/%02m/%02d %02H:%02M:%06.3f") << " " 
               << slocation <<"\n  "
               << msg.text << endl;
         }
         else
         {
            (*pstrm) << "[" << StringUtils::lowerCase(slevel) << "] "
                     << msg.text << endl;
         }
         
                
         (*pstrm).flush();
      }
   }

   void Logger::log(const std::string& text, LogLevel level, ExceptionLocation location)
   {
      CommonTime now; static_cast<Epoch>(now).setLocalTime();
      LogMessage msg(text,level,now,
         location.getFileName(),
         location.getFunctionName(),
         location.getLineNumber());
      log(msg);
   }
   
   //
   // static method
   //

   Logger& Logger::create(std::string logname,
      LogLevel loglevel, 
      std::ostream* logstrm )
   {
      Logger* pLogger = find(logname);
      if (!pLogger)
      {
         pLogger = new Logger(logname, loglevel, logstrm);
         add(pLogger);
      }

      return (*pLogger);
   }

   Logger* Logger::find(const std::string& name)
   {
      
      map<string, Logger*>::iterator it = loggerMap.find(name);
      if (it != loggerMap.end()) return it->second;
      else return 0;
      
   }

   void Logger::add(Logger* pLogger)
   {
      loggerMap.insert(
         map<string, Logger*>::value_type(pLogger->getName(), pLogger) );
   }

   void Logger::destroy(const std::string& name)
   {
      map<string, Logger*>::iterator it = loggerMap.find(name);
      if (it != loggerMap.end())
      {
         delete it->second;
         it->second = 0;
         loggerMap.erase(it);
      }
   }

   void Logger::shutdown()
   {
      map<string, Logger*>::iterator it;
      for (it = loggerMap.begin(); it != loggerMap.end(); ++it)
      {
         delete it->second;
         it->second = 0;
      }
      loggerMap.clear();
   }

   Logger& Logger::get(const std::string& name)
   {
      Logger* pLogger = find(name);
      if (!pLogger)
      {
         if (name == DEFAULT)
         {
            pLogger = new Logger(name,Logger::ERROR,&std::clog);
         }
         
         add(pLogger);
      }
      return (*pLogger);
   }


   //////////////////////////////////////////////////////////////////////////

   
   class AutoLoggerShutdown
   {
   public:
      AutoLoggerShutdown()  {/*Logger::create("",Logger::TRACE);*/}
      ~AutoLoggerShutdown() { Logger::shutdown(); }
   };

   
   static AutoLoggerShutdown als;
   
   Logger& LoggerStream::clog = Logger::get("");
   Logger& LoggerStream::log  = clog;

   Logger& LoggerStream::fatal = Logger::create("fatal",Logger::FATAL,&std::clog);
   Logger& LoggerStream::critical = Logger::create("critical",Logger::CRITICAL,&std::clog);
   Logger& LoggerStream::error = Logger::create("error",Logger::ERROR,&std::cerr);
   Logger& LoggerStream::warning = Logger::create("warning",Logger::WARNING,&std::clog);
   Logger& LoggerStream::notice = Logger::create("notice",Logger::NOTICE,&std::clog);
   Logger& LoggerStream::information = Logger::create("information",Logger::INFORMATION,&std::clog);

   Logger& LoggerStream::debug = Logger::create("debug",Logger::DEBUG,&std::clog);
   Logger& LoggerStream::trace = Logger::create("trace",Logger::TRACE,&std::clog);

}  // End of namespace 'gpstk'


