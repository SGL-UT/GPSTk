%ignore gpstk::TimeSystem::TimeSystem(int i) throw();
%ignore gpstk::TimeSystem::getTimeSystem();

%include "../../../src/TimeSystem.hpp"

%extend gpstk::TimeSystem {
   TimeSystem(std::string sys) throw() {
      TimeSystem* t = new TimeSystem(0);
      t->fromString(sys);
      return t;
   }
};
