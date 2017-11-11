%extend gpstk::RinexSatID
{
   std::string __str__()
   {
      return gpstk::StringUtils::asString(*$self);
   }

   std::string __repr__()
   {
      return "<" + gpstk::StringUtils::asString(*$self) +">";
   }
};

%pythoncode %{
# Define a hash to allow SatID objects to be used in sets/dicts   
def __RinexSatID_hash(self):
   return (self.id, self.system).__hash__()
   
RinexSatID.__hash__ = __RinexSatID_hash
%}
