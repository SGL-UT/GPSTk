#ifndef FORMATTEDDOUBLE_HPP
#define FORMATTEDDOUBLE_HPP

#include <iostream>
#include "StringUtils.hpp"

namespace gpstk
{
      /** There are numerous file formats used in GPS and probably
       * other systems that print floating point numbers that don't
       * conform to standard formats used by C++, including
       * FORTRAN-esque use of 'D' to indicate the exponent, or having
       * a fixed and/or large number of digits in the exponent.  This
       * class tries to resolve that by making a double-precision
       * floating point value that carries formatting information
       * along with it.
       *
       * When using FormattedDouble in a file record wrapper, you
       * would typically define the value stored in the record as a
       * formatted double and initialize it in the record class'
       * constructor with the
       * FormattedDouble(double,StringUtils::FFLead,unsigned,unsigned,unsigned,char,StringUtils::FFSign,StringUtils::FFAlign)
       * constructor, which defines all the relevant formatting
       * information.  The value is whatever you'd normally initialize
       * it to outside the context of the formatting.  The formatting
       * information at this point is retained for output and input
       * and the value can be treated as any other double.
       * Additionally, you can assign a string to this class and it
       * will interpret it according to the formatting information.
       * @see RinexNavHeader for an example use.
       */
   class FormattedDouble
   {
   public:
         /** Prepare a value for stream output.
          * @param[in] d The value that is to be formatted in an ostream.
          * @param[in] lead How the lead-in to the value is to be formatted.
          * @param[in] mantissa How many digits of precision should be
          *   in the mantissa, e.g. mantissa=5 could result in
          *   something like 1.2345e+00.
          * @param[in] exponent How many digits of precision should be
          *   in the exponent, e.g. exponent=3 could result in
          *   something like 1.2345e+000.  Exponents will always be at
          *   least 2 characters in length.
          * @param[in] width The total number of characters in the
          *   formatted value.  If the length of the formatted value
          *   including mantissa, exponent, sign, etc. is >= width, no
          *   additional formatting will take place.  If the length of
          *   the formatted value is < width, it will be padded with
          *   spaces according to align.
          * @param[in] expChar The character used to designate the
          *   exponent, e.g. "e" or "E" or "D".
          * @param[in] sign How numerical sign is to be handled in formatting.
          * @param[in] align How to pad the formatted value according
          *   to width.  Left adds space to the end of the formatted
          *   value while Right inserts space at the beginning.
          */
      FormattedDouble(double d,
                      StringUtils::FFLead lead,
                      unsigned mantissa = 0, unsigned exponent = 2,
                      unsigned width = 0, char expChar = 'e',
                      StringUtils::FFSign sign = StringUtils::FFSign::NegOnly,
                      StringUtils::FFAlign align = StringUtils::FFAlign::Left);

         /** Prepare for stream input.
          * @param[in] width The total number of characters in the
          *   formatted value.  If the length of the formatted value
          *   including mantissa, exponent, sign, etc. is >= width, no
          *   additional formatting will take place.  If the length of
          *   the formatted value is < width, it will be padded with
          *   spaces according to align.
          * @param[in] expChar The character used to designate the
          *   exponent, e.g. "e" or "E" or "D".
          * @note All other formatting values are quietly ignored on
          *   input.  Only the exponent character is needed to
          *   properly read a formatted value.
          */
      explicit FormattedDouble(unsigned width = 0, char expChar = 'e');

         /// Cast this object to a double for math and such.
      operator double() const
      { return val; }

         /// Assign a value without affecting formatting.
      FormattedDouble& operator=(double d)
      { val = d; return *this; }

         /// Assign a value by decoding a string using existing formatting.
      FormattedDouble& operator=(const std::string& s);

      double val;                     ///< The value as read or to be formatted.
      StringUtils::FFLead leadChar;   ///< Leading non-space character.
      unsigned mantissaLen;           ///< How many digits of mantissa.
      unsigned exponentLen;           ///< How many digits of exponent.
      unsigned totalLen;              ///< Total width of space-padded value.
      char exponentChar;              ///< What character delimits the exponent.
      StringUtils::FFSign leadSign;   ///< How to handle signs for positive val.
      StringUtils::FFAlign alignment; ///< Alignment when padding with space.
   }; // class FormattedDouble


      /// Stream output for FormattedDouble.
   std::ostream& operator<<(std::ostream& s, const FormattedDouble& d);
      /** Stream input for FormattedDouble.
       * @warning While the stream input operator does take into
       *   account the value of totalLen, don't make the mistake of
       *   thinking that C++ stream input will only read that many
       *   characters.  That is, if you have something like
       *   "1.2345e+001234" and a width of 10, you won't get 1.2345.
       *   The lack of the delimiter between "1.2345e+00" and "1234"
       *   will cause C++ to attempt to interpret the 1234 as part of
       *   the exponent.  This is a limitation of C++.  If you need to
       *   work around this limitation, you should probably process
       *   substrings instead. */
   std::istream& operator>>(std::istream& s, FormattedDouble& d);
} // namespace gpstk

#endif // FORMATTEDDOUBLE_HPP
