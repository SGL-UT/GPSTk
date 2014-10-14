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

#include <cstring>

#include <sstream>
#include <errno.h>
#include <stdlib.h>

#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/file.h>

#include <stdio.h>
#if !defined(unix) && !defined(__unix__) && !defined(__APPLE__)
#include <winsock.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif

#include <arpa/inet.h>

#include "TCPStreamBuff.hpp"

using namespace std;

namespace gpstk
{
//------------------------------------------------------------------------
   IPaddress::IPaddress(const string& host_name)
   {
      // If the address is in dotted quad notation, this will work.
      address = inet_addr(host_name.c_str());
      if ( address != (uint32_t)(-1) )
         return;
   
      struct hostent *host_ptr = ::gethostbyname(host_name.c_str());
      if( host_ptr == 0 )
      {
         cout << "Host name '" << host_name << "' cannot be resolved";
         return;
      }
      if( host_ptr->h_addrtype != AF_INET )
      {
         cout << "Host name '" << host_name
                   << "' isn't an AF_INET address" << endl;
         return;
      }

      memcpy(&address, host_ptr->h_addr, sizeof(address));
   }


//------------------------------------------------------------------------
   ostream& operator<<(ostream& os, const IPaddress addr)
   {
      struct hostent * const host_ptr = 
         ::gethostbyaddr((char *)&addr.address,
                         sizeof(addr.address), AF_INET);
      if( host_ptr != 0 )
         return os << host_ptr->h_name;

      // Reverse DNS failed, print in the dot notation
      char buffer[80];
      const unsigned int native_addr = ntohl(addr.address);
      sprintf(buffer,"%0d.%0d.%0d.%0d", (native_addr >> 24) & 0xff,
              (native_addr >> 16) & 0xff, (native_addr >> 8) & 0xff,
              native_addr & 0xff);
      return os << buffer;
   }

//------------------------------------------------------------------------
   SocketAddr::SocketAddr(const IPaddress host, const short port_no)
   {
      sin_family = AF_INET;
      sin_port = htons((short)port_no);
      sin_addr.s_addr = host.net_addr();
   }

   ostream& operator<<(ostream& os, const SocketAddr& addr)
   {
      return os << IPaddress(addr.sin_addr.s_addr) << ':'
                << (unsigned short)ntohs((short)addr.sin_port);
   }


//------------------------------------------------------------------------
   int TCPStreamBuff::connect(const SocketAddr target_address)
   {
      if (is_open())
         return 0;

      handle = socket(AF_INET,SOCK_STREAM,0);
      if (!handle)
         return -1;

      //  set_blocking_io(false);
  
      int connect_status = -1;
      for (int i=0; i<5 && connect_status != 0; i++)
      {
         connect_status = ::connect(handle, (sockaddr *)target_address, 
                                    sizeof(target_address));
      }
      if (connect_status !=0)
         return connect_status;
  
      // We do our own buffering...
      char off=0;
      ::setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, &off, sizeof(off));
      return 0;
   }


//------------------------------------------------------------------------
// Take a file handle (which is supposed to be a listening socket), accept
// a connection if any, and return a TCPStreamBuff for that connection. On exit, 
// peeraddr would be an addr of the connected peer.
   int TCPStreamBuff::accept(int listening_socket, SocketAddr& peeraddr)
   {
      // do nothing if we are already connected
      if (is_open())
         return 0;
  
      for(;;)
      {
         socklen_t target_addr_size = sizeof(peeraddr);
         handle = ::accept(listening_socket,(sockaddr *)peeraddr,
                                  &target_addr_size);
         if (handle >= 0)
            break;			// Successfully accepted the connection
         if (errno == EAGAIN || errno == EINTR)
            cout << "to be implimented" << endl;
         else
            return 0;
      }
  
      // We do our own buffering...
      char off=0;
      ::setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, &off, sizeof(off));
      return 0;
   }

} // end of namespace
