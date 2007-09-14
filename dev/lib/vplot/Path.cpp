
/// @file Path.cpp Class to represent a series of points. Class definitions.

#include <cmath>

#include "Path.hpp"

// TO DO: unify constants. Note intentional disuse of ICD GPS 200's pi.
static const double pi=3.14159265358979323846264;


namespace vplot
{
   void Path::addPointRelative(double X, double Y)
   { 
      std::pair<double,double> p(X,Y);
      this->push_back(p);
      return;
   }

   void Path::addPointAbsolute(double X, double Y)
   {
      addPointRelative(X - originX, Y-originY);
      return;
   }

   void Path::addPointDelta(double DX, double DY) throw(VPlotException)
   { 
      using namespace std;

      if (this->empty())
         throw VPlotException("Using relative operation on empty path list.");
  
      list< pair<double, double> >::iterator lastPair = this->end();
      lastPair--;
      std::pair<double,double> p(lastPair->first-DX,lastPair->second-DY);
      this->push_back(p);
      return;
   }

   void Path::rotate(double angleDegrees, double rx, double ry)
   {
      using namespace std;

      double theta = angleDegrees*pi/180,
         costheta = cos(theta),
         sintheta = sin(theta);
      
      for (Path::iterator i=this->begin(); i!=this->end(); i++)
      {
         double dx = (i->first  + originX) - rx;
         double dy = (i->second + originY) - ry;

         i->first  = rx + dx*costheta - dy*sintheta;
         i->second = ry + dx*sintheta + dy*costheta;
      }
   }
   
   void Path::rotate(double angleDegrees)
   {
      this->rotate(angleDegrees, originX, originY);
   }

   void Path::translate(double deltaX, double deltaY)
   {
      originX += deltaX;
      originY += deltaY;      
   }

   Path Path::asAbsolute(void) const
   {
      Path absPath=*this;
      
      for (Path::iterator i=absPath.begin(); i!=absPath.end(); i++)
      {
         i->first  = i->first + originX;
         i->second = i->second + originY;
      }      

      return absPath;
   }
   
} // namespace vplot
