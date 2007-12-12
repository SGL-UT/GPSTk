#pragma ident "$Id: $"

/**
 * @file DataStructures.cpp
 * Set of several data structures to be used by other GPSTk classes.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007
//
//============================================================================


#include "DataStructures.hpp"


using namespace gpstk::StringUtils;
using namespace std;


namespace gpstk
{


      ////// typeValueMap //////


      // Returns a TypeIDSet with all the data types present in 
      // this object.
   TypeIDSet typeValueMap::getTypeID() const
   {
      TypeIDSet typeSet;
      typeValueMap::const_iterator pos;
      for (pos = (*this).begin(); pos != (*this).end(); ++pos)
      {
         typeSet.insert( (*pos).first );
      }
      return typeSet;
   }


      // Returns a typeValueMap with only this type of data.
      // @param type Type of value to be extracted.
   typeValueMap typeValueMap::extractTypeID(const TypeID& type) const
   {
      TypeIDSet typeSet;
      typeSet.insert(type);
      return extractTypeID(typeSet);
   }


      // Returns a typeValueMap with only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data to 
      //                be extracted.
   typeValueMap typeValueMap::extractTypeID(const TypeIDSet& typeSet) const
   {
      typeValueMap tvMap;
      TypeIDSet::const_iterator pos;
      for(pos = typeSet.begin(); pos != typeSet.end(); ++pos)
      {
         typeValueMap::const_iterator itObs;
         itObs = (*this).find(*pos);
         if( itObs != (*this).end() ) 
         {
            tvMap[ (*itObs).first ] = (*itObs).second;
         };
      }

      return tvMap;

   }



      // Modifies this object, keeping only this type of data.
      // @param type Type of value to be kept.
   typeValueMap& typeValueMap::keepOnlyTypeID(const TypeID& type)
   {
      TypeIDSet typeSet;
      typeSet.insert(type);

      return (keepOnlyTypeID(typeSet));
   }


      // Modifies this object, keeping only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data 
      //                to be kept.
   typeValueMap& typeValueMap::keepOnlyTypeID(const TypeIDSet& typeSet)
   {
      typeValueMap tvMap( (*this).extractTypeID(typeSet) );
      (*this) = tvMap;

      return (*this);

   }


      // Modifies this object, removing these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   typeValueMap& typeValueMap::removeTypeID(const TypeIDSet& typeSet)
   {
      TypeIDSet::const_iterator pos;
      for( pos = typeSet.begin(); pos != typeSet.end(); ++pos )
      {
         (*this).erase(*pos);
      }

      return (*this);

   }


      // Returns a reference to the data value (double) with
      // corresponding type.
      // @param type Type of value to be looked for.
   double& typeValueMap::operator()(const TypeID& type)
      throw(TypeIDNotFound)
   {
      typeValueMap::iterator itObs;
      itObs = (*this).find(type);

      if ( itObs != (*this).end() ) 
      {
         return (*itObs).second;
      }
      else
      {
         GPSTK_THROW(TypeIDNotFound("TypeID not found in map"));
      }

   }



      ////// satValueMap //////


      // Returns a SatIDSet with all the satellites present in this object.
   SatIDSet satValueMap::getSatID() const
   {
      SatIDSet satSet;

      satValueMap::const_iterator pos;
      for( pos = (*this).begin(); pos != (*this).end(); ++pos )
      {
         satSet.insert( (*pos).first );
      }

      return satSet;

   }


      // Returns a Vector with all the satellites present in this object.
   Vector<SatID> satValueMap::getVectorOfSatID() const
   {
      std::vector<SatID> temp;

      satValueMap::const_iterator pos;
      for( pos = (*this).begin(); pos != (*this).end(); ++pos )
      {
         temp.push_back( (*pos).first );
      }

      Vector<SatID> result;
      result = temp;

      return result;

   }


      // Returns a satValueMap with only this satellite.
      // @param satellite Satellite to be extracted.
   satValueMap satValueMap::extractSatID(const SatID& satellite) const
   {
      SatIDSet satSet;
      satSet.insert(satellite);

      return extractSatID(satSet);

   }


      // Returns a satValueMap with only one satellite, identified by
      // the given parameters.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   satValueMap satValueMap::extractSatID( const int& p,
                                     const SatID::SatelliteSystem& s ) const
   {
      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return extractSatID(tempSatellite);

   }


      // Returns a satValueMap with only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to
      //               be extracted.
   satValueMap satValueMap::extractSatID(const SatIDSet& satSet) const
   {
      satValueMap svMap;

      SatIDSet::const_iterator pos;
      for( pos = satSet.begin(); pos != satSet.end(); ++pos )
      {
         satValueMap::const_iterator itObs;
         itObs = (*this).find(*pos);

         if( itObs != (*this).end() ) 
         {
            svMap[ (*itObs).first ] = (*itObs).second;
         };
      }

      return svMap;

   }


      // Modifies this object, keeping only this satellite.
      // @param satellite Satellite to be kept.
   satValueMap& satValueMap::keepOnlySatID(const SatID& satellite)
   {
      SatIDSet satSet;
      satSet.insert(satellite);

      return keepOnlySatID(satSet);

   }


      // Modifies this object, keeping only this satellite.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   satValueMap& satValueMap::keepOnlySatID( const int& p,
                                            const SatID::SatelliteSystem& s )
   {
      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return keepOnlySatID(tempSatellite);

   }


      // Modifies this object, keeping only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to be kept.
   satValueMap& satValueMap::keepOnlySatID(const SatIDSet& satSet)
   {
      satValueMap svMap = extractSatID(satSet);
      (*this) = svMap;

      return (*this);

   }


      // Modifies this object, removing these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to
      //               be removed.
   satValueMap& satValueMap::removeSatID(const SatIDSet& satSet)
   {
      SatIDSet::const_iterator pos;
      for( pos = satSet.begin(); pos != satSet.end(); ++pos )
      {
         (*this).erase(*pos);
      }

      return (*this);

   }


      // Returns a reference to the data value (double) with
      // corresponding SatID.
      // @param satellite Satellite to be looked for.
   double& satValueMap::operator()(const SatID& satellite)
      throw(SatIDNotFound)
   {
      satValueMap::iterator itObs;
      itObs = (*this).find(satellite);

      if ( itObs != (*this).end() ) 
      {
         return (*itObs).second;
      }
      else
      {
         GPSTK_THROW(SatIDNotFound("SatID not found in map"));
      }

   }



      ////// satTypeValueMap //////


      /* Returns the total number of data elements in the map.
       * This method DOES NOT suppose that all the satellites have
       * the same number of type values.
       */
   size_t satTypeValueMap::numElements() const 
   {

      size_t numEle(0);

      satTypeValueMap::const_iterator it;
      for( it = (*this).begin(); it != (*this).end(); ++it )
      {
         numEle = numEle + (*it).second.size();
      }

         return numEle;

   }


      // Returns a SatIDSet with all the satellites present in this object.
   SatIDSet satTypeValueMap::getSatID() const
   {
      SatIDSet satSet;

      satTypeValueMap::const_iterator pos;
      for( pos = (*this).begin(); pos != (*this).end(); ++pos )
      {
         satSet.insert( (*pos).first );
      }

      return satSet;

   }


      // Returns a Vector with all the satellites present in this object.
   Vector<SatID> satTypeValueMap::getVectorOfSatID() const
   {
      std::vector<SatID> temp;

      satTypeValueMap::const_iterator pos;
      for( pos = (*this).begin(); pos != (*this).end(); ++pos )
      {
         temp.push_back( (*pos).first );
      }

      Vector<SatID> result;
      result = temp;

      return result;

   }


      // Returns a TypeIDSet with all the data types present in
      // this object. This does not imply that all satellites have these types.
   TypeIDSet satTypeValueMap::getTypeID() const
   {

      TypeIDSet typeSet;

      satTypeValueMap::const_iterator pos;
      for( pos = (*this).begin(); pos != (*this).end(); ++pos )
      {
         typeValueMap::const_iterator it;
         for( it = (*pos).second.begin(); it != (*pos).second.end(); ++it )
         {
            typeSet.insert( (*it).first );
         }
      }

      return typeSet;

   }


      // Returns a satTypeValueMap with only this satellite.
      // @param satellite Satellite to be extracted.
   satTypeValueMap satTypeValueMap::extractSatID(const SatID& satellite) const
   {
      SatIDSet satSet;
      satSet.insert(satellite);

      return extractSatID(satSet);

   }


      // Returns a satTypeValueMap with only one satellite, identified
      // by the given parameters.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   satTypeValueMap satTypeValueMap::extractSatID( const int& p,
                                        const SatID::SatelliteSystem& s) const
   {
      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return extractSatID(tempSatellite);

   }


      // Returns a satTypeValueMap with only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to
      //               be extracted.
   satTypeValueMap satTypeValueMap::extractSatID(const SatIDSet& satSet) const
   {
      satTypeValueMap stvMap;

      SatIDSet::const_iterator pos;
      for( pos = satSet.begin(); pos != satSet.end(); ++pos )
      {
         satTypeValueMap::const_iterator itObs;
         itObs = (*this).find(*pos);
         if( itObs != (*this).end() ) 
         {
            stvMap[ (*itObs).first ] = (*itObs).second;
         };
      }

      return stvMap;

   }


      // Modifies this object, keeping only this satellite.
      // @param satellite Satellite to be kept.
   satTypeValueMap& satTypeValueMap::keepOnlySatID(const SatID& satellite)
   {
      SatIDSet satSet;
      satSet.insert(satellite);

      return keepOnlySatID(satSet);

   }


      // Modifies this object, keeping only this satellite.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   satTypeValueMap& satTypeValueMap::keepOnlySatID( const int& p,
                                             const SatID::SatelliteSystem& s )
   {
      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return keepOnlySatID(tempSatellite);

   }


      // Modifies this object, keeping only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to be kept.
   satTypeValueMap& satTypeValueMap::keepOnlySatID(const SatIDSet& satSet)
   {
      satTypeValueMap stvMap( extractSatID(satSet) );
      (*this) = stvMap;

      return (*this);

   }


      // Returns a satTypeValueMap with only this type of value.
      // @param type Type of value to be extracted.
   satTypeValueMap satTypeValueMap::extractTypeID(const TypeID& type) const
   {
      TypeIDSet typeSet;
      typeSet.insert(type);

      return extractTypeID(typeSet);

   }


      // Returns a satTypeValueMap with only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be extracted.
   satTypeValueMap satTypeValueMap::extractTypeID(const TypeIDSet& typeSet)
      const
   {
      satTypeValueMap theMap;

      satTypeValueMap::const_iterator it;
      for( it = (*this).begin(); it != (*this).end(); ++it )
      {
         typeValueMap tvMap = (*it).second.extractTypeID(typeSet);
         if( tvMap.size() > 0 )
         {
            theMap[(*it).first] = tvMap;
         };
      };

      return theMap;

   }


      // Modifies this object, keeping only this type of data.
      // @param type Type of value to be kept.
   satTypeValueMap& satTypeValueMap::keepOnlyTypeID(const TypeID& type)
   {
      TypeIDSet typeSet;
      typeSet.insert(type);

      return keepOnlyTypeID(typeSet);

   }


      // Modifies this object, keeping only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   satTypeValueMap& satTypeValueMap::keepOnlyTypeID(const TypeIDSet& typeSet)
   {
      satTypeValueMap stvMap( extractTypeID(typeSet) );
      (*this) = stvMap;

      return (*this);

   }


      // Modifies this object, removing these satellites.
      // @param satSet Set (SatIDSet) containing the satellites
      //               to be removed.
   satTypeValueMap& satTypeValueMap::removeSatID(const SatIDSet& satSet)
   {
      SatIDSet::const_iterator pos;
      for( pos = satSet.begin(); pos != satSet.end(); ++pos )
      {
         (*this).erase(*pos);
      }

      return (*this);

   }


      // Modifies this object, removing this type of data.
      // @param type Type of value to be removed.
   satTypeValueMap& satTypeValueMap::removeTypeID(const TypeID& type)
   {
      satTypeValueMap::iterator it;
      for( it = (*this).begin(); it != (*this).end(); ++it )
      {
         (*it).second.removeTypeID(type);
      }

      return (*this);

   }


      // Modifies this object, removing these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   satTypeValueMap& satTypeValueMap::removeTypeID(const TypeIDSet& typeSet)
   {
      TypeIDSet::const_iterator pos;
      for( pos = typeSet.begin(); pos != typeSet.end(); ++pos )
      {
         removeTypeID(*pos);
      }

      return (*this);

   }


      // Returns a GPSTk::Vector containing the data values with this type.
      // @param type Type of value to be returned.
      // This method returns zero if a given satellite does not have this type.
   Vector<double> satTypeValueMap::getVectorOfTypeID(const TypeID& type) const
   {

      std::vector<double> temp;

      typeValueMap::const_iterator itObs;
      satTypeValueMap::const_iterator it;
      for( it = (*this).begin(); it != (*this).end(); ++it )
      {
         itObs = (*it).second.find(type);
         if ( itObs != (*it).second.end() )
         {
            temp.push_back( (*itObs).second );
         }
         else
         {
            temp.push_back( 0.0 );
         }
      }

      Vector<double> result;
      result = temp;

      return result;

   }


      // Returns a GPSTk::Matrix containing the data values in this set.
      // @param typeSet  TypeIDSet of values to be returned.
   Matrix<double> satTypeValueMap::getMatrixOfTypes(const TypeIDSet& typeSet)
      const
   {
         // First, let's create a Matrix<double> of the proper size
      Matrix<double> tempMat( (*this).numSats(), typeSet.size(), 0.0 );

      size_t numRow(0), numCol(0);

      satTypeValueMap::const_iterator it;
      for( it = (*this).begin(); it != (*this).end(); ++it )
      {
         numCol=0;

         typeValueMap::const_iterator itObs;
         TypeIDSet::const_iterator pos;
         for( pos = typeSet.begin(); pos != typeSet.end(); ++pos )
         {
            itObs = (*it).second.find(*pos);
            if( itObs != (*it).second.end() )
            {
               tempMat(numRow, numCol) = (*itObs).second;
            }

            ++numCol;
         }

         ++numRow;

      }

      return tempMat;

   }   // End getMatrixOfTypes(const TypeIDSet& typeSet)


      /* Modifies this object, adding one vector of data with this type,
       * one value per satellite.
       *
       * If type already exists, data is overwritten. If the number of
       * values does not match with the number of satellites, a
       * NumberOfSatsMismatch exception is thrown.
       *
       * Given that dataVector does not store information about the
       * satellites the values correspond to, the user is held responsible
       * for having the data values stored in dataVector in the proper
       * order regarding the SatIDs in this object.
       *
       * @param type          Type of data to be added.
       * @param dataVector    GPSTk Vector containing the data to be added.
       */
   satTypeValueMap& satTypeValueMap::insertTypeIDVector( const TypeID& type,
                                           const Vector<double> dataVector )
      throw(NumberOfSatsMismatch)
   {

      if( dataVector.size() == (*this).numSats() )
      {
         size_t pos = 0;

         satTypeValueMap::iterator it;
         for( it = (*this).begin(); it != (*this).end(); ++it )
         {
            (*it).second[type] = dataVector[pos];
            ++pos;
         }

         return (*this);

      }
      else
      {
         GPSTK_THROW( NumberOfSatsMismatch(" Number of data values in vector \
                                    and number of satellites do not match") );
      }

   }


      /* Modifies this object, adding a matrix of data, one vector
       * per satellite.
       *
       * If types already exists, data is overwritten. If the number of
       * rows in matrix does not match with the number of satellites, a
       * NumberOfSatsMismatch exception is thrown. If the number of columns
       * in matrix does not match with the number of types in typeSet, a
       * NumberOfTypesMismatch exception is thrown.
       *
       * Given that dataMatrix does not store information about the
       * satellites and types the values correspond to, the user is held
       * responsible for having those data values stored in dataMatrix in
       * the proper order regarding the SatIDs in this object and the
       * provided typeSet.
       *
       * @param typeSet       Set (TypeIDSet) containing the types of data
       *                      to be added.
       * @param dataMatrix    GPSTk Matrix containing the data to be added.
       */
   satTypeValueMap& satTypeValueMap::insertMatrix( const TypeIDSet& typeSet,
                                             const Matrix<double> dataMatrix )
      throw(NumberOfSatsMismatch, NumberOfTypesMismatch)
   {

      if( dataMatrix.rows() != (*this).numSats() )
      {
         GPSTK_THROW( NumberOfSatsMismatch("Number of rows in matrix and \
                                         number of satellites do not match") );
      }

      if( dataMatrix.cols() == typeSet.size() )
      {

         size_t pos(0);

         satTypeValueMap::iterator it;
         for( it = (*this).begin(); it != (*this).end(); ++it )
         {

            size_t idx(0);

            TypeIDSet::const_iterator itSet;
            for( itSet = typeSet.begin(); itSet != typeSet.end(); ++itSet )
            {
               (*it).second[(*itSet)] = dataMatrix(pos,idx);
               ++idx;
            }

            ++pos;

         }

         return (*this);

      }
      else
      {
         GPSTK_THROW( NumberOfTypesMismatch("Number of data values per row \
                               in matrix and number of types do not match") );
      }

   }


      // Returns a reference to the typeValueMap with corresponding SatID.
      // @param type Type of value to be looked for.
   typeValueMap& satTypeValueMap::operator()(const SatID& satellite)
      throw(SatIDNotFound)
   {
      satTypeValueMap::iterator itObs;
      itObs = (*this).find(satellite);
      if( itObs != (*this).end() ) 
      {
         return (*itObs).second;
      }
      else
      {
         GPSTK_THROW(SatIDNotFound("SatID not found in map"));
      }

   }










      // Convenience output method for structure satTypeValueMap
   std::ostream& satTypeValueMap::dump( std::ostream& s,
                                        int mode ) const
   {
      satTypeValueMap::const_iterator it;
      for (it = (*this).begin(); it!= (*this).end(); it++) 
      {
            // First, print satellite (system and PRN)
         s << (*it).first << " ";

         typeValueMap::const_iterator itObs;
         for( itObs  = (*it).second.begin(); 
              itObs != (*it).second.end();
              itObs++ )
         {
            if (mode==1)
            {
               s << (*itObs).first << " ";
            }

            s << (*itObs).second << " ";

         }  // End for( itObs = ... )

         s << endl;

      }

      return s;
   } // satTypeValueMap::dump()



      // stream output for satTypeValueMap
   std::ostream& operator<<( std::ostream& s,
                             const satTypeValueMap& stvMap )
   {
      stvMap.dump(s);
      return s;
   }



      // Input for gnssSatTypeValue from RinexObsHeader
   gnssSatTypeValue& operator>>( const RinexObsHeader& roh,
                                 gnssSatTypeValue& f )
   {

         // First, select the right system the data came from
      f.header.source.type = SatIDsystem2SourceIDtype(roh.system);

         // Set the proper name for the receiver
      f.header.source.sourceName = roh.markerName;

      return f;
   }



      // Input for gnssRinex from RinexObsHeader
   gnssRinex& operator>>( const RinexObsHeader& roh,
                          gnssRinex& f )
   {

         // First, select the right system the data came from
      f.header.source.type = SatIDsystem2SourceIDtype(roh.system);

         // Set the proper name for the receiver
      f.header.source.sourceName = roh.markerName;

         // Set the proper antenna type for the receiver
      f.header.antennaType = roh.antType;

         // Set the proper antenna position
      f.header.antennaPosition = roh.antennaPosition;

      return f;
   }



      // Input for gnssSatTypeValue from RinexObsData
   gnssSatTypeValue& operator>>( const RinexObsData& rod,
                                 gnssSatTypeValue& f )
   {

         // Fill header epoch with the proper value
      f.header.epoch = rod.time;

         // Extract the observations map and store it in the body
      f.body = FillsatTypeValueMapwithRinexObsData(rod);

      return f;
   }



      // Input for gnssRinex from RinexObsData
   gnssRinex& operator>>( const RinexObsData& rod,
                          gnssRinex& f )
   {

         // Fill header epoch with the proper value
      f.header.epoch = rod.time;

         // Fill header epoch with the proper value
      f.header.epochFlag = rod.epochFlag;

         // Extract the observations map and store it in the body
      f.body = FillsatTypeValueMapwithRinexObsData(rod);

      return f;
   }



      // Stream input for gnssSatTypeValue
   std::istream& operator>>( std::istream& i,
                             gnssSatTypeValue& f )
      throw(FFStreamError, gpstk::StringUtils::StringException)
   {

      FFStream* ffs = dynamic_cast<FFStream*>(&i);
      if(ffs)
      {
         try
         {
            RinexObsStream& strm = dynamic_cast<RinexObsStream&>(*ffs);
      
               // If the header hasn't been read, read it...
            if(!strm.headerRead) strm >> strm.header;

               // Clear out this object
            RinexObsHeader& hdr = strm.header;

            hdr >> f;
      
            std::string line;

            strm.formattedGetLine(line, true);
      
            if( line.size()>80 ||
                line[0] != ' ' ||
                line[3] != ' ' ||
                line[6] != ' ' )
            {
               FFStreamError e("Bad epoch line");
               GPSTK_THROW(e);
            }

               // process the epoch line, including SV list and clock bias
            short epochFlag = asInt(line.substr(28,1));
            if( (epochFlag < 0) ||
                (epochFlag > 6) )
            {
               FFStreamError e("Invalid epoch flag: " + asString(epochFlag));
               GPSTK_THROW(e);
            }

            f.header.epoch = parseTime(line, hdr);

            short numSvs = asInt(line.substr(29,3));

            RinexObsData rod;

               // Now read the observations ...
            if( epochFlag==0 ||
                epochFlag==1 ||
                epochFlag==6 )
            {
               int isv, ndx, line_ndx;
               vector<SatID> satIndex(numSvs);
               int col=30;

               for (isv=1, ndx=0; ndx<numSvs; isv++, ndx++)
               {
                  if( !(isv % 13) )
                  {
                     strm.formattedGetLine(line);
                     isv = 1;
                     if(line.size() > 80)
                     {
                        FFStreamError err( "Invalid line size:" +
                                           asString(line.size()) );
                        GPSTK_THROW(err);
                     }
                  }
                  try
                  {
                     satIndex[ndx] = RinexSatID( line.substr(col+isv*3-1,3) );
                  }
                  catch (Exception& e)
                  { 
                     FFStreamError ffse(e);
                     GPSTK_THROW(ffse);
                  }
               } // End of for(isv=1, ... )


               for (isv=0; isv < numSvs; isv++)
               {
                  short numObs = hdr.obsTypeList.size();
                  for(ndx=0, line_ndx=0; ndx < numObs; ndx++, line_ndx++)
                  {
                     SatID sat = satIndex[isv];
                     RinexObsHeader::RinexObsType obs_type =
                                                         hdr.obsTypeList[ndx];
                     if( !(line_ndx % 5) )
                     {
                        strm.formattedGetLine(line);
                        line_ndx = 0;
                        if( line.size() > 80 )
                        {
                           FFStreamError err("Invalid line size:" +
                                                      asString(line.size()));
                           GPSTK_THROW(err);
                        }
                     }
               
                     line.resize(80, ' ');
               
                     rod.obs[sat][obs_type].data =
                                 asDouble( line.substr(line_ndx*16,   14) );

                     rod.obs[sat][obs_type].lli =
                                    asInt( line.substr(line_ndx*16+14, 1) );

                     rod.obs[sat][obs_type].ssi =
                                    asInt( line.substr(line_ndx*16+15, 1) );

                  }  // End of for(ndx=0, ... )

               }  // End of for (isv=0; ... )

            }  // End of if( epochFlag==0 || ... )

            f.body = FillsatTypeValueMapwithRinexObsData(rod);

            return i;

         }  // End of "try" block
         ////
         //// ATENTION: This part is VERY UGLY
         ////   Help from the guy who wrote 
         ////        FFStream::tryFFStreamGet(FFData& rec)
         ////   will be very appreciated
         ////
         // EOF - do nothing - eof causes fail() to be set which
         // is handled by std::fstream
         catch (EndOfFile& e)
         {
            return i;
         }
         catch (...)
         {
            return i;
         }

      }  // End of block: "if(ffs) ..."
      else
      {
         FFStreamError e("operator<< stream argument must be an FFStream");
         GPSTK_THROW(e);
      }
        
   }  // End of stream input for gnssSatTypeValue



      // Stream input for gnssRinex
   std::istream& operator>>( std::istream& i,
                             gnssRinex& f )
      throw(FFStreamError, gpstk::StringUtils::StringException)
   {

      FFStream* ffs = dynamic_cast<FFStream*>(&i);
      if(ffs)
      {
         try
         {         
            RinexObsStream& strm = dynamic_cast<RinexObsStream&>(*ffs);
      
               // If the header hasn't been read, read it...
            if(!strm.headerRead) strm >> strm.header;

               // Clear out this object
            RinexObsHeader& hdr = strm.header;

            hdr >> f;
      
            std::string line;

            strm.formattedGetLine(line, true);
      
            if( line.size()>80 ||
                line[0] != ' ' ||
                line[3] != ' ' ||
                line[6] != ' ' )
            {
               FFStreamError e("Bad epoch line");
               GPSTK_THROW(e);
            }

               // process the epoch line, including SV list and clock bias
            short epochFlag = asInt(line.substr(28,1));
            if( (epochFlag < 0) ||
                (epochFlag > 6) )
            {
               FFStreamError e("Invalid epoch flag: " + asString(epochFlag));
               GPSTK_THROW(e);
            }

            f.header.epochFlag = epochFlag;

            f.header.epoch = parseTime(line, hdr);

            short numSvs = asInt(line.substr(29,3));

            RinexObsData rod;

               // Now read the observations ...
            if( epochFlag==0 ||
                epochFlag==1 ||
                epochFlag==6 )
            {
               int isv, ndx, line_ndx;
               vector<SatID> satIndex(numSvs);
               int col=30;

               for( isv=1, ndx=0; ndx<numSvs; isv++, ndx++ )
               {
                  if( !(isv % 13) )
                  {
                     strm.formattedGetLine(line);
                     isv = 1;
                     if( line.size() > 80 )
                     {
                        FFStreamError err("Invalid line size:" +
                                                      asString( line.size()) );
                        GPSTK_THROW(err);
                     }
                  }

                  try
                  {
                     satIndex[ndx] = RinexSatID( line.substr(col+isv*3-1, 3) );
                  }
                  catch (Exception& e)
                  { 
                     FFStreamError ffse(e);
                     GPSTK_THROW(ffse);
                  }
               } // End of for( isv=1 ... )

               for( isv=0; isv < numSvs; isv++ )
               {
                  short numObs = hdr.obsTypeList.size();
                  for( ndx=0, line_ndx=0; ndx < numObs; ndx++, line_ndx++ )
                  {
                     SatID sat = satIndex[isv];
                     RinexObsHeader::RinexObsType obs_type =
                                                         hdr.obsTypeList[ndx];
                     if( !(line_ndx % 5) )
                     {
                        strm.formattedGetLine(line);
                        line_ndx = 0;

                        if( line.size() > 80 )
                        {
                           FFStreamError err("Invalid line size:" +
                                                      asString( line.size()) );
                           GPSTK_THROW(err);
                        }
                     }
               
                     line.resize(80, ' ');
               
                     rod.obs[sat][obs_type].data =
                                    asDouble( line.substr(line_ndx*16, 14) );

                     rod.obs[sat][obs_type].lli =
                                    asInt( line.substr(line_ndx*16+14, 1) );

                     rod.obs[sat][obs_type].ssi =
                                    asInt( line.substr(line_ndx*16+15, 1) );

                  } // End of for( ndx=0 ...)

               }  // End of for( isv=0 ...)

            }  // End of if( epochFlag==0 || ... )

            f.body = FillsatTypeValueMapwithRinexObsData(rod);

            return i;
         }  // End of "try" block
         ////
         //// ATENTION: This part is VERY UGLY
         ////   Help from the guy who wrote
         ////        FFStream::tryFFStreamGet(FFData& rec)
         ////   will be very appreciated
         ////
         // EOF - do nothing - eof causes fail() to be set which
         // is handled by std::fstream
         catch (EndOfFile& e)
         {
            return i;
         }
         catch (...)
         {
            return i;
         }

      }  // End of block: "if (ffs)..."
      else
      {
         FFStreamError e("operator<< stream argument must be an FFStream");
         GPSTK_THROW(e);
      }
        
   }  // End of stream input for gnssRinex


}
