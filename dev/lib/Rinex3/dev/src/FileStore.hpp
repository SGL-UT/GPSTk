#pragma ident "$Id: FileStore.hpp 70 2006-08-01 18:36:21Z ehagen $"



/**
 * @file FileStore.hpp
 * Read and write data
 */

#ifndef GPSTK_FILE_STORE_HPP
#define GPSTK_FILE_STORE_HPP

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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






#include <vector>

#include "Exception.hpp"

namespace gpstk
{
   /** @addtogroup filedirgroup */
   //@{
      /**
       * This is an abstract class used to add a list of files and the interface
       * to read files into a datastore. This is intended to support arbitrary
       * file names, not a list of similiar file names. See the FileHunter framework
       * for support of that type of file.
       */

   template <class HeaderType> class FileStore
   {
   public:

      /// Constructor.
      FileStore() throw() {};

      /// destructor
      virtual ~FileStore() {};
      
      std::vector<std::string> getFileNames() const
      {
         typedef typename FFDataMap::const_iterator const_iterator;
         std::vector<std::string> nv;
         const_iterator i;
         for (i=headerMap.begin(); i!=headerMap.end(); i++)
         nv.push_back(i->first);
         return nv;
      }
      
      void addFile(const std::string& fn, const HeaderType& header) throw()
      { headerMap[fn] = header; };

      const HeaderType& getHeader(const std::string& fn) throw()
      { return headerMap[fn]; };

      /// Load a single file into the store
      virtual void loadFile(const std::string& fileName)
         throw (FileMissingException) = 0;

      virtual void loadFiles(const std::vector<std::string>& fileNames)
      {
         std::vector<std::string>::const_iterator f=fileNames.begin();
         for (f=fileNames.begin(); f!=fileNames.end(); f++)
            loadFile(*f);
      };

      /// Return the size of the (filename,header) map
      unsigned size() const throw() { return headerMap.size(); }

   private:

      typedef std::map<std::string, HeaderType> FFDataMap;

      /// A store of all headers loaded, indexed by file name
      FFDataMap headerMap;
   };

   //@}

}  // namespace

#endif
