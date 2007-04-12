#pragma ident "$Id: TCPStream.hpp 370 2007-01-24 22:01:49Z ocibu $"

#ifndef TCPSTREAM_HPP
#define TCPSTREAM_HPP

#include <iostream>

#include "TCPStreamBuff.hpp"

namespace gpstk
{
   class TCPStream : public std::iostream
   {
      mutable TCPStreamBuff tcp_buffer; // mutable so rdbuf() can be const

   public:
      TCPStream(void)
         : iostream(0)
      {
         init(&tcp_buffer);
      }

      void connect(const SocketAddr target_address)
      { 
         clear(); 
         if (!rdbuf()->connect(target_address)) 
            setstate(std::ios::badbit); 
      }

      TCPStreamBuffer* rdbuf(void) const
      {
         return &tcp_buffer;
      }

      bool is_open() const
      {
         return rdbuf()->is_open();
      }

      void close(void)
      {
         if (!rdbuf()->close())
            setstate(ios::failbit);
      }
   };
} // end of namespace
#endif
