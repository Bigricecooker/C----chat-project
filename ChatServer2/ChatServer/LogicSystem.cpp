#include "LogicSystem.h"
#include "CSession.h"
#include "StatusGrpcClient.h"
#include "RedisMgr.h"
#include "MysqlMgr.h"
#include "UserMgr.h"

LogicSystem::~LogicSystem()
{
	_b_stop = true;
	_consume.notify_one();
	_worker_thread.join();
}

// ����Ϣ����Ͷ����Ϣ
void LogicSystem::PostMsgToQue(std::shared_ptr<LogicNode> msg)
{
	std::unique_lock<std::mutex> lock(_mutex);
	_msg_que.push(msg);
	if (_msg_que.size() == 1)// ��0��Ϊ��1
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

// �����߳�ִ�к���
void LogicSystem::DealMsg()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		// ����Ϊ����δ�ر��������ȴ����ͷ���
		while (_msg_que.empty() && !_b_stop)
		{
			_consume.wait(lock);
		}

		// ����ر����ȵȴ������߼�ִ����
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

		// ���û��ͣ��
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

// ע��ص�����
void LogicSystem::RegisterCallBacks()
{
	// �û���¼
	_fun_callbacks[MSG_CHAT_LOGIN] = bind(&LogicSystem::LoginHandler, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	// �û�����
	_fun_callbacks[ID_SEARCH_USER_REQ] = bind(&LogicSystem::SearchInfo, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

bool LogicSystem::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo)
{
	// ������redis�в�ѯ
	std::string info_str = "";
	bool b_base = RedisMgr::GetInstance()->Get(base_key, info_str);
	if (b_base)
	{
		Json::Reader reader;
		Json::Value root;
		reader.parse(info_str, root);
		userinfo->uid = root["uid"].asInt();
		userinfo->name = root["name"].asString();
		userinfo->pwd = root["pwd"].asString();
		userinfo->email = root["email"].asString();
		userinfo->nick = root["nick"].asString();
		userinfo->desc = root["desc"].asString();
		userinfo->sex = root["sex"].asInt();
		userinfo->icon = root["icon"].asString();
		std::cout << "user login uid is  " << userinfo->uid << " name  is "
			<< userinfo->name << " pwd is " << userinfo->pwd << " email is " << userinfo->email << endl;
	}
	else// û�ҵ�����mysql�в�ѯ������redis
	{
		std::shared_ptr<UserInfo> user_info = nullptr;
		user_info = MysqlMgr::GetInstance()->GetUser(uid);
		if (user_info == nullptr) {
			return false;
		}

		userinfo = user_info;
		// ����redis
		Json::Value redis_root;
		redis_root["uid"] = uid;
		redis_root["pwd"] = userinfo->pwd;
		redis_root["name"] = userinfo->name;
		redis_root["email"] = userinfo->email;
		redis_root["nick"] = userinfo->nick;
		redis_root["desc"] = userinfo->desc;
		redis_root["sex"] = userinfo->sex;
		redis_root["icon"] = userinfo->icon;
		RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());
	}
	return true;
}

void LogicSystem::LoginHandler(shared_ptr<CSession> session, const short& msg_id, const string& msg_data)
{
	Json::Reader reader;
	Json::Value root;
	Json::Value returnroot;
	std::cout << msg_data << std::endl;
	reader.parse(msg_data, root);
	auto uid = root["uid"].asInt();
	auto token = root["token"].asString();
	std::cout << "user login uid is " << uid << " user token is " << token << std::endl;

	// redis�в鿴tokenƥ���Ƿ���ȷ
	std::string uid_str = std::to_string(uid);
	std::string token_key = USERTOKENPREFIX + uid_str;
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

	// ��ȡ�û���Ϣ
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

	// ���ڻ�ȡ������Ϣ...todo

	std::string return_str = returnroot.toStyledString();
	session->Send(return_str, MSG_CHAT_LOGIN_RSP);


	// ����¼������
	auto server_name = ConfigMgr::Inst().GetValue("SelfServer", "Name");
	auto rd_res = RedisMgr::GetInstance()->HGet(LOGIN_COUNT, server_name);
	int count = 0;
	if (!rd_res.empty())
	{
		count = std::stoi(rd_res);
	}
	count++;
	// д��redis
	auto count_str = std::to_string(count);
	bool b_success = RedisMgr::GetInstance()->HSet(LOGIN_COUNT, server_name, count_str);
	if (b_success)
	{
		std::cout << server_name << ": Login count +1;" << std::endl;
	}

	// ���������û�id��
	session->SetUserid(uid);

	// Ϊ�û����õ�¼ip server������
	std::string ipkey = USERIPPREFIX + uid_str;
	RedisMgr::GetInstance()->Set(ipkey, server_name);

	// uid��session�󶨹����Ժ������˲���(�ǵ��Ƴ�)
	UserMgr::GetInstance()->SetUserSession(uid, session);
	return;
}

void LogicSystem::SearchInfo(shared_ptr<CSession> session, const short& msg_id, const string& msg_data)
{
	Json::Reader reader;
	Json::Value root;
	Json::Value returnroot;
	reader.parse(msg_data, root);
	auto uid_str = root["uid"].asString();
	std::cout << "user SearchInfo uid is  " << uid_str << endl;

	// 
	bool b_digit = isPureDigit(uid_str);
	if (b_digit) {
		GetUserByUid(uid_str, returnroot);
	}
	else {
		GetUserByName(uid_str, returnroot);
	}

	// �ظ�
	std::string return_str = returnroot.toStyledString();
	session->Send(return_str, ID_SEARCH_USER_RSP);
}

bool LogicSystem::isPureDigit(const std::string& str)
{
	for (char c : str)
	{
		if (!std::isdigit(c))
		{
			return false;
		}
	}
	return true;
}

void LogicSystem::GetUserByUid(std::string uid_str, Json::Value& rtvalue)
{
	rtvalue["error"] = ErrorCodes::Success;
	// �ȴ�redis����
	std::string base_key = USER_BASE_INFO + uid_str;
	std::string info_str = "";
	bool b_base = RedisMgr::GetInstance()->Get(base_key, info_str);
	if (b_base)
	{
		Json::Reader reader;
		Json::Value root;
		reader.parse(info_str, root);
		auto uid = root["uid"].asInt();
		auto name = root["name"].asString();
		auto pwd = root["pwd"].asString();
		auto email = root["email"].asString();
		auto nick = root["nick"].asString();
		auto desc = root["desc"].asString();
		auto sex = root["sex"].asInt();
		auto icon = root["icon"].asString();
		std::cout << "user  uid is  " << uid << " name  is "
			<< name << " pwd is " << pwd << " email is " << email << " icon is " << icon << endl;

		rtvalue["uid"] = uid;
		rtvalue["pwd"] = pwd;
		rtvalue["name"] = name;
		rtvalue["email"] = email;
		rtvalue["nick"] = nick;
		rtvalue["desc"] = desc;
		rtvalue["sex"] = sex;
		rtvalue["icon"] = icon;
		return;
	}

	// redisû�оͲ�ѯ���ݿ�
	auto uid = std::stoi(uid_str);
	std::shared_ptr<UserInfo> user_info = nullptr;
	user_info = MysqlMgr::GetInstance()->GetUser(uid);
	if (user_info == nullptr)
	{
		rtvalue["error"] = ErrorCodes::UidInvalid;
		return;
	}

	// �����ݷ���redis����
	Json::Value redis_root;
	redis_root["uid"] = user_info->uid;
	redis_root["pwd"] = user_info->pwd;
	redis_root["name"] = user_info->name;
	redis_root["email"] = user_info->email;
	redis_root["nick"] = user_info->nick;
	redis_root["desc"] = user_info->desc;
	redis_root["sex"] = user_info->sex;
	RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());

	//��������
	rtvalue["uid"] = user_info->uid;
	rtvalue["pwd"] = user_info->pwd;
	rtvalue["name"] = user_info->name;
	rtvalue["email"] = user_info->email;
	rtvalue["nick"] = user_info->nick;
	rtvalue["desc"] = user_info->desc;
	rtvalue["sex"] = user_info->sex;
}

void LogicSystem::GetUserByName(std::string name, Json::Value& rtvalue)
{
	rtvalue["error"] = ErrorCodes::Success;
	// �ȴ�redis����
	std::string base_key = NAME_INFO + name;
	std::string info_str = "";
	bool b_base = RedisMgr::GetInstance()->Get(base_key, info_str);
	if (b_base)
	{
		Json::Reader reader;
		Json::Value root;
		reader.parse(info_str, root);
		auto uid = root["uid"].asInt();
		auto name = root["name"].asString();
		auto pwd = root["pwd"].asString();
		auto email = root["email"].asString();
		auto nick = root["nick"].asString();
		auto desc = root["desc"].asString();
		auto sex = root["sex"].asInt();
		auto icon = root["icon"].asString();
		std::cout << "user  uid is  " << uid << " name  is "
			<< name << " pwd is " << pwd << " email is " << email << " icon is " << icon << endl;

		rtvalue["uid"] = uid;
		rtvalue["pwd"] = pwd;
		rtvalue["name"] = name;
		rtvalue["email"] = email;
		rtvalue["nick"] = nick;
		rtvalue["desc"] = desc;
		rtvalue["sex"] = sex;
		rtvalue["icon"] = icon;
		return;
	}

	// redisû�оͲ�ѯ���ݿ�
	std::shared_ptr<UserInfo> user_info = nullptr;
	user_info = MysqlMgr::GetInstance()->GetUser(name);
	if (user_info == nullptr)
	{
		rtvalue["error"] = ErrorCodes::NameInvalid;
		return;
	}

	// �����ݷ���redis����
	Json::Value redis_root;
	redis_root["uid"] = user_info->uid;
	redis_root["pwd"] = user_info->pwd;
	redis_root["name"] = user_info->name;
	redis_root["email"] = user_info->email;
	redis_root["nick"] = user_info->nick;
	redis_root["desc"] = user_info->desc;
	redis_root["sex"] = user_info->sex;
	RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());

	//��������
	rtvalue["uid"] = user_info->uid;
	rtvalue["pwd"] = user_info->pwd;
	rtvalue["name"] = user_info->name;
	rtvalue["email"] = user_info->email;
	rtvalue["nick"] = user_info->nick;
	rtvalue["desc"] = user_info->desc;
	rtvalue["sex"] = user_info->sex;
}
