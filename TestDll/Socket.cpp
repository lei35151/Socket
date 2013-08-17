#include "Socket.h"
#include "src/Data.h"

#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/StreamSocket.h"

using Poco::Net::ServerSocket;
using Poco::Net::TCPServer;
using Poco::Net::TCPServerConnectionFactoryImpl;
using Poco::Net::TCPServerConnectionFactory;
using Poco::Net::TCPServerConnection;
using Poco::Net::StreamSocket;

Poco::Event g_eventWriteDB;
Poco::Mutex g_mutexcopy;

std::string _mydb = "user=root;password=123456;db=mytest;compress=true;auto-reconnect=true;port=3306;host=localhost";

std::vector<LL::Element> RecvElements;

DWORD WINAPI ThreadWriteDB(LPVOID lParam);

class EchoConnection : public TCPServerConnection
{
public:
	EchoConnection(const StreamSocket& s) : TCPServerConnection(s)
	{
// 		m_data = new LL::Data();
// 		m_data->connect(_mydb);

		// 打开定时器
		timer = new Poco::Timer(500, 1000);
		timer->start(Poco::TimerCallback<EchoConnection>(*this, &EchoConnection::onTime));
	}

	void run();
	
private:
	void onTime(Poco::Timer& t);
	void saveRevDataInQueue(char* revdata);

	LL::Data* m_data;
	Poco::Timer *timer;
};

void EchoConnection::onTime(Poco::Timer& t)
{
	g_eventWriteDB.set();
}

void EchoConnection::run()
{
	// 处理服务端业务流程，接收数据并处理流程
	StreamSocket& ss = socket();
	try
	{
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		int n = ss.receiveBytes(buffer, sizeof(buffer));
		saveRevDataInQueue(buffer);

		while (n > 0)
		{
			sprintf(buffer, "%s","server response!");
			n = strlen(buffer);
			ss.sendBytes(buffer, n);

			if (RecvElements.size() > 500)
			{
				g_eventWriteDB.set();
			}

			memset(buffer, 0, sizeof(buffer));
			n = ss.receiveBytes(buffer, sizeof(buffer));
			saveRevDataInQueue(buffer);
		}
	}
	catch (Poco::Exception& exc)
	{
		std::cerr << "EchoConnection: " << exc.displayText() << std::endl;
	}
}

//字符串分割函数
std::vector<std::string> splitrecvdata(std::string str,std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str+=pattern;//扩展字符串以方便操作
	int size=str.size();

	for(int i=0; i<size; i++)
	{
		pos=str.find(pattern,i);
		if(pos<size)
		{
			std::string s=str.substr(i,pos-i);
			result.push_back(s);
			i=pos+pattern.size()-1;
		}
	}
	return result;
}

void EchoConnection::saveRevDataInQueue(char* revdata)
{
	std::vector<std::string> res;
	std::string separator = "||";
	std::string strRecvdata(revdata);

	res = splitrecvdata(strRecvdata, separator);

	LL::Element datagrama;
	datagrama.index = 1;
	datagrama.UUID = 2;
	datagrama.datatype = 3;
	datagrama.port = 4;
	datagrama.ip = "000";
	datagrama.time = "111";
	datagrama.date = "222";
	datagrama.module = "333";
	datagrama.hFun = "444";
	datagrama.datagram = "555";

	g_mutexcopy.lock();
	RecvElements.push_back(datagrama);
	g_mutexcopy.unlock();
}

CSocket::CSocket(NETSTAT netType, int port, const std::string& addr)
{
	if (stat_service == netType)
	{
		initService(port);
	}
	else if (stat_client == netType)
	{
		initClient(addr, port);
	}
}

CSocket::CSocket()
{
	
}
CSocket::~CSocket()
{
	srv->stop();
}

void CSocket::initService(int port)
{
	// 服务端初始化，绑定端口及监听，并将接收数据包注册并通过EchoConnection类实现
	ServerSocket svs(port);
	srv = new TCPServer(new TCPServerConnectionFactoryImpl<EchoConnection>(), svs);
}

void CSocket::Start()
{
	// 创建线程，用于将接收数据写入数据库
	CreateThread(NULL, 0, ThreadWriteDB, NULL, 0, NULL);
	// 启动服务
	srv->start();

	while (true)
	{
		Poco::Thread::sleep(100);
	}
}

void CSocket::initClient(const std::string& addr, int port)
{

}

void CSocket::senddata(const std::string& datagram)
{

}

DWORD WINAPI ThreadWriteDB(LPVOID lParam)
{
	LL::Data* m_data = new LL::Data();
	m_data->connect(_mydb);

	std::vector<LL::Element> RecvStation;
	while(true)
	{
		g_eventWriteDB.wait();

		// 存入临时缓冲，避免socket阻塞
		g_mutexcopy.lock();
		if (RecvElements.size() > 0  )
		{
			for (int i=0; i<RecvElements.size(); i++)
			{
				RecvStation.push_back(RecvElements[i]);
			}

			RecvElements.clear();
		}
		g_mutexcopy.unlock();

		// 缓冲写数据库
		LL::Element element;
		for (int j=0; j<RecvStation.size(); j++)
		{
			element = RecvStation[j];
			m_data->commit(element);
		}

		RecvStation.clear();

		g_eventWriteDB.reset();
	}

	return 0;
}