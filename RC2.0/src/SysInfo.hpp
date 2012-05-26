#pragma ident "$Id$"

/**
 * @file SysInfo.hpp
 * 
 */

#ifndef GPSTK_SYSINFO_HPP
#define GPSTK_SYSINFO_HPP

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
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================

#include <string>

namespace gpstk
{
      /** This class ...
       *
       */
   class SysInfo   
   {
   public:
      static std::string osName();

      static std::string osArchitecture();
      

   protected:

      SysInfo(){}      
      virtual ~SysInfo(){}
         
   };   // End of class 'SysInfo'
   
}   // End of namespace gpstk


#endif  //GPSTK_SYSINFO_HPP

