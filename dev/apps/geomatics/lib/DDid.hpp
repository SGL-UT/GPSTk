#pragma ident "$Id: $"

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
 * @file DDid.hpp
 * Include file defining DDid - double difference identification, a class to handle
 * the bookkeeping of double differences.
 */

#ifndef CLASS_DOUB_DIFF_ID_INCLUDE
#define CLASS_DOUB_DIFF_ID_INCLUDE

//------------------------------------------------------------------------------------
// system includes
#include <string>
#include <ostream>

// GPSTk
#include "Exception.hpp"
// Geomatics
#include "GSatID.hpp"

//------------------------------------------------------------------------------------
// double difference identification
class DDid {
public:
   std::string site1,site2;
   gpstk::GSatID sat1,sat2;
   int ssite,ssat;
      // empty constructor - sites are empty and sats are GPS,-1
   //DDid(void);
      // explicit constructor - do not reorder
   explicit DDid(std::string s1,
                 std::string s2,
                 gpstk::GSatID p1,
                 gpstk::GSatID p2);

      // 'less than' is required for map<DDid,...>.find(DDid)
   bool operator<(const DDid& right) const;
   bool operator>(const DDid& right) const;
   bool operator==(const DDid& right) const;
   bool operator!=(const DDid& right) const;

   // return 0 if either is invalid
   // return +1 if the same, -1 if the same except for one switch
   // return +2 if left is greater than right
   // return -2 if left is less than right
   static int compare(const DDid& left, const DDid& right);

   friend std::ostream& operator<<(std::ostream& s, const DDid& t);

};

//------------------------------------------------------------------------------------
// single difference id
class SDid {
public:
   std::string site1,site2;
   gpstk::GSatID sat;
   int ssite;
      // empty constructor - sites are empty and sat is GPS,-1
   //SDid(void);
      // explicit constructor - do not reorder
   explicit SDid(std::string s1,
                 std::string s2,
                 gpstk::GSatID p);

   bool operator<(const SDid& right) const;
   bool operator>(const SDid& right) const;
   bool operator==(const SDid& right) const;

   // return 0 if either is invalid
   // return +1 if the same, -1 if the same except for one switch
   // return +2 if left is greater than right
   // return -2 if left is less than right
   static int compare(const SDid& left, const SDid& right);

   friend std::ostream& operator<<(std::ostream& s, const SDid& t);
};

//------------------------------------------------------------------------------------
// one-way data id
class OWid {
public:
   std::string site;
   gpstk::GSatID sat;
      // empty constructor - site is empty and sat is GPS,-1
   //OWid(void);
      // explicit constructor - do not reorder
   explicit OWid(std::string& s1, gpstk::GSatID& p);

   bool operator<(const OWid& right) const;
   bool operator>(const OWid& right) const;
   bool operator==(const OWid& right) const;

   // return 0 if either is invalid
   // return +1 if the same
   // return +2 if left is greater than right
   // return -2 if left is less than right
   static int compare(const OWid& left, const OWid& right);

   friend std::ostream& operator<<(std::ostream& s, const OWid& t);
};

//------------------------------------------------------------------------------------
#endif
