%include "std_vector.i"

%template(std_vector_double) std::vector<double>;
%template(std_vector_int) std::vector<int>;

%pythoncode %{
    def __iter__(self):
        self.index = 0
        return self

    def next(self):
          if self.index >= self.size():
            raise StopIteration
          else:
            self.index += 1
            return self[self.index - 1]

    std_vector_double.__iter__ = __iter__
    std_vector_int.__iter__ = __iter__
    std_vector_double.next = next
    std_vector_int.next = next
%}