#pragma ident "$Id$"

/**
 * @file LogMessage.hpp
 * 
 */

#ifndef GPSTK_LOGMESSAGE_HPP
#define GPSTK_LOGMESSAGE_HPP

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
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================

#include <string>
#include "CommonTime.hpp"
#include "SystemTime.hpp"
#include "DebugUtils.hpp"

namespace gpstk
{

   enum LogLevel
   {
      LEVEL_FATAL = 1,   /// A fatal error. The application will most likely terminate. This is the highest priority.
      LEVEL_CRITICAL,    /// A critical error. The application might not be able to continue running successfully.
      LEVEL_ERROR,       /// An error. An operation did not complete successfully, but the application as a whole is not affected.
      LEVEL_WARNING,     /// A warning. An operation completed with an unexpected result.
      LEVEL_NOTICE,      /// A notice, which is an information with just a higher priority.
      LEVEL_INFORMATION, /// An informational message, usually denoting the successful completion of an operation.
      LEVEL_DEBUG,       /// A debugging message.
      LEVEL_TRACE,       /// A tracing message. This is the lowest priority.
      MAX_LEVEL
   };

   static std::string LogLevelName(int level)
   {
      static const std::string names[] = 
      {
         "",
         "Fatal",
         "Critical",
         "Error",
         "Warning",
         "Notice",
         "Information",
         "Debug",
         "Trace"
      };

      GPSTK_ASSERT( (level>=1) && (level<=8) );
      
      return names[level];
   }
      
   class LogMessage   
   {
   public:
      
      LogMessage()
         : source(""),
           text(""),
           level(LEVEL_INFORMATION),
           file(""),
           function(""),
           line(0) {}

      LogMessage( std::string source,
                  std::string text,
                  LogLevel level,
                  CommonTime time = SystemTime(),
                  std::string file = "",
                  std::string function = "",
                  int line = 0)
      {
         this->source = source;
         this->text = text;
         this->level = level;
         this->time = time;
         this->file = file;
         this->function = function;
         this->line = line;
      }
      
      
         /// Destractor
      virtual ~LogMessage(){}
      
   protected:
      std::string source;
      std::string text;
      LogLevel    level;
      CommonTime  time;
      std::string file;
      std::string function;
      int         line;
         
      friend class Logger;
      friend class LogChannel;

   };   // End of class 'LogMessage'
   
}   // End of namespace gpstk


#endif  //GPSTK_LOGMESSAGE_HPP

