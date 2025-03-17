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
    int con_count;// ��������
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
    std::unordered_map<std::string, ChatServer> _servers;// ���������������Ϣ
    int _server_index;
};

