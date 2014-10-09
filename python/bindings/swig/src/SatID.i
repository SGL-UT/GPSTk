%newobject gpstk::SatID::SatID(int p); //Change the ownership of the variable p in the SatID program

%extend gpstk::SatID { //Attach following values to the structure SatID
   SatID(int p) {
      gpstk::SatID helper;
      return new gpstk::SatID(p, gpstk::SatID::systemGPS);
   }
};

%pythoncode %{
# Define a hash to allow SatID objects to be used in sets/dicts   
def __SatID_hash(self):
   return (self.id, self.system).__hash__()

SatID.__hash__ = __SatID_hash
    
%}
