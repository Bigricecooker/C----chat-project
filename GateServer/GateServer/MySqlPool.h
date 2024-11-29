#pragma once
#include "const.h"

// ���ݿ����ӳ�
class MySqlPool
{
public:
	MySqlPool(std::string _url, std::string _user, std::string _passwd, std::string _schema, std::size_t _poolSize);
	~MySqlPool();

	// ��ȡ����
	std::unique_ptr<sql::Connection> getConnection();

	// �����ӷŻ����ӳ�
	void returnConnection(std::unique_ptr<sql::Connection> connection);

	// �ر��̳߳�
	void Close();
private:
	std::atomic<bool> _b_stop;// ��ʶ�Ƿ���Ҫ����(ԭ������)
	std::size_t _poolSize;// ��С
	std::string _url;// host:port
	std::string _user;// �û�
	std::string _passwd;// ����
	std::string _schema;// ���ݿ����
	std::queue<std::unique_ptr<sql::Connection>> _connections;
	std::mutex _mutex;// ��
	std::condition_variable _cond;// ������
};

// ���ݿ������
class MysqlDao
{
public:
	MysqlDao();
	~MysqlDao();

	// ע���û�����
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
private:
	std::unique_ptr<MySqlPool> _pool;
};