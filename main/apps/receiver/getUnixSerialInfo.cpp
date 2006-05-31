#include <iostream>
#include <iomanip>

#include <fcntl.h>   /* File control definitions */
#include <termios.h>
#include <unistd.h>

using namespace std;

int main(void)
{
   struct termios options;  

   int fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
   tcgetattr(fd, &options);

   cout << "c_iflag " << hex << options.c_iflag << endl;
   cout << "c_oflag " << hex << options.c_oflag << endl;
   cout << "c_cflag " << hex << options.c_cflag << endl;
   cout << "c_cc[VMIN] " << options.c_cc[VMIN] << endl;
   cout << "c_cc[VTIME] " << options.c_cc[VTIME] << endl;

   return 0;
}
