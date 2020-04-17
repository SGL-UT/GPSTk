#include "BasicFramework.hpp"
#include "SatMetaDataStore.hpp"

using namespace std;
using namespace gpstk;

class SatMetaDataStoreCheck : public BasicFramework
{
public:
   SatMetaDataStoreCheck(const string& applName);

   bool initialize(int argc, char* argv[], bool pretty=true) throw() override;

   void shutDown() override;

      /// command option for specifying the location of the satellite metadata.
   CommandOptionWithAnyArg satMetaOpt;
      /// Storage for PRN<->SVN translation.
   SatMetaDataStore satMetaDataStore;
};


SatMetaDataStoreCheck ::
SatMetaDataStoreCheck(const string& applName)
      : BasicFramework(applName, "Perform basic sanity checks on a"
                       " SatMetaDataStore CSV file"),
        satMetaOpt('M', "svconfig", "File containing satellite configuration"
                   " information for mapping SVN<->PRN", true)
{
}


bool SatMetaDataStoreCheck ::
initialize(int argc, char* argv[], bool pretty) throw()
{
   if (!BasicFramework::initialize(argc, argv))
      return false;

   for (unsigned i = 0; i < satMetaOpt.getCount(); i++)
   {
      if (!satMetaDataStore.loadData(satMetaOpt.getValue()[i]))
      {
         cerr << "Failed to load \"" << satMetaOpt.getValue()[i]
              << "\"" << endl;
         exitCode = 2;
         return false;
      }
   }
   return true;
}


void SatMetaDataStoreCheck ::
shutDown()
{
   if (exitCode == 0)
      cout << "Success" << endl;
}

int main(int argc, char *argv[])
{
   try
   {
      SatMetaDataStoreCheck app(argv[0]);
      if (!app.initialize(argc, argv))
         return app.exitCode;
      app.run();
      return app.exitCode;
   }
   catch(Exception& e)
   {
      cout << e << endl;
   }
   catch(std::exception& e)
   {
      cout << e.what() << endl;
   }
   catch(...)
   {
      cout << "unknown error" << endl;
   }
      // only reach this point if an exception was caught
   return BasicFramework::EXCEPTION_ERROR;
}
