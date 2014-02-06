%ignore gpstk::TimeSystem::TimeSystem(int i);
%ignore gpstk::TimeSystem::getTimeSystem();

%include "../../../lib/RefTime/TimeSystem.hpp"

%newobject gpstk::TimeSystem::TimeSystem(std::string sys);
%extend gpstk::TimeSystem {
   TimeSystem(std::string sys) {
      TimeSystem* t = new TimeSystem(0);
      t->fromString(sys);
      return t;
   }
};
