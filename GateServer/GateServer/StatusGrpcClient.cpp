#include "StatusGrpcClient.h"
#include "ConfigMgr.h"

StatusConPool::StatusConPool(std::size_t poolsize, std::string host, std::string port):_poolSize(poolsize), _host(host),
_port(port),_b_stop(false)
{
	for (std::size_t i = 0; i < _poolSize; i++)
	{
		std::shared_ptr<Channel> channel = grpc::CreateChannel(_host + ":" + _port,
			grpc::InsecureChannelCredentials());
		_connections.push(StatusService::NewStub(channel));
	}
}

StatusConPool::~StatusConPool()
{
	std::lock_guard<std::mutex> lock(_mutex);
	Close();
	while (!_connections.empty())
	{
		_connections.pop();
	}
}

void StatusConPool::Close()
{
	_b_stop = true;
	_cond.notify_all();
}

std::unique_ptr<StatusService::Stub> StatusConPool::getConnection()
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

void StatusConPool::returnConnection(std::unique_ptr<StatusService::Stub> context)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_b_stop) {
		return;
	}
	_connections.push(std::move(context));
	_cond.notify_one();
}




StatusGrpcClient::StatusGrpcClient()
{
	ConfigMgr& gCfgMgr = ConfigMgr::Inst();
	std::string host = gCfgMgr["StatusServer"]["Host"];
	std::string port = gCfgMgr["StatusServer"]["Port"];
	_pool.reset(new StatusConPool(5, host, port));
}

StatusGrpcClient::~StatusGrpcClient()
{

}

GetChatServerRsp StatusGrpcClient::GetChatServer(int uid)
{
	ClientContext context;
	GetChatServerRsp reply;
	GetChatServerReq request;
	request.set_uid(uid);
	auto stub = _pool->getConnection();
	Status status = stub->GetChatServer(&context, request, &reply);

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
