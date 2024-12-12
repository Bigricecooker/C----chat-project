#include "CServer.h"
#include "AsioIOServicePool.h"

CServer::CServer(boost::asio::io_context& io_context,unsigned short  port):_io_context(io_context),_port(port),
_acceptor(_io_context,tcp::endpoint(tcp::v4(),_port))
{
	std::cout << "Server start success, listen on port : " << _port << std::endl;
	StartAccept();
}

void CServer::StartAccept()
{
	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	auto new_session = std::make_shared<CSession>();
	// 异步接收连接
	_acceptor.async_accept(new_session->GetSocket(), std::bind(&CServer::HandleAccept, this, new_session, placeholders::_1));
}

void CServer::HandleAccept(std::shared_ptr<CSession> session, const boost::system::error_code& error)
{
	if (!error)
	{
		session->Start();// 开始处理
		_sessions.insert(make_pair(session->GetUuid(), session));
	}
	else
	{
		cout << "session accept failed, error is " << error.what() << endl;
	}

	// 继续接收连接
	StartAccept();
}
