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
