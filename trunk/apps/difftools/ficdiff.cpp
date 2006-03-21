#pragma ident "$Id $"

#include "FICFilterOperators.hpp"
#include "FileFilterFrame.hpp"

#include "DiffFrame.hpp"
#include "FICData.hpp"
#include "FICStream.hpp"

using namespace std;
using namespace gpstk;

class FICDiff : public DiffFrame
{
public:
   FICDiff(char* arg0)
         : DiffFrame(arg0, 
                      std::string("binary FIC"))
      {}

protected:
   virtual void process();
};

void FICDiff::process()
{
   try
   {
      FileFilterFrame<FICStream, FICData> ff1(inputFileOption.getValue()[0]);
      FileFilterFrame<FICStream, FICData> ff2(inputFileOption.getValue()[1]);

      ff1.sort(FICDataOperatorLessThanFull());
      ff2.sort(FICDataOperatorLessThanFull());

      pair< list<FICData>, list<FICData> > difflist = 
         ff1.diff(ff2, FICDataOperatorLessThanFull());

      if (difflist.first.empty() && difflist.second.empty())
         exit(0);

      list<FICData>::iterator itr = difflist.first.begin();
      while (itr != difflist.first.end())
      {
         (*itr).dump(cout << '<');
         itr++;
      }

      cout << endl;

      itr = difflist.second.begin();
      while (itr != difflist.second.end())
      {
         (*itr).dump(cout << '>');
         itr++;
      }

   }
   catch(Exception& e)
   {
      cout << e << endl
           << endl
           << "Terminating.." << endl;
   }
   catch(exception& e)
   {
      cout << e.what() << endl
           << endl
           << "Terminating.." << endl;
   }
   catch(...)
   {
      cout << "Unknown exception... terminating..." << endl;
   }

}


int main(int argc, char* argv[])
{
   try
   {
      FICDiff m(argv[0]);
      if (!m.initialize(argc, argv))
         return 0;
      if (!m.run())
         return 1;
      
      return 0;
   }
   catch(Exception& e)
   {
      cout << e << endl;
   }
   catch(exception& e)
   {
      cout << e.what() << endl;
   }
   catch(...)
   {
      cout << "unknown error" << endl;
   }
   return 0;
}
