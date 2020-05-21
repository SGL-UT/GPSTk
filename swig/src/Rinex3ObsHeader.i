
// For whatever reason, swig assumes that it owns the pointers created by the operator|= function, and
// will attempt to delete them later.  It doesn't own them, so this causes a free(): invalid pointer error.
// This disables pointer ownership for that method.
// See: https://github.com/swig/swig/blob/rel-3.0.12/Lib/python/pyopers.swg#L196
#if SWIG_VERSION < 0x030000
    %feature("del", "0") Fields::operator|=;
    %feature("new", "0") Fields::operator|=;
#else

    %feature("del", "0") gpstk::Rinex3ObsHeader::Fields::operator|=;
    %feature("new", "0") gpstk::Rinex3ObsHeader::Fields::operator|=;
#endif

#if SWIG_VERSION < 0x030000
    class Rinex3CorrInfo {
            public:
            std::string satSys;  ///< 1-char SV system (G/R/E/S)
            std::string name;    ///< program name used to apply corrections
            std::string source;  ///< source of corrections (URL)
    };

    /// RINEX 2 extra "WAVELENGTH FACT" lines
    class ExtraWaveFact_ {
            public:
            /// List of Sats with this wavelength factor
            std::vector<SatID> satList;
            /// vector of wavelength factor values
            short wavelengthFactor[2];
    };

    class Fields
    {
        public:
                Fields() = default;
        Fields(const gpstk::Rinex3ObsHeader::FieldSet& fields)
        : fieldsSet(fields)
        {}
        bool operator==(const Fields& rhs) const
        { return fieldsSet == rhs.fieldsSet; }
        bool operator!=(const Fields& rhs) const
        { return !(fieldsSet == rhs.fieldsSet); }
        Fields& set(gpstk::Rinex3ObsHeader::Field f)
        { fieldsSet.insert(f); return *this; }
        Fields& clear(gpstk::Rinex3ObsHeader::Field f)
        { fieldsSet.erase(f); return *this; }
        bool isSet(gpstk::Rinex3ObsHeader::Field f) const
        { return fieldsSet.count(f) > 0; }
        void clear()
        { fieldsSet.clear(); }
        bool empty() const
        { return fieldsSet.empty(); }
        Fields& operator|=(gpstk::Rinex3ObsHeader::Field rhs)
        { return set(rhs); }
        Fields& operator^=(gpstk::Rinex3ObsHeader::Field rhs)
        { return clear(rhs); }
        static bool isValid(double version, const Fields& present)
        { return getRequired(version).isValid(present); }

        Fields operator&(const Fields& rhs) const
        {
            gpstk::Rinex3ObsHeader::FieldSet results;
            set_intersection(fieldsSet.begin(), fieldsSet.end(),
                             rhs.fieldsSet.begin(), rhs.fieldsSet.end(),
                             inserter(results, results.begin()));
            return Fields(results);
        }
        gpstk::Rinex3ObsHeader::Field operator&(gpstk::Rinex3ObsHeader::Field rhs) const
        {
            if (fieldsSet.count(rhs))
                return rhs;
            return validInvalid;
        }
        Fields operator|(const Fields& rhs) const
        {
            gpstk::Rinex3ObsHeader::FieldSet results;
            set_union(fieldsSet.begin(), fieldsSet.end(),
                      rhs.fieldsSet.begin(), rhs.fieldsSet.end(),
                      inserter(results, results.begin()));
            return Fields(results);
        }
        bool isValid(const Fields& present) const
        {
            gpstk::Rinex3ObsHeader::FieldSet results;
            set_difference(fieldsSet.begin(), fieldsSet.end(),
                           present.fieldsSet.begin(), present.fieldsSet.end(),
                           inserter(results, results.begin()));
            return results.empty();
        }
        void describeMissing(const Fields& valid, Exception& exc)
        {
            for (const auto& f : fieldsSet)
            {
                if (!valid.isSet(f))
                {
                    exc.addText("Missing required header field: " + gpstk::Rinex3ObsHeader::asString(f));
                }
            }
        }
        static Fields getRequired(double version)
        {
            if (version < 3.00)     return gpstk::Rinex3ObsHeader::allValid2;
            else if(version < 3.01) return gpstk::Rinex3ObsHeader::allValid30;
            else if(version < 3.02) return gpstk::Rinex3ObsHeader::allValid301;
            else if(version < 3.03) return gpstk::Rinex3ObsHeader::allValid302;
            else if(version < 3.04) return gpstk::Rinex3ObsHeader::allValid303;
            else if(version < 3.05) return gpstk::Rinex3ObsHeader::allValid303;
            return Fields();
        }

        /// All the header fields set in this object.
        gpstk::Rinex3ObsHeader::FieldSet fieldsSet;
        };

        %nestedworkaround gpstk::Rinex3ObsHeader::Rinex3CorrInfo;
        %nestedworkaround gpstk::Rinex3ObsHeader::ExtraWaveFact;
        %nestedworkaround gpstk::Rinex3ObsHeader::Fields;
#else
    %feature("flatnested");
    %rename (Rinex3ObsHeader_ExtraWaveFact) gpstk::Rinex3ObsHeader::ExtraWaveFact;
#endif


%{
#include "Rinex3ObsHeader.hpp"
%}

%include "Rinex3ObsHeader.hpp"

#if SWIG_VERSION < 0x030000
    %{
        typedef gpstk::Rinex3ObsHeader::Rinex3CorrInfo Rinex3CorrInfo;
        typedef gpstk::Rinex3ObsHeader::ExtraWaveFact ExtraWaveFact_;
        typedef gpstk::Rinex3ObsHeader::Fields Fields;
    %}
#else
    %feature("flatnested", "");
#endif



// There's a possible bug in SWIG related to flatnested.
// The static const members of a class are set prior to their type being defined.
// For us, this results in them being a SwigPyObject type instead of a Fields type.
// By re-assigning them after the Fields class is defined, we appear to work around the bug.
%pythoncode %{
Rinex3ObsHeader.allValid2 = _gpstk.cvar.Rinex3ObsHeader_allValid2
Rinex3ObsHeader.allValid30 = _gpstk.cvar.Rinex3ObsHeader_allValid30
Rinex3ObsHeader.allValid301 = _gpstk.cvar.Rinex3ObsHeader_allValid301
Rinex3ObsHeader.allValid302 = _gpstk.cvar.Rinex3ObsHeader_allValid302
Rinex3ObsHeader.allValid303 = _gpstk.cvar.Rinex3ObsHeader_allValid303
%};