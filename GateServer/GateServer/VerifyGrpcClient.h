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

// 池
class RPConPool 
{
public:
	RPConPool(std::size_t poolsize, std::string host, std::string port);
	~RPConPool();

	// 线程关闭
	void Close();

	// 获取存根
	std::unique_ptr<VarifyService::Stub> getConnection();

	// 回收存根
	void returnConnection(std::unique_ptr<VarifyService::Stub> context);
private:
	std::atomic<bool> _b_stop;// 标识是否需要回收(原子类型)
	std::size_t _poolSize;// 大小
	std::string _host;
	std::string _port;
	std::queue<std::unique_ptr<VarifyService::Stub>> _connections;
	std::mutex _mutex;// 锁
	std::condition_variable _cond;// 互斥量
};



// 发送验证码请求客户端 - 单例
class VerifyGrpcClient :public Singleton<VerifyGrpcClient>
{
	friend class Singleton<VerifyGrpcClient>;
public:
	// 获取验证码
	GetVarifyRsp GetVarifyCode(std::string email);
private:
	VerifyGrpcClient();
	/*std::unique_ptr<VarifyService::Stub> stub_;*/

	std::unique_ptr<RPConPool> _pool;
};

