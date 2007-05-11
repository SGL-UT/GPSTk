/**
 * @file DataStructures.hpp
 * Set of several data structures to be used by other GPSTk classes.
 */

#ifndef DATA_STRUCTURES_GPSTK
#define DATA_STRUCTURES_GPSTK

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
//  Dagoberto Salazar - gAGE. 2007
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
#include "ModeledReferencePR.hpp"



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
     * In this regard, four basic indexes are considered enough to completely
     * identify any GNSS value:
     *
     *  - Receiver/Source (SourceID)
     *  - Epoch (DayTime)
     *  - Satellite (SatID)
     *  - Type of value (TypeID)
     *
     * Moreover, all the GNSS data structures have two main parts:
     *
     *  - Header: Containing the indexes that are common to all the values (sometimes
     * with some extra information).
     *
     *  - Body: Containing the GNSS values themselves, organized in std::maps.
     *
     * The general idea is to use the GNSS data structures like WHITE BOXES that 
     * are able to carry all the important data around in an easy way, in order to 
     * do something like the following to process GNSS data:
     *
     * @code
     *   RinexObsStream rin("bahr1620.04o");    // Create the input file stream
     *   gnssRinex gRin;                        // Declare a gnssRinex object
     *   ModeledPR modelPR;                     // Declare a ModeledReferencePR object
     *   SolverLMS solver;                      // Declare an object to apply LMS method
     *   Position refPosition(3633909.1016, 4425275.5033, 2799861.2736);  // Initial position
     *   Position solPosition;                  // Solution
     *
     *   // Feed the gRin data structure
     *   while(rin >> gRin) {
     *      gRin.keepOnlyTypeID(TypeID::C1) >> modelPR >> solver >> solPosition;
     *      cout << solPosition;
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


    /// Thrown when the number of data values and the number of corresponding 
    /// satellites does not match.
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
        inline size_t numTypes() const { return (*this).size(); }

        /// Returns a TypeIDSet with all the data types present in this object.
        inline TypeIDSet getTypeID() const
        {
            TypeIDSet typeSet;
            typeValueMap::const_iterator pos;
            for (pos = (*this).begin(); pos != (*this).end(); ++pos)
            {
                typeSet.insert( (*pos).first );
            }
            return typeSet;
        }

        /// Returns a typeValueMap with only this type of data.
        /// @param type Type of value to be extracted.
        inline typeValueMap extractTypeID(const TypeID& type) const
        {
            TypeIDSet typeSet;
            typeSet.insert(type);
            return (*this).extractTypeID(typeSet);
        }

        /// Returns a typeValueMap with only these types of data.
        /// @param typeSet Set (TypeIDSet) containing the types of data to be extracted.
        inline typeValueMap extractTypeID(const TypeIDSet& typeSet) const
        {
            typeValueMap tvMap;
            TypeIDSet::const_iterator pos;
            for (pos = typeSet.begin(); pos != typeSet.end(); ++pos)
            {
                typeValueMap::const_iterator itObs;
                itObs = (*this).find(*pos);
                if ( itObs != (*this).end() ) 
                {
                    tvMap[ (*itObs).first ] = (*itObs).second;
                };
            }
            return tvMap;
        }

        /// Modifies this object, keeping only this type of data.
        /// @param type Type of value to be kept.
        inline typeValueMap& keepOnlyTypeID(const TypeID& type)
        {
            TypeIDSet typeSet;
            typeSet.insert(type);
            return (*this).keepOnlyTypeID(typeSet);
        }

        /// Modifies this object, keeping only these types of data.
        /// @param typeSet Set (TypeIDSet) containing the types of data to be kept.
        inline typeValueMap& keepOnlyTypeID(const TypeIDSet& typeSet)
        {
            typeValueMap tvMap = (*this).extractTypeID(typeSet);
            (*this) = tvMap;
            return (*this);
        }

        /// Modifies this object, removing this type of data.
        /// @param type Type of value to be removed.
        inline typeValueMap& removeTypeID(const TypeID& type)
        {
            (*this).erase(type);
            return (*this);
        }

        /// Modifies this object, removing these types of data.
        /// @param typeSet Set (TypeIDSet) containing the types of data to be kept.
        inline typeValueMap& removeTypeID(const TypeIDSet& typeSet)
        {
            TypeIDSet::const_iterator pos;
            for (pos = typeSet.begin(); pos != typeSet.end(); ++pos)
            {
                (*this).erase(*pos);
            }
            return (*this);
        }

        /// Returns a reference to the data value (double) with corresponding type.
        /// @param type Type of value to be look for.
        inline double& operator()(const TypeID& type) throw(TypeIDNotFound)
        {
            typeValueMap::iterator itObs;
            itObs = (*this).find(type);
            if ( itObs != (*this).end() ) 
            {
                return (*itObs).second;
            } else GPSTK_THROW(TypeIDNotFound("TypeID not found in map"));
        }

        /// Destructor.
        virtual ~typeValueMap() {};

    };  // End typeValueMap


    /// Map holding SatID with corresponding numeric value.
    struct satValueMap : std::map<SatID, double>
    {

        /// Returns the number of satellites available.
        inline size_t numSats() const { return (*this).size(); }

        /// Returns a SatIDSet with all the satellites present in this object.
        inline SatIDSet getSatID() const
        {
            SatIDSet satSet;
            satValueMap::const_iterator pos;
            for (pos = (*this).begin(); pos != (*this).end(); ++pos)
            {
                satSet.insert( (*pos).first );
            }
            return satSet;
        }

        /// Returns a Vector with all the satellites present in this object.
        inline Vector<SatID> getVectorOfSatID() const
        {
            std::vector<SatID> temp;
            satValueMap::const_iterator pos;
            for (pos = (*this).begin(); pos != (*this).end(); ++pos)
            {
                temp.push_back( (*pos).first );
            }
            Vector<SatID> result;
            result = temp;
            return result;
        }

        /// Returns a satValueMap with only this satellite.
        /// @param satellite Satellite to be extracted.
        inline satValueMap extractSatID(const SatID& satellite) const
        {
            SatIDSet satSet;
            satSet.insert(satellite);
            return (*this).extractSatID(satSet);
        }

        /// Returns a satValueMap with only one satellite, identified by the given parameters.
        /// @param p Satellite PRN number.
        /// @param p System the satellite belongs to.
        inline satValueMap extractSatID(const int& p, const SatID::SatelliteSystem& s) const
        {
            SatID tempSatellite(p, s);  // We build a temporary SatID object
            return (*this).extractSatID(tempSatellite);
        }

        /// Returns a satValueMap with only these satellites.
        /// @param satSet Set (SatIDSet) containing the satellites to be extracted.
        inline satValueMap extractSatID(const SatIDSet& satSet) const
        {
            satValueMap svMap;
            SatIDSet::const_iterator pos;
            for (pos = satSet.begin(); pos != satSet.end(); ++pos)
            {
                satValueMap::const_iterator itObs;
                itObs = (*this).find(*pos);
                if ( itObs != (*this).end() ) 
                {
                    svMap[ (*itObs).first ] = (*itObs).second;
                };
            }
            return svMap;
        }

        /// Modifies this object, keeping only this satellite.
        /// @param satellite Satellite to be kept.
        inline satValueMap& keepOnlySatID(const SatID& satellite)
        {
            SatIDSet satSet;
            satSet.insert(satellite);
            return (*this).keepOnlySatID(satSet);
        }

        /// Modifies this object, keeping only this satellite.
        /// @param p Satellite PRN number.
        /// @param p System the satellite belongs to.
        inline satValueMap& keepOnlySatID(const int& p, const SatID::SatelliteSystem& s)
        {
            SatID tempSatellite(p, s);  // We build a temporary SatID object
            return (*this).keepOnlySatID(tempSatellite);
        }

        /// Modifies this object, keeping only these satellites.
        /// @param satSet Set (SatIDSet) containing the satellites to be kept.
        inline satValueMap& keepOnlySatID(const SatIDSet& satSet)
        {
            satValueMap svMap = (*this).extractSatID(satSet);
            (*this) = svMap;
            return (*this);
        }

        /// Modifies this object, removing this satellite.
        /// @param satellite Satellite to be removed.
        inline satValueMap& removeSatID(const SatID& satellite)
        {
            (*this).erase(satellite);
            return (*this);
        }

        /// Modifies this object, removing these satellites.
        /// @param satSet Set (SatIDSet) containing the satellites to be removed.
        inline satValueMap& removeSatID(const SatIDSet& satSet)
        {
            SatIDSet::const_iterator pos;
            for (pos = satSet.begin(); pos != satSet.end(); ++pos)
            {
                (*this).erase(*pos);
            }
            return (*this);
        }

        /// Returns a reference to the data value (double) with corresponding SatID.
        /// @param satellite Satellite to be look for.
        inline double& operator()(const SatID& satellite) throw(SatIDNotFound)
        {
            satValueMap::iterator itObs;
            itObs = (*this).find(satellite);
            if ( itObs != (*this).end() ) 
            {
                return (*itObs).second;
            } else GPSTK_THROW(SatIDNotFound("SatID not found in map"));
        }

        /// Destructor.
        virtual ~satValueMap() {};

    };  // End of satValueMap


    /// Map holding SatID with corresponding typeValueMap.
    struct satTypeValueMap : std::map<SatID, typeValueMap>
    {

        /// Returns the number of available satellites.
        inline size_t numSats() const { return (*this).size(); }

        /** Returns the total number of data elements in the map.
         * This method DOES NOT suppose that all the satellites have
         * the same number of type values.
         */
        inline size_t numElements() const 
        { 
            size_t numEle(0);
            satTypeValueMap::const_iterator it;
            for (it = (*this).begin(); it != (*this).end(); ++it) 
            {
                numEle = numEle + (*it).second.size();
            }
            return numEle;
        }

        /// Returns a SatIDSet with all the satellites present in this object.
        inline SatIDSet getSatID() const
        {
            SatIDSet satSet;
            satTypeValueMap::const_iterator pos;
            for (pos = (*this).begin(); pos != (*this).end(); ++pos)
            {
                satSet.insert( (*pos).first );
            }
            return satSet;
        }

        /// Returns a Vector with all the satellites present in this object.
        inline Vector<SatID> getVectorOfSatID() const
        {
            std::vector<SatID> temp;
            satTypeValueMap::const_iterator pos;
            for (pos = (*this).begin(); pos != (*this).end(); ++pos)
            {
                temp.push_back( (*pos).first );
            }
            Vector<SatID> result;
            result = temp;
            return result;
        }

        /// Returns a TypeIDSet with all the data types present in this object.
        inline TypeIDSet getTypeID() const
        {
            TypeIDSet typeSet;
            satTypeValueMap::const_iterator pos;
            for (pos = (*this).begin(); pos != (*this).end(); ++pos)
            {
                typeValueMap::const_iterator it;
                for (it = (*pos).second.begin(); it != (*pos).second.end(); ++it)
                {
                    typeSet.insert( (*it).first );
                }
            }
            return typeSet;
        }

        /// Returns a satTypeValueMap with only this satellite.
        /// @param satellite Satellite to be extracted.
        inline satTypeValueMap extractSatID(const SatID& satellite) const
        {
            SatIDSet satSet;
            satSet.insert(satellite);
            return (*this).extractSatID(satSet);
        };

        /// Returns a satTypeValueMap with only one satellite, identified by the given parameters.
        /// @param p Satellite PRN number.
        /// @param p System the satellite belongs to.
        inline satTypeValueMap extractSatID(const int& p, const SatID::SatelliteSystem& s) const
        {
            SatID tempSatellite(p, s);  // We build a temporary SatID object
            return (*this).extractSatID(tempSatellite);
        }

        /// Returns a satTypeValueMap with only these satellites.
        /// @param satSet Set (SatIDSet) containing the satellites to be extracted.
        inline satTypeValueMap extractSatID(const SatIDSet& satSet) const
        {
            satTypeValueMap stvMap;
            SatIDSet::const_iterator pos;
            for (pos = satSet.begin(); pos != satSet.end(); ++pos)
            {
                satTypeValueMap::const_iterator itObs;
                itObs = (*this).find(*pos);
                if ( itObs != (*this).end() ) 
                {
                    stvMap[ (*itObs).first ] = (*itObs).second;
                };
            }
            return stvMap;
        }

        /// Modifies this object, keeping only this satellite.
        /// @param satellite Satellite to be kept.
        inline satTypeValueMap& keepOnlySatID(const SatID& satellite)
        {
            SatIDSet satSet;
            satSet.insert(satellite);
            return (*this).keepOnlySatID(satSet);
        }

        /// Modifies this object, keeping only this satellite.
        /// @param p Satellite PRN number.
        /// @param p System the satellite belongs to.
        inline satTypeValueMap& keepOnlySatID(const int& p, const SatID::SatelliteSystem& s)
        {
            SatID tempSatellite(p, s);  // We build a temporary SatID object
            return (*this).keepOnlySatID(tempSatellite);
        }

        /// Modifies this object, keeping only these satellites.
        /// @param satSet Set (SatIDSet) containing the satellites to be kept.
        inline satTypeValueMap& keepOnlySatID(const SatIDSet& satSet)
        {
            satTypeValueMap stvMap = (*this).extractSatID(satSet);
            (*this) = stvMap;
            return (*this);
        }

        /// Returns a satTypeValueMap with only this type of value.
        /// @param type Type of value to be extracted.
        inline satTypeValueMap extractTypeID(const TypeID& type) const
        {
            TypeIDSet typeSet;
            typeSet.insert(type);
            return (*this).extractTypeID(typeSet);
        };

        /// Returns a satTypeValueMap with only these types of data.
        /// @param typeSet Set (TypeIDSet) containing the types of data to be extracted.
        inline satTypeValueMap extractTypeID(const TypeIDSet& typeSet) const
        {
            satTypeValueMap theMap;
            satTypeValueMap::const_iterator it;
            for (it = (*this).begin(); it != (*this).end(); ++it) 
            {
                typeValueMap tvMap = (*it).second.extractTypeID(typeSet);
                if( tvMap.size() > 0 ) { theMap[(*it).first] = tvMap; };
            };
            return theMap;
        };

        /// Modifies this object, keeping only this type of data.
        /// @param type Type of value to be kept.
        inline satTypeValueMap& keepOnlyTypeID(const TypeID& type)
        {
            TypeIDSet typeSet;
            typeSet.insert(type);
            return (*this).keepOnlyTypeID(typeSet);
        }

        /// Modifies this object, keeping only these types of data.
        /// @param typeSet Set (TypeIDSet) containing the types of data to be kept.
        inline satTypeValueMap& keepOnlyTypeID(const TypeIDSet& typeSet)
        {
            satTypeValueMap stvMap = (*this).extractTypeID(typeSet);
            (*this) = stvMap;
            return (*this);
        }

        /// Modifies this object, removing this satellite.
        /// @param satellite Satellite to be removed.
        inline satTypeValueMap& removeSatID(const SatID& satellite)
        {
            (*this).erase(satellite);
            return (*this);
        }

        /// Modifies this object, removing these satellites.
        /// @param satSet Set (SatIDSet) containing the satellites to be removed.
        inline satTypeValueMap& removeSatID(const SatIDSet& satSet)
        {
            SatIDSet::const_iterator pos;
            for (pos = satSet.begin(); pos != satSet.end(); ++pos)
            {
                (*this).erase(*pos);
            }
            return (*this);
        }

        /// Modifies this object, removing this type of data.
        /// @param type Type of value to be removed.
        inline satTypeValueMap& removeTypeID(const TypeID& type)
        {
            satTypeValueMap::iterator it;
            for (it = (*this).begin(); it != (*this).end(); ++it) 
            {
                (*it).second.removeTypeID(type);
            }
            return (*this);
        }

        /// Modifies this object, removing these types of data.
        /// @param typeSet Set (TypeIDSet) containing the types of data to be kept.
        inline satTypeValueMap& removeTypeID(const TypeIDSet& typeSet)
        {
            TypeIDSet::const_iterator pos;
            for (pos = typeSet.begin(); pos != typeSet.end(); ++pos)
            {
                (*this).removeTypeID(*pos);
            }
            return (*this);
        }

        /// Returns a GPSTk::Vector containing the data values with this type.
        /// @param type Type of value to be returned.
        inline Vector<double> getVectorOfTypeID(const TypeID& type)
        {
            std::vector<double> temp;
            typeValueMap::const_iterator itObs;
            satTypeValueMap::const_iterator it;
            for (it = (*this).begin(); it != (*this).end(); ++it) 
            {
                itObs = (*it).second.find(type);
                if ( itObs != (*it).second.end() ) temp.push_back( (*itObs).second );
            }
            Vector<double> result;
            result = temp;
            return result;
        }

        /** Modifies this object, adding one vector of data with this type, one value 
         * per satellite.
         *
         * If type already exists, data is overwritten. If the number of values does not
         * match with the number of satellites, a NumberOfSatsMismatch exception is thrown.
         *
         * Given that dataVector does not store information about the satellites the 
         * values correspond to, the user is held responsible for having the data values
         * stored in dataVector in the proper order regarding the SatIDs in this object.
         *
         * @param type          Type of data to be added.
         * @param dataVector    GPSTk Vector containing the data to be added.
         */
        inline satTypeValueMap& insertTypeIDVector(const TypeID& type, const Vector<double> dataVector) throw(NumberOfSatsMismatch)
        {
            if ( dataVector.size() == (*this).numSats() )
            {
                size_t pos = 0;
                satTypeValueMap::iterator it;
                for (it = (*this).begin(); it != (*this).end(); ++it) 
                {
                    (*it).second[type] = dataVector[pos];
                    ++pos;
                }
                return (*this);
            } else GPSTK_THROW(NumberOfSatsMismatch("Number of data values in vector and number of satellites do not match"));
        }

        /// Returns a reference to the typeValueMap with corresponding SatID.
        /// @param type Type of value to be look for.
        inline typeValueMap& operator()(const SatID& satellite) throw(SatIDNotFound)
        {
            satTypeValueMap::iterator itObs;
            itObs = (*this).find(satellite);
            if ( itObs != (*this).end() ) 
            {
                return (*itObs).second;
            } else GPSTK_THROW(SatIDNotFound("SatID not found in map"));
        }

        /// Destructor.
        virtual ~satTypeValueMap() {};

    };  // End of satTypeValueMap



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
        gnssData(const HEADER_CLASS& h, const BODY_CLASS& b)
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

    /// GNSS data structure with source, epoch and data type as header (common indexes) and satValueMap as body.
    struct  gnssSatValue : gnssData<sourceEpochTypeHeader, satValueMap>
    {

        /// Returns the number of satellites available in the body (a satValueMap).
        inline size_t numSats() const { return body.numSats(); };

        /// Returns a SatIDSet with all the satellites present in this object.
        inline SatIDSet getSatID() const
        {
            return (*this).body.getSatID();
        }

        /// Returns a Vector with all the satellites present in this object.
        inline Vector<SatID> getVectorOfSatID() const
        {
            return (*this).body.getVectorOfSatID();
        }

        /// Returns a gnssSatValue with only this satellite.
        /// @param satellite Satellite to be extracted.
        inline gnssSatValue extractSatID(const SatID& satellite) const
        {
            gnssSatValue result;
            result.header = (*this).header;
            result.body = (*this).body.extractSatID(satellite);
            return result;
        };

        /// Returns a gnssSatValue with only one satellite, identified by the given parameters.
        /// @param p Satellite PRN number.
        /// @param p System the satellite belongs to.
        inline gnssSatValue extractSatID(const int& p, const SatID::SatelliteSystem& s) const
        {
            SatID tempSatellite(p, s);  // We build a temporary SatID object
            return (*this).extractSatID(tempSatellite);
        };

        /// Returns a gnssSatValue with only these satellites.
        /// @param satSet Set (SatIDSet) containing the satellites to be extracted.
        inline gnssSatValue extractSatID(const SatIDSet& satSet) const
        {
            gnssSatValue result;
            result.header = (*this).header;
            result.body = (*this).body.extractSatID(satSet);
            return result;
        }

        /// Modifies this object, keeping only this satellite.
        /// @param satellite Satellite to be kept.
        inline gnssSatValue& keepOnlySatID(const SatID& satellite)
        {
            SatIDSet satSet;
            satSet.insert(satellite);
            return (*this).keepOnlySatID(satSet);
        }

        /// Modifies this object, keeping only this satellite.
        /// @param p Satellite PRN number.
        /// @param p System the satellite belongs to.
        inline gnssSatValue& keepOnlySatID(const int& p, const SatID::SatelliteSystem& s)
        {
            SatID tempSatellite(p, s);  // We build a temporary SatID object
            return (*this).keepOnlySatID(tempSatellite);
        }

        /// Modifies this object, keeping only these satellites.
        /// @param satSet Set (SatIDSet) containing the satellites to be kept.
        inline gnssSatValue& keepOnlySatID(const SatIDSet& satSet)
        {
            satValueMap svMap = (*this).body.extractSatID(satSet);
            (*this).body = svMap;
            return (*this);
        }

        /// Modifies this object, removing this satellite.
        /// @param satellite Satellite to be removed.
        inline gnssSatValue& removeSatID(const SatID& satellite)
        {
            (*this).body.erase(satellite);
            return (*this);
        }

        /// Modifies this object, removing these satellites.
        /// @param satSet Set (SatIDSet) containing the satellites to be removed.
        inline gnssSatValue& removeSatID(const SatIDSet& satSet)
        {
            SatIDSet::const_iterator pos;
            for (pos = satSet.begin(); pos != satSet.end(); ++pos)
            {
                (*this).body.erase(*pos);
            }
            return (*this);
        }

        /// Returns a reference to the value (double) with corresponding satellite.
        /// @param satellite Satellite to be look for.
        inline double& operator()(const SatID& satellite) throw(SatIDNotFound)
        {
            return (*this).body(satellite);
        }



        /// Destructor.
        virtual ~gnssSatValue() {};

    };  // End of gnssSatValue



    /// GNSS data structure with source, epoch and satellite as header (common indexes) and typeValueMap as body.
    struct  gnssTypeValue : gnssData<sourceEpochSatHeader, typeValueMap>
    {

        /// Returns the number of types available in the body (a typeValueMap).
        inline size_t numTypes() const { return body.numTypes(); };

        /// Returns a TypeIDSet with all the data types present in this object.
        inline TypeIDSet getTypeID() const
        {
            return (*this).body.getTypeID();
        }

        /// Returns a gnssTypeValue with only this type of data.
        /// @param type Type of value to be extracted.
        inline gnssTypeValue extractTypeID(const TypeID& type) const
        {
            gnssTypeValue result;
            result.header = (*this).header;
            result.body = (*this).body.extractTypeID(type);
            return result;
        };

        /// Returns a gnssTypeValue with only these types of data.
        /// @param typeSet Set (TypeIDSet) containing the types of data to be extracted.
        inline gnssTypeValue extractTypeID(const TypeIDSet& typeSet) const
        {
            gnssTypeValue result;
            result.header = (*this).header;
            result.body = (*this).body.extractTypeID(typeSet);
            return result;
        };

        /// Modifies this object, keeping only this type of data.
        /// @param type Type of value to be kept.
        inline gnssTypeValue& keepOnlyTypeID(const TypeID& type)
        {
            TypeIDSet typeSet;
            typeSet.insert(type);
            return (*this).keepOnlyTypeID(typeSet);
        }

        /// Modifies this object, keeping only these types of data.
        /// @param typeSet Set (TypeIDSet) containing the types of data to be kept.
        inline gnssTypeValue& keepOnlyTypeID(const TypeIDSet& typeSet)
        {
            typeValueMap tvMap = (*this).body.extractTypeID(typeSet);
            (*this).body = tvMap;
            return (*this);
        }

        /// Modifies this object, removing this type of data.
        /// @param type Type of value to be removed.
        inline gnssTypeValue& removeTypeID(const TypeID& type)
        {
            (*this).body.erase(type);
            return (*this);
        }

        /// Modifies this object, removing these types of data.
        /// @param typeSet Set (TypeIDSet) containing the types of data to be kept.
        inline gnssTypeValue& removeTypeID(const TypeIDSet& typeSet)
        {
            TypeIDSet::const_iterator pos;
            for (pos = typeSet.begin(); pos != typeSet.end(); ++pos)
            {
                (*this).body.erase(*pos);
            }
            return (*this);
        }

        /// Returns a reference to the value (double) with corresponding type.
        /// @param type TypeID to be look for.
        inline double& operator()(const TypeID& type) throw(TypeIDNotFound)
        {
            return (*this).body(type);
        }


        /// Destructor.
        virtual ~gnssTypeValue() {};

    };  // End of gnssTypeValue



    /// GNSS data structure with source and epoch as header (common indexes) and satTypeValueMap as body.
    struct  gnssSatTypeValue : gnssData<sourceEpochHeader, satTypeValueMap>
    {

        /// Returns the number of satellites available in the body (a satTypeValueMap).
        inline size_t numSats() const { return body.numSats(); };

        /// Returns a TypeIDSet with all the data types present in this object.
        inline TypeIDSet getTypeID() const
        {
            return (*this).body.getTypeID();
        }

        /// Returns a SatIDSet with all the satellites present in this object.
        inline SatIDSet getSatID() const
        {
            return (*this).body.getSatID();
        }

        /// Returns a Vector with all the satellites present in this object.
        inline Vector<SatID> getVectorOfSatID() const
        {
            return (*this).body.getVectorOfSatID();
        }

        /** Returns the total number of data elements in the body.
         * This method DOES NOT suppose that all the satellites have
         * the same number of type values.
         */
        inline size_t numElements() const { return body.numElements(); };

        /// Returns a gnssSatTypeValue with only this satellite.
        /// @param satellite Satellite to be extracted.
        inline gnssSatTypeValue extractSatID(const SatID& satellite) const
        {
            gnssSatTypeValue result;
            result.header = (*this).header;
            result.body = (*this).body.extractSatID(satellite);
            return result;
        };

        /// Returns a gnssSatTypeValue with only one satellite, identified by the given parameters.
        /// @param p Satellite PRN number.
        /// @param p System the satellite belongs to.
        inline gnssSatTypeValue extractSatID(const int& p, const SatID::SatelliteSystem& s) const
        {
            SatID tempSatellite(p, s);  // We build a temporary SatID object
            return (*this).extractSatID(tempSatellite);
        };

        /// Returns a gnssSatTypeValue with only these satellites.
        /// @param satSet Set (SatIDSet) containing the satellites to be extracted.
        inline gnssSatTypeValue extractSatID(const SatIDSet& satSet) const
        {
            gnssSatTypeValue result;
            result.header = (*this).header;
            result.body = (*this).body.extractSatID(satSet);
            return result;
        }

        /// Modifies this object, keeping only this satellite.
        /// @param satellite Satellite to be kept.
        inline gnssSatTypeValue& keepOnlySatID(const SatID& satellite)
        {
            SatIDSet satSet;
            satSet.insert(satellite);
            return (*this).keepOnlySatID(satSet);
        }

        /// Modifies this object, keeping only this satellite.
        /// @param p Satellite PRN number.
        /// @param p System the satellite belongs to.
        inline gnssSatTypeValue& keepOnlySatID(const int& p, const SatID::SatelliteSystem& s)
        {
            SatID tempSatellite(p, s);  // We build a temporary SatID object
            return (*this).keepOnlySatID(tempSatellite);
        }

        /// Modifies this object, keeping only these satellites.
        /// @param satSet Set (SatIDSet) containing the satellites to be kept.
        inline gnssSatTypeValue& keepOnlySatID(const SatIDSet& satSet)
        {
            satTypeValueMap stvMap = (*this).body.extractSatID(satSet);
            (*this).body = stvMap;
            return (*this);
        }

        /// Returns a gnssSatTypeValue with only this type of data.
        /// @param type Type of value to be extracted.
        inline gnssSatTypeValue extractTypeID(const TypeID& type) const
        {
            gnssSatTypeValue result;
            result.header = (*this).header;
            result.body = (*this).body.extractTypeID(type);
            return result;
        };

        /// Returns a gnssSatTypeValue with only these types of data.
        /// @param typeSet Set (TypeIDSet) containing the types of data to be extracted.
        inline gnssSatTypeValue extractTypeID(const TypeIDSet& typeSet) const
        {
            gnssSatTypeValue result;
            result.header = (*this).header;
            result.body = (*this).body.extractTypeID(typeSet);
            return result;
        };

        /// Modifies this object, keeping only this type of data.
        /// @param type Type of value to be kept.
        inline gnssSatTypeValue& keepOnlyTypeID(const TypeID& type)
        {
            TypeIDSet typeSet;
            typeSet.insert(type);
            return (*this).keepOnlyTypeID(typeSet);
        }

        /// Modifies this object, keeping only these types of data.
        /// @param typeSet Set (TypeIDSet) containing the types of data to be kept.
        inline gnssSatTypeValue& keepOnlyTypeID(const TypeIDSet& typeSet)
        {
            satTypeValueMap stvMap = (*this).body.extractTypeID(typeSet);
            (*this).body = stvMap;
            return (*this);
        }

        /// Modifies this object, removing this satellite.
        /// @param satellite Satellite to be removed.
        inline gnssSatTypeValue& removeSatID(const SatID& satellite)
        {
            (*this).body.erase(satellite);
            return (*this);
        }

        /// Modifies this object, removing these satellites.
        /// @param satSet Set (SatIDSet) containing the satellites to be removed.
        inline gnssSatTypeValue& removeSatID(const SatIDSet& satSet)
        {
            SatIDSet::const_iterator pos;
            for (pos = satSet.begin(); pos != satSet.end(); ++pos)
            {
                (*this).body.erase(*pos);
            }
            return (*this);
        }

        /// Modifies this object, removing this type of data.
        /// @param type Type of value to be kept.
        inline gnssSatTypeValue& removeTypeID(const TypeID& type)
        {
            (*this).body.removeTypeID(type);
            return (*this);
        }

        /// Modifies this object, removing these types of data
        /// @param typeSet Set (TypeIDSet) containing the types of data to be kept.
        inline gnssSatTypeValue& removeTypeID(const TypeIDSet& typeSet)
        {
            TypeIDSet::const_iterator pos;
            for (pos = typeSet.begin(); pos != typeSet.end(); ++pos)
            {
                (*this).body.removeTypeID(*pos);
            }
            return (*this);
        }

        /// Returns a GPSTk::Vector containing the data values with this type.
        /// @param type Type of value to be returned.
        inline Vector<double> getVectorOfTypeID(const TypeID& type)
        {
            return ( (*this).body.getVectorOfTypeID(type) );
        }


        /** Modifies this object, adding one vector of data with this type, one value 
         * per satellite.
         *
         * If type already exists, data is overwritten. If the number of values does not
         * match with the number of satellites, a NumberOfSatsMismatch exception is thrown.
         *
         * Given that dataVector does not store information about the satellites the 
         * values correspond to, the user is held responsible for having the data values
         * stored in dataVector in the proper order regarding the SatIDs in this object.
         *
         * @param type          Type of data to be added.
         * @param dataVector    GPSTk Vector containing the data to be added.
         */
        inline satTypeValueMap& insertTypeIDVector(const TypeID& type, const Vector<double> dataVector) throw(NumberOfSatsMismatch)
        {
            return (*this).body.insertTypeIDVector(type, dataVector);
        }


        /** Returns a reference to the typeValueMap with corresponding satellite.
         * This operator allows direct access to data values when chained with the
         * typeValueMap::operator() like this: gRin(sat21)(TypeID::C1).
         *
         * Example:
         *
         * @code
         *   RinexObsStream rin("bahr1620.04o");    // Create the input file stream
         *   gnssRinex gRin;                        // Declare a gnssRinex object
         *   SatID sat21(21,SatID::systemGPS);      // Create a satellite object
         *
         *   // Feed the gRin data structure
         *   while(rin >> gRin)
         *   {
         *      try
         *      {
         *          if (gRin(sat21)(TypeID::C1) == 0.0) gRin(sat21)(TypeID::C1) = 123.456;
         *          cout << "C1 value for satellite G21: " << gRin(sat21)(TypeID::C1) << endl;
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
        inline typeValueMap& operator()(const SatID& satellite) throw(SatIDNotFound)
        {
            return (*this).body(satellite);
        }


        /// Input operator from gnssSatTypeValue to ModeledReferencePR.
        virtual gnssSatTypeValue& operator>>(ModeledReferencePR& modRefPR) throw(Exception);


        /// Destructor.
        virtual ~gnssSatTypeValue() {};

    };  // End of gnssSatTypeValue



    /// GNSS data structure with source, epoch and extra Rinex data as header (common indexes) and satTypeValueMap as body.
    struct gnssRinex : gnssSatTypeValue
    {

        /// Header.
        sourceEpochRinexHeader header;

        /// Returns a gnssRinex with only this satellite.
        /// @param satellite Satellite to be extracted.
        inline gnssRinex extractSatID(const SatID& satellite) const
        {
            gnssRinex result;
            result.header = (*this).header;
            result.body = (*this).body.extractSatID(satellite);
            return result;

        };

        /// Returns a gnssRinex with only one satellite, identified by the given parameters.
        /// @param p Satellite PRN number.
        /// @param p System the satellite belongs to.
        inline gnssRinex extractSatID(const int& p, const SatID::SatelliteSystem& s) const
        {
            SatID tempSatellite(p, s);  // We build a temporary SatID object
            return (*this).extractSatID(tempSatellite);
        };

        /// Returns a gnssRinex with only these satellites.
        /// @param satSet Set (SatIDSet) containing the satellites to be extracted.
        inline gnssRinex extractSatID(const SatIDSet& satSet) const
        {
            gnssRinex result;
            result.header = (*this).header;
            result.body = (*this).body.extractSatID(satSet);
            return result;
        }

        /// Modifies this object, keeping only this satellite.
        /// @param satellite Satellite to be kept.
        inline gnssRinex& keepOnlySatID(const SatID& satellite)
        {
            SatIDSet satSet;
            satSet.insert(satellite);
            return (*this).keepOnlySatID(satSet);
        }

        /// Modifies this object, keeping only this satellite.
        /// @param p Satellite PRN number.
        /// @param p System the satellite belongs to.
        inline gnssRinex& keepOnlySatID(const int& p, const SatID::SatelliteSystem& s)
        {
            SatID tempSatellite(p, s);  // We build a temporary SatID object
            return (*this).keepOnlySatID(tempSatellite);
        }

        /// Modifies this object, keeping only these satellites.
        /// @param satSet Set (SatIDSet) containing the satellites to be kept.
        inline gnssRinex& keepOnlySatID(const SatIDSet& satSet)
        {
            satTypeValueMap stvMap = (*this).body.extractSatID(satSet);
            (*this).body = stvMap;
            return (*this);
        }

        /// Returns a gnssRinex with only this type of data.
        /// @param type Type of value to be extracted.
        inline gnssRinex extractTypeID(const TypeID& type) const
        {
            gnssRinex result;
            result.header = (*this).header;
            result.body = (*this).body.extractTypeID(type);
            return result;
        };

        /// Returns a gnssRinex with only these types of data.
        /// @param typeSet Set (TypeIDSet) containing the types of data to be extracted.
        inline gnssRinex extractTypeID(const TypeIDSet& typeSet) const
        {
            gnssRinex result;
            result.header = (*this).header;
            result.body = (*this).body.extractTypeID(typeSet);
            return result;

        };

        /// Modifies this object, keeping only this type of data.
        /// @param type Type of value to be kept.
        inline gnssRinex& keepOnlyTypeID(const TypeID& type)
        {
            TypeIDSet typeSet;
            typeSet.insert(type);
            return (*this).keepOnlyTypeID(typeSet);
        }

        /// Modifies this object, keeping only these types of data.
        /// @param typeSet Set (TypeIDSet) containing the types of data to be kept.
        inline gnssRinex& keepOnlyTypeID(const TypeIDSet& typeSet)
        {
            satTypeValueMap stvMap = (*this).body.extractTypeID(typeSet);
            (*this).body = stvMap;
            return (*this);
        }


        /// Destructor.
        virtual ~gnssRinex() {};

    };  // End of gnssRinex


    /// Stream input for gnssSatTypeValue.
    /// @param i Input stream.
    /// @param f gnssSatTypeValue receiving the data.
    std::istream& operator>>(std::istream& i, gnssSatTypeValue& f)
            throw(FFStreamError, gpstk::StringUtils::StringException);


    /// Input for gnssSatTypeValue from RinexObsHeader.
    /// @param roh RinexObsHeader holding the data.
    /// @param f gnssSatTypeValue receiving the data.
    gnssSatTypeValue& operator>>(const RinexObsHeader& roh, gnssSatTypeValue& f);

    /// Input for gnssSatTypeValue from RinexObsData.
    /// @param rod RinexObsData holding the data.
    /// @param f gnssSatTypeValue receiving the data.
    gnssSatTypeValue& operator>>(const RinexObsData& rod, gnssSatTypeValue& f);

    /// Input for gnssRinex from RinexObsHeader.
    /// @param roh RinexObsHeader holding the data.
    /// @param f gnssRinex receiving the data.
    gnssRinex& operator>>(const RinexObsHeader& roh, gnssRinex& f);

    /// Input for gnssRinex from RinexObsData.
    /// @param rod RinexObsData holding the data.
    /// @param f gnssRinex receiving the data.
    gnssRinex& operator>>(const RinexObsData& rod, gnssRinex& f);


    /// Convenience function to convert from SatID system to SourceID type.
    /// @param sid Satellite ID.
    inline SourceID::SourceType SatIDsystem2SourceIDtype(const SatID& sid)
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


    /// Convenience function to fill a typeValueMap with data from RinexObsTypeMap.
    inline typeValueMap FilltypeValueMapwithRinexObsTypeMap(const RinexObsData::RinexObsTypeMap& otmap)
    {
        // RinexObsTypeMap is a map from RinexObsType to RinexDatum:
        //   std::map<RinexObsHeader::RinexObsType, RinexDatum>
        // Let's define a iterator to visit the observations type map
        RinexObsData::RinexObsTypeMap::const_iterator itObs;

        // We will need a typeValueMap
        typeValueMap tvMap;

        // Let's visit the RinexObsTypeMap (RinexObsType -> RinexDatum)
        for (itObs = otmap.begin(); itObs!= otmap.end(); ++itObs)
        {

            TypeID type( RinexType2TypeID( (*itObs).first ) );
            tvMap[ type ] = (*itObs).second.data;

            // If this is a phase measurement, let's store corresponding LLI and SSI for this SV and frequency
            if (type == TypeID::L1)
            {
                tvMap[TypeID::LLI1] = (*itObs).second.lli;
                tvMap[TypeID::SSI1] = (*itObs).second.ssi; 
            }
            if (type == TypeID::L2)
            {
                tvMap[TypeID::LLI2] = (*itObs).second.lli;
                tvMap[TypeID::SSI2] = (*itObs).second.ssi; 
            }
            if (type == TypeID::L5)
            {
                tvMap[TypeID::LLI5] = (*itObs).second.lli;
                tvMap[TypeID::SSI5] = (*itObs).second.ssi; 
            }
            if (type == TypeID::L6)
            {
                tvMap[TypeID::LLI6] = (*itObs).second.lli;
                tvMap[TypeID::SSI6] = (*itObs).second.ssi; 
            }
            if (type == TypeID::L7)
            {
                tvMap[TypeID::LLI7] = (*itObs).second.lli;
                tvMap[TypeID::SSI7] = (*itObs).second.ssi; 
            }
            if (type == TypeID::L8)
            {
                tvMap[TypeID::LLI8] = (*itObs).second.lli;
                tvMap[TypeID::SSI8] = (*itObs).second.ssi; 
            }
        }

        return tvMap;

    } // End FilltypeValueMapwithRinexObsTypeMap(const RinexObsData::RinexObsTypeMap& otmap)


    /// Convenience function to fill a satTypeValueMap with data from RinexObsData.
    /// @param rod RinexObsData holding the data.
    inline satTypeValueMap FillsatTypeValueMapwithRinexObsData(const RinexObsData& rod)
    {

        // We need to declare a satTypeValueMap
        satTypeValueMap theMap;

        // Let's define the "it" iterator to visit the observations PRN map
        // RinexSatMap is a map from SatID to RinexObsTypeMap: 
        //      std::map<SatID, RinexObsTypeMap>
        RinexObsData::RinexSatMap::const_iterator it;
        for (it = rod.obs.begin(); it!= rod.obs.end(); ++it) 
        {
            // RinexObsTypeMap is a map from RinexObsType to RinexDatum:
            //   std::map<RinexObsHeader::RinexObsType, RinexDatum>
            // The "second" field of a RinexSatMap (it) is a RinexObsTypeMap (otmap)
            RinexObsData::RinexObsTypeMap otmap = (*it).second;

            theMap[(*it).first] = FilltypeValueMapwithRinexObsTypeMap(otmap);

        }

        return theMap;

    } // End FillsatTypeValueMapwithRinexObsData(const RinexObsData& rod)


    /** Stream input for gnssRinex.
     * This handy operator allows to fed a gnssRinex data structure directly from
     * an input stream such a RinexObsStream object. For example:
     *
     * @code
     *   RinexObsStream rin("bahr1620.04o");    // Create the input file stream
     *   gnssRinex gRin;                        // Declare a gnssRinex object
     *
     *   // Feed the gRin data structure
     *   while(rin >> gRin) {
     *      // Lots of stuff here...
     *   }
     * @endcode
     */
    std::istream& operator>>(std::istream& i, gnssRinex& f)
        throw(FFStreamError, gpstk::StringUtils::StringException);

    /**
     * This function constructs a DayTime object from the given parameters.
     * @param line the encoded time string found in the RINEX record.
     * @param hdr the RINEX Observation Header object for the current RINEX file.
     */
    inline DayTime parseTime(const std::string& line, const RinexObsHeader& hdr) throw(FFStreamError)
    {
      try
      {
            // check if the spaces are in the right place - an easy
            // way to check if there's corruption in the file
         if ( (line[0] != ' ') ||
              (line[3] != ' ') ||
              (line[6] != ' ') ||
              (line[9] != ' ') ||
              (line[12] != ' ') ||
              (line[15] != ' '))
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

         // Real Rinex has epochs 'yy mm dd hr 59 60.0' surprisingly often....
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

   }


   //@}
   
} // namespace gpstk
#endif
