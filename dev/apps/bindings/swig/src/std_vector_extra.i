%include "std_vector.i"


// This section does not need to be duplicated by macros:
%pythoncode %{
def _vector_iter(self):
    self.index = 0
    return self

def _vector_next(self):
    if self.index >= self.size():
        raise StopIteration
    else:
        self.index += 1
        return self[self.index - 1]
%}


%define STD_VECTOR(cName, pyName)
%template( pyName ) std::vector< cName >;
%pythoncode %{
pyName ## .__iter__ = _vector_iter
pyName ## .next = _vector_next
%}
%enddef



// you should update seqToVector if you add more types here:
STD_VECTOR(double, vector_double)
STD_VECTOR(int, vector_int)
STD_VECTOR(std::string, vector_string)


%pythoncode %{
from __builtin__ import range as rng
def seqToVector(seq):
    """Translates a python sequence type to a std::vector backed type.
    This uses the first element to get the type. If there are 0 elements,
    an empty vector_string will be returned.
    You should make sure all elements are of the same type or an error will
    occur.
    """
    if len(seq) == 0:
        return vector_string()
    first = seq[0]
    if isinstance(first, basestring):  # change basestring to str when moves to python3.x
        v = vector_string(len(seq))
    if isinstance(first, int):
        v = vector_int(len(seq))
    if isinstance(first, float):
        v = vector_double(len(seq))
    for i in rng(len(seq)):
        v[i] = seq[i]
    return v

def vectorToSeq(vector):
    """Translates a std::vector backed type (from gpstk.cpp) to a python list."""
    list = [None] * vector.size()  # pre-allocates size to help efficiency
    for i in rng(vector.size()):
        list[i] = vector[i]
    return list
%}
