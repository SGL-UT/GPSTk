%newobject gpstk::SatID::SatID(int p); //Change the ownership of the variable p in the SatID program

%extend gpstk::SatID {
   SatID(int p)
   { return new gpstk::SatID(p, gpstk::SatID::systemGPS); }

   std::string __str__()
   { return gpstk::StringUtils::asString(*$self);}

   std::string __repr__()
   {return "<" + gpstk::StringUtils::asString(*$self) +">";}
};

%rename(__str__) gpstk::StringUitls::asString();

%pythoncode %{
# Define a hash to allow SatID objects to be used in sets/dicts   
def __SatID_hash(self):
   return (self.id, self.system).__hash__()

SatID.__hash__ = __SatID_hash
%}
