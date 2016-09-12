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
* @file ConfData.hpp
* 
*/

#ifndef GPSTK_CONFDATA_HPP
#define GPSTK_CONFDATA_HPP

#include <string>
#include <map>

#include "CommonTime.hpp"
#include "ConfDataSection.hpp"
#include "ConfDataReader.hpp"
#include "ConfDataWriter.hpp"


namespace gpstk
{
   template <class HEADER_CLASS, class BODY_CLASS>
   struct gnssConfData;

   template <class C>
   class ConfCallBack
   {
   public:
      typedef void (C::*Callback)(const std::string&);

      ConfCallBack(C& object, Callback method): _pObject(&object), _method(method){}

      ConfCallBack(const ConfCallBack& ra): _pObject(ra._pObject), _method(ra._method){}

      ~ConfCallBack(){}

      ConfCallBack& operator = (const ConfCallBack& ra)
      {
         _pObject = ra._pObject;
         _method  = ra._method;
         return *this;
      }

      void addSection(const std::string& name){(_pObject->*_method)(name); }

   private:
      ConfCallBack();

      C*       _pObject;
      Callback _method;
   };

   template<class C>
   class ConfData   
   {
   public:
      typedef std::map<std::string,ConfDataSection> SectionMap;
      typedef SectionMap::iterator Iterator;

   public:
      ConfData(const ConfCallBack<C>& callback, const std::string& desc="")
         : callBack(callback),comment(desc),variableWidth(20),valuePrecison(6) 
      { init();}

      virtual ~ConfData(){}

      ConfData& set_comment(const std::string& desc="")
      { comment = desc; return (*this); }

      ConfData& clear(){return init();}

      bool has_section(const std::string& name = Default)
      { return (dataMap.find(name)!=dataMap.end()) ? true : false; }

      ConfDataSection* get_section(const std::string& name = Default)
      {
         SectionMap::iterator it = dataMap.find(name);
         return (it!=dataMap.end()) ? &(it->second) : 0;
      }

      ConfData& add_section(const std::string& name = Default)
      { return add_section(ConfDataSection(),name); }

      ConfData& add_section(const ConfDataItemSet& items,
                            const std::string& name = Default)
      {
         if(dataMap.find(name)==dataMap.end()) dataMap[name] = items;

         return (*this);
      }

      ConfData& append_section(ConfDataItemAbstract* item, 
                               const std::string& name = Default)
      {
         if(has_section(name)) get_section(name)->insert(item);    // add to specific section
         else                  get_section()->insert(item);        // add to default section
         
         return (*this);
      }

      ConfData& append_section(const ConfDataItemSet& items, 
                               const std::string& name = Default)
      {
         if(has_section(name)) get_section(name)->insert(items);  // add to specific section
         else                  get_section()->insert(items);      // add to default section

         return (*this);
      }

      Iterator begin() {return dataMap.begin();}

      Iterator end() {return dataMap.end();}

      inline ConfData& save(const std::string& fileName);

      inline ConfData& load(const std::string& fileName);
      
      void setVariableWidth(const int width = 0)
      { variableWidth = width;}

      void setValuePrecision(const int precision = 6)
      { valuePrecison = precision;}

   protected:

      ConfData& init()
      { dataMap.clear(); return add_section();}

   protected:
      SectionMap dataMap;

      std::string comment;

      ConfCallBack<C> callBack;

      int variableWidth;
      int valuePrecison;

   public:
      static const std::string Default;

   };   // End of class 'ConfData'

   //////////////////////////////////////////////////////////////////////////

   template<class C>
   const std::string ConfData<C>::Default = "DEFAULT";

   template<class C>
   inline ConfData<C>& ConfData<C>::save(const std::string& fileName)
   {
      ConfDataWriter confWriter(fileName);
      confWriter.setVariableWidth(variableWidth);
      confWriter.setValuePrecision(variableWidth);

      DayTime now;
      confWriter.writeCommentLine(comment + " " + now.asString());
      confWriter.writeSeparatorLine("=");
      confWriter.writeBlankLine();

      // write default
      Iterator it = dataMap.find(Default);
      if(it!=dataMap.end())
      {
         for(ConfDataSection::Iterator it2 = it->second.begin();
             it2!=it->second.end();
             ++it2)
         {
            ConfDataItemAbstract* item = (*it2);

            confWriter.writeVariable(item->get_var(),
               item->get_val(),
               item->get_var_comment(),
               item->get_val_comment() );
         }

         confWriter.writeBlankLine();
      }

      // write other section
      for(Iterator it = begin(); it!=end(); it++)
      {
         ConfDataItemSet& section = it->second;

         if(it->first==Default) continue;

         confWriter.writeSection(it->first,section.comment);

         for(ConfDataSection::Iterator it2 = section.begin();
             it2!=section.end();
             ++it2)
         {
            ConfDataItemAbstract* item = (*it2);

            confWriter.writeVariable(item->get_var(),
               item->get_val(),
               item->get_var_comment(),
               item->get_val_comment() );
         }

         confWriter.writeBlankLine();
      }

      confWriter.writeBlankLine();
      confWriter.writeEnd();

      confWriter.close();

      return (*this);

   }  // End of method 'ConfData<C>& ConfData<C>::save()'

   template<class C>
   inline ConfData<C>& ConfData<C>::load(const std::string& fileName)
   {
      try
      {
         ConfDataReader confReader;
         confReader.open(fileName);
         confReader.setFallback2Default(true);

         // read default section
         ConfDataSection* sec = get_section(Default);

         for(ConfDataSection::Iterator it = sec->begin();
            it!=sec->end();
            ++it)
         {
            ConfDataItemAbstract* item = (*it);

            string var = item->get_var();

            item->set_val(confReader.getValue(var));
            item->set_val_comment(confReader.getValueDescription(var));
            item->set_var_comment(confReader.getVariableDescription(var));
         }

         // read other section
         std::string name;
         while ( (name = confReader.getEachSection()) != "" )
         {
            if(name==Default) continue;      // skip default section

            callBack.addSection(name);

            ConfDataSection* sec = get_section(name);

            for(ConfDataSection::Iterator it = sec->begin();
               it!=sec->end();
               ++it)
            {
               ConfDataItemAbstract* item = (*it);

               string var = item->get_var();

               item->set_val(confReader.getValue(var,name));
               item->set_val_comment(confReader.getValueDescription(var,name));
               item->set_var_comment(confReader.getVariableDescription(var,name));
            }
         }

         confReader.close();
      }
      catch(Exception& e)
      {
         Exception E("Error loading the file '"
            + fileName + "', " + string(e.what())); 
         GPSTK_THROW(E);
      }
      catch(exception& e)
      {
         Exception E("Error loading the file '"
            + fileName + "', " + string(e.what())); 
         GPSTK_THROW(E);
      }
      catch(...)
      {
         Exception E("Error loading the file '"
            + fileName + "', " + string("Unknown error.")); 

         GPSTK_THROW(E);
      }

      return (*this);

   }  // End of method 'ConfData<C>& ConfData<C>::load()'

}   // End of namespace gpstk


#endif  //GPSTK_CONFDATA_HPP
