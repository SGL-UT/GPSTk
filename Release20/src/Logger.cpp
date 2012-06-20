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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Wei Yan - Chinese Academy of Sciences . 2009~2015
//
//============================================================================

#include "Logger.hpp"
#include "StringUtils.hpp"
#include "SystemTime.hpp"
#include "TimeString.hpp"
#include "MemoryUtils.hpp"
#include "String.hpp"

namespace gpstk
{
   using namespace std;

   std::map<std::string, Logger*> Logger::loggerMap;
   AutoReleasePool<LogChannel> Logger::channelPool;
   ConsoleLogChannel Logger::defaultChannel;

   Logger::Logger(const Logger& right)
   {
      this->name = right.name;
      this->level = right.level;
      this->channel = right.channel;
   }

   Logger& Logger::operator = (const Logger& right)
   {
      this->name = right.name;
      this->level = right.level;
      this->channel = right.channel;

      return *this;
   }

   void Logger::log(const LogMessage& msg)
   {
      if( (msg.level <= level) && (channel) )
      {
         channel->log(msg);
      }
   }

   void Logger::log(const std::string& text, LogLevel level)
   {
      LogMessage msg(name, text, level);
      log(msg);
   }

   void Logger::log(const std::string& text, LogLevel level, ExceptionLocation location)
   {
      LogMessage msg(name, text, level, SystemTime(),
                     location.getFileName(),
                     location.getFunctionName(),
                     location.getLineNumber());
      log(msg);
   }

   void Logger::setLevel(const std::string& level)
   {
      string temp = toLower(level);

      if (temp == "none")
         setLevel(0);
      else if (temp == "fatal")
         setLevel(LEVEL_FATAL);
      else if (temp == "critical")
         setLevel(LEVEL_CRITICAL);
      else if (temp == "error")
         setLevel(LEVEL_ERROR);
      else if (temp == "warning")
         setLevel(LEVEL_WARNING);
      else if (temp == "notice")
         setLevel(LEVEL_NOTICE);
      else if (temp == "information")
         setLevel(LEVEL_INFORMATION);
      else if (temp == "debug")
         setLevel(LEVEL_DEBUG);
      else if (temp == "trace")
         setLevel(LEVEL_TRACE);
      else
         throw Exception("Not a valid log level"+level);
   }
   
   //
   // static method
   //

   Logger& Logger::nullLogger(const std::string& logname,
                              LogLevel loglevel, 
                              const std::string& pattern)
   {
      LogChannel* pChannel = new LogChannel(pattern);
      channelPool.add(pChannel);

      Logger* pLogger = find(logname);
      if(pLogger)
      {
         return pLogger->setChannel(pChannel);
      }
      else
      {
         return create(logname,pChannel,loglevel);
      }

   }


   Logger& Logger::consoleLogger(const std::string& logname,
                                 LogLevel loglevel, 
                                 const std::string& pattern)
   {
      LogChannel* pChannel = new ConsoleLogChannel(pattern);
      channelPool.add(pChannel);

      Logger* pLogger = find(logname);
      if(pLogger)
      {
         return pLogger->setChannel(pChannel);
      }
      else
      {
         return create(logname,pChannel,loglevel);
      }
   }

   Logger& Logger::fileLogger(const std::string& logname,
                              const std::string& filename,
                              LogLevel loglevel, 
                              const std::string& pattern)
   {
      LogChannel* pChannel = new FileLogChannel(filename,pattern);
      channelPool.add(pChannel);

      Logger* pLogger = find(logname);
      if(pLogger)
      {
         return pLogger->setChannel(pChannel);
      }
      else
      {
         return create(logname,pChannel,loglevel);
      }
   }

   Logger& Logger::create( const std::string& logname,
                           LogChannel* logchannel,
                           LogLevel loglevel )
   {
      Logger* pLogger = find(logname);
      if (!pLogger)
      {
         pLogger = new Logger(logname, loglevel, logchannel);
         add(pLogger);
      }

      return (*pLogger);
   }

   Logger* Logger::find(const std::string& name)
   {
      
      LoggerMap::iterator it = loggerMap.find(name);
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
      LoggerMap::iterator it = loggerMap.find(name);
      if (it != loggerMap.end())
      {
         delete it->second;
         it->second = 0;
         loggerMap.erase(it);
      }
   }

   void Logger::shutdown()
   {
      LoggerMap::iterator it;
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
         if (name == "")
         {
            pLogger = new Logger(name, LEVEL_INFORMATION, &defaultChannel);
         }
         
         add(pLogger);
      }
      return (*pLogger);
   }


   //////////////////////////////////////////////////////////////////////////

   
   class AutoLoggerShutdown
   {
   public:
      AutoLoggerShutdown()  
      {
      }
      
      ~AutoLoggerShutdown() 
      { 
         Logger::shutdown(); 
         Logger::channelPool.release();
      }

   protected:
      
   };

   
   static AutoLoggerShutdown als;

}  // End of namespace 'gpstk'


