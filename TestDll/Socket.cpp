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

std::string _mydb = "user=root;password=123456;db=mytest;compress=true;auto-reconnect=true;port=3306;host=localhost";

class EchoConnection : public TCPServerConnection
{
public:
	EchoConnection(const StreamSocket& s) : TCPServerConnection(s)
	{
		m_data = new LL::Data();
		m_data->connect(_mydb);
	}

	void run()
	{
		LL::Element datagrama;

		// 处理服务端业务流程，接收数据并处理流程
		StreamSocket& ss = socket();
		Poco::Net::SocketAddress addr = ss.address();
		try
		{
			char recvbuffer[1024];
			char sendbuffer[1024];
			memset(recvbuffer, 0, sizeof(recvbuffer));
			memset(sendbuffer, 0, sizeof(sendbuffer));
// 			int n = ss.receiveBytes(recvbuffer, sizeof(recvbuffer));
// 			std::cout << recvbuffer << std::endl;
			int n=0;

			while (n>0)/*(n > 0)*/
			{
				memset(sendbuffer, 0, sizeof(sendbuffer));
				sprintf(sendbuffer, "%s","server response!");
				n = sizeof(sendbuffer);
				ss.sendBytes(sendbuffer, n);

				memset(recvbuffer, 0, sizeof(recvbuffer));
				n = ss.receiveBytes(recvbuffer, sizeof(recvbuffer));
				std::cout << "recv byte:" << recvbuffer << std::endl;

				if (n > 0)
				{
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

					m_data->commit(datagrama);
				}

// 				if (n > 0)
// 				{
// 					datagrama.index = 1;
// 					datagrama.UUID = 2;
// 					datagrama.datatype = 3;
// 					datagrama.port = 4;
// 					datagrama.ip = "000";
// 					datagrama.time = "111";
// 					datagrama.date = "222";
// 					datagrama.module = "333";
// 					datagrama.hFun = "444";
// 					datagrama.datagram = "555";
// 
// 					m_data->commit(datagrama);
// 
// 					memset(sendbuffer, 0, sizeof(sendbuffer));
// 					sprintf(sendbuffer, "%s","server response!");
// 					n = sizeof(sendbuffer);
// 					ss.sendBytes(sendbuffer, n);
// 				}
			}
		}
		catch (Poco::Exception& exc)
		{
			std::cerr << "EchoConnection: " << exc.displayText() << std::endl;
		}
	}

private:
	LL::Data* m_data;
};

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