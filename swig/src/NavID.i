
%extend gpstk::NavID {
   std::string __str__()
   { return gpstk::StringUtils::asString(*$self);}

   std::string __repr__()
   {return "<" + gpstk::StringUtils::asString(*$self) +">";}
};



%pythoncode %{
# Define a hash to allow NavID objects to be used in sets/dicts   
def __NavID_hash(self):
   return (self.navType).__hash__()

NavID.__hash__ = __NavID_hash    
%}
