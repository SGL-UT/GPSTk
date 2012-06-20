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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#ifndef GPSTK_HELMERT_TRANSFORM_HPP
#define GPSTK_HELMERT_TRANSFORM_HPP

#include <map>
#include <string>

#include "Exception.hpp"
#include "Matrix.hpp"
#include "MatrixOperators.hpp"
#include "Position.hpp"
#include "ReferenceFrame.hpp"
#include "Vector.hpp"
#include "Xvt.hpp"

namespace gpstk
{
   class HelmertTransform
   {

   public:

      //Forward declarations so the structs don't have to be moved.
      struct Transform;
      struct TransformParameters;

         /**
          * @brief Top level map typedef, used in the LookupMap typedef.
          * 
          * Map to the different transforms available for a specific
          * reference frame.  The key is the reference frame to translate
          * to, with the value being the transform data.
          */
      typedef std::map<ReferenceFrame, Transform> TransformMap;

         /**
          * @brief Base Level map typedef, used in initial lookups.
          * 
          * Map to the different available reference frames. The key is the
          * reference frame, with the value being a map to its available
          * transforms.  To reduce redundancy, not all available frames are
          * listed here.  For instance, a frame will not be listed if it is
          * already mapped in TransformMaps.
          */
      typedef std::map<ReferenceFrame, TransformMap> LookupMap;

         /// The constant defining the unit Milliarcsecond (mas).
         /// Used for the Rotation parameters in the TransformParameters struct.
      static const double MAS;
         /// The constant defining the unit Parts Per Billion (ppb).
         /// Used for the scale parameter in the TransformParameters struct.
      static const double PPB;

      /**
       * @brief Creates an instance of the HelmertTransform class and returns it.
       * 
       * @return A singleton reference to the HelemrtTransform class.
       */
      static HelmertTransform& instance();

      /**
       * @brief Dumps the contents of the lookup map to the standard output.
       */
      void dump();

      ///--------------------------Define/Get Transforms--------------------------///

      /**
       * @brief Defines a new Transform using the given to/from combination
       *        and params.
       * 
       * If the transform to/from combination or its inverse from/to do not
       * exist, this function defines a new Transform mapping.  Otherwise,
       * this function redefines the current mapping to use the provided
       * transform parameters.
       * 
       * @throw InvalidParameter If either ReferenceFrame is unknown or no
       *                         transform is defined for them.
       * 
       * @param tp    The transform parameters for this new transformation.
       * @param to    The ReferenceFrame this transform changes coodinates to.
       * @param from  The ReferenceFrame this transform changes from.
       */
      void defineTransform(const TransformParameters& tp,
                           const ReferenceFrame& to,
                           const ReferenceFrame& from    )
         throw(InvalidParameter);

      /**
       * @brief Returns the Transform associated with the to/from ReferenceFrame pair.
       * 
       * If the to/from combination does not exist this funtion throws an
       * InvalidParameter exception. This method only searchs for the forward
       * definition that the user provided and will not return a backwards
       * transform definition.
       * 
       * @throw InvalidParameter If either ReferenceFrame is unknown or no transform is defined for them.
       * 
       * @param from The ReferenceFrame that the desired transform originates in.
       * @param to The ReferenceFrame that the desired transform changes to.
       */
      Transform& getTransform(const ReferenceFrame& from,
                              const ReferenceFrame& to   )
         throw(InvalidParameter);

      ///----------------------------Transform Methods----------------------------///

      /**
       * @brief Transforms a Position object to the specified ReferenceFrame.
       */
      Position transform(const ReferenceFrame& to,
                         const Position& pos      )
         throw(InvalidParameter);

      /**
       * @brief Transforms a Xvt object to the specified ReferenceFrame.
       */
      Xvt transform(const ReferenceFrame& to,
                    const Xvt& pos           )
         throw(InvalidParameter);

      /**
       * @brief Transforms a triple in the "to" ReferenceFrame to the "from"
       *        ReferenceFrame.
       * 
       * This method treats the triple as a position and applies a position
       * transformation to the triple.  This means that in addition to the
       * rotation, the method applies a translation to the Triple.
       * 
       * @throw InvalidParameter If either ReferenceFrame is Unknown, or
       *                         the transform doesn't exist.
       * 
       * @param from  The ReferenceFrame the Triple is initially in.
       * @param to    The ReferenceFrame to transform to.
       * @param pos   The position triple to transform.
       * 
       * @return A transformed Triple object.
       */
      Triple posTransform(const ReferenceFrame& from,
                          const ReferenceFrame& to,
                          const Triple& pos          )
         throw(InvalidParameter);

      /**
       * @brief Transforms a triple in the "to" ReferenceFrame to the "from"
       *        ReferenceFrame.
       * 
       * This method treats the triple as a velocity and applies the velocity
       * transformation to the triple.  This means that the translation is not
       * applied to the triple when the transform is made.
       * 
       */
      Triple velTransform(const ReferenceFrame& from,
                          const ReferenceFrame& to,
                          const Triple& vel          )
         throw(InvalidParameter);

      /**
       * @brief Transforms a Vector in the "to" ReferenceFrame to the "from"
       *        ReferenceFrame.
       * 
       * This method treats the vector as a position and applies the position
       * transformation to the vector.  This means that in addition to the
       * rotation, the method also applies a translation to the Vector.
       */
      Vector<double> posTransform(const ReferenceFrame& from,
                                  const ReferenceFrame& to,
                                  const Vector<double>& pos  )
         throw(InvalidParameter);

      /**
       * @brief Transforms a Vector in the "to" ReferenceFrame to the "from"
       * ReferenceFrame.
       * 
       * This method treats the Vector as a velocity and applies the velocity
       * transformation to the Vector. This means that the translation is not
       * applied to the vector when the transform is made.
       */
      Vector<double> velTransform(const ReferenceFrame& from,
                                  const ReferenceFrame& to,
                                  const Vector<double>& vel  )
         throw(InvalidParameter);

         // The struct that gets passed to this class.
         // Holds basic information about the transformation, including
         // its rotation parameters, scale factor and translation values.
         // In addition, there is a string for a brief description of the
         // publication from which these parameters were taken, or other
         // information deemed neccessary.
      /**
       * @brief A set of parameters that define a Helmert Transform.
       * 
       * This struct defines a set of parameters used in building a Helmert
       * Transform.  All fields must have a value except the description.
       * Values of zero cause no effect, thus a value of 0.0 for the scale
       * would result in no scale.
       * 
       * @note Unit must be applied to the parameters by the user.
       */
      struct TransformParameters
      {
         double r1;   ///< The X Axis rotation value in degrees.
         double r2;   ///< The Y Axis rotation value in degrees.
         double r3;   ///< The Z Axis rotation value in degrees.
         double t1;   ///< The X Axis translation value in meters.
         double t2;   ///< The Y Axis translation value in meters.
         double t3;   ///< The Z Axis translation value in meters.
         double scale;   ///< The scale factor of the Rotation matrix. 0 = No Scale.

         ///< A description of this transform and where it came from.
         // Used for publication info.
         std::string description;

      }; //struct TransformParameters

         // The stored struct, takes the params struct and builds the matrices.
      /**
       * @brief The struct holding the prebuilt matrix, its inverse and translation.
       * 
       * This struct is created by the buildTransform method of the HelmertTransform
       * class.  To create a new Transform, pass a TransformParameters struct and
       * two ReferenceFrame objects to the defineTransform method.
       * 
       * @note No units are applied to the parameters -- must be applied by the user.
       */
      struct Transform
      {
         TransformParameters params;     ///< The Transform parameters, for reference.

         Matrix<double> rotation;        ///< The skew symmetric Rotation matrix.
         Matrix<double> inverseRotation; ///< The inverse of the Rotation matrix.
         Vector<double> translation;     ///< The translation vector applied to
                                         ///< position transforms.

      }; //struct Transform


   protected:

         // This function does the work for the other functions.
      /**
       * @brief Resolves the appropriate transform and applies it to the vector.
       * 
       * This method is eventually called by all the transform methods.
       * It first resolves which Translation to use, if it exists, then
       * applies the translation conditionally, as well as the rotation
       * (always).
       * 
       * @throw InvalidParameter If the transform does not exist.
       * 
       * @param from       The ReferenceFrame the vector is in initially.
       * @param to         The ReferenceFrame the vector is transformed to.
       * @param vec        The Vector to transform.
       * @param translate  If the method applies the translation vector.
       * 
       * @note This method changes the passed vector.
       * @return A reference to the transformed Vector.
       */
      Vector<double> helperTransform(const ReferenceFrame& from,
                                     const ReferenceFrame& to,
                                     const Vector<double>& vec,
                                     bool translate             )
         throw(InvalidParameter);

      //Initializer
      /**
       * @brief Sets up the pre-defined transforms.
       * 
       * This method is called the first time HelmertTransform::instance() is
       * called.  Currently, this method defines only the PZ90->WGS84 transform.
       */
      void populateTransformMaps();

      /**
       * @brief Builds a Transform struct from the given TransformParameters.
       * 
       * This method creates a Transform struct from the given TransformParameters
       * by applying the parameters to the matrix like this:
       *   [ ( scale + 1, -r3, r2 ),
       *     ( r3, scale + 1, -r1 ),
       *     ( -r2, r1, scale + 1 ) ]
       * 
       * @param tp The transform parameters to use to build the struct.
       * 
       * @return The built transform struct.
       */
      Transform buildTransform(const TransformParameters& tp)
         throw();

      /// The map of maps containing the to/from ReferenceFrame pairs and
      /// their associated Transform struct.
      LookupMap fromMap;

      ///------------------------------Constructors-------------------------------///

      // Constructors. Because we want this class to be a singleton,
      // these need to be private or protected.
      /**
       * @brief Default Constructor, initializes the maps.
       * 
       * This constructor calls the populateTransformMaps() method.
       */
      HelmertTransform()
         throw();

      /**.
       * @brief Protected to disallow copies.
       */
      HelmertTransform(const HelmertTransform& ht)
         throw()
      {}

      /**
       * @brief Protected to disallow copies.
       */
      HelmertTransform& operator=(const HelmertTransform& ht)
         throw();

   }; //class HelmertTransform

} //namespace gpstk

#endif
