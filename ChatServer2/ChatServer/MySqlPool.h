#pragma once
#include "const.h"

// 数据库连接池
class MySqlPool
{
public:
	MySqlPool(std::string _url, std::string _user, std::string _passwd, std::string _schema, std::size_t _poolSize);
	~MySqlPool();

	// 获取连接
	std::unique_ptr<sql::Connection> getConnection();

	// 将连接放回连接池
	void returnConnection(std::unique_ptr<sql::Connection> connection);

	// 关闭线程池
	void Close();
private:
	std::atomic<bool> _b_stop;// 标识是否需要回收(原子类型)
	std::size_t _poolSize;// 大小
	std::string _url;// host:port
	std::string _user;// 用户
	std::string _passwd;// 密码
	std::string _schema;// 数据库表名
	std::queue<std::unique_ptr<sql::Connection>> _connections;
	std::mutex _mutex;// 锁
	std::condition_variable _cond;// 互斥量
};

// 数据库操作类
class MysqlDao
{
public:
	MysqlDao();
	~MysqlDao();

	// 注册用户操作
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	// 判断用户与邮箱是否对应操作
	bool CheckEmail(const std::string& name, const std::string& email);
	// 重置密码
	bool UpdatePwd(const std::string& name, const std::string& pwd);
	// 判断用户与密码是否对应操作
	bool Checkpwd(const std::string& name, const std::string& pwd, UserInfo& userinfo);
	// uid查询用户信息
	std::shared_ptr<UserInfo> GetUser(const int& uid);
	// name查询用户信息
	std::shared_ptr<UserInfo> GetUser(const std::string& name);
	// 将请求好友数据写入数据库
	bool AddFriendApply(const int& from, const int& to);
	// 获取申请列表
	bool GetApplyList(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list, int begin, int limit);
	// 修改申请状态
	bool AuthFriendApply(int uid, int touid);
	// 添加好友
	bool AddFriend(int uid, int touid, std::string back_name);
	// 获取好友列表
	bool GetFriendList(int uid, std::vector<std::shared_ptr<UserInfo>>& friend_list);
private:
	std::unique_ptr<MySqlPool> _pool;
};