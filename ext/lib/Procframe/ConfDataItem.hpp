//============================================================================
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
//  Copyright 2004, The University of Texas at Austin
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file ConfDataItem.hpp
 * 
 */

#ifndef GPSTK_CONFDATAITEM_HPP
#define GPSTK_CONFDATAITEM_HPP

#include <string>
#include <sstream>
#include <vector>
#include "StringUtils.hpp"
#include "Triple.hpp"
#include "Vector.hpp"
#include "CommonTime.hpp"

namespace gpstk
{

      /** This class ...
       *
       */
   class ConfDataItemAbstract   
   {
   public:
      
      ConfDataItemAbstract(){init();}

      ConfDataItemAbstract(const std::string& var, 
                           const std::string& val, 
                           const std::string& varComment = "",
                           const std::string& valComment = "")
      {
         this->var = var; this->varcomment = varComment;
         this->val = val; this->valcomment = valComment; 
      }

      ConfDataItemAbstract(const ConfDataItemAbstract& right)
      {
         this->var = right.var; this->varcomment = right.varcomment;
         this->val = right.val; this->valcomment = right.valcomment;
      }

      ConfDataItemAbstract& operator=(const ConfDataItemAbstract& right)
      {
         this->var = right.var; this->varcomment = right.varcomment;
         this->val = right.val; this->valcomment = right.valcomment;

         return (*this);
      }
      
      virtual ~ConfDataItemAbstract(){}
      
      std::string get_var()
      { return var;}

      ConfDataItemAbstract& set_var(std::string s)
      { var = s; return (*this); }

      std::string get_var_comment()
      { return varcomment;}

      ConfDataItemAbstract& set_var_comment(std::string s)
      { varcomment = s; return (*this); }

      virtual std::string get_val()
      { return val;}
      
      virtual ConfDataItemAbstract& set_val(std::string s)
      { val = s; return (*this); }

      std::string get_val_comment()
      { return valcomment;}

      ConfDataItemAbstract& set_val_comment(std::string s)
      { valcomment = s; return (*this); }
   
   protected:
      void init()
      { var = varcomment = ""; val = valcomment = ""; }

   protected:
      std::string  var, varcomment;
      std::string  val, valcomment;

   };   // End of class 'ConfDataItemAbstract'
   
   template<class DataType = std::string>
   class ConfDataItem : public ConfDataItemAbstract
   {
   public:
      ConfDataItem(DataType& dat,
                   const std::string& var, 
                   const std::string& val, 
                   const std::string& varComment = "",
                   const std::string& valComment = "") 
         : ConfDataItemAbstract(var,val,varComment,valComment),
           data(dat)
      {
      }

      virtual ~ConfDataItem(){}
      
      std::string get_val()
      { return format(data);}

      ConfDataItemAbstract& set_val(std::string s)
      { parse(s,data); return (*this); }

      // implement the format functions for every DataType
      std::string format(const std::string& d)
      { return d;}
      
      std::string format(const int& d)
      { return StringUtils::asString(d); }

      std::string format(const double& d,int p=4)
      { return StringUtils::asString(d,p);}

      std::string format(const bool& d)
      { return d ? "TRUE" : "FALSE"; }

      std::string format(const CommonTime& d)
      {
         CivilTime c(d);
         return StringUtils::asString(c.year) + " "
              + StringUtils::asString(c.month) + " "
              + StringUtils::asString(c.day) + " "
              + StringUtils::asString(c.hour) + " "
              + StringUtils::asString(c.minute) + " "
              + StringUtils::asString(c.second,3);
      }

      std::string format(const Triple& d)
      { 
         return StringUtils::asString(d[0],4)+" "
               +StringUtils::asString(d[1],4)+" "
               +StringUtils::asString(d[2],4);
      }

      std::string format(const std::vector<int>& d)
      {
         std::string s;

         std::vector<int>::const_iterator it;
         for(it = d.begin(); it!=d.end(); ++it)
         {
            s += StringUtils::asString(*it) + " ";
         }

         return StringUtils::strip(s);
      }

      std::string format(const std::vector<double>& d)
      {
         std::string s;

         std::vector<double>::const_iterator it;
         for(it = d.begin(); it!=d.end(); ++it)
         {
            s += StringUtils::asString(*it, 4) + " ";
         }

         return StringUtils::strip(s);
      }

      std::string format(const std::vector<std::string>& d)
      {
         std::string s;

         std::vector<std::string>::const_iterator it;
         for(it = d.begin(); it!=d.end(); ++it)
         {
            string ss(*it);
            s += StringUtils::strip(ss) + " ";
         }

         return StringUtils::strip(s);
      }

      // implement the format functions for every DataType
      void parse(const std::string& in,std::string& out)
      { out = in; }

      void parse(const std::string& in,int& out)
      {
         if(!StringUtils::isDigitString(in))
         {
            Exception e("Trying to parsing int from a non-digital string '"
                         + in + "'." );
            GPSTK_THROW(e);
         }

         out = StringUtils::asInt(in);
      }

      void parse(const std::string& in,double& out)
      { out = StringUtils::asDouble(in); }

      void parse(const std::string& in,bool& out)
      {
         if(StringUtils::upperCase(in)=="TRUE")
         { 
            out = true; 
         }
         else if(StringUtils::upperCase(in)=="FALSE")
         {
            out = false;
         }
         else  // it's neither TRUE or FALSE
         {
            GPSTK_THROW(
               Exception("Failed to parse a boolean value from "+in
                         +", and it should be 'TRUE' or 'FALSE'.") );

            //out = false;
         }
      }

      void parse(const std::string& in,CommonTime& out)
      {
         int year,month,day,hour,minute; double second;
         istringstream s(in);
         s >> year >> month >> day >> hour >> minute >> second;    
         
         CivilTime c(year,month,day,hour,minute,second);
         out = c.convertToCommonTime();
      }

      void parse(const std::string& in,Triple& out)
      {
         istringstream s(in);
         s >> out[0] >> out[1] >> out[2];
      }

      void parse(const std::string& in,std::vector<int>& out)
      {
         out.clear();

         std::string ss(in);
         std::string firstWord = StringUtils::stripFirstWord(ss);
         while(StringUtils::strip(firstWord)!="")
         {
            out.push_back(StringUtils::asInt(firstWord));

            ss = StringUtils::strip(ss);
            firstWord = StringUtils::stripFirstWord(ss);
         }
      }

      void parse(const std::string& in,std::vector<double>& out)
      {
         out.clear();

         std::string ss(in);
         std::string firstWord = StringUtils::stripFirstWord(ss);
         while(StringUtils::strip(firstWord)!="")
         {
            out.push_back(StringUtils::asDouble(firstWord));

            ss = StringUtils::strip(ss);
            firstWord = StringUtils::stripFirstWord(ss);
         }
      }

      void parse(const std::string& in,std::vector<std::string>& out)
      {
         out.clear();
         
         std::string ss(in);
         std::string firstWord = StringUtils::stripFirstWord(ss);
         while(StringUtils::strip(firstWord)!="")
         {
            out.push_back(firstWord);
            
            ss = StringUtils::strip(ss);
            firstWord = StringUtils::stripFirstWord(ss);
         }
      }

   protected:
      ConfDataItem(){init();}

      DataType& data;
      DataType  data0;
   };

}   // End of namespace gpstk

/*
bool operator==(const ConfDataItemAbstract& right) const
{
if( (var==right.var) && (val==right.val) )
{
return true;
}
else
{
return false;
}
}

bool operator!=(const ConfDataItemAbstract& right) const
{
return !(operator==(right)); 
}

bool operator<(const ConfDataItemAbstract& right) const
{
if(var==right.var)
{
return val < right.val;
}
else
{
return var < right.val;
}
}

bool operator>(const ConfDataItemAbstract& right) const
{
return (!operator<(right) && !operator==(right)); 
}

bool operator<=(const ConfDataItemAbstract& right) const
{
return (operator<(right) || operator==(right)); 
}

bool operator>=(const ConfDataItemAbstract& right) const
{
return !(operator<(right)); 
}

*/

#endif  //GPSTK_CONFDATAITEM_HPP
