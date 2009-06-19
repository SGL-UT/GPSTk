#pragma ident "$Id: ReferenceFrame.hpp 2009-06-19 10:50:45 tvarney $"

#ifndef GPSTK_REFERENCE_FRAME_HPP
#define GPSTK_REFERENCE_FRAME_HPP

#include <iostream>
#include <string>

#include "Exception.hpp"

namespace gpstk{
	
	class ReferenceFrame;
	std::ostream& operator<<(std::ostream& os, ReferenceFrame& rf);
	
	class ReferenceFrame{
		public:
		
		enum FramesEnum{
			Unknown = 0,   /**< Reference frame is unknown*/
			WGS84,   /**< The WGS84 Reference Frame*/
			PZ90   /**< The PZ90 Reference Frame*/
		};
		
		ReferenceFrame(FramesEnum reference = Unknown)
			throw();
		
		ReferenceFrame(int index)
			throw();
		
		virtual ~ReferenceFrame() {	};
		
		void setReferenceFrame(const FramesEnum reference)
			throw();
		FramesEnum getFrame() const
			throw();
		ReferenceFrame& createReferenceFrame(string& name)
		   throw();
		
		std::string& asString() const
			throw();
		void fromString(std::string& name) const
			throw();
		
		ReferenceFrame& addFrame(std::string& name) const
		   throw();
		
		bool operator==(const ReferenceFrame right) const
			throw();
		bool operator!=(const ReferenceFrame right) const
			throw();
		bool operator>(const ReferenceFrame right) const
			throw();
		bool operator<(const ReferenceFrame right) const
			throw();
		bool operator>=(const ReferenceFrame right) const
			throw();
		bool operator<=(const ReferenceFrame right) const
			throw();
		
		private:
		
		FramesEnum frame;
		static std::map<FramesEnum, std::string> names;
	};
}

#endif
