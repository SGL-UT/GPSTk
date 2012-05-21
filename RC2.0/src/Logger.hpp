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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Wei Yan - Chinese Academy of Sciences . 2009~2015
//
//============================================================================

#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <cstdarg>
#include "CommonTime.hpp"
#include "Exception.hpp"
#include "Matrix.hpp"

namespace gpstk
{
      /**
       * This class encapsulates the logging framework.
       * @code
       *    ofstream logstrm("debug.txt");
       *    string logname("debug");
       *
       *    slog(logname).setLevel(Logger::TRACE);
       *    slog(logname).setStream(&logstrm);
       *
       *    GPSTK_ERROR(slog("debug"),"error message");
       *    GPSTK_FATAL(slog("debug"),"crushed");
       *    GPSTK_DEBUG(slog("debug"),"debug message");
       *    GPSTK_DEBUG(slog("debug"),"trace message");
       * @endcode
       */
   class Logger
   {
   public:
         /// The type of log level
      enum LogLevel
      {
         FATAL = 1,   /// A fatal error. The application will most likely terminate. This is the highest priority.
         CRITICAL,    /// A critical error. The application might not be able to continue running successfully.
         ERROR,       /// An error. An operation did not complete successfully, but the application as a whole is not affected.
         WARNING,     /// A warning. An operation completed with an unexpected result.
         NOTICE,      /// A notice, which is an information with just a higher priority.
         INFORMATION, /// An informational message, usually denoting the successful completion of an operation.
         DEBUG,       /// A debugging message.
         TRACE,       /// A tracing message. This is the lowest priority.
         MAX_LEVEL
      };
         
         /// The struct of log message
      struct LogMessage
      {
         std::string text;
         LogLevel    level;
         CommonTime     time;
         std::string file;
         std::string function;
         int         line;
         

         LogMessage()
            :text(""),level(INFORMATION),file(""),function(""),line(0)
         {}

         LogMessage(std::string text,
            LogLevel level,
            CommonTime time = CommonTime(),
            std::string file = "",
            std::string function = "",
            int line = 0)
         {
            this->text = text;
            this->level = level;
            this->time = time;
            this->file = file;
            this->function = function;
            this->line = line;
         }
      };

   public:
         /// Copy constructor
      Logger(const Logger& right);

      Logger& operator = (const Logger& right);

         /// Default deconstructor
      ~Logger(){};

         /// Set log stream
      Logger& setStream(std::ostream* pos)
      { pstrm = pos; return (*this);}
         
         /// Return log stream
      std::ostream* getStream() const
      { return pstrm;}

         /// Sets the Logger's log level.
      Logger& setLevel(int level)
      { this->level = level; return (*this);}

         /// Returns the Logger's log level.
      int getLevel() const
      { return this->level; }

         /// Get logger's name
      std::string getName()
      {return this->name; }

      Logger& setPrintInDetail(bool inDetail = true)
      { this->printInDetail = inDetail; return (*this);}
      
         /// Write log message to logging stream
      void log(const std::string& text, LogLevel level, ExceptionLocation location);

         /// Returns true if at least the given log level is set.
      bool is(int level) const
      { return level >= level;}
      
         /// Returns true if the log level is at least PRIO_FATAL.
      bool fatal() const
      { return level >= FATAL; }
      
         /// Returns true if the log level is at least PRIO_CRITICAL.
      bool critical() const
      { return level >= CRITICAL; }
      
         /// Returns true if the log level is at least PRIO_ERROR.
      bool error() const
      { return level >= ERROR; }
      
         /// Returns true if the log level is at least PRIO_WARNING.
      bool warning() const
      { return level >= WARNING; }
      
         /// Returns true if the log level is at least PRIO_NOTICE.
      bool notice() const
      { return level >= NOTICE; }
      
         /// Returns true if the log level is at least PRIO_INFORMATION.
      bool information() const
      { return level >= INFORMATION; }
      
         /// Returns true if the log level is at least PRIO_DEBUG.
      bool debug() const
      { return level >= DEBUG; }
      
         /// Returns true if the log level is at least PRIO_TRACE.
      bool trace() const
      { return level >= TRACE; }

      
      // static method
      //------------------

         /// Create a logger object and stored in map indexed by name
      static Logger& create(std::string logname,
         LogLevel loglevel=INFORMATION, 
         std::ostream* logstrm = &std::clog);
         
         /// Destroy a logger object by name
      static void destroy(const std::string& name);

         /// Get a logger object by name
      static Logger& get(const std::string& name);
      
         /// Delete all logger objects
      static void shutdown();

     
   protected:
      void log(const LogMessage& msg);
      static Logger* find(const std::string& name);
      static void add(Logger* pLogger);

      bool printInDetail;
         
   protected:
         /// Default constructor
      Logger(){}

      Logger(std::string logname,
         LogLevel loglevel=INFORMATION, 
         std::ostream* logstrm = &std::clog) 
         : name(logname),level(loglevel), pstrm(logstrm),
           printInDetail(false)
      {};

      std::string   name;     /// log name
      int           level;    /// log level
      std::ostream* pstrm;    /// log stream

         /// Object to store all loggers indexed by name
      static std::map<std::string, Logger*> loggerMap;
      
         /// The name of the default logger ("").
      static const std::string DEFAULT; 

      static const std::string LogLevelName[MAX_LEVEL];

   }; // End of class 'Logger'

   template <class T>
   inline std::string mat2str(const Vector<T>& vec, size_t width, size_t digit,
      std::string desc="")
   {
      //std::ostringstream ss;
      std::stringstream ss;
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
      //std::ostringstream ss;
      std::stringstream ss;
      ss << (std::fixed);
      ss << "["<< mat.rows()<<"x"<<mat.cols() <<"]: "<< desc << std::endl;
      ss << std::setw(width) << std::setprecision(digit) << mat;
      return ss.str();
   }
 
   //
   // convenience macros
   //
#define GPSTK_MAX_BUFFER_SIZE 1024*100

#define GPSTK_LOGGING(logger,level,msg) \
   logger.log(msg,level,FILE_LOCATION); 

#define GPSTK_FATAL( msg ) \
   slog.fatal.log(msg,Logger::FATAL,FILE_LOCATION);

#define GPSTK_CRITICAL( msg ) \
   slog.critical.log(msg,Logger::CRITICAL,FILE_LOCATION);

#define GPSTK_ERROR( msg ) \
   slog.error.log(msg,Logger::ERROR,FILE_LOCATION);

#define GPSTK_WARNING( msg ) \
   slog.warning.log(msg,Logger::WARNING,FILE_LOCATION);

#define GPSTK_NOTICE( msg ) \
   slog.notice.log(msg,Logger::NOTICE,FILE_LOCATION);

#define GPSTK_INFORMATION( msg ) \
   slog.information.log(msg,Logger::INFORMATION,FILE_LOCATION);

   //
#define GPSTK_FATAL2(format, ...) \
   {char ss[GPSTK_MAX_BUFFER_SIZE]={0}; \
   sprintf(ss,format,__VA_ARGS__); \
   GPSTK_FATAL(ss);}

#define GPSTK_CRITICAL2(format, ...) \
   {char ss[GPSTK_MAX_BUFFER_SIZE]={0}; \
   sprintf(ss,format,__VA_ARGS__); \
   GPSTK_CRITICAL(ss);}

#define GPSTK_ERROR2(format, ...) \
   {char ss[GPSTK_MAX_BUFFER_SIZE]={0}; \
   sprintf(ss,format,__VA_ARGS__); \
   GPSTK_ERROR(ss);}
   
#define GPSTK_WARNING2(format, ...) \
   {char ss[GPSTK_MAX_BUFFER_SIZE]={0}; \
   sprintf(ss,format,__VA_ARGS__); \
   GPSTK_WARNING(ss);}

#define GPSTK_NOTICE2(format, ...) \
   {char ss[GPSTK_MAX_BUFFER_SIZE]={0}; \
   sprintf(ss,format,__VA_ARGS__); \
   GPSTK_NOTICE(ss);}

#define GPSTK_INFORMATION2(format, ...) \
   {char ss[GPSTK_MAX_BUFFER_SIZE]={0}; \
   sprintf(ss,format,__VA_ARGS__); \
   GPSTK_INFORMATION(ss);}

#if defined(_DEBUG)

#define GPSTK_DEBUG( msg ) \
   slog.debug.log(msg,Logger::DEBUG,FILE_LOCATION);

#define GPSTK_DEBUG2(format, ...) \
   {char ss[GPSTK_MAX_BUFFER_SIZE]={0}; \
   sprintf(ss,format,__VA_ARGS__); \
   GPSTK_DEBUG(ss);}

#define GPSTK_DEBUG_MAT( mat, w, d, desc) \
   {std::string ss = mat2str(mat,w,d,desc); GPSTK_DEBUG(ss);}

#define GPSTK_TRACE( msg ) \
   slog.trace.log(msg,Logger::TRACE,FILE_LOCATION);

#define GPSTK_TRACE2(format, ...) \
   {char ss[GPSTK_MAX_BUFFER_SIZE]={0}; \
   sprintf(ss,format,__VA_ARGS__); \
   GPSTK_TRACE(ss);}

#define GPSTK_TRACE_MAT( mat, w, d, desc) \
   {std::string ss = mat2str(mat,w,d,desc); GPSTK_TRACE(ss);}

#else

#define GPSTK_DEBUG(logger, msg)
#define GPSTK_DEBUG2(format, ...)
#define GPSTK_DEBUG_MAT( mat, w, d, desc)

#define GPSTK_TRACE(logger, msg)
#define GPSTK_TRACE2(format, ...)
#define GPSTK_TRACE_MAT( mat, w, d, desc)

#endif
      /**
       * This class easy the logging framework calling.
       */
   class LoggerStream 
   {
   public:
      Logger& operator() (std::string logname)
      { return Logger::get(logname); }

      Logger& operator[] (std::string logname)
      { return Logger::get(logname); }

      Logger& create(std::string logname,
         Logger::LogLevel loglevel = Logger::INFORMATION, 
         std::ostream* logstrm = &std::clog)
      { return Logger::create(logname,loglevel,logstrm); }

      void destroy(const std::string& logname)
      { Logger::destroy(logname); }

      // Objects to easy access
      static Logger& clog;           // std::clog
      static Logger& log;

      static Logger& fatal;
      static Logger& critical;
      static Logger& error;
      static Logger& warning;
      static Logger& notice;
      static Logger& information;
      static Logger& debug;
      static Logger& trace;

   }; // End of class 'LoggerStream'
   
      /// Entry point of the logging framework
   static LoggerStream slog;

}  // end of namespace 'gpstk'

#endif   // GPSTK_LOGGER_HPP



