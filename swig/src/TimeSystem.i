%newobject gpstk::TimeSystem::TimeSystem(std::string sys); //Creates new object of string class called sys with ownership
%extend gpstk::TimeSystem { //Attach the following functions to the structure TimeSystem
   TimeSystem(std::string sys) {
      TimeSystem* t = new TimeSystem(0);
      t->fromString(sys);
      return t;
   }
};
