#pragma ident "$Id: //depot/msn/main/code/shared/gpstk/SinexData.cpp#5 $"

//lgpl-license START
//lgpl-license END

//dod-release-statement START
//dod-release-statement END

/**
 * @file SinexData.cpp
 * Encapsulate SINEX file data, including I/O
 */

#include "StringUtils.hpp"
#include "SinexStream.hpp"
#include "SinexData.hpp"
#include "SinexTypes.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
namespace Sinex
{

      /// Mappings of block titles to create functions
   BlockFactory  Data::blockFactory;


   void
   Data::initBlockFactory()
   {
      if (blockFactory.size() > 0) return;  /// Already initialized ?

      blockFactory["FILE/REFERENCE"]         = Block<FileReference>::create;
      blockFactory["FILE/COMMENT"]           = Block<FileComment>::create;
      blockFactory["INPUT/HISTORY"]          = Block<InputHistory>::create;
      blockFactory["INPUT/FILES"]            = Block<InputFile>::create;
      blockFactory["INPUT/ACKNOWLEDGMENTS"]  = Block<InputAck>::create;
      blockFactory["INPUT/ACKNOWLEDGEMENTS"] = Block<InputAck>::create;
      blockFactory["NUTATION/DATA"]          = Block<NutationData>::create;
      blockFactory["PRECESSION/DATA"]        = Block<PrecessionData>::create;
      blockFactory["SOURCE/ID"]              = Block<SourceId>::create;
      blockFactory["SITE/ID"]                = Block<SiteId>::create;
      blockFactory["SITE/DATA"]              = Block<SiteData>::create;
      blockFactory["SITE/RECEIVER"]          = Block<SiteReceiver>::create;
      blockFactory["SITE/ANTENNA"]           = Block<SiteAntenna>::create;
      blockFactory["SITE/GPS_PHASE_CENTER"]  = Block<SiteGpsPhaseCenter>::create;
      blockFactory["SITE/GAL_PHASE_CENTER"]  = Block<SiteGalPhaseCenter>::create;
      blockFactory["SITE/ECCENTRICITY"]      = Block<SiteEccentricity>::create;
      blockFactory["SATELLITE/ID"]           = Block<SatelliteId>::create;
      blockFactory["SATELLITE/PHASE_CENTER"] = Block<SatellitePhaseCenter>::create;
      blockFactory["BIAS/EPOCHS"]            = Block<BiasEpoch>::create;
      blockFactory["SOLUTION/EPOCHS"]        = Block<SolutionEpoch>::create;
      blockFactory["SOLUTION/STATISTICS"]    = Block<SolutionStatistics>::create;
      blockFactory["SOLUTION/ESTIMATE"]      = Block<SolutionEstimate>::create;
      blockFactory["SOLUTION/APRIORI"]       = Block<SolutionApriori>::create;
      blockFactory["SOLUTION/MATRIX_ESTIMATE L CORR"]   = Block<SolutionMatrixEstimateLCorr>::create;
      blockFactory["SOLUTION/MATRIX_ESTIMATE L COVA"]   = Block<SolutionMatrixEstimateLCova>::create;
      blockFactory["SOLUTION/MATRIX_ESTIMATE L INFO"]   = Block<SolutionMatrixEstimateLInfo>::create;
      blockFactory["SOLUTION/MATRIX_ESTIMATE U CORR"]   = Block<SolutionMatrixEstimateUCorr>::create;
      blockFactory["SOLUTION/MATRIX_ESTIMATE U COVA"]   = Block<SolutionMatrixEstimateUCova>::create;
      blockFactory["SOLUTION/MATRIX_ESTIMATE U INFO"]   = Block<SolutionMatrixEstimateUInfo>::create;
      blockFactory["SOLUTION/MATRIX_APRIORI L CORR"]    = Block<SolutionMatrixAprioriLCorr>::create;
      blockFactory["SOLUTION/MATRIX_APRIORI L COVA"]    = Block<SolutionMatrixAprioriLCova>::create;
      blockFactory["SOLUTION/MATRIX_APRIORI L INFO"]    = Block<SolutionMatrixAprioriLInfo>::create;
      blockFactory["SOLUTION/MATRIX_APRIORI U CORR"]    = Block<SolutionMatrixAprioriUCorr>::create;
      blockFactory["SOLUTION/MATRIX_APRIORI U COVA"]    = Block<SolutionMatrixAprioriUCova>::create;
      blockFactory["SOLUTION/MATRIX_APRIORI U INFO"]    = Block<SolutionMatrixAprioriUInfo>::create;
      blockFactory["SOLUTION/NORMAL_EQUATION_VECTOR"]   = Block<SolutionNormalEquationVector>::create;
      blockFactory["SOLUTION/NORMAL_EQUATION_MATRIX L"] = Block<SolutionNormalEquationMatrixL>::create;
      blockFactory["SOLUTION/NORMAL_EQUATION_MATRIX U"] = Block<SolutionNormalEquationMatrixU>::create;

   }  // Data::initBlockFactory()


   Data::~Data()
   {
      BlockIter  i = blocks.begin();
      for ( ; i != blocks.end(); ++i)
      {
         delete (*i);
         *i = NULL;
      }
   } // Data::~Data()


   void
   Data::reallyPutRecord(FFStream& s) const
      throw(std::exception, FFStreamError, StringUtils::StringException)
   {
      Sinex::Stream& strm = dynamic_cast<Sinex::Stream&>(s);
      try
      {
         strm << (std::string)header << endl;
      }
      catch (Exception& exc)
      {
         FFStreamError  err;
         GPSTK_THROW(err);
      }
      Blocks::const_iterator  i = blocks.begin();
      for ( ; i != blocks.end(); ++i)
      {
         const BlockBase  *block = *i;
         if (block)
         {
            /// @TODO - Put block comment
            try
            {
               strm << BLOCK_START << block->getTitle() << endl;
               block->putBlock(strm);
               strm << BLOCK_END << block->getTitle() << endl;
            }
            catch (Exception& exc)
            {
               FFStreamError  err(exc);
               GPSTK_THROW(err);
            }
         }
      }
      strm << FILE_END << endl;

   }  // Data::reallyPutRecord()


   void
   Data::reallyGetRecord(FFStream& s)
      throw(std::exception, FFStreamError, StringUtils::StringException)
   {
      Sinex::Stream&  strm = dynamic_cast<Sinex::Stream&>(s);
      bool    terminated = false;
      string  currentBlock;
      string  line;
      blocks.clear();
      strm.formattedGetLine(line, true);  /// EOF possible
      try
      {
         header = line;
      }
      catch (Exception& exc)
      {
         FFStreamError  err(exc);
         GPSTK_THROW(err);
      }
      while (strm.good() )
      {
         try
         {
            strm.formattedGetLine(line, true);  /// EOF possible
         }
         catch (EndOfFile&  e)
         {
            break;
         }
         if (line.size() < 1)
         {
            FFStreamError err("Invalid empty line.");
            GPSTK_THROW(err);
         }
         switch (line[0])
         {
            case BLOCK_START:
            {
               if (currentBlock.size() > 0)
               {
                  FFStreamError err("Unexpected start of block.");
                  GPSTK_THROW(err);
               }
               currentBlock = line.substr(1);
               BlockFactory::iterator i = blockFactory.find(currentBlock);
               if (i == blockFactory.end() )
               {
                  string  errMsg("Invalid block title: ");
                  FFStreamError err(errMsg + currentBlock);
                  GPSTK_THROW(err);
               }
               BlockCreateFunc  createFunc = i->second;
               BlockBase  *block = createFunc();
               if (block)
               {
                  try
                  {
                     block->getBlock(strm);
                     blocks.push_back(block);
                  }
                  catch (Exception& exc)
                  {
                     FFStreamError  err(exc);
                     GPSTK_THROW(err);
                  }
               }
               else
               {
                  string  errMsg("Error creating block: ");
                  FFStreamError err(errMsg + currentBlock);
                  GPSTK_THROW(err);
               }
               break;
            }
            case BLOCK_END:
            {
               if (currentBlock.size() == 0)
               {
                  FFStreamError err("Unexpected end of block.");
                  GPSTK_THROW(err);
               }
               if (currentBlock.compare(line.substr(1) ) != 0)
               {
                  FFStreamError err("Block start and end do not match.");
                  GPSTK_THROW(err);
               }
               currentBlock.clear();
               break;
            }
            case DATA_START:
            {
               FFStreamError err("Missing start of block.");
               GPSTK_THROW(err);
               break;
            }
            case HEAD_TAIL_START:
            {
               if (line.compare("%ENDSNX") == 0)
               {
                  terminated = true;
               }
               else
               {
                  string  errMsg("Invalid line: ");
                  FFStreamError err(errMsg + line);
                  GPSTK_THROW(err);
               }
               break;
            }
            case COMMENT_START:
            {
               /// @TODO - Store
               break;
            }
            default:
            {
               string  errMsg("Invalid line start character: ");
               FFStreamError err(errMsg + line[0]);
               GPSTK_THROW(err);
               break;
            }

         }  // switch
      }
      if (currentBlock.size() > 0)
      {
         string  errMsg("Block not properly terminated: ");
         FFStreamError err(errMsg + currentBlock);
         GPSTK_THROW(err);
      }
      if (!terminated)
      {
         FFStreamError err("File not properly terminated (missing "
                           + FILE_END + " )");
         GPSTK_THROW(err);
      }
   }  // Data::reallyGetRecord()


   bool Data::isValidBlockTitle(const std::string& title)
   {
      initBlockFactory();
      BlockFactory::iterator i = blockFactory.find(title);
      return (i == blockFactory.end() ) ? false: true;

   }  // Data::isValidBlockTitle()


   void
   Data::dump(std::ostream& s) const
   {
      Blocks::const_iterator  i = blocks.begin();
      for ( ; i != blocks.end(); ++i)
      {
         const BlockBase  *block = *i;
         if (block)
         {
            s << setw(6) << block->getSize() << " "
              << block->getTitle() << endl;
         }
      }
   }  // Data::dump()

}  // namespace Sinex

} // namespace gpstk
