#pragma ident "$Id$"

#ifndef TCPSTREAMBUFF_HPP
#define TCPSTREAMBUFF_HPP

#include <sstream>
#include <errno.h>
#include <stdlib.h>

#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "FDStreamBuff.hpp"

namespace gpstk
{
   class SocketAddr;

   class IPaddress
   {
      friend class SocketAddr;
      
      unsigned long address;                // Address: 4 bytes in the network byte order
      IPaddress(const unsigned int netaddr) : address(netaddr) {}

   public:
      IPaddress() : address(INADDR_ANY) {}   // Wildcard address
      IPaddress(const std::string& name);         // Involves the name resolution
      unsigned long net_addr() const { return address; }
  
      friend std::ostream& operator <<(std::ostream& os, const IPaddress addr);
      friend std::ostream& operator <<(std::ostream& os, const SocketAddr& addr);
   };

   class SocketAddr : sockaddr_in
   {
      friend class StreamSocket;
      friend class UDPsocketIn;
      SocketAddr() {}
      
   public:
      SocketAddr(const IPaddress host, const short port_no);
      operator sockaddr * () const      { return (sockaddr *)this; }
      friend std::ostream& operator <<(std::ostream& os, const SocketAddr& addr);
   };


   class TCPStreamBuff : public FDStreamBuff
   {
   public:
      TCPStreamBuff() : FDStreamBuff(-1) {}

      ~TCPStreamBuff() {close();}

      int connect(const SocketAddr target_address);

      // Take a file handle (which is supposed to be a listening socket), 
      // accept a connection if any,  and return the corresponding TCPbuf
      // for that connection. On exit, peeraddr would be an addr of the
      // connected peer
      int accept(int listening_socket, SocketAddr& peeraddr);

   };

} // end of namespace
#endif
