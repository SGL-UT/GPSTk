/**
 * @file ModeledPR.cpp
 * Class to compute modeled pseudoranges of a mobile receiver
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2006, 2007
//
//============================================================================



#include "ModeledPR.hpp"


namespace gpstk
{

    /* Method to set an a priori position of receiver using Bancroft method.
     *
     * @param Tr            Time of observation
     * @param Satellite     Vector of satellites in view
     * @param Pseudorange   Pseudoranges measured from mobile to satellites
     * @param Eph           Satellites Ephemeris
     *
     * @return
     *  0 if OK
     *  -1 if problems arose
     */
    int ModeledPR::Prepare(const DayTime& Tr, std::vector<SatID>& Satellite, std::vector<double>& Pseudorange, const XvtStore<SatID>& Eph)
    {
        Matrix<double> SVP;
        Bancroft Ban;
        Vector<double> vPos;
        PRSolution raimObj;
        
        try
        {
            raimObj.PrepareAutonomousSolution(Tr, Satellite, Pseudorange, Eph, SVP);
            if (Ban.Compute(SVP, vPos) < 0 ) return -1;
        }
        catch(...)
        {
            return -1;
        }

        return Prepare(vPos(0), vPos(1), vPos(2));

    }


    /* Method to set the initial (a priori) position of receiver before 
     * Compute() method.
     * @return
     *  0 if OK
     *  -1 if problems arose
     */
    int ModeledPR::Prepare(const double& aRx, const double& bRx, const double& cRx, Position::CoordinateSystem
        s, GeoidModel *geoid) throw(GeometryException) 
    {
        int result = setInitialRxPosition(aRx, bRx, cRx, s, geoid);
        // If everything is OK, the model is prepared
        if (result ==0) modelPrepared = true; else modelPrepared = false;

        return result;
    }


    /* Method to set the initial (a priori) position of receiver before 
     * Compute() method.
     * @return
     *  0 if OK
     *  -1 if problems arose
     */
    int ModeledPR::Prepare(const Position& RxCoordinates) throw(GeometryException) 
    {
        
        int result = setInitialRxPosition(RxCoordinates); 
        // If everything is OK, the model is prepared
        if (result ==0) modelPrepared = true; else modelPrepared = false;

        return result;
    }



     /* Returns a satTypeValueMap object, adding the new data generated when calling a modeling object.
      *
      * @param time      Epoch.
      * @param gData     Data object holding the data.
      */
    satTypeValueMap& ModeledPR::processModel(const DayTime& time, satTypeValueMap& gData) throw(Exception)
    {
        // First, if the model is not prepared let's take care of it
        if (!getModelPrepared()) Prepare(time, gData);

        ModeledReferencePR::processModel(time, gData);

        return gData;
    }   // End ModeledPR::processModel(const DayTime& time, satTypeValueMap& gData)



} // namespace

