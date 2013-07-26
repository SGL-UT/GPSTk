// This is a SWIG macro that creates helper methods and the read/write methods:
// It creates:
//
//  C++ methods: (all of these are hidden to the end user since the streams are hidden)
//   - static factory method for the stream to create an input stream
//   - static factory method for the stream to create an output stream
//   - readHeader method for the stream
//   - readData method for the stream
//   - writeHeader method for the stream
//   - writeData method for the stream
//
//  gpstk python functions:
//   - readX, where X is the file type
//   - writeX, where X is the file type
//

%define STREAM_HELPER(FORMATNAME)
%extend gpstk:: ## FORMATNAME ## Stream {

    static gpstk:: ## FORMATNAME ## Stream* in ## FORMATNAME ## Stream(std::string fileName) {
        FORMATNAME ## Stream * s = new FORMATNAME ## Stream (fileName.c_str());
        return s;
    }

    static gpstk:: ## FORMATNAME ## Stream* out ## FORMATNAME ## Stream(std::string fileName) {
        FORMATNAME ## Stream * s = new FORMATNAME ## Stream (fileName.c_str(), std::ios::out|std::ios::trunc);
        return s;
    }

    gpstk:: ## FORMATNAME ## Header readHeader() {
        gpstk:: ##FORMATNAME ## Header head;
        (*($self)) >> head;
        return head;
    }
    gpstk:: ## FORMATNAME ## Data readData() {
        gpstk:: ## FORMATNAME ##Data data;
        if( (*($self)) >> data ) {
           return data;
        } else {
            gpstk::EndOfFile e(" FORMATNAME ## Stream reached an EOF.");
            GPSTK_THROW(e);
        }
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
    import os.path
    if not os.path.isfile(fileName):
        raise IOError(fileName + ' does not exist.')
    num_lines = _nlines(fileName)
    stream = FORMATNAME ## Stream .in ##FORMATNAME ## Stream (fileName)
    header = stream.readHeader()
    def read ## FORMATNAME ## Data (fileName):
        while True:
            try:
               yield stream.readData()
            except IOError:
               break
    if lazy:
        return (header, read ##FORMATNAME ## Data (fileName))
    else:
        return (header, [x for x in read ## FORMATNAME ## Data (fileName)])


def write ## FORMATNAME(fileName, header, data):
    """Writes a FORMATNAME Header and sequence of FORMATNAME Data objects to a file.
    Note that this overwrites the file if it already exists.

    Parameters:
    -----------

      fileName:  the name of the file to write to.

      header:  the FORMATNAME Header object

      data:  the sequence of FORMATNAME Data objects
    """
    s = FORMATNAME ## Stream .out ##FORMATNAME ## Stream (fileName)
    s.writeHeader(header)
    for d in data:
        s.writeData(d)
}
%enddef

STREAM_HELPER(SEM)
// STREAM_HELPER(SP3,<=)
STREAM_HELPER(Yuma)
STREAM_HELPER(RinexClock)
STREAM_HELPER(RinexObs)
STREAM_HELPER(RinexNav)
STREAM_HELPER(RinexMet)
STREAM_HELPER(Rinex3Obs)
STREAM_HELPER(Rinex3Nav)
STREAM_HELPER(Rinex3Clock)
STREAM_HELPER(FIC)

