%module RinexObsStream

%{
/* Includes the header in the wrapper code */
%include "../../src/RinexObsStream.hpp"
%}

class RinexObsStream : public FFTextStream
{
   public:

      RinexObsStream(): headerRead(false);

      RinexObsStream(const char* fn, std::ios::openmode mode=std::ios::in)
            : FFTextStream(fn, mode), headerRead(false);

      virtual ~RinexObsStream() {}
      
      virtual void open(const char* fn, std::ios::openmode mode);

      bool headerRead;

      RinexObsHeader header;
   };










