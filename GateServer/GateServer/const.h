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
	Success = 0,// 成功
	Error_json = 1001,// Json解析错误
	RPCFailed = 1002,// RPC请求错误
	VerifyExpired = 1003,// 验证码过期
	VerifyCoderErr = 1004,// 验证码错误
	UserExist = 1005,// 用户已存在
	PasswdErr = 1006,// 密码错误
	EmailNotMatch = 1007,// 邮箱不匹配
	PasswdUpFiled = 1008,// 更新密码失败
	PasswdInvalid = 1009,// 密码更新失败
};

#define CODEPREFIX "code_" // 前缀