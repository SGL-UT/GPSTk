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
#include "NavID.hpp"

#include "TestUtil.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <set>

using namespace std;
using namespace gpstk;

int main()
{
   TUDEF( "NavID", "" );
   
   //==========================================================================================================================
   // Ensure the default constructor instantiates a NavID object with type unknown.
   //==========================================================================================================================
   TUCSM("Default Constructor Test");
   
   NavID dfltTest;
   if ( dfltTest.navType == NavID::ntUnknown ) TUPASS( "" );
   else TUFAIL( "Default instantiation failed." );
   
   //==========================================================================================================================
   // Ensure that the explicit constructor accepts SatID and ObsID arguments and correctly instantiates NavID objects.
   //==========================================================================================================================
   TUCSM("Explicit Constructor Test");
   
      //GPS LNAV
   NavID testIDLNAV( SatID( 1, SatID::systemGPS ), ObsID( ObsID::otNavMsg, ObsID::cbL1, ObsID::tcCA ) );
   if ( testIDLNAV.navType == NavID::ntGPSLNAV ) TUPASS( "" );
   else TUFAIL( "ntGPSLNAV instantiation failed." );
      
      //GPS CNAV L2
   NavID testIDCNAVL2( SatID( 1, SatID::systemGPS ), ObsID( ObsID::otNavMsg, ObsID::cbL2, ObsID::tcC2LM ) );
   if ( testIDCNAVL2.navType == NavID::ntGPSCNAVL2 ) TUPASS( "" );
   else TUFAIL( "ntGPSCNAVL2 instantiation failed." );
   
      //GPS CNAV L5
   NavID testIDCNAVL5( SatID( 1, SatID::systemGPS ), ObsID( ObsID::otNavMsg, ObsID::cbL5, ObsID::tcI5 ) );
   if ( testIDCNAVL5.navType == NavID::ntGPSCNAVL5 ) TUPASS( "" );
   else TUFAIL( "ntGPSCNAVL5 instantiation failed." );
   
      //GPS MNAV
   NavID testIDMNAV( SatID( 1, SatID::systemGPS ), ObsID( ObsID::otNavMsg, ObsID::cbL2, ObsID::tcM ) );
   if ( testIDMNAV.navType == NavID::ntGPSMNAV ) TUPASS( "" );
   else TUFAIL( "ntGPSMNAV instantiation failed." );
   
      //Beidou D1
   NavID testIDBD1( SatID( 6, SatID::systemBeiDou ), ObsID( ObsID::otNavMsg, ObsID::cbB1, ObsID::tcCI1 ) );
   if ( testIDBD1.navType == NavID::ntBeiDou_D1 ) TUPASS( "" );
   else TUFAIL( "ntBeiDou_D1 instantiation failed." );
   
      //Beidou D2
   NavID testIDBD2( SatID( 5, SatID::systemBeiDou ), ObsID( ObsID::otNavMsg, ObsID::cbB2, ObsID::tcCI1 ) );
   if ( testIDBD2.navType == NavID::ntBeiDou_D2 ) TUPASS( "" );
   else TUFAIL( "ntBeiDou_D2 instantiation failed." );
   
      //Glonass Civil F
   NavID testIDGloF( SatID( 2, SatID::systemGlonass ), ObsID( ObsID::otNavMsg, ObsID::cbG1, ObsID::tcGCA ) );
   if ( testIDGloF.navType == NavID::ntGloCivilF ) TUPASS( "" );
   else TUFAIL( "ntGloCivilF instantiation failed." );
   
      //Glonass Civil C
   NavID testIDGloC( SatID( 2, SatID::systemGlonass ), ObsID( ObsID::otNavMsg, ObsID::cbG3, ObsID::tcIQR3 ) );
   if ( testIDGloC.navType == NavID::ntGloCivilC ) TUPASS( "" );
   else TUFAIL( "ntGloCivilC instantiation failed." );
   
      //Galileo Open Sys
   NavID testIDGalOS( SatID( 2, SatID::systemGalileo ), ObsID( ObsID::otNavMsg, ObsID::cbL1, ObsID::tcB ) );
   if ( testIDGalOS.navType == NavID::ntGalOS ) TUPASS( "" );
   else TUFAIL( "ntGalOS instantiation failed." );
   
      //Unknown
   NavID testIDUnkwn( SatID( 1, SatID::systemGPS ), ObsID( ObsID::otNavMsg, ObsID::cbL5, ObsID::tcM ) );
   if ( testIDUnkwn.navType == NavID::ntUnknown) TUPASS( "" );
   else TUFAIL( "ntUnknown instantiation failed." );
   

   //==========================================================================================================================
   // Ensure that the string output of a NavID object matches the correct type.
   //==========================================================================================================================
   TUCSM("String Output Test");
   
   stringstream ln;
   ln << testIDLNAV;
   if ( ln.str() == "GPS_LNAV" ) TUPASS( "" );
   else TUFAIL( "String does not match GPS_LNAV." );
   
   stringstream l2;
   l2 << testIDCNAVL2;
   if ( l2.str() == "GPS_L2_CNAV" ) TUPASS( "" );
   else TUFAIL( "String does not match GPS_L2_CNAV." );
   
   stringstream l5;
   l5 << testIDCNAVL5;
   if ( l5.str() == "GPS_L5_CNAV" ) TUPASS( "" );
   else TUFAIL( "String does not match GPS_L5_CNAV." );
   
   stringstream mn;
   mn << testIDMNAV;
   if ( mn.str() == "GPS_MNAV" ) TUPASS( "" );
   else TUFAIL( "String does not match GPS_MNAV." );
   
   stringstream d1;
   d1 << testIDBD1;
   if ( d1.str() == "Beidou_D1" ) TUPASS( "" );
   else TUFAIL( "String does not match Beidou_D1." );
   
   stringstream d2;
   d2 << testIDBD2;
   if ( d2.str() == "Beidou_D2" ) TUPASS( "" );
   else TUFAIL( "String does not match Beidou_D2." );
   
   stringstream gf;
   gf << testIDGloF;
   if ( gf.str() == "GloCivilF" ) TUPASS( "" );
   else TUFAIL( "String does not match GloCivilF." );
   
   stringstream gc;
   gc << testIDGloC;
   if ( gc.str() == "GloCivilC" ) TUPASS( "" );
   else TUFAIL( "String does not match GloCivilC." );
   
   stringstream os;
   os << testIDGalOS;
   if ( os.str() == "GalOS" ) TUPASS( "" );
   else TUFAIL( "String does not match GPS_LNAV." );
   
   stringstream un;
   un << testIDUnkwn;
   if ( un.str() == "Unknown" ) TUPASS( "" );
   else TUFAIL( "String does not match Unknown." );
   
   //==========================================================================================================================
   // Ensure that the string input constructor accepts a string and generates a NavID object that matches the correct type.
   //==========================================================================================================================
   TUCSM("String Input Constructor Test");
   
   NavID testIDLNAVString( ln.str() );
   if ( testIDLNAVString.navType == NavID::ntGPSLNAV ) TUPASS( "" );
   else TUFAIL( "String input constructor failed to insantiate NavID object." );
   
   NavID testIDCNAVL2String( l2.str() );
   if ( testIDCNAVL2String.navType == NavID::ntGPSCNAVL2 ) TUPASS( "" );
   else TUFAIL( "String input constructor failed to insantiate NavID object." );
   
   NavID testIDCNAVL5String( l5.str() );
   if ( testIDCNAVL5String.navType == NavID::ntGPSCNAVL5 ) TUPASS( "" );
   else TUFAIL( "String input constructor failed to insantiate NavID object." );
   
   NavID testIDMNAVString( mn.str() );
   if ( testIDMNAVString.navType == NavID::ntGPSMNAV ) TUPASS( "" );
   else TUFAIL( "String input constructor failed to insantiate NavID object." );
   
   NavID testIDBD1String( d1.str() );
   if ( testIDBD1String.navType == NavID::ntBeiDou_D1 ) TUPASS( "" );
   else TUFAIL( "String input constructor failed to insantiate NavID object." );
   
   NavID testIDBD2String( d2.str() );
   if ( testIDBD2String.navType == NavID::ntBeiDou_D2 ) TUPASS( "" );
   else TUFAIL( "String input constructor failed to insantiate NavID object." );
   
   NavID testIDGloFString( gf.str() );
   if ( testIDGloFString.navType == NavID::ntGloCivilF ) TUPASS( "" );
   else TUFAIL( "String input constructor failed to insantiate NavID object." );
   
   NavID testIDGloCString( gc.str() );
   if ( testIDGloCString.navType == NavID::ntGloCivilC ) TUPASS( "" );
   else TUFAIL( "String input constructor failed to insantiate NavID object." );
   
   NavID testIDGalOSString( os.str() );
   if ( testIDGalOSString.navType == NavID::ntGalOS ) TUPASS( "" );
   else TUFAIL( "String input constructor failed to insantiate NavID object." );
   
   NavID testIDUnkwnString( un.str() );
   if ( testIDUnkwnString.navType == NavID::ntUnknown ) TUPASS( "" );
   else TUFAIL( "String input constructor failed to insantiate NavID object." );
   
   //==========================================================================================================================
   // Ensure that the map key operators correctly reorder an out-of-order set of NavType enumerators.
   //==========================================================================================================================
   TUCSM("Inequality Test");
   
   set<NavID> testSet;
      //Insert NavTypes into set in backward order.
   testSet.insert( testIDUnkwn );
   testSet.insert( testIDGalOS );
   testSet.insert( testIDGloC );
   testSet.insert( testIDGloF );
   testSet.insert( testIDBD2 );
   testSet.insert( testIDBD1 );
   testSet.insert( testIDMNAV );
   testSet.insert( testIDCNAVL5 );
   testSet.insert( testIDCNAVL2 );
   testSet.insert( testIDLNAV );
   
   int failCount = 0;
      //Instantiate currTest as GPS_LNAV by 
      //instantiating navType as ntGPSLNAV.
   NavID currTest;
   currTest.navType = NavID::ntGPSLNAV;
      //Define cit as iterator for NavID set.
   set<NavID>::const_iterator cit;
   
      //Set nid equal to dereferenced NavID set iterator; cit should
      //initially correspond to first location in set (@ value GPS_LNAV).
      //Compare to currTest which initially corresponds to NavID for first
      //location in NavType enum (ntGPSLNAV).
   for ( cit = testSet.begin(); cit != testSet.end(); cit++ )
   {
      const NavID& nid = *cit;
         //If operators in NavID work correctly, failCount = 0.
      if ( nid != currTest )
      {
         failCount++;
      }
      currTest.navType = static_cast<NavID::NavType>((int)currTest.navType + 1);
   }
   if (!failCount) TUPASS( "" );
   else TUFAIL( "failCount != 0" );
   
   TURETURN();
   return testFramework.countFails();
}

