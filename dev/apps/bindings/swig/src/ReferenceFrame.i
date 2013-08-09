%ignore gpstk::ReferenceFrame::ReferenceFrame(int i);
%ignore gpstk::ReferenceFrame::ReferenceFrame(const char str[]);
%ignore gpstk::ReferenceFrame::setReferenceFrame(const char str[]);
%ignore gpstk::ReferenceFrame::createReferenceFrame(const char str[]);
%ignore gpstk::ReferenceFrame::asString() const;
%include "../../../src/ReferenceFrame.hpp"
