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

namespace gpstk
{
   using namespace std;

   std::map<std::string, Logger*> Logger::loggerMap;

   const std::string Logger::DEFAULT = "";

   const std::string Logger::LogLevelName[MAX_LEVEL]=
   {
      "","fatal","critical","error","warning","notice","information",
      "debug","trace"
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
         ostringstream ss;
         
         ss << "[ " << msg.level << " "<<LogLevelName[msg.level] << " ]";
         string slevel = ss.str();

         ss.str("");
         ss << msg.file <<":"
            << msg.line <<":"
            << msg.function;
         string slocation = ss.str();

         (*pstrm) << msg.time << " " 
                  << slevel <<" "
                  << slocation <<" \n  "
                  << msg.text << endl;
                
         (*pstrm).flush();
      }
   }

   void Logger::log(const std::string& text, LogLevel level, ExceptionLocation location)
   {
      LogMessage msg(text,level,DayTime(),
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
            pLogger = new Logger(name,Logger::INFORMATION);
         }
         
         add(pLogger);
      }
      return (*pLogger);
   }


   //////////////////////////////////////////////////////////////////////////

   class AutoLoggerShutdown
   {
   public:
      AutoLoggerShutdown() {Logger::create("");}
      ~AutoLoggerShutdown() { Logger::shutdown(); }
   };

   
   static AutoLoggerShutdown als;

}  // End of namespace 'gpstk'


