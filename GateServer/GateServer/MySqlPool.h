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
private:
	std::unique_ptr<MySqlPool> _pool;
};