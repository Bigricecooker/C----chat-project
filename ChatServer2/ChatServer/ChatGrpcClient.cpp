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

	// Õ£÷π∑µªÿø’÷∏’Î
	if (_b_stop)
	{
		return nullptr;
	}

	auto context = std::move(_connnections.front());
	_connnections.pop();
	return context;
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

		_pools[cfg[word]["Name"]] = std::make_unique<ChatConPool>(5, cfg[word]["Host"], cfg[word]["Port"]);
	}

}

ChatGrpcClient::~ChatGrpcClient()
{
}


AddFriendRsp ChatGrpcClient::NotifyAddFriend(std::string server_ip, const AddFriendReq& req)
{
	return AddFriendRsp();
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
