#include "CSession.h"
#include "CServer.h"
#include "LogicSystem.h"

CSession::CSession(boost::asio::io_context& io_context, CServer* server):_server(server),_socket(io_context), _b_close(false)
{
	boost::uuids::uuid  a_uuid = boost::uuids::random_generator()();
	_uuid = boost::uuids::to_string(a_uuid);
	_recv_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
}

CSession::~CSession()
{
	std::cout << "~CSession destruct" << endl;
}

void CSession::Start()
{
	_recv_head_node->Clear();
	// 开始读取头部数据
	boost::asio::async_read(_socket, boost::asio::buffer(_recv_head_node->_data, HEAD_TOTAL_LEN), 
		std::bind(&CSession::HandleReadHead,this, std::placeholders::_1, std::placeholders::_2, shared_from_this()));
}

std::string CSession::GetUuid()
{
	return _uuid;
}

tcp::socket& CSession::GetSocket()
{
	return _socket;
}

void CSession::Close()
{
	_socket.close();
	_b_close = true;
}

void CSession::Send(char* msg, short max_length, short msgid)
{
	std::lock_guard<std::mutex> lock(_mutex);
	int send_que_size = _send_que.size();
	if (send_que_size > MAX_SENDQUE) {
		std::cout << "session: " << _uuid << " send que fulled, size is " << MAX_SENDQUE << endl;
		return;
	}

	_send_que.push(std::make_shared<SendNode>(msg, max_length, msgid));
	if (send_que_size > 0)
	{
		return;
	}

	auto& msg_node = _send_que.front();
	boost::asio::async_write(_socket, boost::asio::buffer(msg_node->_data, msg_node->_total_len),
		bind(&CSession::HandleWrite, this, std::placeholders::_1, shared_from_this()));
}

void CSession::Send(std::string msg, short msgid)
{
	std::lock_guard<std::mutex> lock(_mutex);
	int send_que_size = _send_que.size();
	if (send_que_size > MAX_SENDQUE) {
		std::cout << "session: " << _uuid << " send que fulled, size is " << MAX_SENDQUE << endl;
		return;
	}

	_send_que.push(std::make_shared<SendNode>(msg.c_str(),msg.length(), msgid));
	if (send_que_size > 0)
	{
		return;
	}
	// 只有发送队列由空变为1才执行下面

	auto& msg_node = _send_que.front();
	boost::asio::async_write(_socket, boost::asio::buffer(msg_node->_data, msg_node->_total_len),
		bind(&CSession::HandleWrite, this, std::placeholders::_1,  shared_from_this()));
}

void CSession::HandleReadHead(const boost::system::error_code& error, size_t bytes_transferred, std::shared_ptr<CSession> shared_self)
{
	auto self = shared_self;
	try
	{
		if (error)
		{
			std::cout << "handle read failed, error is " << error.what() << std::endl;
			Close();
			_server->ClearSession(_uuid);
			return;
		}

		if (bytes_transferred < HEAD_TOTAL_LEN) {
			std::cout << "read length not match, read [" << bytes_transferred << "] , total ["
				<< HEAD_TOTAL_LEN << "]" << std::endl;
			Close();
			_server->ClearSession(_uuid);
			return;
		}

		// 获取id
		short msg_id = 0;
		memcpy(&msg_id, _recv_head_node->_data, HEAD_ID_LEN);
		//网络字节序转化为本地字节序
		msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
		std::cout << "msg_id is " << msg_id << std::endl;

		// id非法
		if (msg_id > MAX_LENGTH) {
			std::cout << "invalid msg_id is " << msg_id << std::endl;
			_server->ClearSession(_uuid);
			return;
		}
		
		// 获取长度
		short msg_len = 0;
		memcpy(&msg_id, _recv_head_node->_data + HEAD_ID_LEN, HEAD_DATA_LEN);
		//网络字节序转化为本地字节序
		msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);
		std::cout << "msg_len is " << msg_len << std::endl;

		// 长度非法
		if (msg_len > MAX_LENGTH) {
			std::cout << "invalid data length is " << msg_len << std::endl;
			_server->ClearSession(_uuid);
			return;
		}

		_recv_head_node->Clear();
		// 初始化收到的消息结构
		_recv_msg_node = std::make_shared<RecvNode>(msg_len, msg_id);

		// 开始读取消息结构
		boost::asio::async_read(_socket,boost::asio::buffer(_recv_msg_node->_data, _recv_msg_node->_total_len),
			bind(&CSession::HandleRead,this, std::placeholders::_1, std::placeholders::_2, shared_self));
	}
	catch (std::exception& e) 
	{
		std::cout << "Exception code is " << e.what() << std::endl;
	}
}

void CSession::HandleRead(const boost::system::error_code& error, size_t bytes_transferred, std::shared_ptr<CSession> shared_self)
{
	auto self = shared_self;
	try
	{ 
		if (error)
		{
			std::cout << "handle read failed, error is " << error.what() << std::endl;
			Close();
			_server->ClearSession(_uuid);
			return;
		}

		if (bytes_transferred < _recv_head_node->_total_len) {
			std::cout << "read length not match, read [" << bytes_transferred << "] , total ["
				<< _recv_head_node->_total_len << "]" << std::endl;
			Close();
			_server->ClearSession(_uuid);
			return;
		}

		_recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';
		_recv_msg_node->_cur_len = bytes_transferred;
		std::cout << "receive data is " << _recv_msg_node->_data << std::endl;

		// 放入逻辑队列处理
		LogicSystem::GetInstance()->PostMsgToQue(std::make_shared<LogicNode>(self, _recv_msg_node));

		_recv_msg_node->Clear();
		// 继续读取头部
		boost::asio::async_read(_socket, boost::asio::buffer(_recv_head_node->_data, HEAD_TOTAL_LEN),
			std::bind(&CSession::HandleReadHead, this, std::placeholders::_1, std::placeholders::_2, shared_self));
	}
	catch (std::exception& e)
	{
		std::cout << "Exception code is " << e.what() << std::endl;
	}
}

void CSession::HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self)
{
	auto self = shared_self;
	try
	{
		if (!error)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			//cout << "send data " << _send_que.front()->_data+HEAD_LENGTH << endl;
			_send_que.pop();
			if (!_send_que.empty())
			{
				auto& msgnode = _send_que.front();
				boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
					std::bind(&CSession::HandleWrite, this, std::placeholders::_1, shared_self));
			}
		}
		else
		{
			std::cout << "handle write failed, error is " << error.what() << std::endl;
			Close();
			_server->ClearSession(_uuid);
			return;
		}
	}
	catch (std::exception& e)
	{
		std::cout << "Exception code is " << e.what() << std::endl;
	}
}

LogicNode::LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> recvnode):_session(session),_recvnode(recvnode)
{
}
