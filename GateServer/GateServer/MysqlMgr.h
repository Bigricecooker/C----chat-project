#pragma once
#include "const.h"
#include "MySqlPool.h"
#include "singleton.h"

class MysqlMgr :public Singleton<MysqlMgr>
{
    friend class Singleton<MysqlMgr>;
public:
    ~MysqlMgr();
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);// ע���û�
    bool CheckEmail(const std::string& name, const std::string& email);// �ж��û��������Ƿ��Ӧ
    bool UpdatePwd(const std::string& name, const std::string& pwd);// ��������
    bool Checkpwd(const std::string& name, const std::string& pwd, UserInfo& userinfo);// �ж��û��������Ƿ��Ӧ
private:
    MysqlMgr();
    MysqlDao  _dao;
};

