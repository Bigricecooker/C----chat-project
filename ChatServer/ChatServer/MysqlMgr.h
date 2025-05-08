#pragma once
#include "const.h"
#include "MySqlPool.h"
#include "singleton.h"

// ���ݿ����ӹ�����
class MysqlMgr :public Singleton<MysqlMgr>
{
    friend class Singleton<MysqlMgr>;
public:
    ~MysqlMgr();
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);// ע���û�
    bool CheckEmail(const std::string& name, const std::string& email);// �ж��û��������Ƿ��Ӧ
    bool UpdatePwd(const std::string& name, const std::string& pwd);// ��������
    bool Checkpwd(const std::string& name, const std::string& pwd, UserInfo& userinfo);// �ж��û��������Ƿ��Ӧ
    bool AddFriendApply(const int& from, const int& to);// �������������д�����ݿ�
    std::shared_ptr<UserInfo> GetUser(const int& uid);// uid��ѯ�û���Ϣ
    std::shared_ptr<UserInfo> GetUser(const string& name);// name��ѯ�û���Ϣ
private:
    MysqlMgr();
    MysqlDao  _dao;
};

