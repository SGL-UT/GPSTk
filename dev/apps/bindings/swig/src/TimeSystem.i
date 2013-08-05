%ignore gpstk::TimeSystem::TimeSystem(int i) throw();
%ignore gpstk::TimeSystem::getTimeSystem();

%include "../../../src/TimeSystem.hpp"

%newobject gpstk::TimeSystem::TimeSystem(std::string sys) throw();
%extend gpstk::TimeSystem {
   TimeSystem(std::string sys) throw() {
      TimeSystem* t = new TimeSystem(0);
      t->fromString(sys);
      return t;
   }
};
