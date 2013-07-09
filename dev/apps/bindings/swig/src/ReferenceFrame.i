#pragma ident "$Id$"

#ifndef GPSTK_REFERENCE_FRAME_HPP
#define GPSTK_REFERENCE_FRAME_HPP

#include <iostream>
#include <string>
#include <map>

#include "Exception.hpp"

namespace gpstk{
   class ReferenceFrame {
      public:

      enum FramesEnum {
         Unknown = 0,
         WGS84,
         PZ90
      };
      // ReferenceFrame(FramesEnum e);
      ReferenceFrame(int index = 0);
      // ReferenceFrame(const char str[]);
      ReferenceFrame(const std::string str);
      virtual ~ReferenceFrame() {   };
      void setReferenceFrame(const int index);
      // void setReferenceFrame(const char name[]);
      void setReferenceFrame(const std::string& name);
      // FramesEnum getFrame() const
      //    throw();
      // std::string& asString() const;
      // ReferenceFrame& createReferenceFrame(const char str[]);
      ReferenceFrame& createReferenceFrame(std::string& name);
      bool operator==(const ReferenceFrame& right) const
         throw();
      bool operator!=(const ReferenceFrame& right) const
         throw();
      bool operator>(const ReferenceFrame& right) const
         throw();
      bool operator<(const ReferenceFrame& right) const
         throw();
      bool operator>=(const ReferenceFrame& right) const
         throw();
      bool operator<=(const ReferenceFrame& right) const
         throw();
   };
   // std::ostream& operator<<(std::ostream& os, const ReferenceFrame& rf);

#endif   //End of ReferenceFrame Header
}
