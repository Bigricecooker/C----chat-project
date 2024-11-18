#pragma once
#include "const.h"
class LogicSystem;

// ������д��
class HttpConnection :public std::enable_shared_from_this<HttpConnection>
{
public:
	friend class LogicSystem;
	HttpConnection(tcp::socket socket);
	void Start();

private:
	void CheckDeadline();// ��ʱ��
	void WriteResponse();// Ӧ��
	void HandleReq();// ����ע������
	tcp::socket _socket;

	beast::flat_buffer _buffer{ 8192 };// ������������
	http::request<http::dynamic_body> _request;// ������������
	http::response<http::dynamic_body> _response;// ������Ӧ�ͻ���
	net::steady_timer deadline_{//��������ʱ���ж������Ƿ�ʱ
		_socket.get_executor(),std::chrono::seconds(60)
	};
};

