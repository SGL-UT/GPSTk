/** $Id: //depot/msn/prototype/brent/IGEB_Demo/SVNumXRef.hpp#1 $
*   SVNumXRef.hpp
*
*   Applied Research Laboratories, The University of Texas at Austin
*   June 2007
*
*   This class implements a cross-reference between PRN numbers and NAVSTAR IDs.
*   Since PRN numbers are re-used (historically, there's a limit of 1-32 in
*   the PRN IDs) it's necessary provide a several->one PRN ID ->NAVSTAR mapping.
*   On the other hand, the NAVSTAR->PRN ID mapping is unique.
*
*   As a result, the PRN ID->NAVSTAR cross reference supports the ability to 
*   provide a date so the cross-reference can select.
*
*   Note: Thie information is not contained anywhere in the navigation message.
*   Therefore, it needs to come from "outside the system".  The information 
*   contained in the class constructor will also need to updated with each 
*   new launch and disposal.  This is a sore point, but there appears to be
*   no easy way around it. 
*
*  MEMBER METHODS
*     SVNumXRef( ); - Constructor.  Works from a fixed table that is compiled
*                     into the code.
*     int getNAVSTAR( const int PRNID ) const; - Given a PRNID, return the 
*                     NAVSTAR number.  Assumes the relationship at the 
*                     current date/time is the relationship of interest.
*                     May throw "NoNAVSTARNumFound" error.
*     int getNAVSTAR( const int PRNID, const gpstk::DayTime dt ) const; -
*                     Given a PRNID and a date, return the NAVSTAR number
*                     related to thsat PRNID at the date provided.  May 
*                     throw "NoNAVSTARNumFound" error.
*     int getPRNID( const int NAVSTARID ) const; - GIven a NAVSTAR Number
*                     return the corresponding PRNID.
*
*  The following three members parallel the preceding three and provide a 
*  means of determining is the specified information is available prior to
*  making a "get" call.  This is useful if you want to avoid writing 
*  "try/catch" blocks. 
*     bool PRNIDavailable(  const int NAVSTARID ) const;
*     bool NAVSTARIDAvailable( const int PRNID, const gpstk::DayTime dt ) const;
*     bool NAVSTARIDAvailable( const int PRNID ) const;
*
*/
#ifndef SVNUMXREF_HPP
#define SVNUMXREF_HPP

   // Language Headers
#include <map>
#include <utility>

   // Library Headers
#include "DayTime.hpp"
#include "gps_constants.hpp"
#include "Exception.hpp"

   // Project Headers

namespace gpstk
{

class SVNumXRefNode
{
   public:
      SVNumXRefNode( const int NAVSTARNumArg,
                     const gpstk::DayTime begDT,
                     const gpstk::DayTime endDT );
      int getNAVSTARNum() const;
      gpstk::DayTime getBeginTime() const;
      gpstk::DayTime getEndTime() const;
      bool isApplicable( gpstk::DayTime dt ) const;
                  
   protected:
      int NAVSTARNum;
      gpstk::DayTime begValid;
      gpstk::DayTime endValid;
};

typedef std::multimap<int, SVNumXRefNode>::const_iterator SVNumXRefListCI;
typedef std::pair<SVNumXRefListCI,SVNumXRefListCI> SVNumXRefPair;
typedef std::map<int,int>::const_iterator NAVNumXRefCI;

class SVNumXRef
{
   public:
      NEW_EXCEPTION_CLASS( NoPRNNumberFound, gpstk::Exception);
      NEW_EXCEPTION_CLASS( NoNAVSTARNumberFound, gpstk::Exception);

      enum BlockType 
      {
	I,
	II,
	IIA,
	IIR,
	IIR_M
      };  
      SVNumXRef( );
      ~SVNumXRef() {}
      BlockType getBlockType( const int NAVSTARID ) const;
      std::string getBlockTypeString( const int NAVSTARID ) const;
      int getNAVSTAR( const int PRNID ) const;
      int getNAVSTAR( const int PRNID, const gpstk::DayTime dt ) const;
      int getPRNID( const int NAVSTARID ) const;
      bool PRNIDAvailable(  const int NAVSTARID ) const;
      bool NAVSTARIDAvailable( const int PRNID, const gpstk::DayTime dt ) const;
      bool BlockTypeAvailable( const int NAVSTARID ) const;
      bool NAVSTARIDAvailable( const int PRNID ) const;
      bool isApplicable( gpstk::DayTime dt ) const;
      
   protected:
      std::map<int,int> NtoPMap;
      std::multimap<int,SVNumXRefNode> PtoNMap;
      std::map<int,BlockType> NtoBMap;
};

inline int SVNumXRefNode::getNAVSTARNum() const { return(NAVSTARNum); }
inline gpstk::DayTime SVNumXRefNode::getBeginTime() const { return( begValid ); }
inline gpstk::DayTime SVNumXRefNode::getEndTime() const { return( endValid ); }


}
#endif
