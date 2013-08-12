#pragma once
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/TCPServer.h"

enum NETSTAT
{
	stat_service =1,
	stat_client = 2
};

class CSocket
{
public:
	CSocket();
	CSocket(NETSTAT netType, int port, const std::string& addr = "localhost");
	~CSocket();

	//客户端接口
	//void connect(const std::string& addr, int port);
	void senddata(const std::string& datagram);

	//服务端接口，启动socket服务
	void Start();
private:
	void initService(int port);
	void initClient(const std::string& addr, int port);
	Poco::Net::TCPServer* srv;
};