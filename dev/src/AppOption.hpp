#pragma ident "$Id$"

/**
 * @file AppOption.hpp
 * Some classes modified from Poco to process command line options.
 */

#ifndef GPSTK_APPOPTION_HPP
#define GPSTK_APPOPTION_HPP

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
#include <set>
#include "String.hpp"
#include "Exception.hpp"
#include "DebugUtils.hpp"

namespace gpstk
{
      //
      // class OptionCallback 
      //

   class AbstractOptionCallback
   {
   public:
      virtual void invoke(const std::string& name, 
                          const std::string& value) const = 0;

      virtual AbstractOptionCallback* clone() const = 0;

      virtual ~AbstractOptionCallback(){}

   protected:
      AbstractOptionCallback(){}
      AbstractOptionCallback(const AbstractOptionCallback&){}
   };

   template <class C>
   class OptionCallback: public AbstractOptionCallback
   {
   public:
      typedef void (C::*Callback)(const std::string& name, 
                                  const std::string& value);

      OptionCallback(C* pObject, Callback method)
         : _pObject(pObject), _method(method) { GPSTK_CHECK_PTR(pObject); }

      OptionCallback(const OptionCallback& cb)
         : AbstractOptionCallback(cb), _pObject(cb._pObject),_method(cb._method)
      {}

      ~OptionCallback(){}

      OptionCallback& operator = (const OptionCallback& cb)
      {
         if (&cb != this)
         {
            this->_pObject = cb._pObject;
            this->_method  = cb._method;
         }

         return *this;
      }

      void invoke(const std::string& name, const std::string& value) const
      { (_pObject->*_method)(name, value); }

      AbstractOptionCallback* clone() const
      { return new OptionCallback(_pObject, _method); }

   protected:
      OptionCallback(){}

      C* _pObject;
      Callback _method;
   };

   //////////////////////////////////////////////////////////////////////////

      /// This class represents and stores the properties
      /// of a command line option.
      ///
      /// An option has a full name, an optional short name,
      /// a description (used for printing a usage statement),
      /// and an optional argument name.
      /// An option can be optional or required.
      /// An option can be repeatable, which means that it can
      /// be given more than once on the command line.
      ///
      /// An option can be part of an option group. At most one
      /// option of each group may be specified on the command
      /// line.
      ///
      /// An option can be bound to a configuration property.
      /// In this case, a configuration property will automatically
      /// receive the option's argument value.
      ///
      /// A callback method can be specified for options. This method
      /// is called whenever an option is specified on the command line.
      ///
      ///
      /// Option instances are value objects.
      ///
      /// Typcally, after construction, an Option object is immediately
      /// passed to an Options object.
      ///
      /// An Option object can be created by chaining the constructor
      /// with any of the setter methods, as in the following example:
      ///
      ///     Option versionOpt("include", "I", "specify an include directory")
      ///        .required(false)
      ///        .repeatable(true)
      ///        .argument("directory");
   class Option
   {
   public:
      Option()
         : _required(false), _repeatable(false), 
           _argRequired(false),_pCallback(0) {}


      Option(const Option& option)
         : _shortName(option._shortName),
           _fullName(option._fullName),
           _description(option._description),
           _required(option._required),
           _repeatable(option._repeatable),
           _argName(option._argName),
           _argRequired(option._argRequired),
           _group(option._group),
           _pCallback(option._pCallback)
      { if (_pCallback) _pCallback = _pCallback->clone();}


      Option(const std::string& fullName, const std::string& shortName)
         : _shortName(shortName),
           _fullName(fullName),
           _required(false),
           _repeatable(false),
           _argRequired(false),
           _pCallback(0) {}


      Option(const std::string& fullName, 
             const std::string& shortName, 
             const std::string& description, 
             bool required=false)
         : _shortName(shortName),
           _fullName(fullName),
           _description(description),
           _required(required),
           _repeatable(false),
           _argRequired(false),
           _pCallback(0) {}


      Option(const std::string& fullName, 
             const std::string& shortName, 
             const std::string& description, 
             bool required, 
             const std::string& argName, 
             bool argRequired=false)
         : _shortName(shortName),
           _fullName(fullName),
           _description(description),
           _required(required),
           _repeatable(false),
           _argName(argName),
           _argRequired(argRequired),
           _pCallback(0) {}

      ~Option() { delete _pCallback; }

      Option& operator = (const Option& option)
      {
         if (&option != this)
         {
            Option tmp(option);
            swap(tmp);
         }
         return *this;
      }

      void swap(Option& option)
      {
         std::swap(_shortName, option._shortName);
         std::swap(_fullName, option._fullName);
         std::swap(_description, option._description);
         std::swap(_required, option._required);
         std::swap(_repeatable, option._repeatable);
         std::swap(_argName, option._argName);
         std::swap(_argRequired, option._argRequired);
         std::swap(_group, option._group);
         std::swap(_pCallback, option._pCallback);
      }

      Option& shortName(const std::string& name)
      {
         _shortName = name;
         return *this;
      }

      Option& fullName(const std::string& name)
      {
         _fullName = name;
         return *this;
      }

      Option& description(const std::string& text)
      {
         _description = text;
         return *this;
      }

      Option& required(bool flag)
      {
         _required = flag;
         return *this;
      }

      Option& repeatable(bool flag)
      {
         _repeatable = flag;
         return *this;
      }

      Option& argument(const std::string& name, bool required = true)
      {
         _argName     = name;
         _argRequired = required;
         return *this;
      }

      Option& noArgument()
      {
         _argName.clear();
         _argRequired = false;
         return *this;
      }

      Option& Option::group(const std::string& group)
      {
         _group = group;
         return *this;
      }

      Option& callback(const AbstractOptionCallback& cb)
      {
         _pCallback = cb.clone();
         return *this;
      }

      const std::string& shortName() const 
      { return _shortName; }

      const std::string& fullName() const 
      { return _fullName; }

      const std::string& description() const
      { return _description; }

      bool required() const
      { return _required; }

      bool repeatable() const
      { return _repeatable; }

      bool takesArgument() const
      { return !_argName.empty(); }

      bool argumentRequired() const
      { return _argRequired; }

      const std::string& argumentName() const
      { return _argName; }

      const std::string& group() const
      { return _group; }

      AbstractOptionCallback* callback() const
      { return _pCallback; }

      bool matchesShort(const std::string& option) const
      {
         return option.length() > 0 
            && !_shortName.empty() && option.compare(0, _shortName.length(), _shortName) == 0;
      }

      bool matchesFull(const std::string& option) const
      {
         std::string::size_type pos = option.find_first_of(":=");
         std::string::size_type len = pos == std::string::npos ? option.length() : pos;
         return len == _fullName.length()
            && icompare(option, 0, len, _fullName, 0, len) == 0;
      }

      bool matchesPartial(const std::string& option) const
      {
         std::string::size_type pos = option.find_first_of(":=");
         std::string::size_type len = pos == std::string::npos ? option.length() : pos;
         return option.length() > 0 
            && icompare(option, 0, len, _fullName, 0, len) == 0;
      }

      inline void process(const std::string& option, std::string& arg) const;
       
   protected:
      std::string _shortName;
      std::string _fullName;
      std::string _description;
      bool        _required;
      bool        _repeatable;
      std::string _argName;
      bool        _argRequired;
      std::string _group;
      AbstractOptionCallback* _pCallback;

   }; // End of class 'Option'

   inline void Option::process(const std::string& option, 
                               std::string& arg) const
   {
      std::string::size_type pos = option.find_first_of(":=");
      std::string::size_type len = (pos==std::string::npos)?option.length():pos;
      if (icompare(option, 0, len, _fullName, 0, len) == 0)
      {
         if (takesArgument())
         {
            if (argumentRequired() && pos == std::string::npos)
               GPSTK_THROW(Exception(_fullName+" requires "+argumentName()));
            if (pos != std::string::npos)
               arg.assign(option, pos + 1, option.length() - pos - 1);
            else
               arg.clear();
         }
         else if (pos != std::string::npos)
         {
            GPSTK_THROW(Exception("Unexpected argument "+option));
         }
         else arg.clear();
      }
      else if (!_shortName.empty() && 
               option.compare(0, _shortName.length(), _shortName) == 0)
      {
         if (takesArgument())
         {
            if (argumentRequired() && option.length() == _shortName.length())
               GPSTK_THROW(Exception(_shortName+" requires "+argumentName()));
            arg.assign(option, _shortName.length(), 
                       option.length()-_shortName.length());
         }
         else if (option.length() != _shortName.length())
         {
            GPSTK_THROW(Exception("Unexpected argument "+option));
         }
         else arg.clear();
      }
      else GPSTK_THROW(Exception("Unknown option "+option));

   }  // End of method 'Option::process()'


   class OptionSet
   {
   public:
      typedef std::vector<Option> OptionVec;
      typedef OptionVec::const_iterator Iterator;

      OptionSet(){}

      OptionSet(const OptionSet& options)
         : _options(options._options) {}

      ~OptionSet() {}

      OptionSet& operator = (const OptionSet& options)
      {
         if (&options != this) _options = options._options;
         return *this;
      }
      
      void addOption(const Option& option)
      {
         GPSTK_ASSERT(!option.fullName().empty());

         for (Iterator it=begin(); it != end(); it++)
         {
            if (it->fullName() == option.fullName())
            {
               GPSTK_THROW(Exception(it->fullName()));
            }
         }

         _options.push_back(option);
      }

         /// Returns a true iff an option with the given name exists.
         ///
         /// The given name can either be a fully specified short name,
         /// or a partially specified full name. If a partial name
         /// matches more than one full name, false is returned.
         /// The name must either match the short or full name of an
         /// option. Comparison case sensitive for the short name and
         /// not case sensitive for the full name.
      bool hasOption(const std::string& name, bool matchShort = false) const
      {
         bool found = false;
         for (Iterator it = _options.begin(); it != _options.end(); ++it)
         {
            if ((matchShort && it->matchesShort(name)) || 
                (!matchShort && it->matchesFull(name)) )
            {
               if (!found) found = true;
               else return false;
            }
         }
         return found;
      }
      
         /// Returns a reference to the option with the given name.
         ///
         /// The given name can either be a fully specified short name,
         /// or a partially specified full name.
         /// The name must either match the short or full name of an
         /// option. Comparison case sensitive for the short name and
         /// not case sensitive for the full name.
         /// Throws a NotFoundException if no matching option has been found.
         /// Throws an UnknownOptionException if a partial full name matches
         /// more than one option.
      const Option& getOption(const std::string& name, 
                              bool matchShort = false) const
      {
         const Option* pOption = 0;
         for (Iterator it = _options.begin(); it != _options.end(); ++it)
         {
            if ((matchShort && it->matchesShort(name)) || 
                (!matchShort && it->matchesPartial(name)))
            {
               if (!pOption)
               {
                  pOption = &*it;
                  if (!matchShort && it->matchesFull(name)) break;
               }
               else if (!matchShort && it->matchesFull(name))
               {
                  pOption = &*it;
                  break;
               }
               else GPSTK_THROW(Exception("Ambiguous Option " + name));
            }
         }
         if (pOption) return *pOption;
         else GPSTK_THROW(Exception("Unknown Option " + name));
      }
      
      Iterator begin() const { return _options.begin(); }

      Iterator end() const { return _options.end(); }

   protected:	
      OptionVec _options;

   }; // End of class 'OptionSet'
   

      /// An OptionProcessor is used to process the command line
      /// arguments of an application.
      ///
      /// The process() method takes an argument from the command line.
      /// If that argument starts with an option prefix, the argument
      /// is further processed. Otherwise, the argument is ignored and
      /// false is ignored. The argument must match one of the options
      /// given in the OptionSet that is passed to the OptionProcessor
      /// with the constructor. If an option is part of a group, at most
      /// one option of the group can be passed to the OptionProcessor.
      /// Otherwise an IncompatibleOptionsException is thrown.
      /// If the same option is given multiple times, but the option
      /// is not repeatable, a DuplicateOptionException is thrown.
      /// If the option is not recognized, a UnexpectedArgumentException
      /// is thrown.
      /// If the option requires an argument, but none is given, an
      /// MissingArgumentException is thrown.
      /// If no argument is expected, but one is present, a
      /// UnexpectedArgumentException is thrown.
      /// If a partial option name is ambiguous, an AmbiguousOptionException
      /// is thrown.
      ///
      /// The OptionProcessor supports two modes: Unix mode and default mode.
      /// In Unix mode, the option prefix is a dash '-'. A dash must be followed
      /// by a short option name, or another dash, followed by a (partial)
      /// long option name.
      /// In default mode, the option prefix is a slash '/', followed by 
      /// a (partial) long option name.
      /// If the special option '--' is encountered in Unix mode, all following
      /// options are ignored.
   class OptionProcessor
   {
   public:
      OptionProcessor(const OptionSet& options)
         :_options(options),_unixStyle(true),_ignore(false){}
      
      ~OptionProcessor() {}

      void setUnixStyle(bool flag)
      { _unixStyle = flag; }
      

      bool isUnixStyle() const
      { return _unixStyle; }
      

      bool process(const std::string& argument, 
                   std::string& optionName, 
                   std::string& optionArg)
      {
         if (!_ignore)
         {
            if (_unixStyle) return processUnix(argument, optionName, optionArg);
            else return processDefault(argument, optionName, optionArg);
         }
         return false;
      }


      void checkRequired() const
      {
         for (OptionSet::Iterator it = _options.begin(); it != _options.end(); ++it)
         {
            if (it->required() && 
                _specifiedOptions.find(it->fullName())==_specifiedOptions.end())
               GPSTK_THROW(Exception(it->fullName()));
         }
      }


   protected:
      inline bool processUnix(const std::string& argument, 
                              std::string& optionName, 
                              std::string& optionArg );
      
      inline bool processDefault(const std::string& argument, 
                                 std::string& optionName, 
                                 std::string& optionArg);
      
      inline bool processCommon(const std::string& optionStr, 
                                bool isShort, 
                                std::string& optionName, 
                                std::string& optionArg);

      const OptionSet& _options;
      bool _unixStyle;
      bool _ignore;
      std::set<std::string> _groups;
      std::set<std::string> _specifiedOptions;

   }; // End of class 'OptionProcessor'

   inline bool OptionProcessor::processUnix(const std::string& argument, 
                                            std::string& optionName, 
                                            std::string& optionArg )
   {
      std::string::const_iterator it  = argument.begin();
      std::string::const_iterator end = argument.end();
      if (it != end)
      {
         if (*it == '-')
         {
            ++it;
            if (it != end)
            {
               if (*it == '-')
               {
                  ++it;
                  if (it == end)
                  {
                     _ignore = true;
                     return true;
                  }
                  else return processCommon(std::string(it, end), false, optionName, optionArg);
               }
               else return processCommon(std::string(it, end), true, optionName, optionArg);
            }
         }
      }
      return false;

   }  // End of method 'OptionProcessor::processUnix()'

   inline bool OptionProcessor::processDefault(const std::string& argument, 
                                               std::string& optionName, 
                                               std::string& optionArg)
   {
      std::string::const_iterator it  = argument.begin();
      std::string::const_iterator end = argument.end();
      if (it != end)
      {
         if (*it == '/')
         {
            ++it;
            return processCommon(std::string(it, end), false, optionName, optionArg);
         }
      }
      return false;

   }  // End of method 'OptionProcessor::processDefault()'

   inline bool OptionProcessor::processCommon(const std::string& optionStr, 
                                              bool isShort, 
                                              std::string& optionName, 
                                              std::string& optionArg)
   {
      if (optionStr.empty()) throw Exception();
      const Option& option = _options.getOption(optionStr, isShort);
      const std::string& group = option.group();
      if (!group.empty())
      {
         if (_groups.find(group) != _groups.end())
         {
            GPSTK_THROW(Exception(option.fullName()));
         }
         else
            _groups.insert(group);
      }
      if (_specifiedOptions.find(option.fullName())!=_specifiedOptions.end() && 
          !option.repeatable())
      {
         GPSTK_THROW(Exception(option.fullName()));
      }
      _specifiedOptions.insert(option.fullName());
      option.process(optionStr, optionArg);
      optionName = option.fullName();

      return true;

   }  // End of method 'OptionProcessor::processCommon()'

  
   class HelpFormatter
   {
   public:
      HelpFormatter(const OptionSet& options)
         : _options(options),
           _width(78),
           _tabWidth(4),
           _indent(0),
           _unixStyle(true) { _indent = calcIndent(); }

      ~HelpFormatter(){}

      void setCommand(const std::string& command)
      { _command = command; }

      const std::string& getCommand() const
      { return _command; }

      void setUsage(const std::string& usage)
      { _usage = usage; }

      const std::string& getUsage() const
      { return _usage; }

      void setHeader(const std::string& header)
      { _header = header; }

      const std::string& getHeader() const
      { return _header; }

      void setFooter(const std::string& footer)
      { _footer = footer; }

      const std::string& getFooter() const
      { return _footer; }

      void format(std::ostream& ostr) const
      {
         ostr << "Usage: " << _command;
         if (!_usage.empty())
         {
            ostr << ' ';
            formatText(ostr, _usage, (int) _command.length() + 1);
         }
         ostr << '\n';
         if (!_header.empty())
         {
            formatText(ostr, _header, 0);
            ostr << "\n\n";
         }
         ostr << "Options: " << "\n";
         formatOptions(ostr);
         if (!_footer.empty())
         {
            ostr << '\n';
            formatText(ostr, _footer, 0);
            ostr << '\n';
         }
      }

      void setWidth(int width)
      { _width = width; }

      int getWidth() const
      { return _width; }

      void setIndent(int indent)
      { _indent = indent; }

      int getIndent() const
      { return _indent; }

      void setAutoIndent()
      { _indent = calcIndent(); }

      void setUnixStyle(bool flag)
      { _unixStyle = flag; }

      bool isUnixStyle() const
      { return _unixStyle; }

      std::string shortPrefix() const
      { return _unixStyle ? "-" : "/"; }

      std::string longPrefix() const
      { return _unixStyle ? "--" : "/"; }

   protected:

      int calcIndent() const
      {
         int indent = 0;
         for (OptionSet::Iterator it = _options.begin(); it != _options.end(); ++it)
         {
            int shortLen = (int) it->shortName().length();
            int fullLen  = (int) it->fullName().length();
            int n = 0;
            if (_unixStyle && shortLen > 0)
            {
               n += shortLen + (int) shortPrefix().length();
               if (it->takesArgument())
                  n += (int) it->argumentName().length() + (it->argumentRequired() ? 0 : 2);
               if (fullLen > 0) n += 2;
            }
            if (fullLen > 0)
            {
               n += fullLen + (int) longPrefix().length();
               if (it->takesArgument())
                  n += 1 + (int) it->argumentName().length() + (it->argumentRequired() ? 0 : 2);
            }
            n += 2;
            if (n > indent)
               indent = n;
         }
         return indent;
      }

      void formatOptions(std::ostream& ostr) const
      {
         int optWidth = calcIndent();
         for (OptionSet::Iterator it = _options.begin(); it != _options.end(); ++it)
         {
            formatOption(ostr, *it, optWidth);
            formatText(ostr, it->description(), _indent, optWidth);
            ostr << '\n';
         }
      }

      void formatOption(std::ostream& ostr, 
                        const Option& option, int width) const
      {
         int shortLen = (int) option.shortName().length();
         int fullLen  = (int) option.fullName().length();

         int n = 0;
         if (_unixStyle && shortLen > 0)
         {
            ostr << "  " << shortPrefix() << option.shortName();
            n += (int) shortPrefix().length() + (int) option.shortName().length();
            if (option.takesArgument())
            {
               if (!option.argumentRequired()) { ostr << '['; ++n; }
               ostr << option.argumentName();
               n += (int) option.argumentName().length();
               if (!option.argumentRequired()) { ostr << ']'; ++n; }
            }
            if (fullLen > 0) { ostr << ", "; n += 2; }
         }
         if (fullLen > 0)
         {
            ostr << longPrefix() << option.fullName();
            n += (int) longPrefix().length() + (int) option.fullName().length();
            if (option.takesArgument())
            {
               if (!option.argumentRequired()) { ostr << '['; ++n; }
               ostr << '=';
               ++n;
               ostr << option.argumentName();
               n += (int) option.argumentName().length();
               if (!option.argumentRequired()) { ostr << ']'; ++n; }
            }
         }
         while (n < width) { ostr << ' '; ++n; }     
      }

      void formatText(std::ostream& ostr, 
                      const std::string& text, int indent) const
      {
         formatText(ostr, text, indent, indent);
      }
      
      void formatText(std::ostream& ostr, 
                      const std::string& text, 
                      int indent, int firstIndent) const
      {
         int pos = firstIndent;
         int maxWordLen = _width - indent;
         std::string word;
         for (std::string::const_iterator it = text.begin(); it != text.end(); ++it)
         {
            if (*it == '\n')
            {
               clearWord(ostr, pos, word, indent);
               ostr << '\n';
               pos = 0;
               while (pos < indent) { ostr << ' '; ++pos; }
            }
            else if (*it == '\t')
            {
               clearWord(ostr, pos, word, indent);
               if (pos < _width) ++pos;
               while (pos < _width && pos % _tabWidth != 0)
               {
                  ostr << ' ';
                  ++pos;
               }
            }
            else if (*it == ' ')
            {
               clearWord(ostr, pos, word, indent);
               if (pos < _width) { ostr << ' '; ++pos; }
            }
            else 
            {
               if (word.length() == maxWordLen)
               {
                  clearWord(ostr, pos, word, indent);
               }
               else word += *it;
            }
         }
         clearWord(ostr, pos, word, indent);
      }

      void formatWord(std::ostream& ostr, int& pos, 
                      const std::string& word, int indent) const
      {
         if (pos + word.length() > _width)
         {
            ostr << '\n';
            pos = 0;
            while (pos < indent) { ostr << ' '; ++pos; }
         }
         ostr << word;
         pos += (int) word.length();
      }
      
      void clearWord(std::ostream& ostr, int& pos, 
                     std::string& word, int indent) const
      {
         formatWord(ostr, pos, word, indent);
         word.clear();
      }

   private:
      HelpFormatter(const HelpFormatter&);
      
      HelpFormatter& operator = (const HelpFormatter&);

      const OptionSet& _options;

      int _width;
      int _tabWidth;
      int _indent;
      
      std::string _command;
      std::string _usage;
      std::string _header;
      std::string _footer;

      bool _unixStyle;

   }; // End of class 'HelpFormatter'

}   // End of namespace gpstk


#endif  //GPSTK_OPTION_HPP

