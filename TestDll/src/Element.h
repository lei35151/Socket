#pragma once
#include <string>

namespace LL{

	struct Element
	{
		unsigned int	index;
		unsigned int	UUID;
		unsigned short	datatype;
		unsigned short	port;//65535
		std::string		ip;
		std::string		time;
		std::string		date;
		std::string		module;		
		std::string		hFun;		
		std::string		datagram;	

		Element()
		{
			index = 0;
		}
		Element(unsigned long idx, unsigned long uid, unsigned short idatatype, unsigned short iport, 
			const std::string& strip, const std::string& strtime, const std::string& strdate, const std::string& strmodule, const std::string& strfun,
			const std::string& strdatagram)
			: index(idx), UUID(uid), datatype(idatatype), port(iport), ip(strip), time(strtime), date(strdate), module(strmodule),
			hFun(strfun), datagram(strdatagram)
		{
		}
	};
}