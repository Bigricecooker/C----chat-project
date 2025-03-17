#include "LogicSystem.h"
#include "CSession.h"
#include "StatusGrpcClient.h"
#include "RedisMgr.h"

LogicSystem::~LogicSystem()
{
	_b_stop = true;
	_consume.notify_one();
	_worker_thread.join();
}

// 向消息队列投放消息
void LogicSystem::PostMsgToQue(std::shared_ptr<LogicNode> msg)
{
	std::unique_lock<std::mutex> lock(_mutex);
	_msg_que.push(msg);
	if (_msg_que.size() == 1)// 由0变为了1
	{
		lock.unlock();
		_consume.notify_one();
	}
}

LogicSystem::LogicSystem():_b_stop(false)
{
	RegisterCallBacks();
	_worker_thread = std::thread(&LogicSystem::DealMsg,this);
}

// 工作线程执行函数
void LogicSystem::DealMsg()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		// 队列为空且未关闭则阻塞等待并释放锁
		while (_msg_que.empty() && !_b_stop)
		{
			_consume.wait(lock);
		}

		// 如果关闭则先等待所有逻辑执行完
		if (_b_stop)
		{
			while (!_msg_que.empty())
			{
				auto msg_node = _msg_que.front();
				std::cout << "recv_msg id is " << msg_node->_recvnode->_msg_id << std::endl;
				auto call_back_iter = _fun_callbacks.find(msg_node->_recvnode->_msg_id);
				if (call_back_iter == _fun_callbacks.end())
				{
					_msg_que.pop();
					continue;
				}

				call_back_iter->second(msg_node->_session,
					msg_node->_recvnode->_msg_id, msg_node->_recvnode->_data);
				_msg_que.pop();
			}
			return;
		}

		// 如果没有停服
		auto msg_node = _msg_que.front();
		std::cout << "recv_msg id is " << msg_node->_recvnode->_msg_id << std::endl;
		auto call_back_iter = _fun_callbacks.find(msg_node->_recvnode->_msg_id);
		if (call_back_iter == _fun_callbacks.end())
		{
			_msg_que.pop();
			continue;
		}
		call_back_iter->second(msg_node->_session,
			msg_node->_recvnode->_msg_id, msg_node->_recvnode->_data);
		_msg_que.pop();
	}
}

// 注册回调函数
void LogicSystem::RegisterCallBacks()
{
	// 用户登录
	_fun_callbacks[MSG_CHAT_LOGIN] = bind(&LogicSystem::LoginHandler, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

bool LogicSystem::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo)
{
	// 优先在redis中查询
	return false;


	// 没找到则在mysql中查询并存入redis
}

void LogicSystem::LoginHandler(shared_ptr<CSession> session, const short& msg_id, const string& msg_data)
{
	Json::Reader reader;
	Json::Value root;
	Json::Value returnroot;
	reader.parse(msg_data, root);
	auto uid = root["uid"].asInt();
	auto token = root["token"].asString();
	std::cout << "user login uid is " << uid << "user token is " << token << std::endl;
	

	//
	//
	//

	// redis中查看token匹配是否正确
	std::string token_key = USERTOKENPREFIX + uid;
	std::string token_value = "";
	bool success = RedisMgr::GetInstance()->Get(token_key, token_value);
	if (!success)
	{
		returnroot["error"] = ErrorCodes::UidInvalid;
		std::string return_str = returnroot.toStyledString();
		session->Send(return_str, MSG_CHAT_LOGIN_RSP);
		return;
	}
	if (token_value != token)
	{
		returnroot["error"] = ErrorCodes::TokenInvalid;
		std::string return_str = returnroot.toStyledString();
		session->Send(return_str, MSG_CHAT_LOGIN_RSP);
		return;
	}

	// 获取用户信息
	std::string uid_str = std::to_string(uid);
	std::string base_key = USER_BASE_INFO + uid_str;
	auto user_info = std::make_shared<UserInfo>();
	bool b_base = GetBaseInfo(base_key, uid, user_info);
	if (!b_base)
	{
		returnroot["error"] = ErrorCodes::UidInvalid;
		return;
	}

	returnroot["uid"] = uid;
	returnroot["pwd"] = user_info->pwd;
	returnroot["name"] = user_info->name;
	returnroot["email"] = user_info->email;
	returnroot["nick"] = user_info->nick;
	returnroot["desc"] = user_info->desc;
	returnroot["sex"] = user_info->sex;
	returnroot["icon"] = user_info->icon;
	returnroot["error"] = ErrorCodes::Success;

	// 后期获取各种信息...todo

	std::string return_str = returnroot.toStyledString();
	session->Send(return_str, MSG_CHAT_LOGIN_RSP);


	// 将登录数增加
	auto server_name = ConfigMgr::Inst().GetValue("SelfSever", "Name");
	auto rd_res = RedisMgr::GetInstance()->HGet(LOGIN_COUNT, server_name);
	int count = 0;
	if (!rd_res.empty())
	{
		count = std::stoi(rd_res);
	}
	count++;
	// 写入redis
	auto count_str = std::to_string(count);
	bool success = RedisMgr::GetInstance()->HSet(LOGIN_COUNT, server_name, count_str);
	if (success)
	{
		std::cout << server_name << ": Login count +1;" << std::endl;
	}

	// 将连接与用户id绑定
	session->SetUserid(uid);
	return;
}
