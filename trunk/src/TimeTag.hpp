#pragma ident "$Id: //depot/sgl/gpstk/dev/src/TimeTag.hpp#2 $"

#ifndef GPSTK_TIMETAG_HPP
#define GPSTK_TIMETAG_HPP

#include "CommonTime.hpp"
#include "StringUtils.hpp"
#include <map>

namespace gpstk
{
      /** This class is an abstract base class inherited by all the classes
       * that provide a specific time representation.
       * These time representations must ensure or provide a way for the 
       * user to verify their own self-consistency.
       */
   class TimeTag
   {
   public:
      
         /// This is the regular expression prefix that is used when 
         /// searching for integer specifiers.
      static const std::string formatPrefixInt;
         /// This is the regular expression prefix that is used when
         /// searching for float specifiers.
      static const std::string formatPrefixFloat;

         /// Virtual Destructor.
      virtual ~TimeTag() 
         throw()
      {}
      
         /// This function probably won't throw exceptions because if you can
         /// represent a time in this format, you can probably represent it in
         /// the Common format.
      virtual CommonTime convertToCommonTime() const = 0;
      
         /// This function may throw exceptions b/c it is possible that it may
         /// not be possible to represent the  time specified by the Common 
         /// object "bt" in this object.
      virtual void convertFromCommonTime( const CommonTime& ct ) = 0;

         /// This function formats this time to a string.  The exceptions 
         /// thrown would only be due to problems parsing the fmt string.
         /// 
      virtual std::string printf(const std::string& fmt) const
         throw( gpstk::StringUtils::StringException ) = 0;

         /**
          * This function sets this object to the time stored in the given
          * string \a str.
          * @param str the string to parse
          * @param fmt the string containing the time format
          * @throw InvalidRequest if an invalid time is given.
          * @throw StringException on problems parsing the strings.
          */
      virtual void scanf( const std::string& str,
                          const std::string& fmt )
         throw( gpstk::InvalidRequest,
                gpstk::StringUtils::StringException );

         /// This type maps a character ( from a time format ) to its
         /// corresponding value ( from a time string ).
      typedef std::map< char, std::string> IdToValue;
      
         /**
          * This is the workhorse for the scanf family of functions.  It parses
          * the given string \a str with the given format \a fmt in order to
          * extract a time or part of a time from \a str. 
          * @param str the string from which to cull time information 
          * @param fmt the string containing the time format 
          * @param info a mapping of all of the character identifier to their 
          *  respective culled time information
          * @throw StringException on problems parsing the strings.
          */
      static void getInfo( const std::string& str,
                           const std::string& fmt,
                           IdToValue& info )
         throw( gpstk::StringUtils::StringException );

         /**
          * Set this object using the information provided in \a info.
          * @param info the IdToValue object to which this object shall be set.
          * @return true if this object was successfully set using the 
          *  data in \a info, false if not.
          */
      virtual bool setFromInfo( const IdToValue& info )
         throw() = 0;
      
         /// Return a string containing the characters that this class
         /// understands when printing times.
      virtual std::string getPrintChars() const
         throw() = 0;

         /// Return a string containing the default format to use in printing.
      virtual std::string getDefaultFormat() const
         throw() = 0;

         /// Format this time as a string using a format specified by the 
         /// inheriting class.
      virtual std::string asString() const
         throw( gpstk::StringUtils::StringException )
      { return printf( getDefaultFormat() ); }

         /// Hey, it's an implicit casting operator!  Basically just a lazy
         /// way to get at convertToCommonTime().
      operator CommonTime() const
      { return convertToCommonTime(); }

         /// Returns true if this object's members are valid, false otherwise.
      virtual bool isValid() const
         throw() = 0;

   };

} // namespace

std::ostream& operator<<( std::ostream& s,
                          const gpstk::TimeTag& t );

#endif // GPSTK_BASETIME_HPP
