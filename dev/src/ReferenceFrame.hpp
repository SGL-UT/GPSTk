#pragma ident "$Id$"

#ifndef GPSTK_REFERENCE_FRAME_HPP
#define GPSTK_REFERENCE_FRAME_HPP

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

#include <iostream>
#include <string>
#include <map>

#include "Exception.hpp"

namespace gpstk{
   
   /**
    * Coordinate Reference Frame Class
    * 
    * The ReferenceFrame class defines what is essentially a smart enum which
    * can be expanded at runtime that labels classes such as position and
    * Xvt with what coordinate reference frame is used to derive their
    * location on the earth.
    */
   
   class ReferenceFrame{
      public:
      
         ///Defines a set of known ReferenceFrame values.
      enum FramesEnum{
         Unknown = 0,   /**< Reference frame is unknown*/
         WGS84,   /**< The WGS84 Reference Frame*/
         PZ90   /**< The PZ90 Reference Frame*/
      };
      
      /**
       * Create a new ReferenceFrame with the specified FramesEnum.
       * 
       * @param e The FramesEnum of the new ReferenceFrame.
       */
         //This is needed for some reason, without it you get undefined reference errors
         //when compiling.
      ReferenceFrame(FramesEnum e);
      
      /**
       * Create a new ReferenceFrame with the specified index/FramesEnum.
       * 
       * If the specified index is greater than the size of the current names map,
       * this constructor sets itself to ReferenceFrame::Unknown.
       * 
       * @param index The FramesEnum or int index of the new ReferenceFrame.
       */
      ReferenceFrame(int index = 0);
      
      /**
       * Creates a new ReferenceFrame and looks up the given name in the names map.
       * 
       * If the the given name does not exist, the ReferenceFrame is set to
       * ReferenceFrame::Unknown.
       * 
       * @param str A c-string to look up.
       */
      ReferenceFrame(const char str[]);
      
      /**
       * Creates a new ReferenceFrame and looks up the given name in the names map.
       * 
       * If the given name does not exist, this constructor sets itself to
       * ReferenceFrame::Unknown.
       * 
       * @param str The name of the ReferenceFrame to look up in the names map.
       */
      ReferenceFrame(const std::string str);
      
      /**
       * Virtual Destructor, does nothing.
       */
      virtual ~ReferenceFrame() {   };
      
      /**
       * Sets the current ReferenceFrame to the index/FramesEnum specified.
       * 
       * If the index/FramesEnum given is outside the current map bounds, this
       * ReferenceFrame is set to ReferenceFrame::Unknown.
       * 
       * @param index The index/FramesEnum to set this ReferenceFrame to.
       */
      void setReferenceFrame(const int index);
      
      /**
       * Sets the current ReferenceFrame to the string specified.
       * 
       * If the string does not exist in the names map, this ReferenceFrame is
       * set to ReferenceFrame::Unknown.
       * 
       * @param name The c-string name of the ReferenceFrame to set this to.
       */
      void setReferenceFrame(const char name[]);
      
      /**
       * Sets the current ReferenceFrame to the string specified.
       * 
       * If the string does not exist in the names map, this ReferenceFrame is
       * set to ReferenceFrame::Unknown.
       */
      void setReferenceFrame(const std::string& name);
      
      /**
       * Gets the current value of this ReferenceFrame and returns it.
       * 
       * @return The FramesEnum associated with this ReferenceFrame.
       */
      FramesEnum getFrame() const
         throw();
      
      /**
       * Gets the name of this ReferenceFrame from the names map.
       * 
       * @return The string from the names map that corresponds to the value of this ReferenceFrame.
       */
      std::string& asString() const;
      
      /**
       * Creates a new entry in the names map.
       * 
       * This effectively extends the ReferenceFrame enum at runtime for the duration of
       * the program. If the given name already exists, this method does not add a new
       * entry and simply returns the existing value.
       * 
       * @param str The c-string name of the ReferenceFrame to create.
       * 
       * @return A reference to the ReferenceFrame after creation.
       */
      ReferenceFrame& createReferenceFrame(const char str[]);
      
      /**
       * Creates a new entry in the names map.
       * 
       * This effectively extends the ReferenceFrame enum at runtime for the duration of
       * the program. If the given name already exists, this method does not add a new
       * entry and simply returns the existing value.
       * 
       * @param name The name of the ReferenceFrame value to create.
       * 
       * @return A reference to the new ReferenceFrame after creation.
       */
      ReferenceFrame& createReferenceFrame(std::string& name);
      
      /**
       * Compares the frame value of both ReferenceFrames for equality.
       * 
       * Two ReferenceFrames are considered to be equal if the integer value of thier
       * FramesEnum is the same.
       * 
       * @param right The ReferenceFrame to compare to.
       * 
       * @return true if the right ReferenceFrame has the same FramesEnum value.
       */
      bool operator==(const ReferenceFrame& right) const
         throw();
      
      /**
       * Compares the frame value of both ReferenceFrames for inequality.
       * 
       * This is the equivalent to !( this == right )
       * 
       * @param right The ReferenceFrame to compare to.
       * 
       * @return true if the right ReferenceFrame does NOT have the same FramesEnum value.
       */
      bool operator!=(const ReferenceFrame& right) const
         throw();
      
      /**
       * Checks if this ReferenceFrame's value is greater than the right's value.
       * 
       * @param right The ReferenceFrame to compare to.
       * 
       * @return true if this ReferenceFrame's FrameEnum value is greater than the right's value.
       */
      bool operator>(const ReferenceFrame& right) const
         throw();
      
      /**
       * Checks if this ReferenceFrame's value is less than the right's value.
       * 
       * @param right The ReferenceFrame to compare to.
       * 
       * @return true if this ReferenceFrame's FrameEnum value is less than the right's value.
       */
      bool operator<(const ReferenceFrame& right) const
         throw();
      
      /**
       * Checks if this ReferenceFrame's value is greater than or equal to the right's value.
       * 
       * @param right The ReferenceFrame to compare to.
       * 
       * @return true if this ReferenceFrame's FrameEnum value is greater than or equal to the right's value.
       */
      bool operator>=(const ReferenceFrame& right) const
         throw();
      
      /**
       * Checks if this ReferenceFrame's value is less than or equal to the right's value.
       * 
       * @param right The ReferenceFrame to compare to.
       * 
       * @return true if this ReferenceFrame's FrameEnum value is less than or equal to the right's value.
       */
      bool operator<=(const ReferenceFrame& right) const
         throw();
      
      private:
      /**
       * Sets up the static names map.
       * 
       * This method adds the default map values to the names map. It should 
       * not be run directly.
       */
      static bool initialize();
      static bool initFlag;

      /**
       * The FramesEnum value of this ReferenceFrame.
       * 
       * This value represents which frame this ReferenceFrame is. It is also used
       * as the index to it's name in the names map.
       */
      FramesEnum frame;
      /**
       * The static map of ReferenceFrame names.
       * 
       * This map is used in all bounds checking such that 
       */
      static std::map<FramesEnum, std::string> names;
   };   //End of ReferenceFrame class
   
   /**
    * Outputs the name of this ReferenceFrame to the given ostream.
    * 
    * @param os The output stream to print to.
    * @param rf The ReferenceFrame to print to os.
    * 
    * @return The reference to the ostream passed to this operator.
    */
   std::ostream& operator<<(std::ostream& os, const ReferenceFrame& rf);
}   //End of gpstk namespace

#endif   //End of ReferenceFrame Header
