#pragma ident "$Id$"

/**
 * @file Application.hpp
 * 
 */

#ifndef GPSTK_APPLICATION_HPP
#define GPSTK_APPLICATION_HPP

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
#include <vector>
#include "AppOption.hpp"
#include "Logger.hpp"
#include "DayTime.hpp"
#include "Exception.hpp"

namespace gpstk
{
      /** This class ...
       *
       */
   class Application   
   {
   public:
      //typedef std::vector<std::string> ArgVec;

         /// Commonly used exit status codes.
      enum ExitCode
      {
         EXIT_OK          = 0,  /// successful termination
         EXIT_USAGE	     = 64, /// command line usage error
         EXIT_DATAERR     = 65, /// data format error
         EXIT_NOINPUT     = 66, /// cannot open input
         EXIT_NOUSER      = 67, /// addressee unknown
         EXIT_NOHOST      = 68, /// host name unknown
         EXIT_UNAVAILABLE = 69, /// service unavailable
         EXIT_SOFTWARE    = 70, /// internal software error
         EXIT_OSERR	     = 71, /// system error (e.g., can't fork)
         EXIT_OSFILE      = 72, /// critical OS file missing
         EXIT_CANTCREAT   = 73, /// can't create (user) output file
         EXIT_IOERR       = 74, /// input/output error
         EXIT_TEMPFAIL    = 75, /// temp failure; user is invited to retry
         EXIT_PROTOCOL    = 76, /// remote error in protocol
         EXIT_NOPERM      = 77, /// permission denied
         EXIT_CONFIG      = 78  /// configuration error
      };

   public:

      Application(const std::string& author="GPSTk",
                  const std::string& version="1.0.0",
                  const std::string& desc="",
                  const std::string& useage="[options] ...");     

      Application(int argc, char* argv[]);

      virtual ~Application(){}

      void init(int argc, char* argv[]);

      virtual int run();

      Logger& logger() const;

      LogStream logstream();

      Application& version(const std::string& version="1.0.0")
      { appVersion = version; return (*this); }

      Application& description(const std::string& desc="")
      { appDesc = desc; return (*this); }

      Application& useage(const std::string& usage="[options] ...")
      { appUsage = usage; return (*this); }

      Application& author(const std::string& aut="")
      { appAuthor = aut; return (*this); }

      std::string name() { return appName; }

      std::string version() { return appVersion; }
      
      std::string description() { return appDesc; }
      
      std::string useage() { return appUsage; }
      
      std::string author() { return appAuthor; }

      Application& appInfo(const std::string& author,
                           const std::string& version="1.0.0",
                           const std::string& description="",
                           const std::string& useage="[options] <file> ...");
      
      const OptionSet& options() const;
      
      double toltalMilliseconds();

   protected:

      virtual void setupOptions(OptionSet& options){}

      virtual void spinUp(){}

      virtual void process(const std::vector<std::string>& args){}

      virtual void shutDown(){}

   protected:
      std::string commandName() const;

      void handleOption(const std::string& name, 
                                const std::string& value);

      void defineOptions(OptionSet& options);


      void handleDefaultOptions(const std::string& name, 
                                const std::string& value);

      void stopOptionsProcessing();
      
      void processOptions();

      void initialize(Application& self);

      int main(const std::vector<std::string>& args);

      void uninitialize();

   protected:
      static Application* _pInstance;
      bool _initialized;

      std::string _command;
      std::vector<std::string> _args;

      OptionSet _options;
      bool _stopOptionsProcessing;

      bool helpRequested;
      bool unixStyle;

      std::string appName;
      std::string appVersion;
      std::string appDesc;
      std::string appAuthor;
      std::string appUsage;

      int verboseLevel;

      DayTime runTime;

   };   // End of class 'Application'
   

#define GPSTK_APP_MAIN(App)                  \
   int main(int argc, char* argv[])			   \
   {										            \
      App app;	                              \
      try									         \
      {									            \
         app.init(argc, argv);			      \
      }									            \
      catch (Exception* e)		               \
      {									            \
         GPSTK_ERROR("",e->what());		      \
         return Application::EXIT_CONFIG;    \
      }									            \
      catch (std::exception* e)		         \
      {									            \
         GPSTK_ERROR("",e->what());		      \
         return Application::EXIT_CONFIG;    \
      }									            \
      return app.run();					         \
   }

}   // End of namespace gpstk


#endif  //GPSTK_APPLICATION_HPP

