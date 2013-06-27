%include "../../../src/ObsID.hpp"


%pythoncode %{
ObsID.__str__ = lambda self: asString(self)
%}
