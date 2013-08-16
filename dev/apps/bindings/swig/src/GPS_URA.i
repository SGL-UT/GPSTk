// Wrappers on the GPS_URA arrays that SWIG can't use.
// Also wraps a simlar array from GNSSconstants.hpp
// (it puts them in PyObject double* wrappers, which are useless in Python)
// For each array:
//  - add an ignore
//  - add the function
//  - add the function to the override dict key for constants in gpstk_builder.py


%ignore gpstk::SV_ACCURACY_GPS_MIN_INDEX;
%ignore gpstk::SV_ACCURACY_GPS_NOMINAL_INDEX;
%ignore gpstk::SV_ACCURACY_GPS_MAX_INDEX;
%ignore gpstk::SV_CNAV_ACCURACY_GPS_MIN_INDEX;
%ignore gpstk::SV_CNAV_ACCURACY_GPS_NOM_INDEX;
%ignore gpstk::SV_CNAV_ACCURACY_GPS_MAX_INDEX;

%include "../../../src/GPS_URA.hpp"

%define CHECK_BOUNDS(MAX)
   if (index > MAX)
   {
      std::string message = "Index " + std::to_string(index) + " is greater than the max allowed index of " + std::to_string(MAX) + ".";
      gpstk::IndexOutOfBoundsException e(message);
      GPSTK_THROW(e);
   }
%enddef


%inline {
   double sv_accuracy_gps_min_index(int index)
   {
      CHECK_BOUNDS(gpstk::SV_ACCURACY_GPS_MAX_INDEX_VALUE)
      return gpstk::SV_ACCURACY_GPS_MIN_INDEX[index];
   }

   double sv_accuracy_gps_nominal_index(int index)
   {
      CHECK_BOUNDS(gpstk::SV_ACCURACY_GPS_MAX_INDEX_VALUE)
      return gpstk::SV_ACCURACY_GPS_MIN_INDEX[index];
   }

   double sv_accuracy_gps_max_index(int index)
   {
      CHECK_BOUNDS(gpstk::SV_ACCURACY_GPS_MAX_INDEX_VALUE)
      return gpstk::SV_ACCURACY_GPS_MIN_INDEX[index];
   }

   double sv_cnav_accuracy_gps_min_index(int index)
   {
      CHECK_BOUNDS(gpstk::SV_CNAV_NOMINAL_MAX_INDEX)
      return gpstk::SV_ACCURACY_GPS_MIN_INDEX[index];
   }

   double sv_cnav_accuracy_gps_nom_index(int index)
   {
      CHECK_BOUNDS(gpstk::SV_CNAV_NOMINAL_MAX_INDEX)
      return gpstk::SV_ACCURACY_GPS_MIN_INDEX[index];
   }

   double sv_cnav_accuracy_gps_max_index(int index)
   {
      CHECK_BOUNDS(gpstk::SV_CNAV_NOMINAL_MAX_INDEX)
      return gpstk::SV_ACCURACY_GPS_MIN_INDEX[index];
   }

   double sv_accuracy_glo_index(int index)
   {
      CHECK_BOUNDS(gpstk::SV_ACCURACY_GLO_INDEX_MAX)
      return gpstk::SV_ACCURACY_GLO_INDEX[index];
   }

}
