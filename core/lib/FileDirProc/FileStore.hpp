//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/// @file FileStore.hpp

#ifndef GPSTK_FILE_STORE_INCLUDE
#define GPSTK_FILE_STORE_INCLUDE

#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include "Exception.hpp"

namespace gpstk
{
      /// @ingroup FileDirProc
      //@{

      /**
       * This is an abstract class (loadFile is pure virtual) used to
       * add a list of files and the interface to read files into a
       * datastore.  This is intended to support arbitrary file names,
       * not a list of similiar file names. See the FileHunter
       * framework for support of that type of file.
       */
   template <class HeaderType> class FileStore
   {
   private:

         /// A store of all headers loaded, indexed by file name
      std::map<std::string, HeaderType> headerMap;

   public:

         /// Constructor.
      FileStore() throw() {};

         /// destructor
      ~FileStore() {};
      
         /// Get a list of all the file names in the store, as a vector<string>
      std::vector<std::string> getFileNames() const
      {
         std::vector<std::string> names;
         typename std::map<std::string, HeaderType>::const_iterator fit;
         for(fit=headerMap.begin(); fit != headerMap.end(); fit++)
            names.push_back(fit->first);
         return names;
      }
      
         /// Add a filename, with its header, to the store
      void addFile(const std::string& fn, HeaderType& header)
         throw(InvalidRequest)
      {
         if(headerMap.find(fn) != headerMap.end()) {
            dump(std::cout, 1);
            InvalidRequest e("Duplicate file name");
            GPSTK_THROW(e);
         }
         headerMap.insert(make_pair(fn,header));
      }

         /// Access the header for a given filename
      const HeaderType& getHeader(const std::string& fn) const
         throw(InvalidRequest)
      {
         typename std::map<std::string, HeaderType>::const_iterator iter_fn = headerMap.find(fn);
         if( iter_fn == headerMap.end())
         {
            InvalidRequest e("File name not found");
            GPSTK_THROW(e);
         }
         return iter_fn->second;
            // changed from headerMap[fn] since that is a non-const
            // member of std::map and therefore could modify
            // headerMap, making this function not be side-effect free
      }

         /// dump a list of file names
      void dump(std::ostream& os = std::cout, short detail = 0)
         const throw()
      {
         int n(0);
         os << "Dump of FileStore\n";
         typename std::map<std::string, HeaderType>::const_iterator fit;
         for(fit=headerMap.begin(); fit != headerMap.end(); fit++) {
            os << " File " << std::setw(2) << ++n << ": " << fit->first;
            if(detail >= 1) {
               os << " (header for this file follows)\n";
               (fit->second).dump(os);
            }
            else os << std::endl;
         }
         os << "End dump of FileStore\n";
      }

         /// Clear the contents of the (filename, header) map 
      void clear()
         throw()
      {
         headerMap.clear();
      }


         /// Return the size of the (filename,header) map
      unsigned size() const throw() { return headerMap.size(); }

      unsigned nfiles() const throw() { return size(); }

   }; // end class FileStore

      //@}

}  // namespace

#endif // GPSTK_FILE_STORE_INCLUDE
