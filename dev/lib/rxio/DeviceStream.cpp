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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

#include <termios.h> /* POSIX terminal control definitions */

#include "StringUtils.hpp"
#include "FDStreamBuff.hpp"
#include "TCPStreamBuff.hpp"

#include "DeviceStream.hpp"

using namespace std;

namespace gpstk
{
   DeviceStream::DeviceStream(void)
      : fdbuff(NULL)
   {
      open("");
   };

   DeviceStream::DeviceStream(const string& target, ios::openmode mode)
      : fdbuff(NULL), 
        target(target)
   {
      open(target, mode);
   };


   DeviceStream::~DeviceStream()
   { 
      if (fdbuff)
         delete fdbuff;
   }


   void DeviceStream::open(const char* p, ios::openmode mode)
   {
      open(string(p), mode);
   }


   void DeviceStream::open(const string& target, ios::openmode mode)
   {
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
            int fd = ::open(ifn.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
            
            if (fd<0)
               return;
            
            int rtn = fcntl(fd, F_SETFL, 0);
            
            struct termios options;  
            
            options.c_iflag = 0x00 | IGNBRK;// | IGNPAR; // 0x1;
            options.c_lflag = 0x00;
            options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
            options.c_oflag = 0x00;
            options.c_cflag =  0x00 | CS8 | CSIZE | CREAD | HUPCL | CLOCAL; // 0x1cb2;
            cfsetispeed(&options, B115200);
            
            // Final step... apply them
            tcsetattr(fd, TCSANOW, &options);

            deviceType = dtSerial;
            fdbuff = new FDStreamBuff(fd);
            basic_ios<char>::rdbuf(fdbuff);
         }
         else // a regular file
         {
            int fd = ::open(target.c_str(), mode);
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
         if (mode && ios::in)
         {
            copyfmt(cin);
            clear(cin.rdstate());
            basic_ios<char>::rdbuf(cin.rdbuf());
            this->target = "<stdin>";
         }
         else
         {
            copyfmt(cout);
            clear(cin.rdstate());
            basic_ios<char>::rdbuf(cout.rdbuf());
            this->target = "<stdout>";
         }
         deviceType = dtStdio;
      }
   }


}
