%define STREAM_HELPER(FORMATNAME,OP)
%extend gpstk:: ## FORMATNAME ## Stream {
    static gpstk:: ## FORMATNAME ## Stream* out ## FORMATNAME ## Stream(std::string fileName) {
        return new FORMATNAME ## Stream (fileName.c_str(), std::ios::out|std::ios::trunc);
    }

    gpstk:: ## FORMATNAME ## Header readHeader() {
        gpstk:: ##FORMATNAME ## Header head;
        (*($self)) >> head;
        return head;
    }
    gpstk:: ## FORMATNAME ## Data readData() {
        gpstk:: ## FORMATNAME ##Data data;
        (*($self)) >> data;
        return data;
    }
    void writeHeader(const gpstk:: ## FORMATNAME ## Header & head) {
        (*($self)) << head;
    }
    void writeData(const gpstk:: ## FORMATNAME ## Data & data) {
        (*($self)) << data;
    }
}

%pythoncode {
def read ## FORMATNAME(fileName, lazy=False):
    """This reads from a FORMATNAME file and returns a two-element tuple
    of the header and the sequence of data objects.

    Parameters:
    -----------

      lazy:  if the data object sequence should be lazily evaluated.
             If it is, it will be a generator, otherwise, it will be a list.
    """
    num_lines = _nlines(fileName)
    stream = FORMATNAME ## Stream (fileName)
    header = stream.readHeader()
    def read ## FORMATNAME ## Data (fileName):
        while stream.lineNumber OP num_lines:
            yield stream.readData()
    if lazy:
        return (header, read ##FORMATNAME ## Data (fileName))
    else:
        return (header, [x for x in read ## FORMATNAME ## Data (fileName)])


def write ## FORMATNAME(fileName, header, data):
    """Writes a FORMATNAME ## Header and sequence of FORMATNAME ## Data objects to a file.
    Note that this overwrites the file if it already exists.

    Parameters:
    -----------

      fileName:  the name of the file to write to.

      header:  the FORMATNAME ## Header object

      data:  the sequence of FORMATNAME ##Data objects
    """
    s = FORMATNAME ## Stream .out ##FORMATNAME ## Stream (fileName)
    s.writeHeader(header)
    for d in data:
        s.writeData(d)
}
%enddef

STREAM_HELPER(SEM,<=)
STREAM_HELPER(SP3,<=)
STREAM_HELPER(Yuma,<=)
STREAM_HELPER(Rinex3Obs,<)
