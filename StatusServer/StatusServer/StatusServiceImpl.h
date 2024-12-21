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


// ��Ϣ��������Ϣ
struct ChatServer {
    std::string host;
    std::string port;
};

class StatusServiceImpl final : public StatusService::Service
{
    // �̳�StatusService::Service֮���ڲ���Ҫʵ��StatusService�ж��������RPC��������Щ������ͨ���� .proto �ļ��ж���
public:
    StatusServiceImpl();

    // .proto��RPC���� - ����Ϣ����������Ϣ���ظ��ͻ���
    Status GetChatServer(ServerContext* context, const GetChatServerReq* request,
        GetChatServerRsp* reply) override;
    Status Login(ServerContext* context, const LoginReq* request,
        LoginRsp* reply);

    
private:
    // ���û���ʶ�����ư�
    void insertToken(int uid, std::string token);
    // ���������������Ϣ
    ChatServer getChatServer();

    std::mutex _mutex;
    std::vector<ChatServer> _servers;
    int _server_index;
};

