#include "Data.h"

using namespace LL;
using namespace Poco::Data;
#include "Poco/Data/MySQL/MySQL.h"
#include "Poco/Data/MySQL/Connector.h"
#include "Poco/Data/MySQL/MySQLException.h"
#include "Poco/Data/SessionFactory.h"
#include "Poco/Mutex.h"

using namespace Poco::Data;
using Poco::format;
using Poco::Data::MySQL::ConnectionException;
using Poco::Data::MySQL::StatementException;
Poco::SharedPtr<Poco::Data::Session> Data::_pSession = 0;

using Poco::TimerCallback;

Poco::Mutex m_Mutex;
/*
unsigned int	index;
unsigned int	UUID;
unsigned short	datatype;	//数据包类型，发送/接收
unsigned short	port;		//65535
std::string		ip;
std::string		time;		//时间
std::string		date;		//日期
std::string		module;		//模块
str::string		hFun;		//获取函数
str::string		datagram;	//数据报
*/

namespace Poco {
namespace Data {

template <>
class TypeHandler<Element>
{
public:
	static void bind(std::size_t pos, const Element& obj, AbstractBinder* pBinder)
	{
		// the table is defined as Person (LastName VARCHAR(30), FirstName VARCHAR, Address VARCHAR, Age INTEGER(3))
		poco_assert_dbg (pBinder != 0);
		pBinder->bind(pos++, obj.index);
		pBinder->bind(pos++, obj.UUID);
		pBinder->bind(pos++, obj.datatype);
		pBinder->bind(pos++, obj.port);
		pBinder->bind(pos++, obj.ip);
		pBinder->bind(pos++, obj.time);
		pBinder->bind(pos++, obj.date);
		pBinder->bind(pos++, obj.module);
		pBinder->bind(pos++, obj.hFun);
		pBinder->bind(pos++, obj.datagram);
	}

	static void prepare(std::size_t pos, const Element& obj, AbstractPreparation* pPrepare)
	{
		// the table is defined as Person (LastName VARCHAR(30), FirstName VARCHAR, Address VARCHAR, Age INTEGER(3))
		poco_assert_dbg (pPrepare != 0);
		pPrepare->prepare(pos++, obj.index);
		pPrepare->prepare(pos++, obj.UUID);
		pPrepare->prepare(pos++, obj.datatype);
		pPrepare->prepare(pos++, obj.port);
		pPrepare->prepare(pos++, obj.ip);
		pPrepare->prepare(pos++, obj.time);
		pPrepare->prepare(pos++, obj.date);
		pPrepare->prepare(pos++, obj.module);
		pPrepare->prepare(pos++, obj.hFun);
		pPrepare->prepare(pos++, obj.datagram);
	}

	static std::size_t size()
	{
		return 6;
	}

	static void extract(std::size_t pos, Element& obj, const Element& defVal, AbstractExtractor* pExt)
	{
		poco_assert_dbg (pExt != 0);
		if (!pExt->extract(pos++, obj.index))
			obj.index = defVal.index;
		if (!pExt->extract(pos++, obj.UUID))
			obj.UUID = defVal.UUID;
		if (!pExt->extract(pos++, obj.datatype))
			obj.UUID = defVal.datatype;
		if (!pExt->extract(pos++, obj.port))
			obj.port = defVal.port;
		if (!pExt->extract(pos++, obj.ip))
			obj.ip = defVal.ip;
		if (!pExt->extract(pos++, obj.time))
			obj.time = defVal.time;
		if (!pExt->extract(pos++, obj.date))
			obj.date = defVal.date;
		if (!pExt->extract(pos++, obj.module))
			obj.module = defVal.module;
		if (!pExt->extract(pos++, obj.hFun))
			obj.hFun = defVal.hFun;
		if (!pExt->extract(pos++, obj.datagram))
			obj.datagram = defVal.datagram;
	}

private:
	TypeHandler();
	~TypeHandler();
	TypeHandler(const TypeHandler&);
	TypeHandler& operator=(const TypeHandler&);
};

}
}


Data::Data()
{
}

Data::Data(const std::string& dbConnString)
{
	connect(dbConnString);
}

Data::~Data()
{
	timer->stop();
}

void Data::connect(const std::string& dbConnString)
{
	static bool beenHere = false;
	MySQL::Connector::registerConnector();

	// 连接数据库
	if (!beenHere)
	{
		try
		{
			_pSession = new Session(SessionFactory::instance().create(MySQL::Connector::KEY, dbConnString));
		}
		catch (ConnectionException& ex)
		{
			std::cout << "WARNING: Connection failed. MySQL tests will fail!" << std::endl;
			std::cout << ex.displayText() << std::endl;
		}

		if (_pSession && _pSession->isConnected()) 
			std::cout << "*** Connected to " << '(' << dbConnString << ')' << std::endl;
	}

	// 打开定时器
	timer = new Poco::Timer(250, 500);
	timer->start(TimerCallback<Data>(*this, &Data::onTime));
}

void Data::query(unsigned long id)
{
	
}
void Data::commit(Element& e)
{
	WriteToDB(e);
}

void Data::WriteToDB(Element& e)
{
	if (NULL == _pSession)
	{
		return;
	}

	// 当前数据0条时记录时间
	if (0 == elements.size())
	{
		t_start = time(NULL);
	}

	m_Mutex.lock();
	// 数据写入队列
	elements.push_back(e);

	// 小于500条不做存储
	if (elements.size() < 500)
	{
		m_Mutex.unlock();
		return ;
	}

	// 判断表是否存在,如果不存在创建
	createTable();

	// 数据写入数据库
	if (!_pSession)
	{
		try 
		{
			*_pSession << "INSERT INTO Data VALUES (?,?,?,?,?,?,?,?,?,?)", use(elements), now; 
		}
		catch(ConnectionException& ce)
		{ 
			std::cout << ce.displayText() << std::endl; 
		}
		catch(StatementException& se)
		{
			std::cout << se.displayText() << std::endl; 
		}

		elements.clear();
	}

	m_Mutex.unlock();
}
std::vector<Element> Data::ReadFromDB()
{
	// 先清空队列中的信息，然后读取
	readElments.clear();

	if (!_pSession)
	{
		try 
		{ 
			*_pSession << "SELECT * FROM NetData", into(readElments), now; 
		}
		catch(ConnectionException& ce)
		{ 
			std::cout << ce.displayText() << std::endl;
		}
		catch(StatementException& se)
		{ 
			std::cout << se.displayText() << std::endl;
		}
	}

	return readElments;
}

void Data::onTime(Poco::Timer& t)
{
	time_t t_time = time(NULL);

	// 如果10s未提交过数据，则将当前数据提交到数据库
	if (10 < t_time - t_start && elements.size()>0)
	{
		if (NULL != _pSession)
		{
			m_Mutex.lock();
			// 判断表是否存在,如果不存在创建
			createTable();

			try 
			{
				*_pSession << "INSERT NetData VALUES (?,?,?,?,?,?,?,?,?,?)", use(elements), now; 
			}
			catch(ConnectionException& ce)
			{ 
				m_Mutex.unlock();
				std::cout << ce.displayText() << std::endl; 
			}
			catch(StatementException& se)
			{
				m_Mutex.unlock();
				std::cout << se.displayText() << std::endl; 
			}

			elements.clear();

			m_Mutex.unlock();
		}
	}
}

void Data::createTable()
{
	try 
	{ 
		//*_pSession << "CREATE TABLE NetData (id INTEGER, uvid INTEGER, tcpport INTEGER, ip VARCHAR(32), recvtime VARCHAR(32), datainfo VARCHAR(1024))", now; 
		*_pSession << "CREATE TABLE NetData (id INTEGER, uvid INTEGER, datatype INTEGER, tcpport INTEGER, ip VARCHAR(32), recvtime VARCHAR(32), "
			"recvdate VARCHAR(32), module VARCHAR(32), sendfun VARCHAR(32),datagram VARCHAR(1024))", now; 
	}
	catch(ConnectionException& ce)
	{ 
		std::cout << ce.displayText() << std::endl; //fail ("recreatePersonTable()"); 
	}
	catch(StatementException& se)
	{ 
		std::cout << se.displayText() << std::endl; //fail ("recreatePersonTable()"); 
	}
	catch(...)
	{
		std::cout << "error" << std::endl;
	}
}

void Data::dropTable(const std::string& tableName)
{
	try
	{
		*_pSession << format("DROP TABLE %s", tableName), now;
	}
	catch (StatementException& exc)
	{
		std::cout << exc.displayText() << std::endl;
	}
}
