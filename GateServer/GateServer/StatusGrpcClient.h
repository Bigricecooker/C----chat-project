#pragma once
#include "singleton.h"
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "message.pb.h"
#include "const.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;

class StatusConPool
{
public:
	StatusConPool(std::size_t poolsize, std::string host, std::string port);
	~StatusConPool();

	// �̹߳ر�
	void Close();

	// ��ȡ���
	std::unique_ptr<StatusService::Stub> getConnection();

	// ���մ��
	void returnConnection(std::unique_ptr<StatusService::Stub> context);
private:
	std::atomic<bool> _b_stop;// ��ʶ�Ƿ���Ҫ����(ԭ������)
	std::size_t _poolSize;// ��С
	std::string _host;
	std::string _port;
	std::queue<std::unique_ptr<StatusService::Stub>> _connections;
	std::mutex _mutex;// ��
	std::condition_variable _cond;// ������
};

// ״̬����ͻ���
class StatusGrpcClient:public Singleton<StatusGrpcClient>
{
	friend class Singleton<StatusGrpcClient>;
public:
	~StatusGrpcClient();

	GetChatServerRsp GetChatServer(int uid);
private:
	StatusGrpcClient();
	std::unique_ptr<StatusConPool> _pool;
};

