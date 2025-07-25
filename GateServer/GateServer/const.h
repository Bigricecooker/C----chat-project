#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "hiredis.h"
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;


enum ErrorCodes {
	Success = 0,// �ɹ�
	Error_json = 1001,// Json��������
	RPCFailed = 1002,// RPC�������
	VerifyExpired = 1003,// ��֤�����
	VerifyCoderErr = 1004,// ��֤�����
	UserExist = 1005,// �û��Ѵ���
	PasswdErr = 1006,// �������
	EmailNotMatch = 1007,// ���䲻ƥ��
	PasswdUpFailed = 1008,// ��������ʧ��
	PasswdInvalid = 1009,// �������ʧ��
};

struct UserInfo
{
	std::string name;
	std::string email;
	std::string pwd;
	int uid;
};

#define CODEPREFIX "code_" // ǰ׺