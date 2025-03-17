#include "StatusServiceImpl.h"
#include "ConfigMgr.h"
#include "RedisMgr.h"
#include "const.h"
#include <string>
#include <iostream>

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
    auto server_list = cfg["ChatServer"]["Name"];

    std::vector<std::string> words;

    std::stringstream ss(server_list);
    std::string word;

    while (std::getline(ss, word, ','))
    {
        words.push_back(word);
    }

    for (auto& word : words)
    {
        if (cfg[word]["Name"].empty())
        {
            continue;
        }

        ChatServer server;
        server.host = cfg[word]["Host"];
        server.port = cfg[word]["Port"];
        server.name = cfg[word]["Name"];
        _servers[server.name] = server;
    }
    /*ChatServer server;
    server.port = cfg["ChatServer1"]["Port"];
    server.host = cfg["ChatServer1"]["Host"];
    _servers.push_back(server);

    server.port = cfg["ChatServer2"]["Port"];
    server.host = cfg["ChatServer2"]["Host"];
    _servers.push_back(server);*/
}

// .proto中RPC方法 - 将消息服务器的信息返回给客户端
Status StatusServiceImpl::GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* reply)
{
    std::string prefix("Big_rice_cooker status server has received :  ");
    std::cout << prefix << std::endl;
    // 获取连接数最小的聊天服务器信息
    auto server = getChatServer();

    reply->set_host(server.host);
    reply->set_port(server.port);
    reply->set_error(ErrorCodes::Success);
    reply->set_token(generate_unique_string());// 设置访问令牌
    insertToken(request->uid(), reply->token());// 将令牌和用户标识起来
    return Status::OK;
}

Status StatusServiceImpl::Login(ServerContext* context, const LoginReq* request, LoginRsp* reply)
{
    // 获取登录信息
    auto uid = request->uid();
    std::string uid_str = std::to_string(uid);
    auto token = request->token();

    std::lock_guard<std::mutex> lock(_mutex);
    std::string token_key = USERTOKENPREFIX + uid_str;
    std::string token_value = "";

    // redis中找到用户id对应的token
    bool success = RedisMgr::GetInstance()->Get(token_key, token_value);
    if (success)//?
    {
        reply->set_error(ErrorCodes::UidInvalid);
        return Status::OK;
    }

    if (token_value != token)
    {
        reply->set_error(ErrorCodes::TokenInvalid);
        return Status::OK;
    }

    reply->set_error(ErrorCodes::Success);
    reply->set_uid(uid);
    reply->set_token(token);
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

// 返回聊天服务器信息
ChatServer StatusServiceImpl::getChatServer()
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto minServer = _servers.begin()->second;
    auto count_str = RedisMgr::GetInstance()->HGet(LOGIN_COUNT, minServer.name);

    if (count_str.empty())// 没找到可能是服务器没开
    {
        minServer.con_count = INT_MAX;
    }
    else
    {
        minServer.con_count = std::stoi(count_str);
    }

    for (auto& server : _servers)
    {
        if (server.second.name == minServer.name)
        {
            continue;
        }

        auto count_str = RedisMgr::GetInstance()->HGet(LOGIN_COUNT, server.second.name);
        if (count_str.empty())
        {
            server.second.con_count = INT_MAX;
        }
        else
        {
            server.second.con_count = std::stoi(count_str);
        }

        if (server.second.con_count < minServer.con_count)
        {
            minServer = server.second;
        }
    }

    return minServer;// 返回连接数最小的聊天服务器信息
}
