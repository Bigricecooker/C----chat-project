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
	// �ж��û��������Ƿ��Ӧ����
	bool CheckEmail(const std::string& name, const std::string& email);
	// ��������
	bool UpdatePwd(const std::string& name, const std::string& pwd);
	// �ж��û��������Ƿ��Ӧ����
	bool Checkpwd(const std::string& name, const std::string& pwd, UserInfo& userinfo);
	// uid��ѯ�û���Ϣ
	std::shared_ptr<UserInfo> GetUser(const int& uid);
	// name��ѯ�û���Ϣ
	std::shared_ptr<UserInfo> GetUser(const std::string& name);
	// �������������д�����ݿ�
	bool AddFriendApply(const int& from, const int& to);
	// ��ȡ�����б�
	bool GetApplyList(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list, int begin, int limit);
	// �޸�����״̬
	bool AuthFriendApply(int uid, int touid);
	// ��Ӻ���
	bool AddFriend(int uid, int touid, std::string back_name);
	// ��ȡ�����б�
	bool GetFriendList(int uid, std::vector<std::shared_ptr<UserInfo>>& friend_list);
private:
	std::unique_ptr<MySqlPool> _pool;
};