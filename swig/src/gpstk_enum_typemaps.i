

// Create Mappings from C Enum to Python Enum
// Without this they will be transformed to integers.
%define ENUM_MAPPER(C_NAME, P_NAME)
%typemap(out) C_NAME {
    // C_NAME typemap for $symname out
    // printf("Received an out enumeration: %d $symname\n", $1);

    // Note: We need to DECREF the gpstk_mod only.
    // The sys_mod_dict is borrowed, and enum_instance is returned.
    PyObject* sys_mod_dict = PyImport_GetModuleDict();
    PyObject* gpstk_mod = PyMapping_GetItemString(sys_mod_dict, "gpstk");
    PyObject* enum_instance = PyObject_CallMethod(gpstk_mod, "P_NAME", "i", (int)$1);
    Py_DECREF(gpstk_mod);
    $result = enum_instance;
    return $result;
}
%enddef

ENUM_MAPPER(gpstk::SatelliteSystem, SatelliteSystem)
ENUM_MAPPER(gpstk::CarrierBand, CarrierBand)
ENUM_MAPPER(gpstk::TrackingCode, TrackingCode)
ENUM_MAPPER(gpstk::ObservationType, ObservationType)
ENUM_MAPPER(gpstk::NavType, NavType)
ENUM_MAPPER(gpstk::TimeSystem, TimeSystem)
ENUM_MAPPER(gpstk::ReferenceFrame, ReferenceFrame)