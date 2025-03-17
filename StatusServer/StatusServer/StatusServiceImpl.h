#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using message::GetChatServerReq;
using message::GetChatServerRsp;

using message::LoginReq;
using message::LoginRsp;

using message::StatusService;


// 消息服务器信息
class ChatServer 
{
public:
    ChatServer() :host(""), port(""), name(""), con_count(0) {}
    ChatServer(const ChatServer& cs) :host(cs.host), port(cs.port), name(cs.name), con_count(cs.con_count) {}
    ChatServer& operator=(const ChatServer& cs) {
        if (&cs == this) {
            return *this;
        }

        host = cs.host;
        name = cs.name;
        port = cs.port;
        con_count = cs.con_count;
        return *this;
    }
    std::string name;
    std::string host;
    std::string port;
    int con_count;// 连接数量
};

class StatusServiceImpl final : public StatusService::Service
{
    // 继承StatusService::Service之后内部需要实现StatusService中定义的所有RPC方法，这些方法在通常在 .proto 文件中定义
public:
    StatusServiceImpl();

    // .proto中RPC方法 - 将消息服务器的信息返回给客户端
    Status GetChatServer(ServerContext* context, const GetChatServerReq* request,
        GetChatServerRsp* reply) override;
    Status Login(ServerContext* context, const LoginReq* request,
        LoginRsp* reply);

    
private:
    // 将用户标识和令牌绑定
    void insertToken(int uid, std::string token);
    // 返回聊天服务器信息
    ChatServer getChatServer();

    std::mutex _mutex;
    std::unordered_map<std::string, ChatServer> _servers;// 所有聊天服务器信息
    int _server_index;
};

