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
 * @file ConfDataSection.hpp
 * 
 */

#ifndef GPSTK_CONFDATASECTION_HPP
#define GPSTK_CONFDATASECTION_HPP

#include <string>
#include <vector>
#include "ConfDataItem.hpp"

namespace gpstk
{
      /** This class ...
       *
       */
   class ConfDataSection
   {
   public:
      typedef std::vector< ConfDataItemAbstract* > ItemSet;
      typedef ItemSet::iterator Iterator;

   public:
      ConfDataSection(const std::string& desc="")
         : comment(desc){}
   
      virtual ~ConfDataSection(){}

      Iterator begin(){return dataSet.begin();}

      Iterator end(){return dataSet.end();}

      ConfDataSection& clear()
      { dataSet.clear(); return (*this); }

      ConfDataSection& insert(ConfDataItemAbstract* di)
      { dataSet.push_back(di); return (*this); }

      ConfDataSection& insert(const ConfDataSection& s2)
      {
         ConfDataSection ts(s2);
         for(Iterator it = ts.begin(); it!=ts.end(); it++)
         {
            insert(*it);
         }

         return (*this);
      }
      
      ConfDataSection& remove(const std::string& var)
      {
         ConfDataSection s2;
         for(Iterator it = begin(); it!=end(); it++)
         {
            if((*it)->get_var()!=var) s2.insert(*it);
         }

         clear().insert(s2);

         return (*this);
      }
      

      bool exists(const std::string& var)
      {
         bool found(false);

         for(Iterator it = begin(); it!=end(); it++)
         {
            if( (*it)->get_var()==var)
            {
               found = true;
               break;
            }
         }
      
         return found;
      }
      
      ConfDataItemAbstract* item(const std::string& var)
      {
         bool found(false);

         for(Iterator it = begin(); it!=end(); it++)
         {
            if((*it)->get_var()==var)
            {
               return (*it);
            }
         }

         return 0;
      }

   public:
      std::string  comment;

   protected:
      ItemSet dataSet;
         
   };   // End of class 'ConfDataSection'
   
   typedef ConfDataSection ConfDataItemSet;

}   // End of namespace gpstk


#endif  //GPSTK_CONFDATAITEMSET_HPP
