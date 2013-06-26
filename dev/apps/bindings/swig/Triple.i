#pragma ident "$Id$"
#ifndef GPSTK_TRIPLE_HPP
#define GPSTK_TRIPLE_HPP
#include <valarray>
#include <vector>
#include "Exception.hpp"
#include "Vector.hpp"
namespace gpstk
{
   NEW_EXCEPTION_CLASS(GeometryException, gpstk::Exception);
   class Triple
   {
   public:
      Triple();

      Triple(const Triple& right);

      Triple(double a,
             double b,
             double c);

      virtual ~Triple() {}

      // Triple& operator=(const Triple& right);

      // Triple& operator=(const std::valarray<double>& right)
      //    throw(GeometryException);

      Vector<double> toVector();

      std::vector<double> toStdVector();

      double dot(const Triple& right) const
         throw();

      Triple cross(const Triple& right) const
         throw();

      double mag() const
         throw();

      Triple unitVector() const
      	 throw(GeometryException);

      double cosVector(const Triple& right) const
         throw(GeometryException);

      double slantRange(const Triple& right) const
         throw();

      double elvAngle(const Triple& right) const
         throw(GeometryException);

      double azAngle(const Triple& right) const
         throw(GeometryException);

      Triple R1(const double& angle) const
         throw();

      Triple R2(const double& angle) const
         throw();

      Triple R3(const double& angle) const
         throw();

      // double& operator[](const size_t index)
      //    { return theArray[index]; }

      // double operator[](const size_t index) const
      //    { return theArray[index]; }

      bool operator== (const Triple& right) const ;


      Triple operator-(const Triple& right) const ;

      Triple operator+(const Triple& right) const ;

      // friend Triple operator*(double right, const Triple& rhs);

      size_t size(void) const
         { return theArray.size(); }

      friend std::ostream& operator<<(std::ostream& s,
                                      const gpstk::Triple& v);

      // std::valarray<double> theArray;

   };
}
#endif


%extend gpstk::Triple {
	double __getitem__(unsigned int i) {
		return $self->theArray[i];
	}
	gpstk::Triple scale(double scalar) {
		return Triple(scalar * $self->theArray[0],
				      scalar * $self->theArray[1],
				      scalar * $self->theArray[2]);
	}
};


%pythoncode %{

# tuple -> triple translation:
def makeTriple(tuple):
	"""Generates a GPSTk-style Triple from a Python tuple."""
	# TODO: add error checking
	return Triple(tuple[0], tuple[1], tuple[2])

# triple -> tuple translation:

makeTuple = lambda self: (self[0], self[1], self[2])
Triple.makeTuple = makeTuple

Triple.__str__ = lambda self: makeTuple(self).__str__()

%}