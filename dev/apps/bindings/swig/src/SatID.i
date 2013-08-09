%include "../../../src/SatID.hpp"

%newobject gpstk::SatID::SatID(int p) throw();
%extend gpstk::SatID {
   SatID(int p) throw() {
      gpstk::SatID helper;
      return new gpstk::SatID(p, helper.systemGPS);
   }
};
