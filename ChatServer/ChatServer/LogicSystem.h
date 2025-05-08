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

	void PostMsgToQue(std::shared_ptr<LogicNode> msg);// 向消息队列投放消息
private:
	LogicSystem();
	void DealMsg();// 工作线程执行函数
	void RegisterCallBacks();// 注册回调函数
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);// 获取用户信息
	

	// 回调函数
	std::map<short, FunCallBack> _fun_callbacks;
	void LoginHandler(shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	void SearchInfo(shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	void AddFriendApply(shared_ptr<CSession> session, const short& msg_id, const string& msg_data);


	bool isPureDigit(const std::string& str);// 判断是否是数字
	void GetUserByUid(std::string uid_str, Json::Value& rtvalue);// 通过uid获取用户信息
	void GetUserByName(std::string name, Json::Value& rtvalue);// 通过名字获取用户信息

	
	bool _b_stop;
	std::thread _worker_thread;// 工作线程
	std::queue<shared_ptr<LogicNode>> _msg_que;// 消息队列
	std::mutex _mutex;
	std::condition_variable _consume;
	
};

