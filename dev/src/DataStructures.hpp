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
//  Dagoberto Salazar - gAGE. 2006
//
//============================================================================


#include <map>
#include <string>

#include "SatID.hpp"
#include "RinexObsHeader.hpp"
#include "Position.hpp"


namespace gpstk
{
    /** @defgroup DataStructures GPSTk data structures */

   //@{

    /** Set of several data structures to be used by other GPSTk classes.
     */
 

    /// A structure used to store a single generic data point.
    struct BasicDatum
    {
        BasicDatum(): data(0) {};    ///< Default constructor.
        double data;            ///< The actual data point.
    };


    /// A structure used to store a single Rinex data point.
    struct RinexDatum : public BasicDatum
    {
        /// Default constructor.
        RinexDatum(): lli(0),ssi(0) {};
        /// Loss of Lock Indicator. See the RINEX Spec. for an explanation.
        short lli;
        /// Signal Strength Indicator. See the RINEX Spec. for an explanation.
        short ssi;
    };


    /// map from SatID to BasicDatum.
    typedef std::map<SatID, BasicDatum> SatIDDatumMap;


    /// map from SatID to RinexDatum.
    typedef std::map<SatID, RinexDatum> SatIDRinexDatumMap;


    /// map from RinexObsType to BasicDatum.
    typedef std::map<RinexObsHeader::RinexObsType, BasicDatum> ObsTypeDatumMap;


    /// map from RinexObsType to RinexDatum.
    typedef std::map<RinexObsHeader::RinexObsType, RinexDatum> _RinexObsTypeMap;


    /// map from SatID to ObsTypeDatumMap.
    typedef std::map<SatID, ObsTypeDatumMap> SatObsTypeDatumMap;


    /// map from SatID to RinexObsTypeMap.
    typedef std::map<SatID, _RinexObsTypeMap> _RinexSatMap;


    /// This is a base class encapsulating the minimum ammount of information.
    class BaseDataObject
    {
    public:

        /// The time corresponding to the observations
        DayTime time;

        /// Constructor.
        BaseDataObject() : time(DayTime::BEGINNING_OF_TIME) {}

        /// Destructor
        virtual ~BaseDataObject() {}

    }; // end class BaseDataObject


    /// Class encapsulating the data from a given generic observation.
    class OneTypeData : public BaseDataObject
    {
    public:

        /// Observations as a map from SatID to BasicDatum.
        SatIDDatumMap obs;

    }; // end class OneTypeData


    //
    // Note to fellow developers: The idea is that RinexObsData would eventually 
    // inherit from this class, sharing some basic information with other "sister" classes.
    // However, the problem of current inheritance from RinexObsBase ("serving any purpose
    // other than to make the class diagram look nice.") remains...
    //
    /// Base class encapsulating the data from a Rinex observation file.
    class RinexBaseDataObject : public BaseDataObject
    {
    public:

        /** The epoch flag provided by the Rinex observations file. It has the following values:
         * 0 ok
         * 1 power failure since previous epoch
         * 2 start moving antenna
         * 3 new site occupation (end moving antenna)
         *   at least MARKER NAME header record follows
         * 4 header records follow
         * 5 external event
         * 6 cycle slip record - same format as observation, but slips not data,
         *   and LLI and SSI are blank
         */
        short epochFlag;

        /** number of satellites in this observation, except when epochFlag = 2-5,
         * then the number of auxiliary header records to follow.
         */
        short numSvs;

        /// Constructor.
        RinexBaseDataObject() : epochFlag(0), numSvs(0) {}

        /// Destructor
        virtual ~RinexBaseDataObject() {}


    }; // end class RinexBaseDataObject


    /// Class encapsulating the data from a given observation type extracted from a Rinex observation file.
    class RinexOneTypeData : public RinexBaseDataObject
    {
    public:

        /// Observation RINEX type
        RinexObsHeader::RinexObsType typeObs;

        /// Observations as a map from SatID to RinexDatum.
        SatIDRinexDatumMap obs;

    }; // end class RinexOneTypeData



   //-------------------------------------------------------------------------
   // Structures and classes about navigation equations
   //-------------------------------------------------------------------------
   
   
    /// A structure used to store the geometry vector data for a single SV-Rx pair.
    struct geometrySVRXData
    {
        /// Default constructor.
        geometrySVRXData(): uX(0.0), uY(0.0), uZ(0.0) {};

        /// Component in X of unitary vector from SV to receiver.
        double uX;

        /// Component in Y of unitary vector from SV to receiver.
        double uY;

        /// Component in Z of unitary vector from SV to receiver.
        double uZ;
    };


    /// A structure used to store the geometry matrix data for a single SV-Rx pair.
    struct geometryTimeSVRXData : public geometrySVRXData
    {
        /// Default constructor.
        geometryTimeSVRXData(): uT(1.0) {};

        /// Parameter associated with time.
        double uT;
    };


    /// A structure used to store the data needed for simple navigation equations for a single SV-Rx pair.
    struct simpleNavEquationData : public geometryTimeSVRXData
    {
        /// Default constructor.
        simpleNavEquationData(): prefitResidual(0.0) {};

        /// Prefit residual for the a given SV-Rx pair.
        double prefitResidual;
    };


    /// A structure used to store the data needed for double-differenced navigation equations for a single SV-Rx pair.
    struct ddNavEquationData : public geometrySVRXData
    {
        /// Default constructor.
        ddNavEquationData(): prefitResidual(0.0) {};

        /// Prefit residual for the a given SV-Rx pair.
        double prefitResidual;
    };


    /// map from SatID to simpleNavEquationData.
    typedef std::map<SatID, simpleNavEquationData> SatIDsimpleNavEquationMap;


    /// map from SatID to ddNavEquationData.
    typedef std::map<SatID, ddNavEquationData> SatIDddNavEquationMap;


    /// A structure used to store basic information about a receiver.
    struct baseRXData
    {
        /// Receiver name
        std::string rxName;

        /// Receiver position.
        Position rxPosition;

    };


    /// A structure used to store information about a receiver.
    struct RXData : public baseRXData
    {
        /// Antenna number.
        Triple  rxAntennaNumber;

        /// Antenna type.
        Triple  rxAntennaType;

        /// Antenna offset.
        Triple  rxAntennaOffset;

    };


    /// This is a class encapsulating information of a simple navigation equation set.
    class simpleNavEquationObject : public BaseDataObject
    {
    public:

        /// Number of equations in this object
        int numEquations;

        /// Equations as a map from SatID to simpleNavEquationData.
        SatIDsimpleNavEquationMap equationSet;

        /// Method to remove a given equation from the equation set 
        int removeEquation(SatID &sv);

        /// Method to add a given equation to the equation set 
        bool addEquation(SatID &sv, simpleNavEquationData &eq);

    }; // end class simpleNavEquationObject



    /// This is a class encapsulating information of a receiver navigation equation set.
    class RXNavEquationObject : public simpleNavEquationObject
    {
    public:

        /// Receiver data
        RXData receiver;

    }; // end class RXNavEquationObject


    /// This is a class encapsulating information of a double-differenced navigation equation set.
    class ddNavEquationObject : public BaseDataObject
    {
    public:

        /// Number of equations in this object
        int numEquations;

        /// Equations as a map from SatID to ddNavEquationData.
        SatIDddNavEquationMap equationSet;

        /// Method to remove a given equation from the equation set 
        int removeEquation(SatID &sv);

        /// Method to add a given equation to the equation set 
        bool addEquation(SatID &sv, simpleNavEquationData &eq);

    }; // end class ddNavEquationObject


    /// This is a class encapsulating information of a double-differenced receiver navigation equation set.
    class RXddNavEquationObject : public ddNavEquationObject
    {
    public:

        /// Receiver data
        RXData receiver;

    }; // end class RXddNavEquationObject



   //-------------------------------------------------------------------------
   // Structures and classes about solutions
   //-------------------------------------------------------------------------


    /// Basic class to encapsulate a GNSS solution
    class BaseSolutionObject : public BaseDataObject
    {
        /// Position solution.
        Position posSolution;

        /// Postfit residuals as a map from SatID to BasicDatum.
        SatIDDatumMap postfitResiduals;

    }; // end of class BaseSolutionData


    /// Class to store a simple GNSS fix
    class simpleSolutionObject : public BaseSolutionObject
    {
        /// Position solution.
        double rxClockOffset;

    }; // end of class simpleSolutionObject


   //@}
   
}

#endif
