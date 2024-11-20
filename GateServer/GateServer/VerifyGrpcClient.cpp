#include "VerifyGrpcClient.h"
#include "ConfigMgr.h"

RPConPool::RPConPool(std::size_t poolsize, std::string host, std::string port):_poolSize(poolsize),_host(host),
_port(port),_b_stop(false)
{
	for (std::size_t i = 0; i < _poolSize; i++)
	{
		// 创建一个 gRPC 通信通道 --- gRPC 通信的基础。客户端通过此通道与服务端交互
		std::shared_ptr<Channel> channel = grpc::CreateChannel(_host+":"+_port, grpc::InsecureChannelCredentials());// 使用不安全的

		// 创建存根并绑定到指定的通信通道
		// 存根的作用：提供接口，用于调用远程服务方法（例如 GetVarifyCode）。
		//			   客户端通过存根发送请求到服务器并接收响应。
		_connections.push(VarifyService::NewStub(channel));// 走的移动赋值(VarifyService::NewStub(channel)是一个临时的对象)
	}
}

RPConPool::~RPConPool()
{
	std::lock_guard<std::mutex> lock(_mutex);// 让线程先处理完
	Close();
	while (!_connections.empty()) {
		_connections.pop();
	}
}

void RPConPool::Close()
{
	_b_stop = true;
	_cond.notify_all();
}

std::unique_ptr<VarifyService::Stub> RPConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(_mutex);// lock_guard不支持显示释放锁
	_cond.wait(lock, [this]() {// 被唤醒切为true时才会尝试获取锁继续向下执行
		if (_b_stop)
		{
			return true;// 停止
		}
		return !_connections.empty();
		});

	//如果停止则直接返回空指针
	if (_b_stop) {
		return  nullptr;
	}
	
	auto context = std::move(_connections.front());
	_connections.pop();
	return context;
}

void RPConPool::returnConnection(std::unique_ptr<VarifyService::Stub> context)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_b_stop) {
		return;
	}
	_connections.push(std::move(context));
	_cond.notify_one();
}

//----------------------------------------------------------------------

GetVarifyRsp VerifyGrpcClient::GetVarifyCode(std::string email)
{
	ClientContext context;// 上下文 
	GetVarifyRsp reply;// 回包
	GetVarifyReq request;// 请求

	request.set_email(email);// 设置email成员

	// 请求并返回状态（发起 RPC 调用）
	auto stub = _pool->getConnection();
	Status status = stub->GetVarifyCode(&context, request, &reply);// 请求后返回状态
	if (status.ok())
	{
		_pool->returnConnection(std::move(stub));
		return reply;
	}
	else
	{
		_pool->returnConnection(std::move(stub));
		reply.set_error(ErrorCodes::RPCFailed);
		return reply;
	}
}

VerifyGrpcClient::VerifyGrpcClient()
{
	//// 创建一个 gRPC 通信通道 --- gRPC 通信的基础。客户端通过此通道与服务端交互
	//std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());// 使用不安全的
	//// 创建存根并绑定到指定的通信通道
	//// 存根的作用：提供接口，用于调用远程服务方法（例如 GetVarifyCode）。
	////			   客户端通过存根发送请求到服务器并接收响应。
	//stub_ = VarifyService::NewStub(channel);

	// 初始化池，从池中获取绑定了信道的存根
	auto& gCfgMgr = ConfigMgr::Inst();
	std::string host = gCfgMgr["VarifyServer"]["Host"];
	std::string port = gCfgMgr["VarifyServer"]["Port"];
	_pool.reset(new RPConPool(5, host, port));
}
