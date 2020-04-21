#ifndef FILESPECFIND_HPP
#define FILESPECFIND_HPP

#include <list>
#include <string>
#include "CommonTime.hpp"
#include "FileSpec.hpp"

class FileSpecFind_T;

namespace gpstk
{
      /** Replacement for the gpstk FileHunter class, which is slow.
       * Searches for files in a time range using FileSpec metadata.
       *
       * This differs from FileHunter in the following ways:
       *   \li FileHunter returns files matching [start, end] while FileSpecFind
       *       returns files matching [start, end).
       *   \li FileSpecFind is a utility class while FileHunter is instantiated.
       *   \li FileSpecFind works in the context of automounter.
       *   \li FileSpecFind works with relative paths.
       *   \li FileSpecFind is a great deal faster.
       *
       * @note This code will not build under windows as it uses a
       * POSIX function glob() that is not supported by visual studio,
       * as well as globbing patterns that would not work under
       * windows.  As such, it will not be integrated into the gpstk
       * or sgltk.
       *
       * Example with text spec token (%x):
       * @code{.cpp}
       *    gpstk::CommonTime fromTime(gpstk::YDSTime(2018, 211, 0));
       *    gpstk::CommonTime toTime(gpstk::YDSTime(2018, 212, 0));
       *    string site("FOO");
       *    gpstk::FileSpec::FSTStringMap textStore;
       *    textStore[gpstk::FileSpec::text] = site;
       *    std::list<std::string> fileList = gpstk::FileSpecFind::find("/data/%04Y/%05n/%03j/nsh-%x-%5n-%1r-%04Y-%03j-%02H%02M%02S.xml", fromTime, toTime, textStore);
       * @endcode
       *
       *
       * Example without text spec token:
       * @code{.cpp}
       *    gpstk::FileSpec::FSTStringMap fsts;
       *    fromTime = gpstk::YDSTime(2017,1,0);
       *    toTime = gpstk::YDSTime(2017,2,0);
       *    std::list<std::string> fileList = gpstk::FileSpecFind::find(
       *       "/archive/%04Y/%05n/%05n-%04Y%03j-%1r%1t.raw",
       *       fromTime, toTime, fsts);
       * @endcode
       */
   class FileSpecFind
   {
   public:
         /** Search for existing files matching a given file spec and
          * time range.
          * @param[in] fileSpecString The FileSpec (gpstk) that the
          *   files should match.
          * @param[in] start Files/directories that precede this time
          *   will be ignored.
          * @param[in] end Files/directories that are after this time
          *   will be ignored.
          * @param[in] fsts Any FileSpec (not time) token values you
          *   wish to specifically match.  Note that "text" tokens in
          *   particular should be specified here, if fileSpecString
          *   contains one.
          * @warning You will not get any matches for file names that
          *   contain %x if you do not specify a text value in fsts.
          * @return A list of matching file names.
          */
      static std::list<std::string> find(
         const std::string& fileSpecString,
         const gpstk::CommonTime& start,
         const gpstk::CommonTime& end,
         const gpstk::FileSpec::FSTStringMap& fsts);

   private:
         /** Translates gpstk FileSpec formatting tokens into glob expressions.
          * @param[in] token A string containing gpstk FileSpec formatting
          *   tokens e.g. %04Y.
          * @return a string with all of the known gpstk formatting tokens
          *   replaced with glob patterns that have a reasonable chance of
          *   matching names of existing files generated using the FileSpec.
          *   For example, %04Y would become [0-9][0-9][0-9][0-9] */
      static std::string transToken(const std::string& token);

         /** Recursive (into subdirectories) function for find that
          * uses glob.  Look at the .cpp file for a more detailed
          * description of how this works.
          * @param[in] start Files/directories that precede this time
          *   will be ignored.
          * @param[in] end Files/directories that are after this time
          *   will be ignored.
          * @param[in] spec The string representation of the FileSpec
          *   (gpstk) that the files should match.
          * @param[in] dummyFSTS Filler values for the file spec when
          *   creating dummy file names to get appropriate time ranges.
          * @param[in] matched A string containing already-matched
          *   paths when recursion occurs.  This string replaces
          *   spec[0] through spec[pos] when doing searches.
          * @param[in] pos Starting position (from 0) into spec to
          *   start looking for FileSpec tokens.  Used to support
          *   recursion.
          * @return A list of matching file names.
          */
      static std::list<std::string> findGlob(
         const gpstk::CommonTime& start,
         const gpstk::CommonTime& end,
         const std::string& spec,
         const gpstk::FileSpec::FSTStringMap& dummyFSTS,
         const std::string& matched = "",
         std::string::size_type pos = 0);

      friend class ::FileSpecFind_T;
   };
}

#endif // FILESPECFIND_HPP
