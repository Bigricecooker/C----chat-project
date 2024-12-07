#include "StatusServiceImpl.h"
#include "ConfigMgr.h"
#include "RedisMgr.h"
#include "const.h"

// ��������
std::string generate_unique_string() {
    // ����UUID����
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    // ��UUIDת��Ϊ�ַ���
    std::string unique_string = to_string(uuid);
    return unique_string;
}







StatusServiceImpl::StatusServiceImpl():_server_index(0)
{
    auto& cfg = ConfigMgr::Inst();

    ChatServer server;
    server.port = cfg["ChatServer1"]["Port"];
    server.host = cfg["ChatServer1"]["Host"];
    _servers.push_back(server);

    server.port = cfg["ChatServer2"]["Port"];
    server.host = cfg["ChatServer2"]["Host"];
    _servers.push_back(server);
}

// .proto��RPC���� - ����Ϣ����������Ϣ���ظ��ͻ���
Status StatusServiceImpl::GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* reply)
{
    std::string prefix("Big_rice_cooker status server has received :  ");
    std::cout << prefix << std::endl;

    _server_index = (_server_index++) % (_servers.size());
    auto& server = _servers[_server_index];
    reply->set_host(server.host);
    reply->set_port(server.port);
    reply->set_error(ErrorCodes::Success);
    reply->set_token(generate_unique_string());// ���÷�������
    insertToken(request->uid(), reply->token());// �����ƺ��û���ʶ����
    return Status::OK;
}

// ���û���ʶ�����ư�
void StatusServiceImpl::insertToken(int uid, std::string token)
{
    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPREFIX + uid_str;
    // ����redis
    RedisMgr::GetInstance()->Set(token_key, token);
}
