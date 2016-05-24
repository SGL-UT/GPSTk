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
 * @file ConfDataStructures.hpp
 * 
 */

#ifndef GPSTK_CONFDATASTRUCTURES_HPP
#define GPSTK_CONFDATASTRUCTURES_HPP

#include "DataStructures.hpp"
#include "MemoryUtils.hpp"
#include "CommonTime.hpp"
#include "Triple.hpp"
#include "ConfData.hpp"

namespace gpstk
{

   /** This class is designed to handle config data with great easy.
    *
    * Supported data types:
    *    1) int 
    *    2) double
    *    3) bool
    *    4) std::string
    *    5) std::vector<std::string>
    *    6) std::vector<double>
    *    7) std::vector<int>
    *    3) DayTime
    *    4) Triple
    *
    * A typical way to use this class follows:
    *
    * @code
    *      // data to be stored in the default section
    *    struct globalConf
    *    {
    *       DataType dataFolder;
    *       // ...
    *    }
    *       // data to be stored in every section
    *    struct sourceConf
    *    {
    *       std::string name;          // section name
    *       DataType rinexObsFile;
    *    }
    *
    *    struct PODConfData : public gnssConfData<globalConf,sourceConf>
    *    {
    *    protected:
    *       void defineHeader()
    *       {
    *          // 'bind_f2' 'bind_f3' 'bind_f4' can be called to regist the data
    *          bind_f2("",header.dataFolder);
    *       }
    *
    *       void defineBody(sourceConf& it)
    *       {
    *          bind_f2(it.name,it.rinexObsFile);
    *       }
    *    }
    *
    *       // Then we can save and load all the config data automatically.
    *    PODConfData config;
    *    config.save("test.conf");
    *    config.load("test.conf");
    *
    * @endcode
    *
    * The data stored to the config file is like the following:
    *
    * #  10/16/2011 18:11:13
    * # ==================================================================================================================================
    *
    * dataFolder           = e:/global_10151
    * oceanLoadingFile     = GOT002.BLQ
    * satDataFile          = PRN_GPS
    *
    *
    * # CONFIGURATION DATA FOR 'ABPO' SECTION
    * # ----------------------------------------------------------------------------------------------------------------------------------
    * [ABPO]
    * rinexObsFile         = abpo1510.10o
    * antennaPosition      = 4097216.6434 4429119.0506 -2065771.3263
    * antennaModel         = ASH701945G_M    SCIT
    * offsetARP            = 0.0083 0.0000 0.0000
    * offsetL1             = 0.0886 -0.0004 0.0007
    * offsetL2             = 0.1176 -0.0004 -0.0002
    *
    */

   
#define bind_f2(name,data) \
   {std::vector<std::string> t = StringUtils::split(#data,".->"); \
   bindConfig(name,data,std::string(*t.rbegin()),"","");}

#define bind_f3(name,data,valc) \
   {std::vector<std::string> t = StringUtils::split(#data,".->"); \
   bindConfig(name,data,std::string(*t.rbegin()),"",valc);}

#define bind_f4(name,data,varc,valc) \
   {std::vector<std::string> t = StringUtils::split(#data,".->"); \
   bindConfig(name,data,std::string(*t.rbegin()),varc,valc);}


   /// Attention:
   /// The BODY_CLASS have to be with a member name as the key of config map
   template <class HEADER_CLASS, class BODY_CLASS>
   struct gnssConfData : public gnssData< HEADER_CLASS, std::vector< BODY_CLASS > >
   {
      typedef std::vector< BODY_CLASS > BodyVec;
      
      gnssConfData()
         : config(
              ConfCallBack< gnssConfData<HEADER_CLASS,BODY_CLASS> >
              (*this,
               &gnssConfData<HEADER_CLASS,BODY_CLASS>::defineBodyLoad) ) 
      {
         config.setVariableWidth(20);
         config.setValuePrecision(6);
      }
      
      void setVariableWidth(const int width = 0)
      { config.setVariableWidth(width);}

      void setValuePrecision(const int precision = 6)
      { config.setValuePrecision(precision);}

      gnssConfData& init()
      { config.clear(); return (*this); }
      

      gnssConfData& load(const std::string& fileName)
      {
         init();
         defineHeader();
         config.load(fileName);

         return (*this);
      }

      gnssConfData& save(const std::string& fileName)
      {
         init();
         defineHeader();
         defineBodySave();
         config.save(fileName);
         
         return (*this);
      }

      gnssConfData& keepOnly(const std::string& name)
      {
         BODY_CLASS toKeep;;
         for(BodyVec::iterator it=body.begin();
             it!=body.end();
             ++it)
         {
            if(StringUtils::upperCase(it->name)==StringUtils::upperCase(name))
            {
               toKeep = *it;
            }
         }

         body.clear();
         if(toKeep.name==name) body.push_back(toKeep);

         return (*this);
      }

   protected:

      template<class DataType>
      gnssConfData& bindConfig(const std::string& section,
         DataType& data,
         const std::string& var,
         const std::string& varComment="",
         const std::string& valComment="")
      {
         ConfDataItemAbstract* pitem = 
            new ConfDataItem<DataType>(data,       var, "",
            varComment, valComment);

         itemPool.add(pitem); 

         string name = section;
         if(name=="") name = ConfData< gnssConfData<HEADER_CLASS,BODY_CLASS> >::Default;

         config.add_section(name);
         config.append_section(pitem,name);

         return (*this);
      }

      // The following method have to be overrided
      virtual void defineHeader() {}
      virtual void defineBody(BODY_CLASS& item){}

      void defineBodySave()
      {
         for(BodyVec::iterator it=body.begin();
             it!=body.end();
             ++it)
         { defineBody(*it); }
      }

      void defineBodyLoad(const std::string& name)
      {
         BODY_CLASS temp;
         temp.name = name;

         body.push_back(temp);

         defineBody(*body.rbegin());
      }

         // An graceful way to handle the pointers
      AutoReleasePool< ConfDataItemAbstract,
                       ReleasePolicy<ConfDataItemAbstract> > itemPool;

      ConfData< gnssConfData<HEADER_CLASS,BODY_CLASS> > config;
   };
      
}   // End of namespace gpstk


#endif  //GPSTK_CONFDATASTRUCTURES_HPP
