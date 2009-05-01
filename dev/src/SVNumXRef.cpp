/** $Id$
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
// //  Copyright 2004, The University of Texas at Austin
//
//============================================================================
*   SVNumXRefMap.cpp
*
*   Applied Research Laboratories, The University of Texas at Austin
*
*/
   // Language Headers

   // Library Headers
   // Project Headers
#include "SVNumXRef.hpp"

using namespace std;
namespace gpstk
{

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

      // Note: This table start with Block I values
      // Set up NAVSTAR -> PRN ID relationship
      // NAVSTAR ID first, PRN ID second
   NtoPMap.insert( std::pair<const int, XRefNode>(  1, XRefNode(  4, 
                                       DayTime( 1978,  2, 22,  0,  0,  0.0),
                                       DayTime( 1985,  7, 17, 17, 30,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>(  2, XRefNode(  7, 
                                       DayTime( 1978,  6, 13,  0,  0,  0.0),
                                       DayTime( 1988,  2, 12, 23, 59,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>(  3, XRefNode(  6, 
                                       DayTime( 1978, 10,  6,  0,  0,  0.0),
                                       DayTime( 1992,  5, 18, 23, 41,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>(  4, XRefNode(  8, 
                                       DayTime( 1978, 12, 10,  0,  0,  0.0),
                                       DayTime( 1990,  5, 31, 23, 59,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>(  5, XRefNode(  5, 
                                       DayTime( 1980,  2,  9,  0,  0,  0.0),
                                       DayTime( 1984,  5, 11, 23, 59,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>(  6, XRefNode(  9, 
                                       DayTime( 1980,  4, 26,  0,  0,  0.0),
                                       DayTime( 1991,  3,  6,  3, 42,  0.0))));
     // no NAVSTAR 07, I-7 was a launch failure
   NtoPMap.insert( std::pair<const int, XRefNode>(  8, XRefNode( 11, 
                                       DayTime( 1983,  7, 14,  0,  0,  0.0),
                                       DayTime( 1993,  5,  4,  0, 20,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>(  9, XRefNode( 13, 
                                       DayTime( 1984,  6, 13,  0,  0,  0.0),
                                       DayTime( 1993,  5,  4, 18, 17,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 10, XRefNode( 12, 
                                       DayTime( 1984,  9,  8,  0,  0,  0.0),
                                       DayTime( 1996,  3, 26, 23, 59,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 11, XRefNode(  3, 
                                       DayTime( 1985, 10, 30,  0,  0,  0.0),
                                       DayTime( 1994,  4, 14, 21,  0,  0.0))));
     // no NAVSTAR 12, was never launched
   NtoPMap.insert( std::pair<const int, XRefNode>( 13, XRefNode(  2, 
                                       DayTime( 1989,  6, 10,  0,  0,  0.0),
                                       DayTime( 2004,  5, 12, 17,  1,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 14, XRefNode( 14, 
                                       DayTime( 1989,  2, 14,  0,  0,  0.0),
                                       DayTime( 2000,  4, 14, 13, 47,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 15, XRefNode( 15, 
                                       DayTime( 1990, 10,  1,  0,  0,  0.0),
                                       DayTime( 2007,  3, 15, 23, 59,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 16, XRefNode( 16, 
                                       DayTime( 1989,  8, 18,  0,  0,  0.0),
                                       DayTime( 2000, 10, 13,  0, 45,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 17, XRefNode( 17, 
                                       DayTime( 1989, 12, 11,  0,  0,  0.0),
                                       DayTime( 2005,  2, 23, 22,  0,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 18, XRefNode( 18, 
                                       DayTime( 1990,  1, 24,  0,  0,  0.0),
                                       DayTime( 2000,  8, 18,  7, 42,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 19, XRefNode( 19, 
                                       DayTime( 1989, 10, 21,  0,  0,  0.0),
                                       DayTime( 2001,  9, 11, 22,  0,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 20, XRefNode( 20, 
                                       DayTime( 1990,  3, 26,  0,  0,  0.0),
                                       DayTime( 1996, 12, 13, 23, 59,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 21, XRefNode( 21, 
                                       DayTime( 1990,  8,  2,  0,  0,  0.0),
                                       DayTime( 2003,  1, 27, 22,  0,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 22, XRefNode( 22, 
                                       DayTime( 1993,  2,  3,  0,  0,  0.0),
                                       DayTime( 2003,  8,  6, 22,  0,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 23, XRefNode( 23, 
                                       DayTime( 1990, 11, 26,  0,  0,  0.0),
                                       DayTime( 2004,  2, 13, 22,  0,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 23, XRefNode( 32, 
                                       DayTime( 2006, 12,  1,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 24, XRefNode( 24, 
                                       DayTime( 1991,  7,  4,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 25, XRefNode( 25, 
                                       DayTime( 1992,  2, 23,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 26, XRefNode( 26, 
                                       DayTime( 1992,  7,  7,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 27, XRefNode( 27, 
                                       DayTime( 1992,  9,  9,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 28, XRefNode( 28, 
                                       DayTime( 1992,  4, 10,  0,  0,  0.0),
                                       DayTime( 1997,  8, 15, 23, 59,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 29, XRefNode( 29, 
                                       DayTime( 1992, 12, 18,  0,  0,  0.0),
                                       DayTime( 2007, 10, 23, 23, 59,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 30, XRefNode( 30, 
                                       DayTime( 1996,  9, 12,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 31, XRefNode( 31, 
                                       DayTime( 1993,  3, 30,  0,  0,  0.0),
                                       DayTime( 2005, 10, 24, 23, 59,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 32, XRefNode(  1, 
                                       DayTime( 1992, 11, 22,  0,  0,  0.0),
                                       DayTime( 2008,  3, 17, 22,  0,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 33, XRefNode(  3, 
                                       DayTime( 1996,  3, 28,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 34, XRefNode(  4, 
                                       DayTime( 1993, 10, 26,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 35, XRefNode(  5, 
                                       DayTime( 1993,  8, 30,  0,  0,  0.0),
                                       DayTime( 2009,  3, 26, 20, 31,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 36, XRefNode(  6, 
                                       DayTime( 1995,  3, 10,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 37, XRefNode(  7, 
                                       DayTime( 1993,  5, 13,  0,  0,  0.0),
                                       DayTime( 2007,  7, 20, 23, 59,  0.0))));
   NtoPMap.insert( std::pair<const int, XRefNode>( 38, XRefNode(  8, 
                                       DayTime( 1997, 11,  6,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 39, XRefNode(  9, 
                                       DayTime( 1993,  6, 26,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 40, XRefNode( 10, 
                                       DayTime( 1996,  7, 16,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 41, XRefNode( 14, 
                                       DayTime( 2000, 11, 10,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
    // no NAVSTAR 42, IIR-1 was a launch failure
   NtoPMap.insert( std::pair<const int, XRefNode>( 43, XRefNode( 13, 
                                       DayTime( 1997,  7, 23,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 44, XRefNode( 28,
                                       DayTime( 2000,  7, 16,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 45, XRefNode( 21, 
                                       DayTime( 2003,  3, 31,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 46, XRefNode( 11, 
                                       DayTime( 1999, 10,  7,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 47, XRefNode( 22, 
                                       DayTime( 2003, 12, 21,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 48, XRefNode(  7, 
                                       DayTime( 2008,  3, 15,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 49, XRefNode(  1, 
                                       DayTime( 2009,  3, 24,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 51, XRefNode( 20, 
                                       DayTime( 2000,  5, 11,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 52, XRefNode( 31, 
                                       DayTime( 2006,  9, 25,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 53, XRefNode( 17, 
                                       DayTime( 2005,  9, 26,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 54, XRefNode( 18, 
                                       DayTime( 2001,  1, 30,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 55, XRefNode( 15, 
                                       DayTime( 2007, 10, 17,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 56, XRefNode( 16, 
                                       DayTime( 2003,  1, 29,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 57, XRefNode( 29, 
                                       DayTime( 2007, 12, 21,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 58, XRefNode( 12, 
                                       DayTime( 2006, 11, 17,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 59, XRefNode( 19, 
                                       DayTime( 2004,  3, 20,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 60, XRefNode( 23, 
                                       DayTime( 2004,  6, 23,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   NtoPMap.insert( std::pair<const int, XRefNode>( 61, XRefNode(  2, 
                                       DayTime( 2004,  6,  6,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
  
      // Set up PRN ID -> NAVSTAR relationship
      // Note: Because of a bug in the Solaris compler version 5.x,
      // you cannot use make_pair b/c Solaris ASSUMES the key is const AND
      // Sun's implementation of pair lacks the templated copy constructor 
      // template< class a, class b> pair::pair< const pair<a,b>& p >
   PtoNMap.insert( std::pair<const int, XRefNode>(  1, XRefNode( 32, 
                                       DayTime( 1992, 11, 22,  0,  0,  0.0),
                                       DayTime( 2008,  3, 17, 22,  0,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>(  1, XRefNode( 49, 
                                       DayTime( 2009, 3, 24,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>(  2, XRefNode( 13, 
                                       DayTime( 1989,  6, 10,  0,  0,  0.0),
                                       DayTime( 2004,  5, 12, 17,  1,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>(  2, XRefNode( 61, 
                                       DayTime( 2004,  6,  6,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>(  3, XRefNode( 11, 
                                       DayTime( 1985, 10, 30,  0,  0,  0.0),
                                       DayTime( 1994,  4, 14, 21,  0,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>(  3, XRefNode( 33, 
                                       DayTime( 1996,  3, 28,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>(  4, XRefNode(  1, 
                                       DayTime( 1978,  2, 22,  0,  0,  0.0),
                                       DayTime( 1985,  7, 17, 17, 30,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>(  4, XRefNode( 34, 
                                       DayTime( 1993, 10, 26,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>(  5, XRefNode(  5, 
                                       DayTime( 1980,  2,  9,  0,  0,  0.0),
                                       DayTime( 1984,  5, 11, 23, 59,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>(  5, XRefNode( 35, 
                                       DayTime( 1993,  8, 30,  0,  0,  0.0),
                                       DayTime( 2009,  3, 26, 20, 31,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>(  6, XRefNode(  3, 
                                       DayTime( 1978, 10,  6,  0,  0,  0.0),
                                       DayTime( 1992,  5, 18, 23, 41,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>(  6, XRefNode( 36, 
                                       DayTime( 1995,  3, 10,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>(  7, XRefNode(  2, 
                                       DayTime( 1978,  6, 13,  0,  0,  0.0),
                                       DayTime( 1988,  2, 12, 23, 59,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>(  7, XRefNode( 37, 
                                       DayTime( 1993,  5, 13,  0,  0,  0.0),
                                       DayTime( 2007,  7, 20, 23, 59,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>(  7, XRefNode( 48, 
                                       DayTime( 2008,  3, 15,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>(  8, XRefNode(  4, 
                                       DayTime( 1978, 12, 10,  0,  0,  0.0),
                                       DayTime( 1990,  5, 31, 23, 59,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>(  8, XRefNode( 38, 
                                       DayTime( 1997, 11,  6,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>(  9, XRefNode(  6, 
                                       DayTime( 1980,  4, 26,  0,  0,  0.0),
                                       DayTime( 1991,  3,  6,  3, 42,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>(  9, XRefNode( 39, 
                                       DayTime( 1993,  6, 26,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 10, XRefNode( 40, 
                                       DayTime( 1996,  7, 16,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 11, XRefNode(  8, 
                                       DayTime( 1983,  7, 14,  0,  0,  0.0),
                                       DayTime( 1993,  5,  4,  0, 20,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>( 11, XRefNode( 46, 
                                       DayTime( 1999, 10,  7,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 12, XRefNode( 10, 
                                       DayTime( 1984,  9,  8,  0,  0,  0.0),
                                       DayTime( 1996,  3, 26, 23, 59,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>( 12, XRefNode( 58, 
                                       DayTime( 2006, 11, 17,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 13, XRefNode(  9, 
                                       DayTime( 1984,  6, 13,  0,  0,  0.0),
                                       DayTime( 1993,  5,  4, 18, 17,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>( 13, XRefNode( 43, 
                                       DayTime( 1997,  7, 23,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 14, XRefNode( 14, 
                                       DayTime( 1989,  2, 14,  0,  0,  0.0),
                                       DayTime( 2000,  4, 14, 13, 47,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>( 14, XRefNode( 41, 
                                       DayTime( 2000, 11, 10,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 15, XRefNode( 15, 
                                       DayTime( 1990, 10,  1,  0,  0,  0.0),
                                       DayTime( 2007,  3, 15, 23, 59,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>( 15, XRefNode( 55, 
                                       DayTime( 2007, 10, 17,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 16, XRefNode( 16, 
                                       DayTime( 1989,  8, 18,  0,  0,  0.0),
                                       DayTime( 2000, 10, 13,  0, 45,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>( 16, XRefNode( 56, 
                                       DayTime( 2003,  1, 29,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 17, XRefNode( 17, 
                                       DayTime( 1989, 12, 11,  0,  0,  0.0),
                                       DayTime( 2005,  2, 23, 22,  0,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>( 17, XRefNode( 53, 
                                       DayTime( 2005,  9, 26,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 18, XRefNode( 18, 
                                       DayTime( 1990,  1, 24,  0,  0,  0.0),
                                       DayTime( 2000,  8, 18,  7, 42,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>( 18, XRefNode( 54, 
                                       DayTime( 2001,  1, 30,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 19, XRefNode( 19, 
                                       DayTime( 1989, 10, 21,  0,  0,  0.0),
                                       DayTime( 2001,  9, 11, 22,  0,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>( 19, XRefNode( 59, 
                                       DayTime( 2004,  3, 20,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 20, XRefNode( 20, 
                                       DayTime( 1990,  3, 26,  0,  0,  0.0),
                                       DayTime( 1996, 12, 13, 23, 59,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>( 20, XRefNode( 51, 
                                       DayTime( 2000,  5, 11,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 21, XRefNode( 21, 
                                       DayTime( 1990,  8,  2,  0,  0,  0.0),
                                       DayTime( 2003,  1, 27, 22,  0,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>( 21, XRefNode( 45, 
                                       DayTime( 2003,  3, 31,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 22, XRefNode( 22, 
                                       DayTime( 1993,  2,  3,  0,  0,  0.0),
                                       DayTime( 2003,  8,  6, 22,  0,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>( 22, XRefNode( 47, 
                                       DayTime( 2003, 12, 21,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 23, XRefNode( 23, 
                                       DayTime( 1990, 11, 26,  0,  0,  0.0),
                                       DayTime( 2004,  2, 13, 22,  0,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>( 23, XRefNode( 60, 
                                       DayTime( 2004,  6, 23,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 24, XRefNode( 24, 
                                       DayTime( 1991,  7,  4,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 25, XRefNode( 25, 
                                       DayTime( 1992,  2, 23,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 26, XRefNode( 26, 
                                       DayTime( 1992,  7,  7,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 27, XRefNode( 27, 
                                       DayTime( 1992,  9,  9,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 28, XRefNode( 28, 
                                       DayTime( 1992,  4, 10,  0,  0,  0.0),
                                       DayTime( 1997,  8, 15, 23, 59,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>( 28, XRefNode( 44,
                                       DayTime( 2000,  7, 16,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 29, XRefNode( 29, 
                                       DayTime( 1992, 12, 18,  0,  0,  0.0),
                                       DayTime( 2007, 10, 23, 23, 59,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>( 29, XRefNode( 57, 
                                       DayTime( 2007, 12, 21,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 30, XRefNode( 30, 
                                       DayTime( 1996,  9, 12,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 31, XRefNode( 31, 
                                       DayTime( 1993,  3, 30,  0,  0,  0.0),
                                       DayTime( 2005, 10, 24, 23, 59,  0.0))));
   PtoNMap.insert( std::pair<const int, XRefNode>( 31, XRefNode( 52, 
                                       DayTime( 2006,  9, 25,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( std::pair<const int, XRefNode>( 32, XRefNode( 23, 
                                       DayTime( 2006, 12,  1,  0,  0,  0.0),
                                       DayTime::END_OF_TIME  )));
}

int SVNumXRef::getNAVSTAR( const int PRNID, const gpstk::DayTime dt ) const
{
   SVNumXRefPair p = PtoNMap.equal_range( PRNID );
   for (SVNumXRefListCI ci=p.first; ci != p.second; ++ci )
   {
      if (ci->second.isApplicable( dt )) return( ci->second.getNAVSTARNum() );
   }
   
      // We didn't find a NAVSTAR # for this PRN ID and date, so throw an 
      // exception.
   char textOut[80];
   sprintf(textOut,"No NAVSTAR # found associated with PRN ID %d at requested date: %s.", 
            PRNID,dt.printf("%02m/%02d/%04Y").c_str() ); 
   std::string sout = textOut;
   NoNAVSTARNumberFound noFound( sout );
   GPSTK_THROW(noFound); 
}

bool SVNumXRef::NAVSTARIDAvailable( const int PRNID, const gpstk::DayTime dt ) const
{
   SVNumXRefPair p = PtoNMap.equal_range( PRNID );
   for (SVNumXRefListCI ci=p.first; ci != p.second; ++ci )
   {
      if (ci->second.isApplicable( dt )) return( true );
   }
   return( false ); 
}

bool SVNumXRef::NAVSTARIDActive( const int NAVSTARID, const gpstk::DayTime dt ) const
{
   for (SVNumXRefListCI ci=PtoNMap.begin(); ci != PtoNMap.end(); ++ci )
   {
      if (ci->second.getNAVSTARNum()==NAVSTARID &&
          ci->second.isApplicable( dt )         ) return( true );
   }
   return( false ); 
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
   throw( noFound );
}

std::string SVNumXRef::getBlockTypeString( const int NAVSTARID ) const
{
   std::map<int,BlockType>::const_iterator i;
   i = NtoBMap.find( NAVSTARID );
   if (i!=NtoBMap.end())
   {
     switch( getBlockType( NAVSTARID ) )
     {
       case I: return("Block I"); break;
       case II: return("Block II"); break;
       case IIA: return("Block IIA"); break;
       case IIR: return("Block IIR"); break;
       case IIR_M: return("Block IIR_M"); break;
       case IIF: return("Block IIF"); break;
     }

   }
   return("unknown");
}

int SVNumXRef::getPRNID( const int NAVSTARID, const gpstk::DayTime dt ) const
{
   NAVNumXRefPair p = NtoPMap.equal_range( NAVSTARID );
   // If there is only one PRNID for this SVN number return it to maintain
   // compatability with previous versions
   if( p.first == (--p.second) ) return ( p.first->second.getPRNNum() );
   ++p.second;
   for (NAVNumXRefCI ci=p.first; ci != p.second; ++ci )
   {
      if (ci->second.isApplicable( dt )) return( ci->second.getPRNNum() );
   }

      // We didn't find a PRN ID for this NAVSTAR # and date, so throw an 
      // exception.
   char textOut[80];
   sprintf(textOut,"No PRN ID found associated with NAVSTAR Num %d at requested date: %s.", 
            NAVSTARID,dt.printf("%02m/%02d/%04Y").c_str() ); 
   std::string sout = textOut;
   NoNAVSTARNumberFound noFound( sout );
   GPSTK_THROW(noFound); 
   throw( noFound );
}

bool SVNumXRef::PRNIDAvailable( const int NAVSTARID, const gpstk::DayTime dt ) const
{
   NAVNumXRefPair p = NtoPMap.equal_range( NAVSTARID );
   if( p.first == (--p.second) ) return ( true );
   ++p.second;
   for (NAVNumXRefCI ci=p.first; ci != p.second; ++ci )
   {
      if (ci->second.isApplicable( dt )) return( true );
   }
   return( false ); 
}

bool SVNumXRef::BlockTypeAvailable(  const int NAVSTARID ) const
{
   map<int,BlockType>::const_iterator i;
   i = NtoBMap.find(  NAVSTARID );   
   if (i!=NtoBMap.end()) return(true);
   return(false);
}

//-------------- Nethods for XRefNode -----------------
XRefNode::XRefNode( const int NumArg,
                             const gpstk::DayTime begDT,
                             const gpstk::DayTime endDT )
{
   Num = NumArg;
   begValid = begDT;
   endValid = endDT;
}

bool XRefNode::isApplicable( gpstk::DayTime dt ) const
{
   if (dt>=begValid && dt<=endValid) return(true);
   return(false);
}
}
