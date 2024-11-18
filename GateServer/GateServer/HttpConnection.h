#pragma once
#include "const.h"
class LogicSystem;

// 监听读写类
class HttpConnection :public std::enable_shared_from_this<HttpConnection>
{
public:
	friend class LogicSystem;
	HttpConnection(tcp::socket socket);
	void Start();

private:
	void CheckDeadline();// 定时器
	void WriteResponse();// 应答
	void HandleReq();// 处理注册请求
	tcp::socket _socket;

	beast::flat_buffer _buffer{ 8192 };// 用来接受数据
	http::request<http::dynamic_body> _request;// 用来解析数据
	http::response<http::dynamic_body> _response;// 用来回应客户端
	net::steady_timer deadline_{//用来做定时器判断请求是否超时
		_socket.get_executor(),std::chrono::seconds(60)
	};
};

