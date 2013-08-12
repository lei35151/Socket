#pragma once
#include "Element.h"
#include "Poco/Timer.h"
#include "Poco/SharedPtr.h"
#include "Poco/Data/Session.h"

namespace LL{
	class Data
	{
	public:
		Data();
		Data(const std::string& dbConnString);

		~Data();
		void connect(const std::string& dbConnString);
		void query(unsigned long id);
		void commit(Element& e);
	private:
		void WriteToDB(Element& e);
		std::vector<Element> ReadFromDB();
		void onTime(Poco::Timer& t);
		void createTable();
		void dropTable(const std::string& tableName);

		//std::list<Element> elements;
		std::vector<Element> elements;
		std::vector<Element> readElments;

		Poco::Timer *timer;

		static Poco::SharedPtr<Poco::Data::Session> _pSession;

		time_t t_start;
	};
}