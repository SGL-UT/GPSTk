%pythoncode %{

ObsID.__str__ = lambda self: asString(self)

Xv.__str__ = lambda self: 'x:'+ self.x.__str__() + ', v:' + self.v.__str__()
def xvt_str(self):
    output = 'x:' + str(self.x)
    output += ', v:' + str(self.v)
    output += ', clk bias:' + str(self.clkbias)
    output += ', clk drift:' + str(self.clkdrift)
    output += ', relcorr:' + str(self.relcorr)
    return output
Xvt.__str__ = xvt_str

SatID.__str__ = lambda self: asString(self)

%}

%extend gpstk::ReferenceFrame {
	std::string gpstk::ReferenceFrame::__str__() {
        int f = static_cast<int>($self->getFrame());
        if(f == 1) return "WGS84";
        if(f == 2) return "PZ90";
        else return "Unknown";
	}
}

%extend gpstk::SEMHeader {
    std::string __str__() {
        std::ostringstream stream;
        $self->dump(stream);
        return stream.str();
    }
}

