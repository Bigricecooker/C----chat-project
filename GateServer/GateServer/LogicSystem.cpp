#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifyGrpcClient.h"
#include "RedisMgr.h"

LogicSystem::~LogicSystem()
{
}

bool LogicSystem::HandleGet(std::string path, std::shared_ptr<HttpConnection> connection)
{
    if (_get_handlers.find(path) == _get_handlers.end())
    {
        return false;
    }

    _get_handlers[path](connection);
    return true;
}

void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
    _get_handlers.insert(make_pair(url, handler));
}

void LogicSystem::RegPost(std::string url, HttpHandler handler)
{
    _post_handlers.insert(make_pair(url, handler));
}

bool LogicSystem::HandlePost(std::string path, std::shared_ptr<HttpConnection> connection)
{
    if (_post_handlers.find(path) == _post_handlers.end())
    {
        return false;
    }

    _post_handlers[path](connection);
    return true;
}

LogicSystem::LogicSystem()
{
    // ע��GET����
    RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
        beast::ostream(connection->_response.body()) << "receive get_test req";
        });

    // ע��Post����-��ȡ��֤��
    RegPost("/get_varifycode", [](std::shared_ptr<HttpConnection> connection) {
        // ��ȡ����Body����
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;

        // ������Ӧ����
        connection->_response.set(http::field::content_type, "text/json");

        // ����JSON����
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;// ��Ž������JSON����
        bool parse_success = reader.parse(body_str, src_root);

        // ����ʧ�ܴ���
        if (!parse_success)
        {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        // û��email�ֶ�
        if (!src_root.isMember("email"))
        {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        // ��email�ֶ�
        auto email = src_root["email"].asString();

        GetVarifyRsp rsp= VerifyGrpcClient::GetInstance()->GetVarifyCode(email);

        cout << "email is " << email << std::endl;
        root["error"] = rsp.error();
        root["email"] = src_root["email"];
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;
        });


    // ע��Post����-ע���û�
    RegPost("/user_register", [](std::shared_ptr<HttpConnection> connection) {
        // ��ȡ����Body����
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;

        // ������Ӧ����
        connection->_response.set(http::field::content_type, "text/json");

        // ����JSON����
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;// ��Ž������JSON����
        bool parse_success = reader.parse(body_str, src_root);

        // ����ʧ�ܴ���
        if (!parse_success)
        {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        // ���ж���֤���Ƿ��redis�еĶ�Ӧ
        std::string  verify_code;
        bool b_get_varify = RedisMgr::GetInstance()->Get(CODEPREFIX +src_root["email"].asString(), verify_code);
        // ��֤�����
        if (!b_get_varify)
        {
            std::cout << " get varify code expired" << std::endl;
            root["error"] = ErrorCodes::VerifyExpired;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }
        // ��֤����� 
        if (src_root["verifycode"].asString() != verify_code)
        {
            std::cout << " varify code error" << std::endl;
            root["error"] = ErrorCodes::VerifyCoderErr;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        // ���ݿ�����ж��û��Ƿ����


        root["error"] = 0;
        root["email"] = src_root["email"];
        root["user"] = src_root["user"].asString();
        root["passwd"] = src_root["passwd"].asString();
        root["confirm"] = src_root["confirm"].asString();
        root["varifycode"] = src_root["varifycode"].asString();
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;
        });
}
