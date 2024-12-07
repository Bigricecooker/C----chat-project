#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;

// 消息服务器信息
struct ChatServer {
    std::string host;
    std::string port;
};

class StatusServiceImpl final : public StatusService::Service
{
    // 继承StatusService::Service之后内部需要实现StatusService中定义的所有RPC方法，这些方法在通常在 .proto 文件中定义
public:
    StatusServiceImpl();

    // .proto中RPC方法 - 将消息服务器的信息返回给客户端
    Status GetChatServer(ServerContext* context, const GetChatServerReq* request,
        GetChatServerRsp* reply) override;

    
private:
    // 将用户标识和令牌绑定
    void insertToken(int uid, std::string token);

    std::vector<ChatServer> _servers;
    int _server_index;
};

