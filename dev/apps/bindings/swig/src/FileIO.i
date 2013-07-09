%extend gpstk::SEMStream {
    static gpstk::SEMStream* outSEMStream(std::string fileName) {
        return new SEMStream(fileName.c_str(), std::ios::out|std::ios::trunc);
    }

    gpstk::SEMHeader readHeader() {
        gpstk::SEMHeader head;
        (*($self)) >> head;
        return head;
    }
    gpstk::SEMData readData() {
        gpstk::SEMData data;
        (*($self)) >> data;
        return data;
    }
    void writeHeader(const gpstk::SEMHeader& head) {
        (*($self)) << head;
    }
    void writeData(const gpstk::SEMData& data) {
        (*($self)) << data;
    }
}

%pythoncode %{
def readSEMFile(fileName, lazy=False):
    """This reads from a SEM file and returns a two-element tuple
    of the header and the sequence of data objects.

    Parameters:
    -----------

      lazy:  if the data object sequence should be lazily evaluated.
             If it is, it will be a generator, otherwise, it will be a list.
    """
    num_lines = nlines(fileName)
    stream = SEMStream(fileName)
    header = stream.readHeader()
    def readSEMData(fileName):
        while True:
            if stream.lineNumber < num_lines:
                yield stream.readData()
            else:
                break
    if lazy:
        return (header, readSEMData(fileName))
    else:
        return (header, [x for x in readSEMData(fileName)])


def writeSEMFile(fileName, header, data):
    """Writes a SEMHeader and sequence of SEMData objects to a file.
    Note that this overwrites the file if it already exists.

    Parameters:
    -----------

      fileName:  the name of the file to write to.

      header:  the SEMHeader object

      data:  the sequence of SEMData objects
    """
    s = SEMStream.outSEMStream(fileName)
    s.writeHeader(header)
    for d in data:
        s.writeData(d)
%}
