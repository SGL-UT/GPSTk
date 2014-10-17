// Wrappers on the GPS_URA arrays that SWIG can't use.
// Also wraps a similar array from GNSSconstants.hpp
// (it puts them in PyObject double* wrappers, which are useless in Python)
// For each array:
//  - add an ignore
//  - add the function
//  - add the function to the override dict key for constants in gpstk_builder.py

%define CHECK_BOUNDS(MAX)
   if (index > MAX)
   {
      std::ostringstream ss;
      ss << "Index " << index << " is greater than the max allowed index of " << MAX << ".";
      std::string message = ss.str();
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
