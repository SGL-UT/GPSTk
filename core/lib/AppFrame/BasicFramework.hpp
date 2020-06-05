//==============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file BasicFramework.hpp
 * Basic framework for programs in the GPS toolkit
 */

#ifndef GPSTK_BASICFRAMEWORK_HPP
#define GPSTK_BASICFRAMEWORK_HPP

#include "CommandOptionParser.hpp"
#include "MainAdapter.hpp"

namespace gpstk
{

      /** @defgroup AppFrame Frameworks for Applications
       *
       * The application frameworks provide a set of classes that
       * perform the basic functions of applications within the GPS
       * toolkit.  That is, they provide a framework for applications
       * so that the applications only have to implement those
       * features which are unique to that application.
       *
       * The classes are defined in a tree of increasing capability,
       * that is, the BasicFramework class at the root of the tree
       * does very little and implements only those functions which
       * are common to all programs within the toolkit.  Each
       * subsequent subclass adds additional layers to these basic
       * capabilities.
       *
       * The end user is expected to create a class, which inherits
       * from one of these frameworks, and override the appropriate
       * methods in order to perform the necessary function of that
       * program.  The methods to be overridden depend on the
       * framework being used and what the program is intended to
       * do.
       */

      /** @page appbuilding Building Up Applications Using Frameworks
       *
       * \tableofcontents
       *
       * While not GNSS-related, the gpstk provides a set of
       * frameworks to reduce the amount of effort required when
       * writing applications.
       *
       * The classes used for quickly implementing applications are:
       *   - BasicFramework for simple applications with no
       *     repetitive processing.
       *   - LoopedFramework for applications with repetetive processing.
       *   - CommandOption which is the parent class for a myriad of
       *     specialized command-line option processing classes.
       *
       * @section overview Overview
       *
       * Using the application frameworks is pretty consistent across
       * all applications (which of course is the intent).  An
       * application will define a class that derives from one of the
       * frameworks, and overrides the methods of the framework class
       * that are appropriate for the application.  The implementation
       * of the main function is very similar from application to
       * application, with only the application class name changing,
       * in general.
       *
       * The structure of applications using these frameworks follows
       * the following formula:
       *
       * @code{.cpp}
       * class MyApp : public gpstk::BasicFramework
       * {
       * public:
       *    MyApp(const std::string& appName);
       *    virtual ~MyApp();
       *    void process() override;
       * };
       *
       * int main(int argc, char *argv[])
       * {
       *    try
       *    {
       *       MyApp app(argv[0]);
       *       if (app.initialize(argc, argv))
       *       {
       *          app.run();
       *       }
       *       return app.exitCode;
       *    }
       *    catch (gpstk::Exception& e)
       *    {
       *       cerr << e << endl;
       *    }
       *    catch (std::exception& e)
       *    {
       *       cerr << e.what() << endl;
       *    }
       *    catch (...)
       *    {
       *       cerr << "Caught unknown exception" << endl;
       *    }
       *    return gpstk::BasicFramework::EXCEPTION_ERROR;
       * }
       * @endcode
       *
       * @section basic BasicFramework Usage
       *
       * When you need to implement an application that performs a
       * single task before exiting (timeconvert is a good example of
       * this), BasicFramework is probably the ideal framework class
       * for you to use.
       * 
       * The flow of execution for applications derived from the
       * BasicFramework class is as follows (indentation levels
       * indicate the call stack):
       * -# main()
       *   -# Constructor
       *   -# initialize() (often overridden)
       *   -# run() (never overridden)
       *      -# completeProcessing() (rarely overridden)
       *         -# additionalSetup() (often overridden, empty by default)
       *         -# spinUp() (often overridden, empty by default)
       *         -# process() (often overridden, empty by default)
       *      -# shutDown() (sometimes overridden)
       *
       * The functions/methods of an application class perform
       * functions as follows:
       * - main() instantiates the application class and executes the code.
       * - Constructor does all the usual things C++ constructors do,
       *   including calling the parent's class' constructor and
       *   initializing the command-line option objects.
       * - initialize() processes the command-line options.
       * - run() calls the completeProcessing() method in a try/catch
       *   block, and calls shutDown().
       * - completeProcessing() calls additionalSetup(), spinUp() and
       *   process() in that order.  Sometimes it may be overridden
       *   for additional processing (see @ref looped), but this is
       *   fairly rare and typically is only done in specialized
       *   frameworks.
       * - additionalSetup() is a method intended to be used when
       *   additional tasks are needed to be done prior to the main
       *   execution that depend on the completion of tasks in the
       *   initialize() method.
       * - spinUp() is a method intended to be used when additional
       *   tasks are needed to be done prior to the main execution
       *   that depend on the completion of tasks in both the
       *   initialize() method and the additionalSetup() method.
       * - process() is where the meat of the application is,
       *   i.e. where the primary application work takes place.
       * - shutDown() is called at the end of successful (no
       *   exceptions thrown) program execution.  It is meant to be
       *   used to allow an application to clean up after itself, like
       *   closing files, network connections, etc.
       *
       * Why are there so many methods defined?  The main reason is to
       * allow further differentiation when specializing frameworks.
       * That is, there are derived classes that provide further
       * frameworks capabilities where initialization must occur in a
       * very specific order, but the subclasses need to be able to
       * override specific behaviors.
       *
       * @subsection exit Exit Codes
       *
       * The BasicFramework class defines a data member named \a
       * exitCode.  This is intended to be used as the return value
       * for main() (see the example in \ref overview).  It is
       * initialized to 0, which indicates successful completion of
       * applications.  Any non-zero return from main will be
       * interpreted by the shell as a failed execution.  While
       * BasicFramework will set \a exitCode to an error code for
       * exceptions caught within run() or command-line processing
       * errors within initialize(), implementers should remember two
       * things:
       * -# The implementer should set the \a exitCode variable to an
       *    appropriate value when terminating on failure for any
       *    condition not handled as described above.
       * -# The implementer \b must explicitly return the value of \a
       *    exitCode in their main() function.  This is not handled
       *    automatically by the frameworks.
       *
       * The following generic exit codes are defined within BasicFramework:
       * - BasicFramework::EXCEPTION_ERROR
       * - BasicFramework::OPTION_ERROR
       * - BasicFramework::EXIST_ERROR
       *
       * Other exit codes can be defined as needed on a per-application basis.
       *
       * @section looped LoopedFramework Usage
       *
       * LoopedFramework is identical to BasicFramework with the
       * following exceptions:
       * - a boolean flag named \a timeToDie is added to control loop execution.
       * - the process() method is called by completeProcessing() continuously
       *   until the \a timeToDie flag is set to true.
       *
       * LoopedFramework is meant to be used when executing a set of
       * statements repeatedly.  Derived classes should set \a
       * timeToDie to true whenever an appropriate termination
       * condition has been met.
       *
       * @section cmdopt Command-Line Options
       *
       * Command-line option processing used by the application
       * frameworks involves the instantiation of objects derived from
       * the CommandOption class.  Usually this is done inside the
       * application class constructor, however there may be
       * occassions where the command option object is instantiated
       * dynamically based on some conditional (typically only done in
       * other frameworks).  The CommandOption class and children do
       * not have a default constructor, which is the primary reason
       * for instantiating these objects in the application
       * constructor.
       *
       * The CommandOption classes in most cases will let you specify
       * a short command-line option (e.g. 'h' becomes -h on the
       * command line), a long command-line option (e.g. "help"
       * becomes --help on the command line), a description of the
       * command line option to be printed when help is requested, and
       * a flag indicating whether the command-line argument is
       * required or not.  The CommandOption class has several other
       * constructor parameters but using that class directly rather
       * than one of the child classes is pretty much unheard of.
       *
       * Really basic command-line option classes that are never directly used:
       * - CommandOption
       * - RequiredOption
       * - CommandOptionWithArg
       *
       * Command-line options that tage an argument of some sort,
       * usually with format checking of that argument:
       * - CommandOptionNoArg
       * - CommandOptionWithAnyArg - no format checking
       * - CommandOptionWithStringArg
       * - CommandOptionWithNumberArg - integer argument
       * - CommandOptionWithDecimalArg - decimal argument
       * - CommandOptionWithTimeArg - multi-format time argument
       * - CommandOptionWithSimpleTimeArg - pre-defined multi-format
       *   time argument
       * - CommandOptionWithCommonTimeArg - formatted time argument
       * - CommandOptionWithPositionArg - formatted position argument
       * - CommandOptionRest - command-line arguments with no specified option.
       *
       * Classes for defining relationships between real command-line arguments:
       * - CommandOptionNOf - Define a set of options of which some
       *   number must be used.
       * - CommandOptionOneOf - Define a set of options of which at least
       *   one must be used.
       * - CommandOptionAllOf - Define a set of options, all of which
       *   must be used.
       * - CommandOptionMutex - Define a set of options of which only
       *   one may be used.
       * - CommandOptionDependent - Define a pair of options where if
       *   one is specified, the other must be.
       *
       * Classes for implementing logical checks in conjunction
       * (i.e. as one of the "options") in the above classes:
       * - CommandOptionGroupOr - Define a set of options, without
       *   checking, that can be grouped together.
       * - CommandOptionGroupAnd - Define a set of options, without
       *   checking, that must be grouped together.
       *
       * Help options.  These are handled specially by BasicFramework such that
       * - CommandOptionHelp is the parent class for all help
       *   requested on the command-line,
       * - CommandOptionHelpUsage is the command-line option help,
       *   that prints out the standard usage information (in
       *   conjunction with -h or --help arguments).
       * - CommandOptionHelpSimple provides a very basic help feature
       *   that prints a pre-defined message.
       *
       * BasicFramework will exit after printing the appropriate help
       * information when any of the above command-line option types
       * are requested by the user.
       *
       * The arguments to the command-line arguments (e.g. for "-n 5",
       * "5" is the argument) are available as a vector of strings
       * (where each value in the vector corresponds to one use of the
       * command-line argument) via the getValue() method.  In most
       * cases you will need to convert the strings to the appropriate
       * data type yourself (time being an exception).
       *
       * As an example, if a user specifies "-n 5 -n 7 -n 11",
       * getValue() will return an array of the strings "5","7,"11".
       * A common method for handling cases where only one argument is
       * allowed is to use the vector [] operator like
       * "someOpt.getValue()[0]".
       *
       * A very common situation you will have is to limit the number
       * of times a command-line argument can be used.  This is
       * usually done by calling the setMaxCount() method on the
       * CommandOption-derived class in the application's constructor.
       * See the example codes and CommandOption documentation for
       * more details.
       */

      /** @example CommandOption1.cpp
       * This is a very simple example of how to add support for a
       * command-line argument to a BasicFramework-derived
       * application. */

      /** @example CommandOption2.cpp
       * This is an example of how to add support for command-line
       * arguments that incorporate some degree of checking of the
       * option format (integer, float, time) to a
       * BasicFramework-derived application. */

      /** @example CommandOption3.cpp
       * Example use of CommandOptionNOf where two options are
       * expected out of a set of various time options, and
       * CommandOptionOneOf. */

      /// @ingroup AppFrame
      //@{

      /**
       * This is a (very) basic framework for programs in the GPS
       * toolkit.  It is meant to be used by programs that start up,
       * do some processing, and quit.
       *
       * The end user should define subclasses of this class,
       * implementing those methods described as being meant to be
       * overridden; initialize(), additionalSetup(), spinUp(), process(),
       * and shutDown().
       *
       * In use, the user will construct an object of the class
       * derived from this, then call the run() method.
       */
   class BasicFramework
   {
   public:
         /** Exit code used when an exception has been caught in
          * run().  Not guaranteed to be unique to this condition. */
      static const int EXCEPTION_ERROR = 1;
         /** Exit code used when an error has occurred in processing
          * command-line options.  Not guaranteed to be unique to this
          * condition. */
      static const int OPTION_ERROR = 2;
         /** Exit code used when an input file does not exist or is
          * not accessible. */
      static const int EXIST_ERROR = 2;

         /** Constructor for BasicFramework.
          *
          * @param[in] applName   name of the program (argv[0]).
          * @param[in] applDesc   text description of program's function
          *                       (used by CommandOption help).
          */
      BasicFramework( const std::string& applName,
                      const std::string& applDesc )
         throw();


         /// Destructor.
      virtual ~BasicFramework() {};


         /** Process command line arguments. When this method is overridden,
          *  make sure to call the parent class's initialize().
          *
          * @param[in] argc    same as main() argc.
          * @param[in] argv    same as main() argv.
          * @param[in] pretty  Whether the 'pretty print' option will
          *                    be used when printing descriptions. It
          *                    is 'TRUE' by default.
          *
          * @return true if normal processing should proceed (i.e. no
          *         command line errors or help requests).
          */
      virtual bool initialize( int argc,
                               char *argv[],
                               bool pretty = true )
         throw();


         /** Run the program. Processes only once (refer to subclasses
          *  for looped processing).
          *
          * @return false if an exception occurred
          */
      bool run() throw();


         /** A place to store the exit code for the application.
          * Defaults to 0 which indicates successful completion.
          * It is recommended that your application:
          *   1) Set this value on terminal error.
          *   2) Return this value from main in all cases. */
      int exitCode;


   protected:

      int debugLevel;           ///< Debug level for this run of the program.
      int verboseLevel;         ///< Verbose level for this run of the program.
      std::string argv0;        ///< Name of the program.
      std::string appDesc;      ///< Description of program's function.

         /// Enable/increase debugging output.
      CommandOptionNoArg debugOption;
         /// Enable/increase informational output.
      CommandOptionNoArg verboseOption;
         /// Request command-line option usage.
      CommandOptionHelpUsage helpOption;


         /**
          * Called by the run() method; calls additionalSetup(),
          * spinUp(), and process(), in that order.  Generally should
          * not be overridden.
          */
      virtual void completeProcessing();


         /**
          * Additional set-up to be performed before starting
          * processing.  This generally involves things that are
          * necessary for either the spinUp processing or main
          * processing. This method should be implemeneted by the end-user.
          */
      virtual void additionalSetup()
      {}


         /**
          * Code to be executed AFTER initialize() and additionalSetup().
          * This method should be implemeneted by the end-user.
          */
      virtual void spinUp()
      {}


         /**
          * Processing to be performed.  This method should be
          * implemeneted by the end-user.
          */
      virtual void process()
      {}


         /**
          * Clean-up processing to be done before the program ends.
          * This method is executed outside of a try block and should
          * be implemeneted by the end-user.
          */
      virtual void shutDown()
      {}


   private:


         // Do not allow the use of the default constructor.
      BasicFramework();

   }; // End of class 'BasicFramework'

      //@}

}  // End of namespace gpstk
#endif   // GPSTK_BASICFRAMEWORK_HPP
