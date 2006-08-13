#pragma ident "$Id:$"

/**
 * @file ObsArray.hpp
 * Provides ability to operate mathematically on large, logical groups of observations
 * Class declarations.
 */

#ifndef OBSARRAY_HPP
#define OBSARRAY_HPP

#include<map>
#include<string>
#include<valarray>

#include "DayTime.hpp"
#include "RinexEphemerisStore.hpp"
#include "Exception.hpp" 
#include "Expression.hpp"
#include "RinexObsHeader.hpp"

namespace gpstk
{
   typedef int ObsIndex;
   
   /** @defgroup ObsGroup Storage and manipulation of general observations. */
   //@{
 
      /**
       * This class provides the ability to quickly access and manipulate
       * logical groups of observations. Observations can be any basic
       * type, e.g., "P1", or a function of types, e.g., "P1-C1". The
       * observations can be accessed by pass, by PRN, by time, or via
       * as user-defined mask. 
       *
       * Access to observations is provided via std::valarray& .
       * Therefore, no memory allocation is necessary, and indexing
       * can be performed by the user using standard mechanisms: operator[],
       * slice_array, gslice_array, mask_array and indirect_array. Valarray
       * which was designed "specifically for speed of the usual numeric
       * operations." That quote is from p. 662 of Stroustrup's book
       * "The C++ Programming Language," 3rd edition. That book explains
       * valarray and how to use it. Such material explains how ObsArray
       * works internally and how advanced use of ObsArray is accomplished.
       */  

   class ObsArray
   {
   public:

      NEW_EXCEPTION_CLASS(ObsArrayException, gpstk::Exception);

         /**
          * Constructor.
          */
      ObsArray(void);

         /**
          * This function notifies the object to track a particular RINEX
          * data type. This function must be called before loading
          * observations from file.
          */
      ObsIndex add(RinexObsHeader::RinexObsType type);

         /**
          * This function notifies the object to track functions of
          * RINEX data types, e.g., "P1-C1". This function must be called
          * before loading observations from a file.
          */
      ObsIndex add(const std::string& expression);

      ObsIndex getIndexCount(void) const 
         { return indexCount; }

      size_t getNumObsRows(void) const
         { return numObsRows; } 

         /** 
          * This functions loads a RINEX obs and nav file. Both files
          * should be from the same period.
          */ 
      void load(const std::string& obsfilename, 
                const std::string& navfilename);
      void load(const char* obsfilename,
                const char* navfilename) 
      {   
         load(std::string(obsfilename),std::string(navfilename));
      }


      double& operator() (size_t r, size_t c)
      {  return data[r*indexCount + c]; }

      std::valarray<double>   data;
      std::valarray<DayTime>  time;
      std::valarray<RinexPrn> prn;
      std::valarray<double>   azimuth;
      std::valarray<double>   elevation;
      std::valarray<bool>     validAzEl;

   private:

      ObsIndex indexCount;
      std::map<ObsIndex, RinexObsHeader::RinexObsType > basicTypeMap;
      std::map<ObsIndex, bool> isBasic;
      std::map<ObsIndex, Expression > expressionMap;      

      size_t numObsRows;

      RinexEphemerisStore ephStore;

      struct PassInfo 
      {
         RinexPrn prn;
         DayTime firstEpoch, lastEpoch;
         long passNumber;
      };
      long passNumber;
      std::map<long, PassInfo*> passByNumber;
      
   }; // End class ObsArray
   
      //@}   
} // End namespace gpstk

#endif // OBSARRAY_HPP  
