#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifyGrpcClient.h"
#include "RedisMgr.h"
#include "MysqlMgr.h"

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

        // �����һ����ѯ���ݿ��������Ƿ��Ѵ��ڷ�ֹ����֤��о���һ�㣨�ܽ�����������bug��

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
            std::cout << " get verify code expired" << std::endl;
            root["error"] = ErrorCodes::VerifyExpired;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }
        // ��֤����� 
        if (src_root["verifycode"].asString() != verify_code)
        {
            std::cout << " verify code error" << std::endl;
            root["error"] = ErrorCodes::VerifyCoderErr;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        // ���ݿ�����ж��û��Ƿ����
        int uid = MysqlMgr::GetInstance()->RegUser(src_root["user"].asString(), src_root["email"].asString(), src_root["passwd"].asString());
        if (uid == 0 || uid == -1) {
            std::cout << " user or email exist" << std::endl;
            root["error"] = ErrorCodes::UserExist;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        // д�ذ�
        root["error"] = 0;
        root["email"] = src_root["email"].asString();
        root["uid"] = src_root["uid"].asString();
        root["user"] = src_root["user"].asString();
        root["passwd"] = src_root["passwd"].asString();
        root["confirm"] = src_root["confirm"].asString();
        root["verifycode"] = src_root["verifycode"].asString();
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;
        });


    // ע��Post����-��������
    RegPost("/reset_pwd", [](std::shared_ptr<HttpConnection> connection) {
        // ��ȡ����
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;

        // ������Ӧ����
        connection->_response.set(http::field::content_type, "text/json");

        // ����JSON����
        Json::Value root;// �ذ�����
        Json::Reader reader;
        Json::Value src_root;
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

        auto email = src_root["email"].asString();
        auto name = src_root["user"].asString();
        auto pwd = src_root["newpwd"].asString();

        // ���ж���֤���Ƿ��redis�еĶ�Ӧ
        std::string verify_code;
        bool b_get_verify = RedisMgr::GetInstance()->Get(CODEPREFIX + email, verify_code);
        
        // ��֤�����
        if (!b_get_verify)
        {
            std::cout << " get verify code expired" << std::endl;
            root["error"] = ErrorCodes::VerifyExpired;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }
        
        // ��֤�����
        if (verify_code != src_root["verifycode"].asString())
        {
            std::cout << " verify code error" << std::endl;
            root["error"] = ErrorCodes::VerifyCoderErr;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        // ��ѯ���ݿ��ж��û����������Ƿ�ƥ��
        bool email_valid = MysqlMgr::GetInstance()->CheckEmail(name,email);
        if (!email_valid)
        {
            std::cout << " user email not match" << std::endl;
            root["error"] = ErrorCodes::EmailNotMatch;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        // ��������Ϊ��������
        bool b_up = MysqlMgr::GetInstance()->UpdatePwd(name, pwd);
        if (!b_up)
        {
            std::cout << " update pwd failed" << std::endl;
            root["error"] = ErrorCodes::PasswdUpFailed;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        std::cout << "succeed to update password" << pwd << std::endl;

        // д�ذ�
        root["error"] = 0;
        root["email"] = email;
        root["user"] = name;
        root["passwd"] = pwd;
        root["verifycode"] = src_root["verifycode"].asString();
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;
        });

    // ע��Post����-�û���¼
    RegPost("/user_login", [](std::shared_ptr<HttpConnection> connection) {
        // ��ȡ����
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;

        // ������Ӧ����
        connection->_response.set(http::field::content_type, "text/json");

        // ����JSON����
        Json::Value root;// �ذ�����
        Json::Reader reader;
        Json::Value src_root;
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

        auto name = src_root["name"].asString();
        auto pwd = src_root["pwd"].asString();
        UserInfo userinfo;
        
        // ���ݿ��ѯ�û��������Ƿ�ƥ��
        bool pwd_valid = MysqlMgr::GetInstance()->Checkpwd(name, pwd, userinfo);
        if (!pwd_valid) {
            std::cout << " user pwd not match" << std::endl;
            root["error"] = ErrorCodes::PasswdInvalid;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }
        });
}
