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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include <iostream>

#include "HelmertTransform.hpp"

using namespace gpstk;
using namespace std;

// Exception messages

static const string unknownExceptionTeXvt =
   "Unknown ReferenceFrame - Cannot perform Helmert Transformation.";
static const string unknownDefinitionTeXvt =
   "Unknown ReferenceFrame - Cannot define an Unknown transform.";
static const string backwardsDefinition =
   "Cannot define transformation backwards. A transformation is already"
   " defined in the reverse order.";

// Units

const double HelmertTransform::MAS = 7.71605e-10;
const double HelmertTransform::PPB = 1e-9;

// Used by the static instance method to build the maps the first time around.

HelmertTransform::HelmertTransform()
   throw()
{
   populateTransformMaps();
}

// Protected; shouldn't be used. Here to keep the compiler from defining it.

HelmertTransform& HelmertTransform::operator=( const HelmertTransform& right )
   throw()
{
   return *this;
}

// Funtion static variable, initialized the first time around only.

HelmertTransform& HelmertTransform::instance()
{
   static HelmertTransform inst;
   return inst;
}

///--------------------------Define/Get Transforms--------------------------///

void HelmertTransform::defineTransform(const HelmertTransform::TransformParameters& tp,
                                       const ReferenceFrame& to,
                                       const ReferenceFrame& from)
   throw(InvalidParameter)
{
   // Sanity check; no Unknown frames allowed.
   if (from == ReferenceFrame::Unknown || to == ReferenceFrame::Unknown)
   {
      throw InvalidParameter(unknownDefinitionTeXvt);
   }
   
   // Start searching.
   LookupMap::iterator iter = fromMap.find(from);
   if (iter != fromMap.end())
   {
      iter->second[to] = buildTransform(tp);
      fromMap[from] = iter->second;
   }
   else  // Look for it in reverse order.
   {
      iter = fromMap.find(to);
      if (iter != fromMap.end())
      {
            // Check in reverse order.
         TransformMap::iterator transIter = (iter->second).find(from);
         if (transIter != (iter->second).end())
         {
            // Since it is defined in reverse already, throw an error
            // We don't allow reverse redefinitions right now because it
            // could cause some ambiguity in the maps.
            throw InvalidParameter(backwardsDefinition);
         }
      }
      // Since it didn't trip that trap, build it their way.
      TransformMap tmap;
      tmap[to] = buildTransform(tp);
      fromMap[from] = tmap;
   }
}


// Returns the from/to combination if both from/to and to/from are defined.
// This method is VERY similar to the defineTransform method.

HelmertTransform::Transform& HelmertTransform::getTransform(const ReferenceFrame& from,
                                                            const ReferenceFrame& to)
   throw(InvalidParameter)
{
   LookupMap::iterator iter = fromMap.find(from);
   if (iter == fromMap.end())
   {
      throw InvalidParameter("No Transformations defined from " +
                              from.asString() + " to " +
                              to.asString() +
                              ". Could it be defined as " +
                              to.asString() + " to " + 
                              from.asString() + "?");
   }
   else
   {
         TransformMap::iterator transIter = (iter->second).find(to);
         if (transIter != iter->second.end())
         {
            return transIter->second;
         }
         else
         {
            throw InvalidParameter("No Transformations defined from " +
                                    from.asString() + " to " +
                                    to.asString() +
                                    ". Could it be defined as " +
                                    to.asString() + " to " + 
                                    from.asString() + "?");
         }
   }
}

///----------------------------Transform Methods----------------------------///

// Position transform, calls the triple method posTransform.

Position HelmertTransform::transform(const ReferenceFrame& to,
                                     const Position& pos      )
   throw(InvalidParameter)
{
   // Throw an error if either Reference Frame is unknown.
   if (pos.getFrame() == ReferenceFrame::Unknown || to == ReferenceFrame::Unknown)
      throw InvalidParameter(unknownExceptionTeXvt);

   Position cartPos = pos;
   cartPos.transformTo(Position::Cartesian);
   Triple newPosition = (Triple)cartPos;
   newPosition = posTransform(cartPos.getFrame(), to, newPosition);

   cartPos.setReferenceFrame(to);
   // Not sure if this needs to use setECEF or if it is already set.
   return cartPos.setECEF(newPosition[0], newPosition[1], newPosition[2]);
}


// Xvt transform, calls the triple methods posTransform and velTransform.

Xvt HelmertTransform::transform(const ReferenceFrame& to,
                                const Xvt& pos           )
   throw(InvalidParameter)
{
   if (pos.frame == ReferenceFrame::Unknown || to == ReferenceFrame::Unknown)
      throw InvalidParameter(unknownExceptionTeXvt);

   Xvt newXvt = pos;

   newXvt.x = posTransform(newXvt.frame, to, newXvt.x);
   newXvt.v = velTransform(newXvt.frame, to, newXvt.v);

   newXvt.frame = to;
   return newXvt;
}


// Triple position transform, calls the Vector method posTransform.

Triple HelmertTransform::posTransform(const ReferenceFrame& from,
                                      const ReferenceFrame& to,
                                      const Triple& pos          )
   throw(InvalidParameter)
{
   if (from == ReferenceFrame::Unknown || to == ReferenceFrame::Unknown)
      throw InvalidParameter(unknownExceptionTeXvt);
   
   Vector<double> newPos(3,0.0);
   newPos(0) = pos[0];
   newPos(1) = pos[1];
   newPos(2) = pos[2];
   
   newPos = posTransform(from, to, newPos);
   
   return Triple(newPos(0), newPos(1), newPos(2));
}


// Triple velocity transform, calls the Vector method velTransform.

Triple HelmertTransform::velTransform(const ReferenceFrame& from,
                                      const ReferenceFrame& to,
                                      const Triple& vel          )
   throw(InvalidParameter)
{
   if (from == ReferenceFrame::Unknown || to == ReferenceFrame::Unknown)
      throw InvalidParameter(unknownExceptionTeXvt);

   Vector<double> newVel(3,0.0);
   newVel(0) = vel[0];
   newVel(1) = vel[1];
   newVel(2) = vel[2];

   newVel = velTransform(from, to, newVel);

   return Triple(newVel(0), newVel(1), newVel(2));
}


//Vector position transform, calls helperTransform with true as an arg

Vector<double> HelmertTransform::posTransform(const ReferenceFrame& from,
                                              const ReferenceFrame& to,
                                              const Vector<double>& pos  )
   throw(InvalidParameter)
{
   if (from == ReferenceFrame::Unknown || to == ReferenceFrame::Unknown)
      throw InvalidParameter(unknownExceptionTeXvt);

   return helperTransform(from, to, pos, true);
}


//Vector velocity transfrom, calls helperTransform with false as an arg.

Vector<double> HelmertTransform::velTransform(const ReferenceFrame& from,
                                              const ReferenceFrame& to,
                                              const Vector<double>& vel  )
   throw(InvalidParameter)
{
   if (from == ReferenceFrame::Unknown || to == ReferenceFrame::Unknown)
      throw InvalidParameter(unknownExceptionTeXvt);

   return helperTransform(from, to, vel, false);
}

///----------------------------Protected Methods----------------------------///

// Looks up and resolves the needed transform, then applies it.
// If translate is true, the translation vector is used, otherwise not.

Vector<double> HelmertTransform::helperTransform(const ReferenceFrame& from,
                                                 const ReferenceFrame& to,
                                                 const Vector<double>& pvec,
                                                 bool translate             )
   throw(InvalidParameter)
{
   Vector<double> vec = pvec;

   // Search for the from ReferenceFrame.  Forward definition lookup first.
   LookupMap::iterator iter = fromMap.find(from);
   if ( iter != fromMap.end())
   {
      // Find the "to" ReferenceFrame in the previous frames map.
      TransformMap::iterator transIter = (iter->second).find(to);
      if (transIter != (iter->second).end())
      {
         // Apply the transform, conditionally use the translation vector.
         vec = vec * (transIter->second).rotation;
         if (translate)
         {
            vec += (transIter->second).translation;
         }
         return vec;
      }
   }
   // Didn't return, so check it in reverse.
   iter = fromMap.find(to);
   if (iter == fromMap.end())
   {
   	   //Not defined either way, throw an exception
      throw InvalidParameter("Transform " + from.asString() + " to "
                              + to.asString() + " is not defined.");
   }
   // To is defined at the base level, search its map for From.
   // Searching for the reverse transformation.
   TransformMap::iterator transIter = (iter->second).find(from);
   if (transIter != (iter->second).end())
   {
      // Conditionally apply the translation, then apply the rotation.
      if (translate)
      {
         vec -= (transIter->second).translation;
      }
      vec = vec * (transIter->second).inverseRotation;
      return vec;
   }
   else
   {
      // Neither the to or from ReferenceFrame is defined at base level.
      throw InvalidParameter("Transform " + from.asString() + " to "
                             + to.asString() + " is not defined.");
   }
} // end helperTransform();


// Build the PZ90->WGS84 transform and register it.

void HelmertTransform::populateTransformMaps()
{
   HelmertTransform::TransformParameters pz;
      pz.scale = -3e-9;
      pz.r1 = -19 * MAS;
      pz.r2 = -4 * MAS;
      pz.r3 = 353 * MAS;
      pz.t1 =  0.0700;
      pz.t2 = -0.0567;
      pz.t3 = -0.7733;
      pz.description = "Parameters taken from ITRS, PZ-90 and WGS 84: current"
                       " realizations and the\nrelated transformation parameters "
                       "- C. Boucher, Z.Altamimi";

   ReferenceFrame rf(ReferenceFrame::WGS84);
   ReferenceFrame rf2(ReferenceFrame::PZ90);
   defineTransform(pz, rf, rf2);
}


// Builds a transform struct from a transform parameters struct.
// This does not apply units to the parameters.

HelmertTransform::Transform HelmertTransform::buildTransform(const HelmertTransform::TransformParameters& tp)
   throw()
{
   HelmertTransform::Transform trans;
   trans.params = tp;
   trans.rotation = Matrix<double>(3,3,0.0);

   trans.rotation(0,0) = tp.scale + 1;
   trans.rotation(0,1) = -tp.r3;
   trans.rotation(0,2) = tp.r2;

   trans.rotation(1,0) = tp.r3;
   trans.rotation(1,1) = tp.scale + 1;
   trans.rotation(1,2) = -tp.r1;

   trans.rotation(2,0) = -tp.r2;
   trans.rotation(2,1) = tp.r1;
   trans.rotation(2,2) = tp.scale + 1;

   trans.translation = Vector<double>(3,0.0);

   trans.translation(0) = tp.t1;
   trans.translation(1) = tp.t2;
   trans.translation(2) = tp.t3;

   trans.inverseRotation = inverse(trans.rotation);

   return trans;
}


// Print out the contents of the Transform maps.

void HelmertTransform::dump()
{
   for (LookupMap::iterator out = fromMap.begin(); out != fromMap.end(); out)
   {
      cout << out->first << endl;

      TransformMap::iterator in; // Saves a little space in the For loop.
      for (in = (out->second).begin(); in != (out->second).end(); ++in)
      {
         cout << " -> " << (in->first) << endl;
      }
   }
}
