#pragma once
#include "const.h"
#include "CSession.h"

using boost::asio::ip::tcp;

class CSession;
class CServer
{
public:
	CServer(boost::asio::io_context& io_context, unsigned short port);
	~CServer();
	void ClearSession();
private:
	void StartAccept();// 开始接收连接
	void HandleAccept(std::shared_ptr<CSession> session,const boost::system::error_code& error);

	boost::asio::io_context& _io_context;
	std::string _host;
	unsigned short  _port;
	tcp::acceptor _acceptor;
	std::map<std::string, std::shared_ptr<CSession>> _sessions;
	std::mutex _mutex;
};

