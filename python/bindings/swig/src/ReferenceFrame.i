%ignore gpstk::ReferenceFrame::ReferenceFrame(int i);
%rename(__str__) gpstk::ReferenceFrame::asString() const;
%include "../../../../dev/ext/lib/RefTime/ReferenceFrame.hpp"
