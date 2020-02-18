#include "FFTextStream.hpp"

#include <iostream>

using namespace gpstk;
using namespace std;

// the reason for checking ffs.eof() in the try AND catch block is
// because if the user enabled exceptions on the stream with exceptions()
// then eof could throw an exception, in which case we need to catch it
// and rethrow an EOF or FFStream exception.  In any event, EndOfFile
// gets thrown whenever there's an EOF and expectEOF is true
void FFTextStream::formattedGetLine( std::string& line,
                                    const bool expectEOF )
     throw(EndOfFile, FFStreamError, gpstk::StringUtils::StringException)
{
  try
  {
      // The following constant used to be 256, but with the change to
      // RINEX3 formats the possible length of a line increased
      // considerably. A RINEX3 observation file line for Galileo may
      // be 1277 characters long (taking into account all the possible
      // types of observations available, plus the end of line
      // characters), so this constant was conservatively set to
      // 1500 characters. Dagoberto Salazar.
     const int MAX_LINE_LENGTH = 1500;
     char templine[MAX_LINE_LENGTH + 1];
     getline(templine, MAX_LINE_LENGTH);
     lineNumber++;
     //check if line was longer than 256 characters, if so error
     if(fail() && !eof())
     {
        FFStreamError err("Line too long");
        GPSTK_THROW(err);
     }
     line = templine;
     gpstk::StringUtils::stripTrailing(line, '\r');
        // catch EOF when stream exceptions are disabled
     if ((gcount() == 0) && eof())
     {
        if (expectEOF)
        {
           EndOfFile err("EOF encountered");
           GPSTK_THROW(err);
        }
        else
        {
           FFStreamError err("Unexpected EOF encountered");
           GPSTK_THROW(err);
        }
     }
  }
  catch(std::exception &e)
  {
     // catch EOF when exceptions are enabled
     if ((gcount() == 0) && eof())
     {
        if (expectEOF)
        {
           EndOfFile err("EOF encountered");
           GPSTK_THROW(err);
        }
        else
        {
           FFStreamError err("Unexpected EOF");
           GPSTK_THROW(err);
        }
     }
     else
     {
        FFStreamError err("Critical file error: " +
                          std::string(e.what()));
        GPSTK_THROW(err);
     }  // End of 'if ( (gcount() == 0) && eof())'

  }  // End of 'try-catch' block

}  // End of method 'FFTextStream::formattedGetLine()'

void FFTextStream::tryFFStreamGet(FFData& rec)
  throw(FFStreamError, gpstk::StringUtils::StringException) {

  unsigned int initialLineNumber = lineNumber;

  try {
    FFStream::tryFFStreamGet(rec);
  } catch (gpstk::Exception& e) {
    e.addText(std::string("Near file line ") +
        gpstk::StringUtils::asString(lineNumber));
    lineNumber = initialLineNumber;
    mostRecentException = e;
    conditionalThrow();
  }

}

void FFTextStream::tryFFStreamPut(const FFData& rec)
  throw(FFStreamError, gpstk::StringUtils::StringException,std::bad_cast) {

  unsigned int initialLineNumber = lineNumber;

  try {
    FFStream::tryFFStreamPut(rec);
  } catch (gpstk::Exception& e) {
    e.addText(std::string("Near file line ") +
        gpstk::StringUtils::asString(lineNumber));
    lineNumber = initialLineNumber;
    mostRecentException = e;
    conditionalThrow();
  }

}

