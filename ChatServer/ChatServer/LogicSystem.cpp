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
	

	std::string token_key = USERTOKENPREFIX + uid;
	std::string token_value = "";
	// redis�в鿴tokenƥ���Ƿ���ȷ
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

	returnroot["error"] = ErrorCodes::Success;
	std::string return_str = returnroot.toStyledString();
	session->Send(return_str, MSG_CHAT_LOGIN_RSP);
	return;
}
