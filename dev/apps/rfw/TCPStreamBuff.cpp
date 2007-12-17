#pragma ident "$Id$"

#include <sstream>
#include <errno.h>
#include <stdlib.h>

#include <fcntl.h>

#include <stdio.h>

#if !defined(unix) && !defined(__unix__) && !defined(__APPLE__)
  #include <winsock.h>
  #include <windows.h>
#else
  #include <unistd.h>
  #include <netdb.h>
  #include <sys/file.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>
  #include <arpa/inet.h>
#endif


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
