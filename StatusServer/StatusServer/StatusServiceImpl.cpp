#include "StatusServiceImpl.h"
#include "ConfigMgr.h"
#include "RedisMgr.h"
#include "const.h"

// 生成令牌
std::string generate_unique_string() {
    // 创建UUID对象
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    // 将UUID转换为字符串
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

// .proto中RPC方法 - 将消息服务器的信息返回给客户端
Status StatusServiceImpl::GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* reply)
{
    std::string prefix("Big_rice_cooker status server has received :  ");
    std::cout << prefix << std::endl;

    _server_index = (_server_index++) % (_servers.size());
    auto& server = _servers[_server_index];
    reply->set_host(server.host);
    reply->set_port(server.port);
    reply->set_error(ErrorCodes::Success);
    reply->set_token(generate_unique_string());// 设置访问令牌
    insertToken(request->uid(), reply->token());// 将令牌和用户标识起来
    return Status::OK;
}

// 将用户标识和令牌绑定
void StatusServiceImpl::insertToken(int uid, std::string token)
{
    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPREFIX + uid_str;
    // 存入redis
    RedisMgr::GetInstance()->Set(token_key, token);
}
