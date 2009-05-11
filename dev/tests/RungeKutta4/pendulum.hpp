#pragma ident "$Id$"
//file: pendulum.hpp

#include <math.h>
#include "RungeKutta4.hpp"

using namespace gpstk;
using namespace std;

//A pendulum is of the equation d^2x/dt^2 + (g/L)*sin(x) = 0

// The full, nonlinear equation of motion for a simple pendulum
class PendulumIntegrator : public RungeKutta4
{
   public:

      PendulumIntegrator(const Matrix<double>& initialState,
                         double initialTime=0)
            : RungeKutta4(initialState, initialTime)
      {};     

      virtual gpstk::Matrix<double>&
         derivative(long double time, 
                    const gpstk::Matrix<double>& inState,
                    gpstk::Matrix<double>& inStateDot);

      void setPhysics(double accGrav, double length) 
      {  g = accGrav; L = length; };
   
   private: 

   double g; //< the acceleration due to gravity
   double L; //< the length of the pendulum (in meters?)
};

gpstk::Matrix<double>&
PendulumIntegrator::derivative(long double time, 
                               const gpstk::Matrix<double>& inState,
                               gpstk::Matrix<double>& inStateDot)
{
   inStateDot(0,0) = inState(1,0); // velocity along x 
   inStateDot(1,0) = -g/L * sin(inState(0,0));
   return inStateDot;
}
