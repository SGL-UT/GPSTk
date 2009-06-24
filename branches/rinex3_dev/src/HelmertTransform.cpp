#pragma ident "$Id: HelmertTransform.cpp 2009-06-23 9:30:30 tvarney $"

#include "HelmertTransform.hpp"

using namespace gpstk;

const double HelmertTransform::MAS = 7.71605e-10;
const double HelmertTransform::PPB = 1e-9;

HelmertTransform::HelmertTransform()
   throw()
{
   populateTransformMaps();
}

HelmertTransform::HelmertTransform( const HelmertTransform& ht )
   throw()
{
   //Do nothing. This should never be called.
}

HelmertTransform& HelmertTransform::operator=( const HelmertTransform& right )
   throw()
{
   fromMap = right.fromMap;
   return *this;
}

HelmertTransform& HelmertTransform::instance()
                  throw()
{
   static HelmertTransform inst;
   return inst;
}

void HelmertTransform::defineTransform(TransformParameters& tp,
                                       const ReferenceFrame& to,
                                       const ReferenceFrame& from)
                  throw(InvalidParameter&)
{
      //Sanity check, no Unknown frames allowed
   if(from == ReferenceFrame::Unknown || to == ReferenceFrame::Unknown)
   {
      throw InvalidParameter("Unknown ReferenceFrame - Cannot define an Unknown transform.");
   }
   
   //Start Searching
   LookupMap::iterator i = fromMap.find(from);
   if(i != fromMap.end())
   {
      TransformMap tmap = i->second;
      TransformMap::iterator iter = tmap.find(to);
      if(iter != tmap.end())
      {
            //Allow redefinitions
         //throw InvalidParameter("Transformation " + from.asString() +
         //                        " to " + to.asString() + " already exists...");
      }
      //I think...
      tmap[to] = buildTransform(tp);
         //Why, oh why, do I have to do this?
      fromMap[from] = tmap;
   }
   else
   {
         //look for it in reverse order
      i = fromMap.find(to);
      if(i != fromMap.end())
      {
         TransformMap tmap = i->second;
            //look in reverse order
         TransformMap::iterator iter = tmap.find(from);
         if(iter != tmap.end())
         {
               //Since it is defined in reverse already, throw an error
            throw InvalidParameter("Cannot define transformation backwards. A transformation is already defined in the reverse order.");
         }
      }
         //Since it didn't trip that trap, build it their way
      TransformMap tmap;
      tmap[to] = buildTransform(tp);
      fromMap[from] = tmap;
   }
}

//Returns the from / to combination if both from/to and to/from are defined.
Transform& HelmertTransform::getTransform(const ReferenceFrame& from,
                                          const ReferenceFrame& to)
                  throw(InvalidParameter&)
{
      //We want to be able to search through partial definitions
   //if(fromMap.count(to))
   //   throw InvalidParameter("No Transformations defined starting with " + to.asString());
   LookupMap::iterator i = fromMap.find(from);
   if(i == fromMap.end())
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
         TransformMap tmap = i->second;
         TransformMap::iterator iter = tmap.find(to);
         if(iter != tmap.end())
         {
            return iter->second;
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

Position& HelmertTransform::transform(const ReferenceFrame& from,
                                       const ReferenceFrame& to,
                                       Position& pos)
                  throw(InvalidParameter&)
{
      //throw an error if either Reference Frame is unknown
   if(from == ReferenceFrame::Unknown || to == ReferenceFrame::Unknown)
      throw InvalidParameter("Unknown ReferenceFrame - Cannot perform Helmert Transformation.");
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
   if(from == ReferenceFrame::Unknown || to == ReferenceFrame::Unknown)
      throw InvalidParameter("Unknown ReferenceFrame - Cannot perform Helmert Transformation.");
   pos.x = posTransform(from, to, pos.x);
   return pos;
}

Xvt& HelmertTransform::transform(const ReferenceFrame& from,
                                 const ReferenceFrame& to,
                                 Xvt& pos)
                  throw(InvalidParameter&)
{
   if(from == ReferenceFrame::Unknown || to == ReferenceFrame::Unknown)
      throw InvalidParameter("Unknown ReferenceFrame - Cannot perform Helmert Transformation.");
   pos.x = posTransform(from, to, pos.x);
   pos.v = velTransform(from, to, pos.v);
   return pos;
}

Triple& HelmertTransform::posTransform(const ReferenceFrame& from,
                                       const ReferenceFrame& to,
                                       Triple& pos)
                  throw(InvalidParameter&)
{
   if(from == ReferenceFrame::Unknown || to == ReferenceFrame::Unknown)
      throw InvalidParameter("Unknown ReferenceFrame - Cannot perform Helmert Transformation.");
   Vector<double> newPos(3,0.0);
   newPos(0) = pos[0];
   newPos(1) = pos[1];
   newPos(2) = pos[2];
   
   newPos = posTransform(from, to, newPos);
   
   pos = Triple(newPos(0), newPos(1), newPos(2));
   return pos;
}

Triple& HelmertTransform::velTransform(const ReferenceFrame& from,
                                       const ReferenceFrame& to,
                                       Triple& vel)
                  throw(InvalidParameter&)
{
   if(from == ReferenceFrame::Unknown || to == ReferenceFrame::Unknown)
      throw InvalidParameter("Unknown ReferenceFrame - Cannot perform Helmert Transformation.");
   Vector<double> newVel(3,0.0);
   newVel(0) = vel[0];
   newVel(1) = vel[1];
   newVel(2) = vel[2];
   
   newVel = velTransform(from, to, newVel);
   
   vel = Triple(newVel(0), newVel(1), newVel(2));
   return vel;
}

Vector<double> HelmertTransform::posTransform(const ReferenceFrame& from,
                                                const ReferenceFrame& to,
                                                Vector<double>& pos)
                  throw(InvalidParameter&)
{
   if(from == ReferenceFrame::Unknown || to == ReferenceFrame::Unknown)
      throw InvalidParameter("Unknown ReferenceFrame - Cannot perform Helmert Transformation.");
   
   return helperTransform(from, to, pos, true);
}

Vector<double> HelmertTransform::velTransform(const ReferenceFrame& from,
                                                const ReferenceFrame& to,
                                                Vector<double>& vel)
                  throw(InvalidParameter&)
{
   if(from == ReferenceFrame::Unknown || to == ReferenceFrame::Unknown)
      throw InvalidParameter("Unknown ReferenceFrame - Cannot perform Helmert Transformation.");
   
   return helperTransform(from, to, vel, false);
}

Vector<double>& HelmertTransform::helperTransform(const ReferenceFrame& from,
                                 const ReferenceFrame& to,
                                 Vector<double>& vec,
                                 bool translate)
                  throw(InvalidParameter&)
{
   /*LookupMap::iterator i = fromMap.find(from);
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
         vec = vec * t.inverseRotation;
         return vec;
      }
      else
      {
         throw InvalidParameter("Transform " + from.asString() + " to "
                                 + to.asString() + " is not defined.");
      }*/
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
   }
      //Didn't return, so check the next possiblility
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
      vec = vec * t.inverseRotation;
      return vec;
   }
   else
   {
      throw InvalidParameter("Transform " + from.asString() + " to "
                              + to.asString() + " is not defined.");
   }
}//End helperTransform();

void HelmertTransform::populateTransformMaps()
      throw()
{
   TransformParameters pz;
      pz.scale = -3e-9;
      pz.r1 = -19 * MAS;
      pz.r2 = -4 * MAS;
      pz.r3 = 353 * MAS;
      pz.t1 =  0.0700;
      pz.t2 = -0.0567;
      pz.t3 = -0.7733;
   
   ReferenceFrame rf(ReferenceFrame::WGS84);
   ReferenceFrame rf2(ReferenceFrame::PZ90);
   defineTransform(pz, rf, rf2);
}

Transform HelmertTransform::buildTransform(TransformParameters& tp)
   throw()
{
   Transform trans;
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
   
   //Why the * on trans?
   return trans;
}

void HelmertTransform::dump()
{
   ReferenceFrame rf1;
   ReferenceFrame rf2;
   LookupMap::iterator i = fromMap.begin();
   while(i != fromMap.end())
   {
      rf1 = i->first;
      cout << rf1 << endl;
      
      TransformMap tmap = i->second;
      TransformMap::iterator iter = tmap.begin();
      while(iter != tmap.end())
      {
         rf2 = iter->first;
         cout << " -> " << rf2 << endl;
         iter++;
      }
      
      ++i;
   }
}
