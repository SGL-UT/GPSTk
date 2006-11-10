#pragma ident "$Id$"

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

/**
 * @file DDid.cpp
 * Implement DDid - double difference identification, a class to handle the
 * bookkeeping of DDs.
 */

//------------------------------------------------------------------------------------
// system includes
#include "DDid.hpp"

//------------------------------------------------------------------------------------
using namespace gpstk;
using namespace std;

//------------------------------------------------------------------------------------
// empty constructor - sites are empty and sats are GPS,-1
//DDid::DDid(void)
//{
//   site1 = site2 = string("");
//   sat1 = sat2 = GSatID(-1,systemGPS);
//   ssat = ssite = 1;
//}

//------------------------------------------------------------------------------------
// explicit constructor - may reorder input to standard form
DDid::DDid(string s1, string s2, GSatID p1, GSatID p2)
{
   if(s1 == s2 || p1.id == p2.id || p1.system != p2.system) {
      Exception e("DDid constructor with identical parts");
      GPSTK_THROW(e);
   }
   if(s1 < s2) ssite=1; else ssite=-1;
   site1 = (s1 < s2 ? s1 : s2);
   site2 = (s1 < s2 ? s2 : s1);

   if(p1.id < p2.id) ssat=1; else ssat=-1;
   sat1 = (p1.id < p2.id ? p1 : p2);
   sat2 = (p1.id < p2.id ? p2 : p1);
}

//------------------------------------------------------------------------------------
// return 0 if either is invalid
// return +1 if the same, -1 if the same except for one switch
// return +2 if left is greater than right
// return -2 if left is less than right
int DDid::compare(const DDid& left, const DDid& right)
{
   if(left.site1 == left.site2 || left.sat1 == left.sat2 ||
      right.site1 == right.site2 || right.sat1 == right.sat2) return 0;
   if(left.site1 < right.site1) return -2;
   if(left.site1 > right.site1) return  2;
   if(left.site2 < right.site2) return -2;
   if(left.site2 > right.site2) return  2;
   if(left.sat1.id < right.sat1.id) return -2;
   if(left.sat1.id > right.sat1.id) return  2;
   if(left.sat2.id < right.sat2.id) return -2;
   if(left.sat2.id > right.sat2.id) return  2;
   return left.ssite*right.ssite*left.ssat*right.ssat;
}

//------------------------------------------------------------------------------------
bool DDid::operator>(const DDid& that) const
{
   return (DDid::compare(*this,that) == 2);
}

//------------------------------------------------------------------------------------
bool DDid::operator==(const DDid& that) const
{
   return (abs(DDid::compare(*this,that)) == 1);
}

//------------------------------------------------------------------------------------
bool DDid::operator!=(const DDid& that) const
{
   return (abs(DDid::compare(*this,that)) != 1);
}

//------------------------------------------------------------------------------------
bool DDid::operator<(const DDid &that) const
{
   return (DDid::compare(*this,that) == -2);
}

//------------------------------------------------------------------------------------
ostream& operator<<(ostream& s, const DDid& t)
{
   if(t.ssite > 0) s << t.site1 << " " << t.site2 << " ";
   else            s << t.site2 << " " << t.site1 << " ";
   if(t.ssat  > 0) s << t.sat1 << " " << t.sat2;
   else            s << t.sat2 << " " << t.sat1;
   return s;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// empty constructor - sites are empty and sats are GPS,-1
//SDid::SDid(void)
//{
//   site1 = site2 = string("");
//   sat = GSatID(-1,systemGPS);
//   ssite = 1;
//}

//------------------------------------------------------------------------------------
// explicit constructor - may reorder input to standard form
SDid::SDid(string s1, string s2, GSatID p)
{
   if(s1 == s2) {
      Exception e("SDid constructor with identical sites");
      GPSTK_THROW(e);
   }
   if(s1 < s2) ssite=1; else ssite=-1;
   site1 = (s1 < s2 ? s1 : s2);
   site2 = (s1 < s2 ? s2 : s1);
   sat = p;
}

//------------------------------------------------------------------------------------
// return 0 if either is invalid
// return +1 if the same, -1 if the same except for one switch
// return +2 if left is greater than right
// return -2 if left is less than right
int SDid::compare(const SDid& left, const SDid& right)
{
   if(left.site1 == left.site2 || right.site1 == right.site2) return 0;
   if(left.site1 < right.site1) return -2;
   if(left.site1 > right.site1) return  2;
   if(left.site2 < right.site2) return -2;
   if(left.site2 > right.site2) return  2;
   if(left.sat.id < right.sat.id) return -2;
   if(left.sat.id > right.sat.id) return  2;
   return left.ssite*right.ssite;
}

//------------------------------------------------------------------------------------
bool SDid::operator>(const SDid& that) const
{
   return (SDid::compare(*this,that) == 2);
}

//------------------------------------------------------------------------------------
bool SDid::operator==(const SDid& that) const
{
   return (abs(SDid::compare(*this,that)) == 1);
}

//------------------------------------------------------------------------------------
bool SDid::operator<(const SDid &that) const
{
   return (SDid::compare(*this,that) == -2);
}

//------------------------------------------------------------------------------------
ostream& operator<<(ostream& s, const SDid& t)
{
   if(t.ssite > 0) s << t.site1 << " " << t.site2 << " " << t.sat;
   else            s << t.site2 << " " << t.site1 << " " << t.sat;
   return s;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// empty constructor - sites are empty and sats are GPS,-1
//OWid::OWid(void)
//{
//   site = string("");
//   sat = GSatID(-1,systemGPS);
//}

//------------------------------------------------------------------------------------
// explicit constructor - may reorder input to standard form
OWid::OWid(string& s, GSatID& p)
{
   site = s;
   sat = p;
}

//------------------------------------------------------------------------------------
// return 0 if either is invalid
// return +1 if the same, -1 if the same except for one switch
// return +2 if left is greater than right
// return -2 if left is less than right
int OWid::compare(const OWid& left, const OWid& right)
{
   int isit=0;
   if(     left.site == right.site) isit=1;
   else if(left.site < right.site) return -2;
   else if(left.site > right.site) return 2;
   int isat=0;
   if(     left.sat.id == right.sat.id) isat=1;
   else if(left.sat.id < right.sat.id) return -2;
   else if(left.sat.id > right.sat.id) return 2;

   return isit*isat;
}

//------------------------------------------------------------------------------------
bool OWid::operator>(const OWid& that) const
{
   return (OWid::compare(*this,that) == 2);
}

//------------------------------------------------------------------------------------
bool OWid::operator==(const OWid& that) const
{
   return (abs(OWid::compare(*this,that)) == 1);
}

//------------------------------------------------------------------------------------
bool OWid::operator<(const OWid &that) const
{
   return (OWid::compare(*this,that) == -2);
}

//------------------------------------------------------------------------------------
ostream& operator<<(ostream& s, const OWid& t)
{
   s << t.site << " " << t.sat;
   return s;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
