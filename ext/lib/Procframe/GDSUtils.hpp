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
* @file GDSUtils.hpp
* 
*/

#ifndef GPSTK_GDSUTILS_HPP
#define GPSTK_GDSUTILS_HPP

#include <fstream>

#include "DataStructures.hpp"

namespace gpstk
{
   /**
    * Methods to exchange data between gnssDataMap and binary file 
    */

   template<class DataType>
   struct DataBin
   {
      DataType data;

      DataBin() {}

      DataBin(const DataType& d) : data(d) {}
      
      DataBin(const DataType& d,std::ostream& s) : data(d) { write(s); }
      
      DataBin(std::istream& s) { read(s); }
      
      DataType get() { return data; }

      void set(const DataType& d) { data = d;}

      DataBin operator=(const DataBin& right)
      { data = right.data; return (*this); }

      virtual void write(std::ostream& s)
      { s.write((char*)&data,sizeof(DataType));}

      virtual void read(std::istream& s)
      { s.read((char*)&data,sizeof(DataType));}
   };
 
   struct StringBin : DataBin<std::string>
   {
      StringBin() : DataBin(){}

      StringBin(const std::string& str) : DataBin(str){}

      StringBin(const std::string& str,std::ostream& s) 
         : DataBin(str) { write(s); }

      StringBin(std::istream& s) { read(s); }

      void write(std::ostream& s)
      {          
         DataBin<size_t> sizeBin(data.length(),s);
         
         s.write(&data[0],sizeof(char)*sizeBin.get());
      }

      void read(std::istream& s)
      { 
         DataBin<size_t> sizeBin(s);

         data.resize(sizeBin.get(),' ');
         s.read(&data[0],sizeof(char)*sizeBin.get());
      }
   };

   struct TypeIDBin : DataBin<TypeID>
   {
      TypeIDBin() : DataBin(){}

      TypeIDBin(const TypeID& type) : DataBin(type){}

      TypeIDBin(const TypeID& type,std::ostream& s) 
         : DataBin(type) { write(s); }

      TypeIDBin(std::istream& s) { read(s); }

      void write(std::ostream& s)
      { 
         DataBin<TypeID::ValueType> dBin(data.type,s);
      }
   
      void read(std::istream& s)
      { 
         DataBin<TypeID::ValueType> dBin(s);
         
         data.type = dBin.get();
      }
   };

   struct SourceIDBin : DataBin<SourceID>
   {
      SourceIDBin() : DataBin(){}

      SourceIDBin(const SourceID& source) : DataBin(source){}

      SourceIDBin(const SourceID& source,std::ostream& s) 
         : DataBin(source) { write(s); }

      SourceIDBin(std::istream& s) { read(s); }

      void write(std::ostream& s)
      { 
         DataBin<SourceID::SourceType> typeBin(data.type,s);
         StringBin nameBin(data.sourceName,s);
      }

      void read(std::istream& s)
      { 
         DataBin<SourceID::SourceType> typeBin(s);
         StringBin nameBin(s);

         data.type = typeBin.get();
         data.sourceName = nameBin.get();
      }
   };

   struct SatIDBin : DataBin<SatID>
   {
      SatIDBin() : DataBin(){}

      SatIDBin(const SatID& sat) : DataBin(sat){}

      SatIDBin(const SatID& sat,std::ostream& s) : DataBin(sat) { write(s); }

      SatIDBin(std::istream& s) { read(s); }

      void write(std::ostream& s)
      {  
         DataBin<int> idBin(data.id,s);
         DataBin<SatID::SatelliteSystem> sysBin(data.system,s);
      }

      void read(std::istream& s)
      { 
         DataBin<int> idBin(s);
         DataBin<SatID::SatelliteSystem> sysBin(s);
         
         data.id = idBin.get();
         data.system = sysBin.get();
      }
   };


   struct DayTimeBin : DataBin<DayTime>
   {
      DayTimeBin() : DataBin(){}

      DayTimeBin(const DayTime& time) : DataBin(time){}

      DayTimeBin(const DayTime& time,std::ostream& s) 
         : DataBin(time) { write(s); }

      DayTimeBin(std::istream& s) { read(s); }

      void write(std::ostream& s)
      { 
         DataBin<int> yearBin(data.year(),s);
         DataBin<int> monthBin(data.month(),s);
         DataBin<int> dayBin(data.day(),s);
         DataBin<int> hourBin(data.hour(),s);
         DataBin<int> minuteBin(data.minute(),s);
         DataBin<double> secondBin(data.second(),s);
      }

      void read(std::istream& s)
      { 
         DataBin<int> yearBin(s);
         DataBin<int> monthBin(s);
         DataBin<int> dayBin(s);
         DataBin<int> hourBin(s);
         DataBin<int> minuteBin(s);
         DataBin<double> secondBin(s);

         data = DayTime(yearBin.get(),monthBin.get(),dayBin.get(),
                        hourBin.get(),minuteBin.get(),secondBin.get());
      }
   };

   struct typeValueMapBin : DataBin<typeValueMap>
   {
      typeValueMapBin() : DataBin(){}

      typeValueMapBin(const typeValueMap& tvMap) : DataBin(tvMap){}

      typeValueMapBin(const typeValueMap& tvMap,std::ostream& s)
         : DataBin(tvMap) { write(s); }

      typeValueMapBin(std::istream& s) { read(s); }

      void write(std::ostream& s)
      { 
         DataBin<size_t> sizeBin(data.size(),s);

         for( typeValueMap::const_iterator it = data.begin();
              it != data.end();
              ++it )
         {
            TypeIDBin typeb(it->first,s);
            DataBin<double> valueb(it->second,s);
         }
      }

      void read(std::istream& s)
      { 
         data.clear();

         DataBin<size_t> sizeBin(s);

         for(size_t i=0; i<sizeBin.get(); i++)
         {
            TypeIDBin typeb(s);
            DataBin<double> valueb(s);

            data[typeb.get()] = valueb.get();
         }
      }
   };


   struct satTypeValueMapBin : DataBin<satTypeValueMap>
   {
      satTypeValueMapBin() : DataBin(){}

      satTypeValueMapBin(const satTypeValueMap& stvMap) : DataBin(stvMap){}

      satTypeValueMapBin(const satTypeValueMap& stvMap,std::ostream& s) 
         : DataBin(stvMap) { write(s); }

      satTypeValueMapBin(std::istream& s) { read(s); }

      void write(std::ostream& s)
      { 
         DataBin<size_t> sizeBin(data.size(),s);
       
         for( satTypeValueMap::const_iterator it = data.begin();
              it != data.end();
              ++it )
         {
            SatIDBin satBin(it->first,s);
            typeValueMapBin tvMapBin(it->second,s);
         }
      }

      void read(std::istream& s)
      { 
         data.clear();

         DataBin<size_t> sizeBin(s);

         for(size_t i=0; i<sizeBin.get(); i++)
         {
            SatIDBin satBin(s);
            typeValueMapBin tvMapBin(s);

            data[satBin.get()] = tvMapBin.get();
         }
      }
   };

   struct sourceDataMapBin : DataBin<sourceDataMap>
   {
      sourceDataMapBin() : DataBin(){}

      sourceDataMapBin(const sourceDataMap& sdMap) : DataBin(sdMap){}

      sourceDataMapBin(const sourceDataMap& sdMap,std::ostream& s) 
         : DataBin(sdMap) { write(s); }

      sourceDataMapBin(std::istream& s) { read(s); }

      void write(std::ostream& s)
      { 
         DataBin<size_t> sizeBin(data.size(),s);

         sourceDataMap::const_iterator it;
         for( it = data.begin(); it != data.end(); it++ )
         {
            SourceIDBin sourceBin(it->first);
            satTypeValueMapBin stvMapBin(it->second);

            sourceBin.write(s);
            stvMapBin.write(s);
         }
      }

      void read(std::istream& s)
      { 
         data.clear();

         DataBin<size_t> sizeBin(s);

         for(size_t i=0; i<sizeBin.get(); i++)
         {
            SourceIDBin sourceBin(s);
            satTypeValueMapBin stvMapBin(s);

            data[sourceBin.get()] = stvMapBin.get();
         }
      }
   };

   struct gnssDataMapBin : DataBin<gnssDataMap>
   {
      gnssDataMapBin() : DataBin(){}

      gnssDataMapBin(const gnssDataMap& gdMap) : DataBin(gdMap){}

      gnssDataMapBin(const gnssDataMap& gdMap,std::ostream& s) 
         : DataBin(gdMap) { write(s); }

      gnssDataMapBin(std::istream& s) { read(s); }

      void write(std::ostream& s)
      {
         DataBin<size_t> sizeBin(data.size(),s);

         gnssDataMap::const_iterator it;
         for(it=data.begin();it!=data.end();it++)
         {
            DayTimeBin timeBin(it->first,s);
            sourceDataMapBin sdMapBin(it->second,s); 
         }
      }

      void read(std::istream& s)
      {
         data.clear();

         DataBin<size_t> sizeBin(s);

         for(size_t i=0; i<sizeBin.get(); i++)
         {
            DayTimeBin timeBin(s);
            sourceDataMapBin sdMapBin(s);

            data.insert(std::pair<const DayTime,sourceDataMap>(timeBin.get(),
                                                               sdMapBin.get()));
         }
      }
   };

      /// Save the data of a gnssDataMap object to specific binary file
   void saveGnssDataMap(const gnssDataMap& gdsMap,const std::string& file)
   {
      ofstream ofs(file.c_str(),ios::binary);

      gnssDataMapBin gdsMapBin(gdsMap,ofs);

      return;
   }

      /// Load the data of gnssDataMap object from a binary file
   gnssDataMap loadGnssDataMap(const std::string& file)
   {
      ifstream ifs(file.c_str(),ios::binary);

      gnssDataMapBin gdsMapBin(ifs);

      return gdsMapBin.get();
   }

      /// Print the data in a plain text file
   void dumpGnssDataMap(const gnssDataMap& gdsMap,const std::string& file)
   {
      ofstream s(file.c_str());
      s<<fixed;

      const string timeFormat = "%04Y %02m %02d %02H %02M %06.3f";
      const string indentSpace = StringUtils::leftJustify("",4);

      gnssDataMap dataMap(gdsMap);

      while(!dataMap.empty())
      {
         gnssDataMap data = dataMap.frontEpoch();

         DayTime time(data.begin()->first);

         s << StringUtils::leftJustify(time.printf(timeFormat),23) << " {" << endl;

         gnssDataMap::const_iterator it;
         for(it=data.begin();it!=data.end();it++)
         {
            const sourceDataMap& sourceMap(it->second);
            for(sourceDataMap::const_iterator itsrc = sourceMap.begin();
                itsrc != sourceMap.end();
                ++itsrc)
            {
               SourceID source(itsrc->first);
               s << indentSpace <<"receiver: "<< source << " {" << endl;
               
               const satTypeValueMap& stvMap(itsrc->second);
               for(satTypeValueMap::const_iterator itsat = stvMap.begin();
                   itsat != stvMap.end();
                   ++ itsat)
               {
                  SatID satellite(itsat->first);
                  s << indentSpace << indentSpace <<"satellite: "<< satellite <<" {"<<endl;

                  const typeValueMap& tvMap(itsat->second);
                  for(typeValueMap::const_iterator ittyp=tvMap.begin();
                      ittyp != tvMap.end();
                      ++ittyp)
                  {
                     s << indentSpace<<indentSpace<<indentSpace;
                     s << setw(10) << ittyp->first << " " 
                       << setw(18)<< setprecision(6) << ittyp->second << endl;
                  }
                  
                  s << indentSpace << indentSpace << "}" << endl;
                  
               }  // loop in the satellite

               s << indentSpace <<"}"<<endl;

            }  // loop in the sources

         }  // loop in the epoches
         
         s << "}" << endl;    // Epoch

         dataMap.pop_front_epoch();
      }

      s.close();
   }

   gnssDataMap mergeGnssDataMap(const gnssDataMap& gdsMap1,
                                const gnssDataMap& gdsMap2 )
   {
      gnssDataMap gdsMap(gdsMap1);

      // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = gdsMap2.begin();
           it != gdsMap2.end();
           ++it )
      {
         const DayTime& time(it->first);
         const sourceDataMap& sourceMap(it->second);

         for(sourceDataMap::const_iterator itsrc = sourceMap.begin();
             itsrc != sourceMap.end();
             ++itsrc)
         {
            gnssSatTypeValue gds;
            gds.header.epoch = time;
            gds.header.source = itsrc->first;
            gds.body = itsrc->second;

            gdsMap.addGnssSatTypeValue(gds);

         }  // loop in the sources

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

      return gdsMap;
   }

   // source id

   gnssDataMap extractSourceID(const gnssDataMap& gdsMap,
                               const SourceIDSet& sourceSet)
   {
      gnssDataMap dataMap;

      // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = gdsMap.begin();
           it != gdsMap.end();
           ++it )
      {
         const DayTime& time(it->first);
         const sourceDataMap& sourceMap(it->second);

         for(sourceDataMap::const_iterator itsrc = sourceMap.begin();
             itsrc != sourceMap.end();
             ++itsrc)
         {
            SourceIDSet::const_iterator itsrc2 = sourceSet.find(itsrc->first);
            if(itsrc2!=sourceSet.end())
            {
               gnssSatTypeValue gds;
               gds.header.epoch = time;
               gds.header.source = itsrc->first;
               gds.body = itsrc->second;

               dataMap.addGnssSatTypeValue(gds);
            }

         }  // loop in the sources

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

      return dataMap;
   }

   gnssDataMap extractSourceID(const gnssDataMap& gdsMap,
      const SourceID& source)
   {
      SourceIDSet sourceSet;
      sourceSet.insert(source);

      return extractSourceID(gdsMap,sourceSet);
   }

   gnssDataMap keepOnlySourceID(gnssDataMap& gdsMap,
                                const SourceIDSet& sourceSet)
   {
      gdsMap = extractSourceID(gdsMap,sourceSet);
      return gdsMap;
   }

   gnssDataMap keepOnlySourceID(gnssDataMap& gdsMap,
                                const SourceID& source)
   {
      gdsMap = extractSourceID(gdsMap,source);
      return gdsMap;
   }

   gnssDataMap removeSourceID(const gnssDataMap& gdsMap,
                              const SourceIDSet& sourceSet)
   {
      gnssDataMap dataMap;

      // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = gdsMap.begin();
           it != gdsMap.end();
           ++it )
      {
         const DayTime& time(it->first);
         const sourceDataMap& sourceMap(it->second);

         for(sourceDataMap::const_iterator itsrc = sourceMap.begin();
             itsrc != sourceMap.end();
             ++itsrc)
         {
            SourceIDSet::const_iterator itsrc2 = sourceSet.find(itsrc->first);
            if(itsrc2==sourceSet.end())
            {
               gnssSatTypeValue gds;
               gds.header.epoch = time;
               gds.header.source = itsrc->first;
               gds.body = itsrc->second;

               dataMap.addGnssSatTypeValue(gds);
            }

         }  // loop in the sources

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

      return dataMap;
   }

   gnssDataMap removeSourceID(const gnssDataMap& gdsMap,
                              const SourceID& source)
   {
      SourceIDSet sourceSet;
      sourceSet.insert(source);

      return removeSourceID(gdsMap,sourceSet);
   }

   // sat id
   gnssDataMap extractSatID(const gnssDataMap& gdsMap,
                            const SatIDSet& satSet)
   {
      gnssDataMap dataMap;

      // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = gdsMap.begin();
           it != gdsMap.end();
           ++it )
      {
         const DayTime& time(it->first);
         const sourceDataMap& sourceMap(it->second);

         for(sourceDataMap::const_iterator itsrc = sourceMap.begin();
             itsrc != sourceMap.end();
             ++itsrc)
         {
            gnssSatTypeValue gds;
            gds.header.epoch = time;
            gds.header.source = itsrc->first;
            gds.body = itsrc->second;

            gds.body.keepOnlySatID(satSet);

            dataMap.addGnssSatTypeValue(gds);

         }  // loop in the sources

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

      return dataMap;
   }

   gnssDataMap extractSatID(const gnssDataMap& gdsMap,
                            const SatID& sat)
   {
      SatIDSet satSet;
      satSet.insert(sat);
      
      return extractSatID(gdsMap,satSet);
   }

   gnssDataMap keepOnlySatID(gnssDataMap& gdsMap,
                             const SatID& sat)
   {
      gdsMap =  extractSatID(gdsMap,sat);
      return gdsMap;
   }

   gnssDataMap keepOnlySatID(gnssDataMap& gdsMap,
                             const SatIDSet& satSet)
   {
      gdsMap = extractSatID(gdsMap,satSet);
      return gdsMap;
   }
  
   gnssDataMap removeSatID(const gnssDataMap& gdsMap,
                           const SatIDSet& satSet)
   {
      gnssDataMap dataMap;

      // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = gdsMap.begin();
           it != gdsMap.end();
           ++it )
      {
         const DayTime& time(it->first);
         const sourceDataMap& sourceMap(it->second);

         for(sourceDataMap::const_iterator itsrc = sourceMap.begin();
             itsrc != sourceMap.end();
             ++itsrc)
         {
            gnssSatTypeValue gds;
            gds.header.epoch = time;
            gds.header.source = itsrc->first;
            gds.body = itsrc->second;

            gds.body.removeSatID(satSet);

            dataMap.addGnssSatTypeValue(gds);

         }  // loop in the sources

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

      return dataMap;
   }

   gnssDataMap removeSatID(const gnssDataMap& gdsMap,
                           const SatID& sat)
   {
      SatIDSet satSet;
      satSet.insert(sat);

      return removeSatID(gdsMap,satSet);
   }

   // type id
   gnssDataMap extractTypeID(const gnssDataMap& gdsMap,
                             const TypeIDSet& typeSet)
   {
      gnssDataMap dataMap;

      // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = gdsMap.begin();
           it != gdsMap.end();
           ++it )
      {
         const DayTime& time(it->first);
         const sourceDataMap& sourceMap(it->second);

         for(sourceDataMap::const_iterator itsrc = sourceMap.begin();
             itsrc != sourceMap.end();
             ++itsrc)
         {
            gnssSatTypeValue gds;
            gds.header.epoch = time;
            gds.header.source = itsrc->first;
            gds.body = itsrc->second;

            gds.body.keepOnlyTypeID(typeSet);

            dataMap.addGnssSatTypeValue(gds);

         }  // loop in the sources

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

      return dataMap;
   }

   gnssDataMap extractTypeID(const gnssDataMap& gdsMap,
                             const TypeID& type)
   {
      TypeIDSet typeSet;
      typeSet.insert(type);

      return extractTypeID(gdsMap,typeSet);
   }

   gnssDataMap keepOnlyTypeID(gnssDataMap& gdsMap,
                              const TypeIDSet& typeSet)
   {
      gdsMap = extractTypeID(gdsMap,typeSet);
      return gdsMap;
   }

   gnssDataMap keepOnlyTypeID(gnssDataMap& gdsMap,
                              const TypeID& type)
   {
      gdsMap = extractTypeID(gdsMap,type);
      return gdsMap;
   }

   gnssDataMap removeTypeID(const gnssDataMap& gdsMap,
                            const TypeIDSet& typeSet)
   {
      gnssDataMap dataMap;

      // Iterate through all items in the gnssDataMap
      for( gnssDataMap::const_iterator it = gdsMap.begin();
           it != gdsMap.end();
           ++it )
      {
         const DayTime& time(it->first);
         const sourceDataMap& sourceMap(it->second);

         for(sourceDataMap::const_iterator itsrc = sourceMap.begin();
             itsrc != sourceMap.end();
             ++itsrc)
         {
            gnssSatTypeValue gds;
            gds.header.epoch = time;
            gds.header.source = itsrc->first;
            gds.body = itsrc->second;

            gds.body.removeTypeID(typeSet);

            dataMap.addGnssSatTypeValue(gds);

         }  // loop in the sources

      }  // End of 'for( gnssDataMap::const_iterator it = gdMap.begin(); ...'

      return dataMap;
   }

   gnssDataMap removeTypeID(const gnssDataMap& gdsMap,
                            const TypeID& type)
   {
      TypeIDSet typeSet;
      typeSet.insert(type);

      return removeTypeID(gdsMap,typeSet);
   }

   //////////////////////////////////////////////////////////////////////////
   // temp testing code
   
   void testSave()
   {
      gnssDataMap gdsMap;

      DayTime time0(2010,12,31,0,0,0.0);
      for(int i=0;i<1;i++)
      {
         gnssRinex gRin;

         gRin.header.epoch = time0 + i*30.0;
         
         gRin.body[SatID(1,SatID::systemGPS)][TypeID::P1] = 100.0;
         gRin.body[SatID(2,SatID::systemGPS)][TypeID::P1] = 200.0;

         gRin.body[SatID(1,SatID::systemGPS)][TypeID::P2] = 200.0;
         gRin.body[SatID(2,SatID::systemGPS)][TypeID::P2] = 400.0;

         gRin.header.source = SourceID(SourceID::GPS,"test1");
         gdsMap.addGnssRinex(gRin);

         gRin.header.source = SourceID(SourceID::GPS,"test2");
         gdsMap.addGnssRinex(gRin);

         gRin.header.source = SourceID(SourceID::GPS,"test3");
         gdsMap.addGnssRinex(gRin);
      }
      
      saveGnssDataMap(gdsMap,"test.bin");

   }

   void testLoad()
   {
      gnssDataMap gdsMap = loadGnssDataMap("test.bin");

      SourceIDSet sourceSet;
      sourceSet.insert(SourceID(SourceID::GPS,"test1"));

      SatIDSet satSet;
      satSet.insert(SatID(1,SatID::systemGPS));

      TypeIDSet typeSet;
      typeSet.insert(TypeID::P1);

      gdsMap.keepOnlySourceID(sourceSet);
      gdsMap.keepOnlySatID(satSet);
      gdsMap.keepOnlyTypeID(typeSet);

      dumpGnssDataMap(gdsMap,"test.txt");

      int a = 0;
   }

}   // End of namespace gpstk


#endif  //GPSTK_GDSUTILS_HPP
