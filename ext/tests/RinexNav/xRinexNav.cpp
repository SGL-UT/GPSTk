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

#include "TestUtil.hpp"
#include "Exception.hpp"
#include "Rinex3EphemerisStore.hpp"
#include <string>


#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"
#include "RinexNavFilterOperators.hpp"
#include "StringUtils.hpp"


class xRinexNav
{

  // Testing Rinex3NavData with Rinex3EphemerisStore
  void dataTest( void )
  {

    TestUtil testFramework( "Rinex3EphemerisStore", "addToList", __FILE__, __LINE__ );
                        
    const short PRN6 = 6 ;
    gpstk::SatID sid6( PRN6, gpstk::SatID::systemGPS ); 

    try
    {
        gpstk::Rinex3EphemerisStore Store;
        Store.loadFile( "Logs/RinexNavExample.99n" );

        gpstk::CivilTime Time( 1999,9,2,17,51,44, TimeSystem::GPS );

        //Load data into GPSEphemerisStore object so can invoke findUserEphemeris on it

        std::list<gpstk::Rinex3NavData> R3NList;
        gpstk::GPSEphemerisStore GStore;
        std::list<gpstk::Rinex3NavData>::const_iterator it;
        Store.addToList( R3NList );
        for (it=R3NList.begin(); it != R3NList.end(); ++it)
          GStore.addEphemeris( gpstk::EngEphemeris(*it) );

        const gpstk::EngEphemeris& Eph6 = GStore.findUserEphemeris( sid6, Time );
        gpstk::RinexNavData Data( Eph6 );
        list<double> NavDataList = Data.toList();
        
        testFramework.assert( true, 'Load data into Rinex3EphemerisStore, convert to EngEphemeris, store in GPSEphemerisStore, extract with findUserEphemeris', __LINE__ );
    }
    catch( gpstk::Exception& e )
    {
        testFramework.assert( false, 'Threw an unexpected exception', __LINE__ );
    }

    return testFramework.countFails();

  }


};
