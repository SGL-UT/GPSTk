#pragma ident "$Id$"

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

#ifndef DEVICESTREAM_HPP
#define DEVICESTREAM_HPP

#include <string>
#include <termios.h> // POSIX terminal control definitions

#include "FDStreamBuff.hpp"
#include "TCPStreamBuff.hpp"

namespace gpstk
{
   // A class to hid the details of setting up an iostream to take data from
   // write data to one of several sources:
   //   standard input/output
   //   a file
   //   a tcp socket
   //   a serial port
   // See the open() function for how to specify these targets
   // Note that the base type needs to be a decendant of an fstream for this class to work.
   template<class T>
   class DeviceStream : public T
   {
   public:
      DeviceStream(void);

      DeviceStream(const std::string& target,
                   std::ios::openmode mode = std::ios::in);

      void open(const std::string& target,
                std::ios::openmode mode = std::ios::in);

      void open(const char* p,
                std::ios::openmode mode = std::ios::in);

      virtual ~DeviceStream();

      bool is_open() const;

      enum DeviceType {dtStdio, dtFile, dtTCP, dtSerial};

      DeviceType getDeviceType() {return deviceType;}

      std::string getTarget() const {return target;}

   private:
      mutable FDStreamBuff *fdbuff; // mutable so rdbuf() can be const
      std::string target;
      DeviceType deviceType;
   };


   template<class T>
   DeviceStream<T>::DeviceStream(void)
      : fdbuff(NULL)
   {
      open("");
   }


   template<class T>
   DeviceStream<T>::DeviceStream(const std::string& target, std::ios::openmode mode)
      : fdbuff(NULL), 
        target(target)
   {
      open(target, mode);
   };


   template<class T>
   DeviceStream<T>::~DeviceStream()
   { 
      if (fdbuff)
         delete fdbuff;
   }


   template<class T>
   void DeviceStream<T>::open(const char* p, std::ios::openmode mode)
   {
      open(std::string(p), mode);
   }

   template<class T>
   bool DeviceStream<T>::is_open() const
   {
      if (deviceType == dtStdio)
         return true;
      if (fdbuff != NULL)
         return fdbuff->is_open();
   }

   template<class T>
   void DeviceStream<T>::open(const std::string& target, std::ios::openmode mode)
   {
      using namespace std;
      this->target = target;
      if (target != "")
      {
         if (target.substr(0, 4) == "tcp:")
         {
            string ifn=target;
            int port = 25;
            ifn.erase(0,4);
            string::size_type i = ifn.find(":");
            if (i<ifn.size())
            {
               port = StringUtils::asInt(ifn.substr(i+1));
               ifn.erase(i);
            }

            TCPStreamBuff *tcpbuff = new TCPStreamBuff();

            SocketAddr client(ifn, port);
            if (tcpbuff->connect(client))
            {
               cerr << "Could not connect to " << ifn << endl;
               exit(-1);
            }   

            deviceType = dtTCP;
            fdbuff = tcpbuff;
            basic_ios<char>::rdbuf(fdbuff);
         }
         else if (target.substr(0, 4) == "ser:") // A serial port
         {
            string ifn=target;
            ifn.erase(0,4);

            int fd = ::open(ifn.c_str(), O_RDWR | O_NOCTTY);
            if (fd<0)
            {
               cout << "Error opening " << ifn.c_str() << endl;
               return;
            }

	    // Not sure why this was being done it really should have been
	    // This is just another way to force blocking I/O
            int rc;
            rc = fcntl(fd, F_SETFL, 0);
           if (rc < 0)
            {
               cout << "Error in fcntl, rc=" << rc << endl;
               return;
            }

            struct termios options;
            rc=tcgetattr(fd, &options);

            options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR
                                 | IGNCR | ICRNL | IXON);
            options.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHONL | ISIG | IEXTEN);
            options.c_oflag &= ~OPOST;
            options.c_cflag &= ~(CSIZE | PARENB);
            options.c_cflag |= CS8 | CREAD | HUPCL | CLOCAL;

            options.c_cc[VTIME] = 0; // Wait forever
            options.c_cc[VMIN] = 16; // And always get at least 16 characters
            if (rc==cfsetospeed(&options, B115200))
               cout << "Error in cfsetospeed(), rc=" << rc << endl;
            if (rc==cfsetispeed(&options, B115200))
               cout << "Error in cfsetispeed(), rc=" << rc << endl;

            // Final step... apply them
            if (rc==tcsetattr(fd, TCSANOW, &options))
               cout << "Error in tcsetattr(), rc=" << rc << endl;

            deviceType = dtSerial;
            fdbuff = new FDStreamBuff(fd);
            basic_ios<char>::rdbuf(fdbuff);
         }
         else // a regular file
         {
            int flags=O_RDONLY;
            if (mode & ios::out)
               flags=O_WRONLY|O_CREAT;
            if (mode & ios::app) flags |= O_APPEND;
            if (mode & ios::trunc) flags |= O_TRUNC;

            int fd = ::open(target.c_str(), flags, 0666);
            if (fd<0)
            {
               cerr << "Could not open: " << target.c_str() << endl;
               return;
            }
            fdbuff = new FDStreamBuff(fd);
            basic_ios<char>::rdbuf(fdbuff);

            deviceType = dtFile;
         }
      }
      else  // Use standard input/output
      {
         if (mode && ios::out)
         {
            T::copyfmt(cout);
            T::clear(cin.rdstate());
            basic_ios<char>::rdbuf(cout.rdbuf());
            this->target = "<stdout>";
         }
         else
         {
            T::copyfmt(cin);
            T::clear(cin.rdstate());
            basic_ios<char>::rdbuf(cin.rdbuf());
            this->target = "<stdin>";
         }
         deviceType = dtStdio;
      }
   }

} // end of namespace
#endif
