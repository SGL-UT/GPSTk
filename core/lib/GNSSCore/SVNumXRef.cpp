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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
// //  Copyright 2004, The University of Texas at Austin
//
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
//===========================================================================
/*  SVNumXRefMap.cpp
*
*   Applied Research Laboratories, The University of Texas at Austin
*
*/
#include "SVNumXRef.hpp"
#include "CivilTime.hpp"
#include "TimeString.hpp"

#include <iostream>
#include <stdio.h>
#include <sstream>

using namespace std;
using namespace gpstk;

SVNumXRef::SVNumXRef( )
{
   NtoBMap.insert( make_pair(  1,  I )); 
   NtoBMap.insert( make_pair(  2,  I ));
   NtoBMap.insert( make_pair(  3,  I ));
   NtoBMap.insert( make_pair(  4,  I ));
   NtoBMap.insert( make_pair(  5,  I ));
   NtoBMap.insert( make_pair(  6,  I ));
     // no NAVSTAR 07, I-7 was a launch failure
   NtoBMap.insert( make_pair(  8,  I ));
   NtoBMap.insert( make_pair(  9,  I ));
   NtoBMap.insert( make_pair( 10,  I ));
   NtoBMap.insert( make_pair( 11,  I ));
     // no NAVSTAR 12, was never launched
   NtoBMap.insert( make_pair( 13, II ));
   NtoBMap.insert( make_pair( 14, II ));
   NtoBMap.insert( make_pair( 15, II ));
   NtoBMap.insert( make_pair( 16, II ));
   NtoBMap.insert( make_pair( 17, II ));
   NtoBMap.insert( make_pair( 18, II ));
   NtoBMap.insert( make_pair( 19, II ));
   NtoBMap.insert( make_pair( 20, II ));
   NtoBMap.insert( make_pair( 21, II ));
   NtoBMap.insert( make_pair( 22,IIA ));
   NtoBMap.insert( make_pair( 23,IIA ));
   NtoBMap.insert( make_pair( 24,IIA ));
   NtoBMap.insert( make_pair( 25,IIA ));
   NtoBMap.insert( make_pair( 26,IIA ));
   NtoBMap.insert( make_pair( 27,IIA ));
   NtoBMap.insert( make_pair( 28,IIA ));
   NtoBMap.insert( make_pair( 29,IIA ));
   NtoBMap.insert( make_pair( 30,IIA ));
   NtoBMap.insert( make_pair( 31,IIA ));
   NtoBMap.insert( make_pair( 32,IIA ));
   NtoBMap.insert( make_pair( 33,IIA ));
   NtoBMap.insert( make_pair( 34,IIA ));
   NtoBMap.insert( make_pair( 35,IIA ));
   NtoBMap.insert( make_pair( 36,IIA ));
   NtoBMap.insert( make_pair( 37,IIA ));
   NtoBMap.insert( make_pair( 38,IIA ));
   NtoBMap.insert( make_pair( 39,IIA ));
   NtoBMap.insert( make_pair( 40,IIA )); 
   NtoBMap.insert( make_pair( 41,IIR ));
    // no NAVSTAR 42, IIR-1 was a launch failure
   NtoBMap.insert( make_pair( 43,IIR )); 
   NtoBMap.insert( make_pair( 44,IIR ));
   NtoBMap.insert( make_pair( 45,IIR ));
   NtoBMap.insert( make_pair( 46,IIR ));
   NtoBMap.insert( make_pair( 47,IIR ));
   NtoBMap.insert( make_pair( 48,IIR_M));
   NtoBMap.insert( make_pair( 49,IIR_M));
   NtoBMap.insert( make_pair( 50,IIR_M));
   NtoBMap.insert( make_pair( 51,IIR ));
   NtoBMap.insert( make_pair( 52,IIR_M));
   NtoBMap.insert( make_pair( 53,IIR_M));
   NtoBMap.insert( make_pair( 54,IIR ));
   NtoBMap.insert( make_pair( 55,IIR_M));
   NtoBMap.insert( make_pair( 56,IIR ));
   NtoBMap.insert( make_pair( 57,IIR_M));
   NtoBMap.insert( make_pair( 58,IIR_M));
   NtoBMap.insert( make_pair( 59,IIR ));
   NtoBMap.insert( make_pair( 60,IIR ));
   NtoBMap.insert( make_pair( 61,IIR ));
   NtoBMap.insert( make_pair( 62,IIF )); 
   NtoBMap.insert( make_pair( 63,IIF ));
   NtoBMap.insert( make_pair( 64,IIF ));
   NtoBMap.insert( make_pair( 65,IIF )); 
   NtoBMap.insert( make_pair( 66,IIF )); 
   NtoBMap.insert( make_pair( 67,IIF ));
   NtoBMap.insert( make_pair( 68,IIF ));
   NtoBMap.insert( make_pair( 69,IIF ));
   NtoBMap.insert( make_pair( 70,IIF ));
   NtoBMap.insert( make_pair( 71,IIF ));
   NtoBMap.insert( make_pair( 72,IIF ));
   NtoBMap.insert( make_pair( 73,IIF ));


      // Note: This table start with Block I values
      // Set up NAVSTAR -> PRN ID relationship
      // NAVSTAR ID first, PRN ID second
   NtoPMap.insert( std::pair<const int, XRefNode>(  1, XRefNode(  4, 
								  CivilTime( 1978,  2, 22,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 1985,  7, 17, 17, 30,  0.0, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>(  2, XRefNode(  7, 
                                       CivilTime( 1978,  6, 13,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 1988,  2, 12, 23, 59, 59.9, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>(  3, XRefNode(  6, 
                                       CivilTime( 1978, 10,  6,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 1992,  5, 18, 23, 41,  0.0, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>(  4, XRefNode(  8, 
                                       CivilTime( 1978, 12, 10,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 1990,  5, 31, 23, 59, 59.9, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>(  5, XRefNode(  5, 
                                       CivilTime( 1980,  2,  9,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 1984,  5, 11, 23, 59, 59.9, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>(  6, XRefNode(  9, 
                                       CivilTime( 1980,  4, 26,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 1991,  3,  6,  3, 42,  0.0, TimeSystem::GPS))));
      // no NAVSTAR 07, I-7 was a launch failure
   NtoPMap.insert( std::pair<const int, XRefNode>(  8, XRefNode( 11, 
                                       CivilTime( 1983,  7, 14,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 1993,  5,  4,  0, 20,  0.0, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>(  9, XRefNode( 13, 
                                       CivilTime( 1984,  6, 13,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 1993,  5,  4, 18, 17,  0.0, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 10, XRefNode( 12, 
                                       CivilTime( 1984,  9,  8,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 1996,  3, 26, 23, 59, 59.9, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 11, XRefNode(  3, 
                                       CivilTime( 1985, 10, 30,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 1994,  4, 14, 21,  0,  0.0, TimeSystem::GPS))));
      // no NAVSTAR 12, was never launched
   NtoPMap.insert( std::pair<const int, XRefNode>( 13, XRefNode(  2, 
                                       CivilTime( 1989,  6, 10,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2004,  5, 12, 17,  1,  0.0, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 14, XRefNode( 14, 
                                       CivilTime( 1989,  2, 14,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2000,  4, 14, 13, 47,  0.0, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 15, XRefNode( 15, 
                                       CivilTime( 1990, 10,  1,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2007,  3, 15, 23, 59, 59.9, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 16, XRefNode( 16, 
                                       CivilTime( 1989,  8, 18,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2000, 10, 13,  0, 45,  0.0, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 17, XRefNode( 17, 
                                       CivilTime( 1989, 12, 11,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2005,  2, 23, 22,  0,  0.0, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 18, XRefNode( 18, 
                                       CivilTime( 1990,  1, 24,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2000,  8, 18,  7, 42,  0.0, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 19, XRefNode( 19, 
                                       CivilTime( 1989, 10, 21,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2001,  9, 11, 22,  0,  0.0, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 20, XRefNode( 20, 
                                       CivilTime( 1990,  3, 26,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 1996, 12, 13, 23, 59, 59.9, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 21, XRefNode( 21, 
                                       CivilTime( 1990,  8,  2,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2003,  1, 27, 22,  0,  0.0, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 22, XRefNode( 22, 
                                       CivilTime( 1993,  2,  3,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2003,  8,  6, 22,  0,  0.0, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 23, XRefNode( 23, 
                                       CivilTime( 1990, 11, 26,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2004,  2, 13, 22,  0,  0.0, TimeSystem::GPS))));
   // NANU #2016008 (end) - SVN 23 Decommissioned
   NtoPMap.insert( std::pair<const int, XRefNode>( 23, XRefNode( 32, 
                                       CivilTime( 2006, 12,  1,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2016,  1,  25, 0,  0,  0.0, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 24, XRefNode( 24, 
                                       CivilTime( 1991,  7,  4,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2011,  9, 30, 23, 59, 59.9, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 25, XRefNode( 25, 
                                       CivilTime( 1992,  2, 23,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2009, 12, 18, 22, 28,  0.0, TimeSystem::GPS))));
   // NANU 2015005 (end) - extended due to additional data.
   NtoPMap.insert( std::pair<const int, XRefNode>( 26, XRefNode( 26, 
                                       CivilTime( 1992,  7,  7,  0,  0,  0.0, TimeSystem::GPS),
				       CivilTime( 2015, 1, 20, 23, 59, 59.9, TimeSystem::GPS)))); 
   //  NANU # (start). NANU 2012061 (end)
   //  NANU #(start). NANU 2011059 (end).   
   NtoPMap.insert( std::pair<const int, XRefNode>( 27, XRefNode( 27, 
                                       CivilTime( 1992,  9,  9,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2011,  8, 10, 23, 59, 59.9, TimeSystem::GPS))));
   // NANU 2011105 (start). NANU 2012063 (end)     
   NtoPMap.insert( std::pair<const int, XRefNode>( 27, XRefNode( 27,
                                       CivilTime( 2011, 12, 16, 22, 38,  0.0, TimeSystem::GPS),
                                       CivilTime( 2012, 10,  6, 23, 59, 59.9, TimeSystem::GPS))));
   // NANU 2013074 (start). NANU 2013080 (end)     
   NtoPMap.insert( std::pair<const int, XRefNode>( 27, XRefNode( 30, 
                                       CivilTime( 2013, 12,  3,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2013, 12, 17, 23, 59, 59.9, TimeSystem::GPS))));
   // NANU 2015010 (start). NANU 2013019 (end) plus actual received data.
   NtoPMap.insert( std::pair<const int, XRefNode>( 27, XRefNode( 26, 
                                       CivilTime( 2015,  2, 26,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2015,  3, 16, 23, 59, 59.9, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 28, XRefNode( 28, 
                                       CivilTime( 1992,  4, 10,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 1997,  8, 15, 23, 59, 59.9, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 29, XRefNode( 29, 
                                       CivilTime( 1992, 12, 18,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2007, 10, 23, 23, 59, 59.9, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 30, XRefNode( 30, 
                                       CivilTime( 1996,  9, 12,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2011,  8,  4, 23, 59, 59.9, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 31, XRefNode( 31, 
                                       CivilTime( 1993,  3, 30,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2005, 10, 24, 23, 59, 59.9, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 32, XRefNode(  1, 
                                       CivilTime( 1992, 11, 22,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2008,  3, 17, 22,  0,  0.0, TimeSystem::GPS))));
   //  NANU 2012018 (start). NANU 2012024 (end)
   NtoPMap.insert( std::pair<const int, XRefNode>( 32, XRefNode(  24, 
                                       CivilTime( 2012,  3, 14,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2012,  4, 24, 23, 59, 59.9, TimeSystem::GPS))));
   //  NANU 2013049 (start). NANU 2013054 (end)
   NtoPMap.insert( std::pair<const int, XRefNode>( 32, XRefNode(  30, 
                                       CivilTime( 2013,  8, 22,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2013,  9, 18, 23, 59, 59.9, TimeSystem::GPS))));
   //  NANU 2015007 (start). NANU 2015101 (end) plus actual received data
   NtoPMap.insert( std::pair<const int, XRefNode>( 32, XRefNode(  26, 
                                       CivilTime( 2015,  2,  5,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2015,  2, 24, 23, 59, 59.9, TimeSystem::GPS))));
   // NANU 2014063 (end).
   // NANU claimed 8/2 end date. SV continued to broadcast until 8/18/2014.
   NtoPMap.insert( std::pair<const int, XRefNode>( 33, XRefNode(  3, 
                                       CivilTime( 1996,  3, 28,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2014,  8, 18 ,  23,  59,  59.9, TimeSystem::GPS))));
   // NANU 2015091 (end)
   NtoPMap.insert( std::pair<const int, XRefNode>( 34, XRefNode(  4, 
                                       CivilTime( 1993, 10, 26,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2015, 11, 2,  22,  0,  0.0, TimeSystem::GPS))));
                                       
   NtoPMap.insert( std::pair<const int, XRefNode>( 35, XRefNode(  5, 
                                       CivilTime( 1993,  8, 30,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2009,  3, 26, 20, 31,  0.0, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 35, XRefNode(  1, 
                                       CivilTime( 2011,  6,  1,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2011,  7, 12, 23, 59, 59.9, TimeSystem::GPS))));
   // See PRN 30 notes below for how we arrived at the end time.
   NtoPMap.insert( std::pair<const int, XRefNode>( 35, XRefNode(  30, 
                                       CivilTime( 2011,  8,  6, 20,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2013,  5,  1, 22, 00,  0.0, TimeSystem::GPS))));
   // NANU 2013029 (start), NANU 2013049 (end-IMPLIED)
   //   Note: NANU 2013029 says Approximately 5/8/13 SVN 49 will continue

   //   broadcasting as PRN 30. Signal evidence indicates that SVN 49 used PRN 27 until 
   //   about 5/9/2013 1600  Therefore.  In the meantime SOME SVN was transmitting
   //   PRN 30 through 5/6/2013 2000+.  Therefore, we've added an "extension" to the
   //   SVN 35 decommissioning and moved the SVN49/PRN30 start time.

   NtoPMap.insert( std::pair<const int, XRefNode>( 35, XRefNode(  30, 
                                       CivilTime( 2013,  5,  2,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2013,  5,  6, 22, 00,  0.0, TimeSystem::GPS))));
                                       
   NtoPMap.insert( std::pair<const int, XRefNode>( 35, XRefNode(  3, 
                                       CivilTime( 2014,  9,  5,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2014, 10, 22,  0, 00,  0.0, TimeSystem::GPS))));
                                       
   // NANU 2014015/2014019 (end)
   // NANU claimed 2/21 end date. SV continued to broadcast until 3/3/2014.
   NtoPMap.insert( std::pair<const int, XRefNode>( 36, XRefNode(  6, 
                                       CivilTime( 1995,  3, 10,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2014,  3, 3, 23, 59,  59.9, TimeSystem::GPS))));
   // NANU 2015080 (start) 
   NtoPMap.insert( std::pair<const int, XRefNode>( 36, XRefNode( 10, 
                                       CivilTime( 2015, 9, 16,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2015, 10, 26, 23, 59, 59.9, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 37, XRefNode(  7, 
                                       CivilTime( 1993,  5, 13,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2007,  7, 20, 23, 59, 59.9, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 37, XRefNode(  1, 
                                       CivilTime( 2008, 10, 23,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2009,  1,  6, 23, 59, 59.9, TimeSystem::GPS))));
   //  NANU 2012024 (start). NANU 201249 (end)
   NtoPMap.insert( std::pair<const int, XRefNode>( 37, XRefNode(  24, 
                                       CivilTime( 2012,  4, 25,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2012,  8,  7, 23, 59, 59.9, TimeSystem::GPS))));
   //  NANU 2013054 (start). NANU 2013074 (end)
   NtoPMap.insert( std::pair<const int, XRefNode>( 37, XRefNode(  30, 
                                       CivilTime( 2013,  9, 19,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2013, 12,  2, 23, 59, 59.9, TimeSystem::GPS))));
   // NANU 2015021 (end)
   NtoPMap.insert( std::pair<const int, XRefNode>( 38, XRefNode(  8, 
                                       CivilTime( 1997, 11,  6,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2015, 4, 13, 23,  59,  59.9, TimeSystem::GPS))));
   // NANU 2014046 (end)
   // NANU claimed 5/19 end date. SV continued tp broadcast until 5/27/2014.
   NtoPMap.insert( std::pair<const int, XRefNode>( 39, XRefNode(  9, 
                                       CivilTime( 1993,  6, 26,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2014,  5, 27, 23,  59,  59.9, TimeSystem::GPS))));
   // NANU 2014050 (start)
   NtoPMap.insert( std::pair<const int, XRefNode>(  39, XRefNode( 9,
                                       CivilTime( 2014,  6, 13,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2014,  8,  1,  23,  59,  59.9, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 40, XRefNode( 10, 
                                       CivilTime( 1996,  7, 16,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2015,  8, 3,  23,  59,  59.9, TimeSystem::GPS))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 41, XRefNode( 14, 
                                       CivilTime( 2000, 11, 10,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
      // no NAVSTAR 42, IIR-1 was a launch failure
   NtoPMap.insert( std::pair<const int, XRefNode>( 43, XRefNode( 13, 
                                       CivilTime( 1997,  7, 23,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 44, XRefNode( 28,
                                       CivilTime( 2000,  7, 16,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 45, XRefNode( 21, 
                                       CivilTime( 2003,  3, 31,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 46, XRefNode( 11, 
                                       CivilTime( 1999, 10,  7,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 47, XRefNode( 22, 
                                       CivilTime( 2003, 12, 21,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 48, XRefNode(  7, 
                                       CivilTime( 2008,  3, 15,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 49, XRefNode(  1, 
                                       CivilTime( 2009,  3, 24,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2011,  5,  6, 16,  0,  0.0, TimeSystem::GPS))));
   //  NANU 2012003 (start). NANU 2012018 (end)
   NtoPMap.insert( std::pair<const int, XRefNode>( 49, XRefNode(  24, 
                                       CivilTime( 2012,  2,  1,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2012,  3, 13, 23, 59, 59.9, TimeSystem::GPS))));
   //  NANU 2012049 (start). NANU # (end)
   NtoPMap.insert( std::pair<const int, XRefNode>( 49, XRefNode( 24, 
                                       CivilTime( 2012,  8,  8,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2012,  8, 22, 23, 59, 59.9, TimeSystem::GPS))));
   //  NANU 2012064 (start).
   //  We don't know exactly WHEN this relationship stopped.  NANU 2013021 states when
   //  it resumes.  
   NtoPMap.insert( std::pair<const int, XRefNode>( 49, XRefNode( 27, 
                                       CivilTime( 2012, 10, 18,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2012, 12, 31, 23, 59, 59.9, TimeSystem::GPS))));
   //  NANU 2013021 (start), 2013031 (end-IMPLIED)
   //  See PRN30/SVN49 notes below for how we derived this end time.
   NtoPMap.insert( std::pair<const int, XRefNode>( 49, XRefNode( 27, 
                                       CivilTime( 2013,  3, 27,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2013,  5,  9, 16, 44, 59.9, TimeSystem::GPS))));
   //  NANU 2013029 (start), 2013049 (end-IMPLIED)
   NtoPMap.insert( std::pair<const int, XRefNode>( 49, XRefNode( 30, 
                                       CivilTime( 2013,  5,  9, 16, 45,  0.0, TimeSystem::GPS),
                                       CivilTime( 2013,  8, 21, 23, 59, 59.9, TimeSystem::GPS))));
   //  NANU 2013080 (start), 2014018 (end-IMPLIED)
   NtoPMap.insert( std::pair<const int, XRefNode>( 49, XRefNode( 30, 
                                       CivilTime( 2013, 12, 18,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2014,  2, 20, 23, 59,  0.0, TimeSystem::GPS))));
   //  NANU 2014032 (start), 2014045 (end-IMPLIED)
   NtoPMap.insert( std::pair<const int, XRefNode>( 49, XRefNode(  6, 
                                       CivilTime( 2014,  4,  3,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2014,  5, 15, 23, 59, 59.9, TimeSystem::GPS))));
   //  NANU 2015032 (start)
   NtoPMap.insert( std::pair<const int, XRefNode>( 49, XRefNode(  8, 
                                       CivilTime( 2015,  4,  30,  0,  0,  0.0, TimeSystem::GPS),
                                       CivilTime( 2015,  7,   1, 16,  0,  0.0, TimeSystem::GPS))));   
   // NANU 2016009 (general/start)
   NtoPMap.insert( std::pair<const int, XRefNode>( 49, XRefNode(  4, 
                                       CivilTime( 2016,  2, 4,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 50, XRefNode(  5, 
                                       CivilTime( 2009,  8, 27,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 51, XRefNode( 20, 
                                       CivilTime( 2000,  5, 11,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 52, XRefNode( 31, 
                                       CivilTime( 2006,  9, 25,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 53, XRefNode( 17, 
                                       CivilTime( 2005,  9, 26,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 54, XRefNode( 18, 
                                       CivilTime( 2001,  1, 30,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 55, XRefNode( 15, 
                                       CivilTime( 2007, 10, 17,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 56, XRefNode( 16, 
                                       CivilTime( 2003,  1, 29,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 57, XRefNode( 29, 
                                       CivilTime( 2007, 12, 21,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 58, XRefNode( 12, 
                                       CivilTime( 2006, 11, 17,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 59, XRefNode( 19, 
                                       CivilTime( 2004,  3, 20,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 60, XRefNode( 23, 
                                       CivilTime( 2004,  6, 23,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 61, XRefNode(  2, 
                                       CivilTime( 2004,  6,  6,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 62, XRefNode( 25,
                                       CivilTime( 2010,  5, 28,  3,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 63, XRefNode( 1,
                                       CivilTime( 2011,  7, 20,  9, 36, 36.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   // NANU 2014018                                          
   NtoPMap.insert( std::pair<const int, XRefNode>( 64, XRefNode( 30,
                                       CivilTime( 2014,  2, 21,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
                                       
   NtoPMap.insert( std::pair<const int, XRefNode>( 65, XRefNode( 24,
                                       CivilTime( 2012, 10,  4,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  ))); 
                                       
   NtoPMap.insert( std::pair<const int, XRefNode>( 66, XRefNode( 27,
                                       CivilTime( 2013,  5, 15,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   //NANU  2014045 (start)
   NtoPMap.insert( std::pair<const int, XRefNode>( 67, XRefNode(  6,
                                       CivilTime( 2014,  5, 17,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   //
   NtoPMap.insert( std::pair<const int, XRefNode>( 68, XRefNode(  9,
                                       CivilTime( 2014,  8,  2,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
                                       
   NtoPMap.insert( std::pair<const int, XRefNode>( 69, XRefNode(  3,
                                       CivilTime( 2014,  10,  29,  0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   // NANU 2016011 LAUNCH
   NtoPMap.insert( std::pair<const int, XRefNode>( 70, XRefNode(  32,
                                       CivilTime( 2016,  2,  5,  13,  30,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   //NANU  2015019 LAUNCH                   
   NtoPMap.insert( std::pair<const int, XRefNode>( 71, XRefNode(  26,
                                       CivilTime( 2015,   3,  25,  18,  36,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   //NANU  2015068 LAUNCH
   NtoPMap.insert( std::pair<const int, XRefNode>( 72, XRefNode(  8,
                                       CivilTime( 2015,  7,   15, 0,  0,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
   //Added before NANU was sent.
   NtoPMap.insert( std::pair<const int, XRefNode>( 73, XRefNode(  10,
                                       CivilTime( 2015,  10,   31, 16,  23,  0.0, TimeSystem::GPS),
                                       CommonTime::END_OF_TIME  )));
                                         
                                       
      //Iterate through the data to produce the PtoNMap
   multimap<int,XRefNode>::const_iterator itate;
      //Iterates through Navstar by PRN map for each relationship building the corresponding map
   for (itate=NtoPMap.begin(); itate != NtoPMap.end(); itate++)
   {
      std::pair<const int, gpstk::XRefNode> values = *itate;
         //Grabs the values to then insert into PtoN
      int navNum = values.first;
      int prnNum = values.second.getPRNNum();
      TimeRange valid = values.second.getTimeRange();
		//Insert to tree
      PtoNMap.insert( std::pair<const int, XRefNode>( prnNum, XRefNode( navNum, valid )));
   }

}

int SVNumXRef::getNAVSTAR( const int PRNID, const gpstk::CommonTime dt ) const
{
   SVNumXRefPair p = PtoNMap.equal_range( PRNID );
   for (SVNumXRefListCI ci=p.first; ci != p.second; ++ci )
   {
      if (ci->second.isApplicable( dt )) return ci->second.getNAVSTARNum();
   }
   
      // We didn't find a NAVSTAR # for this PRN ID and date, so throw an 
      // exception.
   char textOut[80];
   sprintf(textOut,"No NAVSTAR # found associated with PRN ID %d at requested date: %s.", 
            PRNID,printTime(dt,"%02m/%02d/%04Y").c_str() ); 
   std::string sout = textOut;
   NoNAVSTARNumberFound noFound( sout );
   GPSTK_THROW(noFound); 
   return 0;
}

bool SVNumXRef::NAVSTARIDAvailable( const int PRNID, const gpstk::CommonTime dt ) const
{
   SVNumXRefPair p = PtoNMap.equal_range( PRNID );
   for (SVNumXRefListCI ci=p.first; ci != p.second; ++ci )
   {
      if (ci->second.isApplicable( dt )) return true;
   }
   return false; 
}

bool SVNumXRef::NAVSTARIDActive( const int NAVSTARID, const gpstk::CommonTime dt ) const
{
  return PRNIDAvailable ( NAVSTARID, dt) ; 
}

SVNumXRef::BlockType SVNumXRef::getBlockType( const int NAVSTARID ) const
{
   map<int,BlockType>::const_iterator i;
   i = NtoBMap.find(  NAVSTARID );
   if (i!=NtoBMap.end()) return(i->second);
   
      // We didn't find a BlockType for this NAVSTAR #, so throw an 
      // exception.
   char textOut[80];
   sprintf(textOut,"No BlockType found associated with NAVSTAR Num %d.", 
            NAVSTARID);
   std::string sout = textOut;
   NoNAVSTARNumberFound noFound( sout );
   GPSTK_THROW(noFound); 
   return SVNumXRef::BlockType();
}

std::string SVNumXRef::getBlockTypeString( const int NAVSTARID ) const
{
   std::map<int,BlockType>::const_iterator i;
   i = NtoBMap.find( NAVSTARID );
   if (i!=NtoBMap.end())
   {
	 BlockType bt = i->second;
     switch( bt )
     {
       case I: return("Block I"); break;
       case II: return("Block II"); break;
       case IIA: return("Block IIA"); break;
       case IIR: return("Block IIR"); break;
       case IIR_M: return("Block IIR_M"); break;
       case IIF: return("Block IIF"); break;
     }
   }
   return "unknown";
}

int SVNumXRef::getPRNID( const int NAVSTARID, const gpstk::CommonTime dt ) const
{
   NAVNumXRefPair p = NtoPMap.equal_range( NAVSTARID );
   for (NAVNumXRefCI ci=p.first; ci != p.second; ++ci )
   {
      if (ci->second.isApplicable( dt )) return ci->second.getPRNNum();
   }

      // We didn't find a PRN ID for this NAVSTAR # and date, so throw an 
      // exception.
   char textOut[80];
   sprintf(textOut,"No PRN ID found associated with NAVSTAR Num %d at requested date: %s.", 
            NAVSTARID,printTime(dt,"%02m/%02d/%04Y").c_str() ); 
   std::string sout = textOut;
   NoNAVSTARNumberFound noFound( sout );
   GPSTK_THROW(noFound);
   return 0;
}

bool SVNumXRef::PRNIDAvailable( const int NAVSTARID, const gpstk::CommonTime dt ) const
{
   NAVNumXRefPair p = NtoPMap.equal_range( NAVSTARID );
   for (NAVNumXRefCI ci=p.first; ci != p.second; ++ci )
   {
      if (ci->second.isApplicable( dt )) return true;
   }
   return false; 
}

bool SVNumXRef::BlockTypeAvailable(  const int NAVSTARID ) const
{
   map<int,BlockType>::const_iterator i;
   i = NtoBMap.find(  NAVSTARID );   
   if (i!=NtoBMap.end()) return true;
   return false;
}

//----------Dumps out a List of and ALSO checks for Overlaps is overlap is set to true
//----------Navstar #s---PRN---start time---end time---
//----------PRN---Navstar #s---start time---end time---
void SVNumXRef::dump(std::ostream& out) const
{
      //iterate through the data
   multimap<int,XRefNode>::const_iterator it;
   bool pastCurrent = false;
      //header
   std::string start_end_h = "                              START"
							   "                          END\n";
   std::string svn_h = "    SVN      PRN    MM/DD/YYYY DOY HH:MM:SS"
									   "       MM/DD/YYYY DOY HH:MM:SS\n";
   out << start_end_h << svn_h;
      //Iterates through Navstar by PRN
   for (it=NtoPMap.begin(); it != NtoPMap.end(); it++)
   {
      std::pair<const int, gpstk::XRefNode> mm = *it;
      out << "     " << setw(2) << mm.first 
          << "       " << mm.second.toString() << endl;
   }
   out << "\n\n\n";
      //iterate through the data
   multimap<int,XRefNode>::const_iterator iter;
      //resest pastCurrent
   pastCurrent = false;
      //header
   std::string prn_h = "   PRN       SVN    MM/DD/YYYY DOY HH:MM:SS"
									   "       MM/DD/YYYY DOY HH:MM:SS\n";
   out << start_end_h << prn_h;
      //Iterates through PRN by Navstar
   for (iter = PtoNMap.begin(); iter != PtoNMap.end(); iter++)
   {
      std::pair<const int, gpstk::XRefNode> pp = *iter;
      out << "    " << setw(2) << pp.first 
          << "        " << pp.second.toString() <<endl;
   }
}

//-------------- Methods for XRefNode -----------------
XRefNode::XRefNode( const int NumArg,
			     const gpstk::TimeRange tr )
{	
	Num = NumArg;
	valid = tr;
}
								
XRefNode::XRefNode( const int NumArg,
                             const gpstk::CommonTime begDT,
                             const gpstk::CommonTime endDT )
{
   Num = NumArg;
   valid = TimeRange( begDT, endDT );
}

bool XRefNode::isApplicable( gpstk::CommonTime dt ) const
{
   if (valid.inRange(dt)) return true;
   return false;
}

std::string XRefNode::toString() const
{
   std::string sout;
   std::string tform = "%02m/%02d/%4Y %03j %02H:%02M:%05.2f";
   //create stringstream to convert Num to a string to concatinate to sout
   std::stringstream ss;
   ss << setfill('0') << setw(2) << Num;
   ss << "    " << printTime( valid.getStart(), tform );
   ss << "    ";
   if( valid.getEnd() == CommonTime::END_OF_TIME)
	 ss << "End of Time";
   else
	 ss << printTime( valid.getEnd(), tform );
	
   return ss.str();
}

// Returns true if there are no overlaps, and false otherwise 		  
bool SVNumXRef::isConsistent() const
{
   bool retVal = true;
   // Defining iterators
   multimap<int, XRefNode>::const_iterator cit1;
   multimap<int, XRefNode>::const_iterator cit2;
   // loops through the multimap
   for (cit1 = NtoPMap.begin(); cit1 != NtoPMap.end(); cit1++)
   {
     cit2 = cit1;
     cit2++;  // cit2 always starts the nested loop one higher than cit1
     for (; cit2 != NtoPMap.end(); cit2++)
       {
	 int key1 = cit1->first;		// keys represent the SVN numbers
	 int key2 = cit2->first;
	 const XRefNode xr1 = cit1->second;	// these const xr variables represent the XRefNode so we can access the begin and end times
	 const XRefNode xr2 = cit2->second;	// of each SVN/PRN pair
	 int val1 = xr1.getPRNNum();		// vals represent the PRN numbers
	 int val2 = xr2.getPRNNum();
	 
	 if ((key1 == key2) || (val1 == val2))	// checks initial condition for an overlap; if neither are true, there is no overlap
	   {
	     const TimeRange& tr1 = xr1.getTimeRange();
	     const TimeRange& tr2 = xr2.getTimeRange();
	     if (tr1.overlaps(tr2))
	       {
		 retVal = false;
		 std::cout << "Overlap between SV"
			   << setw(2) << key1 << "/PRN"
			   << setw(2) << val1 << "at"
			   << tr1.printf() << endl;
		 std::cout << "            and"
			   << setw(2) << key2 << "/PRN"
			   << setw(2) << val2 << "at"
			   << tr2.printf() << endl;
	       }
	   }
    
       }
   }
   return retVal;					// if we reach this point, we know there are no overlaps
}      
