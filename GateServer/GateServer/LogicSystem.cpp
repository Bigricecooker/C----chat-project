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
    // 注册GET请求
    RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
        beast::ostream(connection->_response.body()) << "receive get_test req";
        });

    // 注册Post请求-获取验证码
    RegPost("/get_varifycode", [](std::shared_ptr<HttpConnection> connection) {
        // 提取请求Body内容
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;

        // 设置响应类型
        connection->_response.set(http::field::content_type, "text/json");

        // 解析JSON数据
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;// 存放解析后的JSON对象
        bool parse_success = reader.parse(body_str, src_root);

        // 解析失败处理
        if (!parse_success)
        {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        // 没有email字段
        if (!src_root.isMember("email"))
        {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        // 有email字段
        auto email = src_root["email"].asString();

        GetVarifyRsp rsp= VerifyGrpcClient::GetInstance()->GetVarifyCode(email);

        cout << "email is " << email << std::endl;
        root["error"] = rsp.error();
        root["email"] = src_root["email"];
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;
        });


    // 注册Post请求-注册用户
    RegPost("/user_register", [](std::shared_ptr<HttpConnection> connection) {
        // 提取请求Body内容
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;

        // 设置响应类型
        connection->_response.set(http::field::content_type, "text/json");

        // 解析JSON数据
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;// 存放解析后的JSON对象
        bool parse_success = reader.parse(body_str, src_root);

        // 解析失败处理
        if (!parse_success)
        {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        // 先判断验证码是否和redis中的对应
        std::string  verify_code;
        bool b_get_varify = RedisMgr::GetInstance()->Get(CODEPREFIX +src_root["email"].asString(), verify_code);
        // 验证码过期
        if (!b_get_varify)
        {
            std::cout << " get varify code expired" << std::endl;
            root["error"] = ErrorCodes::VerifyExpired;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }
        // 验证码错误 
        if (src_root["verifycode"].asString() != verify_code)
        {
            std::cout << " varify code error" << std::endl;
            root["error"] = ErrorCodes::VerifyCoderErr;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        // 数据库查找判断用户是否存在


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
