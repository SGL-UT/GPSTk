#include "FormattedDouble.hpp"

namespace gpstk
{
   FormattedDouble ::
   FormattedDouble(double d, StringUtils::FFLead lead, unsigned mantissa,
                   unsigned exponent, unsigned width, char expChar,
                   StringUtils::FFSign sign, StringUtils::FFAlign align)
         : val(d), leadChar(lead), mantissaLen(mantissa), exponentLen(exponent),
           totalLen(width), exponentChar(expChar), leadSign(sign),
           alignment(align)
   {
   }


   FormattedDouble ::
   FormattedDouble(unsigned width, char expChar)
         : val(0), leadChar(StringUtils::FFLead::NonZero), mantissaLen(0),
           exponentLen(2), totalLen(width), exponentChar(expChar),
           leadSign(StringUtils::FFSign::NegOnly),
           alignment(StringUtils::FFAlign::Left)
   {
   }


   FormattedDouble ::
   FormattedDouble(const std::string& str, unsigned width, char expChar)
         : val(0), leadChar(StringUtils::FFLead::NonZero), mantissaLen(0),
           exponentLen(2), totalLen(width), exponentChar(expChar),
           leadSign(StringUtils::FFSign::NegOnly),
           alignment(StringUtils::FFAlign::Left)
   {
      this->operator=(str);
   }


   FormattedDouble& FormattedDouble ::
   operator=(const std::string& s)
   {
      if ((exponentChar != 'e') && (exponentChar != 'E'))
      {
            // If the exponent character is different from standard,
            // we need to do some tweaking.
            // have to make a copy to modify
         std::string copy(s);
         std::string::size_type pos = copy.find(exponentChar);
         if (pos != std::string::npos)
         {
            copy[pos] = 'e'; // change exponent character to a readable one
         }
         std::istringstream iss(copy);
         iss >> val;
      }
      else
      {
            // If the exponent character is standard, the standard
            // istream double reading should work fine regardless of
            // any other tweaks.
         std::istringstream iss(s);
         iss >> val;
      }
      return *this;
   }


   std::ostream& operator<<(std::ostream& s, const FormattedDouble& d)
   {
      s << StringUtils::floatFormat(d.val, d.leadChar, d.mantissaLen,
                                    d.exponentLen, d.totalLen, d.exponentChar,
                                    d.leadSign, d.alignment);
      return s;
   }


   std::istream& operator>>(std::istream& s, FormattedDouble& d)
   {
      if (d.totalLen)
         s >> std::setw(d.totalLen);
      if ((d.exponentChar != 'e') && (d.exponentChar != 'E'))
      {
            // If the exponent character is different from standard,
            // we need to do some tweaking.
         std::string str;
         s >> str;
         std::string::size_type pos = str.find(d.exponentChar);
         if (pos != std::string::npos)
         {
            str[pos] = 'e'; // change exponent character to a readable one
         }
         std::istringstream iss(str);
         iss >> d.val;
      }
      else
      {
            // If the exponent character is standard, the standard
            // istream double reading should work fine regardless of
            // any other tweaks.
         s >> d.val;
      }
      return s;
   }


   void FormattedDouble ::
   dump(std::ostream& s) const
   {
      s << "FormattedDouble(" << val << "," << static_cast<int>(leadChar) << ","
        << mantissaLen << "," << exponentLen << "," << totalLen << ","
        << exponentChar << "," << static_cast<int>(leadSign) << ","
        << static_cast<int>(alignment) << ")" << std::endl;
   }
} // namespace gpstk
