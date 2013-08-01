%include "../../../src/SatID.hpp"

%extend gpstk::SatID {
   SatID(int p) throw() {
      gpstk::SatID helper;
      return new gpstk::SatID(p, helper.systemGPS);
   }
};
