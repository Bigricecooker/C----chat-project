#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"


using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

// ��
class RPConPool 
{
public:
	RPConPool(std::size_t poolsize, std::string host, std::string port);
	~RPConPool();

	// �̹߳ر�
	void Close();

	// ��ȡ���
	std::unique_ptr<VarifyService::Stub> getConnection();

	// ���մ��
	void returnConnection(std::unique_ptr<VarifyService::Stub> context);
private:
	std::atomic<bool> _b_stop;// ��ʶ�Ƿ���Ҫ����(ԭ������)
	std::size_t _poolSize;// ��С
	std::string _host;
	std::string _port;
	std::queue<std::unique_ptr<VarifyService::Stub>> _connections;
	std::mutex _mutex;// ��
	std::condition_variable _cond;// ������
};



// ������֤������ͻ��� - ����
class VerifyGrpcClient :public Singleton<VerifyGrpcClient>
{
	friend class Singleton<VerifyGrpcClient>;
public:
	// ��ȡ��֤��
	GetVarifyRsp GetVarifyCode(std::string email);
private:
	VerifyGrpcClient();
	/*std::unique_ptr<VarifyService::Stub> stub_;*/

	std::unique_ptr<RPConPool> _pool;
};

