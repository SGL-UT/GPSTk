#ifndef RINEXPVT_HPP
#define RINEXPVT_HPP

#include <string>
#include <cstring>
#include <vector>

#include "icd_200_constants.hpp"
#include "RinexSatID.hpp"

#include "RinexObsBase.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsData.hpp"
#include "RinexObsStream.hpp"

#include "RinexNavBase.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"
#include "BCEphemerisStore.hpp"

#include "RinexMetBase.hpp"
#include "RinexMetData.hpp"
#include "RinexMetHeader.hpp"
#include "RinexMetStream.hpp"
#include "TropModel.hpp"

#include "SP3EphemerisStore.hpp"

#include "PRSolution.hpp"

#include "IonoModel.hpp"

#include "BasicFramework.hpp"

using namespace std;
using namespace gpstk;

class RINEXPVTSolution : public BasicFramework
{
public:

   // Linear relation between iono delay on L1 and L2. 
   static const double gamma;

   // Maximum sane value of the ionosphere delay
   static const double maxIonoDelay;


   RINEXPVTSolution(char *arg0);
  
   virtual bool initialize(int argc, char *argv[]) throw();
   
  
protected:
   virtual void process();
   

   EphemerisStore *virtualEphStore;
   BCEphemerisStore bcestore;
   SP3EphemerisStore sp3store;
   PRSolution prSolver;
   GGTropModel ggTropModel;

   bool gotMet;
   bool spsSolution;
   bool ppsSolution;
   bool transformENU;
   bool removeIonosphere;
   bool hasBCEstore;

   double elevationMask;
  
   Geodetic aprioriPosition, enuOrigin;
   Triple eastVector, upVector, northVector;
   
   bool aprioriPositionDefined;
   
   string obsFileName;
   string metFileName;
   string logFileName;

   string epochFormat;

   bool searchNear;
   bool useSmoother;
   bool logfileOn;

   CommandOptionWithAnyArg obsOption;
   CommandOptionWithAnyArg navOption;
   CommandOptionWithAnyArg peOption;
   CommandOptionWithAnyArg metOption;
   CommandOptionWithAnyArg timeFormatOption;
   CommandOptionWithAnyArg enuOption;
   CommandOptionWithAnyArg elevationMaskOption;
   CommandOptionWithAnyArg logfileOption;
   CommandOptionWithAnyArg rateOption;

   CommandOptionNoArg spsOption;
   CommandOptionNoArg ppsOption;
   CommandOptionNoArg ionoOption;
   CommandOptionNoArg searchNearOption;
   CommandOptionNoArg smootherOption;

   ofstream logStream;
};

#endif










