%define STREAM_HELPER(FORMATNAME,STREAMNAME,HEADERNAME,DATANAME)
%extend gpstk:: ##STREAMNAME {
    static gpstk:: ##STREAMNAME * out ##STREAMNAME(std::string fileName) {
        return new STREAMNAME (fileName.c_str(), std::ios::out|std::ios::trunc);
    }

    gpstk:: ##HEADERNAME readHeader() {
        gpstk:: ##HEADERNAME head;
        (*($self)) >> head;
        return head;
    }
    gpstk:: ##DATANAME readData() {
        gpstk:: ##DATANAME data;
        (*($self)) >> data;
        return data;
    }
    void writeHeader(const gpstk:: ##HEADERNAME & head) {
        (*($self)) << head;
    }
    void writeData(const gpstk:: ##DATANAME & data) {
        (*($self)) << data;
    }
}

%pythoncode {
def read ##FORMATNAME(fileName, lazy=False):
    """This reads from a FORMATNAME file and returns a two-element tuple
    of the header and the sequence of data objects.

    Parameters:
    -----------

      lazy:  if the data object sequence should be lazily evaluated.
             If it is, it will be a generator, otherwise, it will be a list.
    """
    num_lines = nlines(fileName)
    stream = STREAMNAME (fileName)
    header = stream.readHeader()
    def read ##DATANAME (fileName):
        while True:
            if stream.lineNumber <= num_lines:
                yield stream.readData()
            else:
                break
    if lazy:
        return (header, read ##DATANAME (fileName))
    else:
        return (header, [x for x in read ##DATANAME (fileName)])


def write ##FORMATNAME(fileName, header, data):
    """Writes a HEADERNAME and sequence of ##DATANAME objects to a file.
    Note that this overwrites the file if it already exists.

    Parameters:
    -----------

      fileName:  the name of the file to write to.

      header:  the ##HEADERNAME object

      data:  the sequence of ##DATANAME objects
    """
    s = STREAMNAME .out ##STREAMNAME (fileName)
    s.writeHeader(header)
    for d in data:
        s.writeData(d)
}
%enddef


STREAM_HELPER(SEM,SEMStream,SEMHeader,SEMData)
STREAM_HELPER(SP3,SP3Stream,SP3Header,SP3Data)
STREAM_HELPER(Yuma,YumaStream,YumaHeader,YumaData)
