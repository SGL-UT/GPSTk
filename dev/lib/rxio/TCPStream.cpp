#pragma ident * $Id$

#include <sstream>
#include <errno.h>
#include <stdlib.h>

#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/file.h>
#include <netinet/tcp.h>

#include "TCPStream.hpp"

using std::cout;
using std::endl;

#define BSIZE 4096

#include <arpa/inet.h>

namespace gpstk
{
IPaddress::IPaddress(const std::string& host_name)
{
   // If the address is in dotted quad notation, this will work.
   address = inet_addr(host_name.c_str());
   if ( address != (uint32_t)(-1) )
      return;
   
   struct hostent *host_ptr = ::gethostbyname(host_name.c_str());
   if( host_ptr == 0 )
   {
      std::cout << "Host name '" << host_name << "' cannot be resolved";
      return;
   }
   if( host_ptr->h_addrtype != AF_INET )
   {
      std::cout << "Host name '" << host_name
                << "' isn't an AF_INET address" << endl;
      return;
   }

   memcpy(&address, host_ptr->h_addr, sizeof(address));
}


ostream& operator << (ostream& os, const IPaddress addr)
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

ostream& operator << (ostream& os, const SocketAddr& addr)
{
   return os << IPaddress(addr.sin_addr.s_addr) << ':'
             << (unsigned short)ntohs((short)addr.sin_port);
}


//------------------------------------------------------------------------
TCPbuf* TCPbuf::connect(const SocketAddr target_address)
{
   if (is_open())
      return 0;

   socket_handle = socket(AF_INET,SOCK_STREAM,0);
   if (!socket_handle)
      return 0;

   //  set_blocking_io(false);
  
   int connect_status = -1;
   for (int i=0; i<5 && connect_status != 0; i++)
   {
      connect_status = ::connect(socket_handle, (sockaddr *)target_address, 
                                sizeof(target_address));
      if (false)
         cout << "connect_status " << connect_status
              << "(" << strerror(connect_status)
              << ")" << endl;
      
      //    if( errno == EINPROGRESS || errno == EINTR )
      //      callbacks.yield();
   }
   if (connect_status !=0)
      return (TCPbuf*)0;
  
   set_blocking_io(true);
  
   // We do our own buffering...
   char off=0;
   ::setsockopt(socket_handle, IPPROTO_TCP, TCP_NODELAY, &off, sizeof(off));
   return this;
}


// Take a file handle (which is supposed to be a listening socket), accept
// a connection if any, and return a TCPbuf for that connection. On exit, 
// peeraddr would be an addr of the connected peer.
TCPbuf* TCPbuf::accept(int listening_socket, SocketAddr& peeraddr)
{
   // do nothing if we are already connected
   if (is_open())
      return 0;
  
   for(;;)
   {
      socklen_t target_addr_size = sizeof(peeraddr);
      socket_handle = ::accept(listening_socket,(sockaddr *)peeraddr,
                               &target_addr_size);
      if (socket_handle >= 0)
         break;			// Successfully accepted the connection
      if (errno == EAGAIN || errno == EINTR)
         std::cout << "to be implimented" << std::endl;
      else
         return this;
   }
  
   set_blocking_io(true);
  
   // We do our own buffering...
   char off=0;
   ::setsockopt(socket_handle, IPPROTO_TCP, TCP_NODELAY, &off, sizeof(off));
   return this;
}


// Close the socket
TCPbuf* TCPbuf::close(void)
{
   if (is_open())
   {
      ::close(socket_handle);
      socket_handle = -1;
   }
   return  this;
}

// All that has really been tested is blocking. Non-blocking I/O will need
// more code to make it functional.
void TCPbuf::set_blocking_io(const bool onoff)
{
   int rc;
   rc = ::fcntl(socket_handle,F_GETFL,0);
   rc = ::fcntl(socket_handle,F_SETFL,onoff ? rc & ~O_NONBLOCK :
                rc | O_NONBLOCK);
}

// Write characters to the stream, giving time. Return the number of
// characters actually written (which is always n, or EOF in case of error).
int TCPbuf::write(const char * buffer, const int n)
{
   if( !is_open() )
      return EOF;
   if( n == 0 )
      return 0;
  
   const char * const buffer_end = buffer + n;
   while( buffer < buffer_end )
   {
      const int char_written = ::write(socket_handle, buffer, buffer_end-buffer);
      if( char_written > 0 )
         buffer += char_written;
      else
         if (char_written < 0 && !(errno == EAGAIN || errno == EINTR ))
            return EOF;
         else
            std::cout << "to be implimented" << endl;
   }
   return n;
}

// Read characters from the stream into a given buffer (of given size n)
// If there is nothing to read, yield and keep trying. Return the number
// of characters actually read, or 0 (in the case of EOF) or EOF on error.
int TCPbuf::read(char * buffer, const int n)
{
   if( !is_open() )
      return EOF;
   if( n == 0 )
      return 0;
   
   for(;;)
   {
      const int char_read = ::read(socket_handle, buffer, n);
      if( char_read >= 0 )
         return char_read;
      if( errno == EAGAIN || errno == EINTR )
         std::cout << "to be implimented" << std::endl;
      else
         return EOF;
   }
}

// See libg++-2.7.2/libio/iostream.info-1 for some nice words on how this
// stuff should work.
// Flush (write out) the put area, resetting pptr if the write was successful
// Return 0, or EOF on error
int TCPbuf::sync(void)
{
   const int n = pptr() - pbase();
   if( n == 0 )
      return 0;
   return write(pbase(), n) == n ? (pbump(-n), 0) : EOF;
}


// Write out the buffer into the communication channel After that, put a
// character c (unless it's EOF) Return 0, or EOF on error. This method
// allocates a buffer if there wasn't any, and switches it to the "put mode"
// (discarding all data that may have been in the buffer)
int TCPbuf::overflow(int ch)
{
   if (sync() == EOF)
      return EOF;

   if (base() == 0)			// If there wasn't any buffer,
      doallocate();			// ... make one
   
   setg(base(),base(),base());		// Make the get area completely empty
   setp(base(),ebuf());			// Give all the buffer to the put area

   if (pptr() == 0)
      cout << "Throw an exception here or something bad." << endl;

   if (ch != EOF)
      *pptr() = ch, pbump(1);

   return 0;
}


// Fill in the get area, and return its first character. Actually read into
// the main buffer from the beginning (syncing pending output if was any)
// and set the get area to that part of the buffer. Also set pptr() = eptr()
// so that the first write would call overflow() (which would discard the
// read data)
int TCPbuf::underflow(void)
{
   if( gptr() < egptr() )
      return *(unsigned char*)gptr();

   if( sync() == EOF )	// commit all pending output first
      return EOF;	// libg++ uses switch_to_get_mode() in here

   if( base() == 0 )	// If there wasn't any buffer, make one first
      doallocate();
  
   //assert( base() );
   base();
   const int count = read(base(),ebuf() - base());
   setg(base(),base(),base() + (count <= 0 ? 0 : count));
   setp(base(),base());		// no put area - do overflow on the first put
   return count <= 0 ? EOF : *(unsigned char*)gptr();
}

// Allocate a new buffer
int TCPbuf::doallocate(void)
{
   const int size = BSIZE;
   // have to do malloc() as ~streambuf() does free() on the buffer
   char *p = (char *)malloc(size);

   if (p == NULL)
      cout << "throw an exception here?" << endl;

   setb(p, p+size, true);
   return 1;
}

// Associate a user buffer with the TCPbuf
TCPbuf* TCPbuf::setbuf(char* p, const int len)
{
  if( streambuf::setbuf(p,len) == 0 )
    return 0;

  setp(base(),base());
  setg(base(),base(),base());
  return this;
}

void TCPbuf::dump(ostream& out) const
{
   std::ostringstream ost;
   ost << "TCPbuf: " 
       << " H:" << socket_handle << endl
       << " put: " << std::hex << pbase() << " - " << epptr()
       << " curr:" << pptr() - pbase() << endl
       << " get:"  << eback() << " - " << egptr()
       << " curr:" << gptr() - eback() << endl
       << " buff:" << base() << " - " << ebuf()
       << endl;
   //printf("\nSend buffer size %d\n",get_sock_opt(SO_SNDBUF,int()));
   out << ost.str();
}

} // end of namespace
