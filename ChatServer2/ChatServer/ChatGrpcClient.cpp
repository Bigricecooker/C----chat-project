#include "ChatGrpcClient.h"
#include "RedisMgr.h"
#include "ConfigMgr.h"

#include "CSession.h"
//#include "MysqlMgr.h"
//#include "UserMgr.h"


ChatConPool::ChatConPool(size_t poolsize, std::string host, std::string port):_poolsize(poolsize),
_host(host),_port(port),_b_stop(false)
{
	for (size_t i = 0; i < _poolsize; i++)
	{
		std::shared_ptr<Channel> channel = grpc::CreateChannel(_host + ":" + _port, grpc::InsecureChannelCredentials());
		_connnections.push(ChatService::NewStub(channel));
	}
}

ChatConPool::~ChatConPool()
{
	std::lock_guard<std::mutex> lock(_mutex);
	Close();

	while (!_connnections.empty())
	{
		_connnections.pop();
	}

}

void ChatConPool::Close()
{
	_b_stop = true;
	_cond.notify_all();
}

std::unique_ptr<ChatService::Stub> ChatConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_cond.wait(lock, [this]() {
		if (_b_stop)
		{
			return true;
		}

		return !_connnections.empty();
		});

	// 停止返回空指针
	if (_b_stop)
	{
		return nullptr;
	}

	auto context = std::move(_connnections.front());
	_connnections.pop();
	return context;
}

void ChatConPool::returnConnection(std::unique_ptr<ChatService::Stub> context)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_b_stop) {
		return;
	}
	_connnections.push(std::move(context));
	_cond.notify_one();
}





// ---------------------------------------------------------------------------------



ChatGrpcClient::ChatGrpcClient()
{
	auto& cfg = ConfigMgr::Inst();
	auto server_list = cfg["PeerServer"]["Servers"];
	std::vector<std::string> words;

	std::stringstream ss(server_list);
	std::string word;

	while (std::getline(ss, word, ',')) 
	{
		words.push_back(word);
	}
	for (auto& word : words) 
	{
		if (cfg[word]["Name"].empty()) {
			continue;
		}
		// 与其他聊天服务器的连接数据，每个聊天服务器有5个与其他任意的一个聊天服务器的连接
		_pools[cfg[word]["Name"]] = std::make_unique<ChatConPool>(5, cfg[word]["Host"], cfg[word]["Port"]);
	}

}

ChatGrpcClient::~ChatGrpcClient()
{
}

 
AddFriendRsp ChatGrpcClient::NotifyAddFriend(std::string server_ip, const AddFriendReq& req)
{
	AddFriendRsp rsp;
	auto find_iter = _pools.find(server_ip);// 找对应的聊天服务器的rpc连接
	if (find_iter == _pools.end()) {
		return rsp;
	}

	auto& pool = find_iter->second;
	ClientContext context;
	auto stub = pool->getConnection();
	Status status = stub->NotifyAddFriend(&context, req, &rsp);
	if (!status.ok()) {
		rsp.set_error(ErrorCodes::RPCFailed);
		rsp.set_applyuid(req.applyuid());
		rsp.set_touid(req.touid());
		pool->returnConnection(std::move(stub));
		return rsp;
	}

	rsp.set_error(ErrorCodes::Success);
	rsp.set_applyuid(req.applyuid());
	rsp.set_touid(req.touid());
	pool->returnConnection(std::move(stub));

	return rsp;

}

AuthFriendRsp ChatGrpcClient::NotifyAuthFriend(std::string server_ip, const AuthFriendReq& req)
{
	return AuthFriendRsp();
}

bool ChatGrpcClient::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo)
{
	return false;
}

TextChatMsgRsp ChatGrpcClient::NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq& req, const Json::Value& rtvalue)
{
	return TextChatMsgRsp();
}
