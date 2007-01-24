#pragma ident "$Id$"


#ifndef TCPSTREAM_HPP
#define TCPSTREAM_HPP

#include <stdio.h>
#if !defined(unix) && !defined(__unix__)
#include <winsock.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include <iostream>
using std::ostream;
using std::iostream;
using std::streambuf;
using std::streamoff;
using std::streampos;
using std::ios;

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
  
      friend ostream& operator << (ostream& os, const IPaddress addr);
      friend ostream& operator << (ostream& os, const SocketAddr& addr);
   };

   class SocketAddr : sockaddr_in
   {
      friend class StreamSocket;
      friend class UDPsocketIn;
      SocketAddr() {}
      
   public:
      SocketAddr(const IPaddress host, const short port_no);
      operator sockaddr * () const      { return (sockaddr *)this; }
      friend ostream& operator << (ostream& os, const SocketAddr& addr);
   };

   struct Buffer
   {
      char * start;
      char * end;
      bool do_dispose;
      void dispose()
      { 
         if(!do_dispose) return;
         free(start); start = end = 0; do_dispose = false;
      }
      Buffer(void) : start(0), end(0), do_dispose(false) {}
      ~Buffer(void) { dispose(); }
   };


   // TCPStream buffer: Its streambuf parent is supposed to handle all buffering
   // chores. We only have to allocate the buffer and provide for its filling
   // and flushing.
   class TCPbuf : public streambuf
   {
   public:
      TCPbuf() : socket_handle(-1), bufferSize(4096)
      {}

      ~TCPbuf()
      {close();};

      // These are the functions that actually read/write from the socket to/from
      // a given buffer. They return the number of bytes transmitted (or
      // EOF on error)
      int write(const char * buffer, const int n);
      int read(char * buffer, const int n);

      bool is_open() const { return socket_handle >= 0; }
      TCPbuf* close();
      virtual TCPbuf* setbuf(char* p, const int len);
  
      // TCP stream is strictly sequential
      streampos seekoff(streamoff, ios::seekdir, int)
      { return EOF; }

      // Actual connection operation
      TCPbuf* connect(const SocketAddr target_address);
  
      void dump(ostream& out) const;

      // Some TCP specific stuff
      void set_blocking_io(const bool onoff);

      // Enable/disable SIGIO upon arriving of a new packet
      void enable_sigio(const bool onoff);
  
      // Take a file handle (which is supposed to be a listening socket), 
      // accept a connection if any,  and return the corresponding TCPbuf
      // for that connection. On exit, peeraddr would be an addr of the
      // connected peer
      TCPbuf* accept(int listening_socket, SocketAddr& peeraddr);

      Buffer buffer;
      int socket_handle;
      unsigned bufferSize;
  
   protected:
      // Standard streambuf functions we have to
      // provide an implementation for
      virtual int overflow(int c = EOF);	// Write out a "put area"
      virtual int underflow(void);		// Fill in a "get area"
      virtual int sync(void);		// Commit all uncommitted writes
      virtual int doallocate(void);	// Allocate a new buffer

      // we use the same buffer for get and put areas
      char * base(void) const { return buffer.start; }
      char * ebuf(void) const { return buffer.end; }
      void setb(char * buffer_beg, char * buffer_end, bool do_dispose)
      { buffer.start = buffer_beg; buffer.end = buffer_end;
      buffer.do_dispose = do_dispose;}
   };


   class TCPStream : public iostream
   {
      mutable TCPbuf tcp_buffer;	// mutable so rdbuf() can be const
   public:
      TCPStream(void) : iostream(0)
      { init(&tcp_buffer); }

      void connect(const SocketAddr target_address)
      { clear(); if (!rdbuf()->connect(target_address)) setstate(ios::badbit); }

      TCPbuf* rdbuf(void) const
      { return &tcp_buffer; }

      bool is_open() const
      { return rdbuf()->is_open(); }

      void close(void)
      { if (!rdbuf()->close()) setstate(ios::failbit); }
   };
} // end of namespace
#endif
