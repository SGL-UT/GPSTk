//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

#ifndef TCPSTREAM_HPP
#define TCPSTREAM_HPP

#include <fstream>

#include "TCPStreamBuff.hpp"

namespace gpstk
{
   class TCPStream : public std::fstream
   {
      mutable TCPStreamBuff tcp_buffer; // mutable so rdbuf() can be const

   public:
      TCPStream(void)
      {
         init(&tcp_buffer);
      }

      void connect(const SocketAddr target_address)
      { 
         clear(); 
         if (!rdbuf()->connect(target_address)) 
            setstate(std::ios::badbit); 
      }

      TCPStreamBuff* rdbuf(void) const
      {
         return &tcp_buffer;
      }

      bool is_open() const
      {
         return rdbuf()->is_open();
      }

      void close(void)
      {
         rdbuf()->close();
      }
   };
} // end of namespace
#endif
