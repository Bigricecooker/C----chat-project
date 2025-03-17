#pragma once
#include "const.h"
#include <boost/asio.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include "MsgNode.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class LogicSystem;
class CServer;
class CSession : public std::enable_shared_from_this<CSession>
{
public:
	CSession(boost::asio::io_context& io_context, CServer* server);
	~CSession();

	void Start();
	std::string GetUuid();// 返回该连接的uuid
	tcp::socket& GetSocket();// 获取该socket连接
	void Close();
	
	void Send(char* msg, short max_length, short msgid);
	void Send(std::string msg, short msgid);// 回消息

	void SetUserid(int uid_str);// 设置与该连接绑定的用户id
	int GetUserid();
private:
	void HandleReadHead(const boost::system::error_code& error, size_t  bytes_transferred, std::shared_ptr<CSession> shared_self);// 读取头部回调
	void HandleRead(const boost::system::error_code& error, size_t bytes_transferred, std::shared_ptr<CSession>shared_self);// 读取消息回调
	void HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession>shared_self);// 读取消息回调
	std::string _uuid;
	tcp::socket _socket;
	CServer* _server;
	
	//收到的消息结构
	std::shared_ptr<RecvNode> _recv_msg_node;
	bool _b_head_parse;
	//收到的头部结构
	std::shared_ptr<MsgNode> _recv_head_node;

	std::mutex _mutex;
	std::queue<std::shared_ptr<SendNode>> _send_que;// 消息发送队列

	bool _b_close;

	int _userid;
};


// 逻辑节点
class LogicNode {
	friend class LogicSystem;
public:
	LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> recvnode);
private:
	std::shared_ptr<CSession> _session;
	std::shared_ptr<RecvNode> _recvnode;
};