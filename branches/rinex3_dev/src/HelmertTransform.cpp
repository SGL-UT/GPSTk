#pragma ident "$Id: HelmertTransform.cpp 2009-06-19 10:50:00 tvarney $"

#include "HelmertTransform.hpp"

const HelmertTransform& HelmertTransform::instance()
                  throw()
{
   static HelmertTransform inst;
   return inst;
}

void HelmertTransform::defineTransform(const TransformParameters& tp,
                                       const ReferenceFrame& to,
                                       const ReferenceFrame& from)
                  throw()
{
   
}
   //Should this throw an error if they search for the inverse or
   //give them the inverse anyways?
Transform& HelmertTransform::getTransform(const ReferenceFrame& from,
                                          const ReferenceFrame& to)
                  throw(InvalidParameter&)
{
   
}

Position& HelmertTransform::transform(const ReferenceFrame& from,
                                       const ReferenceFrame& to,
                                       Position& pos)
                  throw(InvalidParameter&)
{
   pos.transformTo(Position::Cartesian);
   Triple newPosition = (Triple)pos;
   newPosition = posTransform(from, to, newPosition);
   //Not sure if I need to use setECEF or if it is already set.
   return pos.setECEF(newPosition[0], newPosition[1], newPosition[2]);
}
Xt& HelmertTransform::transform(const ReferenceFrame& from,
                                 const ReferenceFrame& to,
                                 Xt& pos)
                  throw(InvalidParameter&)
{
   pos.x = posTransform(from, to, pos.x);
   return pos;
}
Xvt& HelmertTransform::transform(const ReferenceFrame& from,
                                 const ReferenceFrame& to,
                                 Xvt& pos)
                  throw(InvalidParameter&)
{
   pos.x = posTransform(from, to, pos.x);
   pos.v = velTransform(from, to, pos.v);
   return pos;
}
Triple& HelmertTransform::posTransform(const ReferenceFrame& from,
                                       const ReferenceFrame& to,
                                       Triple& pos)
                  throw(InvalidParameter&)
{
   Vector<double> newPos(3,0.0);
   newPos(0) = pos[0];
   newPos(0) = pos[0];
   newPos(0) = pos[0];
   
   newPos = velTransform(from, to, newPos);
   
   pos = Triple(newPos(0), newPos(1), newPos(2))
   return pos;
}
Triple& HelmertTransform::velTransform(const ReferenceFrame& from,
                                       const ReferenceFrame& to,
                                       Triple& vel)
                  throw(InvalidParameter&)
{
   Vector<double> newVel(3,0.0);
   newVel(0) = vel[0];
   newVel(0) = vel[0];
   newVel(0) = vel[0];
   
   newVel = velTransform(from, to, newVel);
   
   vel = Triple(newVel(0), newVel(1), newVel(2))
   return vel;
}
Vector<double> HelmertTransform::posTransform(const ReferenceFrame& from,
                                                const ReferenceFrame& to,
                                                Vector<double>& pos)
                  throw(InvalidParameter&)
{
   return helperTransform(from, to, pos, true);
}

Vector<double> HelmertTransform::velTransform(const ReferenceFrame& from,
                                                const ReferenceFrame& to,
                                                Vector<double>& vel)
                  throw(InvalidParameter&)
{
   return helperTransform(from, to, vel, false);
}

Vector<double>& helperTransform(ReferenceFrame& from,
                                 ReferenceFrame& to,
                                 Vector<double>& vec,
                                 bool translate)
                  throw(InvalidParameter&)
{
   LookupMap::iterator i = fromMap.find(from);
   if( i != fromMap.end())
   {
      TransformMap t = i->second;
      TransformMap::iterator iter = t.find(to);
      if(iter != t.end())
      {
         Transform t = iter->second;
         vec = vec * t.rotation;
         if(translate)
         {
            vec += t.translation;
         }
         return vec;
      }
      else
      {
         throw InvalidParameter("Transform " + from.asString() + " to "
                                 + to.asString() + " is not defined.");
      }
   }
   else
   {
      i = fromMap.find(to);
      if(i == fromMap.end())
      {
         throw InvalidParameter("Transform " + from.asString() + " to "
                                 + to.asString() + " is not defined.");
      }
      TransformMap t = i->second;
      TransformMap::iterator iter = t.find(from);
      if(iter != t.end())
      {
         Transform t = iter->second;
         if(translate)
         {
            vec -= t.translation;
         }
         vec = vec * inverseRotation;
      }
      else
      {
         throw InvalidParameter("Transform " + from.asString() + " to "
                                 + to.asString() + " is not defined.");
      }
   }
}
