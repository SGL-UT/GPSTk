#pragma ident "$Id$"

#include <iostream>
#include <list>

#include "BasicFramework.hpp"
#include "CommandOption.hpp"
#include "StringUtils.hpp"
#include "icd_200_constants.hpp"

#include "CACodeGenerator.hpp"
#include "PCodeGenerator.hpp"
#include "P0CodeGenerator.hpp"

using namespace gpstk;
using namespace std;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CodeDump : public BasicFramework
{
public:
   CodeDump() throw();

   bool initialize(int argc, char *argv[]) throw();

private:
   virtual void process();

   long long chips;
   int truncate;

   CodeGenerator *codeGen;
};


//-----------------------------------------------------------------------------
CodeDump::CodeDump() throw() :
   BasicFramework("codeDump", "A program to output the chips from a the code generators."),
   chips(1023), truncate(0)
{}


bool CodeDump::initialize(int argc, char *argv[]) throw()
{
   using namespace gpstk::StringUtils;

   CommandOptionWithAnyArg
      lengthOpt('l', "length",
                "Specifies how many chips to output. The default "
                "is 1023 chips."),

      truncateOpt('t', "truncate",
                "Just output this many chips from the beginning and the end of the sequence. The default is to output all chips."),

      codeOpt('c', "code",
              "The code to generate. ARG takes the form of "
              "code:prn:offset. Code is either c or p. "
              "Prn is an integer between 1 and 32. "
              "Offset is a number in chips. For example, to generate P code "
              "for PRN 3, with no initial time offset, "
              "specify -c p:3:0");
   
   if (!BasicFramework::initialize(argc,argv)) 
      return false;

   if (!codeOpt.getCount())
   {
      cout << "Must specify a code to generate. Bye." << endl;
      return false;
   }

   string val=codeOpt.getValue()[0];
   const char delim(':');
   if (numWords(val, delim) != 3)
   {
      cout << "Error in code parameter:" << val << endl;
      return false;
   }

   string code =   lowerCase(word(val, 0, delim));
   int     prn =       asInt(word(val, 1, delim));
   long offset =      asInt(word(val, 2, delim));

   switch (code[0])
   {
      case 'c': codeGen = new CACodeGenerator(prn); break;
      case 'p': codeGen = new PCodeGenerator(prn); break;
      case 'q': codeGen = new P0CodeGenerator(prn); break;
      default:
         cout << "Unsupported code: " << code << endl;
         return false;
   }

   if (offset != 0)
      codeGen->setIndex(offset);

   if (lengthOpt.getCount())
      chips = asInt(lengthOpt.getValue()[0]);

   if (truncateOpt.getCount())
      truncate = asInt(truncateOpt.getValue()[0]);

   return true;
}


//-----------------------------------------------------------------------------
void CodeDump::process()
{
   for (long long i = 0; i<chips; i++)
   {
      bool bit = **codeGen;
      ++(*codeGen);
      if (!truncate || i<truncate || i>chips-1-truncate)
         cout << bit << flush;
      if (truncate>0 && i==truncate)
         cout << " ... ";
   }
   cout << endl;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   try
   {
      CodeDump crap;
      if (!crap.initialize(argc, argv))
         exit(0);
      crap.run();
   }
   catch (gpstk::Exception &exc)
   { cerr << exc << endl; }
   catch (std::exception &exc)
   { cerr << "Caught std::exception " << exc.what() << endl; }
   catch (...)
   { cerr << "Caught unknown exception" << endl; }
}
