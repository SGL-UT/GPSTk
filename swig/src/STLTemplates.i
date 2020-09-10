%{
#include "SatID.hpp"
#include "SatelliteSystem.hpp"
%}

%include "SatID.hpp"
%include "SatelliteSystem.hpp"

//////////////////////////////////////////////////
//            std::vector wraps
//////////////////////////////////////////////////
%template(vector_GNSS)       std::vector<gpstk::SatelliteSystem>;
%template(vector_SatID)      std::vector<gpstk::SatID>;
%template(vector_ObsID)      std::vector<gpstk::ObsID>;
%template(vector_double)     std::vector<double>;
%template(vector_int)        std::vector<int>;
%template(vector_string)     std::vector<std::string>;
%template(vector_RinexDatum) std::vector<gpstk::RinexDatum>;
%template(vector_RinexSatID) std::vector<gpstk::RinexSatID>;
%template(vector_RinexObsID) std::vector<gpstk::RinexObsID>;
%template(vector_RinexObsType) std::vector<gpstk::RinexObsType>;
%template(vector_Rinex3ObsData) std::vector<gpstk::Rinex3ObsData>;

%pythoncode %{
    import sys
    if sys.version_info[0] < 3:
        from collections import Iterable
    else:
        from collections.abc import Iterable
    enum_vec_classes = [vector_GNSS]
    for cls in enum_vec_classes:
        orig_constructor = cls.__init__
        def new_constructor(self, *args):
            # We assume that the argument is not exhaustible
            if len(args) == 1 and isinstance(args[0], Iterable) and all(isinstance(x, int) for x in args[0]):
                orig_constructor(self)
                for x in args[0]:
                    self.append(x)
            else:
                orig_constructor(self, *args)
        cls.__init__ = new_constructor
%}

//////////////////////////////////////////////////
//             std::map wraps
//////////////////////////////////////////////////
%template()     std::pair<swig::SwigPtr_PyObject, swig::SwigPtr_PyObject>;
%template(cmap) std::map<swig::SwigPtr_PyObject, swig::SwigPtr_PyObject>;

%template(map_string_double) std::map<std::string, double>;
%template(map_string_int)    std::map<std::string, int>;
%template(map_string_char)   std::map<std::string, char>;

%template(map_double_string) std::map<double, std::string>;
%template(map_double_int)    std::map<double, int>;

%template(map_int_char)   std::map<int, char>;
%template(map_int_double) std::map<int, double>;
%template(map_int_string) std::map<int, std::string>;

%template(map_char_string) std::map<char, std::string>;
%template(map_char_int)    std::map<char, int>;

// AKA RinexObsData::RinexObsTypeMap
%template(map_RinexObsType_RinexDatum) std::map<gpstk::RinexObsType, gpstk::RinexDatum>;

// AKA RinexObsData::RinexSatMap
%template(map_RinexSatID_RinexObsTypeMap) std::map<gpstk::SatID, std::map<gpstk::RinexObsType, gpstk::RinexDatum> >;

// the type of Rinex3ObsHeader::mapObsTypes
%template(map_string_vector_RinexObsID) std::map<std::string, std::vector<gpstk::RinexObsID> >;

%template(map_RinexSatID_vector_RinexDatum) std::map<gpstk::RinexSatID, std::vector<gpstk::RinexDatum> >;

// These need to be instantiated so the SvObsEpoch and ObsEpoch classes will work correctly
%template(map_SvObsEpoch) std::map< gpstk::ObsID, double >;
%template(map_ObsEpoch) std::map<gpstk::SatID, gpstk::SvObsEpoch>;

// AKA ObsEpochMap
%template(ObsEpochMap) std::map<gpstk::CommonTime, gpstk::ObsEpoch>;

// These are needed to properly interpret an ObsID instance
%template(map_ObservationType_string) std::map<enum gpstk::ObservationType, std::string>;
%template(map_CarrierBand_string) std::map<enum gpstk::CarrierBand, std::string>;
%template(map_TrackingCode_string) std::map<enum gpstk::TrackingCode, std::string>;
%template(map_char_ObservationType) std::map<char, enum gpstk::ObservationType>;
%template(map_char_CarrierBand) std::map<char, enum gpstk::CarrierBand>;
%template(map_char_TrackingCode) std::map<char, enum gpstk::TrackingCode>;
%template(map_ObservationType_char) std::map<enum gpstk::ObservationType,char>;
%template(map_CarrierBand_char) std::map<enum gpstk::CarrierBand,char>;
%template(map_TrackingCode_char) std::map<enum gpstk::TrackingCode,char>;

//////////////////////////////////////////////////
//             std::set wraps
//////////////////////////////////////////////////
%template(set_int) std::set<int>;
%template(set_double) std::set<double>;
%template(set_string) std::set<std::string>;
