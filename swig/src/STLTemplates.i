
//////////////////////////////////////////////////
//            std::vector wraps
//////////////////////////////////////////////////
%template(vector_SatID)      std::vector<gpstk::SatID>;
%template(vector_double)     std::vector<double>;
%template(vector_int)        std::vector<int>;
%template(vector_string)     std::vector<std::string>;
%template(vector_RinexDatum) std::vector<gpstk::RinexDatum>;
%template(vector_RinexSatID) std::vector<gpstk::RinexSatID>;
%template(vector_RinexObsID) std::vector<gpstk::RinexObsID>;
%template(vector_RinexObsType) std::vector<gpstk::RinexObsType>;
%template(vector_Rinex3ObsData) std::vector<gpstk::Rinex3ObsData>;


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
%template(map_ObservationType_string) std::map<enum gpstk::ObsID::ObservationType, std::string>;
%template(map_CarrierBand_string) std::map<enum gpstk::ObsID::CarrierBand, std::string>;
%template(map_TrackingCode_string) std::map<enum gpstk::ObsID::TrackingCode, std::string>;
%template(map_char_ObservationType) std::map<char, enum gpstk::ObsID::ObservationType>;
%template(map_char_CarrierBand) std::map<char, enum gpstk::ObsID::CarrierBand>;
%template(map_char_TrackingCode) std::map<char, enum gpstk::ObsID::TrackingCode>;
%template(map_ObservationType_char) std::map<enum gpstk::ObsID::ObservationType,char>;
%template(map_CarrierBand_char) std::map<enum gpstk::ObsID::CarrierBand,char>;
%template(map_TrackingCode_char) std::map<enum gpstk::ObsID::TrackingCode,char>;

// ValidType wraps
%template(validtype_float) gpstk::ValidType<float>;
%template(validtype_double) gpstk::ValidType<double>;
%template(validtype_char) gpstk::ValidType<char>;
%template(validtype_short) gpstk::ValidType<short>;
%template(validtype_int) gpstk::ValidType<int>;
%template(validtype_long) gpstk::ValidType<long>;
%template(validtype_uchar) gpstk::ValidType<unsigned char>;
%template(validtype_ushort) gpstk::ValidType<unsigned short>;
%template(validtype_uint) gpstk::ValidType<unsigned int>;
%template(validtype_ulong) gpstk::ValidType<unsigned long>;

//////////////////////////////////////////////////
//             std::set wraps
//////////////////////////////////////////////////
%template(set_int) std::set<int>;
%template(set_double) std::set<double>;
%template(set_string) std::set<std::string>;
