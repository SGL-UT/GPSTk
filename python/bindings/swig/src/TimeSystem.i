%ignore gpstk::TimeSystem::TimeSystem(int i); //Ignore the declaration i in TimeSystem
%ignore gpstk::TimeSystem::getTimeSystem(); //Ignore the declaration getTimeSystem in TimeSystem

%include "../../../../dev/ext/lib/RefTime/TimeSystem.hpp" //Get the header file TimeSystem

%newobject gpstk::TimeSystem::TimeSystem(std::string sys); //Creates new object of string class called sys with ownership
%extend gpstk::TimeSystem { //Attach the following functions to the structure TimeSystem
   TimeSystem(std::string sys) {
      TimeSystem* t = new TimeSystem(0);
      t->fromString(sys);
      return t;
   }
};
