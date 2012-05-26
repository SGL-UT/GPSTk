#pragma ident "$Id$"

/**
* @file Logger.hpp
* Logging framework
*/

#ifndef GPSTK_LOGGER_HPP
#define GPSTK_LOGGER_HPP

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

#include <iostream>
#include <map>
#include <string>
#include <cstdarg>
#include "CommonTime.hpp"
#include "Exception.hpp"
#include "Matrix.hpp"
#include "LogMessage.hpp"
#include "LogChannel.hpp"
#include "MemoryUtils.hpp"
#include "FormatUtils.hpp"
#include "StreamBuf.hpp"

namespace gpstk
{

   class LogStream;

   class Logger
   {
   public:
      Logger(const Logger& right);

      Logger& operator = (const Logger& right);

      ~Logger(){};

      Logger& setChannelPattern(const std::string fmt="%t")
      { 
         if(this->channel) this->channel->setPattern(fmt); 
         return (*this);
      }

      Logger& setChannel(LogChannel* pChannel=&defaultChannel)
      { this->channel = pChannel; return (*this);}
      
      LogChannel* getChannel() const
      { return channel; }
     
      Logger& setLevel(int level)
      { this->level = level; return (*this);}

      void setLevel(const std::string& level);

      int getLevel() const
      { return this->level; }

      std::string getName()
      {return this->name; }

      void log(const std::string& text, LogLevel level);

      void log(const std::string& text, LogLevel level, ExceptionLocation location);

      void fatal(const std::string& msg)
      { log(msg,LEVEL_FATAL); }

      void critical(const std::string& msg)
      { log(msg,LEVEL_CRITICAL); }

      void error(const std::string& msg)
      { log(msg,LEVEL_ERROR); }
 
      void warning(const std::string& msg)
      { log(msg,LEVEL_WARNING); }

      void notice(const std::string& msg)
      { log(msg,LEVEL_NOTICE);}
 
      void information(const std::string& msg)
      { log(msg,LEVEL_INFORMATION); }

      void debug(const std::string& msg)
      { log(msg,LEVEL_DEBUG); }	

      void trace(const std::string& msg)
      { log(msg,LEVEL_TRACE); };

      bool is(int level) const
      { return level >= level;}
      
      bool fatal() const
      { return level >= LEVEL_FATAL; }
      
      bool critical() const
      { return level >= LEVEL_CRITICAL; }
      
      bool error() const
      { return level >= LEVEL_ERROR; }
      
      bool warning() const
      { return level >= LEVEL_WARNING; }
      
      bool notice() const
      { return level >= LEVEL_NOTICE; }
      
      bool information() const
      { return level >= LEVEL_INFORMATION; }
      
      bool debug() const
      { return level >= LEVEL_DEBUG; }
      
      bool trace() const
      { return level >= LEVEL_TRACE; }

      
      // static method
      //------------------

      static Logger& create( const std::string& logname,
                             LogChannel* logchannel = &defaultChannel,
                             LogLevel loglevel = LEVEL_INFORMATION);
         
      static void destroy(const std::string& name);


      static Logger& get(const std::string& name);
      

      static void shutdown();


      static Logger& nullLogger(const std::string& logname,
                                LogLevel loglevel = LEVEL_INFORMATION, 
                                const std::string& pattern = "%p: %t");


      static Logger& consoleLogger(const std::string& logname,
                           LogLevel loglevel = LEVEL_INFORMATION, 
                           const std::string& pattern = "%p: %t");


      static Logger& fileLogger(const std::string& logname,
                                const std::string& filename,
                                LogLevel loglevel = LEVEL_INFORMATION, 
                                const std::string& pattern = "%p: %t");
     
   protected:

      void log(const LogMessage& msg);
      
      static Logger* find(const std::string& name);
      
      static void add(Logger* pLogger);
   
   protected:
      typedef std::map<std::string, Logger*> LoggerMap;

      Logger(){}

      Logger(const std::string& logname,
             LogLevel    loglevel   = LEVEL_INFORMATION, 
             LogChannel* logchannel = &defaultChannel) 
         : name(logname),level(loglevel), channel(logchannel) {}

      std::string   name;     /// log name
      int           level;    /// log level
      LogChannel*   channel;  /// log channel

      static LoggerMap loggerMap;
      

   public:
      static ConsoleLogChannel defaultChannel;
      static AutoReleasePool<LogChannel> channelPool;

      friend class LogStreamBuf;

   }; // End of class 'Logger'



   template <class T>
   inline std::string mat2str(const Vector<T>& vec, size_t width, size_t digit,
                              std::string desc="")
   {
      std::ostringstream ss;
      ss << std::fixed;
      ss << "["<< vec.size() << "x1]: " << desc << std::endl;
      for(int i=0;i<vec.size();i++)
      {
         ss << " " << std::setw(width) << std::setprecision(digit) << vec[i];
         if((i+1)!=vec.size()) ss << std::endl;
      }      
      return ss.str();
   }

   template <class T>
   inline std::string mat2str(const Matrix<T>& mat, size_t width, size_t digit,
                              std::string desc="")
   {
      std::ostringstream ss;
      ss << std::fixed;
      ss << "["<< mat.rows()<<"x"<<mat.cols() <<"]: "<< desc << std::endl;
      ss << std::setw(width) << std::setprecision(digit) << mat;
      return ss.str();
   }

   //
   // convenience macros
   //
#define GPSTK_LOGGER_STREAM(name) \
   LogStream( Logger::get(name) )

#define GPSTK_NULL_LOGGER(name) \
   Logger::nullLogger(name)

#define GPSTK_CONSOLE_LOGGER(name) \
   Logger::consoleLogger(name)

#define GPSTK_FILE_LOGGER(name,file) \
   Logger::fileLogger(name,file)

#define GPSTK_LOGGER_PATTERN(name,pattern) \
   Logger::get(name).setChannelPattern(pattern)

#define GPSTK_LOGGER_LEVEL(name,level) \
   Logger::get(name).setLevel(name,level)


#define GPSTK_FATAL(name, msg) \
   if (Logger::get(name).fatal()) Logger::get(name).log(msg, LEVEL_FATAL, FILE_LOCATION); else (void) 0

#define GPSTK_FATAL_F1(name, fmt, arg1) \
   if (Logger::get(name).fatal()) Logger::get(name).log(FormatUtils::format((fmt), arg1), LEVEL_FATAL, FILE_LOCATION); else (void) 0

#define GPSTK_FATAL_F2(name, fmt, arg1, arg2) \
   if (Logger::get(name).fatal()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2)), LEVEL_FATAL, FILE_LOCATION); else (void) 0

#define GPSTK_FATAL_F3(name, fmt, arg1, arg2, arg3) \
   if (Logger::get(name).fatal()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2), (arg3)), LEVEL_FATAL, FILE_LOCATION); else (void) 0

#define GPSTK_FATAL_F4(name, fmt, arg1, arg2, arg3, arg4) \
   if (Logger::get(name).fatal()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2), (arg3), (arg4)), LEVEL_FATAL, FILE_LOCATION); else (void) 0

#define GPSTK_FATAL_MAT(name, mat, w, d, desc) \
   if (Logger::get(name).fatal()) Logger::get(name).log(mat2str(mat,w,d,desc), LEVEL_FATAL, FILE_LOCATION); else (void) 0


#define GPSTK_CRITICAL(name, msg) \
   if (Logger::get(name).critical()) Logger::get(name).log(msg, LEVEL_CRITICAL, FILE_LOCATION); else (void) 0

#define GPSTK_CRITICAL_F1(name, fmt, arg1) \
   if (Logger::get(name).critical()) Logger::get(name).log(FormatUtils::format((fmt), (arg1)), LEVEL_CRITICAL, FILE_LOCATION); else (void) 0

#define GPSTK_CRITICAL_F2(name, fmt, arg1, arg2) \
   if (Logger::get(name).critical()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2)), LEVEL_CRITICAL, FILE_LOCATION); else (void) 0

#define GPSTK_CRITICAL_F3(name, fmt, arg1, arg2, arg3) \
   if (Logger::get(name).critical()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2), (arg3)), LEVEL_CRITICAL, FILE_LOCATION); else (void) 0

#define GPSTK_CRITICAL_F4(name, fmt, arg1, arg2, arg3, arg4) \
   if (Logger::get(name).critical()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2), (arg3), (arg4)), LEVEL_CRITICAL, FILE_LOCATION); else (void) 0

#define GPSTK_CRITICAL_MAT(name, mat, w, d, desc) \
   if (Logger::get(name).fatal()) Logger::get(name).log(mat2str(mat,w,d,desc),LEVEL_CRITICAL, FILE_LOCATION); else (void) 0


#define GPSTK_ERROR(name, msg) \
   if (Logger::get(name).error()) Logger::get(name).log(msg, LEVEL_ERROR, FILE_LOCATION); else (void) 0

#define GPSTK_ERROR_F1(name, fmt, arg1) \
   if (Logger::get(name).error()) Logger::get(name).log(FormatUtils::format((fmt), (arg1)), LEVEL_ERROR, FILE_LOCATION); else (void) 0

#define GPSTK_ERROR_F2(name, fmt, arg1, arg2) \
   if (Logger::get(name).error()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2)), LEVEL_ERROR, FILE_LOCATION); else (void) 0

#define GPSTK_ERROR_F3(name, fmt, arg1, arg2, arg3) \
   if (Logger::get(name).error()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2), (arg3)), LEVEL_ERROR, FILE_LOCATION); else (void) 0

#define GPSTK_ERROR_F4(name, fmt, arg1, arg2, arg3, arg4) \
   if (Logger::get(name).error()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2), (arg3), (arg4)), LEVEL_ERROR, FILE_LOCATION); else (void) 0

#define GPSTK_ERROR_MAT(name, mat, w, d, desc) \
   if (Logger::get(name).fatal()) Logger::get(name).log(mat2str(mat,w,d,desc), LEVEL_ERROR, FILE_LOCATION); else (void) 0


#define GPSTK_WARNING(name, msg) \
   if (Logger::get(name).warning()) Logger::get(name).log(msg, LEVEL_WARNING, FILE_LOCATION); else (void) 0

#define GPSTK_WARNING_F1(name, fmt, arg1) \
   if (Logger::get(name).warning()) Logger::get(name).log(FormatUtils::format((fmt), (arg1)), LEVEL_WARNING, FILE_LOCATION); else (void) 0

#define GPSTK_WARNING_F2(name, fmt, arg1, arg2) \
   if (Logger::get(name).warning()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2)), LEVEL_WARNING, FILE_LOCATION); else (void) 0

#define GPSTK_WARNING_F3(name, fmt, arg1, arg2, arg3) \
   if (Logger::get(name).warning()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2), (arg3)), LEVEL_WARNING, FILE_LOCATION); else (void) 0

#define GPSTK_WARNING_F4(name, fmt, arg1, arg2, arg3, arg4) \
   if (Logger::get(name).warning()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2), (arg3), (arg4)), LEVEL_WARNING, FILE_LOCATION); else (void) 0

#define GPSTK_WARNING_MAT(name, mat, w, d, desc) \
   if (Logger::get(name).fatal()) Logger::get(name).log(mat2str(mat,w,d,desc), LEVEL_WARNING, FILE_LOCATION); else (void) 0
   

#define GPSTK_NOTICE(name, msg) \
   if (Logger::get(name).notice()) Logger::get(name).log(msg, LEVEL_NOTICE, FILE_LOCATION); else (void) 0

#define GPSTK_NOTICE_F1(name, fmt, arg1) \
   if (Logger::get(name).notice()) Logger::get(name).log(FormatUtils::format((fmt), (arg1)), LEVEL_NOTICE, FILE_LOCATION); else (void) 0

#define GPSTK_NOTICE_F2(name, fmt, arg1, arg2) \
   if (Logger::get(name).notice()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2)), LEVEL_NOTICE, FILE_LOCATION); else (void) 0

#define GPSTK_NOTICE_F3(name, fmt, arg1, arg2, arg3) \
   if (Logger::get(name).notice()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2), (arg3)), LEVEL_NOTICE, FILE_LOCATION); else (void) 0

#define GPSTK_NOTICE_F4(name, fmt, arg1, arg2, arg3, arg4) \
   if (Logger::get(name).notice()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2), (arg3), (arg4)), LEVEL_NOTICE, FILE_LOCATION); else (void) 0

#define GPSTK_NOTICE_MAT(name, mat, w, d, desc) \
   if (Logger::get(name).fatal()) Logger::get(name).log(mat2str(mat,w,d,desc), LEVEL_NOTICE, FILE_LOCATION); else (void) 0


#define GPSTK_INFORMATION(name, msg) \
   if (Logger::get(name).information()) Logger::get(name).log(msg, LEVEL_INFORMATION, FILE_LOCATION); else (void) 0

#define GPSTK_INFORMATION_F1(name, fmt, arg1) \
   if (Logger::get(name).information()) Logger::get(name).log(FormatUtils::format((fmt), (arg1)), LEVEL_INFORMATION, FILE_LOCATION); else (void) 0

#define GPSTK_INFORMATION_F2(name, fmt, arg1, arg2) \
   if (Logger::get(name).information()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2)), LEVEL_INFORMATION, FILE_LOCATION); else (void) 0

#define GPSTK_INFORMATION_F3(name, fmt, arg1, arg2, arg3) \
   if (Logger::get(name).information()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2), (arg3)), LEVEL_INFORMATION, FILE_LOCATION); else (void) 0

#define GPSTK_INFORMATION_F4(name, fmt, arg1, arg2, arg3, arg4) \
   if (Logger::get(name).information()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2), (arg3), (arg4)), LEVEL_INFORMATION, FILE_LOCATION); else (void) 0

#define GPSTK_INFORMATION_MAT(name, mat, w, d, desc) \
   if (Logger::get(name).fatal()) Logger::get(name).log(mat2str(mat,w,d,desc), LEVEL_INFORMATION, FILE_LOCATION); else (void) 0

#if defined(_DEBUG)

#define GPSTK_DEBUG(name, msg) \
   if (Logger::get(name).debug()) Logger::get(name).log(msg, LEVEL_DEBUG, FILE_LOCATION); else (void) 0

#define GPSTK_DEBUG_F1(name, fmt, arg1) \
   if (Logger::get(name).debug()) Logger::get(name).log(FormatUtils::format((fmt), (arg1)), LEVEL_DEBUG, FILE_LOCATION); else (void) 0

#define GPSTK_DEBUG_F2(name, fmt, arg1, arg2) \
   if (Logger::get(name).debug()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2)), LEVEL_DEBUG, FILE_LOCATION); else (void) 0

#define GPSTK_DEBUG_F3(name, fmt, arg1, arg2, arg3) \
   if (Logger::get(name).debug()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2), (arg3)), LEVEL_DEBUG, FILE_LOCATION); else (void) 0

#define GPSTK_DEBUG_F4(name, fmt, arg1, arg2, arg3, arg4) \
   if (Logger::get(name).debug()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2), (arg3), (arg4)), LEVEL_DEBUG, FILE_LOCATION); else (void) 0

#define GPSTK_DEBUG_MAT(name, mat, w, d, desc) \
   if (Logger::get(name).fatal()) Logger::get(name).log(mat2str(mat,w,d,desc), LEVEL_DEBUG, FILE_LOCATION); else (void) 0


#define GPSTK_TRACE(name, msg) \
   if (Logger::get(name).trace()) Logger::get(name).log(msg, LEVEL_TRACE, FILE_LOCATION); else (void) 0

#define GPSTK_TRACE_F1(name, fmt, arg1) \
   if (Logger::get(name).trace()) Logger::get(name).log(FormatUtils::format((fmt), (arg1)), LEVEL_TRACE, FILE_LOCATION); else (void) 0

#define GPSTK_TRACE_F2(name, fmt, arg1, arg2) \
   if (Logger::get(name).trace()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2)), LEVEL_TRACE, FILE_LOCATION); else (void) 0

#define GPSTK_TRACE_F3(name, fmt, arg1, arg2, arg3) \
   if (Logger::get(name).trace()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2), (arg3)), LEVEL_TRACE, FILE_LOCATION); else (void) 0

#define GPSTK_TRACE_F4(name, fmt, arg1, arg2, arg3, arg4) \
   if (Logger::get(name).trace()) Logger::get(name).log(FormatUtils::format((fmt), (arg1), (arg2), (arg3), (arg4)), LEVEL_TRACE, FILE_LOCATION); else (void) 0

#define GPSTK_TRACE_MAT(name, mat, w, d, desc) \
   if (Logger::get(name).fatal()) Logger::get(name).log(mat2str(mat,w,d,desc), LEVEL_TRACE, FILE_LOCATION); else (void) 0

#else // #if defined(_DEBUG)

#define GPSTK_DEBUG(name, msg)
#define GPSTK_DEBUG_F1(name, fmt, arg1)
#define GPSTK_DEBUG_F2(name, fmt, arg1, arg2)
#define GPSTK_DEBUG_F3(name, fmt, arg1, arg2, arg3)
#define GPSTK_DEBUG_F4(name, fmt, arg1, arg2, arg3, arg4)
#define GPSTK_DEBUG_MAT(name, mat, w, d, desc)

#define GPSTK_TRACE(name, msg)
#define GPSTK_TRACE_F1(name, fmt, arg1)
#define GPSTK_TRACE_F2(name, fmt, arg1, arg2)
#define GPSTK_TRACE_F3(name, fmt, arg1, arg2, arg3)
#define GPSTK_TRACE_F4(name, fmt, arg1, arg2, arg3, arg4)
#define GPSTK_TRACE_MAT(name, mat, w, d, desc)

#endif   // #if defined(_DEBUG)


//////////////////////////////////////////////////////////////////////////

   /// This class implements a streambuf interface to a Logger.
class LogStreamBuf: public StreamBuf   
{
public:
   LogStreamBuf(Logger& logger, LogLevel level)
      :_logger(logger),_level(level),_message("") {}
 
   ~LogStreamBuf(){}

   void setLevel(LogLevel level) { _level = level; }

   LogLevel getLevel() const { return _level; }

   Logger& logger() const { return _logger; }

   void setLogger(Logger& logger){_logger=logger;}

private:
   int writeToDevice(char c)
   {
      if (c == '\n' || c == '\r')
      {
         LogMessage msg(_logger.name, _message, _level);
         _logger.log(msg);

         _message.clear();
      }
      else _message += c;
      return c;
   }

private:
   Logger&           _logger;
   LogLevel          _level;
   std::string       _message;
};


class LogStream : public std::ostream
{
public:
   LogStream(Logger& logger, LogLevel level = LEVEL_INFORMATION)
      : _buf(logger,level), std::ostream(&_buf) {}

   LogStream(const std::string& loggerName, LogLevel level = LEVEL_INFORMATION)
      : _buf(Logger::get(loggerName),level), std::ostream(&_buf) {}

   LogStream(const LogStream& right)
      : _buf(right._buf.logger(),right._buf.getLevel()), std::ostream(&_buf) {}

   ~LogStream(){}

   LogStream& operator=(const LogStream& right)
   {
      _buf.setLogger(right._buf.logger());
      _buf.setLevel(right._buf.getLevel());
   }

   LogStream& fatal(){ return setLevel(LEVEL_FATAL);}

   LogStream& fatal(const std::string& message)
   { _buf.logger().log(message,LEVEL_FATAL); }

   LogStream& critical(){ return setLevel(LEVEL_CRITICAL);}

   LogStream& critical(const std::string& message)
   { _buf.logger().log(message,LEVEL_CRITICAL); }

   LogStream& error(){ return setLevel(LEVEL_ERROR);}

   LogStream& error(const std::string& message)
   { _buf.logger().log(message,LEVEL_ERROR); }

   LogStream& warning(){ return setLevel(LEVEL_WARNING);}

   LogStream& warning(const std::string& message)
   { _buf.logger().log(message,LEVEL_WARNING); }

   LogStream& notice(){ return setLevel(LEVEL_NOTICE);}

   LogStream& notice(const std::string& message)
   { _buf.logger().log(message,LEVEL_NOTICE); }

   LogStream& information(){ return setLevel(LEVEL_INFORMATION);}

   LogStream& information(const std::string& message)
   { _buf.logger().log(message,LEVEL_INFORMATION); }

   LogStream& debug(){ return setLevel(LEVEL_DEBUG);}

   LogStream& debug(const std::string& message)
   { _buf.logger().log(message,LEVEL_DEBUG); }

   LogStream& trace(){ return setLevel(LEVEL_TRACE);}

   LogStream& trace(const std::string& message)
   { _buf.logger().log(message,LEVEL_TRACE); }

   LogStream& setLevel(LogLevel level)
   { _buf.setLevel(level); return (*this); }


protected:
   LogStreamBuf _buf;

}; // End of method 'LogStream'


}  // end of namespace 'gpstk'

#endif   // GPSTK_LOGGER_HPP



