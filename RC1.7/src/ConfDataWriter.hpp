#pragma ident "$Id$"

/**
* @file ConfDataWriter.hpp
* Class to write configuration data files.
*/

#ifndef GPSTK_CONFDATA_WRITER_HPP
#define GPSTK_CONFDATA_WRITER_HPP


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
//  Wei Yan - Chinese Academy of Sciences  2009, 2010
//
//============================================================================


#include <string>
#include <map>
#include "FFTextStream.hpp"
#include "StringUtils.hpp"

using namespace std;

namespace gpstk
{

	
	   /** @addtogroup formattedfile */
      //@{

      /** This is a class to write configuration data files.
       *
       * A typical way to use this class follows:
       *
       * @code
       *
       *      // Declare a ConfDataReader object
       *   ConfDataWriter confWriter;
       *
       *      // Open and parse configuration file
       *   confWriter.open("configuration.txt");
       *   
       *   confWriter.writeVariable(cutOffElevation,
       *         10.0,
       *         "minimum allowed satellite elevation",
       *         "in degrees");
       *   
       *
       * @endcode
       *
       *
       * The configuration file follows the following format:
       *
       * - Anything after a '#' or a ';' is a comment
       * - Blank lines are ignored.
       * - No line may have more than 255 characters.
       * - Variable and section names are ALWAYS converted to uppercase.
       * - Variable and section names MUST start with a letter, and must only
       *   contain a mix of letters, numbers, dashes (-) and underscores (_).
       * - Section names must be enclosed in brackets ([]).
       * - The Variable/Value pairs are separated by either '=' or ':'.
       * - You may add comments to variables and values. Use a comma to
       *   separate such comments.
       * - If you use the same variable name within a given section, only the
       *   last value will be used. Remember that names are ALWAYS converted to
       *   uppercase.
       *
       *
       */
	class ConfDataWriter:public FFTextStream
	{
	public:
		   /** Common constructor. It will always open 'file' for read and will
		    *  configuration data in one pass.
		    *
		    * @param file    Configuration data file to read
		    *
		    */
		ConfDataWriter(const char* file)
			: FFTextStream(file, std::ios::out)
		{ writeHeader(); };


		   /** Common constructor. It will always open 'fn' for read and will
		    *  configuration data in one pass.
		    *
		    * @param file    Configuration data file to read
		    *
		    */
		ConfDataWriter(const string& file)
			: FFTextStream(file.c_str(), std::ios::out)
		{ writeHeader(); };


		   /// Method to open a configuration data file to be written.
		virtual void open(const char* fn);


		   /// Method to open a configuration data file to be written.
		virtual void open(const string& fn)
		{ open( fn.c_str() ); };


         /// Write a common header for all of the configuration data files
         /// written by this class. 
		virtual void writeHeader();
         
         /// Write a comment line start by '#'
		void writeCommentLine(const string& comment);
         
         /// Write a comment line as a separator line
         /// @param s    char of the separator line
         /// @param n    size of the separator line
		void writeSeparatorLine(const string& s = "-",
                              const int&    n = 130);

         /// Write several blank lines default write one line
		void writeBlankLine(const int& n=1);
         
         /** Write a string variable with general format
          *
          * @param var          variable name
          * @param val          variable value
          * @param varComment   variable comment 
          * @param valComment   value comment
          */
		void writeVariable(const string& var, 
                         const string& val, 
                         const string& varComment = "",
                         const string& valComment = "");
		
		   
         /** Write a integer variable with general format
          *
          * @param var          variable name
          * @param val          variable value
          * @param varComment   variable comment 
          * @param valComment   value comment
          */
      void writeVariable(const string& var,
                         const int&    val,
                         const string& varComment = "",
                         const string& valComment = "")
      { writeVariable(var,StringUtils::asString(val),varComment,valComment);};
		
         
         /** Write a double variable with general format
          *
          * @param var          variable name
          * @param val          variable value
          * @param varComment   variable comment 
          * @param valComment   value comment
          */
      void writeVariable(const string& var,
                         const double& val,
                         const string& varComment = "",
                         const string& valComment = "")
      { writeVariable(var,StringUtils::asString(val),varComment,valComment);};


		   
         /** Write a string variable list with general format
          *
          * @param var          variable name
          * @param valList      variable list values
          * @param n            size of the variable list
          * @param varComment   variable comment 
          * @param valComment   value comment
          */
		void writeVariableList(const string& var,
                             const string  valList[],
                             const int&    n,
                             const string& varComment = "",
                             const string& valComment = "");
		

         /** Write a string variable list with general format
          *
          * @param var          variable name
          * @param valList      variable list values by std::vector
          * @param varComment   variable comment 
          * @param valComment   value comment
          */
      void writeVariableList(const string&         var,
                             vector<string>        valList,
                             const string&         varComment = "",
                             const string&         valComment = "");
		

         /** Write a int variable list with general format
          *
          * @param var          variable name
          * @param valList      variable list values
          * @param n            size of the variable list
          * @param varComment   variable comment 
          * @param valComment   value comment
          */
		void writeVariableList(const string& var,
                             const int     valList[],
                             const int&    n,
                             const string& varComment = "",
                             const string& valComment = "");
		

         /** Write a double variable list with general format
          *
          * @param var          variable name
          * @param valList      variable list values
          * @param n            size of the variable list
          * @param varComment   variable comment 
          * @param valComment   value comment
          */
      void writeVariableList(const string& var,
                             const double  valList[],
                             const int&    n,
                             const string& varComment = "",
                             const string& valComment = "");
		
      
         /** Write a new section with some comment
          *
          * @param name          name of the section to be written
          * @param comment       comment of the section to be written
          */
		void writeSection(const string& name,
                        const string& comment = "");
         
         /// Write a common tailer for all of the configuration data files
         /// written by this class.
		void writeEnd();

	protected:

         /// Write a string line to the file.
		void formattedPutLine(const string& sline);


	};  // End of 'class ConfDataWriter'

       //@}

}  // End of 'namespace gpstk'



#endif  //  GPSTK_CONFDATA_WRITER_HPP