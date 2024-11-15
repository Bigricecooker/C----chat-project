#include "HttpConnection.h"
#include "LogicSystem.h"

HttpConnection::HttpConnection(tcp::socket socket):_socket(std::move(socket))// socket没有拷贝构造,只有移动构造
{
}

void HttpConnection::Start()
{
	auto self = shared_from_this();
	http::async_read(_socket, _buffer, _request, [self](beast::error_code ec,
		std::size_t bytes_transferred) 
		{
			try
			{
				if (ec)
				{
					std::cout << "http read err is" << ec.what() << std::endl;
					return;
				}
				// http服务器不需要做粘包处理，这是因为 HTTP 协议本身的设计有助于避免和识别粘包
				boost::ignore_unused(bytes_transferred);// 所以可以把发送的字节数忽略掉

				// 开始处理读到的数据
				self->HandleReq();
				self->CheckDeadline();
			}
			catch (std::exception exp)
			{
				std::cout << "exception is " << exp.what() << std::endl;
			}
		});
}

void HttpConnection::CheckDeadline()
{
	auto self = shared_from_this();
	deadline_.async_wait([self](beast::error_code ec) {
		if (!ec)
		{
			// 没出错关闭套接字，定时器超时
			self->_socket.close(ec);
		}
		});
}

void HttpConnection::WriteResponse()
{
	auto self = shared_from_this();
	_response.content_length(_response.body().size());
	http::async_write(_socket, _response, [self](beast::error_code ec,
		std::size_t bytes_transferred) {
			// 发送完数据后断开发送端
			self->_socket.shutdown(tcp::socket::shutdown_send, ec);
			// 取消定时器
			self->deadline_.cancel();
		});
}

void HttpConnection::HandleReq()
{
	// 设置版本
	_response.version(_request.version());
	//设置为短链接
	_response.keep_alive(false);
	// GET请求
	if (_request.method() == http::verb::get)
	{
		bool success = LogicSystem::GetInstance()->HandleGet(_request.target(),shared_from_this());
		if (!success)
		{
			// 设置状态码
			_response.result(http::status::not_found);
			// 设置回应类型
			_response.set(http::field::content_type, "text/plain");// 文本类型
			// 响应体
			beast::ostream(_response.body()) << "url not found\r\n";
			// 回包
			WriteResponse();
			return;
		}
		_response.result(http::status::ok);
		_response.set(http::field::server, "GateServer");
		WriteResponse();
		return;
	}
}
