#pragma ident "$Id$"

/**
 * @file Application.cpp
 * 
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
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================

#include "Application.hpp"
#include "SysInfo.hpp"

namespace gpstk
{
   Application* Application::_pInstance = 0;

   Application::Application(const std::string& author,
                            const std::string& version,
                            const std::string& desc,
                            const std::string& useage)
      : helpRequested(false),unixStyle(true),
        _stopOptionsProcessing(false),
        _initialized(false)
   {
      appInfo(author,version,desc,useage);

      _pInstance = this;
   }

   Application::Application(int argc, char* argv[])
      : helpRequested(false),unixStyle(true),
        _stopOptionsProcessing(false),
        _initialized(false)
   {
      _pInstance = this;

      init(argc,argv);
   }
   

   void Application::init(int argc, char* argv[])
   {
      _command = argv[0];
      _args.reserve(argc);
      for(int i = 0; i < argc; i++)
      {
         _args.push_back( std::string(argv[i]) );
      }
      appName = commandName();
      processOptions();
   }

   int Application::run()
   {
      int rc = EXIT_SOFTWARE;
      initialize(*this);
      try
      {
         rc = main(_args);
      }
      catch (Exception& e)
      {
         logger().error(e.what());
      }
      catch (std::exception& e)
      {
         logger().error(e.what());
      }
      catch (...)
      {
         logger().fatal("Unknown system exception");
      }
      
      uninitialize();

      return rc;
   }

   void Application::processOptions()
   {
      defineOptions(_options);

      OptionProcessor processor(_options);
      processor.setUnixStyle(unixStyle);
      
      _args.erase(_args.begin());
      
      std::vector<std::string>::iterator it = _args.begin();
      while (it != _args.end() && !_stopOptionsProcessing)
      {
         std::string name;
         std::string value;
         if (processor.process(*it, name, value))
         {
            if (!name.empty()) // "--" option to end options processing
            {
               handleOption(name, value);
            }
            it = _args.erase(it);
         }
         else it++;
      }
      if (!_stopOptionsProcessing) processor.checkRequired();
   }

   void Application::defineOptions(OptionSet& options)
   {
      setupOptions(options);
      
      options.addOption(
         Option("verbose", "v", "Increase verbosity [0-8]")
         .required(false)
         .repeatable(true)
         .argument("level")
         .callback(OptionCallback<Application>(this, 
                                        &Application::handleDefaultOptions)));

      options.addOption(
         Option("help", "h", "Display help information")
         .required(false)
         .repeatable(false)
         .callback(OptionCallback<Application>(this, 
                                         &Application::handleDefaultOptions)));

   }

   void Application::handleOption(const std::string& name, 
                                  const std::string& value)
   {
      const Option& option = _options.getOption(name);

      if (option.callback()) option.callback()->invoke(name, value);
   }

   void Application::handleDefaultOptions(const std::string& name, 
                                          const std::string& value)
   {
      // Help
      if(name=="help")
      {
         helpRequested = true;

         HelpFormatter helpFormatter(options());
         helpFormatter.setUnixStyle(unixStyle);
         helpFormatter.setAutoIndent();
    
         std::string helpFooter  = commandName() + " " + appVersion + " on ";
         helpFooter += SysInfo::osName() + " ["+SysInfo::osArchitecture()+"]    ";
         helpFooter += "Copyright 2010-2015  "+appAuthor+ ".";


         helpFormatter.setCommand(commandName());
         helpFormatter.setHeader(appDesc);
         helpFormatter.setUsage(appUsage);
         helpFormatter.setFooter(helpFooter);

         helpFormatter.format(std::cout);

         stopOptionsProcessing();
      }

      if(name=="verbose")
      {
         verboseLevel = StringUtils::asInt(value);
         if(verboseLevel<0 || verboseLevel>8) 
         {
            verboseLevel = 6;
            logger().warning("wrong verbose level, and set it to default 6 [information]");
         }
      }
   }

   void Application::stopOptionsProcessing()
   {
      _stopOptionsProcessing = true;
   }

   void Application::initialize(Application& self)
   {
      if(!helpRequested)
      {
         try
         {
            spinUp();
         }
         catch(Exception& e)
         {
            logger().error(std::string("[spinUp] ") + e.what());
         }
         catch(std::exception& e)
         {
            logger().error(std::string("[spinUp] ") + e.what());
         }
         catch(...)
         {
            logger().error(std::string("[spinUp] ") + "unknown system error.");
         }  
      }

      _initialized = true;
   }

   int Application::main(const std::vector<std::string>& args)
   {
      if(!helpRequested)
      {
         try
         {
            runTime.update();
            process(args);
         }
         catch(Exception& e)
         {
            logger().error(e.what());
            return EXIT_SOFTWARE;
         }
         catch(std::exception& e)
         {
            logger().error(e.what());
            return EXIT_SOFTWARE;
         }
         catch(...)
         {
            logger().error("Unknown system error.");
            return EXIT_SOFTWARE;
         }
      }

      return EXIT_OK;
   }

   void Application::uninitialize()
   {
      if(!helpRequested)
      {
         try
         {
            shutDown();
         }
         catch(Exception& e)
         {
            logger().error(std::string("[shutDown] ") + e.what());
         }
         catch(std::exception& e)
         {
            logger().error(std::string("[shutDown] ") + e.what());
         }
         catch(...)
         {
            logger().error(std::string("[shutDown] ") + "unknown system error.");
         }  
      }

      _initialized = false;

   }  // End of method 'Application::uninitialize()'


   Logger& Application::logger() const
   {
      return Logger::get("");
   }

   LogStream Application::logstream()
   {
      return LogStream(Logger::get(""));
   }

   std::string Application::commandName() const
   {
      std::vector<std::string> paths = split(_command,"/\\");
      return (*paths.rbegin());
   }

   const OptionSet& Application::options() const
   {
      return _options;
   }

   double Application::toltalMilliseconds()
   {
      return runTime.elapsed()*1000.0;
   }

   Application& Application::appInfo(const std::string& author,
                                     const std::string& version,
                                     const std::string& description,
                                     const std::string& useage)
   {
      appAuthor = author;
      appVersion = version;
      appDesc = description;
      appUsage = useage;

      return (*this);
   }


}   // End of namespace gpstk

