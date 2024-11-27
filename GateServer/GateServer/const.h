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
	PasswdUpFiled = 1008,// ��������ʧ��
	PasswdInvalid = 1009,// �������ʧ��
};

#define CODEPREFIX "code_" // ǰ׺