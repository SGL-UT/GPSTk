#pragma ident "$Id$"


#include <iostream>
#include <string>
using std::istream;
using std::cout;
using std::endl;

#include "TCPStream.hpp"
#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"

#include <sys/errno.h>

int main(int argc, char* argv[])
{
   try
   {
      gpstk::SocketAddr client(std::string("localhost"), 4621);
      gpstk::IPaddress any;
      gpstk::SocketAddr server(any, 4621);
      int debugLevel = 0;
      gpstk::CommandOptionNoArg
         debugOption('d', "debug", "Enable debug output."),
         serverOption('s', "server", "Run as server, not client");
      std::string appDescription("Test program for the TCPStream stuff.");
      gpstk::CommandOptionParser cop(appDescription);
      cop.parseOptions(argc, argv);
      if (cop.hasErrors())
      {
         cop.dumpErrors(cout);
         cop.displayUsage(cout);
         exit(0);
      }

      debugLevel = debugOption.getCount();

      gpstk::TCPStream tcpStream;

      if (serverOption.getCount())
      {
         cout << "Running as a server on " << server << endl;
         int listening_socket = ::socket(AF_INET,SOCK_STREAM,0);
         if (listening_socket < 0)
            cout << "Couldn't create listening socket"
                 << " (" << strerror(errno) << ")" << endl, exit(-1);
         
         int value = 1;
         int rc=0;
         
         if (::setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR,
                          (char*)&value, sizeof(value)))
            cout << "Couldn't set reuse address"
                 << " (" << strerror(errno) << ")" << endl, exit(-1);
         
         if (::bind(listening_socket, (sockaddr *)server, sizeof(sockaddr)))
            cout << "Couldn't bind"
                 << " (" << strerror(errno) << ")" << endl, exit(-1);
         
         if (::listen(listening_socket, 5))
            cout << "Couldn't listen"
                 << " (" << strerror(errno) << ")" << endl, exit(-1);
         
         gpstk::TCPStream link;
         gpstk::SocketAddr peeraddr = gpstk::SocketAddr(any,1);
         link.rdbuf()->accept(listening_socket,peeraddr);
         cout << "Accepted connection from " << peeraddr << endl;
         link.close();
      }
      else
      {
         cout << "Running as client, :" << client << endl;
         
         tcpStream.connect(client);
         if( !tcpStream.good() )
            cout << "Connection failed!" << endl, exit(0);
      }
   }
   catch (...)
   {
      cout << "Caught one" << endl;
   }
}
