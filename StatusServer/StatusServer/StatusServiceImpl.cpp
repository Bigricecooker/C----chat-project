#include "StatusServiceImpl.h"
#include "ConfigMgr.h"
#include "RedisMgr.h"
#include "const.h"
#include <string>
#include <iostream>

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

// .proto��RPC���� - ����Ϣ����������Ϣ���ظ��ͻ���
Status StatusServiceImpl::GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* reply)
{
    std::string prefix("Big_rice_cooker status server has received :  ");
    std::cout << prefix << std::endl;
    // ��ȡ��������С�������������Ϣ
    auto server = getChatServer();

    reply->set_host(server.host);
    reply->set_port(server.port);
    reply->set_error(ErrorCodes::Success);
    reply->set_token(generate_unique_string());// ���÷�������
    insertToken(request->uid(), reply->token());// �����ƺ��û���ʶ����
    return Status::OK;
}

Status StatusServiceImpl::Login(ServerContext* context, const LoginReq* request, LoginRsp* reply)
{
    // ��ȡ��¼��Ϣ
    auto uid = request->uid();
    std::string uid_str = std::to_string(uid);
    auto token = request->token();

    std::lock_guard<std::mutex> lock(_mutex);
    std::string token_key = USERTOKENPREFIX + uid_str;
    std::string token_value = "";

    // redis���ҵ��û�id��Ӧ��token
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

// ���û���ʶ�����ư�
void StatusServiceImpl::insertToken(int uid, std::string token)
{
    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPREFIX + uid_str;
    // ����redis
    RedisMgr::GetInstance()->Set(token_key, token);
}

// ���������������Ϣ
ChatServer StatusServiceImpl::getChatServer()
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto minServer = _servers.begin()->second;
    auto count_str = RedisMgr::GetInstance()->HGet(LOGIN_COUNT, minServer.name);

    if (count_str.empty())// û�ҵ������Ƿ�����û��
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

    return minServer;// ������������С�������������Ϣ
}
