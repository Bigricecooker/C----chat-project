#pragma once
#include "const.h"
#include "Singleton.h"
#include "ConfigMgr.h"
#include "message.grpc.pb.h"
#include "message.pb.h"
#include <grpcpp/grpcpp.h>
#include <queue>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <memory>
#include <mutex>
#include <condition_variable>



using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::AddFriendReq;
using message::AddFriendRsp;

using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::GetChatServerRsp;
using message::LoginRsp;
using message::LoginReq;
using message::ChatService;

using message::TextChatMsgReq;
using message::TextChatMsgRsp;
using message::TextChatData;

// RPCÁ¬½Ó³Ø
class ChatConPool
{
public:
	ChatConPool(size_t poolsize, std::string host, std::string port);;
	~ChatConPool();

	void Close();

	unique_ptr<ChatService::Stub> getConnection();
private:
	atomic<bool> _b_stop;
	size_t _poolsize;
	std::string _host;
	std::string _port;
	std::queue<std::unique_ptr<ChatService::Stub>>_connnections;
	std::mutex _mutex;
	std::condition_variable _cond;
};


class ChatGrpcClient:public Singleton<ChatGrpcClient>
{
	friend class Singleton<ChatGrpcClient>;
public:
	~ChatGrpcClient();

	AddFriendRsp NotifyAddFriend(std::string server_ip, const AddFriendReq& req);
	AuthFriendRsp NotifyAuthFriend(std::string server_ip, const AuthFriendReq& req);
	TextChatMsgRsp NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq& req, const Json::Value& rtvalue);
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);

private:
	ChatGrpcClient();

	unordered_map<std::string, std::unique_ptr<ChatConPool>> _pools;
};

