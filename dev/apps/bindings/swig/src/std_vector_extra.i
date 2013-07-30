%template(vector_SatID) std::vector<gpstk::SatID>;
%template(vector_double) std::vector<double>;
%template(vector_int) std::vector<int>;
%template(vector_string) std::vector<std::string>;
%template(vector_RinexDatum) std::vector<gpstk::RinexDatum>;
%template(vector_RinexSatID) std::vector<gpstk::RinexSatID>;


%pythoncode %{
_namefixes = {'str': 'string', 'float' : 'double'}
def seqToVector(seq, outtype=''):
    """Translates a python iterable type to a std::vector backed type.
    This uses the first element to get the type. If there are 0 elements,
    None is returned, unless the outtype is specified.

    This raises a TypeError if it uses a type that is not templated or if the
    sequence does not use the same type for all its elements.

    Note that this recopies the contents of the sequence and is a linear time operation.

    You may specify the what the name (string name) of the C++ backed type is. If it is blank (the default),
    the function will attempt to discover what the appropriate type is. For example, you
    might use outtype=\'vector_double\'.
    """
    import __builtin__

    if outtype == '':
        if len(seq) == 0:
            return None
        t = seq[0].__class__.__name__
        if t in _namefixes:
            t = _namefixes[t]
        new_name = 'vector_' + t
    else:
        new_name = outtype
    try:
        v = globals()[new_name](len(seq))  # constructs an object of class t
    except:
        if outtype == '':
            raise TypeError('There is no vector wrapper for ' + t)
        else:
            raise TypeError('The type ' + outtype + ' does not exist')

    first_type = type(seq[0])
    for i in __builtin__.range(len(seq)):
        if type(seq[i]) != first_type:
            raise TypeError('Type mismatch in sequence: {0} vs. {1}'
                            .format(first_type, type(seq[i])))
        v[i] = seq[i]
    return v

def vectorToSeq(vector):
    """Translates a std::vector backed type (from gpstk.cpp) to a python list.

    Note that this recopies the contents of the vector and is a linear time operation.
    """
    list = [None] * vector.size()  # pre-allocates size to help efficiency
    import __builtin__
    for i in __builtin__.range(vector.size()):
        list[i] = vector[i]
    return list
%}
