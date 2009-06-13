#pragma ident "$Id$"

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009
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

      for( typeValueMap::const_iterator pos = (*this).begin();
           pos != (*this).end();
           ++pos )
      {
         typeSet.insert( (*pos).first );
      }

      return typeSet;

   }  // End of method 'typeValueMap::getTypeID()'



      // Returns a typeValueMap with only this type of data.
      // @param type Type of value to be extracted.
   typeValueMap typeValueMap::extractTypeID(const TypeID& type) const
   {

      TypeIDSet typeSet;
      typeSet.insert(type);

      return extractTypeID(typeSet);

   }  // End of method 'typeValueMap::extractTypeID()'



      // Returns a typeValueMap with only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data to
      //                be extracted.
   typeValueMap typeValueMap::extractTypeID(const TypeIDSet& typeSet) const
   {

      typeValueMap tvMap;

      for( TypeIDSet::const_iterator pos = typeSet.begin();
           pos != typeSet.end();
           ++pos )
      {

         typeValueMap::const_iterator itObs( (*this).find(*pos) );
         if( itObs != (*this).end() )
         {
            tvMap[ (*itObs).first ] = (*itObs).second;
         };
      }

      return tvMap;

   }  // End of method 'typeValueMap::extractTypeID()'



      // Modifies this object, keeping only this type of data.
      // @param type Type of value to be kept.
   typeValueMap& typeValueMap::keepOnlyTypeID(const TypeID& type)
   {

      TypeIDSet typeSet;
      typeSet.insert(type);

      return (keepOnlyTypeID(typeSet));

   }  // End of method 'typeValueMap::keepOnlyTypeID()'



      // Modifies this object, keeping only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   typeValueMap& typeValueMap::keepOnlyTypeID(const TypeIDSet& typeSet)
   {

      typeValueMap tvMap( (*this).extractTypeID(typeSet) );
      (*this) = tvMap;

      return (*this);

   }  // End of method 'typeValueMap::keepOnlyTypeID()'



      // Modifies this object, removing these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   typeValueMap& typeValueMap::removeTypeID(const TypeIDSet& typeSet)
   {

      for( TypeIDSet::const_iterator pos = typeSet.begin();
           pos != typeSet.end();
           ++pos )
      {
         (*this).erase(*pos);
      }

      return (*this);

   }  // End of method 'typeValueMap::removeTypeID()'



      /* Returns the data value (double) corresponding to provided type.
       *
       * @param type       Type of value to be looked for.
       */
   double typeValueMap::getValue(const TypeID& type) const
      throw(TypeIDNotFound)
   {

      typeValueMap::const_iterator itObs( (*this).find(type) );
      if ( itObs != (*this).end() )
      {
         return (*itObs).second;
      }
      else
      {
         GPSTK_THROW(TypeIDNotFound("TypeID not found in map"));
      }

   }  // End of method 'typeValueMap::getValue()'



      // Returns a reference to the data value (double) with
      // corresponding type.
      // @param type Type of value to be looked for.
   double& typeValueMap::operator()(const TypeID& type)
      throw(TypeIDNotFound)
   {

      typeValueMap::iterator itObs ( (*this).find(type) );

      if ( itObs != (*this).end() )
      {
         return (*itObs).second;
      }
      else
      {
         GPSTK_THROW(TypeIDNotFound("TypeID not found in map"));
      }

   }  // End of method 'typeValueMap::operator()'



      ////// satValueMap //////


      // Returns a SatIDSet with all the satellites present in this object.
   SatIDSet satValueMap::getSatID() const
   {

      SatIDSet satSet;

      for( satValueMap::const_iterator pos = (*this).begin();
           pos != (*this).end();
           ++pos )
      {
         satSet.insert( (*pos).first );
      }

      return satSet;

   }  // End of method 'satValueMap::getSatID()'



      // Returns a Vector with all the satellites present in this object.
   Vector<SatID> satValueMap::getVectorOfSatID() const
   {

      std::vector<SatID> temp;

      for( satValueMap::const_iterator pos = (*this).begin();
           pos != (*this).end();
           ++pos )
      {
         temp.push_back( (*pos).first );
      }

      Vector<SatID> result;
      result = temp;

      return result;

   }  // End of method 'satValueMap::getVectorOfSatID()'



      // Returns a satValueMap with only this satellite.
      // @param satellite Satellite to be extracted.
   satValueMap satValueMap::extractSatID(const SatID& satellite) const
   {

      SatIDSet satSet;
      satSet.insert(satellite);

      return extractSatID(satSet);

   }  // End of method 'satValueMap::extractSatID()'



      // Returns a satValueMap with only one satellite, identified by
      // the given parameters.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   satValueMap satValueMap::extractSatID( const int& p,
                                     const SatID::SatelliteSystem& s ) const
   {

      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return extractSatID(tempSatellite);

   }  // End of method 'satValueMap::extractSatID()'



      // Returns a satValueMap with only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to
      //               be extracted.
   satValueMap satValueMap::extractSatID(const SatIDSet& satSet) const
   {

      satValueMap svMap;

      for( SatIDSet::const_iterator pos = satSet.begin();
           pos != satSet.end();
           ++pos )
      {
         satValueMap::const_iterator itObs( (*this).find(*pos) );

         if( itObs != (*this).end() )
         {
            svMap[ (*itObs).first ] = (*itObs).second;
         };
      }

      return svMap;

   }  // End of method 'satValueMap::extractSatID()'



      // Modifies this object, keeping only this satellite.
      // @param satellite Satellite to be kept.
   satValueMap& satValueMap::keepOnlySatID(const SatID& satellite)
   {

      SatIDSet satSet;
      satSet.insert(satellite);

      return keepOnlySatID(satSet);

   }  // End of method 'satValueMap::keepOnlySatID()'



      // Modifies this object, keeping only this satellite.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   satValueMap& satValueMap::keepOnlySatID( const int& p,
                                            const SatID::SatelliteSystem& s )
   {

      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return keepOnlySatID(tempSatellite);

   }  // End of method 'satValueMap::keepOnlySatID()'



      // Modifies this object, keeping only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to be kept.
   satValueMap& satValueMap::keepOnlySatID(const SatIDSet& satSet)
   {

      satValueMap svMap = extractSatID(satSet);
      (*this) = svMap;

      return (*this);

   }  // End of method 'satValueMap::keepOnlySatID()'



      // Modifies this object, removing these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to
      //               be removed.
   satValueMap& satValueMap::removeSatID(const SatIDSet& satSet)
   {

      for( SatIDSet::const_iterator pos = satSet.begin();
           pos != satSet.end();
           ++pos )
      {
         (*this).erase(*pos);
      }

      return (*this);

   }  // End of method 'satValueMap::removeSatID()'



      /* Returns the data value (double) corresponding to provided SatID.
       *
       * @param satellite     Satellite to be looked for.
       */
   double satValueMap::getValue(const SatID& satellite) const
      throw(TypeIDNotFound)
   {

      satValueMap::const_iterator itObs( (*this).find(satellite) );
      if ( itObs != (*this).end() )
      {
         return (*itObs).second;
      }
      else
      {
         GPSTK_THROW(SatIDNotFound("SatID not found in map"));
      }

   }  // End of method 'satValueMap::getValue()'



      // Returns a reference to the data value (double) with
      // corresponding SatID.
      // @param satellite Satellite to be looked for.
   double& satValueMap::operator()(const SatID& satellite)
      throw(SatIDNotFound)
   {

      satValueMap::iterator itObs( (*this).find(satellite) );

      if ( itObs != (*this).end() )
      {
         return (*itObs).second;
      }
      else
      {
         GPSTK_THROW(SatIDNotFound("SatID not found in map"));
      }

   }  // End of method 'satValueMap::operator()'



      ////// satTypeValueMap //////


      /* Returns the total number of data elements in the map.
       * This method DOES NOT suppose that all the satellites have
       * the same number of type values.
       */
   size_t satTypeValueMap::numElements() const
   {

      size_t numEle(0);

      for( satTypeValueMap::const_iterator it = (*this).begin();
           it != (*this).end();
           ++it )
      {
         numEle = numEle + (*it).second.size();
      }

      return numEle;

   }  // End of method 'satTypeValueMap::numElements()'



      // Returns a SatIDSet with all the satellites present in this object.
   SatIDSet satTypeValueMap::getSatID() const
   {

      SatIDSet satSet;

      for( satTypeValueMap::const_iterator pos = (*this).begin();
           pos != (*this).end();
           ++pos )
      {
         satSet.insert( (*pos).first );
      }

      return satSet;

   }  // End of method 'satTypeValueMap::getSatID()'



      // Returns a Vector with all the satellites present in this object.
   Vector<SatID> satTypeValueMap::getVectorOfSatID() const
   {

      std::vector<SatID> temp;

      for( satTypeValueMap::const_iterator pos = (*this).begin();
           pos != (*this).end();
           ++pos )
      {
         temp.push_back( (*pos).first );
      }

      Vector<SatID> result;
      result = temp;

      return result;

   }  // End of method 'satTypeValueMap::getVectorOfSatID()'



      // Returns a TypeIDSet with all the data types present in
      // this object. This does not imply that all satellites have these types.
   TypeIDSet satTypeValueMap::getTypeID() const
   {

      TypeIDSet typeSet;

      for( satTypeValueMap::const_iterator pos = (*this).begin();
           pos != (*this).end();
           ++pos )
      {

         for( typeValueMap::const_iterator it = (*pos).second.begin();
              it != (*pos).second.end();
              ++it )
         {
            typeSet.insert( (*it).first );
         }

      }

      return typeSet;

   }  // End of method 'satTypeValueMap::getTypeID()'



      // Returns a satTypeValueMap with only this satellite.
      // @param satellite Satellite to be extracted.
   satTypeValueMap satTypeValueMap::extractSatID(const SatID& satellite) const
   {

      SatIDSet satSet;
      satSet.insert(satellite);

      return extractSatID(satSet);

   }  // End of method 'satTypeValueMap::extractSatID()'



      // Returns a satTypeValueMap with only one satellite, identified
      // by the given parameters.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   satTypeValueMap satTypeValueMap::extractSatID( const int& p,
                                        const SatID::SatelliteSystem& s) const
   {

      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return extractSatID(tempSatellite);

   }  // End of method 'satTypeValueMap::extractSatID()'



      // Returns a satTypeValueMap with only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to
      //               be extracted.
   satTypeValueMap satTypeValueMap::extractSatID(const SatIDSet& satSet) const
   {

      satTypeValueMap stvMap;

      for( SatIDSet::const_iterator pos = satSet.begin();
           pos != satSet.end();
           ++pos )
      {
         satTypeValueMap::const_iterator itObs( (*this).find(*pos) );
         if( itObs != (*this).end() )
         {
            stvMap[ (*itObs).first ] = (*itObs).second;
         };
      }

      return stvMap;

   }  // End of method 'satTypeValueMap::extractSatID()'



      // Modifies this object, keeping only this satellite.
      // @param satellite Satellite to be kept.
   satTypeValueMap& satTypeValueMap::keepOnlySatID(const SatID& satellite)
   {

      SatIDSet satSet;
      satSet.insert(satellite);

      return keepOnlySatID(satSet);

   }  // End of method 'satTypeValueMap::keepOnlySatID()'



      // Modifies this object, keeping only this satellite.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   satTypeValueMap& satTypeValueMap::keepOnlySatID( const int& p,
                                             const SatID::SatelliteSystem& s )
   {

      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return keepOnlySatID(tempSatellite);

   }  // End of method 'satTypeValueMap::keepOnlySatID()'



      // Modifies this object, keeping only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to be kept.
   satTypeValueMap& satTypeValueMap::keepOnlySatID(const SatIDSet& satSet)
   {

      satTypeValueMap stvMap( extractSatID(satSet) );
      (*this) = stvMap;

      return (*this);

   }  // End of method 'satTypeValueMap::keepOnlySatID()'



      // Returns a satTypeValueMap with only this type of value.
      // @param type Type of value to be extracted.
   satTypeValueMap satTypeValueMap::extractTypeID(const TypeID& type) const
   {

      TypeIDSet typeSet;
      typeSet.insert(type);

      return extractTypeID(typeSet);

   }  // End of method 'satTypeValueMap::extractTypeID()'



      // Returns a satTypeValueMap with only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be extracted.
   satTypeValueMap satTypeValueMap::extractTypeID(const TypeIDSet& typeSet)
      const
   {

      satTypeValueMap theMap;

      for( satTypeValueMap::const_iterator it = (*this).begin();
           it != (*this).end();
           ++it )
      {

         typeValueMap tvMap( (*it).second.extractTypeID(typeSet) );
         if( tvMap.size() > 0 )
         {
            theMap[(*it).first] = tvMap;
         };

      };

      return theMap;

   }  // End of method 'satTypeValueMap::extractTypeID()'



      // Modifies this object, keeping only this type of data.
      // @param type Type of value to be kept.
   satTypeValueMap& satTypeValueMap::keepOnlyTypeID(const TypeID& type)
   {

      TypeIDSet typeSet;
      typeSet.insert(type);

      return keepOnlyTypeID(typeSet);

   }  // End of method 'satTypeValueMap::keepOnlyTypeID()'



      // Modifies this object, keeping only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   satTypeValueMap& satTypeValueMap::keepOnlyTypeID(const TypeIDSet& typeSet)
   {

      satTypeValueMap stvMap( extractTypeID(typeSet) );
      (*this) = stvMap;

      return (*this);

   }  // End of method 'satTypeValueMap::keepOnlyTypeID()'



      // Modifies this object, removing these satellites.
      // @param satSet Set (SatIDSet) containing the satellites
      //               to be removed.
   satTypeValueMap& satTypeValueMap::removeSatID(const SatIDSet& satSet)
   {

      for( SatIDSet::const_iterator pos = satSet.begin();
           pos != satSet.end();
           ++pos )
      {
         (*this).erase(*pos);
      }

      return (*this);

   }  // End of method 'satTypeValueMap::removeSatID()'



      // Modifies this object, removing this type of data.
      // @param type Type of value to be removed.
   satTypeValueMap& satTypeValueMap::removeTypeID(const TypeID& type)
   {

      for( satTypeValueMap::iterator it = (*this).begin();
           it != (*this).end();
           ++it )
      {
         (*it).second.removeTypeID(type);
      }

      return (*this);

   }  // End of method 'satTypeValueMap::removeTypeID()'



      // Modifies this object, removing these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   satTypeValueMap& satTypeValueMap::removeTypeID(const TypeIDSet& typeSet)
   {

      for( TypeIDSet::const_iterator pos = typeSet.begin();
           pos != typeSet.end();
           ++pos )
      {
         removeTypeID(*pos);
      }

      return (*this);

   }  // End of method 'satTypeValueMap::removeTypeID()'



      // Returns a GPSTk::Vector containing the data values with this type.
      // @param type Type of value to be returned.
      // This method returns zero if a given satellite does not have this type.
   Vector<double> satTypeValueMap::getVectorOfTypeID(const TypeID& type) const
   {

         // Let's declare a STL vector
      std::vector<double> temp;

      for( satTypeValueMap::const_iterator it = (*this).begin();
           it != (*this).end();
           ++it )
      {

         typeValueMap::const_iterator itObs( (*it).second.find(type) );
         if ( itObs != (*it).second.end() )
         {
            temp.push_back( (*itObs).second );
         }
         else
         {
            temp.push_back( 0.0 );
         }

      }

         // Let's declare a GPSTk Vector
      Vector<double> result;

         // Transform STL vector into GPSTk Vector
      result = temp;

      return result;

   }  // End of method 'satTypeValueMap::getVectorOfTypeID()'



      // Returns a GPSTk::Matrix containing the data values in this set.
      // @param typeSet  TypeIDSet of values to be returned.
   Matrix<double> satTypeValueMap::getMatrixOfTypes(const TypeIDSet& typeSet)
      const
   {

         // First, let's create a Matrix<double> of the proper size
      Matrix<double> tempMat( (*this).numSats(), typeSet.size(), 0.0 );

      size_t numRow(0), numCol(0);

      for( satTypeValueMap::const_iterator it = (*this).begin();
           it != (*this).end();
           ++it )
      {
         numCol=0;

         for( TypeIDSet::const_iterator pos = typeSet.begin();
              pos != typeSet.end();
              ++pos )
         {

            typeValueMap::const_iterator itObs( (*it).second.find(*pos) );
            if( itObs != (*it).second.end() )
            {
               tempMat(numRow, numCol) = (*itObs).second;
            }

            ++numCol;
         }

         ++numRow;

      }

      return tempMat;

   }  // End of method 'satTypeValueMap::getMatrixOfTypes()'



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

         for( satTypeValueMap::iterator it = (*this).begin();
              it != (*this).end();
              ++it )
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

   }  // End of method 'satTypeValueMap::insertTypeIDVector()'



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

         for( satTypeValueMap::iterator it = (*this).begin();
              it != (*this).end();
              ++it )
         {

            size_t idx(0);

            for( TypeIDSet::const_iterator itSet = typeSet.begin();
                 itSet != typeSet.end();
                 ++itSet )
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

   }  // End of method 'satTypeValueMap::insertMatrix()'



      /* Returns the data value (double) corresponding to provided SatID
       * and TypeID.
       *
       * @param satellite     Satellite to be looked for.
       * @param type          Type to be looked for.
       */
   double satTypeValueMap::getValue( const SatID& satellite,
                                     const TypeID& type ) const
      throw( SatIDNotFound, TypeIDNotFound )
   {

      satTypeValueMap::const_iterator itObs( (*this).find(satellite) );
      if( itObs != (*this).end() )
      {
         return (*itObs).second.getValue( type );
      }
      else
      {
         GPSTK_THROW(SatIDNotFound("SatID not found in map"));
      }

   }  // End of method 'satTypeValueMap::getValue()'



      // Returns a reference to the typeValueMap with corresponding SatID.
      // @param type Type of value to be looked for.
   typeValueMap& satTypeValueMap::operator()(const SatID& satellite)
      throw(SatIDNotFound)
   {

      satTypeValueMap::iterator itObs( (*this).find(satellite) );
      if( itObs != (*this).end() )
      {
         return (*itObs).second;
      }
      else
      {
         GPSTK_THROW(SatIDNotFound("SatID not found in map"));
      }

   }  // End of method 'satTypeValueMap::operator()'




      ////// gnssSatValue //////


      // Returns a gnssSatValue with only this satellite.
      // @param satellite Satellite to be extracted.
   gnssSatValue gnssSatValue::extractSatID(const SatID& satellite) const
   {

      gnssSatValue result;
      result.header = (*this).header;
      result.body = (*this).body.extractSatID(satellite);

      return result;

   }  // End of method 'gnssSatValue::extractSatID()'



      // Returns a gnssSatValue with only one satellite, identified by
      // the given parameters.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   gnssSatValue gnssSatValue::extractSatID( const int& p,
                                       const SatID::SatelliteSystem& s ) const
   {

      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return extractSatID(tempSatellite);

   }  // End of method 'gnssSatValue::extractSatID()'



      // Returns a gnssSatValue with only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites
      //               to be extracted.
   gnssSatValue gnssSatValue::extractSatID(const SatIDSet& satSet) const
   {

      gnssSatValue result;
      result.header = (*this).header;
      result.body = (*this).body.extractSatID(satSet);

      return result;

   }  // End of method 'gnssSatValue::extractSatID()'



      // Modifies this object, keeping only this satellite.
      // @param satellite Satellite to be kept.
   gnssSatValue& gnssSatValue::keepOnlySatID(const SatID& satellite)
   {

      SatIDSet satSet;
      satSet.insert(satellite);

      return keepOnlySatID(satSet);

   }  // End of method 'gnssSatValue::keepOnlySatID()'



      // Modifies this object, keeping only this satellite.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   gnssSatValue& gnssSatValue::keepOnlySatID( const int& p,
                                              const SatID::SatelliteSystem& s )
   {

      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return keepOnlySatID(tempSatellite);

   }  // End of method 'gnssSatValue::keepOnlySatID()'



      // Modifies this object, keeping only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to be kept.
   gnssSatValue& gnssSatValue::keepOnlySatID(const SatIDSet& satSet)
   {

      satValueMap svMap ( (*this).body.extractSatID(satSet) );
      (*this).body = svMap;

      return (*this);

   }  // End of method 'gnssSatValue::keepOnlySatID()'



      // Modifies this object, removing these satellites.
      // @param satSet Set (SatIDSet) containing the satellites
      //               to be removed.
   gnssSatValue& gnssSatValue::removeSatID(const SatIDSet& satSet)
   {

      for( SatIDSet::const_iterator pos = satSet.begin();
           pos != satSet.end();
           ++pos )
      {
         (*this).body.erase(*pos);
      }

         return (*this);

   }  // End of method 'gnssSatValue::removeSatID()'




      ////// gnssTypeValue //////


      // Returns a gnssTypeValue with only this type of data.
      // @param type Type of value to be extracted.
   gnssTypeValue gnssTypeValue::extractTypeID(const TypeID& type) const
   {

      gnssTypeValue result;
      result.header = (*this).header;
      result.body = (*this).body.extractTypeID(type);

      return result;

   }  // End of method 'gnssTypeValue::extractTypeID()'



      // Returns a gnssTypeValue with only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be extracted.
   gnssTypeValue gnssTypeValue::extractTypeID(const TypeIDSet& typeSet) const
   {

      gnssTypeValue result;
      result.header = (*this).header;
      result.body = (*this).body.extractTypeID(typeSet);

      return result;

   }  // End of method 'gnssTypeValue::extractTypeID()'



      // Modifies this object, keeping only this type of data.
      // @param type Type of value to be kept.
   gnssTypeValue& gnssTypeValue::keepOnlyTypeID(const TypeID& type)
   {

      TypeIDSet typeSet;
      typeSet.insert(type);

      return keepOnlyTypeID(typeSet);

   }  // End of method 'gnssTypeValue::keepOnlyTypeID()'



      // Modifies this object, keeping only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   gnssTypeValue& gnssTypeValue::keepOnlyTypeID(const TypeIDSet& typeSet)
   {

      typeValueMap tvMap( (*this).body.extractTypeID(typeSet) );
      (*this).body = tvMap;

      return (*this);

   }  // End of method 'gnssTypeValue::keepOnlyTypeID()'



      // Modifies this object, removing these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   gnssTypeValue& gnssTypeValue::removeTypeID(const TypeIDSet& typeSet)
   {

      for( TypeIDSet::const_iterator pos = typeSet.begin();
           pos != typeSet.end();
           ++pos )
      {
         (*this).body.erase(*pos);
      }

      return (*this);

   }  // End of method 'gnssTypeValue::removeTypeID()'




      ////// gnssSatTypeValue //////


      // Returns a gnssSatTypeValue with only this satellite.
      // @param satellite Satellite to be extracted.
   gnssSatTypeValue gnssSatTypeValue::extractSatID(const SatID& satellite)
      const
   {

      gnssSatTypeValue result;
      result.header = (*this).header;
      result.body = (*this).body.extractSatID(satellite);

      return result;

   }  // End of method 'gnssSatTypeValue::extractSatID()'



      // Returns a gnssSatTypeValue with only one satellite, identified
      // by the given parameters.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   gnssSatTypeValue gnssSatTypeValue::extractSatID( const int& p,
                                       const SatID::SatelliteSystem& s ) const
   {

      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return extractSatID(tempSatellite);

   }  // End of method 'gnssSatTypeValue::extractSatID()'



      // Returns a gnssSatTypeValue with only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites
      //               to be extracted.
   gnssSatTypeValue gnssSatTypeValue::extractSatID(const SatIDSet& satSet) const
   {

      gnssSatTypeValue result;
      result.header = (*this).header;
      result.body = (*this).body.extractSatID(satSet);

      return result;

   }  // End of method 'gnssSatTypeValue::extractSatID()'



      // Modifies this object, keeping only this satellite.
      // @param satellite Satellite to be kept.
   gnssSatTypeValue& gnssSatTypeValue::keepOnlySatID(const SatID& satellite)
   {

      SatIDSet satSet;
      satSet.insert(satellite);

      return keepOnlySatID(satSet);

   }  // End of method 'gnssSatTypeValue::keepOnlySatID()'



      // Modifies this object, keeping only this satellite.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   gnssSatTypeValue& gnssSatTypeValue::keepOnlySatID( const int& p,
                                             const SatID::SatelliteSystem& s )
   {

      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return keepOnlySatID(tempSatellite);

   }  // End of method 'gnssSatTypeValue::keepOnlySatID()'



      // Modifies this object, keeping only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to be kept.
   gnssSatTypeValue& gnssSatTypeValue::keepOnlySatID(const SatIDSet& satSet)
   {

      satTypeValueMap stvMap( (*this).body.extractSatID(satSet) );
      (*this).body = stvMap;

      return (*this);

   }  // End of method 'gnssSatTypeValue::keepOnlySatID()'



      // Returns a gnssSatTypeValue with only this type of data.
      // @param type Type of value to be extracted.
   gnssSatTypeValue gnssSatTypeValue::extractTypeID(const TypeID& type) const
   {

      gnssSatTypeValue result;
      result.header = (*this).header;
      result.body = (*this).body.extractTypeID(type);

      return result;

   }  // End of method 'gnssSatTypeValue::extractTypeID()'



      // Returns a gnssSatTypeValue with only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be extracted.
   gnssSatTypeValue gnssSatTypeValue::extractTypeID(const TypeIDSet& typeSet)
      const
   {

      gnssSatTypeValue result;
      result.header = (*this).header;
      result.body = (*this).body.extractTypeID(typeSet);

      return result;

   }  // End of method 'gnssSatTypeValue::extractTypeID()'



      // Modifies this object, keeping only this type of data.
      // @param type Type of value to be kept.
   gnssSatTypeValue& gnssSatTypeValue::keepOnlyTypeID(const TypeID& type)
   {

      TypeIDSet typeSet;
      typeSet.insert(type);

      return keepOnlyTypeID(typeSet);

   }  // End of method 'gnssSatTypeValue::keepOnlyTypeID()'



      // Modifies this object, keeping only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   gnssSatTypeValue& gnssSatTypeValue::keepOnlyTypeID(const TypeIDSet& typeSet)
   {

      satTypeValueMap stvMap( (*this).body.extractTypeID(typeSet) );
      (*this).body = stvMap;

      return (*this);

   }  // End of method 'gnssSatTypeValue::keepOnlyTypeID()'



      // Modifies this object, removing these satellites.
      // @param satSet Set (SatIDSet) containing the satellites
      //               to be removed.
   gnssSatTypeValue& gnssSatTypeValue::removeSatID(const SatIDSet& satSet)
   {

      for( SatIDSet::const_iterator pos = satSet.begin();
           pos != satSet.end();
           ++pos )
      {
         (*this).body.erase(*pos);
      }

      return (*this);

   }  // End of method 'gnssSatTypeValue::removeSatID()'



      // Modifies this object, removing these types of data
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   gnssSatTypeValue& gnssSatTypeValue::removeTypeID(const TypeIDSet& typeSet)
   {

      for( TypeIDSet::const_iterator pos = typeSet.begin();
           pos != typeSet.end();
           ++pos )
      {
         (*this).body.removeTypeID(*pos);
      }

      return (*this);

   }  // End of method 'gnssSatTypeValue::removeTypeID()'




      ////// gnssRinex //////


      // Returns a gnssRinex with only this satellite.
      // @param satellite Satellite to be extracted.
   gnssRinex gnssRinex::extractSatID(const SatID& satellite) const
   {

      gnssRinex result;
      result.header = (*this).header;
      result.body = (*this).body.extractSatID(satellite);

      return result;

   }  // End of method 'gnssRinex::extractSatID()'



      // Returns a gnssRinex with only one satellite, identified by
      // the given parameters.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   gnssRinex gnssRinex::extractSatID( const int& p,
                                      const SatID::SatelliteSystem& s ) const
   {

      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return extractSatID(tempSatellite);

   }  // End of method 'gnssRinex::extractSatID()'



      // Returns a gnssRinex with only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites
      //               to be extracted.
   gnssRinex gnssRinex::extractSatID(const SatIDSet& satSet) const
   {

      gnssRinex result;
      result.header = (*this).header;
      result.body = (*this).body.extractSatID(satSet);

      return result;

   }  // End of method 'gnssRinex::extractSatID()'



      // Modifies this object, keeping only this satellite.
      // @param satellite Satellite to be kept.
   gnssRinex& gnssRinex::keepOnlySatID(const SatID& satellite)
   {

      SatIDSet satSet;
      satSet.insert(satellite);

      return keepOnlySatID(satSet);

   }  // End of method 'gnssRinex::keepOnlySatID()'



      // Modifies this object, keeping only this satellite.
      // @param p Satellite PRN number.
      // @param p System the satellite belongs to.
   gnssRinex& gnssRinex::keepOnlySatID( const int& p,
                                        const SatID::SatelliteSystem& s )
   {

      SatID tempSatellite(p, s);  // We build a temporary SatID object

      return keepOnlySatID(tempSatellite);

   }  // End of method 'gnssRinex::keepOnlySatID()'



      // Modifies this object, keeping only these satellites.
      // @param satSet Set (SatIDSet) containing the satellites to be kept.
   gnssRinex& gnssRinex::keepOnlySatID(const SatIDSet& satSet)
   {

      satTypeValueMap stvMap( (*this).body.extractSatID(satSet) );
      (*this).body = stvMap;

      return (*this);

   }  // End of method 'gnssRinex::keepOnlySatID()'



      // Returns a gnssRinex with only this type of data.
      // @param type Type of value to be extracted.
   gnssRinex gnssRinex::extractTypeID(const TypeID& type) const
   {

      gnssRinex result;
      result.header = (*this).header;
      result.body = (*this).body.extractTypeID(type);

      return result;

   }  // End of method 'gnssRinex::extractTypeID()'



      // Returns a gnssRinex with only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be extracted.
   gnssRinex gnssRinex::extractTypeID(const TypeIDSet& typeSet) const
   {

      gnssRinex result;
      result.header = (*this).header;
      result.body = (*this).body.extractTypeID(typeSet);

      return result;

   }  // End of method 'gnssRinex::extractTypeID()'



      // Modifies this object, keeping only this type of data.
      // @param type Type of value to be kept.
   gnssRinex& gnssRinex::keepOnlyTypeID(const TypeID& type)
   {

      TypeIDSet typeSet;
      typeSet.insert(type);

      return keepOnlyTypeID(typeSet);

   }  // End of method 'gnssRinex::keepOnlyTypeID()'



      // Modifies this object, keeping only these types of data.
      // @param typeSet Set (TypeIDSet) containing the types of data
      //                to be kept.
   gnssRinex& gnssRinex::keepOnlyTypeID(const TypeIDSet& typeSet)
   {

      satTypeValueMap stvMap( (*this).body.extractTypeID(typeSet) );
      (*this).body = stvMap;

      return (*this);

   }  // End of method 'gnssRinex::keepOnlyTypeID()'




      ////// Other stuff //////



      // Convenience output method for structure satTypeValueMap
   std::ostream& satTypeValueMap::dump( std::ostream& s,
                                        int mode ) const
   {

      for( satTypeValueMap::const_iterator it = (*this).begin();
           it!= (*this).end();
           it++ )
      {

            // First, print satellite (system and PRN)
         s << (*it).first << " ";

         for( typeValueMap::const_iterator itObs = (*it).second.begin();
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

   }  // End of method 'satTypeValueMap::dump()'



      // stream output for satTypeValueMap
   std::ostream& operator<<( std::ostream& s,
                             const satTypeValueMap& stvMap )
   {

      stvMap.dump(s);
      return s;

   }  // End of 'operator<<'



      // Input for gnssSatTypeValue from RinexObsHeader
   gnssSatTypeValue& operator>>( const RinexObsHeader& roh,
                                 gnssSatTypeValue& f )
   {

         // First, select the right system the data came from
      f.header.source.type = SatIDsystem2SourceIDtype(roh.system);

         // Set the proper name for the receiver
      f.header.source.sourceName = roh.markerName;

      return f;

   }  // End of 'operator>>'



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

   }  // End of 'operator>>'



      // Input for gnssSatTypeValue from RinexObsData
   gnssSatTypeValue& operator>>( const RinexObsData& rod,
                                 gnssSatTypeValue& f )
   {

         // Fill header epoch with the proper value
      f.header.epoch = rod.time;

         // Extract the observations map and store it in the body
      f.body = FillsatTypeValueMapwithRinexObsData(rod);

      return f;

   }  // End of 'operator>>'



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

   }  // End of 'operator>>'



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

               } // End of 'for (isv=1, ndx=0; ndx<numSvs; isv++, ndx++)'


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

                  }  // End of 'for(ndx=0, line_ndx=0; ndx < numObs; ...)'

               }  // End of 'for (isv=0; isv < numSvs; isv++)'

            }  // End of 'if( epochFlag==0 || ... )'

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



      // Convenience function to convert from SatID system to SourceID type.
      // @param sid Satellite ID.
   SourceID::SourceType SatIDsystem2SourceIDtype(const SatID& sid)
   {

         // Select the right system the data came from
      switch(sid.system)
      {
         case SatID::systemGPS:
            return SourceID::GPS;
            break;
         case SatID::systemGalileo:
            return SourceID::Galileo;
            break;
         case SatID::systemGlonass:
            return SourceID::Glonass;
            break;
         case SatID::systemGeosync:
            return SourceID::Geosync;
            break;
         case SatID::systemLEO:
            return SourceID::LEO;
            break;
         case SatID::systemTransit:
            return SourceID::Transit;
            break;
         case SatID::systemMixed:
            return SourceID::Mixed;
            break;
         default:
            return SourceID::Unknown;
      }

   } // End SatIDsystem2SourceIDtype(const SatID& sid)



      // Convenience function to fill a typeValueMap with data
      // from RinexObsTypeMap.
   typeValueMap FilltypeValueMapwithRinexObsTypeMap(
                                 const RinexObsData::RinexObsTypeMap& otmap )
   {

         // RinexObsTypeMap is a map from RinexObsType to RinexDatum:
         //   std::map<RinexObsHeader::RinexObsType, RinexDatum>

         // We will need a typeValueMap
      typeValueMap tvMap;

         // Let's visit the RinexObsTypeMap (RinexObsType -> RinexDatum)
      for( RinexObsData::RinexObsTypeMap::const_iterator itObs = otmap.begin();
           itObs!= otmap.end();
           ++itObs )
      {

         TypeID type( RinexType2TypeID( (*itObs).first ) );
         tvMap[ type ] = (*itObs).second.data;

            // If this is a phase measurement, let's store corresponding
            // LLI and SSI for this SV and frequency
            // Also, the values for phase measurements will be given in meters
         if( type == TypeID::L1 )
         {
            tvMap[TypeID::LLI1] = (*itObs).second.lli;
            tvMap[TypeID::SSI1] = (*itObs).second.ssi;
            tvMap[ type ] = tvMap[ type ] * L1_WAVELENGTH;
         }
         if( type == TypeID::L2 )
         {
            tvMap[TypeID::LLI2] = (*itObs).second.lli;
            tvMap[TypeID::SSI2] = (*itObs).second.ssi;
            tvMap[ type ] = tvMap[ type ] * L2_WAVELENGTH;
         }
         if( type == TypeID::L5 )
         {
            tvMap[TypeID::LLI5] = (*itObs).second.lli;
            tvMap[TypeID::SSI5] = (*itObs).second.ssi;
            tvMap[ type ] = tvMap[ type ] * L5_WAVELENGTH;
         }
         if( type == TypeID::L6 )
         {
            tvMap[TypeID::LLI6] = (*itObs).second.lli;
            tvMap[TypeID::SSI6] = (*itObs).second.ssi;
            tvMap[ type ] = tvMap[ type ] * L6_WAVELENGTH;
         }
         if( type == TypeID::L7 )
         {
            tvMap[TypeID::LLI7] = (*itObs).second.lli;
            tvMap[TypeID::SSI7] = (*itObs).second.ssi;
            tvMap[ type ] = tvMap[ type ] * L7_WAVELENGTH;
         }
         if( type == TypeID::L8 )
         {
            tvMap[TypeID::LLI8] = (*itObs).second.lli;
            tvMap[TypeID::SSI8] = (*itObs).second.ssi;
            tvMap[ type ] = tvMap[ type ] * L8_WAVELENGTH;
         }

      }  // End of "for( itObs = otmap.begin(); ..."

      return tvMap;

   } // End FilltypeValueMapwithRinexObsTypeMap()



      // Convenience function to fill a satTypeValueMap with data
      // from RinexObsData.
      // @param rod RinexObsData holding the data.
   satTypeValueMap FillsatTypeValueMapwithRinexObsData(const RinexObsData& rod)
   {

         // We need to declare a satTypeValueMap
      satTypeValueMap theMap;

         // Let's define the "it" iterator to visit the observations PRN map
         // RinexSatMap is a map from SatID to RinexObsTypeMap:
         //      std::map<SatID, RinexObsTypeMap>
      for( RinexObsData::RinexSatMap::const_iterator it = rod.obs.begin();
           it!= rod.obs.end();
           ++it )
      {
            // RinexObsTypeMap is a map from RinexObsType to RinexDatum:
            //   std::map<RinexObsHeader::RinexObsType, RinexDatum>
            // The "second" field of a RinexSatMap (it) is a
            // RinexObsTypeMap (otmap)
         RinexObsData::RinexObsTypeMap otmap = (*it).second;

         theMap[(*it).first] = FilltypeValueMapwithRinexObsTypeMap(otmap);

      }

      return theMap;

   } // End FillsatTypeValueMapwithRinexObsData(const RinexObsData& rod)



      /* This function constructs a DayTime object from the given parameters.
       *
       * @param line    the encoded time string found in the RINEX record.
       * @param hdr     the RINEX Observation Header object for the current
       *                RINEX file.
       */
   DayTime parseTime( const std::string& line,
                      const RinexObsHeader& hdr )
      throw(FFStreamError)
   {

      try
      {
            // check if the spaces are in the right place - an easy
            // way to check if there's corruption in the file
         if ( (line[0] != ' ')  ||
              (line[3] != ' ')  ||
              (line[6] != ' ')  ||
              (line[9] != ' ')  ||
              (line[12] != ' ') ||
              (line[15] != ' ') )
         {
            FFStreamError e("Invalid time format");
            GPSTK_THROW(e);
         }

            // if there's no time, just return a bad time
         if (line.substr(0,26) == std::string(26, ' '))
         {
            return DayTime(DayTime::BEGINNING_OF_TIME);
         }

         int year, month, day, hour, min;
         double sec;
         int yy = hdr.firstObs.year()/100;
         yy *= 100;

         year  = StringUtils::asInt(   line.substr(1,  2 ));
         month = StringUtils::asInt(   line.substr(4,  2 ));
         day   = StringUtils::asInt(   line.substr(7,  2 ));
         hour  = StringUtils::asInt(   line.substr(10, 2 ));
         min   = StringUtils::asInt(   line.substr(13, 2 ));
         sec   = StringUtils::asDouble(line.substr(15, 11));

            // Real Rinex has epochs 'yy mm dd hr 59 60.0'
            // surprisingly often....
         double ds=0;
         if(sec >= 60.) { ds=sec; sec=0.0; }
         DayTime rv(yy+year, month, day, hour, min, sec);
         if(ds != 0) rv += ds;

         return rv;
      }
         // string exceptions for substr are caught here
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + std::string(e.what()));
         GPSTK_THROW(err);
      }
      catch (gpstk::Exception& e)
      {
         std::string text;
         for(int i=0; i<(int)e.getTextCount(); i++) text += e.getText(i);
         FFStreamError err("gpstk::Exception in parseTime(): " + text);
         GPSTK_THROW(err);
      }

   }  // End of parseTime()


}  // End of namespace gpstk
