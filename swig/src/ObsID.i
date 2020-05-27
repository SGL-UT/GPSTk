
%extend gpstk::ObsID {
   std::string __str__()
   { return gpstk::StringUtils::asString(*$self);}

   std::string __repr__()
   {return "<" + gpstk::StringUtils::asString(*$self) +">";}
};



%pythoncode %{
# Define a hash to allow ObsID objects to be used in sets/dicts   
def __ObsID_hash(self):
   return (self.type, self.code, self.band).__hash__()

ObsID.__hash__ = __ObsID_hash    
%}
