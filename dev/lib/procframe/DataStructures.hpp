#pragma ident "$Id$"

/**
 * @file DataStructures.hpp
 * Set of several data structures to be used by other GPSTk classes.
 */

#ifndef DATASTRUCTURES_HPP
#define DATASTRUCTURES_HPP

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


#include <utility>
#include <vector>
#include <set>
#include <map>
#include <string>

#include "DataHeaders.hpp"
#include "FFData.hpp"
#include "RinexObsStream.hpp"
#include "RinexObsData.hpp"
#include "StringUtils.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"
#include "icd_200_constants.hpp"



namespace gpstk
{

      /** @defgroup DataStructures GPSTk data structures 
       *
       * This is a set of several data structures to be used by other 
       * GPSTk classes.
       *
       * Each data structure is composed of a header and a body. The header
       * contains the information that is common to all the data stored in
       * the structure, and the body contains the data themselves along with
       * the information (indexes) necessary to access them.
       *
       * In this regard, four basic indexes are considered enough to
       * completely identify any GNSS value:
       *
       *  \li Receiver/Source (SourceID)
       *  \li Epoch (DayTime)
       *  \li Satellite (SatID)
       *  \li Type of value (TypeID)
       *
       * Moreover, all the GNSS data structures have two main parts:
       *
       *  \li Header: Containing the indexes that are common to all the values
       *              (sometimes with some extra information).
       *
       *  \li Body: Containing the GNSS values themselves, organized in
       *            std::maps.
       *
       * The general idea is to use the GNSS data structures like WHITE BOXES
       * that are able to carry all the important data around in an easy way,
       * in order to do something like the following to process GNSS data:
       *
       * @code
       *   RinexObsStream rin("bahr1620.04o"); // Create the input file stream
       *   gnssRinex gRin;                     // Declare a gnssRinex object
       *
       *   ModeledPR modelPR;          // Declare a ModeledReferencePR object
       *   SolverLMS solver;           // Declare an object to apply LMS method
       *
       *
       *   // ... other inicialization code here ...
       *
       *
       *   // Feed the gRin data structure
       *   while(rin >> gRin)
       *   {
       *
       *      gRin.keepOnlyTypeID(TypeID::C1) >> modelPR >> solver;
       *
       *      // Print the results for this epoch
       *      cout << gRin.header.epoch.DOYsecond() << "  ";   // Epoch
       *      cout << solver.solution[0] << "  ";              // dx
       *      cout << solver.solution[1] << "  ";              // dy
       *      cout << solver.solution[2] << "  ";              // dz
       *
       *   }
       * @endcode
       *
       */

      //@{


      // First, we must declare some important exception objects


      /// Thrown when attempting to access a value and the corresponding TypeID
      /// does not exist in the map.
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(TypeIDNotFound, gpstk::Exception);


      /// Thrown when attempting to access a value and the corresponding SatID
      /// does not exist in the map.
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(SatIDNotFound, gpstk::Exception);


      /// Thrown when the number of data values and the number of
      /// corresponding types does not match.
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(NumberOfTypesMismatch, gpstk::Exception);


      /// Thrown when the number of data values and the number of
      /// corresponding satellites does not match.
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(NumberOfSatsMismatch, gpstk::Exception);


      // Now, some useful type definitions

      /// Set containing TypeID objects.
   typedef std::set<TypeID> TypeIDSet;

      /// Set containing SatID objects.
   typedef std::set<SatID> SatIDSet;



      /// Map holding TypeID with corresponding numeric value.
   struct typeValueMap : std::map<TypeID, double>
   {

         /// Returns the number of different types available.
      inline size_t numTypes() const
      { return (*this).size(); }


         /// Returns a TypeIDSet with all the data types present in 
         /// this object.
      TypeIDSet getTypeID() const;


         /// Returns a typeValueMap with only this type of data.
         /// @param type Type of value to be extracted.
      typeValueMap extractTypeID(const TypeID& type) const;


         /// Returns a typeValueMap with only these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data to 
         ///                be extracted.
      typeValueMap extractTypeID(const TypeIDSet& typeSet) const;


         /// Modifies this object, keeping only this type of data.
         /// @param type Type of value to be kept.
      typeValueMap& keepOnlyTypeID(const TypeID& type);


         /// Modifies this object, keeping only these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data 
         ///                to be kept.
      typeValueMap& keepOnlyTypeID(const TypeIDSet& typeSet);


         /// Modifies this object, removing this type of data.
         /// @param type Type of value to be removed.
      typeValueMap& removeTypeID(const TypeID& type)
      { (*this).erase(type); return (*this); }


         /// Modifies this object, removing these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data
         ///                to be kept.
      typeValueMap& removeTypeID(const TypeIDSet& typeSet);


         /// Returns a reference to the data value (double) with
         /// corresponding type.
         /// @param type Type of value to be looked for.
      double& operator()(const TypeID& type)
         throw(TypeIDNotFound);


         /// Destructor.
      virtual ~typeValueMap() {};

    };  // End typeValueMap



      /// Map holding SatID with corresponding numeric value.
   struct satValueMap : std::map<SatID, double>
   {

         /// Returns the number of satellites available.
      size_t numSats() const
      { return (*this).size(); }


         /// Returns a SatIDSet with all the satellites present in this object.
      SatIDSet getSatID() const;


         /// Returns a Vector with all the satellites present in this object.
      Vector<SatID> getVectorOfSatID() const;


         /// Returns a satValueMap with only this satellite.
         /// @param satellite Satellite to be extracted.
      satValueMap extractSatID(const SatID& satellite) const;


         /// Returns a satValueMap with only one satellite, identified by
         /// the given parameters.
         /// @param p Satellite PRN number.
         /// @param p System the satellite belongs to.
      satValueMap extractSatID( const int& p,
                                const SatID::SatelliteSystem& s ) const;


         /// Returns a satValueMap with only these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites to
         ///               be extracted.
      satValueMap extractSatID(const SatIDSet& satSet) const;


         /// Modifies this object, keeping only this satellite.
         /// @param satellite Satellite to be kept.
      satValueMap& keepOnlySatID(const SatID& satellite);


         /// Modifies this object, keeping only this satellite.
         /// @param p Satellite PRN number.
         /// @param p System the satellite belongs to.
      satValueMap& keepOnlySatID( const int& p,
                                  const SatID::SatelliteSystem& s );


         /// Modifies this object, keeping only these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites to be kept.
      satValueMap& keepOnlySatID(const SatIDSet& satSet);


         /// Modifies this object, removing this satellite.
         /// @param satellite Satellite to be removed.
      satValueMap& removeSatID(const SatID& satellite)
      { (*this).erase(satellite); return (*this); }


         /// Modifies this object, removing the given satellites.
         /// @param satSet Set (SatIDSet) containing the satellites to
         ///               be removed.
      satValueMap& removeSatID(const SatIDSet& satSet);


         /// Returns a reference to the data value (double) with
         /// corresponding SatID.
         /// @param satellite Satellite to be looked for.
      double& operator()(const SatID& satellite)
         throw(SatIDNotFound);


         /// Destructor.
      virtual ~satValueMap() {};

   };  // End of satValueMap



      /// Map holding SatID with corresponding typeValueMap.
   struct satTypeValueMap : std::map<SatID, typeValueMap>
   {

         /// Returns the number of available satellites.
      size_t numSats() const
      { return (*this).size(); }


         /** Returns the total number of data elements in the map.
          * This method DOES NOT suppose that all the satellites have
          * the same number of type values.
          */
      size_t numElements() const;


         /// Returns a SatIDSet with all the satellites present in this object.
      SatIDSet getSatID() const;


         /// Returns a Vector with all the satellites present in this object.
      Vector<SatID> getVectorOfSatID() const;


         /// Returns a TypeIDSet with all the data types present in
         /// this object.  This does not imply that all satellites have
         /// these types.
      TypeIDSet getTypeID() const;


         /// Returns a satTypeValueMap with only this satellite.
         /// @param satellite Satellite to be extracted.
      satTypeValueMap extractSatID(const SatID& satellite) const;


         /// Returns a satTypeValueMap with only one satellite, identified
         /// by the given parameters.
         /// @param p Satellite PRN number.
         /// @param p System the satellite belongs to.
      satTypeValueMap extractSatID( const int& p,
                                    const SatID::SatelliteSystem& s) const;


         /// Returns a satTypeValueMap with only these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites to
         ///               be extracted.
      satTypeValueMap extractSatID(const SatIDSet& satSet) const;


         /// Modifies this object, keeping only this satellite.
         /// @param satellite Satellite to be kept.
      satTypeValueMap& keepOnlySatID(const SatID& satellite);


         /// Modifies this object, keeping only this satellite.
         /// @param p Satellite PRN number.
         /// @param p System the satellite belongs to.
      satTypeValueMap& keepOnlySatID( const int& p,
                                      const SatID::SatelliteSystem& s );


         /// Modifies this object, keeping only these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites to be kept.
      satTypeValueMap& keepOnlySatID(const SatIDSet& satSet);


         /// Returns a satTypeValueMap with only this type of value.
         /// @param type Type of value to be extracted.
      satTypeValueMap extractTypeID(const TypeID& type) const;


         /// Returns a satTypeValueMap with only these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data
         ///                to be extracted.
      satTypeValueMap extractTypeID(const TypeIDSet& typeSet) const;


         /// Modifies this object, keeping only this type of data.
         /// @param type Type of value to be kept.
      satTypeValueMap& keepOnlyTypeID(const TypeID& type);


         /// Modifies this object, keeping only these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data
         ///                to be kept.
      satTypeValueMap& keepOnlyTypeID(const TypeIDSet& typeSet);


         /// Modifies this object, removing this satellite.
         /// @param satellite Satellite to be removed.
      satTypeValueMap& removeSatID(const SatID& satellite)
      { (*this).erase(satellite); return (*this); }


         /// Modifies this object, removing these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites
         ///               to be removed.
      satTypeValueMap& removeSatID(const SatIDSet& satSet);


         /// Modifies this object, removing this type of data.
         /// @param type Type of value to be removed.
      satTypeValueMap& removeTypeID(const TypeID& type);


         /// Modifies this object, removing these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data
         ///                to be kept.
      satTypeValueMap& removeTypeID(const TypeIDSet& typeSet);


         /// Returns a GPSTk::Vector containing the data values with this type.
         /// @param type Type of value to be returned.
         /// This method returns zero if a given satellite does not have
         /// this type.
      Vector<double> getVectorOfTypeID(const TypeID& type) const;


         /// Returns a GPSTk::Matrix containing the data values in this set.
         /// @param typeSet  TypeIDSet of values to be returned.
      Matrix<double> getMatrixOfTypes(const TypeIDSet& typeSet) const;


         /** Modifies this object, adding one vector of data with this type,
          *  one value per satellite.
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
      satTypeValueMap& insertTypeIDVector( const TypeID& type,
                                           const Vector<double> dataVector )
         throw(NumberOfSatsMismatch);


         /** Modifies this object, adding a matrix of data, one vector
          *  per satellite.
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
      satTypeValueMap& insertMatrix( const TypeIDSet& typeSet,
                                     const Matrix<double> dataMatrix )
         throw(NumberOfSatsMismatch, NumberOfTypesMismatch);


         /// Returns a reference to the typeValueMap with corresponding SatID.
         /// @param type Type of value to be look for.
      typeValueMap& operator()(const SatID& satellite)
         throw(SatIDNotFound);


         /// Convenience output method
      virtual std::ostream& dump( std::ostream& s,
                                  int mode = 0) const;


         /// Destructor.
      virtual ~satTypeValueMap() {};

   };  // End of satTypeValueMap



      /// stream output for satTypeValueMap
   std::ostream& operator<<( std::ostream& s,
                             const satTypeValueMap& stvMap);



      /// Map holding epoch with corresponding satTypeValueMap.
   typedef std::map<DayTime, satTypeValueMap>  epochSatTypeValueMap;

      /// Map holding epoch with corresponding satValueMap.
   typedef std::map<DayTime, satValueMap>      epochSatValueMap;

      /// Map holding epoch with corresponding typeValueMap.
   typedef std::map<DayTime, typeValueMap>     epochTypeValueMap;




      /// Basic gnssData structure.
   template <class HEADER_CLASS, class BODY_CLASS>
   struct gnssData
   {

         /// Header.
      HEADER_CLASS header;


         /// Body.
      BODY_CLASS   body;



         /// Default constructor.
      gnssData() {}


         /// Common constructor.
      gnssData( const HEADER_CLASS& h,
                const BODY_CLASS& b )
      {
         header = h;
         body = b;
      }


         /// Copy constructor.
      template<class H, class B>
      gnssData(const gnssData<H,B>& g)
      {
         header = g.header;
         body = g.body;
      }


         /// Destructor.
      virtual ~gnssData() {};

   };  // End of gnssData




      // Further type definitions

      /// GNSS data structure with source, epoch and data type as header
      /// (common indexes) and satValueMap as body.
   struct  gnssSatValue : gnssData<sourceEpochTypeHeader, satValueMap>
   {


         /// Returns the number of satellites available in the body,
         /// which is a satValueMap.
      size_t numSats() const
      { return body.numSats(); };


         /// Returns a SatIDSet with all the satellites present in this object.
      SatIDSet getSatID() const
      { return (*this).body.getSatID(); }


         /// Returns a Vector with all the satellites present in this object.
      Vector<SatID> getVectorOfSatID() const
      { return body.getVectorOfSatID(); }


         /// Returns a gnssSatValue with only this satellite.
         /// @param satellite Satellite to be extracted.
      gnssSatValue extractSatID(const SatID& satellite) const;


         /// Returns a gnssSatValue with only one satellite, identified by
         /// the given parameters.
         /// @param p Satellite PRN number.
         /// @param p System the satellite belongs to.
      gnssSatValue extractSatID( const int& p,
                                 const SatID::SatelliteSystem& s ) const;


         /// Returns a gnssSatValue with only these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites
         ///               to be extracted.
      gnssSatValue extractSatID(const SatIDSet& satSet) const;


         /// Modifies this object, keeping only this satellite.
         /// @param satellite Satellite to be kept.
      gnssSatValue& keepOnlySatID(const SatID& satellite);


         /// Modifies this object, keeping only this satellite.
         /// @param p Satellite PRN number.
         /// @param p System the satellite belongs to.
      gnssSatValue& keepOnlySatID( const int& p,
                                   const SatID::SatelliteSystem& s );


         /// Modifies this object, keeping only these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites to be kept.
      gnssSatValue& keepOnlySatID(const SatIDSet& satSet);


         /// Modifies this object, removing this satellite.
         /// @param satellite Satellite to be removed.
      gnssSatValue& removeSatID(const SatID& satellite)
      { (*this).body.erase(satellite); return (*this); }


         /// Modifies this object, removing these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites
         ///               to be removed.
      gnssSatValue& removeSatID(const SatIDSet& satSet);


         /// Returns a reference to the value (double) with corresponding
         /// satellite.
         /// @param satellite Satellite to be looked for.
      double& operator()(const SatID& satellite)
         throw(SatIDNotFound)
      { return (*this).body(satellite); }


         /// Destructor.
      virtual ~gnssSatValue() {};


   };  // End of gnssSatValue




      /// GNSS data structure with source, epoch and satellite as header
      /// (common indexes) and typeValueMap as body.
   struct  gnssTypeValue : gnssData<sourceEpochSatHeader, typeValueMap>
   {

         /// Returns the number of types available in the body,
         /// which is a typeValueMap.
      size_t numTypes() const
      { return body.numTypes(); };


         /// Returns a TypeIDSet with all the data types present
         /// in this object.
      TypeIDSet getTypeID() const
      { return (*this).body.getTypeID(); }


         /// Returns a gnssTypeValue with only this type of data.
         /// @param type Type of value to be extracted.
      gnssTypeValue extractTypeID(const TypeID& type) const;


         /// Returns a gnssTypeValue with only these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data
         ///                to be extracted.
      gnssTypeValue extractTypeID(const TypeIDSet& typeSet) const;


         /// Modifies this object, keeping only this type of data.
         /// @param type Type of value to be kept.
      gnssTypeValue& keepOnlyTypeID(const TypeID& type);


         /// Modifies this object, keeping only these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data
         ///                to be kept.
      gnssTypeValue& keepOnlyTypeID(const TypeIDSet& typeSet);


         /// Modifies this object, removing this type of data.
         /// @param type Type of value to be removed.
      gnssTypeValue& removeTypeID(const TypeID& type)
      { (*this).body.erase(type); return (*this); }


         /// Modifies this object, removing these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data
         ///                to be kept.
      gnssTypeValue& removeTypeID(const TypeIDSet& typeSet);


         /// Returns a reference to the value (double) with corresponding type.
         /// @param type TypeID to be looked for.
      double& operator()(const TypeID& type)
         throw(TypeIDNotFound)
      { return (*this).body(type); }


         /// Destructor.
      virtual ~gnssTypeValue() {};


   };  // End of gnssTypeValue



      /// GNSS data structure with source and epoch as header
      /// (common indexes) and satTypeValueMap as body.
   struct  gnssSatTypeValue : gnssData<sourceEpochHeader, satTypeValueMap>
   {

         /// Returns the number of satellites available in the body,
         /// which is a satTypeValueMap.
      size_t numSats() const
      { return body.numSats(); };


         /// Returns a TypeIDSet with all the data types present in
         /// this object.
      TypeIDSet getTypeID() const
      { return (*this).body.getTypeID(); }


         /// Returns a SatIDSet with all the satellites present in this object.
      SatIDSet getSatID() const
      { return (*this).body.getSatID(); }


         /// Returns a Vector with all the satellites present in this object.
      Vector<SatID> getVectorOfSatID() const
      { return (*this).body.getVectorOfSatID(); }


         /** Returns the total number of data elements in the body.
          * This method DOES NOT suppose that all the satellites have
          * the same number of type values.
          */
      size_t numElements() const
      { return body.numElements(); };


         /// Returns a gnssSatTypeValue with only this satellite.
         /// @param satellite Satellite to be extracted.
      gnssSatTypeValue extractSatID(const SatID& satellite) const;


         /// Returns a gnssSatTypeValue with only one satellite, identified
         /// by the given parameters.
         /// @param p Satellite PRN number.
         /// @param p System the satellite belongs to.
      gnssSatTypeValue extractSatID( const int& p,
                                     const SatID::SatelliteSystem& s ) const;


         /// Returns a gnssSatTypeValue with only these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites
         ///               to be extracted.
      gnssSatTypeValue extractSatID(const SatIDSet& satSet) const;


         /// Modifies this object, keeping only this satellite.
         /// @param satellite Satellite to be kept.
      gnssSatTypeValue& keepOnlySatID(const SatID& satellite);


         /// Modifies this object, keeping only this satellite.
         /// @param p Satellite PRN number.
         /// @param p System the satellite belongs to.
      gnssSatTypeValue& keepOnlySatID( const int& p,
                                       const SatID::SatelliteSystem& s );


         /// Modifies this object, keeping only these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites to be kept.
      gnssSatTypeValue& keepOnlySatID(const SatIDSet& satSet);


         /// Returns a gnssSatTypeValue with only this type of data.
         /// @param type Type of value to be extracted.
      gnssSatTypeValue extractTypeID(const TypeID& type) const;


         /// Returns a gnssSatTypeValue with only these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data
         ///                to be extracted.
      gnssSatTypeValue extractTypeID(const TypeIDSet& typeSet) const;


         /// Modifies this object, keeping only this type of data.
         /// @param type Type of value to be kept.
      gnssSatTypeValue& keepOnlyTypeID(const TypeID& type);


         /// Modifies this object, keeping only these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data
         ///                to be kept.
      gnssSatTypeValue& keepOnlyTypeID(const TypeIDSet& typeSet);


         /// Modifies this object, removing this satellite.
         /// @param satellite Satellite to be removed.
      gnssSatTypeValue& removeSatID(const SatID& satellite)
      { (*this).body.erase(satellite); return (*this); }


         /// Modifies this object, removing these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites
         ///               to be removed.
      gnssSatTypeValue& removeSatID(const SatIDSet& satSet);


         /// Modifies this object, removing this type of data.
         /// @param type Type of value to be kept.
      gnssSatTypeValue& removeTypeID(const TypeID& type)
      { (*this).body.removeTypeID(type); return (*this); }


         /// Modifies this object, removing these types of data
         /// @param typeSet Set (TypeIDSet) containing the types of data
         ///                to be kept.
      gnssSatTypeValue& removeTypeID(const TypeIDSet& typeSet);


         /// Returns a GPSTk::Vector containing the data values with this type.
         /// @param type Type of value to be returned.
      Vector<double> getVectorOfTypeID(const TypeID& type) const
      { return ( (*this).body.getVectorOfTypeID(type) ); }


         /** Modifies this object, adding one vector of data with this type,
          *  one value per satellite.
          *
          * If type already exists, data is overwritten. If the number of
          * values does not match with the number of satellites, a
          * NumberOfSatsMismatch exception is thrown.
          *
          * Given that dataVector does not store information about the
          * satellites the values correspond to, the user is held responsible
          * for having the data values stored in dataVector in the proper order
          * regarding the SatIDs in this object.
          *
          * @param type          Type of data to be added.
          * @param dataVector    GPSTk Vector containing the data to be added.
          */
      gnssSatTypeValue& insertTypeIDVector( const TypeID& type,
                                            const Vector<double> dataVector )
         throw(NumberOfSatsMismatch)
      { (*this).body.insertTypeIDVector(type, dataVector); return (*this); }


         /** Modifies this object, adding a matrix of data, one vector
          *  per satellite.
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
          * @param typeSet    Set (TypeIDSet) containing the types of data
          *                   to be added.
          * @param dataMatrix GPSTk Matrix containing the data to be added.
          */
      gnssSatTypeValue& insertMatrix( const TypeIDSet& typeSet,
                                      const Matrix<double> dataMatrix )
         throw(NumberOfSatsMismatch, NumberOfTypesMismatch)
      { (*this).body.insertMatrix(typeSet, dataMatrix); return (*this); }


         /** Returns a reference to the typeValueMap with corresponding
          *  satellite.
          *
          * This operator allows direct access to data values when chained
          * with the typeValueMap::operator(), like this:
          *
          *    gRin(sat21)(TypeID::C1).
          *
          * Example:
          *
          * @code
          *   // Create the input file stream
          *   RinexObsStream rin("bahr1620.04o");
          *
          *   // Declare a gnssRinex object
          *   gnssRinex gRin;
          *
          *   // Create a satellite object
          *   SatID sat21(21,SatID::systemGPS);
          *
          *   // Feed the gRin data structure
          *   while(rin >> gRin)
          *   {
          *      try
          *      {
          *          if (gRin(sat21)(TypeID::C1) == 0.0)
          *          {
          *             gRin(sat21)(TypeID::C1) = 123.456;
          *          }
          *
          *          cout << "C1 value for satellite G21: "
          *               << gRin(sat21)(TypeID::C1) << endl;
          *      }
          *      catch (SatIDNotFound& e)
          *      {
          *          cout << endl << "Satellite G21 not found." << endl;
          *      };
          *   }
          * @endcode
          *
          * @param satellite Satellite to be looked for.
          */
      typeValueMap& operator()(const SatID& satellite)
         throw(SatIDNotFound)
      { return (*this).body(satellite); }


         /// Destructor.
      virtual ~gnssSatTypeValue() {};


   };  // End of gnssSatTypeValue



      /// GNSS data structure with source, epoch and extra Rinex data as
      /// header (common indexes) and satTypeValueMap as body.
   struct gnssRinex : gnssSatTypeValue
   {


         /// Header.
      sourceEpochRinexHeader header;


         /// Returns a gnssRinex with only this satellite.
         /// @param satellite Satellite to be extracted.
      gnssRinex extractSatID(const SatID& satellite) const;


         /// Returns a gnssRinex with only one satellite, identified by
         /// the given parameters.
         /// @param p Satellite PRN number.
         /// @param p System the satellite belongs to.
      gnssRinex extractSatID( const int& p,
                              const SatID::SatelliteSystem& s ) const;


         /// Returns a gnssRinex with only these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites
         ///               to be extracted.
      gnssRinex extractSatID(const SatIDSet& satSet) const;


         /// Modifies this object, keeping only this satellite.
         /// @param satellite Satellite to be kept.
      gnssRinex& keepOnlySatID(const SatID& satellite);


         /// Modifies this object, keeping only this satellite.
         /// @param p Satellite PRN number.
         /// @param p System the satellite belongs to.
      gnssRinex& keepOnlySatID( const int& p,
                                const SatID::SatelliteSystem& s );


         /// Modifies this object, keeping only these satellites.
         /// @param satSet Set (SatIDSet) containing the satellites to be kept.
      gnssRinex& keepOnlySatID(const SatIDSet& satSet);


         /// Returns a gnssRinex with only this type of data.
         /// @param type Type of value to be extracted.
      gnssRinex extractTypeID(const TypeID& type) const;


         /// Returns a gnssRinex with only these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data
         ///                to be extracted.
      gnssRinex extractTypeID(const TypeIDSet& typeSet) const;


         /// Modifies this object, keeping only this type of data.
         /// @param type Type of value to be kept.
      gnssRinex& keepOnlyTypeID(const TypeID& type);


         /// Modifies this object, keeping only these types of data.
         /// @param typeSet Set (TypeIDSet) containing the types of data
         ///                to be kept.
      gnssRinex& keepOnlyTypeID(const TypeIDSet& typeSet);


         /// Destructor.
      virtual ~gnssRinex() {};


    };  // End of gnssRinex



      /// Object defining the structure of a GNSS equation. The header is the
      /// prefit and the body is a TypeIDSet containing the unknowns.
   struct  gnssEquationDefinition : gnssData<TypeID, TypeIDSet>
   {

         /// Default constructor.
      gnssEquationDefinition() {};


         /// Common constructor.
      gnssEquationDefinition(const TypeID& h, const TypeIDSet& b)
      {
         header = h;
         body   = b;
      }


         /// Destructor.
      virtual ~gnssEquationDefinition() {};


   };  // End of gnssEquationDefinition



      /// Object defining the structure of a GNSS linear combination. The
      /// header is the result type and the body is a typeValueMap containing
      /// the GNSS data types to be combined plus corresponding coefficients.
   struct  gnssLinearCombination : gnssData<TypeID, typeValueMap>
   {

         /// Default constructor.
      gnssLinearCombination() {};


         /// Common constructor.
      gnssLinearCombination(const TypeID& h, const typeValueMap& b)
      {
         header = h;
         body   = b;
      }


         /// Destructor.
      virtual ~gnssLinearCombination() {};


   };  // End of gnssLinearCombination


      /// List containing gnssLinearCombination objects.
   typedef std::list<gnssLinearCombination> LinearCombList;



      /// Stream input for gnssSatTypeValue.
      /// @param i Input stream.
      /// @param f gnssSatTypeValue receiving the data.
   std::istream& operator>>( std::istream& i,
                             gnssSatTypeValue& f)
      throw(FFStreamError, gpstk::StringUtils::StringException);


      /// Input for gnssSatTypeValue from RinexObsHeader.
      /// @param roh RinexObsHeader holding the data.
      /// @param f gnssSatTypeValue receiving the data.
   gnssSatTypeValue& operator>>( const RinexObsHeader& roh,
                                 gnssSatTypeValue& f );


      /// Input for gnssSatTypeValue from RinexObsData.
      /// @param rod RinexObsData holding the data.
      /// @param f gnssSatTypeValue receiving the data.
   gnssSatTypeValue& operator>>( const RinexObsData& rod,
                                 gnssSatTypeValue& f );


      /// Input for gnssRinex from RinexObsHeader.
      /// @param roh RinexObsHeader holding the data.
      /// @param f gnssRinex receiving the data.
   gnssRinex& operator>>( const RinexObsHeader& roh,
                          gnssRinex& f );


      /// Input for gnssRinex from RinexObsData.
      /// @param rod RinexObsData holding the data.
      /// @param f gnssRinex receiving the data.
   gnssRinex& operator>>( const RinexObsData& rod,
                          gnssRinex& f );


      /// Convenience function to convert from SatID system to SourceID type.
      /// @param sid Satellite ID.
   SourceID::SourceType SatIDsystem2SourceIDtype(const SatID& sid);


      /// Convenience function to fill a typeValueMap with data
      /// from RinexObsTypeMap.
   typeValueMap FilltypeValueMapwithRinexObsTypeMap(
                                 const RinexObsData::RinexObsTypeMap& otmap );


      /// Convenience function to fill a satTypeValueMap with data
      /// from RinexObsData.
      /// @param rod RinexObsData holding the data.
   satTypeValueMap FillsatTypeValueMapwithRinexObsData( 
                                                   const RinexObsData& rod );


      /** Stream input for gnssRinex.
       *
       * This handy operator allows to fed a gnssRinex data structure
       * directly from an input stream such a RinexObsStream object.
       *
       * For example:
       *
       * @code
       *   // Create the input file stream
       *   RinexObsStream rin("bahr1620.04o");
       *
       *   // Declare a gnssRinex object
       *   gnssRinex gRin;
       *
       *   // Feed the gRin data structure
       *   while( rin >> gRin )
       *   {
       *       // Lots of stuff in here...
       *   }
       * @endcode
       */
   std::istream& operator>>( std::istream& i,
                             gnssRinex& f )
      throw(FFStreamError, gpstk::StringUtils::StringException);


      /** This function constructs a DayTime object from the given parameters.
       * 
       * @param line    the encoded time string found in the RINEX record.
       * @param hdr     the RINEX Observation Header object for the current
       *                RINEX file.
       */
   DayTime parseTime( const std::string& line,
                      const RinexObsHeader& hdr )
      throw(FFStreamError);


      //@}
   
} // namespace gpstk
#endif // DATASTRUCTURES_HPP
