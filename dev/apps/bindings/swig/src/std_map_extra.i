namespace std {
  %template(map_string_double) map<std::string, double>;
  %template(map_string_int) map<std::string, int>;
  %template(map_string_char) map<std::string, char>;

  %template(map_double_string) map<double, std::string>;
  %template(map_double_int) map<double, int>;

  %template(map_int_char) map<int, char>;
  %template(map_int_double) map<int, double>;
  %template(map_int_string) map<int, std::string>;

  %template(map_char_string) map<char, std::string>;
  %template(map_char_int) map<char, int>;

  %template(map_RinexSatID_vector_RinexDatum) std::map<gpstk::RinexSatID, std::vector<gpstk::RinexDatum> >;
  %template(map_ObsID_double) std::map< gpstk::ObsID,double >;
  %template(map_SatID_SvObsEpoch) std::map< gpstk::SatID,gpstk::SvObsEpoch >;
}

%template() std::pair<swig::SwigPtr_PyObject, swig::SwigPtr_PyObject>;
%template(cmap) std::map<swig::SwigPtr_PyObject, swig::SwigPtr_PyObject>;



%pythoncode %{
def dictToMap(dict, outtype=''):
    """Translates a python dict type to a std::map backed type.
    This uses the first element to get the type. If there are 0 elements,
    None is returned.

    This raises a TypeError if it uses a type that is not templated or if the
    dict does not use the same key,value type for all its elements.

    Note that this recopies the contents of the dict and is a linear time operation.

    You may specify the what the name (string name) of the C++ backed type is. If it is blank (the default),
    the function will attempt to discover what the appropriate type is. For example, you
    might use outType=\'map_string_int\'.
    """
    import __builtin__
    if len(dict) == 0:
        return None
    first_key = dict.keys()[0]
    t_key = first_key.__class__.__name__
    t_value = dict[first_key].__class__.__name__

    # some python names don't map to the vector wrap names perfectly, so they get fixed:
    if t_key in _namefixes:
        t_key = _namefixes[t_key]
    if t_value in _namefixes:
        t_value = _namefixes[t_value]

    if outtype == '':
        new_name = 'map_' + t_key + '_' + t_value
    else:
        new_name = outtype

    try:
        m = globals()[new_name]()  # constructs an object of class t
    except:
        if outtype == '':
            raise TypeError('There is no map wrapper for ' + t)
        else:
            raise TypeError('The type ' + outtype + ' does not exist')

    t_key = type(first_key)
    t_value = type(dict[first_key])
    for key, value in dict.iteritems():
        if type(key) != t_key or type(value) != t_value:
            raise TypeError('Type mismatch in dict: ({0}, {1}) vs. ({2}, {3})'
                            .format(t_key, t_value, type(key), type(value)))
        m[key] = value
    return m

def mapToDict(map):
    """Translates a std::map backed type (from gpstk.cpp) to a python dict.

    Note that this recopies the contents of the map and is a linear time operation.
    """
    dict = {}
    for key, value in map.iteritems():
        dict[key] = value
    return dict
%}
