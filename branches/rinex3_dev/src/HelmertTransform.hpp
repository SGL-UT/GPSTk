#pragma ident "$Id: HelmertTransform.hpp 2009-06-25 14:10:20 tvarney $"

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

#ifndef GPSTK_HELMERT_TRANSFORM_HPP
#define GPSTK_HELMERT_TRANSFORM_HPP

#include <map>
#include <iostream>
#include <string>

#include "Position.hpp"
#include "Xt.hpp"
#include "Xvt.hpp"
#include "Matrix.hpp"
#include "Vector.hpp"
#include "MatrixOperators.hpp"
#include "ReferenceFrame.hpp"
#include "Exception.hpp"

namespace gpstk{
   
   using namespace std;
   using namespace gpstk;
   
      //The struct that gets passed to this class
      //holds basic information about the transformation, including
      //it's rotation parameters, scale factor and translation values.
      //In addition, there is a string for a brief description of the
      //publication from which these parameters were taken, or other
      //information deemed neccessary.
   struct TransformParameters{
      double r1;
      double r2;
      double r3;
      double t1;
      double t2;
      double t3;
      double scale;
      
         //used for publication info
      std::string description;
   };//struct TransformParameters
   
      //The stored struct, takes the params object and builds the matricies.
   struct Transform{
      TransformParameters params;
      
      Matrix<double> rotation;
      Matrix<double> inverseRotation;
      Vector<double> translation;
   };//struct Transform
   
      //Map to the different transforms available for a specific
      //reference frame. The key is the reference frame to translate
      //to, with the value being the transform data.
   typedef std::map<ReferenceFrame, Transform> TransformMap;
      //Map to the different available reference frames. The key is the
      //reference frame, with the value being a map to it's available
      //transforms. To reduce redundancy, not all available frames are
      //listed here. For instance, a frame will not be listed if it is
      //already mapped in one of TransformMaps.
   typedef std::map<ReferenceFrame, TransformMap> LookupMap;
   
   class HelmertTransform{
      public:
      ~HelmertTransform() {   };
      
      static const double MAS;
      static const double PPB;
      
      static HelmertTransform& instance()
                        throw();
      
      void dump();
      
         //If the transform to/from combination or it's inverse
         //from/to do not exist this function defines a new Transform
         //mapping. Otherwise, this function redefines the current
         //mapping to use the provided transform parameters.
      void defineTransform(struct TransformParameters& tp,
                           const ReferenceFrame& to,
                           const ReferenceFrame& from)
                        throw(InvalidParameter&);
      
      struct Transform& getTransform(const ReferenceFrame& from,
                                       const ReferenceFrame& to)
                        throw(InvalidParameter&);
      
      
         // The transformation methods //
      Position& transform(const ReferenceFrame& from,
                           const ReferenceFrame& to,
                           Position& pos)
                        throw(InvalidParameter&);
      
      Xt& transform(const ReferenceFrame& from,
                     const ReferenceFrame& to,
                     Xt& pos)
                        throw(InvalidParameter&);
      
      Xvt& transform(const ReferenceFrame& from,
                     const ReferenceFrame& to,
                     Xvt& pos)
                        throw(InvalidParameter&);
      
      Triple& posTransform(const ReferenceFrame& from,
                           const ReferenceFrame& to,
                           Triple& pos)
                        throw(InvalidParameter&);
      
      Triple& velTransform(const ReferenceFrame& from,
                           const ReferenceFrame& to,
                           Triple& vel)
                        throw(InvalidParameter&);
      
      Vector<double> posTransform(const ReferenceFrame& from,
                                    const ReferenceFrame& to,
                                    Vector<double>& pos)
                        throw(InvalidParameter&);
      
      Vector<double> velTransform(const ReferenceFrame& from,
                                    const ReferenceFrame& to,
                                    Vector<double>& vel)
                        throw(InvalidParameter&);
      
      protected:
      
         //This function does the work for the other functions.
      Vector<double>& helperTransform(const ReferenceFrame& from,
                                       const ReferenceFrame& to,
                                       Vector<double>& vec,
                                       bool translate)
                        throw(InvalidParameter&);
      
      //Initializer
      void populateTransformMaps()
                        throw();
      
      Transform buildTransform(TransformParameters& tp)
         throw();
      
      LookupMap fromMap;
      
         //Constructors. Because we want this class to be a singleton,
         //these need to be private or protected.
      HelmertTransform()
         throw();
      HelmertTransform(const HelmertTransform& ht)
         throw();
      HelmertTransform& operator=(const HelmertTransform& ht)
         throw();
   };//class HelmertTransform
   
}//namespace gpstk

#endif
