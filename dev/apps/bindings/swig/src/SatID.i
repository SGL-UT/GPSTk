%include "../../../src/SatID.hpp"

%newobject gpstk::SatID::SatID(int p);
%extend gpstk::SatID {
   SatID(int p) {
      gpstk::SatID helper;
      return new gpstk::SatID(p, gpstk::SatID::systemGPS);
   }
};
