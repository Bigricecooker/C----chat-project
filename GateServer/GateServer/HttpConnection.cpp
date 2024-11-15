#include "HttpConnection.h"
#include "LogicSystem.h"

HttpConnection::HttpConnection(tcp::socket socket):_socket(std::move(socket))// socketû�п�������,ֻ���ƶ�����
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
				// http����������Ҫ��ճ������������Ϊ HTTP Э�鱾�����������ڱ����ʶ��ճ��
				boost::ignore_unused(bytes_transferred);// ���Կ��԰ѷ��͵��ֽ������Ե�

				// ��ʼ�������������
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
			// û����ر��׽��֣���ʱ����ʱ
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
			// ���������ݺ�Ͽ����Ͷ�
			self->_socket.shutdown(tcp::socket::shutdown_send, ec);
			// ȡ����ʱ��
			self->deadline_.cancel();
		});
}

void HttpConnection::HandleReq()
{
	// ���ð汾
	_response.version(_request.version());
	//����Ϊ������
	_response.keep_alive(false);
	// GET����
	if (_request.method() == http::verb::get)
	{
		bool success = LogicSystem::GetInstance()->HandleGet(_request.target(),shared_from_this());
		if (!success)
		{
			// ����״̬��
			_response.result(http::status::not_found);
			// ���û�Ӧ����
			_response.set(http::field::content_type, "text/plain");// �ı�����
			// ��Ӧ��
			beast::ostream(_response.body()) << "url not found\r\n";
			// �ذ�
			WriteResponse();
			return;
		}
		_response.result(http::status::ok);
		_response.set(http::field::server, "GateServer");
		WriteResponse();
		return;
	}
}
