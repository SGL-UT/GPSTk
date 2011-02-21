
/**
* @file GPSTK_CONVERTC1TOP1_HPP.hpp
* Class to convert C1 to P1.
*/

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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010, 2011
//
//============================================================================


#include "ConvertC1ToP1.hpp"

namespace gpstk
{
      // Index initially assigned to this class
   int ConvertC1ToP1::classIndex = 4800000;

      // Returns an index identifying this object.
   int ConvertC1ToP1::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string ConvertC1ToP1::getClassName() const
   { return "ConvertC1ToP1"; }

      /* Sets name of file containing DCBs data.
       * @param name      Name of the file containing DCB(P1-C1)
       */
   ConvertC1ToP1& ConvertC1ToP1::setDCBFile(const string& fileP1C1)
   {
      dcbP1C1.open(fileP1C1);
      
      return (*this);
   }

      /* Returns a satTypeValueMap object, adding the new data generated
       *  when calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ConvertC1ToP1::Process( const DayTime& time,
                                                satTypeValueMap& gData )
      throw(ProcessingException)
   {
      try
      {
         // Loop through all the satellites
         satTypeValueMap::iterator it;
         for (it = gData.begin(); it != gData.end(); ++it)
         {
            SatID sat = it->first;
            
            typeValueMap::iterator ittC1 = it->second.find(TypeID::C1);
            typeValueMap::iterator ittP1 = it->second.find(TypeID::P1);
            
            if( (ittC1!=it->second.end()) && (ittP1==it->second.end()) )
            {
               double Bp1c1(0.0);      // in ns
               try
               {
                  Bp1c1 = dcbP1C1.getDCB(sat);
               }
               catch(...)
               {
                  Bp1c1 = 0.0;
               }

               it->second[TypeID::P1] = it->second[TypeID::C1] 
                                       +Bp1c1*(C_GPS_M * 1.0e-9);
            }

         }  // End of 'for (it = gData.begin(); it != gData.end(); ++it)'

         return gData;
            
      }
      catch(Exception& u)
      {

         // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
            + StringUtils::asString( getIndex() ) + ":"
            + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'ConvertC1ToP1::Process()'


}  // End of namespace gpstk



