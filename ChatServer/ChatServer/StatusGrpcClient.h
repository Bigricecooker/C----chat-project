#pragma once
#include "const.h"
#include "Singleton.h"
#include "ConfigMgr.h"
#include "message.grpc.pb.h"
#include "message.pb.h"
#include <grpcpp/grpcpp.h>
#include <queue>
#include <condition_variable>
using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

//using message::GetChatServerReq;
//using message::GetChatServerRsp;

using message::LoginRsp;
using message::LoginReq;

using message::StatusService;


class StatusConPool
{
public:
	StatusConPool(std::size_t poolsize, std::string host, std::string port);
	~StatusConPool();

	// 线程关闭
	void Close();

	// 获取存根
	std::unique_ptr<StatusService::Stub> getConnection();

	// 回收存根
	void returnConnection(std::unique_ptr<StatusService::Stub> context);
private:
	std::atomic<bool> _b_stop;// 标识是否需要回收(原子类型)
	std::size_t _poolSize;// 大小
	std::string _host;
	std::string _port;
	std::queue<std::unique_ptr<StatusService::Stub>> _connections;
	std::mutex _mutex;// 锁
	std::condition_variable _cond;// 互斥量
};


class StatusGrpcClient :public Singleton<StatusGrpcClient>
{
	friend class Singleton<StatusGrpcClient>;
public:
	~StatusGrpcClient();

	LoginRsp Login(int uid, std::string token);
private:
	StatusGrpcClient();
	std::unique_ptr<StatusConPool> _pool;
};

