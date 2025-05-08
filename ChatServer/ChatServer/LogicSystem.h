#pragma once
#include "Singleton.h"
#include <queue>
#include <thread>
#include <queue>
#include <map>
#include <functional>
#include "const.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <unordered_map>

class CSession;
class LogicNode;
typedef  function<void(shared_ptr<CSession>, const short& msg_id, const string& msg_data)> FunCallBack;
class LogicSystem :public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem();

	void PostMsgToQue(std::shared_ptr<LogicNode> msg);// ����Ϣ����Ͷ����Ϣ
private:
	LogicSystem();
	void DealMsg();// �����߳�ִ�к���
	void RegisterCallBacks();// ע��ص�����
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);// ��ȡ�û���Ϣ
	

	// �ص�����
	std::map<short, FunCallBack> _fun_callbacks;
	void LoginHandler(shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	void SearchInfo(shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	void AddFriendApply(shared_ptr<CSession> session, const short& msg_id, const string& msg_data);


	bool isPureDigit(const std::string& str);// �ж��Ƿ�������
	void GetUserByUid(std::string uid_str, Json::Value& rtvalue);// ͨ��uid��ȡ�û���Ϣ
	void GetUserByName(std::string name, Json::Value& rtvalue);// ͨ�����ֻ�ȡ�û���Ϣ

	
	bool _b_stop;
	std::thread _worker_thread;// �����߳�
	std::queue<shared_ptr<LogicNode>> _msg_que;// ��Ϣ����
	std::mutex _mutex;
	std::condition_variable _consume;
	
};

