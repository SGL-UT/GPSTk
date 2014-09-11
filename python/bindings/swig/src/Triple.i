// C++ extensions:
%extend gpstk::Triple {
   double __getitem__(unsigned int i) {
      // element [] access
      return $self->theArray[i];
   }
   
   //----------------------------------------
   // Alternate implementation of indexing
   // Remove this comment block after testing
   //----------------------------------------
   //    double __getitem__(unsigned int i) {
   //       return (*($self))[i];
   //    }

   gpstk::Triple scale(double scalar) {
      // since operator* can't be wrapped
      return Triple(scalar * $self->theArray[0],
                    scalar * $self->theArray[1],
                    scalar * $self->theArray[2]);
   }
   int __len__() {
      return 3;
   }
 
   std::string __str__() {
      std::ostringstream ss;
      size_t i;
      for(i = 0; i < $self->size() - 1; i++)
         ss << (*($self))[i] << ", ";
      ss << (*($self))[i];
      return ss.str();
   }
 
   
   
   
};


// Python extensions:
%pythoncode %{
# tuple -> triple translation:
def makeTriple(tuple):
    """Generates a GPSTk-style Triple from a Python tuple."""
    # TODO: add error checking
    return Triple(tuple[0], tuple[1], tuple[2])

# triple -> tuple translation:
def makeTuple(self):
    """Given a GPSTk Triple, returns a tuple representing it."""
    return (self[0], self[1], self[2])
Triple.makeTuple = makeTuple

Triple.__str__ = lambda self: makeTuple(self).__str__()
%}
